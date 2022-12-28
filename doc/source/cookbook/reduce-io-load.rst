.. _reduce-io-load:

Reduce I/O load
===============

With large :command:`module` setup containing hundreds or even thousands of
modulefiles, the processing of commands :subcmd:`module avail<avail>` or
:subcmd:`module load<load>`. This is especially the case when all these
modulefiles are hosted in a shared filesystem mounted on all nodes of a
supercomputer concurrently used by many users.

Such slowness comes from the analysis of the file hierarchy under each enabled
modulepath that determines what files are modulefiles, what are the module
aliases or symbolic versions set on them, etc. This analysis generates a
significant number of I/O operations that linearly grow with the number of
modulefiles. Slowness can be more or less observed depending on the underlying
storage system and the number of available modulefiles.

This recipe provides examples of the features of Modules that could be
leveraged to reduce the I/O load implied by modulepath analysis. It starts
with the generation of an example modulefile setup that will be progressively
tweaked to observe I/O load reduction.

A general assumption is made here that the more I/O operations there are, the
slower the :command:`module` commands could be. So reducing the number of
these I/O operations leads to reducing the I/O load.

The `strace`_ utility, the Linux syscall tracer, will be used to watch the
I/O operations produced by each :command:`module` command run. Examples will
focus on the :subcmd:`module avail<avail>` command, which is one of the
most I/O intensive and widely utilized by users.

.. _strace: https://strace.io/

Implementation
--------------

Several features of Modules could be used to reduce the number of I/O
operations to analyze the content of modulepath:

* `Modules Tcl extension library`_
* `Modulepath rc file`_
* `mcookie_check configuration option`_
* `Virtual modules`_
* `Module cache`_

Each of the above features contributes to an I/O reduction as described in the
following section. Combined use of all these features will give the biggest
I/O operation save.

Example setup
-------------

For this recipe, a full modulefile setup has to be generated and it will be
progressively modified for the different Modules features that will be used.

2 modulepaths are created with 15 module names in each, each provided in 4
different versions. In one modulepath a :file:`.version` rc file is set for
each module name to determine a default version. In the other modulepath a
:file:`.modulerc` file is set for each module name to define a symbolic
version.

.. code-block:: sh

    mkdir -p example/reduce-io-load/applications
    mkdir -p example/reduce-io-load/libraries
    # create dummy application modulefiles
    cd example/reduce-io-load/applications
    for n in a b c d e f g h i j k l m n o; do
        mkdir app$n;
        for v in 1.0 2.0 3.0 4.0; do
          echo '#%Module' >app$n/$v;
        done;
        # define default version with .version file
        echo '#%Module' >app$n/.version
        echo 'set ModulesVersion 2.0' >>app$n/.version
      done
    cd -
    # create dummy library modulefiles
    cd example/reduce-io-load/libraries
    for n in a b c d e f g h i j k l m n o; do
        mkdir lib$n;
        for v in 1.0 2.0 3.0 4.0; do
          echo '#%Module' >lib$n/$v;
        done;
        # define symbolic version with .modulerc file
        echo -e '#%Module' >lib$n/.modulerc
        echo -e "module-version lib$n/3.0 sym" >>lib$n/.modulerc
      done
    cd -

Some non-modulefile files are added within these modulepaths to simulate
documentation files left by mistake in these directories:

.. code-block:: sh

    touch example/reduce-io-load/applications/appg/README
    touch example/reduce-io-load/applications/appl/README
    touch example/reduce-io-load/libraries/libg/README
    touch example/reduce-io-load/libraries/libl/README

The file permission mode of some modulefiles is set to protect them from being
read and simulate a hidden modulefile:

.. code-block:: sh

    chmod a-r example/reduce-io-load/applications/appg/1.0
    chmod a-r example/reduce-io-load/applications/appl/1.0
    chmod a-r example/reduce-io-load/libraries/libg/1.0
    chmod a-r example/reduce-io-load/libraries/libl/1.0

Additional modulefiles are created with a magic cookie header telling that
they are not compatible with the current version of Modules:

.. code-block:: sh

    echo '#%Module99' >example/reduce-io-load/applications/appg/5.0
    echo '#%Module99' >example/reduce-io-load/applications/appl/5.0
    echo '#%Module99' >example/reduce-io-load/libraries/libg/5.0
    echo '#%Module99' >example/reduce-io-load/libraries/libl/5.0

Once all of the above steps done, we end up with a small scale regular
modulefile setup containing 116 modulefiles available to us.

.. parsed-literal::

    :ps:`$` module purge
    :ps:`$` export MODULEPATH=
    :ps:`$` module use example/reduce-io-load/applications
    :ps:`$` module use example/reduce-io-load/libraries
    :ps:`$` module -o "" avail -t | wc -l
    116


Modules Tcl extension library
-----------------------------

Modules is shipped by default with a Tcl extension library that extends the
Tcl language in order to provide more optimized I/O commands to read a file or
a directory content than native Tcl commands do.

**Compatible with Modules v4.3+**

If we rebuild Modules without this library enabled, we will see the benefits
of having it enabled:

.. code-block:: sh

    make distclean
    ./configure --disable-libtclenvmodules
    make modulecmd-test.tcl
    chmod +x modulecmd-test.tcl
    eval $(tclsh ./modulecmd-test.tcl bash autoinit)

The :command:`strace` tool gives the number and the kind of I/O operations
performed during the ``module avail`` command:

.. parsed-literal::

    :ps:`$` strace -f -c -S name -e trace=%file,%desc -U calls,errors,name \\
        --silence=attach $MODULES_CMD bash avail 2>no_extlib.out

Once this first stat output is obtained, rebuild Modules with Tcl extension
library enabled and fetch :command:`icdiff` tool to compare results.

.. code-block:: sh

    make distclean
    ./configure --enable-libtclenvmodules
    make modulecmd-test.tcl lib/libtclenvmodules.so
    chmod +x modulecmd-test.tcl
    make icdiff

Then collect stats and compare results obtained:

.. parsed-literal::

    :ps:`$` strace -f -c -S name -e trace=%file,%desc -U calls,errors,name \\
        --silence=attach $MODULES_CMD bash avail 2>with_extlib.out
    :ps:`$` ./icdiff --cols=76 no_extlib.out with_extlib.out
    :sgrhi:`no_extlib.out`                         :sgrhi:`with_extlib.out`
        calls    errors syscall               calls    errors syscall
    --------- --------- ----------------  --------- --------- ----------------
           :sgrb:`36`         2 access                   :sgra:`37`         2 access
          :sgrb:`248`           close                   :sgra:`217`           close
            2           dup2                      2           dup2
            8         6 execve                    8         6 execve
          :sgrb:`166`           :sgrhi:`fcntl`                    :sgra:`12`           :sgrhi:`fcntl`
            :sgrb:`1`           getcwd                    :sgra:`2`           getcwd
          :sgrb:`128`           :sgrhi:`getdents64`               :sgra:`64`           :sgrhi:`getdents64`
          :sgrb:`166       161` :sgrhi:`ioctl`                    :sgra:`12         7` :sgrhi:`ioctl`
            9         4 lseek                     9         4 lseek
           :sgrb:`50`           mmap                     :sgra:`54`           mmap
           :sgrb:`86`           newfstatat               :sgra:`55`           newfstatat
          :sgrb:`250`        10 openat                  :sgra:`219`        10 openat
            2           pipe                      2           pipe
           10           pread64                  10           pread64
          :sgrb:`354`           read                    :sgra:`355`           read
         :sgrb:`1452      1452` :sgrhi:`readlink`                 :sgra:`25        25` :sgrhi:`readlink`
          :sgrb:`354`         2 :sgrhi:`stat`                    :sgra:`196`         2 :sgrhi:`stat`
            1           unlink                    1           unlink
           20           write                    20           write
    --------- --------- ----------------  --------- --------- ----------------
         :sgrb:`3335      1637` :sgrhi:`total`                  :sgra:`1292        56` :sgrhi:`total`

Modules Tcl extension library greatly reduces the number of filesystem I/O
operations by removing unneeded ``ioctl``, ``fcntl`` and ``readlink`` system
calls done (by Tcl ``open`` command) to read each file. Directory content read
is also improved by fetching hidden and regular files in one pass, which
divides by 2 the number of ``getdents`` call. ``stat`` calls are also reduced
as files found in directories are not checked prior attempting to opening
them.

Modulepath rc file
------------------

A ``.modulerc`` file found at the root of an enabled modulepath directory is
now evaluated when modulepath is walked through to locate modulefiles. This
file could hold the rc definition of the whole modules located in the
modulepath, instead of having specific ``.modulerc`` or ``.version`` file for
each module directory within the modulepath.

**Compatible with Modules v4.3+**

Let's migrate the ``.modulerc`` definition under each module directory in the
``.modulerc`` file at the root of the modulepath directory. And also translate
the content of ``.version`` files in :mfcmd:`module-version` commands that
could be stored in this top-level rc file. Then all the ``.modulerc`` and
``.version`` files under module directories are deleted to only keep one
``.modulerc`` per modulepath.

.. code-block:: sh

    cd example/reduce-io-load/applications
    echo '#%Module' >.modulerc
    for n in *; do
      v=$(grep set $n/.version | cut -d ' ' -f 3);
      echo "module-version $n/$v default" >>.modulerc;
      rm -f $n/.version
    done
    cd -
    cd example/reduce-io-load/libraries
    echo '#%Module' >.modulerc
    for n in *; do
      grep module-version $n/.modulerc >>.modulerc;
      rm -f $n/.modulerc
    done
    cd -

Once this change on the module trees has been done, collect new statistics and
compare them to those generated previously.

.. parsed-literal::

    :ps:`$` strace -f -c -S name -e trace=%file,%desc -U calls,errors,name \\
        --silence=attach $MODULES_CMD bash avail 2>with_modulepath_rc.out
    :ps:`$` ./icdiff --cols=76 with_extlib.out with_modulepath_rc.out
    :sgrhi:`with_extlib.out`                       :sgrhi:`with_modulepath_rc.out`
        calls    errors syscall               calls    errors syscall
    --------- --------- ----------------  --------- --------- ----------------
           :sgrb:`37`         2 :sgrhi:`access`                    :sgra:`9`         2 :sgrhi:`access`
          :sgrb:`217`           :sgrhi:`close`                   :sgra:`189`           :sgrhi:`close`
            2           dup2                      2           dup2
            8         6 execve                    8         6 execve
           12           fcntl                    12           fcntl
            2           getcwd                    2           getcwd
           64           getdents64               64           getdents64
           12         7 ioctl                    12         7 ioctl
            9         4 lseek                     9         4 lseek
           54           mmap                     54           mmap
           55           newfstatat               55           newfstatat
          :sgrb:`219`        10 :sgrhi:`openat`                  :sgra:`191`        10 :sgrhi:`openat`
            2           pipe                      2           pipe
           10           pread64                  10           pread64
          :sgrb:`355`           :sgrhi:`read`                    :sgra:`299`           :sgrhi:`read`
           25        25 readlink                 25        25 readlink
          :sgrb:`196`         2 :sgrhi:`stat`                    :sgra:`168`         2 :sgrhi:`stat`
            1           unlink                    1           unlink
           12           write                    12           write
    --------- --------- ----------------  --------- --------- ----------------
         :sgrb:`1292`        56 :sgrhi:`total`                  :sgra:`1124`        56 :sgrhi:`total`

With this change we have saved the ``access``, ``stat``, ``open``, ``read``
and ``close`` calls needed to analyze the 15 ``.modulerc`` and 15 ``.version``
files that have been removed and replaced by 2 top-level ``.modulerc`` files.

mcookie_check configuration option
----------------------------------

A new configuration option named :mconfig:`mcookie_check` has been introduced
to control the verification made to files to determine if they are
modulefiles. By default this configuration option is set to ``always`` and
when searching for modulefiles within enabled modulepaths each file below
these directories is opened to check if it starts with the Modules magic
cookie (i.e., ``#%Module`` file signature).

These historical checks lead to a large number of I/O operations but if the
option is set to the ``eval`` value, files are not checked anymore when
searching for modulefiles, only when evaluating them.

**Compatible with Modules v5.1+**

Here we are setting the :mconfig:`mcookie_check` configuration to the ``eval``
value to skip all test on files hosted in modulepath directories and see what
happens when listing available modulefiles:

.. parsed-literal::

    :ps:`$` module config mcookie_check eval
    :ps:`$` module -o "" avail -t | wc -l
    128
    :ps:`$` module avail -o "" appg appl libg libl
    appg/1.0  appg/2.0  appg/3.0  appg/4.0  appg/5.0  appg/README
    
    appl/1.0  appl/2.0  appl/3.0  appl/4.0  appl/5.0  appl/README
    
    libg/1.0  libg/2.0  libg/3.0(sym)  libg/4.0  libg/5.0  libg/README
    
    libl/1.0  libl/2.0  libl/3.0(sym)  libl/4.0  libl/5.0  libl/README

12 more modulefiles are returned. Indeed the file checks done by default are
useful but we could disable them if a special care is given to the content of
modulepath directories. First, we have to eliminate the non-modulefiles stored
in these directories:

.. code-block:: sh

    rm -f example/reduce-io-load/applications/appg/README
    rm -f example/reduce-io-load/applications/appl/README
    rm -f example/reduce-io-load/libraries/libg/README
    rm -f example/reduce-io-load/libraries/libl/README

Then a :mfcmd:`module-hide` command in modulepath's ``.modulerc`` file should
be added for each the modulefile that is read-protected through the use of
file permission modes. This way :command:`module` will know that modulefile is
hidden.

.. code-block:: sh

    cd example/reduce-io-load/applications
    echo 'module-hide --hard appg/1.0' >>.modulerc
    echo 'module-hide --hard appl/1.0' >>.modulerc
    cd -
    cd example/reduce-io-load/libraries
    echo 'module-hide --hard libg/1.0' >>.modulerc
    echo 'module-hide --hard libl/1.0' >>.modulerc
    cd -

A :mfcmd:`module-hide` command in modulepath's ``.modulerc`` file should also
be defined for each modulefile requiring a specific Modules version in its
magic cookie. :command:`module` will skip such modulefiles in case its version
is not compatible with them.

.. code-block:: sh

    cd example/reduce-io-load/applications
    echo 'if {$ModuleToolVersion < 99} {' >>.modulerc
    echo '  module-hide --hard appg/5.0' >>.modulerc
    echo '  module-hide --hard appl/5.0' >>.modulerc
    echo '}' >>.modulerc
    cd -
    cd example/reduce-io-load/libraries
    echo 'if {$ModuleToolVersion < 99} {' >>.modulerc
    echo '  module-hide --hard libg/5.0' >>.modulerc
    echo '  module-hide --hard libl/5.0' >>.modulerc
    echo '}' >>.modulerc
    cd -

Once this specific setup has been achieved to get in full control of the
content of each modulepaths, we get the accurate listing result with the
*eval* mode of :mconfig:`mcookie_check`.

.. parsed-literal::

    :ps:`$` module -o "" avail -t | wc -l
    116
    :ps:`$` strace -f -c -S name -e trace=%file,%desc -U calls,errors,name \\
        --silence=attach $MODULES_CMD bash avail 2>mcookie_check_eval.out
    :ps:`$` ./icdiff --cols=76 with_modulepath_rc.out mcookie_check_eval.out
    :sgrhi:`with_modulepath_rc.out`                :sgrhi:`mcookie_check_eval.out`
        calls    errors syscall               calls    errors syscall
    --------- --------- ----------------  --------- --------- ----------------
            9         2 access                    9         2 access
          :sgrb:`189`           :sgrhi:`close`                    :sgra:`65`           :sgrhi:`close`
            2           dup2                      2           dup2
            8         6 execve                    8         6 execve
           12           fcntl                    12           fcntl
            2           getcwd                    2           getcwd
           64           getdents64               64           getdents64
           12         7 ioctl                    12         7 ioctl
            9         4 lseek                     9         4 lseek
           54           mmap                     54           mmap
           55           newfstatat               55           newfstatat
          :sgrb:`191        10` :sgrhi:`openat`                   :sgra:`63`         6 :sgrhi:`openat`
            2           pipe                      2           pipe
           10           pread64                  10           pread64
          :sgrb:`299`           :sgrhi:`read`                    :sgra:`175`           :sgrhi:`read`
           25        25 readlink                 25        25 readlink
          :sgrb:`168`         2 stat                    :sgra:`164`         2 stat
            1           unlink                    1           unlink
           12           write                    12           write
    --------- --------- ----------------  --------- --------- ----------------
         :sgrb:`1124        56` :sgrhi:`total`                   :sgra:`744        52` :sgrhi:`total`

A significant drop in I/O calls can be observed from this finely tuned setup.
``open``, ``read`` and ``close`` calls are saved for the 4 non-modulefiles
deleted, the 8 modulefiles made hidden and of course for the 116 regular
modulefiles available. As demonstrated, such gain requires a complete control
over the modulepath content and specific care for unsupported or
read-protected files.

Virtual modules
---------------

A virtual module stands for a module name associated to a modulefile. Instead
of looking for files under modulepaths to get modulefiles, a virtual module
is defined in ``.modulerc`` file with the :mfcmd:`module-virtual` modulefile
command which saves walk down I/O operations to analyze modulepath directory
content.

**Compatible with Modules v4.1+**

Let's create 2 new modulepaths that will only contain a ``.modulerc`` file in
which a virtual module is defined for each existing modulefile in initial
modulepath. Content of the ``.modulerc`` in the initial modulepaths is also
copied in the ``.modulerc`` of the *virtual* modulepaths.

.. code-block:: sh

    mkdir example/reduce-io-load/applications-virt
    mkdir example/reduce-io-load/libraries-virt
    cd example/reduce-io-load/applications
    echo '#%Module' >../applications-virt/.modulerc
    for mod in */*; do
      echo "module-virtual $mod ../applications/$mod" \
        >>../applications-virt/.modulerc;
    done
    grep -v '#%Module' .modulerc >>../applications-virt/.modulerc
    cd -
    cd example/reduce-io-load/libraries
    echo '#%Module' >../libraries-virt/.modulerc
    for mod in */*; do
      echo "module-virtual $mod ../libraries/$mod" \
        >>../libraries-virt/.modulerc;
    done
    grep -v '#%Module' .modulerc >>../libraries-virt/.modulerc
    cd -

Once the setup of the virtual modulepaths is finished, the environment of the
:command:`module` command has to be changed to use these new modulepaths
instead of the original ones.

.. parsed-literal::

    :ps:`$` module unuse example/reduce-io-load/applications
    :ps:`$` module unuse example/reduce-io-load/libraries
    :ps:`$` module use example/reduce-io-load/applications-virt
    :ps:`$` module use example/reduce-io-load/libraries-virt

Then we can check we obtain the same output as with the original setup, 116
modulefiles available. After that collect I/O operation statistics and compare
them to those previously fetched.

.. parsed-literal::

    :ps:`$` module -o "" avail -t | wc -l
    116
    :ps:`$` strace -f -c -S name -e trace=%file,%desc -U calls,errors,name \\
        --silence=attach $MODULES_CMD bash avail 2>with_virtual_modules.out
    :ps:`$` ./icdiff --cols=76 mcookie_check_eval.out with_virtual_modules.out
    :sgrhi:`mcookie_check_eval.out`                :sgrhi:`with_virtual_modules.out`
    ...
        calls    errors syscall               calls    errors syscall
    --------- --------- ----------------  --------- --------- ----------------
            9         2 access                    9         2 access
           :sgrb:`65`           :sgrhi:`close`                    :sgra:`35`           :sgrhi:`close`
            2           dup2                      2           dup2
            8         6 execve                    8         6 execve
           12           fcntl                    12           fcntl
            2           getcwd                    2           getcwd
           :sgrb:`64`           :sgrhi:`getdents64`                :sgra:`4`           :sgrhi:`getdents64`
           12         7 ioctl                    12         7 ioctl
            9         4 lseek                     9         4 lseek
           54           mmap                     54           mmap
           :sgrb:`55`           :sgrhi:`newfstatat`               :sgra:`25`           :sgrhi:`newfstatat`
           :sgrb:`63`         6 :sgrhi:`openat`                   :sgra:`33`         6 :sgrhi:`openat`
            2           pipe                      2           pipe
           10           pread64                  10           pread64
          175           read                    175           read
           25        25 readlink                 25        25 readlink
          :sgrb:`164`         2 :sgrhi:`stat`                     :sgra:`10`         2 :sgrhi:`stat`
            1           unlink                    1           unlink
           12           write                    12           write
    --------- --------- ----------------  --------- --------- ----------------
          :sgrb:`744`        52 :sgrhi:`total`                   :sgra:`440`        52 :sgrhi:`total`

A large I/O operation drop is observed with the *virtual* modulepath setup.
The analysis of the 15 module directories under each of the 2 original
modulepaths is not anymore needed as the ``.modulerc`` in the 2 *virtual*
modulepaths already point to the modulefile location. ``stat``, ``open``,
``getdents`` and ``close`` I/O calls are saved due to that.

Module cache
------------

Module cache can be built for every modulepaths with :subcmd:`cachebuild`
sub-command. When a cache file is found for an enabled modulepath, this file
is evaluated instead of walking down the content of the modulepath directory.

**Compatible with Modules v5.3+**

Here we start over the module setup at the end of the `Modulepath rc file`_
section, restoring :mconfig:`mcookie_check` configuration to default, building
cache and setting cache read buffer to maximum value.

.. parsed-literal::

    :ps:`$` module unuse example/reduce-io-load/applications-virt
    :ps:`$` module unuse example/reduce-io-load/libraries-virt
    :ps:`$` module use example/reduce-io-load/applications
    :ps:`$` module use example/reduce-io-load/libraries
    :ps:`$` module config mcookie_check always
    :ps:`$` module cachebuild
    Creating :sgrhi:`example/reduce-io-load/libraries/.modulecache`
    Creating :sgrhi:`example/reduce-io-load/applications/.modulecache`
    :ps:`$` module config cache_buffer_bytes 1000000

Once things are setup, new statistics are collected and compared between when
cache is used or when it is ignored.

.. parsed-literal::

    :ps:`$` module -o "" avail -t | wc -l
    116
    :ps:`$` strace -f -c -S name -e trace=%file,%desc -U calls,errors,name \\
        --silence=attach $MODULES_CMD bash avail 2>with_cache.out
    :ps:`$` strace -f -c -S name -e trace=%file,%desc -U calls,errors,name \\
        --silence=attach $MODULES_CMD bash avail --ignore-cache 2>no_cache.out
    :ps:`$` ./icdiff --cols=76 no_cache.out with_cache.out
    :sgrhi:`no_cache.out`                          :sgrhi:`with_cache.out`
        calls    errors syscall               calls    errors syscall
    --------- --------- ----------------  --------- --------- ----------------
            :sgrb:`9`         2 access                   :sgra:`10`         2 access
          :sgrb:`183`           :sgrhi:`close`                    :sgra:`29`           :sgrhi:`close`
            2           dup2                      2           dup2
            9         7 execve                    9         7 execve
           :sgrb:`12`           fcntl                    :sgra:`15`           fcntl
            1           getcwd                    1           getcwd
           :sgrb:`64`           :sgrhi:`getdents64`
           :sgrb:`12         7` ioctl                    :sgra:`15        10` ioctl
            :sgrb:`9`         4 lseek                    :sgra:`10`         4 lseek
           :sgrb:`37`           mmap                     :sgra:`38`           mmap
          :sgrb:`221`         2 :sgrhi:`newfstatat`               :sgra:`30`         2 :sgrhi:`newfstatat`
          :sgrb:`187`        10 :sgrhi:`openat`                   :sgra:`33`        10 :sgrhi:`openat`
            2           pipe2                     2           pipe2
           10           pread64                  10           pread64
          :sgrb:`324`           :sgrhi:`read`                    :sgra:`209`           :sgrhi:`read`
           :sgrb:`35        35` readlink                 :sgra:`54        54` readlink
            1           unlink                    1           unlink
           12           write                    12           write
    --------- --------- ----------------  --------- --------- ----------------
         :sgrb:`1130        67` :sgrhi:`total`                   :sgra:`480        89` :sgrhi:`total`

When a cache is found, one file is read instead of checking all directories
and files in modulepath directory. Only the modulefiles and directories that
are not accessible for everyone are live checked after reading cache file to
see if these elements are available to current user.

It explains the significant I/O call drop that can be observed here. Some I/O
calls are slightly higher due to the evaluation of module cache files.


Wrap-up
-------

Combining all the 4 first features or last one detailed above leads to a
significant drop in I/O operations. Almost all remaining I/O calls are made
for the initialization of the :command:`module` command run.

It is advised to run this recipe code on your setup to observe the I/O load
gain you could obtain. As said earlier the less I/O operations there are, the
faster the :command:`module` command could be. But this highly depends on your
storage system, on the number of modulefiles and on the number of active
users. You may not notice a big difference if your modulefiles are installed
on a local SSD storage whereas it can be a game changer if instead the
modulefiles are hosted on a shared HDD filesystem that is accessed by
hundreds of users.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

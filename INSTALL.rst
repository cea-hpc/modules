.. _INSTALL:

Installing Modules on Unix
==========================

This document is an overview of building and installing Modules on a Unix
system.


Requirements
------------

Modules consists of one Tcl script so to run it from a user shell the only
requirement is to have a working version of ``tclsh`` (version 8.5 or later)
available on your system. ``tclsh`` is a part of Tcl
(http://www.tcl.tk/software/tcltk/).

To install Modules from a distribution tarball or a clone of the git
repository, a build step is there to tailor the :file:`modulecmd.tcl` and the
initialization scripts to the chosen installation configuration and create the
documentation files. This build step requires the tools to be found on your
system:

* bash
* make
* sed
* runtest

When also installing Modules Tcl extension library (enabled by default), these
additional tools are needed:

* grep
* gcc
* tcl-devel >= 8.5

When installing from a distribution tarball, documentation is pre-built and
scripts to configure Modules Tcl extension library build are already
generated. Thus no additional software is required. When installing from a
clone of the git repository or from a git archive export, documentation and
scripts to prepare for compilation have to be built and the following tools
are required:

* autoconf
* automake
* autopoint
* python
* sphinx >= 1.0

Get Modules
-----------

Modules can usually be installed with the package manager of your Unix system.
It it is available by default `on most Linux distributions, on OS X and
FreeBSD <https://repology.org/project/environment-modules/versions>`_ either
under the name of ``modules`` or ``environment-modules``.

If you want to install Modules from sources, tarballs from all Modules'
releases can be retrieved from one of the following link:

* https://github.com/cea-hpc/modules/releases/
* https://sourceforge.net/projects/modules/files/Modules/

For instance to download then unpack the last release of Modules:

.. parsed-literal::

    :ps:`$` curl -LJO |gh_tgz_dl_url|
    :ps:`$` tar xfz modules-\ |version|\ .tar.gz

Installation instructions
-------------------------

The simplest way to build and install Modules is:

.. parsed-literal::

    :ps:`$` ./configure
    :ps:`$` make
    :ps:`$` make install

Some explanation, step by step:

1. ``cd`` to the directory containing the package's source code. Your system
   must have the above requirements installed to properly build scripts, and
   documentation if build occurs from a clone of the git repository.

2. Type ``./configure`` to adapt the installation for your system. At this
   step you can choose the installation paths and the features you want to
   enable in the initialization scripts (see `Build and installation options`_
   section below for a complete overview of the available options)

3. Type ``make`` to adapt scripts to the configuration, build Tcl extension
   library if enabled and build documentation if working from git repository.

4. Optionally, type ``make test`` to run the test suite.

5. Type ``make install`` to install modulecmd.tcl, initialization scripts and
   documentation.

6. Optionally, type ``make testinstall`` to run the installation test suite.

7. You can remove the built files from the source code directory by typing
   ``make clean``. To also remove the files that ``configure`` created, type
   ``make distclean``.

A default installation process like described above will install Modules
under ``/usr/local/Modules``. You can change this with the :instopt:`--prefix`
option. By default, ``/usr/local/Modules/modulefiles`` will be setup as
the default directory containing modulefiles. :instopt:`--modulefilesdir`
option enables to change this directory location. For example:

.. parsed-literal::

    :ps:`$` ./configure --prefix=/usr/share/Modules \\
                  --modulefilesdir=/etc/modulefiles

See `Build and installation options`_ section to discover all ``./configure``
option available.

.. note:: GNU Make is excepted to be used for this build and installation
   process. On non-Linux systems, the ``gmake`` should be called instead of
   ``make``.


Configuration
-------------

Once installed you should review and adapt the configuration to make it fit
your needs. The following steps are provided for example. They are not
necessarily mandatory as it depends of the kind of setup you want to achieve.

1. Enable Modules initialization at shell startup. An easy way to get module
   function defined and its associated configuration setup at shell startup
   is to make the initialization scripts part of the system-wide environment
   setup in ``/etc/profile.d``. To do so, make a link in this directory to the
   profile scripts that can be found in your Modules installation init
   directory:

   .. parsed-literal::

       :ps:`$` ln -s PREFIX/init/profile.sh /etc/profile.d/modules.sh
       :ps:`$` ln -s PREFIX/init/profile.csh /etc/profile.d/modules.csh

   These profile scripts will automatically adapt to the kind of ``sh`` or
   ``csh`` shell you are running.

   Another approach may be to get the Modules initialization script sourced
   from the shell configuration startup file. For instance following line
   could be added to the end of the ``~/.bashrc`` file if running Bash shell::

       source PREFIX/init/bash

   Beware that shells have multiple ways to initialize depending if they are
   a login shell or not and if they are launched in interactive mode or not.

2. Define module paths to enable by default. Edit :file:`initrc` configuration
   file in the directory designated by the :instopt:`--etcdir` option or edit
   :file:`modulespath` in the same directory.

   If you use :file:`modulespath` configuration file, add one line mentioning
   each modulefile directory::

       /path/to/regular/modulefiles
       /path/to/other/modulefiles

   If you use :file:`initrc` configuration file, add one line mentioning each
   modulefile directory prefixed by the :subcmd:`module use<use>` command:

   .. code-block:: tcl

       module use /path/to/regular/modulefiles
       module use /path/to/other/modulefiles

   In case both configuration files :file:`initrc` and :file:`modulespath` are
   present, Modules initialization process will first evaluate
   :file:`modulespath` then :file:`initrc`.

   By default, the modulepaths specified on the :instopt:`--with-modulepath`
   installation option are automatically defined for use in :file:`initrc` (or
   in :file:`modulespath` if the :instopt:`--enable-modulespath` installation
   option has been set).

   .. note:: If you have set :instopt:`--with-initconf-in` to ``initdir`` to
      install the Modules initialization configurations in the configuration
      directory designated by the :instopt:`--initdir` option, the configuration
      files :file:`initrc` and :file:`modulespath` are respectively named
      ``modulerc`` and ``.modulespath``.

3. Define modulefiles to load by default. Edit :file:`initrc` configuration
   file. Modulefiles to load cannot be specified in :file:`modulespath` file.
   Add there all the modulefiles you want to load by default at Modules
   initialization time.

   Add one line mentioning each modulefile to load prefixed by the
   :subcmd:`module load<load>` command:

   .. code-block:: tcl

       module load foo
       module load bar

   By default, the modules specified on the :instopt:`--with-loadedmodules`
   installation option are automatically defined for load in :file:`initrc`.

   Alternatively, if users have :ref:`module collections<collections>` saved
   in their :envvar:`HOME` directory, you may prefer restoring their default
   collection when Modules initializes rather loading the default module list:

   .. code-block:: tcl

       if {[is-saved default]} {
           module restore
       } else {
           module load foo
           module load bar
       }

   In fact you can add to the :file:`initrc` configuration file any kind of
   supported module command, like :subcmd:`module config<config>` commands to
   tune :command:`module`'s default behaviors. This configuration way is
   recommended over directly modifying the shell initialization scripts.

If you go through the above steps you should have a valid setup tuned to your
needs. After that you still have to write modulefiles to get something to
load and unload in your newly configured Modules setup. In case you want to
achieve a specific setup, some additional steps may be required:

4. In case the configuration you expect cannot be achieved through the
   :file:`initrc` configuration file, you may review and tune the
   initialization scripts. These files are located in the directory designated
   by the :instopt:`--initdir` option. Beware that these scripts could be
   overwritten when upgrading to a newer version of Modules, so configuration
   should be done through the :file:`initrc` file as far as possible.

5. If you want to alter the way the :file:`modulecmd.tcl` script operates, the
   :file:`siteconfig.tcl` script may be used. This Tcl file is located in the
   directory designated by the :instopt:`--etcdir` option. Every time the
   :command:`module` command is called, it executes the :file:`modulecmd.tcl`
   script which in turns sources the :file:`siteconfig.tcl` script during its
   startup phase. The site-specific configuration script could override
   default configuration values and more largely could supersede all
   procedures defined in :file:`modulecmd.tcl` to obtain specific behaviors.

6. Prior running the module sub-command specified as argument, the
   :file:`modulecmd.tcl` script evaluates the global run-command files. These
   files are either the :file:`rc` file in the directory designated by the
   :instopt:`--etcdir` option, the file designated in the
   :envvar:`MODULERCFILE` environment variable or the user-specific RC file
   located in :file:`$HOME/.modulerc`. The RC files are modulefiles (limited
   to a subset of the :ref:`modulefile Tcl
   commands<Modules specific Tcl commands>`) that could define global module
   aliases, virtual modules or module properties such as tags, forbidding
   rules, etc.

To learn more about siteconfig and run-command files, see the
:ref:`Modulecmd startup` section in :ref:`module(1)` reference manual. You may
also look at the available :ref:`setup recipes<cookbook>` to get concrete
deployment examples of these files.


Build and installation options
------------------------------

Options available at the ``./configure`` installation step are described
below.  These options enable to choose the installation paths and the
features to enable or disable. You can also get a description of these
options by typing ``./configure --help``.

Fine tuning of the installation directories (the default value for each option
is displayed within brakets):

.. instopt:: --bindir=DIR

 Directory for executables reachable by users [``PREFIX/bin``]

.. instopt:: --datarootdir=DIR

 Base directory to set the man and doc directories [``PREFIX/share``]

.. instopt:: --docdir=DIR

 Directory to host documentation other than man pages like README, license
 file, etc [``DATAROOTDIR/doc``]

.. instopt:: --etcdir=DIR

 Directory for the executable configuration scripts
 [``PREFIX/etc``]

 .. only:: html

    .. versionadded:: 4.1

.. instopt:: --initdir=DIR

 Directory for the per-shell environment initialization scripts
 [``PREFIX/init``]

.. instopt:: --libdir=DIR

 Directory for object code libraries like libtclenvmodules.so [``PREFIX/lib``]

.. instopt:: --libexecdir=DIR

 Directory for executables called by other executables like modulecmd.tcl
 [``PREFIX/libexec``]

.. instopt:: --mandir=DIR

 Directory to host man pages [``DATAROOTDIR/man``]

.. instopt:: --modulefilesdir=DIR

 Directory of main modulefiles also called system modulefiles
 [``PREFIX/modulefiles``]

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --prefix=PREFIX

 Installation root directory [``/usr/local/Modules``]

.. instopt:: --vimdatadir=DIR

 Directory to host Vim addon files [``DATAROOTDIR/vim/vimfiles``]

 .. only:: html

    .. versionadded:: 4.3

Optional Features (the default for each option is displayed within
parenthesis, to disable an option replace ``enable`` by ``disable`` for
instance :instopt:`--disable-set-manpath<--enable-set-manpath>`):

.. instopt:: --enable-advanced-version-spec

 Activate the advanced module version specifiers which enables to finely
 select module versions by specifying after the module name a version
 constraint prefixed by the ``@`` character. This option also allows to
 specify module variants. (default=yes)

 .. only:: html

    .. versionadded:: 4.4

    .. versionchanged:: 5.0
       Configuration option default set to ``yes``

.. instopt:: --enable-append-binpath

 Append rather prepend binary directory to the PATH environment variable when
 the :instopt:`--enable-set-binpath` option is enabled. (default=no)

 .. only:: html

    .. versionadded:: 4.2

.. instopt:: --enable-append-manpath

 Append rather prepend man page directory to the MANPATH environment variable
 when the :instopt:`--enable-set-manpath` option is enabled. (default=no)

 .. only:: html

    .. versionadded:: 4.2

.. instopt:: --enable-auto-handling

 Set modulecmd.tcl to automatically apply automated modulefiles handling
 actions, like loading the pre-requisites of a modulefile when loading this
 modulefile. (default=yes)

 .. only:: html

    .. versionadded:: 4.2

    .. versionchanged:: 5.0
       Configuration option default set to ``yes``

.. instopt:: --enable-avail-indepth

 When performing an :subcmd:`avail` sub-command, include in search results the
 matching modulefiles and directories and recursively the modulefiles and
 directories contained in these matching directories when enabled or limit
 search results to the matching modulefiles and directories found at the depth
 level expressed by the search query if disabled. (default=yes)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --enable-color

 Control if output should be colored by default or not.  A value of ``yes``
 equals to the ``auto`` color mode.  ``no`` equals to the ``never`` color
 mode. (default=yes)

 .. only:: html

    .. versionadded:: 4.3

    .. versionchanged:: 5.0
       Configuration option default set to ``yes``

.. instopt:: --enable-doc-install

 Install the documentation files in the documentation directory defined with
 the :instopt:`--docdir` option. This feature has no impact on manual pages
 installation. Disabling documentation file installation is useful in case of
 installation process handled via a package manager which handles by itself
 the installation of this kind of documents. (default=yes)

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --enable-example-modulefiles

 Install some modulefiles provided as example in the system modulefiles
 directory defined with the :instopt:`--modulefilesdir` option. (default=yes)

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --enable-extended-default

 Allow to specify module versions by their starting part, i.e. substring
 separated from the rest of the version string by a ``.`` character.
 (default=yes)

 .. only:: html

    .. versionadded:: 4.4

    .. versionchanged:: 5.0
       Configuration option default set to ``yes``

.. instopt:: --enable-implicit-default

 Define an implicit default version, for modules with none explicitly defined,
 to select when the name of the module to evaluate is passed without the
 mention of a specific version. When this option is disabled the name of the
 module passed for evaluation should be fully qualified elsewhere an error is
 returned. (default=yes)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --enable-implicit-requirement

 Implicitly define a prereq or a conflict requirement toward modules specified
 respectively on :mfcmd:`module load<module>` or :mfcmd:`module
 unload<module>` commands in modulefile. (default=yes)

 .. only:: html

    .. versionadded:: 4.7

.. instopt:: --enable-libtclenvmodules

 Build and install the Modules Tcl extension library which provides optimized
 Tcl commands for the modulecmd.tcl script. (default=yes)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --enable-mcookie-version-check

 Enable check of the version specified right after Modules magic cookie
 (``#%Module``) in modulefiles, which defines the minimal version of the
 Modules tool to use in order to evaluate the modulefile. (default=yes)

 .. only:: html

    .. versionadded:: 4.7

.. instopt:: --enable-ml

 Define the :command:`ml` command, a handy frontend to the module command,
 when Modules initializes. (default=yes)

 .. only:: html

    .. versionadded:: 4.5

.. instopt:: --enable-modulespath, --enable-dotmodulespath

 Set the module paths defined by :instopt:`--with-modulepath` option in a
 :file:`modulespath` file (following C version fashion) within the
 initialization directory defined by the :instopt:`--etcdir` option rather
 than within the :file:`initrc` file. Or respectively, if option
 :instopt:`--with-initconf-in` has been set to ``initdir``, in a
 ``.modulespath`` file within the configuration directory defined by the
 :instopt:`--initdir` option rather than within the ``modulerc`` file.
 (default=no)

 .. only:: html

    .. versionadded:: 4.0

    .. versionchanged:: 4.3
       Option ``--enable-modulespath`` added

.. instopt:: --enable-multilib-support

 Support multilib systems by looking in modulecmd.tcl at an alternative
 location where to find the Modules Tcl extension library depending on current
 machine architecture. (default=no)

 .. only:: html

    .. versionadded:: 4.6

.. instopt:: --enable-new-features

 Enable all new features that are disabled by default due to the substantial
 behavior changes they imply on Modules 5. This option does not enable other
 configuration option currently. (default=no)

 .. only:: html

    .. versionadded:: 4.7

    .. versionchanged:: 5.0
       Configuration option has been reset following major version change as
       :instopt:`--enable-auto-handling`, :instopt:`--enable-color`,
       :instopt:`--with-icase=search<--with-icase>`,
       :instopt:`--enable-extended-default` and
       :instopt:`--enable-advanced-version-spec` are set by default on Modules
       5.

.. instopt:: --enable-quarantine-support

 Generate code in module function definition to add support for the
 environment variable quarantine mechanism (default=no)

 .. only:: html

    .. versionadded:: 4.1

    .. versionchanged:: 5.0
       Configuration option default set to ``no``

.. instopt:: --enable-set-binpath

 Prepend binary directory defined by the :instopt:`--bindir` option to the
 PATH environment variable in the shell initialization scripts. (default=yes)

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --enable-set-manpath

 Prepend man page directory defined by the :instopt:`--mandir` option to the
 MANPATH environment variable in the shell initialization scripts.
 (default=yes)

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --enable-set-shell-startup

 Set when module function is defined the shell startup file to ensure that the
 module function is still defined in sub-shells. (default=no)

 .. only:: html

    .. versionadded:: 4.3

    .. versionchanged:: 5.0
       Configuration option default set to ``no``

.. instopt:: --enable-silent-shell-debug-support

 Generate code in module function definition to add support for silencing
 shell debugging properties (default=no)

 .. only:: html

    .. versionadded:: 4.2

    .. versionchanged:: 5.0
       Configuration option default set to ``no``

.. instopt:: --enable-versioning

 Append Modules version to installation prefix and deploy a ``versions``
 modulepath shared between all versioning enabled Modules installation. A
 modulefile corresponding to Modules version is added to the shared modulepath
 and enables to switch from one Modules version to another.  (default=no)

.. instopt:: --enable-vim-addons

 Install the Vim addon files in the Vim addons directory defined with the
 :instopt:`--vimdatadir` option. (default=yes)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --enable-wa-277

 Activate workaround for issue #277 related to Tcsh history mechanism which
 does not cope well with default module alias definition. Note that enabling
 this workaround solves Tcsh history issue but weakens shell evaluation of the
 code produced by modulefiles. (default=no)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --enable-windows-support

 Install all required files for Windows platform (:command:`module`,
 :command:`ml` and :command:`envml` command batch file and ``cmd.cmd``
 initialization script). (default=no)

 .. only:: html

    .. versionadded:: 4.5


Optional Packages (the default for each option is displayed within
parenthesis, to disable an option replace ``with`` by ``without`` for
instance :instopt:`--without-modulepath<--with-modulepath>`):

.. instopt:: --with-avail-output=LIST

 Specify the content to report on avail sub-command regular output in addition
 to the available module names. Elements accepted in LIST are: ``modulepath``,
 ``alias``, ``dirwsym``, ``sym``, ``tag`` and ``key`` (elements in LIST are
 separated by ``:``). The order of the elements in LIST does not matter.
 (default=\ ``modulepath:alias:dirwsym:sym:tag:key``)

 .. only:: html

    .. versionadded:: 4.7

.. instopt:: --with-avail-terse-output=LIST

 Specify the content to report on avail sub-command terse output in addition
 addition to the available module names. Elements accepted in LIST are:
 ``modulepath``, ``alias``, ``dirwsym``, ``sym``, ``tag`` and ``key`` (elements
 in LIST are separated by ``:``). The order of the elements in LIST does not
 matter. (default=\ ``modulepath:alias:dirwsym:sym:tag``)

 .. only:: html

    .. versionadded:: 4.7

.. instopt:: --with-bin-search-path=PATHLIST

 List of paths to look at when searching the location of tools required to
 build and configure Modules (default=\ ``/usr/bin:/bin:/usr/local/bin``)

 .. only:: html

    .. versionadded:: 4.2

.. instopt:: --with-dark-background-colors=SGRLIST

 Default color set to apply if terminal background color is defined to
 ``dark``. SGRLIST follows the same syntax than used in ``LS_COLORS``. Each
 element in SGRLIST is an output item associated to a Select Graphic Rendition
 (SGR) code. Elements in SGRLIST are separated by ``:``.  Output items are
 designated by keys.

 Items able to be colorized are: highlighted element (``hi``), debug
 information (``db``), trace information (``tr``) tag separator (``se``);
 Error (``er``), warning (``wa``), module error (``me``) and info (``in``)
 message prefixes; Modulepath (``mp``), directory (``di``), module alias
 (``al``), module variant (``va``), module symbolic version (``sy``) and
 module ``default`` version (``de``).

 :ref:`Module tags` can also be colorized. The key to set in the color palette
 to get a graphical rendering of a tag is the tag name or the tag abbreviation
 if one is defined for tag. The SGR code applied to a tag name is ignored if
 an abbreviation is set for this tag thus the SGR code should be defined for
 this abbreviation to get a graphical rendering. Each basic tag has by default
 a key set in the color palette, based on its abbreviated string: auto-loaded
 (``aL``), forbidden (``F``), hidden and hidden-loaded (``H``), loaded
 (``L``), nearly-forbidden (``nF``), sticky (``S``) and super-sticky (``sS``).

 For a complete SGR code reference, see
 https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters.
 (default=\ ``hi=1:db=2:tr=2:se=2:er=91:wa=93:me=95:in=94:mp=1;94:di=94:al=96:va=93:sy=95:de=4:cm=92:aL=100:L=90;47:H=2:F=41:nF=43:S=46:sS=44``)

 .. only:: html

    .. versionadded:: 4.3

    .. versionchanged:: 4.6
       Output item for trace information (``tr``) added

    .. versionchanged:: 4.7
       Output items for module tags auto-loaded (``aL``), forbidden (``F``),
       hidden and hidden-loaded (``H``), loaded (``L``), nearly-forbidden
       (``nF``), sticky (``S``) and super-sticky (``sS``) added

    .. versionchanged:: 4.8
       Output item for module variant (``va``) added

.. instopt:: --with-editor=BIN

 Name or full path of default editor program to use to open modulefile through
 the :subcmd:`edit` sub-command. (default=\ ``vi``)

 .. only:: html

    .. versionadded:: 4.8

.. instopt:: --with-icase=VALUE

 Apply a case insensitive match to module specification on :subcmd:`avail`,
 :subcmd:`whatis` and :subcmd:`paths` sub-commands (when set to ``search``) or
 on all module sub-commands and modulefile Tcl commands for the module
 specification they receive as argument (when set to ``always``). Case
 insensitive match is disabled when this option is set to ``never``.
 (default=\ ``search``)

 .. only:: html

    .. versionadded:: 4.4

    .. versionchanged:: 5.0
       Configuration option default set to ``search``

.. instopt:: --with-initconf-in=VALUE

 Location where to install Modules initialization configuration files. Either
 ``initdir`` or ``etcdir`` (default=\ ``etcdir``)

 .. only:: html

    .. versionadded:: 4.1

    .. versionchanged:: 5.0
       Configuration option default set to ``etcdir``

.. instopt:: --with-light-background-colors=SGRLIST

 Default color set to apply if terminal background color is defined to
 ``light``. Expect the same syntax than described for
 :instopt:`--with-dark-background-colors`.
 (default=\ ``hi=1:db=2:tr=2:se=2:er=31:wa=33:me=35:in=34:mp=1;34:di=34:al=36:va=33:sy=35:de=4:cm=32:aL=107:L=47:H=2:F=101:nF=103:S=106:sS=104``)

 .. only:: html

    .. versionadded:: 4.3

    .. versionchanged:: 4.6
       Output item for trace information (``tr``) added

    .. versionchanged:: 4.7
       Output items for module tags auto-loaded (``aL``), forbidden (``F``),
       hidden and hidden-loaded (``H``), loaded (``L``), nearly-forbidden
       (``nF``), sticky (``S``) and super-sticky (``sS``) added

    .. versionchanged:: 4.8
       Output item for module variant (``va``) added

.. instopt:: --with-list-output=LIST

 Specify the content to report on list sub-command regular output in addition
 to the loaded module names. Elements accepted in LIST are: ``header``,
 ``idx``, ``variant``, ``sym``, ``tag`` and ``key`` (elements in LIST are
 separated by ``:``). The order of the elements in LIST does not matter.
 (default=\ ``header:idx:variant:sym:tag:key``)

 .. only:: html

    .. versionadded:: 4.7

    .. versionchanged:: 4.8
       Element ``variant`` added and set by default

.. instopt:: --with-list-terse-output=LIST

 Specify the content to report on list sub-command terse output in addition
 to the loaded module names. Elements accepted in LIST are: ``header``,
 ``idx``, ``variant``, ``sym``, ``tag`` and ``key`` (elements in LIST are
 separated by ``:``). The order of the elements in LIST does not matter.
 (default=\ ``header``)

 .. only:: html

    .. versionadded:: 4.7

    .. versionchanged:: 4.8
       Element ``variant`` added

.. instopt:: --with-loadedmodules=MODLIST

 Default modulefiles to load at Modules initialization time. Each modulefile
 in this list should be separated by ``:``. Defined value is registered in the
 :file:`initrc` configuration file or in the ``modulerc`` file if
 :instopt:`--with-initconf-in` is set to ``initdir``.  (default=no)

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --with-locked-configs=CONFIGLIST

 Ignore environment variable superseding value for the listed configuration
 options. Accepted option names in CONFIGLIST are :mconfig:`extra_siteconfig`
 and :mconfig:`implicit_default` (each option name should be separated by
 whitespace character). (default=no)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --with-modulepath=PATHLIST

 Default path list to setup as the default modulepaths.  Each path in this
 list should be separated by ``:``.  Defined value is registered in the
 :file:`initrc` or :file:`modulespath` configuration file, depending on the
 :instopt:`--enable-modulespath` option. These files are respectively called
 ``modulerc`` and ``.modulespath`` if :instopt:`--with-initconf-in` is set to
 ``initdir``. The path list value is read at initialization time to populate
 the MODULEPATH environment variable. By default, this modulepath is composed
 of the directory set for the system modulefiles
 (default=\ ``PREFIX/modulefiles`` or
 ``BASEPREFIX/$MODULE_VERSION/modulefiles`` if versioning installation mode
 enabled)

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --with-moduleshome

 Location of the main Modules package file directory (default=\ ``PREFIX``)

 .. only:: html

    .. versionadded:: 4.4

.. instopt:: --with-nearly-forbidden-days=VALUE

 Define the number of days a module is considered nearly forbidden prior
 reaching its expiry date. VALUE should be an integer comprised between 0 and
 365. (default=\ ``14``)

 .. only:: html

    .. versionadded:: 4.6

.. instopt:: --with-pager=BIN

 Name or full path of default pager program to use to paginate informational
 message output (can be superseded at run-time by environment variable)
 (default=\ ``less``)

 .. only:: html

    .. versionadded:: 4.1

.. instopt:: --with-pager-opts=OPTLIST

 Settings to apply to default pager program (default=\ ``-eFKRX``)

 .. only:: html

    .. versionadded:: 4.1

.. instopt:: --with-python=BIN

 Name or full path of Python interpreter command to set as shebang for helper
 scripts. (default=\ ``python``)

 .. only:: html

    .. versionadded:: 4.5

.. instopt:: --with-quarantine-vars=<VARNAME[=VALUE] ...>

 Environment variables to put in quarantine when running the module command to
 ensure it a sane execution environment (each variable should be separated by
 space character). A value can eventually be set to a quarantine variable
 instead of emptying it. (default=no)

 .. only:: html

    .. versionadded:: 4.1

.. instopt:: --with-search-match=VALUE

 When searching for a module with :subcmd:`avail` sub-command, match query
 string against module name start (``starts_with``) or any part of module name
 string (``contains``). (default=\ ``starts_with``)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --with-tag-abbrev=ABBRVLIST

 Define the abbreviation to use when reporting each module tag. Each element
 in ABBRVLIST is a tag name associated to an abbreviation string (elements in
 ABBRVLIST are separated by ``:``).
 (default=\ ``auto-loaded=aL:loaded=L:hidden=H:hidden-loaded=H:forbidden=F:nearly-forbidden=nF:sticky=S:super-sticky=sS``)

 .. only:: html

    .. versionadded:: 4.7

.. instopt:: --with-tag-color-name=TAGLIST

 Define the tags whose graphical rendering should be applied over their name
 instead of over the name of the module they are attached to. Each element in
 TAGLIST is a tag name or abbreviation (elements in TAGLIST are separated by
 ``:``). (default=)

 .. only:: html

    .. versionadded:: 4.7

.. instopt:: --with-tcl

 Directory containing the Tcl configuration script ``tclConfig.sh``. Useful to
 compile Modules Tcl extension library if this file cannot be automatically
 found in default locations.

.. instopt:: --with-tclinclude

 Directory containing the Tcl header files. Useful to compile Modules Tcl
 extension library if these headers cannot be automatically found in default
 locations.

.. instopt:: --with-tclsh=BIN

 Name or full path of Tcl interpreter shell (default=\ ``tclsh``)

 .. only:: html

    .. versionadded:: 4.0

.. instopt:: --with-terminal-background=VALUE

 The terminal background color that determines the color set to apply by
 default between the ``dark`` background colors or the ``light`` background
 colors (default=\ ``dark``)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --with-unload-match-order=VALUE

 When unloading a module if multiple loaded modules match the request, unload
 module loaded first (``returnfirst``) or module loaded last (``returnlast``)
 (default=\ ``returnlast``)

 .. only:: html

    .. versionadded:: 4.3

.. instopt:: --with-variant-shortcut=SHORTCUTLIST

 Define the shortcut characters that could be used to specify variant names.
 Each element in SHORTCUTLIST is a variant name associated to a shortcut
 character (e.g., ``foo=%``). Shortcuts cannot exceed a length of 1 character
 and cannot be alphanumeric characters ([A-Za-z0-9]) or characters with
 already a special meaning ([+~/@=-]). Elements in SHORTCUTLIST are separated
 by ``:``. (default=)

 .. only:: html

    .. versionadded:: 4.8

.. instopt:: --with-verbosity=VALUE

 Specify default message verbosity. accepted values are ``silent``,
 ``concise``, ``normal``, ``verbose``, ``verbose2``, ``trace``, ``debug`` and
 ``debug2``.  (default=\ ``normal``)

 .. only:: html

    .. versionadded:: 4.3

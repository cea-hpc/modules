.. _lint-sub-command:

lint sub-command
================

Goal is to provide a :subcmd:`lint` sub-command to check syntax of
modulefiles.


Configuration
-------------

* :mconfig:`tcl_linter` configuration option defines linter program to use and
  its options

  * ``nagelfar.tcl`` is the default
  * :instopt:`--with-tcl-linter` and :instopt:`--with-tcl-linter-opts`
    installation options help to change this default at installation time
  * When changed with :subcmd:`config` sub-command, :envvar:`MODULES_LINTER`
    environment variable is set

* Tcl linter program defined is checked for existence by ``configure`` script

  * Use fully qualified path resolved if found
  * Print a warning message if not found
  * No error if not found as :subcmd:`lint` is not the central use case

    * Could be configured or installed after Modules installation

* :instopt:`--enable-nagelfar-addons` installation option defines if
  Nagelfar-specific syntax databases and plugins to lint modulefiles have to
  be installed.

  * Specific syntax databases and plugins to lint modulefiles, modulerc and
    global/user rc are not enabled if this option is disabled

* :instopt:`--nagelfardatadir` installation option defines where to install
  Nagelfar-specific files to lint modulefiles.


Sub-command properties
----------------------

General properties:

* Shortcut name: none
* Accepted option: ``--all``, ``--icase``
* Expected number of argument: 0 to N
* Accept boolean variant specification: no
* Parse module version specification: yes
* Fully read modulefile when checking validity: no
* Sub-command only called from top level: yes

Sub-command acts similarly than :subcmd:`edit` sub-command:

* Resolves each specification passed as argument to a single modulefile
* Modulefile could be specified with icase, extended_default and
  advanced_version_spec features

  * But no boolean specification, as we are looking for modulefiles, not a
    module variant

* Call an external command over the resolved modulefiles

In case no argument is provided, it means to select everything:

* Every global and user rc file
* Every .modulerc and .version files in enabled modulepaths
* Every available modulefiles in enabled modulepaths
* Include modules declared forbidden
* If ``--all`` option set also select all hidden modulefiles in enabled
  modulepaths

Arguments could also be a file path:

* need to distinguish if it is a global/user rc file, a modulerc or a
  modulefile to accurately lint designated file
* file is considered a global/user rc file if file path corresponds to user or
  global rc file location
* file is considered a modulerc if filename equals ``.version`` or
  ``.modulerc``
* otherwise file is considered a modulefile

Processing is aborted as soon as one lint command execution fails


Nagelfar linting
----------------

* Analyze full content of designated files
* It does not evaluate files sourced by designated files
* If :instopt:`--enable-nagelfar-addons` is enabled (default) a specific
  syntax database and plugin is added to the Nagelfar command line to lint
  specified file depending on its type (global/user rc, modulerc or
  modulefile)

  * Modulefile command, their options and syntaxes are checked based on this
    file type
  * Since not the same command set can be used in global/user rc file, in
    modulerc and modulefile


Report output
-------------

* New message block: ``Linting <modulefile>``
* All messages produced by Nagelfar reported under modulefile message block
* Parse Nagelfar messages to report in a structured way:

  * ``<SEVERITY> line <line_number>: message``
  * Message is output as a block in case it consists of several lines
  * All message prefix (prior ``:``) is highlighted based on severity
  * Nagelfar severities mapped to Modules severities:

    * W = WARNING (sgrkey: wa)
    * E = ERROR (sgrkey: er)
    * N = NOTICE (sgrkey: in)

* Output and verbosity levels:

  * By default, do not output message block if no message to report for
    modulefile
  * If verbose mode set, report empty block if no linting message
  * If silent mode set, report nothing, exit code helps to know if linter has
    reported error

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

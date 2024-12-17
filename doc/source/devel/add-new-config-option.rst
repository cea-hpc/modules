.. _add-new-config-option:

Add new configuration option
============================

This document is a guide for Modules developers that wish to introduce a new
configuration option for the :command:`module` command.

Core code
---------

Introducing new configuration option means declaring it then using it (get its
value then branch code in :file:`modulecmd.tcl` script to adapt
:command:`module` command behavior.

#. First declare configuration option in ``g_config_defs`` definition array

    - File to edit: :file:`tcl/init.tcl.in`
    - Example commit: :ghcommit:`2199edf8`

   New configuration option name should be short yet understandable. If
   composed of multiple words, they should be joined by ``_`` character.
   Option value could be superseded by an environment variable, using same name
   than config, upper-cased and prefixed by ``MODULES_``.

   Entry in ``g_config_defs`` definition array uses option name as key then a
   list as value composed with:

    - Superseding environment variable
    - Default value
    - Is configuration lockable to default value (0 or 1)
    - Value kind (``i`` for integer, ``s`` for string, ``b`` for boolean,
      ``l`` for colon-separated list, ``o`` for other)
    - Valid value list (empty list if no validation list)
    - Internal value representation (optional)
    - Specific procedure to call to initialize option value (optional)
    - Valid value list kind (empty string, ``intbe`` or ``eltlist``)

#. Update the Tcl code that compose the :file:`modulecmd.tcl` script to adapt
   behavior depending on the value defined (by default or superseded) for the
   configuration option. Use the ``getConf`` helper procedure to fetch
   configuration option value. This part highly depends on the kind of
   configuration option introduced.

    - File to edit: file(s) in :file:`tcl/` directory
    - Example commit: :ghcommit:`4baf5dc4`

Installation scripts
--------------------

If the default value of the configuration option may be selected at
installation time, an installation option should be added on the
:file:`configure` script.

#. Define installation option argument in :file:`configure` script, add it to
   the internal help message of this shell script, define an internal
   ``<optionname>`` variable that is set to the default installation option
   value, then updated when install option argument is set.

    - File to edit: :file:`configure`
    - Example commit: :ghcommit:`c00ecefa`

#. Define the ``<optionname>`` variable in :file:`Makefile.inc.in` set to the
   ``@<optionname>@`` mark, that will be replaced by chosen value for option
   when :file:`configure` script will generate :file:`Makefile.inc`. Define
   the ``install_<optionname>`` variable in :file:`site.exp.in` to get value
   set for option within testsuite. Update ``translate-in-script`` rule in
   :file:`Makefile` to add translation of the ``@<optionname>@`` mark into
   selected value.

    - Files to edit:

      - :file:`Makefile.inc.in`
      - :file:`Makefile`
      - :file:`site.exp.in`

    - Example commit: :ghcommit:`c00ecefa`

#. Set the ``@<optionname>@`` mark as the default value for configuration
   option in ``g_config_defs`` definition array in Tcl core code

    - File to edit: :file:`tcl/init.tcl.in`
    - Example commit: :ghcommit:`c00ecefa`

   When the *make* installation step is performed the :file:`modulecmd.tcl`
   script file is generated with the ``@<optionname>@`` mark replaced by
   selected default value.

Initialization scripts
----------------------

New configuration option should be referred in the shell completion scripts as
argument for the :subcmd:`config` sub-command.

Files that should be edited to add reference to the new option:

- :file:`init/Makefile` (contains definitions to build *bash* and *tcsh*
  completion scripts
- :file:`init/fish_completion`
- :file:`init/zsh-functions/_module.in`

Example commit: :ghcommit:`abfef4ed`

Documentation
-------------

Man pages and other user documentation have to be updated to describe the
introduced option.

Files that should be edited:

- :file:`doc/source/module.rst` (module manpage)

  - add configuration option description with ``mconfig`` anchor under
    :subcmd:`config` sub-command section
  - add environment variable description with ``envvar`` anchor under
    *ENVIRONMENT* section

- :file:`doc/source/changes.rst`

  - add configuration option under *Modules configuration options* section of
    current Modules major version
  - add environment variable under *Environment* section of current Modules
    major version

- :file:`INSTALL.rst`

  - add configuration and related environment variable and installation
    option in the table under *Configuration options* section

Example commits:

- configuration option doc: :ghcommit:`f8e5eac6`
- related environment variable doc: :ghcommit:`eed719f0`

In case an installation option has been added, it should be covered by documentation

- File to edit: :file:`INSTALL.rst`
- Example commit: :ghcommit:`3d19ab52`

Testsuite
---------

Non-regression testsuite must be adapted first to ensure existing tests still
pass then to add specific tests to check the behavior of the added
configuration option and ensure overall code coverage does not drop.

#. First, clear the run test environment from predefined configuration value,
   set over the associated environment variable.

    - Files that should be edited:

      - :file:`testsuite/modules.00-init/010-environ.exp`
      - :file:`testsuite/install.00-init/010-environ.exp`

    - Example commit: :ghcommit:`1c9fe1bd`

   If default value for option could be set at installation time, it may be
   important to take this chosen default value into account, thanks to the
   ``install_<optionname>`` Tcl variable set in :file:`site.exp`.

#. Add new configuration option to the list of options tested over the
   :subcmd:`config` sub-command.

    - File to edit: :file:`testsuite/modules.70-maint/220-config.exp`
    - Example commit: :ghcommit:`1c9fe1bd`

#. Craft specific tests to validate the correct behavior of the configuration
   when not set, set with valid or invalid values.

    - File to edit: depends on what is impacted by the new configuration
      option, if some tests already exist for concerned behavior (existing
      testsuite file to update) or not (new testfile to create).
    - Example commit: :ghcommit:`122039e5`

#. Optionally if a new installation option has been introduced, it may be
   interesting to adapt CI configuration to test a value different than the
   default one

    - Files to either edit:

      - :file:`.cirrus.yml`
      - :file:`.github/workflows/linux_tests.yaml`

    - Example commit: :ghcommit:`8bf6fb54`

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

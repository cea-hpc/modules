.. _output-configuration:

Output configuration
====================

Configuration
-------------

- Introduce options to define the content of the output:

  - :mconfig:`avail_output`: elements to report on :subcmd:`avail` sub-command regular mode
  - :mconfig:`avail_terse_output`: elements to report on ``avail`` sub-command terse mode
  - :mconfig:`list_output`: elements to report on :subcmd:`list` sub-command regular mode
  - :mconfig:`list_terse_output`: elements to report on ``list`` sub-command terse mode

Specification
-------------

- Output configuration is available

  - For ``avail`` sub-command, on regular and :option:`--terse` modes
  - For ``list`` sub-command, on regular and :option:`--terse` modes

- Output configuration is not available (but could be added later on)

  - For :option:`--long` and :option:`--json` modes
  - For :subcmd:`aliases`, :subcmd:`savelist`, :subcmd:`whatis`/:subcmd:`search` and :subcmd:`display`/:subcmd:`help` sub-commands

- Output configuration defines the content of the output not its format

  - Options :option:`--terse`, :option:`--long` and :option:`--json` defines output format

- These configurations are set by default with the elements reported by default

  - ``avail_output``: ``modulepath:alias:dirwsym:sym:tag:variantifspec:key``
  - ``avail_terse_output``: ``modulepath:alias:dirwsym:sym:tag:variantifspec``
  - ``list_output``: ``header:idx:variant:sym:tag:key``
  - ``list_terse_output``: ``header``

- The above default value could be superseded:

  - with an environment variable, that can be set through the use of the :subcmd:`config` sub-command

    - :envvar:`MODULES_AVAIL_OUTPUT` to supersede ``avail_output`` default value
    - :envvar:`MODULES_AVAIL_TERSE_OUTPUT` to supersede ``avail_terse_output`` default value
    - :envvar:`MODULES_LIST_OUTPUT` to supersede ``list_output`` default value
    - :envvar:`MODULES_LIST_TERSE_OUTPUT` to supersede ``list_terse_output`` default value

  - with the :option:`-o`/:option:`--output` command-line option

    - which applies to the current output mode defined
    - ``-o``/``--output`` also supersedes environment variable definition

- Accepted elements in value lists are:

  - For ``avail`` options: modulepath, alias, dirwsym, indesym, sym, tag, key,
    variant, variantifspec
  - For ``list`` options: header, idx, variant, alias, indesym, sym, tag, key

- If the ``-o``/``--output`` options are wrongly specified

  - An error is raised and evaluation terminates
  - It may happen in the following situations

    - No value set after the option
    - Option set on unsupported module sub-command
    - Element set in option value unsupported by module sub-command
    - Elements set in option value not separated by colon character (``:``)
    - Option set on unsupported output format (``--long`` or ``--json``)
    - The above situations apply whether command is called from the terminal or within a modulefile

- For all these new configuration options

  - accepted value is a list of strings separated by colon character (``:``)
  - order of elements in the list does not matter
  - an empty string is a valid value (means only the modulefile name has to be reported)

- If the ``MODULES_AVAIL_OUTPUT``/``MODULES_AVAIL_TERSE_OUTPUT``/``MODULES_LIST_OUTPUT``/``MODULES_LIST_TERSE_OUTPUT`` env vars are wrongly specified

  - Their value is ignored
  - So the default value takes precedence, unless a ``-o``/``--output`` option is specified
  - Value in environment variable is wrongly specified for instance in the following situations

    - Element set in option value unsupported by module sub-command
    - Elements set in option value not separated by colon character (``:``)

- ``avail_output`` supersedes ``avail_report_dir_sym`` and ``avail_report_mfile_sym`` configurations

  - Enabled ``avail_report_dir_sym`` corresponds to adding ``dirsym`` value to ``avail_output``
  - Enabled ``avail_report_mfile_sym`` corresponds to adding ``sym`` value to ``avail_output``
  - Both ``avail_report_dir_sym`` and ``avail_report_mfile_sym`` could be removed safely as:

    - it was not possible to define them at configure time
    - or change default value with an environment variable

- Some output content cannot be controlled at the moment with the output options:

  - Hidden modules is exclusively controlled by ``--all`` option to get these modules unveiled
  - Indepth/no-indepth output is exclusively controlled by ``--no-indepth/--indepth`` and related configuration option

- When *modulepath* element is removed from an ``avail``-related option

  - all the modulefiles returned from all searched modulepaths are merged and sorted as a single set
  - a module appearing in several modulepaths is only reported once
  - tags or symbols applying to a lower priority module with same name are still reported

Output key
^^^^^^^^^^

- An output key is added to print a legend explaining the output

  - Meaning of ``()``, ``{}`` or ``<>`` is explained
  - Default version is colored specifically
  - Every tag shortened to a defined abbreviation
  - Every tag colored specifically
  - Every variant type set (``variant=value``, ``+boolvariant``, ``-boolvariant``, ``%shortcutvariantvalue``, etc)
  - Legend entries only concern elements that can be found in reported output
  - Legend entries are not reported on JSON output mode

- Output key is enabled by default on ``avail`` and ``list`` sub-command output

  - Key is reported at the end of the output
  - No key section is reported if no element need to be described (no color, no symbol, no tag, etc)

- Variant keys are not adapted for ``avail`` output

  - Even if all possible values are reported instead of the only one set
  - Users should be able to understand ``name=val1,val2,...`` corresponds to
    the ``name=value`` key entry
  - Same applies to the shortcut variant key

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

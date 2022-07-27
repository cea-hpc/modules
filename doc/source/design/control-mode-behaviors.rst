.. _control-mode-behaviors:

Control mode behaviors
======================

- Modulefile commands that modify environment variables applies

  - a behavior when modulefile is loaded
  - and another behavior when modulefile is unloaded

- Depending on the situation, it seems interesting to apply a different
  behavior when modulefile is unloaded:

  - restore value unset at load time, either:

    - append-on-unload
    - prepend-on-unload

  - remove value from list: remove-on-unload
  - unset full variable value: unset-on-unload
  - set another value, either:

    - append-on-unload
    - prepend-on-unload
    - set-on-unload (for non-path-like variable)

  - do nothing: noop-on-unload

- Also when modulefile loads sometimes it may be interesting to apply a
  slightly different behavior depending on the situation:

  - set value only if variable is undefined: set-if-undef

- Concerned modulefile commands:

  - :mfcmd:`module use<module>`
  - :mfcmd:`module unuse<module>`
  - :mfcmd:`setenv`
  - :mfcmd:`unsetenv`
  - :mfcmd:`append-path`
  - :mfcmd:`prepend-path`
  - :mfcmd:`remove-path`

- Only applies when the above commands are called from a modulefile (as some
  of these commands can also act as module sub-commands)

Unload behavior
---------------

- Unload behavior of concerned modulefile commands:

  +--------------+-------------------------+--------------------------+-----------------------+
  | Command      | Default unload behavior | Specific unload behavior | Alternative behaviors |
  |              |                         |                          | to add                |
  +==============+=========================+==========================+=======================+
  | module use   | remove-on-unload        | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | module unuse | remove-on-unload        | *n/a*                    | append-on-unload,     |
  |              |                         |                          | prepend-on-unload,    |
  |              |                         |                          | noop-on-unload        |
  +--------------+-------------------------+--------------------------+-----------------------+
  | setenv       | unset-on-unload         | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | unsetenv     | noop-on-unload          | set-on-unload (when 2nd  | unset-on-unload       |
  |              |                         | argument is provided)    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | append-path  | remove-on-unload        | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | prepend-path | remove-on-unload        | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | remove-path  | noop-on-unload          | *n/a*                    | remove-on-unload,     |
  |              |                         |                          | append-on-unload,     |
  |              |                         |                          | prepend-on-unload     |
  +--------------+-------------------------+--------------------------+-----------------------+

- When either restoring or setting other value for path-like modulefile
  commands, it is needed to know if the value should be appended or prepended.

- Either restoring or setting other value result in the same behavior name

  - *append-on-unload* and *prepend-on-unload*
  - position of the option will determine if the values to set are:

    - those unset at load time: when option is placed prior value list
    - a different list of value: when option is placed after value list to
      unset at load time and another value list is defined after option

- The *set-on-unload* behavior is only useful for modulefile commands which
  unset value on load mode, as commands that set value on load mode mainly
  need to unset the value set.

- Default unload behavior of ``module unuse``, ``unsetenv`` and
  ``remove-path`` commands can also be set as an option in their argument
  list. Helps to dynamically choose the behavior to apply, which could be the
  default behavior.

Load behavior
-------------

- Load behavior of concerned modulefile commands:

  +--------------+-----------------------+-----------------------+
  | Command      | Default load behavior | Alternative behaviors |
  |              |                       | to add                |
  +==============+=======================+=======================+
  | module use   | set-on-load           |                       |
  +--------------+-----------------------+-----------------------+
  | module unuse | remove-on-load        |                       |
  +--------------+-----------------------+-----------------------+
  | setenv       | set-on-load           | set-if-undef          |
  +--------------+-----------------------+-----------------------+
  | unsetenv     | unset-on-load         |                       |
  +--------------+-----------------------+-----------------------+
  | append-path  | add-on-load           |                       |
  +--------------+-----------------------+-----------------------+
  | prepend-path | add-on-load           |                       |
  +--------------+-----------------------+-----------------------+
  | remove-path  | remove-on-load        |                       |
  +--------------+-----------------------+-----------------------+

Modulefile command options
--------------------------

- Alternative behaviors could be specified by setting the associated option
  over the modulefile command:

  - ``--remove-on-unload``
  - ``--append-on-unload``
  - ``--prepend-on-unload``
  - ``--noop-on-unload``
  - ``--unset-on-unload``
  - ``--set-if-undef``

- The ``--append-on-unload`` and ``--prepend-on-unload`` options either:

  - restore the value list removed at load time if placed prior this value
    list
  - set a different value list if a different list of value is specified
    after option (all the values set after the option are considered element
    to add to the path-like variable)
  - both options also accept to be set prior or right after variable name or
    even right at the end of argument list to restore value list removed at
    load time

- The ``--set-if-undef`` option does not change the behavior of ``setenv``
  when modulefile is unloaded: environment variable will be unset whether the
  option is set or not.

- The ``--remove-on-unload`` option can be set anywhere in argument list

- The ``--remove-on-unload``, ``--append-on-unload`` and
  ``--prepend-on-unload`` options cannot be mixed with the ``--index`` option
  of ``remove-path`` sub-command.

- The ``--remove-on-unload``, ``--append-on-unload``, ``--prepend-on-unload``
  and ``--noop-on-unload`` options:

  - can be placed at different positions in argument list to limit error
    cases
  - if multiple options of this kind are defined in argument list, the one
    defined at the most right is retained

Misc
----

- *FUTURE*: what is applied to:

  - ``module use`` may also be interesting for ``module load``
  - ``module unuse`` may also be interesting for ``module unload``

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

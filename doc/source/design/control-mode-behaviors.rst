.. _control-mode-behaviors:

Control mode behaviors
======================

- Modulefile commands that modify environment variables applies

    - a behavior when modulefile is loaded
    - and another behavior when modulefile is unloaded

- Depending on the situation, it seems interesting to apply a different
  behavior when modulefile is unloaded:

    - restore value unset at load time: restore-on-unload
    - remove value from list: remove-on-unload
    - unset full variable value: unset-on-unload
    - set another value: set-on-unload

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

- Unload behavior of concerned modulefile commands:

  +--------------+-------------------------+--------------------------+-----------------------+
  | Command      | Default unload behavior | Specific unload behavior | Alternative behaviors |
  |              |                         |                          | to add                |
  +==============+=========================+==========================+=======================+
  | module use   | remove-on-unload        | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | module unuse | remove-on-unload        | *n/a*                    | restore-on-unload,    |
  |              |                         |                          | set-on-unload         |
  +--------------+-------------------------+--------------------------+-----------------------+
  | setenv       | unset-on-unload         | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | unsetenv     | *nop*                   | set-on-unload (when 2nd  | unset-on-unload       |
  |              |                         | argument is provided)    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | append-path  | remove-on-unload        | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | prepend-path | remove-on-unload        | *n/a*                    |                       |
  +--------------+-------------------------+--------------------------+-----------------------+
  | remove-path  | *nop*                   | *n/a*                    | restore-on-unload,    |
  |              |                         |                          | remove-on-unload,     |
  |              |                         |                          | set-on-unload         |
  +--------------+-------------------------+--------------------------+-----------------------+

- *Note*: the *set-on-unload* behavior is only useful for modulefile commands
  which unset value on load mode, as commands that set value on load mode
  mainly need to unset the value set

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

- Alternative behaviors could be specified by setting the associated option
  over the modulefile command:

    - ``--restore-on-unload``
    - ``--set-on-unload``
    - ``--unset-on-unload``
    - ``--remove-on-unload``
    - ``--set-if-undef``

- The ``--set-on-unload`` option requires an argument to define the value to
  set when unloading the module

    - All the values set after the option are considered element to add to the
      path-like variable

- The ``--set-if-undef`` option does not change the behavior of ``setenv``
  when modulefile is unloaded: environment variable will be unset whether the
  option is set or not.

- *FUTURE*: what is applied to:

    - ``module use`` may also be interesting for ``module load``
    - ``module unuse`` may also be interesting for ``module unload``

.. _lmod-tcl-modulefile-compat:

Lmod Tcl modulefile compatibility
=================================

- Goal is to be able to evaluate with :command:`modulecmd.tcl` a Tcl
  modulefile generated for Lmod

  - No evaluation error
  - Be as close as possible to the behavior of Lmod

- As of Modules v5.0, the following Tcl modulefile command of Lmod are not
  supported:

  - ``add-property``
  - ``remove-property``
  - ``extensions``
  - ``depends-on``
  - ``prereq-any``
  - ``always-load``
  - ``module load-any``
  - ``pushenv``
  - ``require-fullname``
  - ``family``


``add-property``/``remove-property``
------------------------------------

- Property mechanism of Lmod does not completely fit into Modules tag
  mechanism:

  - Property are first defined in a global RC file
  - Then associated to a given modulefile with ``add-property`` command
  - Could be unset with ``remove-property``
  - Defined as a key-value

- These commands will be first implemented as a no-operation command (``nop``)

  - No error raised if used
  - And no warning message to avoid polluting output

- These commands are intended for use only within modulefile evaluation
  context (not within modulerc)

- *FUTURE*: it could be interesting to map some properties on tags like the
  ``lmod:sticky`` property which corresponds to the ``sticky`` tag


``extensions``
--------------

- Extension mechanism is an additional information added on ``avail`` and
  ``spider`` sub-command output

  - Requires to evaluate modulefile on ``avail`` processing
  - Not used to load the modulefile declaring them when an extension name is
    passed to ``load`` sub-command or ``depends-on`` modulefile command

- This command will be first implemented as a no-operation command (``nop``)

  - No error raised if used
  - And no warning message to avoid polluting output

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)

- *FUTURE*: may be interesting to improve the concept by making the extension
  names an alias on the modulefile declaring them

  - Will automatically achieve dependency consistency this way
  - Could resolve dependencies based on extension names if modulefile is
    evaluated during an ``avail``


``prereq-any``
--------------

- This command is an alias over ``prereq`` command

  - In Lmod, ``prereq`` acts as a *prereq-all*
  - Whereas on Modules ``prereq`` acts already as a *prereq-any*

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)


``require-fullname``
--------------------

- Raise an error if modulefile has been designed with implicit or explicit
  default name

  - This error aborts modulefile evaluation
  - Occurs for instance if ``foo/1.0`` is loaded with ``foo`` or
    ``foo/default`` name
  - Which means ``require-fullname`` has precedence over explicitly set
    default version
  - If an alias or a symbolic version (other than ``default``) point to the
    modulefile, no error occurs if modulefile is designated using these
    alternative names.

- ``require-fullname`` is implemented to only apply on *load* evaluation mode

  - On Lmod, it applies on *load*, *unload*, and *display* modes
  - It seems important not to apply the constraint on *unload* and *display*
    modes to ease user's experience

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)


``depends-on``
--------------

- Auto load one or more modules said as dependencies when modulefile is
  evaluated in *load* mode

  - Corresponds to the *Requirement Load* module auto handling mechanism.
  - Semantically this command corresponds to a requirement declaration.
  - Make it an alias over :mfcmd:`prereq` but with each argument set as a
    *prereq*all* not a *prereq-any*.
  - If :mconfig:`auto_handling` option is disabled, requirement will not be
    loaded and an error is raised. This will be different than Lmod as
    with Modules the modulefile commands defines the semantic (*this is
    a dependency*) then the automation is defined by the module command
    configuration, not by the modulefile like done in Lmod.

- Auto unload the dependency modules when modulefile is unloaded if no other
  loaded module depends on them

  - Corresponds to the *Useless Requirement Unload* module auto handling
    mechanism
  - Like for *load* evaluation, automation is configured at the module
    command level, not by individual modulefiles

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)


``always-load``
---------------

- Auto load on or more modules said as dependencies when modulefile is
  evaluated in *load* mode

  - Semantically this command corresponds to a requirement declaration.
  - Make it an alias over :mfcmd:`module load<module>`
  - Add ``keep-loaded`` tag to the modules loaded this way
  - When several modules are specified, it acts as an *AND* operation, which
    means all specified modules are required

- When modulefile is unloaded, the *always-load* modules are not automatically
  unloaded as they own the ``keep-loaded`` tag

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)


``module load-any``
-------------------

- Auto load first valid module in a list when modulefile is evaluated in
  *load* mode

  - Semantically this command corresponds to a requirement declaration.
  - Acting as an *OR* operation
  - Evaluation stops after first module in list loaded

    - Whether called from a modulefile evaluation context or from top
      evaluation context
    - Different than Lmod that apply the :subcmd:`load` sub-command
      behavior when called from top evaluation context and does not stop
      after first modulefile loaded

  - If the evaluation of first module to load in list ends in error

    - When called from a modulefile evaluation context

      - Error is silenced
      - Next module in list is tried
      - It behaves this way like a :mfcmd:`prereq` command with
        auto_handling mode enabled
      - Proceed this way whatever the auto_handling state
      - Different than Lmod that aborts modulefile evaluation

    - Otherwise when called from top evaluation context

      - Error message is reported
      - Next module in list is tried
      - Different than Lmod that aborts processing

  - If first modules to load are unknown

    - No message reported
    - ``load-any`` continues until finding a module in the specified list

  - If a module in the list is already loaded

    - When called from a modulefile evaluation context

      - ``load-any`` is not performed as requirement is considered
        already satisfied
      - Better cope this way with the expressed requirement
      - It behaves this way like a :mfcmd:`prereq` command
      - Proceed this way whatever the auto_handling state
      - Different behavior than Lmod that still proceed to load the
        module in the list from the left to the right until loading one
        or finding one loaded

    - Otherwise when called from top evaluation context

      - An attempt to load first module in list is still issued
      - And pursued from left to right until loading one module or
        finding one loaded

  - ``load-any`` acts similarly to ``try-load`` but with an *OR* operation
    behavior instead of an *AND* operation

  - An error is obtained if none of the listed modules can be loaded if
    none of their load attempt generated an error message

  - If no argument is provided an error is obtained, like done for
    ``try-load``

- When modulefile is unloaded, an attempt to unload all specified module is
  made

  - Correspond to the behavior of a ``module unload``
  - Modules which are still depended by other loaded modules will not be
    unloaded

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)


``module try-load``
-------------------

- ``try-load`` sub-command and modulefile command has been introduced in
  Modules version 4.8.

- ``try-load`` modulefile command acts as an individual ``prereq`` for each
  modulefile specified.

- Each modulefile specified are considered optional requirement, as no error
  is raised if they cannot be loaded.

  - An update is made on version 5.2 to record each modulefile specified on
    ``try-load`` as optional requirement even if their load attempt did not
    succeed.
  - With this change, if the optional requirement is loaded later on, the
    module declaring the ``try-load`` command will be automatically reload
    (if ``auto_handling`` is enabled) to take the new availability of its
    optional requirement into account.


``family``
----------

- Defines membership in family *name* and ensures that only one member of a
  given family is currently loaded.

  - Semantically this command corresponds to the definition of both:

    - a conflict on family *name*
    - a module alias *name* over currently loading module

- Also defines the :envvar:`MODULES_FAMILY_\<NAME\>` environment variable set
  to the currently loading module name minus its version number.

  - As family *name* is used in environment variable name, it requires that
    *name* should only use characters that are accepted there
  - Accepted characters for family *name* are *[a-zA-Z0-9_]*
  - An error is generated in case other kind of characters are found in
    specified family *name*

- The :envvar:`LMOD_FAMILY_\<NAME\>` environment variable is also set in
  addition to :envvar:`MODULES_FAMILY_\<NAME\>` and set to the same value.
  This way existing scripts or modulefiles relying on this variable do not
  need to be changed to be compatible with Modules.

- When modulefile is unloaded, the ``MODULES_FAMILY_<NAME>`` and
  ``LMOD_FAMILY_<NAME>`` environment variables are unset

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)


``pushenv``
-----------

- Sets an environment variable with a value specified as argument but saves
  the previous value set to restore it when modulefile is unloaded.

- Use a :envvar:`__MODULES_PUSHENV_\<VAR\>` environment variable as a stack to
  record the previous values of environment variable ``<VAR>``.

  - Each element in this Modules-specific variable is the combination of the
    currently evaluating modulename and pushed value.

    - Combination joined with the ampersand character
    - Each element in variable separated by colon character

  - When unloading, the value set by this module is removed not the value on
    top of the list.
  - Different than Lmod that restores the value on top of the stack even if
    unloading module were not the one defining the top value currently in
    use.

- When saving value set before any module

  - An empty module name is used to push to the stack.
  - When restoring this initial value, initial entry in stack is also
    cleared (as no other module unload will unset it).

- It is not expected that for the same environment variable, :mfcmd:`pushenv`
  is mixed with:

  - ``setenv``, ``unsetenv``
  - ``append-path``, ``prepend-path``, ``remove-path``
  - These other modulefile commands clear the pushenv stack environment
    variable (like ``setenv``/``unsetenv`` clear the reference counter
    environment variable of the ``*-path`` commands)

- It is not expected that :mfcmd:`pushenv` is called multiple times for the
  same environment variable in the same modulefile

  - Inconsistent results may be obtained if environment variable value is
    used in modulefile to set other variables.
  - Especially that unload evaluation of modulefile will not process the
    ``pushenv`` commands in the reverse order but in the script order.
  - When checked during modulefile evaluation, lastly defined value remains
  - However the operation is consistent at the end of modulefile evaluation,
    as all values are withdrawn from stack and a value defined somewhere
    else is restored.
  - pushenv stack environment variable correctly handles multiple entries
    coming from same modulefile, even multiple identical values.

- For Lua modulefiles, Lmod handles a specific ``false`` value which clears
  environment variable

  - Lmod does not implement this for Tcl modulefile
  - Maybe because ``false`` cannot be distinguished from any other value
  - So this specific behavior is also not supported on Modules

- This command is intended for use only within modulefile evaluation context
  (not within modulerc)

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

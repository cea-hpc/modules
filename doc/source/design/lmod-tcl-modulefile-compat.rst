.. _lmod-tcl-modulefile-compat:

Lmod Tcl modulefile compatiblity
================================

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

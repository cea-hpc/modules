.. _mod-to-sh-sub-command:

mod-to-sh sub-command
=====================

Produce shell code of environment changes made by loading designated
modulefiles and send this content to the report message channel. User views
the produced shell code as output.

:subcmd:`mod-to-sh` could be send as the opposite of :subcmd:`sh-to-mod`. The
former transforms a modulefile into shell code whereas the latter transforms
shell code into a modulefile.


Configuration
-------------

None.


Sub-command properties
----------------------

General properties:

* Shortcut name: none
* Accepted option: ``--force``, ``--icase``, ``--auto``, ``--no-auto``
* Expected number of argument: 2 to N
* Accept boolean variant specification: yes
* Parse module version specification: yes
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: yes (``load`` mode)

Arguments expected:

* First argument should be a supported shell name

  * like *bash* or *fish*
  * An error is produced if an unsupported shell is specified
  * It defines the language that sub-command will use to produce shell code

* After the first argument, comes the modulefile specification

  * Multiple modulefiles can be specified
  * With advanced version specifiers and variants
  * It defines the modulefiles to load

``mod-to-sh`` loads designated modulefile(s) and instead of rendering the
corresponding environment changes to the stdout channel, the shell code is
sent to the message report channel (stderr).

Target shell is changed, from the one specified as argument to
:file:`modulecmd.tcl` script to the one specified as argument to ``mod-to-sh``
sub-command.

Once shell code is produced and sent to the message channel (stderr), target
shell defined for :file:`modulecmd.tcl` is restored to get on the regular
code channel (stdout) the evaluation status. For instance for *sh* shell:
``test 0`` if successful, ``test 0 = 1`` otherwise.

Shell code produced by ``mod-to-sh`` is only sent to message channel. It is
not sent to the code channel.

``modtosh_real_shell`` state is introduced and set by ``mod-to-sh``
sub-command. When defined, shell code rendering procedure knows if code should
be sent to the message channel and then cleared, not to be rendered on code
channel. This state contains the target shell of :file:`modulecmd.tcl`. So
shell rendering procedure can restore target shell then produce the final
shell code to render (evaluation status).

Modulefile evaluation is made like a regular ``module load``. Thus if
specified modulefile is already loaded, its corresponding environment changes
will not appear on the ``mod-to-sh`` result.

Shell code produced for ``mod-to-sh`` is filtered to only contain environment
changes of designated modulefiles and not the Modules-specific environment
context (like ``LOADEDMODULES`` or ``_LMFILES_`` environment variables).

``mod-to-sh`` sub-command automatically adjusts the :mconfig:`verbosity`
configuration option to the ``silent`` mode. As produced shell code is sent
to the message channel, it should not mix with report messages. Setting
verbosity to ``silent`` disables loading messages or :mfcmd:`reportWarning`,
:mfcmd:`reportError` and :mfcmd:`puts stderr<puts>` messages defined in
modulefiles. :mconfig:`verbosity` is not changed if set to ``trace`` mode or
any higher mode (debugging).


module-info command
-------------------

As ``mod-to-sh`` leads to modulefile evaluation, the :mfcmd:`module-info
command<module-info>` modulefile command returns the ``mod-to-sh`` string
when used in a modulefile evaluated through this sub-command. It can also be
queried against this command name string to return Boolean value depending if
``mod-to-sh`` is the sub-command currently running.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

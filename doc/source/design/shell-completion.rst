.. _shell-completion:

Shell completion
================

Goal is to provide tool to define shell completion through modulefile. Need
comes from the ability of the :mfcmd:`source-sh` modulefile command to
reproduce :ref:`the shell changes made by a sourced shell
script<source-shell-script-in-modulefile>`. Completion definitions are part
of those shell changes, so there is a need to be able to define them through
modulefile command.

:mfcmd:`complete` and :mfcmd:`uncomplete` modulefile commands are introduced
to give this ability to define or unset completion for commands in shell that
provides a command to define such completion. Such shells are:

* bash
* tcsh
* fish

No shell command to setup completion has been found on other shells. For
instance on zsh, completion is made through files saved in specific
directories.


complete
--------

:mfcmd:`complete` takes as argument:

* shell: name of the shell this completion definition should be applied to
* command: name of the command the completion targets
* completion arguments aka body: parameter to define completion on shell

*Shell* should correspond to one of the supported shell name. When modulefile
is evaluated, the :mfcmd:`complete` command will produce shell code as output
only if *shell* name corresponds to the shell the :file:`modulecmd.tcl` script
has been initialized for.

If an unsupported *shell* name is set, :mfcmd:`complete` command is simply
ignored. No error is produced as the shell specified may be supported on a
latter version of the :file:`modulecmd.tcl` script and the modulefile may be
provided for use by different version of this script.

*Command* name should be a non-empty string. Otherwise an evaluation error is
returned.

*Body* is shell-specific. When modulefile is evaluated the body defined on a
:mfcmd:`complete` command is not checked by module. Possible options are
numerous and really different from one shell to another so it is up to the
modulefile author to ensure the body definition used is correctly supported on
targeted shell.

*Body* value is not escaped to fit shell evaluation. Defined syntax should be
compatible with this evaluation. It is up to the modulefile writer to use the
correct syntax as it cannot be guessed depending on what writer wants to
achieve.

For shells like fish:

* Completion definition consist of multiple complete commands for a single
  *command* name.
* Prior applying completion definition, any previous completion is cleared for
  *command*. We ensure modulefile provides the full completion definition for
  *command* this way.

**Note**: when loading module on fish shell, completion is first fully removed
for *command*. It is expected that modulefile provides the full completion
definition for *command* and not rely on previous definition made elsewhere.

:mfcmd:`complete` is evaluated on *load* but also *refresh* mode. Important on
the later to get completion correctly redefined when initializing sub-shells.
On *unload* mode, :mfcmd:`complete` becomes :mfcmd:`uncomplete`.

uncomplete
----------

:mfcmd:`uncomplete` is evaluated on *unload* mode. Like other *undo* command,
it is ignored when unloading a modulefile.

:mfcmd:`uncomplete` does not require a *shell* name as argument. Only the
*command* name is expected.

*Command* name should be a non-empty string. Otherwise an evaluation error is
returned.

:mfcmd:`uncomplete` command is provided in case of a need to specifically
unset a defined completion. There should not be a lot of use case for this
command, but it does not cost much to add as it is needed to undo the
:mfcmd:`complete` command when a modulefile is unloaded.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

.. _conflict-unload:

Conflict unload
===============

This document described the Conflict Unload mechanism (ConUn) part of the
automated module handling mechanisms. Conflict Unload goals are to remove
conflicting modules when loading a new one.

``conflict_unload`` configuration option
----------------------------------------

A new configuration option :mconfig:`conflict_unload` is introduced as
Conflict Unload introduces a new behavior. :mconfig:`auto_handling` option
should also be enabled to activate Conflict Unload.

``conflict_unload`` option is disabled by default not to change by default the
current behavior, as this option is introduced on a feature release (not a
major release).

``conflict_unload`` option will be removed and behavior merged into
``auto_handling`` option at next major version change (v6).

New behavior
------------

When a module is loading, Conflict Unload mechanism unloads all loaded modules
conflicting with the new module to load or the modules it requires. Goal is to
satisfy the last module load request (i.e., produce an environment that
satisfy this request).

As a result, a module load triggering a Conflict Unload mechanism may as a
consequence also trigger Dependent Reload, Dependent Unload and Useless
Requirement Unload mechanisms. These last 2 mechanisms were not triggered by
a top load action prior that (only on a top unload or switch actions). These
mechanisms are triggered either by the top level module to load or the
modules it requires.

*FUTURE*: another approach would be to satisfy the load request without
changing the environment asked up to now. So auto-loaded modules may be
changed, as well as modules asked by they generic name (e.g., *name*, not
their fully qualified name (e.g., *name/version*). This approach may be
developed in the future and a specific configuration option is be needed to
define the expected Conflict Unload approach.

Behavior examples
-----------------

The following modules are loaded and form a dependency chain. Each module
declares a reflexive conflict::

    A1 > B1 > C1

After a ``module load B2``:

* Loaded environment should be ``A2 > B2 > C1``
* C1 should be unloaded by Dependent Reload mechanism
* B1 should be unloaded by Conflict Unload mechanism
* A1 should be unloaded by Useless Requirement Unload
* A2 should be loaded by Requirement Load
* B2 is then loaded
* C1 is then reloaded again

After a ``module load C2``:

* Loaded environment should be ``A2 > B2 > C2``
* C1 should be unloaded by Conflict Unload mechanism
* B1 and A1 should be unloaded by Useless Requirement Unload
* A2 and B2 be loaded by Requirement Load
* C2 is then loaded

Conflict kinds
--------------

Conflict Unload mechanism should target the 3 different conflict situations:

* Conflicts declared by loaded modules against loading modules
* Conflicts declared by loading modules against loaded modules (through
  ``conflict`` or ``module unload/switch`` modulefile commands)
* Loading module is already loaded but with a different set of variants

Loading modules here apply to asked module and the modules it requires.

Implementation
--------------

Current implementation is to apply Conflict Unload mechanism either on loading
top module or the modules it requires. Subsequent mechanisms produced by ConUn
(UReqUn, DepRe and DepUn) are thus also produced on each loading module (top
or requirements). Produces where they occur and not globally scheduled.

Such implementation is less optimized than resolving every requirements and
conflicts information to get expected loaded modules in a minimum number of
evaluations.

Here, a module spotted as UReqUn may be a ReqLo for the new module to load.
Thus depending on the modulefile definition, the UReqUn module may be unloaded
and then reloaded instead of staying still.

When loading a module, prior evaluating it if a conflict is declared against
it (or is already loaded with an alternative variant set) by one or more:

* loaded modules, unload this or these modules. This or these unloads may
  trigger either DepRe, DepUn and UReqUn mechanisms.
* loading modules, raise an error as requested environment cannot be satisfied

When evaluating a loading modulefile (either asked module or its
requirements) and this module defines a conflict:

* if one or more loaded modules match this conflict, unload them (and trigger
  DepRe, DepUn and UReqUn mechanisms if needed)
* if one or more loading modules match this conflict, raise an error as
  requested environment cannot be satisfied

Unload of multiple conflicting modules is achieved in the reversed loaded
order (last loaded is unloaded first).

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

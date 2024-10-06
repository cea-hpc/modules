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
a top load action prior that (only on a top unload or switch actions). DepUn
and DepRe mechanisms are triggered either by the top level module to load or
the modules it requires. UReqUn mechanism is only triggered by the top level
module to load.

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
* A1 should be unloaded by Conflict Unload mechanism
* A2 should be loaded by Requirement Load
* B2 is then loaded
* C1 is then reloaded again

After a ``module load C2``:

* Loaded environment should be ``A2 > B2 > C2``
* C1 should be unloaded by Conflict Unload mechanism
* B1 and A1 should be unloaded by Conflict Unload
* A2 and B2 be loaded by Requirement Load
* C2 is then loaded

Conflict kinds
--------------

Conflict Unload mechanism targets the 3 different conflict situations:

* Conflicts declared by loaded modules against loading modules
* Conflicts declared by loading modules against loaded modules (through
  ``conflict`` or ``module unload/switch`` modulefile commands)
* Loading module is already loaded but with a different set of variants

Loading modules here apply to asked module and the modules it requires.

Implementation
--------------

Current implementation is to apply Conflict Unload mechanism either on loading
top module or the modules it requires. Subsequent mechanisms produced by ConUn
(DepRe and DepUn) are thus also produced on each loading module (top or
requirements). Produces where they occur and not globally scheduled. UReqUn is
globally scheduled on top evaluation.

Such implementation is less optimized than resolving every requirements and
conflicts information to get expected loaded modules in a minimum number of
evaluations.

When loading a module, prior evaluating it, if a conflict is declared against
it (or this module is already loaded with an alternative variant set) by one
or more:

* loaded modules, unload this or these modules. This or these unloads may
  trigger either DepRe and DepUn mechanisms.
* loading modules, raise an error as requested environment cannot be satisfied

When evaluating a loading modulefile (either asked module or its
requirements) and this module defines a conflict:

* if one or more loaded modules match this conflict, unload them (and trigger
  DepRe and DepUn mechanisms if needed)
* if one or more loading modules match this conflict, raise an error as
  requested environment cannot be satisfied

Unload of multiple conflicting modules is achieved in the reversed loaded
order (last loaded is unloaded first).

UReqUn modules is scheduled after main load action, thus only from the top
context. At this point all auto-loaded requirements of ConUn modules can be
found and set as UReqUn modules to unload.

Adaptation of automated module handling mechanisms
--------------------------------------------------

Introduction of *Conflict Unload* mechanism requires to adapt some of the
other automated module handling mechanisms. It is also a good opportunity to
improve these mechanisms.

Useless Requirement Unload
^^^^^^^^^^^^^^^^^^^^^^^^^^

Changes made to the UReqUn mechanism:

* UReqUn module list is computed after main module action
* UReqUn mechanism is only triggered on top level evaluation

  * Avoid to trigger the unload of modules that may be ReqLo in the remaining
    parts of the process
  * As a consequence some modules are considered ConUn modules instead of
    UReqUn modules


* UReqUn is introduced on load evaluation (to unload useless requirements
  coming from unload of ConUn modules)
* For switch evaluation, UReqUn is moved from end of switch-unload phase to
  end of overall switch action (before DepRe reload phase)

  * As a consequence, some UReqUn modules may be considered ConUn modules of
    switched-on module.
  * It advocates for enabling ``conflict_unload`` option otherwise an error
    would be obtained on such situation (which was not the case previously
    as UReqUn process occurred at the end of switch unload phase)

* DepRe modules that also are UReqUn modules are unloaded during DepRe unload
  phase, rather extracted from DepRe to be processed during UReqUn. These
  modules are reported as UReqUn modules to users (as they are not reloaded)
* DepRe modules also identified as UReqUn modules, may not be a dependency of
  an unloaded module (like the other UReqUn modules).

Dependent Reload
^^^^^^^^^^^^^^^^

Changes made to the DepRe mechanism:

* DepRe modules coming from UReqUn modules (i.e., modules in conflict with
  UReqUn modules) are computed when computing UReqUn module list and their
  DepRe unload phase is made right before UReqUn module unload, thus apart
  from the other DepRe modules

  * As a consequence these DepRe modules may not be reloaded in the same order
    compared to the other DepRe modules

* Unload phase of DepRe modules is mixed with unload of DepUn modules in order
  to perform these unload in the reverse load order (to ensure a module is
  unloaded before its requirements)

* Reload of DepRe module is attempted only it is loadable. Which means its
  requirements are loaded and it does not conflict with current loaded
  environment. If module is sticky (super-sticky or sticky and not forced)
  attempt to load DepRe module is performed even if not loadable. If force
  mode is enabled reload of non-loadable DepRe module is not attempted, unless
  if tagged super-sticky. Not reloaded DepRe module is considered as a DepUn
  module.

* All reloads of DepRe modules are now performed on top load, unload or switch
  evaluation rather right after the load, unload or switch that triggers the
  unload phase of the DepRe mechanism. As a consequence, all Dependent Reload
  modules are reloaded after main module evaluation.

Notes
-----

* A DepUn module of a ConUn module, may be a ReqLo of loading module

  * Depending on the order of declaration of the requirement and conflict, a
    different kind of error is obtained.
  * If requirement is defined prior conflict, ReqLo will seem satisfied when
    executing the prereq or module load command, but an attempt to unload it
    will occur afterward.

* A DepRe module of a ConUn module, may also be a ReqLo of loading module

  * DepRe is unloaded locally by the ConUn evaluation but reloaded at the end
    of the overall evaluation. It if is also a ReqLo, it will be reloaded when
    the ReqLo is expressed, instead of being reloaded at the very end.
  * When DepRe is reloaded as ReqLo, it currently looses its extra tags and
    gets the auto-loaded tag applied.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

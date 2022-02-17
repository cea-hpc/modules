.. _sticky-modules:

Sticky modules
==============

Configuration
-------------

- No specific configuration

Specification
-------------

- Once loaded a sticky module cannot be unloaded unless forced or reloaded.
- If module is super-sticky instead of sticky, it cannot be unloaded even if forced, only if it is reloaded afterward

- Stickyness definition relies on :ref:`module-tags`

    - ``module-tag sticky foo/1.0`` defines module *foo/1.0* as sticky
    - ``module-tag super-sticky foo/1.0`` defines module *foo/1.0* as super-sticky

- Stickyness specified over a symbolic version or a module alias has no effect

    - ``module-tag`` allows to specify a symbolic module version or a module alias
    - but associated tag will apply to the symbolic version or alias only
    - as modulefile targeted by symbol or alias does not inherit their tags
    - so a sticky or super-sticky tag set on a symbolic version or alias has no effect

- Sticky module can be swapped with another version from same module when stickiness is defined over module parent name

    - For instance if stickyness is defined over module *foo*, *foo/1.0* can be swapped with *foo/2.0*
    - Such swap could occur from a ``restore`` or a ``switch`` sub-command
    - As soon as stickyness is defined over a precise module version name (like *foo/1.0*) such module cannot be swapped by another version of *foo* module
    - Stickyness defined over module parent name (like *foo*) means *any version from module foo must be loaded*
    - When stickyness is defined for several module versions using advanced version specifiers like *foo@1:* or *foo@1.0,2.0*

        - it means stickyness applies to the module versions
        - thus they cannot be swapped by another version

    - In case stickiness is defined over module parent name and another ``module-tag`` defines stickyness over specific module version name

        - it means stickyness applies to the module version
        - thus these versions targeted specifically with ``module-tag`` cannot be swapped by another version from same module

- When a super-sticky module depends on a non-super-sticky module

    - If a forced ``purge`` command occurs, the dependent module will be unloaded
    - Which let the super-sticky module with a missing dependency

- An error is reported when trying to unload a sticky or super-sticky module

    - As the expected unload is not performed
    - So with the return code and message, user knows that what he/she asked for was not done
    - Same goes for the ``purge`` command: user expects no more module loaded

        - so an error is returned as after the command the sticky module is still loaded

    - When the unload is forced a warning message is reported

        - the sticky module is unloaded as expected but a warning message is done
        - as the module was tagged not to be unloaded by default
        - user should know he/she has done something specific


Current limitations
-------------------

- When swapping a sticky module explicitly targeted by the ``module-tag`` command and which is the default version

    - For instance ``module-tag sticky foo/1.0`` and ``module-version foo/1.0 default``
    - If specified swapped-on module is the generic module name, for instance *foo*
    - ``switch`` sub-command raises an error even if the sticky module is the default version (either implicit or explicitly set) for this module

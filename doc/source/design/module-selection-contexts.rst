.. _module-selection-contexts:

Module selection contexts
=========================

Description of the different contexts where a module is expressed to get resolved to modulefiles.

.. _module_identification_to_select_one_module:

Module identification to select one module
------------------------------------------

- specification expresses exactly one module
- must qualify version to select in case a module has multiple versions
- expression may
    - be absolute like *mod/version*
    - be relative to the modulefile currently being evaluated like *./version* or */version*
    - refer to symbolic version or module alias
- specification used to resolve argument passed to the following commands:
    - ``module-version``
    - ``module-alias``
    - ``module-virtual`` (cannot use symbolic version or module alias, should define an actual module)
    - ``module-info symbols``
    - ``module-info version``
    - ``module-info loaded``
- relies on ``getModuleNameVersion`` procedure to get absolute name

.. _module_version_specification_to_return_all_matching_modules:

Module version specification to return all matching modules
-----------------------------------------------------------

- specification expresses one or multiple modules
- expression may
    - be absolute like *mod/version*
    - refer to implicit or explicitly defined default version like *mod*
    - refer to extended default version like *mod/1* (to refer to *mod/1.1.1*)
    - refer to symbolic version or module alias
    - specify multiple versions as list (*mod@1,2*) or range (*mod@:1* or *mod@1:2*)
- all modules matching specification are retrieved with ``getModules``
- specification used to resolve argument passed to the following commands:
    - ``avail``
    - ``paths``
    - ``whatis``
- used to get all alias and symbolic version of loaded modules treated by:
    - ``list``
- used to get all existing aliases and symbolic version (no module specification) by:
    - ``aliases``
- used to get all existing modulefiles (no module specification) by:
    - ``search``

.. _module_version_specification_to_select_one_module:

Module version specification to select one module
-------------------------------------------------

- specification expresses one or multiple modules
- relies on the module version specification to return all matching modules
- then among matching modules, one is selected with ``getPathToModule``:
    - the one set has the default version (also called the explicit default version)
    - or the highest version in case no explicit default is found among results (also called the implicit default version)
    - nothing is returned in case no explicit default is found among matching modules and implicit default version mechanism is disabled
- module selection may trigger another match retrieval
    - for instance in case selection leads to an alias that resolves to a bare module name
- specification used to resolve argument passed to the following commands:
    - ``load``
    - ``unload`` (with attempt to match against loaded modules to resolve argument)
    - ``switch``
    - ``help``
    - ``test``
    - ``display``
    - ``path``
    - ``is-avail``
- used to resolve aliases or symbolic versions treated by following commands:
    - ``paths``
    - ``search``

.. _module_version_specification_to_check_compatibility:

Module version specification to check compatibility
---------------------------------------------------

- specification expresses one or multiple modules
- expression may
    - be absolute like *mod/version*
    - refer to implicit or explicitly defined default version like *mod*
    - refer to extended default version like *mod/1* (to refer to *mod/1.1.1*)
    - refer to symbolic version or module alias
    - specify multiple versions as list (*mod@1,2*) or range (*mod@:1* or *mod@1:2*)
- version specification is matched against loaded or loading modules with variety of procedures:
    - ``getLoadedMatchingName``
    - ``getLoadedWithClosestName``
    - ``isModuleEvaluated``
    - ``doesModuleMatchesName``
    - ``doesLoadingModuleMatchesName``
- specification used to resolve argument passed to the following commands:
    - ``is-loaded``
    - ``info-loaded``
    - ``prereq``
    - ``conflict``
- for prereq command when version specification does not have a match among loaded or loading modules
    - version specification is used to select one module per module specification
    - for instance ``prereq mod@:1.8 mod@1.10`` triggers load tentative of default version among *mod@:1.8* then if this tentative fails load of *mod@1.10* is attempted
    - another example with ``prereq mod mod@1.10``, which triggers load tentative of *mod/default* then if it fails load of mod@1.10 is attempted


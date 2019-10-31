.. _insensitive-case:

Insensitive case
================

Configuration
-------------

- Introduce the ``icase`` option name
    - Which is made persistent through the ``MODULES_ICASE`` environment variable
    - Many people asked for separate options as applying an icase approach to a module loading context is not seen desired by everybody whereas it is in an module search context
    - Defined levels of enablement are:
        - ``never``
        - ``search``
        - ``always``
    - icase option will be set by default to
        - ``never`` in v4 not to change existing behaviors
        - ``search`` in v5 as it seems to be a general improvement for everybody
    - A command-line switch ``--icase`` (short form ``-i``) is added
        - Was there in Modules 3 (for search sub-command only)
        - When set, equals to an ``always`` icase mode

- No immediate need for a ``--no-icase`` command-line switch
    - Combining configuration option and ``--icase`` command-line switch seems sufficient

Specification
-------------

- When enabled, match query string in a case insensitive manner
    - query *soFT* returns soft, SOFT, soFT, SOft, sOft and so on

- In case multiple files correspond to the same icase word
    - like *soft*, *soFT*, *SoFt*, *SOFT* filenames
    - query *SOFT* returns *SOFT* (exact match is returned among possibilities)
    - query *SoFt* returns *SoFt* (exact match is returned among possibilities)
    - query *SOft* returns *soft* (highest dictionarily-sorted match is returned among possibilities)
    - query *soFt* returns *soft* (highest dictionarily-sorted match is returned among possibilities)

- When icase is enabled for search context it applies to
    - module specification passed as argument in following context:
        - :ref:`module_version_specification_to_return_all_matching_modules`

- When icase is enabled for all context it applies to
    - search context like described above
    - module specification passed as argument in following contexts:
        - :ref:`module_version_specification_to_select_one_module`
        - :ref:`module_version_specification_to_check_compatibility`
    - module alias and symbolic version resolution triggered by
        - ``module-info alias``
        - ``module-info version``
        - ``getPathToModule``
        - ``isModuleEvaluated``
    - the gathering of all module alias and symbolic version targeting a given module
        - this is processed by ``getAllModuleResolvedName`` procedure
        - which is called by
            - ``doesLoadingModuleMatchesName``
            - ``cmdModuleLoad``

- Note that whatis specification passed as argument to the ``search`` sub-command is always matched in a case insensitive manner

Corner cases
------------

- When looking for the best match among loaded modules to select one module to unload, lastly loaded module, or firstly loaded module depending on ``unload_match_order`` configuration, will be returned
    - When insensitive case is enabled, last or first icase match will be returned even if an exact match is present among the loaded module list
    - This behavior has an impact in case multiple available modules correspond to the same insensitive case string
        - For instance *iCaSe* and *IcAsE* modules

- When ``icase`` is enabled on all contexts and multiple directories match same icase module name
    - for instance following modules exist: *ICASE/1.1*, *icase/1.2*, *iCaSe/1.3* and *iCaSe/1.4*
    - a ``module avail -i icase`` will sort *iCaSe/1.4* as the highest entry
    - however a ``module load -i icase`` command will load *icase/1.2*
        - as *icase* directory matches query
    - and also ``module load -i ICase`` command will load *icase/1.2*
        - as no directory *ICase* exists, result is returned from highest directory: *icase*

.. _advanced-module-version-specifiers:

Advanced module version specifiers
==================================

Configuration
-------------

- Introduce ``advanced_version_spec`` option name
    - off by default in v4 as previously *soft@1* could be a module name
    - on by default in v5
    - in case ``extended_default`` is disabled
        - means short-hand notation cannot be used
            - for soft/1.1 query soft@1 returns nothing
    - in case ``implicit_default`` is disabled
        - means a default should be found over version range or list in selection context

Specification
-------------

- Following Spack spec
    - see https://github.com/spack/spack/blob/develop/lib/spack/spack/spec.py
    - or https://spack.readthedocs.io/en/latest/basic_usage.html#version-specifier
    - this specs covers all needs to specify module versions finely
    - Spack users are already familiar with it
    - it copes very well with command-line typing, avoiding most problematic characters
        - that are interpreted by shells (like < or >)
    - specification for one module could
        - be almost condensed into one word "soft@1.8:"
        - or be expanded into multiple "soft @1.8:"

- same grammar used whatever the context
    - command-line or as argument to modulefile command (like command)

- versions are specified whether
    - as specific words (separated by " ")
    - or as suffix to module name

- change command specifications which were previously accepting list of modules
    - like *module1 module2/vers module3*
    - now these modules could express versions appended to their name with *@*
        - like *module1@1.8 module2@vers module3*
    - or these versions could be defined as words next to module name
        - like *module1@1.8 module2 @vers module3*
    - as a consequence, it denies use of *@* in module names
    - such change requires an option to be enabled to avoid breaking compat

- single version could be specified with soft@vers
    - which matches *soft/vers* modulefile

- version could be specified as range
    - *soft@:vers* or *soft@vers:* or *soft@vers1:vers2*
    - Tcl-dictionarily determine what is between specified range
    - version specified could be text, like if symbolic version names are used
    - should benefit from extended default specification
        - to just express version with their major release number for instance

- version could be specified as list
    - soft@vers,vers,vers
    - version specified could be text, like if symbolic version names are used
    - should benefit from extended default specification
        - to just express version with their major release number for instance
    - an empty string among list is considered as a specification error
        - for instance *soft@vers,vers,* or *soft@vers,,vers*

- when using extended default syntax
    - version selection is performed same way for *@vers* than for */vers*
    - described in extended default design

- when ``icase`` is enabled for selection context and multiple directories match module name
    - for instance query is *ICase@1.1,1.2,1.4* and following modules exist: *ICASE/1.1*, *icase/1.2*, *iCaSe/1.3* and *iCaSe/1.4*
    - as no *ICase* directory exists, and a version in highest directory *icase* matches query (*1.2*), *icase/1.2* is returned
    - if query is *iCaSe@1.1,1.2,1.4*, *iCaSe/1.4* will be selected as *iCaSe* directory matches query module name
    - if query is *ICase@1.1,1.4* or *icase@1.1,1.4*, as no version match in highest directory *iCaSe/1.4* will be selected

- in case of deep modulefiles
    - specified version is matched at the level directly under specified module name
        - not below levels
        - for instance soft@vers, will match *soft/vers*, not *soft/deep/vers*
        - to specify version for deep modules:*"soft/deep@vers*
        - or even: *soft@deep/vers*
            - as frontier between module name and version should not be imposed by module tool

- whatever the syntax, should keep in mind *soft* or *soft/vers* could be module aliases

- in case version is specified multiple times
    - lastly mentionned (read from left to right) value is retained (it overwrite previous values)
    - like *module@1.8 @2.0* or *module@1.8@2.0*
    - exception made for fully qualified modulefile where an @vers is added
    - like in *soft/1.8@1.10" or "soft/1.8 @1.10*
        this *@* version specifier should be ignored as it cannot overwrite qualified modulefile name

- in case modulefile is named *module@vers* in filesystem
    - it is not found when option ``advanced_version_spec`` is enabled
    - as it is translated to *module/vers*

- when special characters like *?* or \* are used in version name or value
    - they are treated literally, no wildcard meaning is applied
    - like currently done when specifying module version on command-line
        - which leads to errors as no corresponding module is found::

            $ module load loc_dv6/*
            ERROR: Unable to locate a modulefile for 'loc_dv6/*'

- if version range or list does not contain a defined default
    - in a selection context
        - highest version is returned if ``implicit_default`` is enabled
        - error returned if ``implicit_default`` is disabled
            - even if version range or list specifies non-existent modules and only one existent module
    - in a compatibility expression context
        - range or list is matched against loaded environment whether the ``implicit_default`` state
        - when no match found and evaluation are triggered, selection context applies

- when version is specified over an alias
    - should proceed like for real modulefile
    - when alias equal to a bare module with no version
        - foo is alias on bar modulefile, bar is a file (not a dir with version modulefiles)
        - query alias@:2 should behave like query alias/2
    - when alias equal to a module/version modulefile
        - foo is alias on bar/3 modulefile
        - query alias@:2 should behave like query alias/2

- Contexts where it could be used
    - :ref:`module_version_specification_to_return_all_matching_modules`
    - :ref:`module_version_specification_to_select_one_module`
    - :ref:`module_version_specification_to_check_compatibility`

- impact of advanced version specifier implementation over code
    - question especially over ``auto_handling`` code like conflict and prereq handling
    - it should not impact triggers and actions
    - but consist in an overall change of procedures comparing queries against loaded environment
        - procedures like ``doesModuleConflict``
    - also adapting ``getModules`` to restrict version possibilites to what has been specified
        - for instance with query *soft@1,2* should only return versions matching

- prereq/conflict persistency
    - ``LMPREREQ`` and ``LMCONFLICT`` content should reflect specified version constraint
    - it could be expressed in these variables somewhat like it is specified to the
        - prereq/conflict modulefile commands
        - for instance ``MODULES_LMPREREQ=soft/1.10&bar@1.8,1.10&foo@<2|foo@3<4``
        - delimiters characters are *:*, *&* and *|*
            - so use of characters * *, *@*, *,* is not an issue
            - but for *:* which express version ranges it should be substituted to *<*

- prereq/conflict specification
    - could consolidate different version set for same module on the same prereq/conflict list
        - to indicate a preferred order (if available)
        - like ``prereq foo@1.8 foo@1.10``
        - or ``prereq foo @1.8 foo@1.10``
    - also to trigger alternative requirement resolution in case first one failed
        - as each module version specification leads to one evaluation only
            - even if multiple modulefiles correspond to this specification
                - like ``prereq soft@1.8,1.9,1.10`` will lead to *soft/1.10* load
            - best candidate is chosen from matches
                - in case ``implicit_default`` is disabled an explicit default should be part of the list or range for the triggered evaluation to succeed
        - whereas ``prereq soft@1.8 soft@1.9 soft@1.10`` will lead to a tentative load
            - of *soft/1.8*, then *soft/1.9* if it failed then *soft/1.8* if it also failed

Corner cases
------------

- When ``icase`` is enabled on all contexts and multiple directories match same icase module name
    - for instance following modules exist: *ICASE/1.1*, *icase/1.2*, *iCaSe/1.3* and *iCaSe/1.4*
    - a ``module avail -i icase`` will sort *iCaSe/1.4* as the highest entry
    - however a ``module load -i icase@1.1,1.2,1.4`` command will load *icase/1.2*
        - as *icase* directory matches query and version *1.2* is found in *icase* directory
    - but a ``module load -i icase@1.1,1.4`` command will load *iCaSe/1.4*
        - as no version *1.1* nor *1.4* is found in *icase* directory

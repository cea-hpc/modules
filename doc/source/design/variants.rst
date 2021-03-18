.. _variants:

Variants
========

Configuration
-------------

- Variant mechanism requires the :mconfig:`advanced_version_spec` option to be enabled

    - Variants can be seen as an extension of the advanced module version specifiers


Specification
-------------

Defining
^^^^^^^^

- variants are defined for modulefiles within concerned modulefiles with the :mfcmd:`variant` modulefile command

    - variants are bound to the modulefile their target
    - especially modulefile evaluation may be guided by the value selected for the variants
    - so it makes more sense to have variants defined within modulefiles rather in modulercs

- ``variant --default? defaultval? --multivalued? --alias? {aliasname...?} name value ...?``

- ``name`` restrained to the accepted variable name

    - which should correspond to a sequence of one or more characters that are a letter, digit or underscore
    - if we want to match Spack here also: ``[A-Za-z0-9_][A-Za-z0-9_.-]*``
    - raise error otherwise

- a variant ``name`` that has already been defined as variant or variant alias for this modulefile evaluation overwrites previous definition

    - it is up to modulefile author to ensure there is no duplicate
    - no need this way to check in ``modulecmd.tcl`` code if variant has already been defined

- may have a default value (when ``--default`` argument is passed)

    - if not, variant is undefined
    - argument ``--default`` should be associated a *defaultval* argument

        - raise error otherwise

    - default value should be part of possible value list

        - error is raised otherwise when default value is used, as it does not correspond to an accepted value

            - ex: ``variant name --default 1.8 1.0 2.0``

- may be multi-valued (when ``--multivalued`` argument is passed)

    - if so a list is set with selected values

- must have a list of accepted values

    - passed value(s) must correspond to those accepted

        - raise error otherwise
        - raised error is a global error to signal specification issue, not a modulefile error

- may be aliased (when argument ``--alias`` is passed) in which case:

    - argument ``--alias`` should be associated one or a list of alias names

        - raise error otherwise

    - defined alias names should be valid variant name (i.e.: ``[A-Za-z0-9_][A-Za-z0-9_.-]*``)

        - raise error otherwise

    - should not be already defined as variant or variant alias

        -  raise error otherwise

    - variant alias could negate its target (when alias name is prefixed by ``-``)

        - meaning if alias variant is set to ``false`` its target is set to ``true``
        - only possible if targeted variant is Boolean

            - raise error otherwise

        - no support of ``~`` as an alternative negated prefix to only get one way to write negation

- require a value to be set

    - raise error if no value set for variant (no value specified and no defined default)

- to validate variant value is coherent against other variant values or any other test

    - it is up to the modulefile author to write the appropriate test after all variant declaration
    - and then raise error if variant combination is not appropriate
    - as there is no way to automatically determine when all variants are declared


Evaluating
^^^^^^^^^^

- when reaching ``variant`` command in modulefile

- set a key ``name`` in array ``ModuleVariant`` if variant ``name`` has been specified or if it owns a default value

    - variable ``ModuleVariant($name)`` is not defined if variant not specified and no default for it

        - error is raised if used in this case

- if variant ``name`` can be aliased, if alias is set it specifies the value of variant ``name``

    - evaluated from left to right, in case variant is set and also its aliases
    - negating aliases sets its reversed value to Boolean variant

- variant alias is not instantiated in ``ModuleVariant`` array

    - therefore accessing ``$ModuleVariant($aliasname)`` will raise an error

- raise error if variant ``name``:

    - has been specified but passed value is incorrect
    - is single-valued but multiple values are passed
    - is wrongly declared in modulefile

- then variable ``$ModuleVariant(name)`` could be used to adapt modulefile evaluation

- to know all variant currently set, use ``[array names ModuleVariant]``

    - but beware that only the variant already evaluated will be set in array
    - must use after all the ``variant`` to be set in the modulefile to accurately get all variant defined

- if variant accepted values are Booleans, variable could be directly used in conditional tests

    - like ``if {$ModuleVariant($name)} {...``

- quoting Tcl doc, Booleans could have many forms:

    - If string is any of *0*, *false*, *no*, or *off*, then Tcl_GetBoolean stores a zero value at ``*boolPtr``.
    - If string is any of *1*, *true*, *yes*, or *on*, then 1 is stored at ``*boolPtr``.
    - Any of these values may be abbreviated, and upper-case spellings are also acceptable

- variants with a :ref:`shortcut <variant-shortcut>` defined for them, are resolved to their full name and transmitted this way to the modulefile

    - therefore no entry in ``ModuleVariant`` array is made for the shortcut name

- special variant ``version`` is instanciated as variable ``ModuleVariant(version)`` if declared

    - will be set to value that may be specified with the ``@`` shortcut
    - will initially work as any other variant (accepted list of value, default value)

        - using ``variant version --default 1.8 1.8 1.10``
        - is equivalent of having two modulefiles *mod/1.8* and *mod/1.10*
        - with a default defined: ``module-version mod/1.8 default``

    - FUTURE: could be refined later on to accept range then select latest version or defined default if in range
    - note that ``ModuleVariant(version)`` is set only if a ``version`` variant is declared in modulefile
    - querying ``[module-info name]`` returns modulefile designation appended by the version variant: ``@versionvalue``

- at the end of the modulefile evaluation, if module specification contains a variant which is not defined in modulefile

    - an error is raised

- for the different evaluation modes:

    - *load*, *display*, *help* and *test* applies evaluation mechanism described above

        - for the *display*, *help* and *test* modes it helps to see how the modulefile reacts to the different variant value
        - on *display* mode, the ``variant`` command is reported in addition of its evaluation

    - *unload* evaluates the mechanism described above but the specified variants are fetched from loaded module persistent information

        - ``version`` variant value is fetched from the module specification to identify the module to unload
        - other variant specification on the unload command are dropped once matching loaded module is identified
        - which means the specification recorded corresponds to the one from the loaded module

            - as a consequence ``[module-info specified]`` returns the loaded module specification and not the specification defined on the ``module unload`` command

        - variant values are defined within modulefile context by the evaluation of the ``variant`` modulefile commands

            - like it is done during a *load* modulefile evaluation
            - this way variables related to variant are instantiated the same way whether module is loading or unloading
            - so it is easier for modulefile author to understand how the modulefile code is evaluated

        - variant evaluated during modulefile unload may produce an error

            - if variant value not found defined or if value recorded in persistency does not match an allowed value
            - should encourage variants to be consistent between the load and unload evaluation phases

    - *whatis* ignores all variants from the module specification (only the module names are retained)

        - like for ``setenv`` or ``*-path`` modulefile commands on this mode, ``variant`` will set the ``ModuleVariant`` array with a empty string for each variant defined in the modulefile
        - this is done to avoid the *undefined variable* error if these variables are used later in the modulefile
        - FUTURE: if the different ``version`` variant values are considered as different modulefiles in the future, then *whatis* may evaluates the ``version`` variant from module specification


Persistency
^^^^^^^^^^^

- once a module is loaded its defined variants are made persistent in user environment

    - to keep track of how loaded modules were evaluated
    - and enable query commands on these loaded modules without need to reevaluate modulefile

- variants defined are made persistent in :envvar:`MODULES_LMVARIANT` environment variable

    - following same approach than for :envvar:`MODULES_LMPREREQ`
    - each loaded module with defined variants (default value or specifically set) will expose these variants value and if the value is the default one in a record with following syntax:

        - ``loadedmodule&(+|-)boolvariantname1|isdefaultvalue&variantname2|value2|value3...|isdefaultvalue``

    - for each variant it is recorded if the value set corresponds to the variant default value or not

        - such information is useful to save collection when pin version mechanism is disabled
        - on such setup the variant definition should not recorded in collection if this is the default value which is set

    - each *loadedmodule* record are joined in ``MODULES_LMVARIANT`` separated by ``:`` character

- variant alias are also made persistent in :envvar:`MODULES_LMVARIANTALTNAME` environment variable

    - each loaded module with defined variants (default value or specifically set) which could be aliased will expose their aliases in a record with following syntax

        - ``loadedmodule&variantname1|aliasname1|-aliasname2&variant2|aliasname3...``

    - each loadedmodule record are joined in ``MODULES_LMVARIANTALTNAME`` separated by ``:`` character

- Boolean variants are stored in the form ``+name`` or ``-name``

    - which enables to determine this variant is of Boolean type
    - and check against query using different Boolean representations

        - like ``serial=0``, ``serial=on``, ``serial=false``, etc.

- when the special variant ``version`` is defined for a loaded module

    - the value of this variant is part of loaded module identification
    - ``@versionvalue`` is appended to the module name, for instance ``mod@1.2``
    - such identification is then recorded in persistency variables to designate loaded module (like ``LOADEDMODULES``, ``MODULES_LMPREREQ``, ``MODULES_LMVARIANT``, etc)
    - this way in case a modulefile allows the load of two of its versions in the user environment, it is possible to distinguish these two loaded versions (to unload one of them for instance)
    - with this identification, it is possible to distinguish a traditional module (identified by ``mod/version``) from a module using ``version`` variant (identified by ``mod@version``)
    - note that if a modulefile ``mod/1.2`` defines a ``version`` variant, it will be identified as ``mod/1.2@versionvalue``

        - so the ``version`` variant should not be defined if each version of the module has its own modulefile
        - ``version`` variant is useful if a single modulefile is used to instantiate every version of the module

    - FUTURE: when it will be possible to override the shortcut for ``version`` variant it will be important to identify version value in loaded module identification string
      with a designation that is not dependent of the selected shortcut

- loaded module identification stops at the module name and ``version`` variant (if defined)

    - other variants are not considered as part of the identification
    - as it is not foreseen useful to have the same module loaded multiple times with different variant values, unless for ``version`` variant


Specifying
^^^^^^^^^^

- following Spack spec

    - see https://github.com/spack/spack/blob/develop/lib/spack/spack/spec.py
    - or https://spack.readthedocs.io/en/latest/basic_usage.html#variants
    - this specs covers all needs to specify variant on Modules
    - Spack users are already familiar with it,
    - it copes very well with command-line typing, avoiding most problematic characters

        - that are interpreted by shells (like < or >)

    - specification for one module could

        - be almost condensed into one word *hdf5@1.8+debug*
        - or be expanded into multiple *hdf5 @1.8 +debug*

- same grammar used whatever the context

    - command-line or as argument to modulefile command (like command)

- variants are specified whether

    - as specific words (separated by " ")
    - or as suffix to module name

- change command specifications which were previously accepting list of modules

    - like *module1 module2 module3*
    - now these modules could express variants appended to their name

        - like *module1@1.8+debug module2~shared module3*

    - or these variants could be defined as words next to module name

        - like *module1@1.8 +debug module2 shared=false module3*

    - as a consequence it denies:

        - use of *+*, *~* and *=* in module names
        - and use of *-* as first character of module names

    - also a command-line argument starting with the *-* character is not anymore considered as an invalid option

        - it is considered as an element of the module specification (potential negated boolean variant)
        - unless if set prior the sub-command designation

    - such change requires an option to be enabled to avoid breaking compat

        - this is why to enable variant, the ``advanced_version_spec`` option has to be enabled

- a valued-variant is specified by *name=value*

    - this kind of variant cannot be directly appended to module name
    - thus it must be specified as a separate word
    - in case variant can be multi-valued, a ``,`` character separates each value to set

- a Boolean variant can be specified with its bare name prefixed by *+*, *-* or *~*

    - when directly appended to module name string (no space) only *+* and *~* are recognized

        - *-* in this case is retained as part of previous name/value

    - the negation prefix *-* is not supported on the :command:`ml` command

        - as this *-* prefix means to unload a module on this command

    - negation prefix plus Boolean variant name should not equal a command-line option short form

        - command-line option takes precedence
        - for instance the ``-t`` will always be treated as ``--terse`` and not the negation of a ``t`` variant

    - beware that the negation prefix *~* when used as a separate word may trigger the tilda resolution of the currently running shell

        - if a username matches a Boolean variant name, using the ``~name`` form on the shell command-line will leads to the resolution of the HOME directory path of user ``name``

    - Boolean variant could also be specified using the *name=value* form

        - in which case, it should be written as a separate word
        - value could be any syntax recognized as a true or false string

            - false: *0*, *false*, *no*, or *off*
            - true: *1*, *true*, *yes*, or *on*
            - Any of these values may be abbreviated, and upper-case spellings are also acceptable.

- variant may be specified with a shortcut if any set (see :ref:`variant-shortcut`)

    - a shortcut is appended to the module designation word or specified as separate word, combined or not with other variant

        - for instance for the ``@`` shortcut: *module@versspec+boolvar*, *module+boolvar@versspec*, *module +boolvar@versspec*

    - even if a shortcut is set, the variant could also be expressed as valued variant name

- in case variant is specified multiple times

    - lastly mentioned (read from left to right) value is retained (it overwrites previous values)

        - a *merge all passed values in list* is not the methodology retained here

            - to offer only one way to specify multiple values for a variant
            - same handling way whatever the variant properties
            - as prior modulefile evaluation, variant properties are unknown (single or multi valued?)

    - like *name=value1 name=value2*

        - or *name=value name=value*
        - or *name=value1,value2 name=value3*
        - or *name=value1 name=value2,value3*
        - or *@vers1 version=vers2*
        - or for boolean *+name~name*
        - or *~name -name*
        - or *~name name=value1 name=value2,value3*
        - or in case of variant aliases *+name~aliastoname*

    - at the specification time variant aliases are not known

        - so the full module specification has to be transmitted toward the modulefile to determine what is the value at the most right position
        - for instance *name=value1 aliasname=value2* with *aliasname* being an alias of *name*
        - specification can just be cleared from the obvious duplicates (same variant name defined multiple times on the line)

- when special characters like *?* or \* are used in variant name or value

    - they are treated literally, no wildcard meaning is applied
    - like currently done when specifying module version on command-line

        - which leads to errors as no corresponding module is found::

            $ module load loc_dv6/*
            ERROR: Unable to locate a modulefile for 'loc_dv6/*'

- when a variant is specified but it does not correspond to a variant defined in the evaluated modulefile

    - an error is raised at the end of modulefile evaluation
    - need to wait for the end of modulefile evaluation to be sure the variant is defined nowhere in modulefile code

- special variant ``version`` has to be specified with ``@`` shortcut or by its full variant name (``version=value``)

    - traditional separator character ``/`` cannot be used to specify variant version
    - if used, a modulefile named ``mod/version`` will be searched and a *module not found* error will be raised

- specification may be passed to commands to verify a given module and variant combination is loaded

    - which should be performed without evaluating modulefiles
    - like for ``is-loaded`` sub-command:

        - ``module is-loaded hdf5+parallel``
        - or ``hdf5@1.8 parallel=true``
        - or ``hdf5 -serial``
        - or ``hdf5 serial=0``

    - checks rely on the content of the ``MODULES_LMVARIANT`` and ``MODULES_LMVARIANTALTNAME`` variables

        - which store variants set for loaded modules and eventual variant aliases of variant set

    - with this information it is possible to compare query against what is loaded

        - a variant specified on query which is not part of the variables means a different module/variant combination

            - even if variant from query is not valid for module, which cannot be known

    - verification mechanism of a sub-command like ``is-loaded`` should be preserved

        - which means a query not mentioning a specific value for a variant should match a loaded module which specify a variant value that differs from this variant default

    - the module identification part in specification may be resolved from a symbolic version or a module alias to an actual modulefile

- the ``@loaded`` specification is translated into the name, version and variant list of corresponding loaded module

    - for instance ``mod/3.0 foo=val1`` is loaded so ``mod@loaded`` is translated into ``mod/3.0 foo=val1``
    - in case the ``@loaded`` specification is followed by variant specified, those variant specifications are ignored
    - following the above example, ``mod@loaded foo=val2`` is translated into ``mod/3.0 foo=val1``


.. _variant-shortcut:

Variant shortcut
""""""""""""""""

- shortcuts can be set to abbreviate variant names and simplify their specification

    - a shortcut abbreviates ``name=`` into a unique character
    - when using shortcut, variant value is specified as ``<shortcut>value``
    - for instance, if the ``%`` is set as the shortcut for a ``toolchain`` variant, value ``foss21a`` is specified as ``%foss21a``

- shortcut can be set through the :mconfig:`variant_shortcut` configuration option

    - this option holds a colon separated list of shortcut definitions
    - each definition have the following form: ``variantname=shortcut_character``
    - for instance: ``toolchain=%:foo=^``

- shortcut must be:

    - a single character
    - excluding characters already used for other concerns or in module names (*-*, *+*, *~*, */*, *@*, *=*, *[a-zA-Z0-9]*)
    - when set through ``config`` sub-command or ``--with-variant-shortcut`` installation option: an error is raised when a shortcut is badly specified
    - if a badly specified shortcut ends up in modulecmd configuration, it is purely ignored

- shortcut does not apply to Boolean variants

    - as shortcuts are intended to be a prefix, they cannot combine with *-*, *+* or *~* Boolean prefixes

- by default, when ``advanced_version_spec`` is enabled, the ``@`` character is set as a shortcut for the ``version`` variant

    - this shortcut is not referred in ``MODULES_VARIANT_SHORTCUT`` thus it cannot be unset
    - FUTURE: superseding of this ``@`` shortcut in ``MODULES_VARIANT_SHORTCUT`` may be introduced in the future

        - but currently entries in ``MODULES_VARIANT_SHORTCUT`` for ``version`` variant are ignored

- variant shortcuts could be used on the command-line or within modulefiles even if it is not recommended to use them in the latter case

    - as if user updates the ``variant_shortcut`` configuration option, it will broke underlying modulefiles using a de-configured shortcuts


Reporting
^^^^^^^^^

- Defined variants are reported on ``list`` sub-command results

    - Reported joined to modulefile name, within curly braces
    - Each variant definition separated by a colon
    - For instance ``module/version{vr=val:+boolvr}``

- Variants are reported by default on ``list`` sub-command

    - as they qualify what exact flavor of the module is loaded
    - so it enables users to really catch what has been loaded exactly
    - They can be removed from output using the :ref:`output configuration<output-configuration>` mechanism

- Variants defined by modulefiles are not reported currently on ``avail`` sub-command as it requires to evaluate the modulefiles

    - FUTURE: this could be implemented later on, but such evaluation of all modulefiles would be really expensive
    - it will take a lot more time to get ``avail`` results (unless a valid cache is found)

- A specific color key is defined to highlight variants: ``va``

- Variant report depends on variant type

    - valued variant: ``variant=value``
    - boolean variant: ``+variant`` or ``-variant``
    - valued variant with shortcut set: ``<shortcut>value`` (for instance if ``%`` is a defined shortcut: ``%value``)

- Variant aliases are not reported

    - to keep output tight

- Special variant ``version`` is reported right after the module name

    - with ``@`` shortcut
    - using variant highlight color if any
    - for instance: ``module@version{othervariant=value}``

- Variants are reported on the *Loading*, *Unloading* and *Switching* informational messages

    - As they qualify what exact flavor of the module is loaded, unloaded or switched
    - They are put along the module name and version designation
    - They are reported using their short form, like for ``list`` sub-command to keep output tight
    - Separated between each other by space character
    - Each variant specification is enclosed between single quotes if it contains a space character
    - The whole variant specification is enclosed between curly braces (``{}``) and separated from module name version by space character

        - Easier this way to distinguish variant specification from module name version on informational messages where multiple module designation are reported

    - These designations have to be recorded

        - prior module evaluation and based on variant specification (passed as argument) in order to be ready for any report prior the end of modulefile evaluation (in case of error for instance)
        - then refined after module evaluation with the variant accurately set in loaded environment

- Variants are also reported along module name and version in the :mconfig:`auto_handling` informational messages


Recording collection
^^^^^^^^^^^^^^^^^^^^

- The variant specification set for loaded modules has to be recorded when saving it in a collection

    - Restoring such environment should apply the variant specified on the module to load
    - Lines like the following one could now be found in collection: ``module load hdf5@1.10 +parallel``
    - Important to distinguish multiple modules specified on a single line from a module specified with variant defined

- In case the :mconfig:`collection_pin_version` configuration option is disabled variant set to their default value should not be recorded in collection

    - Following the same scheme than for module version
    - When saving collection, the *is-default*value* information stored in persistency variable (``MODULES_LMVARIANT``) helps to know whether to value set to a variant is or not the default one
    - The save mechanism will rely on this information to exclude or not the variant specification in the generated collection output

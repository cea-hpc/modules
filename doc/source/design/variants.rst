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

  - variants are bound to the modulefile they target
  - especially modulefile evaluation may be guided by the value selected for the variants
  - so it makes more sense to have variants defined within modulefiles rather in modulercs

- ``variant --default? defaultval? --alias? {aliasname...?} name value? ...?``

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

  - default value should be part of possible value list if value list provided

    - error is raised otherwise when default value is used, as it does not correspond to an accepted value

      - ex: ``variant name --default 1.8 1.0 2.0``

    - when no list of accepted value is provided, default value could be any value

- may be set as a Boolean variant (when ``--boolean`` argument is passed)

  - a Boolean variant may equal *true* or *false*

- may have a list of accepted values

  - unless if defined as a Boolean variant

  - passed value(s) must correspond to those accepted

    - raise error otherwise
    - raised error is a global error to signal specification issue, not a modulefile error

  - an error is raised if one or more accepted values are defined on a Boolean variant

  - non-Boolean variants cannot define Boolean value in their accepted value list

    - as Boolean values are translated to their canonical form (*0* or *1*) when specified
    - an error is raised otherwise
    - exception made for the *0* and *1* integers

  - when no list of accepted value is provided it means any value is accepted

    - it is up to the modulefile author to write the appropriate test after variant declaration to check that the value provided is acceptable

- may be aliased (when argument ``--alias`` is passed) in which case:

  - argument ``--alias`` should be associated one or a list of alias names

    - raise error otherwise

  - defined alias names should be valid variant name (i.e.: ``[A-Za-z0-9_][A-Za-z0-9_.-]*``)

    - raise error otherwise

  - should not be already defined as variant or variant alias

    - raise error otherwise

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

- as special variant ``version`` will not be implemented on Modules 4.8, an error is raised if a variant is named ``version``

  - to ensure no usage of this variant name will be made before its special mechanism availability

- special variant ``version`` is instantiated as variable ``ModuleVariant(version)`` if declared

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

  - *load*, *help* and *test* apply evaluation mechanism described above

    - for the *help* and *test* modes it helps to see how the modulefile reacts to the different variant value

  - *display* applies evaluation mechanism described above but allow variant to be unspecified

    - to help learning all the variant defined by the modulefile
    - as a result unspecified variant is not instantiated in the ``ModuleVariant`` array variable
    - with unspecified variant a user will not accurately see how the modulefile reacts to the different variant value

      - evaluation error is obtained if accessing the ``ModuleVariant`` array variable for an unspecified variant

    - on *display* mode, the ``variant`` command is reported in addition of its evaluation

  - *unload* evaluates the mechanism described above but the specified variants are fetched from loaded module persistent information

    - ``version`` variant value is fetched from the module specification to identify the module to unload
    - other variant specification on the unload command are dropped once matching loaded module is identified

      - however this specification remains available when querying ``[module-info specified]``

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
    - however variant specified in module specification is used to filter modules to evaluate in *whatis* mode thanks to the *extra match search* mechanism
    - FUTURE: if the different ``version`` variant values are considered as different modulefiles in the future, then *whatis* may evaluates the ``version`` variant from module specification

getvariant
""""""""""

- A :mfcmd:`getvariant` modulefile command is added following the same principle than :mfcmd:`getenv`

  - A variant name is passed as argument
  - Corresponding variant value is returned if it is defined
  - If not defined the *value if not defined* argument is returned
  - By default *value if not defined* argument equals to the empty string

- ``getvariant`` should be preferred to accessing variant value in ``ModuleVariant`` array

- On display evaluation mode, ``getvariant`` returns the variant name enclosed in curly braces

  - Which helps to report where the variant is used in environment variable content
  - The variant name and enclosing curly braces are graphically enhanced with ``va`` SGR code


Persistency
^^^^^^^^^^^

- once a module is loaded its defined variants are made persistent in user environment

  - to keep track of how loaded modules were evaluated
  - and enable query commands on these loaded modules without need to reevaluate modulefile

- variants defined are made persistent in :envvar:`__MODULES_LMVARIANT` environment variable

  - following same approach than for :envvar:`__MODULES_LMPREREQ`
  - each loaded module with defined variants (default value or specifically set) will expose:

    - these variants value
    - and if the value is the default one and if this default was specifically asked
    - in a record with following syntax:
    - ``loadedmodule&boolvariantname1|isbooleanvariant|isdefaultvalue&variantname2|value2|value3...|isbooleanvariant|isdefaultvalue``

  - for each variant it is recorded if the variant is a Boolean variant

    - which enables to compare value in a Boolean way
    - and to report variant value with the *+variant* or *-variant* syntax

  - for each variant it is recorded if the value set corresponds to the variant default value or not

    - such information is useful to save collection when pin version mechanism is disabled
    - on such setup the variant definition should not recorded in collection if this is the default value which is set

  - in addition to know if the variant value is default or not, it is recorded if the default value was:

    - specifically asked (*isdefaultvalue=1*)
    - or automatically set (*isdefaultvalue=2*)
    - this information will be useful in the FUTURE to determine if a variant may be automatically swapped by another

  - each *loadedmodule* record are joined in ``__MODULES_LMVARIANT`` separated by ``:`` character

- variant alias are also made persistent in :envvar:`__MODULES_LMVARIANTALTNAME` environment variable

  - each loaded module with defined variants (default value or specifically set) which could be aliased will expose their aliases in a record with following syntax

    - ``loadedmodule&variantname1|aliasname1|-aliasname2&variant2|aliasname3...``

  - each loadedmodule record are joined in ``__MODULES_LMVARIANTALTNAME`` separated by ``:`` character

- when persistency information is corrupted

  - a missing or non Boolean ``isdefaultvalue`` information means variant value is not the default
  - a missing or non Boolean ``isbooleanvariant`` information means variant is not a Boolean variant
  - a non-Boolean value set on a Boolean variant means variant equals *0*

- Boolean variants are stored in the form ``+name`` or ``-name``

  - which enables to determine this variant is of Boolean type
  - and check against query using different Boolean representations

    - like ``serial=0``, ``serial=on``, ``serial=false``, etc.

- when the special variant ``version`` is defined for a loaded module

  - the value of this variant is part of loaded module identification
  - ``@versionvalue`` is appended to the module name, for instance ``mod@1.2``
  - such identification is then recorded in persistency variables to designate loaded module (like ``LOADEDMODULES``, ``__MODULES_LMPREREQ``, ``__MODULES_LMVARIANT``, etc)
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
    - or set on sub-commands that do not accept module specification as argument

  - such change requires an option to be enabled to avoid breaking compat

    - this is why to enable variant, the ``advanced_version_spec`` option has to be enabled

- a valued-variant is specified by *name=value*

  - this kind of variant cannot be directly appended to module name
  - thus it must be specified as a separate word

- a Boolean variant can be specified with its bare name prefixed by *+*, *-* or *~*

  - when directly appended to module name string (no space) only *+* and *~* are recognized

    - *-* in this case is retained as part of previous name/value

  - the negation prefix *-* is not supported on the :command:`ml` command

    - as this *-* prefix means to unload a module on this command

  - negation prefix plus Boolean variant name should not equal a command-line option short form

    - command-line option takes precedence
    - for instance the ``-t`` will always be treated as ``--terse`` and not the negation of a ``t`` variant

  - beware that the negation prefix *~* when used as a separate word may trigger the tilde resolution of the currently running shell

    - if a username matches a Boolean variant name, using the ``~name`` form on the shell command-line will leads to the resolution of the HOME directory path of user ``name``

  - module name could end with one or more *+* characters

    - it could be distinguished from a Boolean variant specification as no character should follow these trailing *+*

  - Boolean variant could also be specified using the *name=value* form

    - in which case, it should be written as a separate word
    - value could be any syntax recognized as a true or false string

      - false: *0*, *false*, *no*, or *off*
      - true: *1*, *true*, *yes*, or *on*
      - Any of these values may be abbreviated, and upper-case spellings are also acceptable.

  - when specified Boolean value is translated to its canonical form (*0* or *1*)

- variant may be specified with a shortcut if any set (see :ref:`variant-shortcut`)

  - a shortcut is appended to the module designation word or specified as separate word, combined or not with other variant

    - for instance for the ``@`` shortcut: *module@versspec+boolvar*, *module+boolvar@versspec*, *module +boolvar@versspec*

  - even if a shortcut is set, the variant could also be expressed as valued variant name

- in case variant is specified multiple times

  - on selection context, lastly mentioned (read from left to right) value is retained (it overwrites previous values)

    - a *merge all passed values in list* is not the methodology retained here

      - same handling way whatever the variant properties

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

  - on search context, all specifications form an *AND* operation

    - *name=value1 name=value2* means find modules defining *value1* and *value2* as available values for *name* variant

- on search context, several values may be set on one variant criterion

  - like *name=value1,value2*
  - which means find modules defining either *value1* or *value2* as available value for *name* variant
  - an error is obtained if performed on selection context

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

  - checks rely on the content of the ``__MODULES_LMVARIANT`` and ``__MODULES_LMVARIANTALTNAME`` variables

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

- variant can also be specified to designate module on :mfcmd:`module-tag`, :mfcmd:`module-hide` or :mfcmd:`module-forbid` commands

  - tags may only apply when a given variant of a module is loaded
  - it may be useful to decommission a given variant of a module prior others
  - or to forbid the use of a given variant to some users

- variants specified on search context are taken into account to filter results

  - due to the :ref:`extra-match-search` mechanism
  - it concerns the ``avail``, ``whatis`` and ``paths`` sub-commands
  - only available modules matching the variant specification will be retained
  - for instance ``avail mod foo=var`` returns all versions of *mod* module where a variant *foo* is defined with *var* as a possible value
  - exception is made for ``is-avail`` and ``path`` search sub-command

    - as they are more module selection commands rather an available module search commands
    - it does not take info account variants defined within module specification

  - these search commands (except ``is-avail`` and ``path``) allow module specification without module name and version

    - only variant name and value specified (e.g., ``module avail foo=var``)
    - only modules declaring such variant with such version will be retained in result

- variants specified on loaded module list context are taken into account to filter results

  - concerns ``list`` sub-command
  - not related to *extra match search* as comparison is made against loaded modules not content of available modules
  - only loaded modules matching the variant specification will be retained
  - module specification without module name and version is allowed on this context

    - only variant name and value specified (e.g., ``module list foo=var``)
    - only loaded modules declaring such variant set with this version will be retained in result

- variant cannot be specified over the :mfcmd:`module-alias`, :mfcmd:`module-version`, :mfcmd:`module-virtual` commands

- variant passed as argument to :mfcmd:`module-info` ``alias``, ``version`` or ``symbols`` will not match anything

- ``module-info loaded`` only accepts modulefile as argument, not variant specification

  - it also only return loaded module name and version, without the variant set


Variant in requirement specification
""""""""""""""""""""""""""""""""""""

- :mfcmd:`prereq`/:mfcmd:`conflict` specification

  - could consolidate different variation set for same module on the same prereq/conflict list

    - to indicate a preferred order (if available)
    - like ``prereq netcdf -debug netcdf +debug``
    - or ``prereq netcdf var=val1 netcdf var=val2 netcdf``

  - in last example, could not currently consolidate definition into ``prereq netcdf var=val1,val2,default``

    - in case of requirement alternatives, all possibilities should be written as independent

      - like ``prereq module@vers variant=val1 module@vers variant=val2``

    - to clearly indicate a priority order to apply when for instance attempting to load these requirements
    - FUTURE: a value selection mechanism, like when selecting a module version among others, would help here

- prereq/conflict persistency

  - :envvar:`__MODULES_LMPREREQ` and :envvar:`__MODULES_LMCONFLICT` content should reflect specified variant constraint

  - it could be expressed in these variables as it is specified over the prereq/conflict modulefile commands

    - for instance ``__MODULES_LMPREREQ=hdf5/1.10&mpi@1.8 +shared variant=name&netcdf``
    - use of characters `` ``, ``+``, ``~``, ``,`` is not an issue

      - as delimiters characters in these environment variables are ``:``, ``&`` and ``|``


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

  - as shortcuts are intended to be a prefix, they do not add benefit compared to *-*, *+* or *~* Boolean prefixes
  - however a shortcut could be defined on a boolean variant (e.g., ``%true`` or ``%0``)

- by default, when ``advanced_version_spec`` is enabled, the ``@`` character is set as a shortcut for the ``version`` variant

  - this shortcut is not referred in ``MODULES_VARIANT_SHORTCUT`` thus it cannot be unset
  - FUTURE: superseding of this ``@`` shortcut in ``MODULES_VARIANT_SHORTCUT`` may be introduced in the future

    - but currently entries in ``MODULES_VARIANT_SHORTCUT`` for ``version`` variant are ignored

- variant shortcuts could be used on the command-line or within modulefiles even if it is not recommended to use them in the latter case

  - as if user updates the ``variant_shortcut`` configuration option, it will broke underlying modulefiles using a de-configured shortcuts

- module designation in collection does not use variant shortcuts

  - when shortcut configuration is changed it should not impact collection definition


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

- Variants defined by modulefiles may be reported on ``avail`` sub-command

  - Not by default, if ``variant`` value is added to ``avail_output`` or ``avail_terse_output``
  - Or if ``variantifspec`` value is added to ``avail_output`` or
    ``avail_terse_output`` and a variant is specified in search query
  - It requires an :ref:`extra-match-search`
  - Which in turns requires more processing time as modulefiles are evaluated
  - Having cache file available helps to reduce the I/O load caused by such process

- A specific color key is defined to highlight variants: ``va``

- Variant report depends on variant type

  - valued variant: ``variant=value``
  - boolean variant: ``+variant`` or ``-variant``
  - valued variant with shortcut set: ``<shortcut>value`` (for instance if ``%`` is a defined shortcut: ``%value``)
  - in case a shortcut is defined over a Boolean variant, Boolean report prevails over shortcut

- On ``avail``, all possible values of variant are reported

  - Values are separated from each other with ``,`` character
  - Boolean variant are reported as valued variant with ``on`` and ``off`` values reported
  - Example: ``mod/1.0{variant=val1,val2,boolvariant=on,off:%val1,val2}``
  - A ``*`` value is reported in possible value list for free-value variant

    - To indicate all values are possible
    - Loaded variant value and default variant value are also reported if any

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
  - When saving collection, the *is-default-value* information stored in persistency variable (``__MODULES_LMVARIANT``) helps to know whether the value set to a variant is or not the default one
  - The save mechanism will rely on this information to exclude or not the variant specification in the generated collection output
  - Within this *is-default-value* hint, the *was-this-default-specified-by-user* sub-information is not preserved when saving collection

    - if collection is not pinned, default value is excluded whether it was specifically set by user or not


Comparing module specification including variants
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- When a module specification has to be compared on a non-loaded or non-loading modules context

  - If this specification contains variants

    - Applies especially to the search commands taking a module specification as argument
    - Specified variants are taken into account through :ref:`extra-match-search` mechanism

      - Once matching name and version are found
      - A *scan* evaluation is made on them
      - Only those declaring specified variants and values are retained in results

    - It applies to ``avail``, ``whatis`` and ``paths`` sub-commands
    - Exception is made for ``is-avail`` and ``path`` sub-commands

      - do not trigger *extra match search* process
      - even if variant specified in module specification argument

  - If this specification does not contain variant

    - There is no variant set on non-loaded or non-loading modules we are comparing to
    - Match is performed over module name and version

- When a module specification has to be compared against loaded or loading modules

  - If this specification contains variants

    - It should be matched against the variants set on loaded or loading modules
    - No variant set for loaded or loading module means no match

  - If this specification does not contain variant

    - Loaded or loading modules match is only made on their name
    - No comparison occurs over the variants set on loaded or loading modules

- To compare variant set on loaded or loading modules

  - A ``ismodlo`` flag is added to the ``modEq`` procedure
  - With this flag it is known if ``modEq`` operates on a:

    - non-loaded or non-loading context (0),
    - loading context (1)
    - loaded context (2)

  - Variants set on loading or loaded modules will be fetched in case ``ismodlo`` is equal to 1 or 2
  - Loaded or loading modules are passed to ``modEq`` by their name/version designation

    - No change here
    - And no change required in all procedures that perform such comparison

  - Alternative names should also be tested like main module name with variants set

    - As the alternative names currently apply to module name and version only
    - Name and version could be compared on their own
    - Then variants could be compared
    - Which means all applying names are compared then if a match is found variants are compared

- Specific comparison occurs when restoring collection

  - When a collection is restored an unspecified variant for a given module to load means this variant has to be set at its default value
  - So when comparing against loaded environment, an unspecified variant in collection only matches if variant set on loaded environment is the default one
  - Collection procedures now rely on the ``modEq`` procedure

    - ``ismodlo`` flag is set to ``3`` to indicate a collection context
    - This collection context leads to also compare simplified module names (in addition to alternative names)
    - And to treat an unspecified variant on tested pattern as a default variant value

- There is no need to compare variants on following procedures

  - ``getLoadedWithClosestName``

    - Find a loaded module whose name and version is the closest to passed specification
    - Variant specified on loaded modules or on specification is ignored here

  - ``modStartNb``

    - Only used to compare module name and versions
    - Used by ``getLoadedWithClosestName`` and ``isModuleHidden``

  - ``modEqStatic``

    - Always used over non-loaded or non-loading context
    - Used by ``findModules`` and ``getModules``

  - ``getEqArrayKey``
  - ``cmdModuleSearch``
  - ``cmdModuleSwitch``
  - ``getModules``

    - Unless if module specification contains a variant definition, which
      triggers an *extra match search* process
    - Used by ``cmdModuleAvail``, ``getPathToModule``, ``isStickinessReloading``,
      ``cmdModulePaths``, ``cmdModuleSearch`` and ``cmdModuleAliases``

  - ``getPathToModule``

    - Which calls to ``getModules``
    - Used by ``cmdModulePath``, ``cmdModuleSearch``, ``cmdModuleSwitch``, ``cmdModuleLoad``,
      ``cmdModuleUnload``, ``cmdModuleTest``, ``cmdModuleHelp``, ``getAllModuleResolvedName``,
      ``is-avail``, ``getSimplifiedLoadedModuleList`` and ``cmdModuleDisplay``

  - ``getAllModuleResolvedName``

- Variant comparison is needed on following procedures

  - ``setModuleDependency``
  - ``getUnmetDependentLoadedModuleList``
  - ``getDirectDependentList``
  - ``cmdModuleLoad``
  - ``cmdModuleList``
  - ``conflict``
  - ``getLoadedMatchingName``
  - ``doesModuleConflict``
  - ``getModuleTag``

    - Useful when a tag is defined only when a specific variant is set

  - ``collectModuleTag``

    - Useful when a tag is defined only when a specific variant is set

  - ``getModuleHidingLevel``

    - Useful when a module with a specific variant value set is defined hidden
    - FUTURE: if variants are reported on ``avail``, hiding a variant specific value
      would have an effect on this sub-command

  - ``isModuleHidden``

    - Useful when a module with a specific variant value set is defined hidden
    - FUTURE: if variants are reported on ``avail``, hiding a variant specific value
      would have an effect on this sub-command


Specific impact
^^^^^^^^^^^^^^^

- When loading a module with variant, if this module is already loaded but with a different variant set an error is raised

- Tags applying specifically on variants do not appear over ``avail`` result

  - Even when variants are processed on ``avail`` mode
  - However if a module is loaded and tags apply to the variant selected, these tags will appear on the module designation within ``avail`` result

- Forbidding a specific variant of a module will not exclude it from search results

  - The module will still be reported on ``avail`` or evaluated on ``whatis``
  - Even if this specific variant is searched
  - As search sub-commands report all possible variants of available modules

- Hiding a specific variant of a module will not hide the module from search results

  - Even if this specific variant is searched
  - As search sub-commands report all possible variants of available modules

- Sticky modules can be swapped by another sticky modules if the stickiness applies to the generic module name

  - It stays true even if module has variants
  - Which means if stickiness designate module version or several versions, sticky module cannot be changed once loaded
  - Variant change cannot either occur
  - FUTURE: this may be revised to allow variant change if needs arise

- Stickiness can be defined over specific variant value, like any other tag

  - In case stickiness applies to the default value of a variant
  - When swapping sticky modules by another still matching the sticky rule
  - The variant with default value has to be explicitly specified for the swap to be allowed

    - As it cannot be guessed prior loading module that the default value of the variant will match the sticky rule
    - It applies to both sticky module swap context: ``restore`` and ``switch``

- On ``module-info tags``, currently defined tags need to get fetched when called

  - As variant specified prior ``module-info tags`` call may change the list of tags that apply
  - Especially when a variant inherits its default value as it is not specified when loading module

- ``module-info specified`` returns module name version and variants specified to designate the currently evaluating module

- ``module-info name`` only returns module name and version, variants are not part of the result

  - Variants can be fetched in the ``ModuleVariant`` array within evaluation context


Corner cases
------------

- When loading a variant which is an optional requirement for an already loaded module

  - If this optional requirement is loaded without specifying its variant value to get the default variant value
  - Currently it is not seen as an optional requirement at the load time
  - Thus dependent module is not reloaded
  - FUTURE: the *deps* evaluation mode that will be triggered prior module load may fix this current limitation

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

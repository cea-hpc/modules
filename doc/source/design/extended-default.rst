.. _extended-default:

Extended default
================

Configuration
-------------

- introduce ``extended_default`` option name
- off by default in v4 as previously soft/1 was an error
- on by default in v5, option could even disappear
- set as a separate option than *advanced_version_spec*
    - as it affects also basic soft/vers version specification
    - seem easier to understand for user if concepts are distinguished

Specification
-------------

- Take partial version identifier and returns matches
    - for soft/10.1.2.4 soft/10.1.2.3 soft/10.1.1
    - query soft/10 returns highest among 3
    - query soft/10.1 returns highest among 3
    - query soft/1 returns nothing
    - query soft/10.1.2 returns highest among soft/10.1.2.4 soft/10.1.2.3

- In situation where soft/1.1(default) soft/1.2 soft/2.1 soft/2.2
    - query soft/1 returns soft/1.1
    - query soft/2 returns soft/2.2

- Character considered as version number separator: ``.``
    - list: ``.`` and ``-``
    - ``-`` was also considered initially, but cannot determine in all case the highest version specified after this character (may find a hash name, strings like *rc*, *alpha*, *beta*, etc)
    - not possible with ``+`` as it is used by variant specification

- Does not apply to the root part of module name
    - e.g. *foo.2*

- If ``implicit_default`` is disabled
    - it makes ``extended_default`` inoperant if queried version does not include a defined default
    - even if only one modulefile matches query
    - with situation described above query soft/1 returns soft/1.1
    - but query soft/2 returns an error as no default is found among version 2 modulefiles

- Contexts where it could be used
    - :ref:`module_version_specification_to_return_all_matching_modules`
        - on specification that are processed as a wild search, ``extended_default`` has no impact
            - as a wildcard character ``*`` is appended to specification
            - affects ``avail`` sub-command
    - :ref:`module_version_specification_to_select_one_module`
    - :ref:`module_version_specification_to_check_compatibility`


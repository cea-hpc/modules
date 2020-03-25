.. _default-latest-version-specifiers:

Default and latest version specifiers
=====================================

Configuration
-------------

- Rely on ``advanced_version_spec`` option

    - No need for an extra configuration option
    - When option is on it automatically enables use of these two new version specifiers
    - When ``advanced_version_spec`` is off, ``mod@latest`` is considered as a modulefile name to find
    - in case ``implicit_default`` is disabled

        - means a ``default`` symbol or a ``latest`` symbol should be found defined to respectively make use of the ``@default`` or ``@latest`` specifiers
        - an error is otherwise raised


Specification
-------------

- When a ``default`` or a ``latest`` symbol is defined

    - ``@default`` or respectively ``@latest`` will resolve to the symbol target

- When no ``default`` or ``latest`` symbol is defined

    - ``@default`` and ``@latest`` point to the highest existing version (also called the implicit default)
    - note that if a ``default`` symbol is defined but not a ``latest`` symbol, ``@default`` points to the defined default and ``@latest`` points to the highest version that exists

- ``default`` or ``latest`` version specifiers can be employed:

    - in single version specification: ``@default`` or ``@latest``
    - in version list: ``@vers1,default`` or ``@latest,default,vers2``

- ``default`` or ``latest`` version specifiers cannot be employed in version range

    - otherwise an error is raised

- When a ``default`` or ``latest`` modulefile exists

    - ``@default`` or respectively ``@latest`` will resolve to the existing modulefile
    - no automatic symbol will be recorded in this case as ``default`` or ``latest`` are regular versions


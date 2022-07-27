.. _default-latest-version-specifiers:

Default and latest version specifiers
=====================================

Configuration
-------------

- Rely on :mconfig:`advanced_version_spec` option

  - No need for an extra configuration option
  - When option is on it automatically enables use of these two new version specifiers
  - When ``advanced_version_spec`` is off, ``mod@latest`` is considered as a modulefile name to find
  - in case ``implicit_default`` is disabled

    - means a ``default`` symbol or a ``latest`` symbol should be found defined to respectively make use of the ``@default`` or ``@latest`` specifiers
    - a not found error is otherwise raised, as specified version does not exist


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

- ``default`` or ``latest`` version specifiers can also be specified with the traditional ``mod/version`` syntax

  - for instance ``mod/default`` or ``mod/latest``

- When a ``default`` or ``latest`` modulefile exists

  - ``@default`` or respectively ``@latest`` will resolve to the existing modulefile
  - no automatic symbol will be recorded in this case as ``default`` or ``latest`` are regular versions

- To be included in module search result, the version specifiers should fully match search query or search query should not target a specific module version

  - the automatically defined symbolic versions are included in results for queries like ``mod@latest`` or ``mod``
  - but not for queries like ``mod@la``, ``mod@def``, ``mod@lat*``, ``mod@def??lt``

- Automatically defined ``default`` and ``latest`` version specifiers are not displayed to avoid overloading output:

  - on ``module list`` output
  - on ``module avail`` output
  - those two sub-commands only display symbolic versions manually defined

- Alternative module names deduced from the automatically defined version specifiers need to be tracked

  - in loaded environment for each targeted module loaded
  - to keep track loaded module is default or latest version
  - thus keeping ability to answer queries like ``is-loaded mod@latest`` from further modulefile evaluation or module command-line
  - this information is kept in the :envvar:`__MODULES_LMALTNAME` environment variable, along other alternative names
  - Auto symbols in this variable are recorded with a ``as|`` prefix to distinguish them from other alternative names

    - for instance ``mod/1.2&mod/regular_symbol&as|mod/latest``
    - it helps to filter auto symbols from regular symbols that need to be displayed

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

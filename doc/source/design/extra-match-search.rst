.. _extra-match-search:

Extra match search
==================

Traditional module lookup only performs modulefile search, module version
resolution, module alias and symbolic version resolution. This is based on
file lookup and evaluation of modulerc files.

Searching modules with a query exceeding traditional lookup is called *extra
match search*. It mainly looks at modulefile content to retain those matching
specified query. This content lookup requires a specific modulefile evaluation
(see `Modulefile evaluation mode for extra match search`_).

When extra match search applies
-------------------------------

On :ref:`module_version_specification_to_return_all_matching_modules` context.
On such context, variant specification in search query was previously ignored.
Extra match search adds here a feature to filter returned results.

On :ref:`module_version_specification_to_select_one_module` context, extra
match search is not enabled at the moment. As it changes obtained result. For
instance when loading a module with a given variant value, *extra match
search* will remove from available modules those not declaring this variant
and its specified value. It is considered to add an option to apply *extra
match search* to the *select one module* context. This option will be disabled
by default on Modules v5 and enabled by default on v6.

What triggers an extra match search?
------------------------------------

* module variants should be reported
* variant is specified in query on :subcmd:`avail`, :subcmd:`paths` or
  :subcmd:`whatis`
* an extra specifier is specified in query

Extra match search process
--------------------------

Extra match search is performed from ``getModules`` procedure. All subsequent
procedures relying on ``getModules`` will then benefit from this extra process
without a change.

Extra match search is done right after *phase 3: elaborate directory content
with default element selection*. At this stage:

* symbolic versions, aliases and virtual modules are known
* dynamically hidden or expired elements have been filtered
* automatically defined symbols are set
* initial directory content and their default version are set

This way the subsequent phases that filters result based on name and version
search query, adjusts directory content and no-indepth mode does not need to be
updated and will apply on the extra match search result.

As extra match search is performed after *phase 3*, search operation will only
applies on result obtained from traditional lookup.

In case no-indepth mode is asked and extra match search should be performed,
traditional file lookup (by ``findModules``) is made to return all existing
files. Not only those matching depth level. As extra match filtering process
may eliminate some entries, all of them should be tested to obtain accurate
result.

Extra match search will only be performed if search query requires it. It will
be determined by ``isExtraMatchSearchRequired`` procedure. See `What triggers
an extra match search?`_ for applying conditions. Current search module
specification is passed as argument. This string helps to find if:

* variant has been specified in *ModuleVersSpec* structure
* extra specifier has been specified in *ModuleVersSpec* structure

If search query is a traditional one, no extra match search is performed. It
saves this way substantial processing time as no evaluation of modulefile is
performed.

``getModules`` will call a dedicated procedure named
``filterExtraMatchSearch`` to handle the extra match search process. It will
obtain the name of result array as argument and will bind to this array
variable (to change it without copy). Search module specification is also set
as argument to help to know the extra match query to filter modules on.

``filterExtraMatchSearch`` evaluates all entries in obtained result array that
are of modulefile or virtual types. See `Modulefile evaluation mode for extra
match search`_ section. With the consolidated information gathered by these
evaluations, procedure filters entries in result array that do not match the
extra match query.

Symbolic versions
^^^^^^^^^^^^^^^^^

From ``filterExtraMatchSearch``, symbolic version elements are retained unless
if they target an element that is withdrawn by the filter process.

They need to be filtered if their target is filtered as it may confuse some
sub-commands like ``paths`` if they are part of ``getModules`` result.

During wild search (no module name specified) with variant specified, auto
symbolic versions (*latest* and *default*) are disabled. This is done
especially for sub-commands like ``paths`` that take getModules result and try
to resolve each entry. In this situation it is important not to have any auto
symbol that indirectly targets an element withdrawn from extra match filter.

Module aliases
^^^^^^^^^^^^^^

From ``filterExtraMatchSearch``, module alias elements are purely withdrawn by
the filter process as they are not resolved on *return all matching modules*
context. They are also withdrawn even if searched module name matches alias
name.

They need to be filtered when search query mentions a specific variant and
value as alias does not define variant in itself.

When variants have to be reported but are not part of the search query, then
no filtering process occurs, aliases are not withdrawn.

Directories overridden by aliases take precedence over these aliases when
*extra match search* is performed.

FUTURE: it may be revised to implement extra match search on *select one
module* context.

Module tags
^^^^^^^^^^^

Modulefiles tagged *forbidden* are excluded from evaluation mechanism. If
forbidden tag only applies to some specific variant values, modulefile is
evaluated.

Note that in a *select one module* context, if forbidden tag applies
specifically to searched variant, this module is excluded from result.

If module hidden tag only applies to some specific variant values, modulefile
is evaluated in scan mode.

Error entries
^^^^^^^^^^^^^

Files in error (e.g., without a module header string, not readable) are
ignored by ``filterExtraMatchSearch`` and kept in result array. These error
entries are useful to report the message they contain if erroneous file was
specifically selected.

Query grammar
-------------

In extra match search query, variants are expressed like in *select one
module* context:

* multiple variants mentioned act as an *AND* operation

  * ``module avail mod/1.0 foo=val1 bar=val2``
  * means *foo* equals *val1* and *bar* equals *val2*

* same variant mentioned multiple times: only the last mention is retained

  * ``module avail mod/1.0 foo=val1 foo=val2``
  * means *foo* equals *val2*

FUTURE: expressing an *OR* operation

* for instance by mentioning multiple values separated by ``,`` character
* ``module avail mod/1.0 foo=val1,val2 bar=val2``
* means *foo* equals *val1* or *val2* and *bar* equals *val2*
* such syntax is equivalent to version list specification
* does not seem useful to allow ranges like for version
* FIXME: allow such syntax on any context or only for *return all matching
  modules* context?

FUTURE: expressing a *NOT* operation

* adding a character like ``~`` or ``!`` prior value
* ``module avail mod/1.0 foo=!val1 bar=val2``
* means *foo* equals any value expect *val1* and *bar* equals *val2*
* FIXME: allow such syntax on any context or only for *return all matching
  modules* context?
* NOTE: *NOT* operator is not specified currently on Spack's side. Some
  discussions mention use of ``!``, which does not work for Tcsh shell

What triggers ``scan`` evaluation?
----------------------------------

* everything that triggers an extra match search
* except *tag* extra specifier, as tags are defined outside of modulefiles

Modulefile evaluation mode for extra match search
-------------------------------------------------

A specific evaluation mode is created for looking at modulefile content in
order to solve an extra match search. This mode is named ``scan``.

Modulefile Tcl interpreter is configured to run a specific variant of
modulefile Tcl commands. These variants will gather content of modulefile in
global structures that will be searched after all *scan* evaluations to
determine what modules match the extra query. Names of scan variant of
modulefile Tcl command scan variant procedures are internally suffixed with
*-sc*.

If modulefile content queries the current evaluation mode, it will obtain the
``scan`` string during a *scan* evaluation. We expect modulefile code to cope
with this behavior, especially to still be able to get all *load* information
of modulefile, even if mode is not ``load``. A ``load`` string cannot be
returned during a *scan* evaluation as modulefile may have some code to
produce log message when a *load* attempt is detected.

Even if extra match search only concern a specific element, like querying
module variants, all modulefile commands are setup to gather all content
information about modulefile. This way, it is evaluated the same way whatever
the query. Scan evaluation code will also be simpler.

Environment variable definition through modulefile commands like ``setenv``,
``append-path``, etc are handled like in ``whatis`` evaluation mode. Variables
are set to an empty value. Some processing time is saved this way. It may have
an impact on some complex modulefiles: content scan may not be accurate. If
this is an issue on some setup, an option could be added in the future to set
variable to their expected value also on ``whatis`` and ``scan`` evaluation
modes.

When an error is raised from a modulefile evaluation. No error message is
reported as such message is silenced during the whole ``avail`` process.
Rendered result is ok for all kind of errors even for ``break`` or ``exit``.
No error result is rendered for these two, unlike for a ``whatis`` evaluation,
not to disturb the whole search result for one modulefile not coping with scan
evaluation.

``puts`` commands in modulefile are rendered like on a ``whatis`` evaluation
mode: output is effectively produced toward the designated channel. It is up
to modulefile writer to handle the ``scan`` evaluation, like done for
``whatis``.

``source-sh`` commands in modulefile are rendered like on a ``load``
evaluation. They will produce ``setenv``, ``set-alias``, ``set-function`` and
so on commands that will be in turn evaluated in *scan* mode.

Specific impact
---------------

* With a ``contains`` search match every modulefiles in modulepath are
  evaluated in scan mode.

  - FUTURE: there may have room for optimisation here

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:

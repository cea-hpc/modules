.. _hide-or-forbid-modulefile:

Hide or forbid modulefile
=========================

Configuration
-------------

- No specific configuration


Use cases
---------

- Restrict usage of particular software to a limited set of user (RestrictUsage)

    - Included in module specification result for a not granted user: no
    - Visible for a not granted user on a full ``avail``: no
    - Visible for a not granted user on a targeted ``avail``: no
    - Visible for a not granted user once loaded on a ``list``: yes
    - Load tentative for a not granted user: error

- Allow usage of particular software once clearance has been obtained (AllowOnceCleared)

    - Included in module specification result for a not granted user: yes
    - Visible for a not granted user on a full ``avail``: yes
    - Visible for a not granted user on a targeted ``avail``: yes
    - Visible for a not granted user once loaded on a ``list``: yes
    - Load tentative for a not granted user: error

- Expire a software after a given date (Expire)

    - Included in module specification result for a not granted user: no after expiration date
    - Visible for a not granted user on a full ``avail``: no after expiration date
    - Visible for a not granted user on a targeted ``avail``: no after expiration date
    - Visible for a not granted user once loaded on a ``list``: yes, even after expiration date
    - Load tentative for a not granted user: error after expiration date

- Disclose a software after a given date (Disclose)

    - Included in module specification result for a not granted user: no prior disclosure date
    - Visible for a not granted user on a full ``avail``: no prior disclosure date
    - Visible for a not granted user on a targeted ``avail``: no prior disclosure date
    - Visible for a not granted user once loaded on a ``list``: yes, even prior disclosure date
    - Load tentative for a not granted user: error prior disclosure date

- Hide software not of interest for current user (HideNotOfInt)

    - Included in module specification result for a not granted user: yes
    - Visible for a not granted user on a full ``avail``: no, unless specific option set
    - Visible for a not granted user on a targeted ``avail``: yes
    - Visible for a not granted user once loaded on a ``list``: yes
    - Load tentative for a not granted user: success

- Hide software only useful for other software as dependency (HideDep)

    - Included in module specification result for a not granted user: yes
    - Visible for a not granted user on a full ``avail``: no, unless specific option set
    - Visible for a not granted user on a targeted ``avail``: yes
    - Visible for a not granted user once loaded on a ``list``: yes
    - Load tentative for a not granted user: success

- Hide dependency software once loaded (HideDepOnceLoaded)

    - Included in module specification result for a not granted user: see HideDep
    - Visible for a not granted user on a full ``avail``: see HideDep
    - Visible for a not granted user on a targeted ``avail``: see HideDep
    - Visible for a not granted user once loaded on a ``list``: no, unless specific option set
    - Load tentative for a not granted user: see HideDep


Specification
-------------

- 2 new modulefile commands are introduced for the needs described above: ``module-hide`` and ``module-forbid``

- ``module-hide`` removes visibility of specified modules

    - ``module-hide`` acts when modules are searched (``avail``, ``whatis`` and ``search`` sub-commands)

    - Visibility is however enabled if hidden module is specifically searched

        - On all context hidden module *mod/1.0* is included in result for instance if *mod/1.0* or *mod@1.0,2.0* are specficied
        - But hidden module *mod/1.0* is excluded from result if *mod@:2.0* or *mod@1:* are specified
        - And is also excluded from result if *mod* or *mod/** are specified when *mod/1.0* is set default
        - Unless if search is made to select one module since in this context a *mod* search query is equivalent to *mod/default*

    - Excluded from module resolution result

        - Unless precisely specified on the following selection contexts:

            - :ref:`module_version_specification_to_select_one_module`
            - :ref:`module_version_specification_to_check_compatibility`
            - :ref:`module_version_specification_to_return_all_matching_modules`

        - For example, the hidden module *mod/1.0*

            - is included in ``module load mod/1.0`` result
            - is excluded from ``module load mod/1`` result, even if default symbol targets it
            - is excluded from ``module load mod`` result, unless if default symbol targets it (as query is equivalent to *mod/default*)
            - is excluded from ``module load mod@:2`` result, even if default symbol targets it
            - is included in ``module load mod@1.0,2.0`` result
            - is included/excluded the same way for ``prereq`` and ``conflict`` sub-commands than ``load`` sub-command
            - is matched by ``is-loaded`` and ``info-loaded`` sub-commands querying it once loaded
            - is excluded from ``module whatis`` result
            - is included/excluded the same way for ``whatis`` sub-command than ``avail`` sub-command
            - is excluded from ``module avail`` result
            - is excluded from ``module avail m*`` result
            - is included in ``module avail mod/1.0`` result
            - is excluded from ``module avail mod/1`` result, even if default symbol targets it
            - is excluded from ``module avail mod`` result, even if default symbol targets it (as query is NOT equivalent to *mod/default* in this context)
            - is excluded from ``module avail mod@:2`` result, even if default symbol targets it
            - is included in ``module avail mod@1.0,2.0`` result

    - Included in module resolution result if ``--all`` option of ``avail``, ``whatis``, ``search`` and ``aliases`` sub-commands is set

        - ``--all`` option does not apply to ``is-avail`` sub-command to make it coherent with ``load`` sub-command (eg. a ``is-avail mod`` returning true implies ``load mod`` effectively loading a module)

    - Visibility of a module targeted by a ``module-hide`` command acts similarly than for a file whose name is prefixed by a dot character on Unix platform

    - If ``--soft`` option is set on ``module-hide`` command, module hiding is weakened

        - Module is always included in resolution result for the following contexts

            - :ref:`module_version_specification_to_select_one_module`
            - :ref:`module_version_specification_to_check_compatibility`

        - For example, the hidden module *mod/1.0*

            - is included in ``module load mod/1.0`` result
            - is included in ``module load mod/1`` result
            - is included in ``module load mod`` result
            - is included in ``module load mod@:2`` result
            - is included in ``module load mod@1.0,2.0`` result
            - is included/excluded the same way for ``prereq`` and ``conflict`` sub-commands than ``load`` sub-command
            - is matched by ``is-loaded`` and ``info-loaded`` sub-commands querying it once loaded
            - is excluded from ``module whatis`` result
            - is included/excluded the same way for ``whatis`` sub-command than ``avail`` sub-command
            - is excluded from ``module avail`` result
            - is excluded from ``module avail m*`` result
            - is included in ``module avail mod/1.0`` result
            - is included in ``module avail mod/1`` result
            - is included in ``module avail mod`` result
            - is included in ``module avail mod@:2`` result
            - is included in ``module avail mod@1.0,2.0`` result

    - If ``--hide-once-loaded`` option is set on ``module-hide``, hiding also applies to specified modules once they are loaded

        - Hidden once loaded modules do not appear on ``module list``
        - Unless ``--all`` option is set on ``list`` sub-command
        - When those modules are loaded, they are recorded in ``MODULES_LMHIDDEN`` environment variable to keep track of their hidden status
        - Hidden once loaded status does not affect ``is-loaded``: these modules will always be reported if they match ``is-loaded`` queries

- ``module-forbid`` bans specified modules

    - They are strictly hidden

        - ``--all`` option of ``avail`` sub-command cannot unveil them

    - Excluded from module resolution result

        - Unless precisely specified on the following selection contexts:

            - :ref:`module_version_specification_to_select_one_module`
            - :ref:`module_version_specification_to_check_compatibility`

        - Which means it is always excluded from resolution on following context:

            - :ref:`module_version_specification_to_return_all_matching_modules`

        - For example, the forbidden module *mod/1.0*

            - is included in ``module load mod/1.0`` result
            - is excluded from ``module load mod`` result, even if default symbol targets it
            - is excluded from ``module load mod/1`` result, even if default symbol targets it
            - is excluded from ``module load mod@:2`` result, even if default symbol targets it
            - is included in ``module load mod@1.0,2.0`` result
            - is included/excluded the same way for ``prereq`` and ``conflict`` sub-commands than ``load`` sub-command
            - is matched by ``is-loaded`` and ``info-loaded`` sub-commands querying it once loaded
            - is excluded from any ``avail`` query result
            - is included/excluded the same way for ``whatis`` sub-command than ``avail`` sub-command

    - Error is rendered when trying to load them

    - Visibility of a module targeted by a ``module-forbid`` command acts the same than for a modulefile with no read permission granted on filesystem

- ``module-hide`` accepts options that change its behavior:

    - ``--hide-once-loaded``: hides module from loaded module list
    - ``--soft``: lightweight module hide
    - ``--not-user``: specify a list of users unaffected by hide mechanism
    - ``--not-group``: specify a list of groups whose member are unaffected by hide mechanism
    - ``--before``: enables hide mechanism until a given date
    - ``--after``: enables hide mechanism after a given date

- ``module-forbid`` accepts options that change its behavior:

    - ``--visible``: disables the hidden property of the command which means module is normally included in resolution
    - ``--not-user``: specify a list of users unaffected by forbid mechanism
    - ``--not-group``: specify a list of groups whose member are unaffected by forbid mechanism
    - ``--before``: enables forbid mechanism until a given date
    - ``--after``: enables forbid mechanism after a given date

- Each use case expressed above are covered by following command:

    - RestrictUsage: ``module-forbid``
    - AllowOnceCleared: ``module-forbid --visible``
    - Expire: ``module-forbid --after``
    - Disclose: ``module-forbid --before``
    - HideNotOfInt: ``module-hide --soft``
    - HideDep: ``module-hide --soft``
    - HideDepOnceLoaded: ``module-hide --soft --hide-once-loaded``

- ``module-hide`` and ``module-forbid`` accept the specification of several modules

    - For instance ``module-hide mod1 mod2...``
    - :ref:`advanced-module-version-specifiers` are supported if relative module option is enabled
    - Full path specification are not supported, as modulerc are not evaluated when reaching a modulefile specified as full path

        - For instance, ``/path/to/modulefiles/.modulerc`` is not evaluated when loading ``/path/to/modulefiles/mod/1.0``
        - Thus ``module-hide`` and ``module-forbid`` commands set in this modulerc files are not evaluated
        - If module is specified as full path, no error is returned, but it will have no effect as demonstrated above

            - Unless on very specific cases, where a global rc file defines these hidden/forbidden commands for the full path modules

- ``--not-user`` and ``--not-group`` specification is only supported on Unix platform

    - Error is rendered if these 2 options are used on Windows platform

- ``--before`` and ``--after`` are also supported by ``module-hide`` to phase-out modules prior forbidden them

- ``--before`` and ``--after`` accept a date as value

    - FIXME: specify date format
    - if both ``--before`` and ``--after`` options are set and *before* date is greater than *after* date

        - targeted module is always hidden/forbidden
        - no error is returned

- ``module-hide`` and ``module-forbid`` are intended to be used in modulerc files

    - as they impact modulefile resolution
    - they also need to be enabled in modulefile context as global/user rc files are evaluated as modulefile, not modulerc

- several ``module-hide`` or ``module-forbid`` calls for the same module will supersede each other

    - definition with the highest hiding level wins
    - which means the most restrictive call wins
    - a ``--hide-once-loaded`` status set is kept even if corresponding ``module-hide`` call is not the highest one
    - the multiple definitions can come accross different modulerc files (global, modulepath or modulefile rc levels)

- Module specification passed as argument to ``module-hide`` and ``module-forbid`` are matched exactly against available modules

    - Exception made to the *extended_default* and *icase* variables
    - Which means wildcard characters like *\** or *?* are treated literally

- Auto symbols (*@default* and *@latest*) are adapted when a *latest* version is hidden

    - Auto symbols are applied to this version if it is selected specifically (for instance loaded by its full name)
    - Auto symbols are applied to another version when hidden latest is not selected specifically, even if specified with *@latest* auto symbol

- When module specification of ``module-hide`` or ``module-forbid`` targets:

    - A symbolic version

        - This symbol only is hidden
        - Modulefile targeted by hidden symbolic version stays visible

    - An alias

        - This alias only is hidden
        - Modulefile targeted by hidden alias stays visible

    - A modulefile targeted by either symbolic version or alias

        - This modulefile is hidden and all symbolic versions targeting it
        - Aliases targeting modulefile stays visible (thus resolving alias in *load* or *whatis* contexts make hidden modulefile target visible)

- Hidden alias or symbolic version should not appear in the list of alternative names of loaded modules

    - Unless this alias or symbolic version were used to designate the module to load
    - When ``default`` symbolic version is set hidden

        - also remove parent module name from the list of alternative names
        - if resolution query corresponds to parent module name, unhide ``default`` symbol

- On ``avail`` sub-command

    - Hidden symbolic versions are not reported along module they target

        - Unless for symbols specifically designated in search query

    - A ``--default`` filtered search considers search query matches ``default`` symbol

        - So ``default`` symbolic version will appear in result even if hidden

- Different hiding level are considered

    - *-1*: module is not hidden
    - *0*: soft hiding (applied with ``module-hide --soft``)
    - *1*: regular hiding (applied with dot name module or default ``module-hide`` command)
    - *2*: hard hiding (applied with ``module-forbid``)

- Hiding threshold

    - is *0* by default, which means module is considered hidden if its hiding level is greater or equal to *0*
    - is raised to *2* when ``--all`` option is applied, which means module is considered hidden if its hiding level is greater or equal to *2*

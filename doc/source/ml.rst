.. _ml(1):

ml
==

SYNOPSIS
--------

**ml** [*switches*] [*sub-command* [*sub-command-args*]]

**ml** [*switches*] [[-]\ *modulefile*...]


DESCRIPTION
-----------

:command:`ml` is a user interface to the Modules package. The Modules package
provides for the dynamic modification of the user's environment via
*modulefiles*.

:command:`ml` acts as a shortcut command to the :command:`module` command thus
it supports all the command line switches and module sub-commands that are
supported by :command:`module`.

:command:`ml` also provides handy shortcuts to list currently loaded
modulefiles, when no argument is provided to :command:`ml`; to load
modulefiles, when modulefile names are passed right after :command:`ml`
command name; to unload modulefiles, when modulefile names prefixed by a minus
sign (``-``) are passed right after :command:`ml` command name.

Multiple modulefiles to load and to unload can be specified on a single
:command:`ml` command line. In this situation unloads are treated first in
specified order, then loads are processed also in specified order (see
`EXAMPLES`_ section below). If an error occurs among either modulefile loads
or unloads, command line processing is stopped and every unloads and loads
performed are rolled back.

:command:`ml` command line is parsed first to match every known command line
switches or module sub-commands. So to load a modulefile via the :command:`ml`
shortcut syntax, modulefile name should not equal a module sub-command name.
Moreover to unload a modulefile via the :command:`ml` shortcut syntax,
modulefile name should not equal a command line switch short name.

See the :ref:`DESCRIPTION<module DESCRIPTION>` section in :ref:`module(1)` for
the list of supported command line switches and module sub-commands.


EXAMPLES
--------

Loading modulefile ``foo`` then look at currently loaded modulefiles::

    $ ml foo
    $ ml
    Currently Loaded Modulefiles:
     1) foo

Unloading modulefile ``foo`` then list modulefiles still loaded::

    $ ml -foo
    $ ml
    No Modulefiles Currently Loaded.

Mixing load and unload of modulefiles in a single command. All specified
unloads are processed first then loads are performed::

    $ ml -v -foo bar -baz qux
    Unloading foo
    Unloading baz
    Loading bar
    Loading qux


EXIT STATUS
-----------

The :command:`ml` command exits with **0** if its execution succeed. Elsewhere
``1`` is returned.


ENVIRONMENT
-----------

See the :ref:`ENVIRONMENT<module ENVIRONMENT>` section in :ref:`module(1)` for
the list of supported environment variables.


SEE ALSO
--------

:ref:`module(1)`, :ref:`modulefile(4)`


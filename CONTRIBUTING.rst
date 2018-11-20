.. _CONTRIBUTING:

Contributing
============

Thank you for considering contributing to Modules!

Support questions
-----------------

Please use the `modules-interest mailing list`_ for questions. Do not use the
issue tracker for this.

.. _modules-interest mailing list: https://sourceforge.net/projects/modules/lists/modules-interest

Asking for new features
-----------------------

Please submit your new feature wishes first to the `modules-interest mailing
list`_. Discussion will help to clarify your needs and sometimes the wanted
feature may already be available.

Reporting issues
----------------

* Describe what you expected to happen.
* If possible, include a `minimal, complete, and verifiable example`_ to help
  us identify the issue.
* Describe what actually happened. Run the ``module`` command in ``--debug``
  mode and include all the debug output obtained in your report.
* List your Modules and Tcl versions.
* Provide the name and content of the modulefiles you try to manipulate.

.. _minimal, complete, and verifiable example: https://stackoverflow.com/help/mcve

.. _submitting-patches:

Submitting patches
------------------

* Whether your patch is supposed to solve a bug or add a new feature, please
  include tests. In case of a bug, explain clearly under which circumstances
  it happens and make sure the test fails without your patch.
* If you are not yet familiar with the ``git`` command and `GitHub`_, please
  read the `don't be afraid to commit`_ tutorial.

.. _GitHub: https://github.com/
.. _don't be afraid to commit: https://dont-be-afraid-to-commit.readthedocs.io/en/latest/index.html

Start coding
~~~~~~~~~~~~

* Create a branch to identify the issue or feature you would like to work on
* Using your favorite editor, make your changes, `committing as you go`_.
* Comply to the `coding conventions of this project <coding-conventions_>`_.
* Include tests that cover any code changes you make. Make sure the test fails
  without your patch.
* `Run the tests <running-the-tests_>`_ and `verify coverage <running-test-coverage_>`_.
* Push your commits to GitHub and `create a pull request`_.

.. _committing as you go: https://dont-be-afraid-to-commit.readthedocs.io/en/latest/git/commandlinegit.html#commit-your-changes
.. _create a pull request: https://help.github.com/articles/creating-a-pull-request/

.. _running-the-tests:

Running the tests
~~~~~~~~~~~~~~~~~

Run the basic test suite with::

   make test

This only runs the tests for the current environment. `Travis-CI`_ and
`AppVeyor`_ will run the full suite when you submit your pull request.

.. _Travis-CI: https://travis-ci.org/cea-hpc/modules/
.. _AppVeyor: https://ci.appveyor.com/project/xdelaruelle/modules-a6nha

.. _running-test-coverage:

Running test coverage
~~~~~~~~~~~~~~~~~~~~~

Generating a report of lines that do not have test coverage can indicate where
to start contributing or what your tests should cover for the code changes you
submit.

Run ``make testcoverage`` which will automatically setup the `Nagelfar`_ Tcl
code coverage tool in your ``modules`` development directory. Then the full
testsuite will be run in coverage mode and a ``modulecmd.tcl_m`` annotated
script will be produced::

   make testcoverage
   # then open modulecmd.tcl_m and look for ';# Not covered' lines

.. _Nagelfar: http://nagelfar.sourceforge.net/

Building the docs
~~~~~~~~~~~~~~~~~

Build the docs in the ``doc`` directory using Sphinx::

   cd doc
   make html

Open ``_build/html/index.html`` in your browser to view the docs.

Read more about `Sphinx`_.

.. _Sphinx: https://www.sphinx-doc.org

.. _coding-conventions:

Coding conventions
~~~~~~~~~~~~~~~~~~

* Maximum line length is 78 characters
* Use 3 spaces to indent code (do not use tab character)
* Adopt `Tcl minimal escaping style`_
* Procedure names: ``lowerCameCase``
* Variable names: ``nocaseatall``
* Curly brace and square bracket placement::

   if {![info exists ::g_already_report]} {
      set ::g_already_report 1
   }

.. _Tcl minimal escaping style: https://wiki.tcl-lang.org/page/Tcl+Minimal+Escaping+Style

Submitting installation recipes
-------------------------------

* If you want to share your installation tips and tricks, efficient ways you
  have to write or organize your modulefiles or some extension you made to the
  ``module`` command please add a recipe to the cookbook section of the
  documentation.
* Create a directory under ``doc/example`` and put there the extension code
  or example modulefiles your recipe is about.
* Describe this recipe through a `reStructuredText`_ document in
  ``doc/source/cookbook``. It is suggested to have an *Implementation*,
  an *Installation* and an *Usage example* sections in that document to get
  as much as possible the same document layout across recipes.
* `Submit a patch <submitting-patches_>`_ with all the above content.

.. _reStructuredText: http://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html


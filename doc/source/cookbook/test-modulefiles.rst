.. _test-modulefiles:

Testing Modulefiles
===================

The following is an example for a ``ModulesTest`` proc of a Modulefile and its output.
It checks whether the ``TESTDIR`` is a directory, checks that it can enter it,
and whether a file ``TESTFILE`` can successfully be created there.

This code gets executed when you use the ``test`` command.

Code
----

.. literalinclude:: ../../example/test-modulefiles/modulefiles/test_dir_and_file
   :caption: test_dir_and_file


Usage example
-------------

Enable the modulepath where the example modulefiles are located::

   $ module use example/test-modulefiles/modulefiles

Run the test both with the test directory not existing and existing::

    $ module test test_dir_and_file
    -------------------------------------------------------------------
    Module Specific Test for .../modulefiles/test_dir_and_file:
    
    Running ModulesTest for directory existence...
    ERROR: Is not a directory: /tmp/testuser/testdir
    Running ModulesTest for directory existence...done
    Running ModulesTest for directory permissions...
    ERROR: Was not able to enter directory /tmp/testuser/testdir: couldn't change working directory to "/tmp/testuser/testdir": no such file or directory
    Running ModulesTest for directory permissions...done
    Running ModulesTest for file creation...
    ERROR: Was not able to create file /tmp/testuser/testdir/testfile: couldn't open "/tmp/testuser/testdir/testfile": no such file or directory
    Running ModulesTest for file creation...done
    Test result: FAIL
    -------------------------------------------------------------------
    $ mkdir /tmp/$USER/testdir
    $ module test test_dir_and_file
    -------------------------------------------------------------------
    Module Specific Test for .../modulefiles/test_dir_and_file:
    
    Running ModulesTest for directory existence...
    Is a directory: /tmp/testuser/testdir
    Running ModulesTest for directory existence...done
    Running ModulesTest for directory permissions...
    Was able to enter directory /tmp/testuser/testdir
    Running ModulesTest for directory permissions...done
    Running ModulesTest for file creation...
    Was able to create file /tmp/testuser/testdir/testfile
    Running ModulesTest for file creation...done
    Test result: PASS
    -------------------------------------------------------------------

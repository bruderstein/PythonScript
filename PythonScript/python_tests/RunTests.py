# -*- coding: utf-8 -*-
import sys
import unittest
import os
from Npp import console, notepad, MESSAGEBOXFLAGS

def main():

    # A debug build influences the test runs due to the assertions and print outputs.
    if notepad.messageBox(
        "Make sure you have a release build of the PythonScript plugin if you want to run the test suite.\n\nShould the test suite be started?",
        "Run Tests?",
        MESSAGEBOXFLAGS.ICONQUESTION | MESSAGEBOXFLAGS.YESNO
    ) == MESSAGEBOXFLAGS.RESULTNO:
        return

    # unittest module expects argv to be set
    sys.argv = ['']

    (path, runTestsName) = os.path.split(__file__)

    test_suites = []

    results = unittest.TestResult()
    loader = unittest.TestLoader()
    test_suites = loader.discover(os.path.join(path, 'tests'))

    alltests = unittest.TestSuite(test_suites)

    console.clear()


    alltests.run(results)

    # console.write('Tests Run: {} Errors: {} Failures: {}'.format(results.testRun, results.errors, results.failures))

    def writeTestFailure(error):
        console.write('TEST: %s\n' % error[0])
        console.writeError(error[1])
        console.write('\n----------------------------\n')

    if results.errors:
        for error in results.errors:
            writeTestFailure(error)

    if results.failures:
        for error in results.failures:
            writeTestFailure(error)

    if results.errors or results.failures:
        console.writeError('Tests Run: {} Errors: {} Failures: {}\n'.format(results.testsRun, len(results.errors), len(results.failures)))
    else:
        console.write('Tests Run: {} Errors: {} Failures: {}\n'.format(results.testsRun, len(results.errors), len(results.failures)))

    if results.skipped:
        console.write('Skipped: {}\n'.format(len(results.skipped)))
        for skipped_test in results.skipped:
            console.write('     {}  -  {}\n'.format(skipped_test[0], skipped_test[1]))

    console.show()


main()

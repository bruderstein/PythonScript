# -*- coding: utf-8 -*-
import sys
import unittest
import os

# unittest module expects argv to be set
sys.argv = ['']

(path, runTestsName) = os.path.split(__file__)

test_suites = []

for test_name in  os.listdir(os.path.join(path, 'tests')):
    (test_name, ext) = os.path.splitext(test_name)
    if ext == '.py':
        test_module = reload(__import__('npp_unit_tests.tests.' + test_name))
        test_suite = getattr(test_module.tests, test_name)
        if test_suite:
            reload(test_suite)
        if hasattr(test_suite, 'suite'):
            test_suites.append(test_suite.suite)


alltests = unittest.TestSuite(test_suites)

console.show()
console.clear()


results = unittest.TestResult()

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

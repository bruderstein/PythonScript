# -*- coding: utf-8 -*-
import sys
import unittest

# unittest module expects argv to be set
sys.argv = ['']

import npp_unit_tests.tests.ReplaceUTF8TestCase
reload(npp_unit_tests.tests.ReplaceUTF8TestCase)

import npp_unit_tests.tests.ReplaceAnsiTestCase
reload(npp_unit_tests.tests.ReplaceAnsiTestCase)

alltests = unittest.TestSuite([npp_unit_tests.tests.ReplaceUTF8TestCase.suite,
                               npp_unit_tests.tests.ReplaceAnsiTestCase.suite])


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

console.show();


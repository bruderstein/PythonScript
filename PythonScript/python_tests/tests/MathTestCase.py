# -*- coding: utf-8 -*-
import unittest

class MathTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_complex_abs(self):
        cplx = complex(-2, -1)
        self.assertAlmostEqual(abs(cplx), 2.236067977, places = 6)

suite = unittest.TestLoader().loadTestsFromTestCase(MathTestCase)
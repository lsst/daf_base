#!/usr/bin/env python

import unittest

from lsst.daf.base import DateTime
import time

class DateTimeTestCase(unittest.TestCase):
    """A test case for DateTime."""

    def testMJD(self):
        ts = DateTime(45205.125, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 399006000000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 399006021000000000L)
        self.assertAlmostEqual(ts.mjd(DateTime.UTC), 45205.125)
        self.assertAlmostEqual(ts.mjd(DateTime.TAI),
                45205.125 + 21.0 / 86400.0)

    def testNsecs(self):
        ts = DateTime(1192755473000000000L, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1192755473000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 1192755506000000000L)
        self.assertEqual(ts.nsecs(), 1192755506000000000L)
        self.assertAlmostEqual(ts.mjd(DateTime.UTC), 54392.040196759262)

    def testBoundaryMJD(self):
        ts = DateTime(47892.0, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 631152000000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 631152025000000000L)
        self.assertEqual(ts.mjd(DateTime.UTC), 47892.0)

    def testCrossBoundaryNsecs(self):
        ts = DateTime(631151998000000000L, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 631151998000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 631152022000000000L)

    def testNsecsTAI(self):
        ts = DateTime(1192755506000000000L, DateTime.TAI)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1192755473000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 1192755506000000000L)
        self.assertEqual(ts.nsecs(), 1192755506000000000L)
        self.assertAlmostEqual(ts.mjd(DateTime.UTC), 54392.040196759262)

    def testNsecsDefault(self):
        ts = DateTime(1192755506000000000L)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1192755473000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 1192755506000000000L)
        self.assertEqual(ts.nsecs(), 1192755506000000000L)
        self.assertAlmostEqual(ts.mjd(DateTime.UTC), 54392.040196759262)

    def testNow(self):
        for i in xrange(100):
            secs = time.time()
            ts = DateTime.now()
            diff = ts.nsecs(DateTime.UTC) / 1.0e9 - secs 
            self.assert_(diff >= 0)
            self.assert_(diff < 1e-3)

if __name__ == '__main__':
    unittest.main()

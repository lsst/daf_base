#!/usr/bin/env python

import unittest

from lsst.daf.base import DateTime
import lsst.pex.exceptions as pexExcept
import time

class DateTimeTestCase(unittest.TestCase):
    """A test case for DateTime."""

    def testMJD(self):
        ts = DateTime(45205.125, DateTime.MJD, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 399006000000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 399006021000000000L)
        self.assertAlmostEqual(ts.getDate(DateTime.MJD, DateTime.UTC), 45205.125)
        self.assertAlmostEqual(ts.getDate(DateTime.MJD, DateTime.TAI),
                45205.125 + 21.0 / 86400.0)

    def testNsecs(self):
        ts = DateTime(1192755473000000000L, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1192755473000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 1192755506000000000L)
        self.assertEqual(ts.nsecs(), 1192755506000000000L)
        self.assertAlmostEqual(ts.getDate(DateTime.MJD, DateTime.UTC), 54392.040196759262)

    def testBoundaryMJD(self):
        ts = DateTime(47892.0, DateTime.MJD, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 631152000000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 631152025000000000L)
        self.assertEqual(ts.getDate(DateTime.MJD, DateTime.UTC), 47892.0)

    def testCrossBoundaryNsecs(self):
        ts = DateTime(631151998000000000L, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), 631151998000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 631152022000000000L)

    def testNsecsTAI(self):
        ts = DateTime(1192755506000000000L, DateTime.TAI)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1192755473000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 1192755506000000000L)
        self.assertEqual(ts.nsecs(), 1192755506000000000L)
        self.assertAlmostEqual(ts.getDate(DateTime.MJD, DateTime.UTC), 54392.040196759262)

    def testNsecsDefault(self):
        ts = DateTime(1192755506000000000L)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1192755473000000000L)
        self.assertEqual(ts.nsecs(DateTime.TAI), 1192755506000000000L)
        self.assertEqual(ts.nsecs(), 1192755506000000000L)
        self.assertAlmostEqual(ts.getDate(DateTime.MJD, DateTime.UTC), 54392.040196759262)

    def testNow(self):
        for i in xrange(100):
            secs = time.time()
            ts = DateTime.now()
            diff = ts.nsecs(DateTime.UTC) / 1.0e9 - secs 
            self.assertAlmostEqual(diff, 0, places=3)

    def testIsoEpoch(self):
        ts = DateTime("19700101T000000Z")
        self.assertEqual(ts.nsecs(DateTime.UTC), 0L)
        self.assertEqual(ts.toString(), "1970-01-01T00:00:00.000000000Z")

    def testIsoBasic(self):
        ts = DateTime("20090402T072639.314159265Z")
        self.assertEqual(ts.nsecs(DateTime.TAI), 1238657233314159265L)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1238657199314159265L)
        self.assertEqual(ts.toString(), "2009-04-02T07:26:39.314159265Z")

    def testIsoExpanded(self):
        ts = DateTime("2009-04-02T07:26:39.314159265Z")
        self.assertEqual(ts.nsecs(DateTime.TAI), 1238657233314159265L)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1238657199314159265L)
        self.assertEqual(ts.toString(), "2009-04-02T07:26:39.314159265Z")

    def testIsoNoNSecs(self):
        ts = DateTime("2009-04-02T07:26:39Z")
        self.assertEqual(ts.nsecs(DateTime.TAI), 1238657233000000000L)
        self.assertEqual(ts.nsecs(DateTime.UTC), 1238657199000000000L)
        self.assertEqual(ts.toString(), "2009-04-02T07:26:39.000000000Z")

    def testIsoThrow(self):
        self.assertRaises(pexExcept.LsstCppException, lambda: DateTime("20090401"))
        self.assertRaises(pexExcept.LsstCppException, lambda: DateTime("20090401T"))
        self.assertRaises(pexExcept.LsstCppException, lambda: DateTime("2009-04-01T"))
        self.assertRaises(pexExcept.LsstCppException, lambda: DateTime("2009-04-01T23:36:05"))
        self.assertRaises(pexExcept.LsstCppException, lambda: DateTime("20090401T23:36:05-0700"))
        self.assertRaises(pexExcept.LsstCppException, lambda: DateTime("2009/04/01T23:36:05Z"))
        self.assertRaises(pexExcept.LsstCppException, lambda: DateTime("2009/04/01T23:36:05Z"))

if __name__ == '__main__':
    unittest.main()

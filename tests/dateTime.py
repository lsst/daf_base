#!/usr/bin/env python

#
# LSST Data Management System
# Copyright 2008, 2009, 2010 LSST Corporation.
#
# This product includes software developed by the
# LSST Project (http://www.lsst.org/).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the LSST License Statement and
# the GNU General Public License along with this program.  If not,
# see <http://www.lsstcorp.org/LegalNotices/>.
#

from __future__ import division
import unittest
from builtins import range
from past.builtins import long

from lsst.daf.base import DateTime
import lsst.pex.exceptions as pexExcept
import os
import time


class DateTimeTestCase(unittest.TestCase):
    """A test case for DateTime."""

    def testMJD(self):
        ts = DateTime(45205.125, DateTime.MJD, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), long(399006000000000000))
        self.assertEqual(ts.nsecs(DateTime.TAI), long(399006021000000000))
        self.assertAlmostEqual(ts.get(DateTime.MJD, DateTime.UTC), 45205.125)
        self.assertAlmostEqual(ts.get(DateTime.MJD, DateTime.TAI), 45205.125 + 21.0/86400.0)

    def testLeapSecond(self):
        trials = ((45205., 21),
                  (41498.99, 10),
                  (41499.01, 11),
                  (57203.99, 35),
                  (57204.01, 36),
                  (57000., 35),
                  (57210., 36))
        for mjd, diff in trials:
            ts = DateTime(mjd, DateTime.MJD, DateTime.UTC)
            delta = ts.nsecs(DateTime.TAI) - ts.nsecs(DateTime.UTC)
            self.assertEqual(delta/1E9, diff)

    def testNsecs(self):
        ts = DateTime(long(1192755473000000000), DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), long(1192755473000000000))
        self.assertEqual(ts.nsecs(DateTime.TAI), long(1192755506000000000))
        self.assertEqual(ts.nsecs(), long(1192755506000000000))
        self.assertAlmostEqual(ts.get(DateTime.MJD, DateTime.UTC), 54392.040196759262)
        ts2 = ts
        self.assertEqual(ts, ts2)
        ts2 = DateTime(long(1192755473000000000), DateTime.UTC)
        self.assertEqual(ts, ts2)
        ts2 = DateTime(long(1234567890000000000), DateTime.UTC)
        self.assertNotEqual(ts, ts2)

    def testBoundaryMJD(self):
        ts = DateTime(47892.0, DateTime.MJD, DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), long(631152000000000000))
        self.assertEqual(ts.nsecs(DateTime.TAI), long(631152025000000000))
        self.assertEqual(ts.get(DateTime.MJD, DateTime.UTC), 47892.0)

    def testCrossBoundaryNsecs(self):
        ts = DateTime(long(631151998000000000), DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), long(631151998000000000))
        self.assertEqual(ts.nsecs(DateTime.TAI), long(631152022000000000))

    def testNsecsTAI(self):
        ts = DateTime(long(1192755506000000000), DateTime.TAI)
        self.assertEqual(ts.nsecs(DateTime.UTC), long(1192755473000000000))
        self.assertEqual(ts.nsecs(DateTime.TAI), long(1192755506000000000))
        self.assertEqual(ts.nsecs(), long(1192755506000000000))
        self.assertAlmostEqual(ts.get(DateTime.MJD, DateTime.UTC), 54392.040196759262)

    def testNsecsDefault(self):
        ts = DateTime(long(1192755506000000000))
        self.assertEqual(ts.nsecs(DateTime.UTC), long(1192755473000000000))
        self.assertEqual(ts.nsecs(DateTime.TAI), long(1192755506000000000))
        self.assertEqual(ts.nsecs(), long(1192755506000000000))
        self.assertAlmostEqual(ts.get(DateTime.MJD, DateTime.UTC), 54392.040196759262)

    def testNow(self):
        successes = 0
        for _ in range(10):
            secs = time.time()
            ts = DateTime.now()
            diff = ts.nsecs(DateTime.UTC)/1.0e9 - secs
            if diff > -0.001 and diff < 0.1:
                successes += 1
        self.assertGreaterEqual(successes, 3)

    def testIsoEpoch(self):
        ts = DateTime("19700101T000000Z", DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.UTC), long(0))
        self.assertEqual(ts.toString(ts.UTC), "1970-01-01T00:00:00.000000000Z")

    def testIsoUTCBasic(self):
        """Test basic ISO string input and output of UTC dates"""
        for dateSep in ("-", ""):  # "-" date separator is optional
            for timeSep in (":", ""):  # ":" time separator is optional
                for decPt in (".", ","):  # "." or "," may be used as decimal point
                    dateStr = "2009{0}04{0}02T07{1}26{1}39{2}314159265Z".format(dateSep, timeSep, decPt)
                    ts = DateTime(dateStr, DateTime.UTC)
                    self.assertEqual(ts.nsecs(DateTime.TT), long(1238657265498159265))
                    self.assertEqual(ts.nsecs(DateTime.TAI), long(1238657233314159265))
                    self.assertEqual(ts.nsecs(DateTime.UTC), long(1238657199314159265))
                    self.assertEqual(ts.toString(ts.UTC), "2009-04-02T07:26:39.314159265Z")

    def testIsoNonUTCBasics(self):
        """Test basic ISO string input and output of TAI and TT dates"""
        for scale in (DateTime.TAI, DateTime.TT):
            for dateSep in ("-", ""):  # "-" date separator is optional
                for timeSep in (":", ""):  # ":" time separator is optional
                    for decPt in (".", ","):  # "." or "," may be used as decimal point
                        dateStr = "2009{0}04{0}02T07{1}26{1}39{2}314159265".format(dateSep, timeSep, decPt)
                        ts = DateTime(dateStr, scale)
                        self.assertEqual(ts.toString(scale), "2009-04-02T07:26:39.314159265")

    def testIsoExpanded(self):
        ts = DateTime("2009-04-02T07:26:39.314159265Z", DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.TAI), long(1238657233314159265))
        self.assertEqual(ts.nsecs(DateTime.UTC), long(1238657199314159265))
        self.assertEqual(ts.toString(ts.UTC), "2009-04-02T07:26:39.314159265Z")

    def testIsoNoNSecs(self):
        ts = DateTime("2009-04-02T07:26:39Z", DateTime.UTC)
        self.assertEqual(ts.nsecs(DateTime.TAI), long(1238657233000000000))
        self.assertEqual(ts.nsecs(DateTime.UTC), long(1238657199000000000))
        self.assertEqual(ts.toString(ts.UTC), "2009-04-02T07:26:39.000000000Z")

    def testIsoThrow(self):
        with self.assertRaises(pexExcept.DomainError):
            DateTime("2009-04-01T23:36:05", DateTime.UTC)  # Z time zone required for UTC
        for scale in (DateTime.TAI, DateTime.TT):
            with self.assertRaises(pexExcept.DomainError):
                DateTime("2009-04-01T23:36:05Z", scale)  # Z time zone forbidden for TAI or TT

        for scale in (DateTime.TAI, DateTime.TT, DateTime.UTC):
            with self.assertRaises(pexExcept.DomainError):
                DateTime("20090401", scale)  # time required
            with self.assertRaises(pexExcept.DomainError):
                DateTime("20090401T", DateTime.UTC)  # time required
            with self.assertRaises(pexExcept.DomainError):
                DateTime("2009-04-01T", DateTime.UTC)  # time required
            with self.assertRaises(pexExcept.DomainError):
                DateTime("2009-04-01T23:36:05-0700", DateTime.UTC)  # time zone offset not supported
            with self.assertRaises(pexExcept.DomainError):
                DateTime("2009/04/01T23:36:05Z", DateTime.UTC)  # "/" not valid
            with self.assertRaises(pexExcept.DomainError):
                DateTime("2009-04-01T23:36", DateTime.UTC)  # partial time
            with self.assertRaises(pexExcept.DomainError):
                DateTime("2009-04", DateTime.UTC)  # partial date without time
            with self.assertRaises(pexExcept.DomainError):
                DateTime("2009-04T23:36.05", DateTime.UTC)  # partial date with time
            with self.assertRaises(pexExcept.DomainError):
                DateTime("09-04-01T23:36:05", DateTime.UTC)  # 2 digit year

    def testStr(self):
        timeStr1 = "2004-03-01T12:39:45.1"
        fullTimeStr1 = "2004-03-01T12:39:45.100000000"
        dt1 = DateTime(timeStr1, DateTime.TAI)
        self.assertEqual(str(dt1), "DateTime(\"{}\", TAI)".format(fullTimeStr1))
        self.assertEqual(repr(dt1), "DateTime(\"{}\", TAI)".format(fullTimeStr1))

        timeStr2 = "2004-03-01T12:39:45.000000001"
        dt2 = DateTime(timeStr2, DateTime.TAI)
        self.assertEqual(str(dt2), "DateTime(\"{}\", TAI)".format(timeStr2))
        self.assertEqual(repr(dt2), "DateTime(\"{}\", TAI)".format(timeStr2))

    def testNsecsTT(self):
        ts = DateTime(long(1192755538184000000), DateTime.TT)
        self.assertEqual(ts.nsecs(DateTime.UTC), long(1192755473000000000))
        self.assertEqual(ts.nsecs(DateTime.TAI), long(1192755506000000000))
        self.assertEqual(ts.nsecs(), long(1192755506000000000))
        self.assertAlmostEqual(ts.get(DateTime.MJD, DateTime.UTC), 54392.040196759262)

    def testFracSecs(self):
        ts = DateTime("2004-03-01T12:39:45.1Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '2004-03-01T12:39:45.100000000Z')
        ts = DateTime("2004-03-01T12:39:45.01Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '2004-03-01T12:39:45.010000000Z')
        ts = DateTime("2004-03-01T12:39:45.000000001Z", DateTime.UTC)  # nanosecond
        self.assertEqual(ts.toString(ts.UTC), '2004-03-01T12:39:45.000000001Z')
        ts = DateTime("2004-03-01T12:39:45.0000000001Z", DateTime.UTC)  # too small
        self.assertEqual(ts.toString(ts.UTC), '2004-03-01T12:39:45.000000000Z')

    def testNegative(self):
        ts = DateTime("1969-03-01T00:00:32Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-03-01T00:00:32.000000000Z')
        ts = DateTime("1969-01-01T00:00:00Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-01-01T00:00:00.000000000Z')
        ts = DateTime("1969-01-01T00:00:40Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-01-01T00:00:40.000000000Z')
        ts = DateTime("1969-01-01T00:00:38Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-01-01T00:00:38.000000000Z')
        ts = DateTime("1969-03-01T12:39:45Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-03-01T12:39:45.000000000Z')
        ts = DateTime("1969-03-01T12:39:45.000000001Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-03-01T12:39:45.000000001Z')

        # Note slight inaccuracy in UTC-TAI-UTC round-trip
        ts = DateTime("1969-03-01T12:39:45.12345Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-03-01T12:39:45.123449996Z')
        ts = DateTime("1969-03-01T12:39:45.123456Z", DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-03-01T12:39:45.123455996Z')

        ts = DateTime()
        self.assertEqual(ts.toString(ts.UTC), '1969-12-31T23:59:51.999918240Z')

        ts = DateTime(long(-1), DateTime.TAI)
        self.assertEqual(ts.toString(ts.UTC), '1969-12-31T23:59:51.999918239Z')
        ts = DateTime(long(0), DateTime.TAI)
        self.assertEqual(ts.toString(ts.UTC), '1969-12-31T23:59:51.999918240Z')
        ts = DateTime(long(1), DateTime.TAI)
        self.assertEqual(ts.toString(ts.UTC), '1969-12-31T23:59:51.999918241Z')

        ts = DateTime(long(-1), DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1969-12-31T23:59:59.999999999Z')
        ts = DateTime(long(0), DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1970-01-01T00:00:00.000000000Z')
        ts = DateTime(long(1), DateTime.UTC)
        self.assertEqual(ts.toString(ts.UTC), '1970-01-01T00:00:00.000000001Z')

    def testConvert(self):
        year = 2012
        month = 7
        day = 19
        hour = 18
        minute = 29
        second = 33

        ts = DateTime(year, month, day, hour, minute, second, DateTime.UTC)
        dt = ts.toPython(DateTime.UTC)

        self.assertEqual(dt.year, year)
        self.assertEqual(dt.month, month)
        self.assertEqual(dt.day, day)
        self.assertEqual(dt.hour, hour)
        self.assertEqual(dt.minute, minute)
        self.assertEqual(dt.second, second)


class TimeZoneBaseTestCase(DateTimeTestCase):
    timezone = ""

    def setUp(self):
        self.tz = os.environ.setdefault('TZ', "")
        os.environ['TZ'] = self.timezone

    def tearDown(self):
        if self.tz == "":
            del os.environ['TZ']
        else:
            os.environ['TZ'] = self.tz


class BritishTimeTestCase(TimeZoneBaseTestCase):
    timezone = "Europe/London"


class BritishTime2TestCase(TimeZoneBaseTestCase):
    timezone = "GMT0BST"


class PacificTimeTestCase(TimeZoneBaseTestCase):
    timezone = "PST8PDT"


if __name__ == '__main__':
    unittest.main()

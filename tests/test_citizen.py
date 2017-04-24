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
from __future__ import absolute_import, division, print_function
import unittest

import lsst.utils.tests
from lsst.daf.base import Citizen, PropertyList


class CitizenTestCase(lsst.utils.tests.TestCase):

    def testId(self):
        a = PropertyList()
        b = PropertyList()
        self.assertEqual(b.getId(), a.getId() + 1)

    def testNextMemIdAndCensus(self):
        memId0 = Citizen.getNextMemId()
        self.assertEqual(Citizen.census(0, memId0), 0)

        a = PropertyList()
        self.assertEqual(Citizen.census(0, memId0), 1)
        self.assertEqual(a.census(0, memId0), 1)

        memId1 = a.getNextMemId()
        b = PropertyList()
        self.assertEqual(Citizen.census(0, memId0), 2)
        self.assertEqual(a.census(0, memId0), 2)
        self.assertEqual(Citizen.census(0, memId1), 1)
        self.assertEqual(a.census(0, memId1), 1)

        del b
        self.assertEqual(Citizen.census(0, memId0), 1)
        self.assertEqual(a.census(0, memId0), 1)
        self.assertEqual(Citizen.census(0, memId1), 0)
        self.assertEqual(a.census(0, memId1), 0)

        del a
        self.assertEqual(Citizen.census(0, memId0), 0)


class TestMemory(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()


if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()

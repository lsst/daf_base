# This file is part of daf_base
#
# Developed for the LSST Data Management System.
# This product includes software developed by the LSST Project
# (http://www.lsst.org/).
# See the COPYRIGHT file at the top-level directory of this distribution
# for details of code ownership.
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
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Test YAML serialization"""

import os
import unittest

try:
    import yaml
except ImportError:
    yaml = None

import lsst.daf.base

TESTDIR = os.path.abspath(os.path.dirname(__file__))


class YAMLTestCase(unittest.TestCase):

    @unittest.skipIf(yaml is None, "yaml module not installed")
    def setUp(self):
        pass

    def assertEqualPL(self, pl1, pl2):
        self.assertEqual(pl1, pl2)
        self.assertEqual(len(pl1), len(pl2))
        for name in pl1.getOrderedNames():
            self.assertEqual(pl1.getArray(name), pl2.getArray(name))
            self.assertEqual(pl1.getScalar(name), pl2.getScalar(name))
            self.assertEqual(pl1.typeOf(name), pl2.typeOf(name))

    def assertEqualPS(self, ps1, ps2):
        self.assertEqual(ps1, ps2)
        self.assertEqual(len(ps1), len(ps2))
        self.assertEqual(ps1.nameCount(), ps2.nameCount())
        self.assertEqual(set(ps1.paramNames(False)), set(ps2.paramNames(False)))
        for name in ps1.paramNames(False):
            self.assertEqual(ps1.getArray(name), ps2.getArray(name))
            self.assertEqual(ps1.getScalar(name), ps2.getScalar(name))
            self.assertEqual(ps1.typeOf(name), ps2.typeOf(name))

    def testYamlPS(self):
        ps = lsst.daf.base.PropertySet()
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        ps.setLongLong("int64_t", 0xfeeddeadbeef)
        ps.setFloat("float", 3.14159)
        ps.setDouble("double", 2.718281828459045)
        ps.set("char*", "foo")
        ps.setString("string", "bar")
        ps.set("char*", u"foo")
        ps.setString("string", u"bar")
        ps.set("int2", 2009)
        ps.set("dt", lsst.daf.base.DateTime("20090402T072639.314159265Z", lsst.daf.base.DateTime.UTC))
        ps.set("blank", "")

        ps2 = yaml.load(yaml.dump(ps))
        self.assertIsInstance(ps2, lsst.daf.base.PropertySet)
        self.assertEqualPS(ps, ps2)
        print(ps2)
        print(f"Length: {len(ps2)}")
        for n in ps2:
            print(f"N is {n}")

    def testYamlPL(self):
        apl = lsst.daf.base.PropertyList()
        apl.setBool("bool", True)
        apl.setShort("short", 42)
        apl.setInt("int", 2008)
        apl.setLongLong("int64_t", 0xfeeddeadbeef)
        apl.setFloat("float", 3.14159)
        apl.setDouble("double", 2.718281828459045)
        apl.set("char*", "foo")
        apl.setString("string", "bar")
        apl.set("int2", 2009)
        apl.set("dt", lsst.daf.base.DateTime("20090402T072639.314159265Z", lsst.daf.base.DateTime.UTC))

        apl2 = yaml.load(yaml.dump(apl))
        self.assertIsInstance(apl2, lsst.daf.base.PropertyList)
        self.assertEqualPL(apl, apl2)
        print(apl2)
        print(repr(apl2))
        print(f"Length: {len(apl2)}")
        for n, v in apl2.items():
            print(f"Npl is {n}: {v}")

    def testYamlNest(self):
        """Test nested property sets"""
        ps = lsst.daf.base.PropertySet()
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)

        ps2 = lsst.daf.base.PropertySet()
        ps2.setString("string", "foo")
        ps2.setString("string2", "bar")

        ps.setPropertySet("ps", ps2)

        ps3 = yaml.load(yaml.dump(ps))
        self.assertEqualPS(ps3, ps)
        self.assertEqualPS(ps3.getPropertySet("ps"), ps.getPropertySet("ps"))

        # Now for a PropertyList
        apl = lsst.daf.base.PropertyList()
        apl.setBool("bool", True)
        apl.setShort("short", 42)
        apl.setInt("int", 2008)
        apl.add("withcom", "string", "a comment")
        apl.setPropertySet("ps", ps3)

        apl2 = yaml.load(yaml.dump(apl))
        self.assertEqualPL(apl2, apl)

    def testYamlDateTime(self):
        ts = lsst.daf.base.DateTime("2004-03-01T12:39:45.1Z", lsst.daf.base.DateTime.UTC)
        ts2 = yaml.load(yaml.dump(ts))
        self.assertIsInstance(ts2, lsst.daf.base.DateTime)
        self.assertEqual(ts, ts2)

    def testLoader(self):
        """Test loading of reference YAML files"""
        # Old and new serialization of a propertyList
        with open(os.path.join(TESTDIR, "data", "fitsheader-tuple.yaml")) as fd:
            old = yaml.load(fd)
        with open(os.path.join(TESTDIR, "data", "fitsheader.yaml")) as fd:
            new = yaml.load(fd)
        self.assertIsInstance(new, lsst.daf.base.PropertyList)
        self.assertIsInstance(old, lsst.daf.base.PropertyList)

        # There is no __eq__
        self.assertEqualPL(old, new)


if __name__ == '__main__':
    unittest.main()

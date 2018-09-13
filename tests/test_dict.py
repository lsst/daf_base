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

"""Test dict emulation"""

import unittest
import copy

import lsst.daf.base


class DictTestCase(unittest.TestCase):

    def setUp(self):

        pl = lsst.daf.base.PropertyList()
        pl.setBool("bool", True)
        pl.setShort("short", 42)
        pl.setInt("int", 2008)
        pl.setLongLong("int64_t", 0xfeeddeadbeef)
        pl.setFloat("float", 3.14159)
        pl.setDouble("double", 2.718281828459045)
        pl.set("char*", "foo", "char comment")
        pl.setString("string", "bar")
        pl.set("int2", 2009, "int comment")
        pl.set("dt", lsst.daf.base.DateTime("20090402T072639.314159265Z", lsst.daf.base.DateTime.UTC))

        ps = lsst.daf.base.PropertySet()
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        ps.setLongLong("long", 5)  # Deliberately small
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
        ps.addInt("int", 2009)

        ps2 = lsst.daf.base.PropertySet()
        ps2.setBool("bool2", False)
        ps2.setShort("short2", 16)
        ps2.setInt("int2", 2018)

        ps.setPropertySet("ps2", ps2)
        pl.setPropertySet("ps2", ps2)

        self.ps = ps
        self.pl = pl

    def testCopyPropertySet(self):
        shallow = copy.copy(self.ps)
        self.assertIsInstance(shallow, lsst.daf.base.PropertySet)
        self.assertIn("ps2", shallow)
        self.assertEqual(shallow, self.ps)

        deep = copy.deepcopy(self.ps)
        self.assertIsInstance(deep, lsst.daf.base.PropertySet)
        self.assertEqual(deep, self.ps)
        del deep["ps2"]
        self.assertNotIn("ps2", deep)
        self.assertIn("ps2", self.ps)

    def testDictPropertySet(self):
        container = self.ps
        self.assertIn("string", container)
        self.assertIn("ps2", container)
        self.assertNotIn("ps2.bool2", container)
        self.assertIn("bool2", container.getScalar("ps2"))

        # Compare dict-like interface to pure dict version
        d = container.toDict()
        self.assertEqual(len(d), len(container))

        # Set some values
        container["new"] = "string"
        container["array"] = [1, 2, 3]
        container["dot.delimited"] = "delimited"
        self.assertIn("dot", container)

        # Assign a PropertySet
        ps2 = lsst.daf.base.PropertySet()
        ps2.setString("newstring", "stringValue")
        container["newps2"] = ps2
        ps2["newint"] = 5
        self.assertEqual(container.getScalar("newps2.newint"), ps2.getScalar("newint"))

        # Dict should be converted to a PropertySet
        container["dict"] = {"a": 1, "b": 2}
        self.assertEqual(container.getScalar("dict.b"), 2)

        container["a_property_list"] = self.pl

    def testDictPropertyList(self):
        container = self.pl
        self.assertIn("string", container)
        self.assertIn("ps2.bool2", container)

        # Set a comment
        container["int2#COMMENT"] = "new comment"
        self.assertEqual(container.getComment("int2"), "new comment")

        # Compare dict-like interface to pure dict version
        d = container.toDict()
        self.assertEqual(len(d), len(container))

        # Set some values
        container["new"] = "string"
        container["array"] = [1, 2, 3]
        container["dot.delimited"] = "delimited"

        # Assign a PropertySet
        ps2 = lsst.daf.base.PropertySet()
        ps2["newstring"] = "stringValue"
        container["newps2"] = ps2
        ps2["newint"] = 5  # This should have no effect on container
        self.assertEqual(container.getScalar("newps2.newstring"), ps2.getScalar("newstring"))
        self.assertNotIn("newps2.newinst", container)
        self.assertIn("newint", ps2)

        # Dict should be converted to a PropertySet
        container["dict"] = {"a": 1, "b": 2}
        self.assertEqual(container.getScalar("dict.b"), 2)


if __name__ == '__main__':
    unittest.main()

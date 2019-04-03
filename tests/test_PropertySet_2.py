#
# LSST Data Management System
#
# Copyright 2008-2017  AURA/LSST.
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

import pickle
import unittest

import numpy as np

import lsst.utils.tests
import lsst.daf.base as dafBase
import lsst.pex.exceptions as pexExcept


class PropertySetTestCase(unittest.TestCase):
    """A test case for PropertySet."""

    def testConstruct(self):
        ps = dafBase.PropertySet()
        self.assertIsNotNone(ps)

    def checkPickle(self, original):
        new = pickle.loads(pickle.dumps(original, 4))
        self.assertEqual(new, original)

    def testScalar(self):
        ps = dafBase.PropertySet()
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
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z", dafBase.DateTime.UTC))
        ps.set("blank", "")
        ps.set("undef", None)

        self.assertEqual(ps.typeOf("bool"), dafBase.PropertySet.TYPE_Bool)
        self.assertEqual(ps.getBool("bool"), True)
        self.assertEqual(ps.typeOf("short"), dafBase.PropertySet.TYPE_Short)
        self.assertEqual(ps.getShort("short"), 42)
        self.assertEqual(ps.typeOf("int"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.getInt("int"), 2008)
        self.assertEqual(ps.typeOf("int64_t"),
                         dafBase.PropertySet.TYPE_LongLong)
        self.assertEqual(ps.getLongLong("int64_t"), 0xfeeddeadbeef)
        self.assertEqual(ps.typeOf("float"), dafBase.PropertySet.TYPE_Float)
        self.assertAlmostEqual(ps.getFloat("float"), 3.14159, 6)
        self.assertEqual(ps.typeOf("double"), dafBase.PropertySet.TYPE_Double)
        self.assertEqual(ps.getDouble("double"), 2.718281828459045)
        self.assertEqual(ps.typeOf("char*"), dafBase.PropertySet.TYPE_String)
        self.assertEqual(ps.getString("char*"), "foo")
        self.assertEqual(ps.typeOf("string"), dafBase.PropertySet.TYPE_String)
        self.assertEqual(ps.getString("string"), "bar")
        self.assertEqual(ps.typeOf("int2"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.getInt("int2"), 2009)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("int2"), 2009)
        self.assertEqual(ps.getArray("int2"), [2009])
        self.assertEqual(ps.getScalar("int2"), 2009)
        self.assertEqual(ps.typeOf("dt"), dafBase.PropertySet.TYPE_DateTime)
        self.assertEqual(ps.getDateTime("dt").nsecs(), 1238657233314159265)
        self.assertEqual(ps.getString("blank"), "")

        self.assertIsNone(ps.getScalar("undef"))
        self.assertEqual(ps.typeOf("undef"), dafBase.PropertyList.TYPE_Undef)
        with self.assertWarns(DeprecationWarning):
            self.assertIsNone(ps.get("undef"))

        self.checkPickle(ps)

        # Now replace the undef value with a defined value
        ps.set("undef", "not undefined")
        self.assertEqual(ps.getScalar("undef"), "not undefined")
        self.assertFalse(ps.isUndefined("undef"))
        self.assertEqual(ps.typeOf("undef"), dafBase.PropertyList.TYPE_String)

    def testNumPyScalars(self):
        """Test that we can also pass NumPy array scalars to PropertySet setters.
        """
        ps = dafBase.PropertySet()
        ps.setShort("short", np.int16(42))
        ps.setInt("int", np.int32(2008))
        ps.setLongLong("int64_t", np.int64(0xfeeddeadbeef))
        ps.setFloat("float", np.float32(3.14159))
        ps.setDouble("double", np.float64(2.718281828459045))
        self.assertEqual(ps.typeOf("short"), dafBase.PropertySet.TYPE_Short)
        self.assertEqual(ps.getShort("short"), 42)
        self.assertEqual(ps.typeOf("int"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.getInt("int"), 2008)
        self.assertEqual(ps.typeOf("int64_t"),
                         dafBase.PropertySet.TYPE_LongLong)
        self.assertEqual(ps.getLongLong("int64_t"), 0xfeeddeadbeef)
        self.assertEqual(ps.typeOf("float"), dafBase.PropertySet.TYPE_Float)
        self.assertAlmostEqual(ps.getFloat("float"), 3.14159, 6)
        self.assertEqual(ps.typeOf("double"), dafBase.PropertySet.TYPE_Double)
        self.assertEqual(ps.getDouble("double"), 2.718281828459045)
        self.checkPickle(ps)

    def testGetDefault(self):
        ps = dafBase.PropertySet()
        ps.setInt("int", 42)
        self.assertEqual(ps.getInt("int"), 42)
        self.assertEqual(ps.getInt("int", 2008), 42)
        self.assertEqual(ps.getInt("foo", 2008), 2008)
        self.checkPickle(ps)

    def testExists(self):
        ps = dafBase.PropertySet()
        ps.setInt("int", 42)
        self.checkPickle(ps)
        self.assertEqual(ps.exists("int"), True)
        self.assertEqual(ps.exists("foo"), False)

    def testGetVector(self):
        ps = dafBase.PropertySet()
        v = [42, 2008, 1]
        ps.setInt("ints", v)
        ps.setInt("ints2", [10, 9, 8])
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        self.assertEqual(ps.getInt("ints2"), 8)
        self.assertEqual(ps.getArrayInt("ints2"), [10, 9, 8])
        self.assertEqual(ps.getArray("ints"), v)
        self.assertEqual(ps.getScalar("ints"), v[-1])
        ps.setInt("int", 999)
        with self.assertWarns(DeprecationWarning):
            x = ps.get("int")
        self.assertEqual(x, 999)
        self.assertEqual(ps.getArray("int"), [999])
        self.assertEqual(ps.getScalar("int"), 999)
        self.checkPickle(ps)

    def testGetVector2(self):
        ps = dafBase.PropertySet()
        v = [42, 2008, 1]
        ps.setInt("ints", v)
        ps.setInt("ints2", [10, 9, 8])
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v[0], w[0])
        self.assertEqual(v[1], w[1])
        self.assertEqual(v[2], w[2])
        self.assertEqual(ps.getInt("ints2"), 8)
        self.assertEqual(ps.getArrayInt("ints2"), [10, 9, 8])
        self.checkPickle(ps)

    def testAddScalar(self):
        ps = dafBase.PropertySet()
        v = [42, 2008, 1]
        ps.setInt("ints", v)
        ps.addInt("ints", -999)
        ps.add("other", "foo")
        ps.add("ints", 13)
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 5)
        self.assertEqual(v[0], w[0])
        self.assertEqual(v[1], w[1])
        self.assertEqual(v[2], w[2])
        self.assertEqual(w[3], -999)
        self.assertEqual(w[4], 13)
        self.assertEqual(ps.getString("other"), "foo")
        self.checkPickle(ps)

    def testSetAddVector(self):
        ps = dafBase.PropertySet()
        boolArr = [True, False]
        intArr = [1, -1, 2]
        floatArr = [1.2, 99.5, -35.1]
        strArr = ["this", "is", "some strings"]
        ps.set("bools", boolArr)
        ps.set("ints", intArr)
        ps.set("floats", floatArr)
        ps.set("strs", strArr)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("bools"), boolArr)
        self.assertEqual(ps.getArray("bools"), boolArr)
        self.assertEqual(ps.getScalar("bools"), boolArr[-1])
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("ints"), intArr)
        self.assertEqual(ps.getArray("ints"), intArr)
        self.assertEqual(ps.getScalar("ints"), intArr[-1])
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("floats"), floatArr)
        self.assertEqual(ps.getArray("floats"), floatArr)
        self.assertEqual(ps.getScalar("floats"), floatArr[-1])
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("strs"), strArr)
        self.assertEqual(ps.getArray("strs"), strArr)
        self.assertEqual(ps.getScalar("strs"), strArr[-1])

        ps.add("bools", list(reversed(boolArr)))
        ps.add("ints", list(reversed(intArr)))
        ps.add("floats", list(reversed(floatArr)))
        ps.add("strs", list(reversed(strArr)))
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("bools"), boolArr + list(reversed(boolArr)))
        self.assertEqual(ps.getArray("bools"), boolArr + list(reversed(boolArr)))
        self.assertEqual(ps.getScalar("bools"), boolArr[0])
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("ints"), intArr + list(reversed(intArr)))
        self.assertEqual(ps.getArray("ints"), intArr + list(reversed(intArr)))
        self.assertEqual(ps.getScalar("ints"), intArr[0])
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("floats"), floatArr + list(reversed(floatArr)))
        self.assertEqual(ps.getArray("floats"), floatArr + list(reversed(floatArr)))
        self.assertEqual(ps.getScalar("floats"), floatArr[0])
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("strs"), strArr + list(reversed(strArr)))
        self.assertEqual(ps.getArray("strs"), strArr + list(reversed(strArr)))
        self.assertEqual(ps.getScalar("strs"), strArr[0])
        self.checkPickle(ps)

    def testDateTimeToString(self):
        ps = dafBase.PropertySet()
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z", dafBase.DateTime.UTC))
        self.assertEqual(ps.toString(),
                         "dt = 2009-04-02T07:26:39.314159265Z\n")
        self.checkPickle(ps)

    def testGetScalarThrow(self):
        ps = dafBase.PropertySet()
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        with self.assertWarns(DeprecationWarning):
            with self.assertRaises(KeyError):
                ps.get("foo")
        self.checkPickle(ps)

    def testSubPS(self):
        ps = dafBase.PropertySet()
        ps1 = dafBase.PropertySet()
        ps1.set("a", 1)
        ps.setPropertySet("b", ps1)
        self.assertEqual(ps.getArray("b"), ps1)
        self.assertEqual(ps.getScalar("b"), ps1)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("b.a"), 1)
        self.assertEqual(ps.getArray("b.a"), [1])
        self.assertEqual(ps.getScalar("b.a"), 1)
        ps.set("c", ps1)
        self.assertEqual(ps.getArray("c"), ps1)
        self.assertEqual(ps.getScalar("c"), ps1)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("c.a"), 1)
        self.assertEqual(ps.getArray("c.a"), [1])
        self.assertEqual(ps.getScalar("c.a"), 1)
        ps.set("c.a", 2)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("b.a"), 2)
        self.assertEqual(ps.getArray("b.a"), [2])
        self.assertEqual(ps.getScalar("b.a"), 2)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("b").get("a"), 2)
        self.checkPickle(ps)

    def testCopy(self):
        dest = dafBase.PropertySet()
        source = dafBase.PropertySet()
        value1 = [1.5, 3.2]
        source.set("srcItem1", value1)
        dest.copy("destItem1", source, "srcItem1")
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(dest.get("destItem1"), value1)
        self.assertEqual(dest.getArray("destItem1"), value1)
        self.assertEqual(dest.getScalar("destItem1"), value1[-1])

        # items are replaced, regardless of type
        dest.set("destItem2", "string value")
        value2 = [5, -4, 3]
        source.set("srcItem2", value2)
        dest.copy("destItem2", source, "srcItem2")
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(dest.get("destItem2"), value2)
        self.assertEqual(dest.getArray("destItem2"), value2)
        self.assertEqual(dest.getScalar("destItem2"), value2[-1])

        # asScalar copies only the last value
        dest.copy("destItem2Scalar", source, "srcItem2", asScalar=True)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(dest.get("destItem2Scalar"), value2[-1])
        self.assertEqual(dest.getArray("destItem2Scalar"), [value2[-1]])
        self.assertEqual(dest.getScalar("destItem2Scalar"), value2[-1])


class FlatTestCase(unittest.TestCase):
    """A test case for flattened PropertySets."""

    def testConstruct(self):
        ps = dafBase.PropertySet(flat=True)
        self.assertIsNotNone(ps)

    def testScalar(self):
        ps = dafBase.PropertySet(flat=True)
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        ps.setLongLong("int64_t", 0xfeeddeadbeef)
        ps.setFloat("float", 3.14159)
        ps.setDouble("double", 2.718281828459045)
        ps.set("char*", "foo")
        ps.setString("string", "bar")
        ps.set("int2", 2009)
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z", dafBase.DateTime.UTC))
        ps.set("autobool", True)

        self.assertEqual(ps.typeOf("bool"), dafBase.PropertySet.TYPE_Bool)
        self.assertIs(ps.getBool("bool"), True)
        with self.assertWarns(DeprecationWarning):
            self.assertIs(ps.get("bool"), True)
        self.assertEqual(ps.getArray("bool"), [True])
        self.assertIs(ps.getScalar("bool"), True)
        self.assertEqual(ps.typeOf("short"), dafBase.PropertySet.TYPE_Short)
        self.assertEqual(ps.getShort("short"), 42)
        self.assertEqual(ps.typeOf("int"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.getInt("int"), 2008)
        self.assertEqual(ps.typeOf("int64_t"),
                         dafBase.PropertySet.TYPE_LongLong)
        self.assertEqual(ps.getLongLong("int64_t"), 0xfeeddeadbeef)
        self.assertEqual(ps.typeOf("float"), dafBase.PropertySet.TYPE_Float)
        self.assertAlmostEqual(ps.getFloat("float"), 3.14159, 6)
        self.assertEqual(ps.typeOf("double"), dafBase.PropertySet.TYPE_Double)
        self.assertEqual(ps.getDouble("double"), 2.718281828459045)
        self.assertEqual(ps.typeOf("char*"), dafBase.PropertySet.TYPE_String)
        self.assertEqual(ps.getString("char*"), "foo")
        self.assertEqual(ps.typeOf("string"), dafBase.PropertySet.TYPE_String)
        self.assertEqual(ps.getString("string"), "bar")
        self.assertEqual(ps.typeOf("int2"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.getInt("int2"), 2009)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("int2"), 2009)
        self.assertEqual(ps.getArray("int2"), [2009])
        self.assertEqual(ps.getScalar("int2"), 2009)
        self.assertEqual(ps.typeOf("dt"), dafBase.PropertySet.TYPE_DateTime)
        self.assertEqual(ps.getDateTime("dt").nsecs(), 1238657233314159265)
        self.assertEqual(ps.typeOf("autobool"), dafBase.PropertySet.TYPE_Bool)
        with self.assertWarns(DeprecationWarning):
            self.assertIs(ps.get("autobool"), True)
        self.assertEqual(ps.getArray("autobool"), [True])
        self.assertIs(ps.getScalar("autobool"), True)

    def testGetDefault(self):
        ps = dafBase.PropertySet(flat=True)
        ps.setInt("int", 42)
        self.assertEqual(ps.getInt("int"), 42)
        self.assertEqual(ps.getInt("int", 2008), 42)
        self.assertEqual(ps.getInt("foo", 2008), 2008)

    def testExists(self):
        ps = dafBase.PropertySet(flat=True)
        ps.setInt("int", 42)
        self.assertEqual(ps.exists("int"), True)
        self.assertEqual(ps.exists("foo"), False)

    def testGetVector(self):
        ps = dafBase.PropertySet(flat=True)
        v = [42, 2008, 1]
        ps.setInt("ints", v)
        ps.setInt("ints2", [10, 9, 8])
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        self.assertEqual(ps.getInt("ints2"), 8)
        self.assertEqual(ps.getArrayInt("ints2"), [10, 9, 8])
        self.assertEqual(ps.getArray("ints"), v)
        self.assertEqual(ps.getScalar("ints"), v[-1])
        ps.setInt("int", 999)
        with self.assertWarns(DeprecationWarning):
            x = ps.get("int")
        self.assertEqual(x, 999)
        self.assertEqual(ps.getArray("int"), [999])
        self.assertEqual(ps.getScalar("int"), 999)

    def testGetVector2(self):
        ps = dafBase.PropertySet(flat=True)
        v = [42, 2008, 1]
        ps.setInt("ints", v)
        ps.setInt("ints2", [10, 9, 8])
        w = ps.getArrayInt("ints")
        self.assertEqual(w, v)
        self.assertEqual(ps.getInt("ints2"), 8)
        self.assertEqual(ps.getArrayInt("ints2"), [10, 9, 8])

    def testAddScalar(self):
        ps = dafBase.PropertySet(flat=True)
        v = [42, 2008, 1]
        ps.setInt("ints", v)
        ps.addInt("ints", -999)
        ps.add("other", "foo")
        ps.add("ints", 13)
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 5)
        self.assertEqual(v[0], w[0])
        self.assertEqual(v[1], w[1])
        self.assertEqual(v[2], w[2])
        self.assertEqual(w[3], -999)
        self.assertEqual(w[4], 13)
        self.assertEqual(ps.getString("other"), "foo")

    def testDateTimeToString(self):
        ps = dafBase.PropertySet(flat=True)
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z", dafBase.DateTime.UTC))
        self.assertEqual(ps.toString(),
                         "dt = 2009-04-02T07:26:39.314159265Z\n")

    def testGetScalarThrow(self):
        ps = dafBase.PropertySet(flat=True)
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        with self.assertWarns(DeprecationWarning):
            with self.assertRaises(KeyError):
                ps.get("foo")

    def testSubPS(self):
        ps = dafBase.PropertySet(flat=True)
        ps1 = dafBase.PropertySet()
        ps1.set("a", 1)
        ps1.add("a", 2)
        ps1.set("foo", "bar")
        ps.setPropertySet("b", ps1)
        self.assertEqual(ps.exists("b.a"), True)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("b.a"), [1, 2])
        self.assertEqual(ps.getArray("b.a"), [1, 2])
        self.assertEqual(ps.getScalar("b.a"), 2)
        self.assertEqual(ps.exists("b"), False)
        self.assertEqual(ps.exists("b.foo"), True)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("b.foo"), "bar")
        self.assertEqual(ps.getArray("b.foo"), ["bar"])
        self.assertEqual(ps.getScalar("b.foo"), "bar")

        ps.set("b.c", 20)
        self.assertEqual(ps.exists("b.c"), True)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(ps.get("b.c"), 20)
        self.assertEqual(ps.getArray("b.c"), [20])
        self.assertEqual(ps.getScalar("b.c"), 20)
        self.assertEqual(ps.exists("b"), False)

    def testCombine(self):
        ps = dafBase.PropertySet()
        ps.set("ps1.pre", 1)
        ps.set("ps1.pre", 1)
        ps.set("ps1.post", 2)
        ps.set("int", 42)
        ps.set("double", 3.14)
        ps.set("ps2.plus", 10.24)
        ps.set("ps2.minus", -10.24)
        ps.set("ps3.sub.subsub", "foo")

        psp = dafBase.PropertySet()
        psp.set("ps1.pre", 3)
        psp.add("ps1.pre", 4)
        psp.set("int", 2008)
        psp.set("ps2.foo", "bar")
        psp.set("ps4.top", "bottom")

        ps.combine(psp)

        self.assertIsInstance(ps.getScalar("ps1"), dafBase.PropertySet)
        self.assertIsInstance(ps.getScalar("ps2"), dafBase.PropertySet)
        self.assertIsInstance(ps.getScalar("ps3"), dafBase.PropertySet)
        self.assertIsInstance(ps.getScalar("ps3.sub"), dafBase.PropertySet)
        self.assertIsInstance(ps.getScalar("ps4"), dafBase.PropertySet)

        self.assertFalse(ps.isArray("ps1"))
        self.assertTrue(ps.isArray("ps1.pre"))
        self.assertFalse(ps.isArray("ps1.post"))
        self.assertFalse(ps.isArray("ps2"))
        self.assertFalse(ps.isArray("ps2.plus"))
        self.assertFalse(ps.isArray("ps2.minus"))
        self.assertFalse(ps.isArray("ps2.foo"))
        self.assertFalse(ps.isArray("ps3"))
        self.assertFalse(ps.isArray("ps3.sub"))
        self.assertFalse(ps.isArray("ps3.subsub"))
        self.assertFalse(ps.isArray("ps4"))
        self.assertFalse(ps.isArray("ps4.top"))
        self.assertTrue(ps.isArray("int"))
        self.assertFalse(ps.isArray("double"))

        self.assertEqual(ps.valueCount("ps1.pre"), 3)
        self.assertEqual(ps.valueCount("int"), 2)

        v = ps.getArray("ps1.pre")
        self.assertEqual(v, [1, 3, 4])
        v = ps.getArray("int")
        self.assertEqual(v, [42, 2008])

    def testCombineThrow(self):
        ps = dafBase.PropertySet()
        ps.set("int", 42)

        psp = dafBase.PropertySet()
        psp.set("int", 3.14159)

        with self.assertRaises(TypeError):
            ps.combine(psp)

    def testToDict(self):
        ps = dafBase.PropertySet()
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        ps.setLongLong("int64_t", 0xfeeddeadbeef)
        ps.setInt("ints", [10, 9, 8])

        ps2 = dafBase.PropertySet()
        ps2.set("ps", ps)
        ps2.setFloat("float", 3.14159)
        ps2.setDouble("double", 2.718281828459045)
        ps2.set("char*", "foo")
        ps2.setString("string", "bar")
        ps2.set("dt", dafBase.DateTime("20090402T072639.314159265Z", dafBase.DateTime.UTC))
        ps2.set("undef", None)

        d = ps2.toDict()
        self.assertIsInstance(d, dict)

        self.assertIsInstance(d["float"], float)
        self.assertAlmostEqual(d["float"], 3.14159, 6)
        self.assertIsInstance(d["double"], float)
        self.assertEqual(d["double"], 2.718281828459045)
        self.assertIsNone(d["undef"])

        self.assertIsInstance(d["char*"], str)
        self.assertEqual(d["char*"], "foo")
        self.assertIsInstance(d["string"], str)
        self.assertEqual(d["string"], "bar")
        self.assertIsInstance(d["dt"], dafBase.DateTime)
        self.assertEqual(d["dt"].nsecs(), 1238657233314159265)

        d2 = d["ps"]
        self.assertIsInstance(d2, dict)

        self.assertIsInstance(d2["bool"], bool)
        self.assertEqual(d2["bool"], True)
        self.assertIsInstance(d2["short"], (int, int))
        self.assertEqual(d2["short"], 42)
        self.assertIsInstance(d2["int"], (int, int))
        self.assertEqual(d2["int"], 2008)
        self.assertIsInstance(d2["int64_t"], (int, int))
        self.assertEqual(d2["int64_t"], 0xfeeddeadbeef)
        self.assertIsInstance(d2["ints"], list)
        self.assertIsInstance(d2["ints"][0], (int, int))
        self.assertEqual(d2["ints"], [10, 9, 8])

    def testAddVector(self):
        ps = dafBase.PropertySet()
        v = [42, 2008, 1]
        ps.set("ints", v)

        vv = [-42, -2008, -1]
        ps.add("ints", vv)

        w = ps.getArray("ints")
        self.assertEqual(w, v + vv)

    def testArrayProperties(self):
        ps = dafBase.PropertySet()
        v = [42, 2008, 1]
        ps.set("ints", v)
        ps.set("int", 365)
        ps.set("ints2", -42)
        ps.add("ints2", -2008)

        self.assertTrue(ps.isArray("ints"))
        self.assertFalse(ps.isArray("int"))
        self.assertTrue(ps.isArray("ints2"))
        self.assertEqual(ps.valueCount("ints"), 3)
        self.assertEqual(ps.valueCount("int"), 1)
        self.assertEqual(ps.valueCount("ints2"), 2)
        self.assertEqual(ps.typeOf("ints"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.typeOf("int"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.typeOf("ints2"), dafBase.PropertySet.TYPE_Int)

    def testHierarchy(self):
        ps = dafBase.PropertySet()
        psp = dafBase.PropertySet()

        psp.set("pre", 1)
        ps.set("ps1", psp)
        psp.set("post", 2)
        ps.set("int", 42)
        ps.set("ps2", dafBase.PropertySet())
        ps.getPropertySet("ps2").set("plus", 10.24)
        ps.set("ps2.minus", -10.24)
        ps.set("ps3.sub1", "foo")
        ps.set("ps3.sub2", "bar")

        self.assertTrue(ps.exists("ps1"))
        self.assertTrue(ps.exists("ps2"))
        self.assertTrue(ps.exists("ps3"))
        self.assertTrue(ps.exists("ps1.pre"))
        self.assertTrue(ps.exists("ps1.post"))
        self.assertTrue(ps.exists("ps2.plus"))
        self.assertTrue(ps.exists("ps2.minus"))
        self.assertTrue(ps.exists("ps3.sub1"))
        self.assertTrue(ps.exists("ps3.sub2"))

        self.assertTrue(ps.isPropertySetPtr("ps1"))
        self.assertTrue(ps.isPropertySetPtr("ps2"))
        self.assertTrue(ps.isPropertySetPtr("ps3"))
        self.assertFalse(ps.isPropertySetPtr("int"))
        self.assertFalse(ps.isPropertySetPtr("ps1.pre"))
        self.assertFalse(ps.isPropertySetPtr("ps1.post"))
        self.assertFalse(ps.isPropertySetPtr("ps2.plus"))
        self.assertFalse(ps.isPropertySetPtr("ps2.minus"))
        self.assertFalse(ps.isPropertySetPtr("ps3.sub1"))
        self.assertFalse(ps.isPropertySetPtr("ps3.sub2"))

        psp1 = ps.getPropertySet("ps1")
        psp2 = ps.getPropertySet("ps2")
        psp3 = ps.getPropertySet("ps3")
        self.assertIsInstance(psp1, dafBase.PropertySet)
        self.assertIsInstance(psp2, dafBase.PropertySet)
        self.assertIsInstance(psp3, dafBase.PropertySet)
        self.assertEqual(psp1, psp)
        self.assertTrue(psp1.exists("pre"))
        self.assertTrue(psp1.exists("post"))
        self.assertTrue(psp2.exists("plus"))
        self.assertTrue(psp2.exists("minus"))
        self.assertTrue(psp3.exists("sub1"))
        self.assertTrue(psp3.exists("sub2"))
        self.assertEqual(psp1.getAsInt("pre"), 1)
        self.assertEqual(psp1.getAsInt("post"), 2)
        self.assertEqual(psp2.getAsDouble("plus"), 10.24)
        self.assertEqual(psp2.getAsDouble("minus"), -10.24)
        self.assertEqual(psp3.getAsString("sub1"), "foo")
        self.assertEqual(psp3.getAsString("sub2"), "bar")

        # Make sure checking a subproperty doesn't create it.
        self.assertFalse(ps.exists("ps2.pre"))
        self.assertFalse(ps.exists("ps2.pre"))
        # Make sure checking an element doesn't create it.
        self.assertFalse(ps.exists("ps4"))
        self.assertFalse(ps.exists("ps4"))
        # Make sure checking a subproperty with a nonexistent parent doesn't
        # create it.
        self.assertFalse(ps.exists("ps4.sub"))
        self.assertFalse(ps.exists("ps4.sub"))
        # Make sure checking a subproperty doesn't create its parent.
        self.assertFalse(ps.exists("ps4"))

#
    def testvariousThrows(self):
        ps = dafBase.PropertySet()
        ps.set("int", 42)
        with self.assertRaises(pexExcept.InvalidParameterError):
            ps.set("int.sub", "foo")
        with self.assertRaises(TypeError):
            ps.getDouble("int")
        with self.assertRaises(LookupError):
            ps.getDouble("double")
        with self.assertRaises(KeyError):
            ps.getArray("double")
        with self.assertRaises(LookupError):
            ps.typeOf("double")
        with self.assertRaises(TypeError):
            ps.add("int", 4.2)

        v = [3.14159, 2.71828]
        with self.assertRaises(TypeError):
            ps.add("int", v)
        ps.remove("foo.bar")
        ps.remove("int.sub")

    def testNames(self):
        ps = dafBase.PropertySet()
        ps.set("ps1.pre", 1)
        ps.set("ps1.post", 2)
        ps.set("int", 42)
        ps.set("double", 3.14)
        ps.set("ps2.plus", 10.24)
        ps.set("ps2.minus", -10.24)

        self.assertEqual(ps.nameCount(), 4)
        self.assertEqual(ps.nameCount(False), 8)

        v = set(ps.names())
        self.assertEqual(len(v), 4)
        self.assertEqual(v, {"double", "int", "ps1", "ps2"})
        v = set(ps.names(False))
        self.assertEqual(len(v), 8)
        self.assertEqual(v, {"double", "int", "ps1", "ps1.post",
                             "ps1.pre", "ps2", "ps2.minus", "ps2.plus"})

    def testParamNames(self):
        ps = dafBase.PropertySet()
        ps.set("ps1.pre", 1)
        ps.set("ps1.post", 2)
        ps.set("int", 42)
        ps.set("double", 3.14)
        ps.set("ps2.plus", 10.24)
        ps.set("ps2.minus", -10.24)

        v = set(ps.paramNames())
        self.assertEqual(len(v), 2)
        self.assertEqual(v, {"double", "int"})
        v = set(ps.paramNames(False))
        self.assertEqual(len(v), 6)
        self.assertEqual(v, {"double", "int", "ps1.post", "ps1.pre",
                             "ps2.minus", "ps2.plus"})

    def testPropertySetNames(self):
        ps = dafBase.PropertySet()
        ps.set("ps1.pre", 1)
        ps.set("ps1.post", 2)
        ps.set("int", 42)
        ps.set("double", 3.14)
        ps.set("ps2.plus", 10.24)
        ps.set("ps2.minus", -10.24)
        ps.set("ps3.sub.subsub", "foo")

        v = set(ps.propertySetNames())
        self.assertEqual(len(v), 3)
        self.assertEqual(v, {"ps1", "ps2", "ps3"})
        v = set(ps.propertySetNames(False))
        self.assertEqual(len(v), 4)
        self.assertEqual(v, {"ps1", "ps2", "ps3", "ps3.sub"})

    def testGetAs(self):
        ps = dafBase.PropertySet()
        ps.set("bool", True)
        ps.setShort("short", 42)
        ps.set("int", 2008)
        ps.set("int64_t", 0xfeeddeadbeef)
        f = 3.14159
        ps.setFloat("float", f)
        d = 2.718281828459045
        ps.setDouble("double", d)
        ps.setString("char*", "foo")
        ps.set("char*2", "foo2")
        ps.set("string", "bar")
        psp = dafBase.PropertySet()
        psp.set("bottom", "x")
        ps.set("top", psp)

        self.assertIs(ps.getAsBool("bool"), True)
        self.assertEqual(ps.getAsInt("bool"), 1)
        self.assertEqual(ps.getAsInt("short"), 42)
        self.assertEqual(ps.getAsInt("int"), 2008)
        with self.assertRaises(TypeError):
            ps.getAsInt("int64_t")
        self.assertEqual(ps.getAsInt64("bool"), 1)
        self.assertEqual(ps.getAsInt64("short"), 42)
        self.assertEqual(ps.getAsInt64("int"), 2008)
        self.assertEqual(ps.getAsInt64("int64_t"), 0xfeeddeadbeef)
        with self.assertRaises(TypeError):
            ps.getAsInt64("float")
        self.assertEqual(ps.getAsDouble("bool"), 1.0)
        self.assertEqual(ps.getAsDouble("short"), 42.0)
        self.assertEqual(ps.getAsDouble("int"), 2008.0)
        self.assertEqual(ps.getAsDouble("int64_t"),
                         float(0xfeeddeadbeef))
        self.assertAlmostEqual(ps.getAsDouble("float"), 3.14159, places=5)
        self.assertAlmostEqual(ps.getAsDouble("double"), 2.718281828459045, places=15)
        with self.assertRaises(TypeError):
            ps.getAsDouble("char*")
        self.assertEqual(ps.getAsString("char*"), "foo")
        self.assertEqual(ps.getAsString("char*2"), "foo2")
        self.assertEqual(ps.getAsString("string"), "bar")
        with self.assertRaises(TypeError):
            ps.getAsString("int")
        self.assertEqual(ps.getAsString("top.bottom"), "x")
        self.assertEqual(ps.getAsPropertySetPtr("top"), psp)
        with self.assertRaises(TypeError):
            ps.getAsPropertySetPtr("top.bottom")

    def testRemove(self):
        ps = dafBase.PropertySet()
        ps.set("int", 42)
        ps.set("double", 3.14159)
        ps.set("ps1.plus", 1)
        ps.set("ps1.minus", -1)
        ps.set("ps1.zero", 0)
        self.assertEqual(ps.nameCount(False), 6)

        ps.remove("int")
        self.assertFalse(ps.exists("int"))
        self.assertEqual(ps.getAsDouble("double"), 3.14159)
        self.assertEqual(ps.getAsInt("ps1.plus"), 1)
        self.assertEqual(ps.getAsInt("ps1.minus"), -1)
        self.assertEqual(ps.getAsInt("ps1.zero"), 0)
        self.assertEqual(ps.nameCount(False), 5)

        ps.remove("ps1.zero")
        self.assertFalse(ps.exists("int"))
        self.assertEqual(ps.getAsDouble("double"), 3.14159)
        self.assertFalse(ps.exists("ps1.zero"))
        self.assertEqual(ps.getAsInt("ps1.plus"), 1)
        self.assertEqual(ps.getAsInt("ps1.minus"), -1)
        self.assertEqual(ps.nameCount(False), 4)

        ps.remove("ps1")
        self.assertFalse(ps.exists("int"))
        self.assertEqual(ps.getAsDouble("double"), 3.14159)
        self.assertFalse(ps.exists("ps1"))
        self.assertFalse(ps.exists("ps1.plus"))
        self.assertFalse(ps.exists("ps1.minus"))
        self.assertFalse(ps.exists("ps1.zero"))
        self.assertEqual(ps.nameCount(False), 1)

        ps.remove("double")
        self.assertFalse(ps.exists("int"))
        self.assertFalse(ps.exists("double"))
        self.assertFalse(ps.exists("ps1"))
        self.assertFalse(ps.exists("ps1.plus"))
        self.assertFalse(ps.exists("ps1.minus"))
        self.assertFalse(ps.exists("ps1.zero"))
        self.assertEqual(ps.nameCount(False), 0)

    def testDeepCopy(self):
        ps = dafBase.PropertySet()
        ps.set("int", 42)
        psp = dafBase.PropertySet()
        psp.set("bottom", "x")
        ps.set("top", psp)

        psp2 = ps.deepCopy()
        self.assertTrue(psp2.exists("int"))
        self.assertTrue(psp2.exists("top.bottom"))
        self.assertEqual(psp2.getAsInt("int"), 42)
        self.assertEqual(psp2.getAsString("top.bottom"), "x")
        # Make sure it was indeed a deep copy.
        ps.set("int", 2008)
        ps.set("top.bottom", "y")
        self.assertEqual(ps.getAsInt("int"), 2008)
        self.assertEqual(ps.getAsString("top.bottom"), "y")
        self.assertEqual(psp.getAsString("bottom"), "y")
        self.assertEqual(psp2.getAsInt("int"), 42)
        self.assertEqual(psp2.getAsString("top.bottom"), "x")

    def testToString(self):
        ps = dafBase.PropertySet()
        ps.set("bool", True)
        s = 42
        ps.setShort("short", s)
        ps.set("int", 2008)
        ps.set("int64_t", 0xfeeddeadbeef)
        f = 3.14159
        ps.setFloat("float", f)
        d = 2.718281828459045
        ps.setDouble("double", d)
        ps.setString("char*", "foo")
        ps.set("char*2", "foo2")
        ps.set("string", "bar")
        ps.set("ps1.pre", 1)
        ps.set("ps1.post", 2)
        ps.set("ps2.plus", 10.24)
        ps.set("ps2.minus", -10.24)
        ps.set("ps3.sub.subsub", "foo")
        ps.add("v", 10)
        ps.add("v", 9)
        ps.add("v", 8)

        self.assertEqual(ps.toString(),
                         "bool = 1\n"
                         "char* = \"foo\"\n"
                         "char*2 = \"foo2\"\n"
                         "double = 2.7182818284590\n"
                         "float = 3.141590\n"
                         "int = 2008\n"
                         "int64_t = 280297596632815\n"
                         "ps1 = {\n"
                         "..post = 2\n"
                         "..pre = 1\n"
                         "}\n"
                         "ps2 = {\n"
                         "..minus = -10.240000000000\n"
                         "..plus = 10.240000000000\n"
                         "}\n"
                         "ps3 = {\n"
                         "..sub = {\n"
                         "....subsub = \"foo\"\n"
                         "..}\n"
                         "}\n"
                         "short = 42\n"
                         "string = \"bar\"\n"
                         "v = [ 10, 9, 8 ]\n"
                         )
        self.assertEqual(ps.toString(True),
                         "bool = 1\n"
                         "char* = \"foo\"\n"
                         "char*2 = \"foo2\"\n"
                         "double = 2.7182818284590\n"
                         "float = 3.141590\n"
                         "int = 2008\n"
                         "int64_t = 280297596632815\n"
                         "ps1 = { ... }\n"
                         "ps2 = { ... }\n"
                         "ps3 = { ... }\n"
                         "short = 42\n"
                         "string = \"bar\"\n"
                         "v = [ 10, 9, 8 ]\n"
                         )


class TestMemory(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()


if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()

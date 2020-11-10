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

import lsst.utils.tests
import lsst.daf.base as dafBase


class FloatSubClass(float):
    """Intended to be something like numpy.float64, without introducing a
    dependency on numpy
    """
    pass


class PropertyListTestCase(unittest.TestCase):
    """A test case for PropertyList.
    """

    def testConstruct(self):
        apl = dafBase.PropertyList()
        self.assertIsNotNone(apl)

    def checkPickle(self, original):
        new = pickle.loads(pickle.dumps(original, 2))
        self.assertEqual(new, original)
        return new

    def testScalar(self):
        apl = dafBase.PropertyList()
        apl.setBool("bool", True)
        apl.setShort("short", 42)
        apl.setInt("int", 2008)
        apl.setLongLong("int64_t", 0xfeeddeadbeef)
        apl.setFloat("float", 3.14159)
        apl.setDouble("double", 2.718281828459045)
        apl.set("char*", "foo")
        apl.setString("string", "bar")
        apl.set("int2", 2009)
        apl.set("dt", dafBase.DateTime("20090402T072639.314159265Z", dafBase.DateTime.UTC))
        apl.set("subclass", FloatSubClass(1.23456789))
        apl.set("undef", None)

        self.assertTrue(apl.isUndefined("undef"))
        self.assertFalse(apl.isUndefined("string"))
        self.assertEqual(apl.typeOf("bool"), dafBase.PropertyList.TYPE_Bool)
        self.assertEqual(apl.getBool("bool"), True)
        self.assertEqual(apl.typeOf("short"), dafBase.PropertyList.TYPE_Short)
        self.assertEqual(apl.getShort("short"), 42)
        self.assertEqual(apl.typeOf("int"), dafBase.PropertyList.TYPE_Int)
        self.assertEqual(apl.getInt("int"), 2008)
        self.assertEqual(apl.typeOf("int64_t"),
                         dafBase.PropertyList.TYPE_LongLong)
        self.assertEqual(apl.getLongLong("int64_t"), 0xfeeddeadbeef)
        self.assertEqual(apl.typeOf("float"), dafBase.PropertyList.TYPE_Float)
        self.assertAlmostEqual(apl.getFloat("float"), 3.14159, 6)
        self.assertEqual(apl.typeOf("double"), dafBase.PropertyList.TYPE_Double)
        self.assertEqual(apl.getDouble("double"), 2.718281828459045)
        self.assertEqual(apl.typeOf("char*"), dafBase.PropertyList.TYPE_String)
        self.assertEqual(apl.getString("char*"), "foo")
        self.assertEqual(apl.typeOf("string"), dafBase.PropertyList.TYPE_String)
        self.assertEqual(apl.getString("string"), "bar")
        self.assertEqual(apl.typeOf("int2"), dafBase.PropertyList.TYPE_Int)
        self.assertEqual(apl.getInt("int2"), 2009)
        self.assertEqual(apl.get("int2"), 2009)
        self.assertEqual(apl.getArray("int2"), [2009])
        self.assertEqual(apl.getScalar("int2"), 2009)
        self.assertEqual(apl.typeOf("dt"), dafBase.PropertyList.TYPE_DateTime)
        self.assertEqual(apl.getDateTime("dt").nsecs(), 1238657233314159265)
        self.assertEqual(apl.getDouble("subclass"), 1.23456789)
        self.assertEqual(apl["int2"], 2009)

        self.assertIsNone(apl.getScalar("undef"))
        self.assertEqual(apl.typeOf("undef"), dafBase.PropertyList.TYPE_Undef)
        self.assertIsNone(apl.get("undef"))
        self.assertIsNone(apl["undef"])
        self.assertEqual(apl.valueCount(), 12)
        self.checkPickle(apl)

        # Now replace the undef value with a defined value
        apl.set("undef", "not undefined")
        self.assertEqual(apl.getScalar("undef"), "not undefined")
        self.assertFalse(apl.isUndefined("undef"))
        self.assertEqual(apl.typeOf("undef"), dafBase.PropertyList.TYPE_String)
        self.assertEqual(apl.valueCount(), 12)

    def testGetDefault(self):
        apl = dafBase.PropertyList()
        apl.setInt("int", 42)
        self.assertEqual(apl.getInt("int"), 42)
        self.assertEqual(apl.getInt("int", 2008), 42)
        self.assertEqual(apl.getInt("foo", 2008), 2008)
        self.assertEqual(apl.get("int"), 42)
        self.assertEqual(apl.get("int", 2008), 42)
        self.assertEqual(apl.get("foo", 2008), 2008)
        self.assertEqual(apl.get("foo2", default="missing"), "missing")
        self.assertIsNone(apl.get("foo"))

    def testExists(self):
        apl = dafBase.PropertyList()
        apl.setInt("int", 42)
        self.assertEqual(apl.exists("int"), True)
        self.assertEqual(apl.exists("foo"), False)

    def testGetVector(self):
        apl = dafBase.PropertyList()
        v = [42, 2008, 1]
        apl.setInt("ints", v)
        apl.setInt("ints2", [10, 9, 8])
        w = apl.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        self.assertEqual(apl.getInt("ints2"), 8)
        self.assertEqual(apl.getArrayInt("ints2"), [10, 9, 8])
        w = apl.get("ints")
        self.assertIsInstance(w, int)
        self.assertEqual(v[-1], w)
        self.assertEqual(apl["ints"], v[-1])
        self.assertEqual(apl.getArray("ints"), v)
        self.assertEqual(apl.getScalar("ints"), v[-1])
        self.assertEqual(apl.valueCount(), 6)
        apl.setInt("int", 999)
        x = apl.get("int")
        self.assertEqual(x, 999)
        self.assertEqual(apl.getArray("int"), [999])
        self.assertEqual(apl.getScalar("int"), 999)
        self.assertEqual(apl["int"], 999)
        self.assertEqual(apl.valueCount(), 7)

        self.checkPickle(apl)

    def testGetVector2(self):
        apl = dafBase.PropertyList()
        v = [42, 2008, 1]
        apl.setInt("ints", v)
        apl.setInt("ints2", [10, 9, 8])
        w = apl.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v[0], w[0])
        self.assertEqual(v[1], w[1])
        self.assertEqual(v[2], w[2])
        self.assertEqual(apl.getInt("ints2"), 8)
        self.assertEqual(apl.getArrayInt("ints2"), [10, 9, 8])

        self.checkPickle(apl)

    def testAddScalar(self):
        apl = dafBase.PropertyList()
        v = [42, 2008, 1]
        apl.setInt("ints", v)
        apl.addInt("ints", -999)
        apl.add("other", "foo")
        apl.add("ints", 13)
        apl.add("subclass", FloatSubClass(1.23456789))
        w = apl.getArrayInt("ints")
        self.assertEqual(len(w), 5)
        self.assertEqual(v[0], w[0])
        self.assertEqual(v[1], w[1])
        self.assertEqual(v[2], w[2])
        self.assertEqual(w[3], -999)
        self.assertEqual(w[4], 13)
        self.assertEqual(apl.getString("other"), "foo")
        self.assertEqual(apl.get("subclass"), 1.23456789)
        self.assertEqual(apl.getArray("subclass"), [1.23456789])
        self.assertEqual(apl.getScalar("subclass"), 1.23456789)
        self.assertEqual(apl.valueCount(), 7)

    def testDateTimeToString(self):
        apl = dafBase.PropertyList()
        apl.set("dt", dafBase.DateTime("20090402T072639.314159265Z", dafBase.DateTime.UTC))
        self.assertEqual(apl.toString(),
                         "dt = 2009-04-02T07:26:39.314159265Z\n")

    def testGetScalarThrow(self):
        apl = dafBase.PropertyList()
        apl.setBool("bool", True)
        apl.setShort("short", 42)
        apl.setInt("int", 2008)
        apl.setLongLong("int64_t", 0xfeeddeadbeef)
        apl.setFloat("float", 3.14159)
        apl.setDouble("double", 2.718281828459045)
        apl.setString("string", "bar")

        with self.assertRaises(KeyError):
            apl["foo"]
        with self.assertRaises(TypeError):
            apl.getBool("short")
        with self.assertRaises(TypeError):
            apl.getBool("int")
        with self.assertRaises(TypeError):
            apl.getShort("int")
        with self.assertRaises(TypeError):
            apl.getInt("short")
        with self.assertRaises(TypeError):
            apl.getInt("bool")
        with self.assertRaises(TypeError):
            apl.getDouble("float")
        with self.assertRaises(TypeError):
            apl.getFloat("double")
        with self.assertRaises(TypeError):
            apl.getString("int")

    def testAddVector(self):
        apl = dafBase.PropertyList()
        v = [42, 2008, 1]
        apl.set("ints", v)
        apl.add("ints", [-42, -2008, -1])
        subclass = [FloatSubClass(1.23), FloatSubClass(4.56), FloatSubClass(7.89)]
        apl.add("subclass", subclass)
        self.assertEqual(apl.getArrayInt("ints"),
                         [42, 2008, 1, -42, -2008, -1])
        self.assertEqual(apl.get("subclass"), subclass[-1])
        self.assertEqual(apl.getArray("subclass"), subclass)
        self.assertEqual(apl.getScalar("subclass"), subclass[-1])
        self.assertEqual(apl.valueCount(), 9)

    def testComment(self):
        apl = dafBase.PropertyList()
        apl.set("NAXIS", 2, "two-dimensional")
        self.assertEqual(apl.get("NAXIS"), 2)
        self.assertEqual(apl.getArray("NAXIS"), [2])
        self.assertEqual(apl.getScalar("NAXIS"), 2)
        self.assertEqual(apl.getComment("NAXIS"), "two-dimensional")
        apl.set("NAXIS", 3, "three-dimensional")
        self.assertEqual(apl.get("NAXIS"), 3)
        self.assertEqual(apl.getArray("NAXIS"), [3])
        self.assertEqual(apl.getScalar("NAXIS"), 3)
        self.assertEqual(apl.getComment("NAXIS"), "three-dimensional")
        self.assertEqual(apl.valueCount(), 1)

    def testOrder(self):
        apl = dafBase.PropertyList()
        apl.set("SIMPLE", True)
        apl.set("BITPIX", -32)
        apl.set("NAXIS", 2)
        apl.set("COMMENT", "This is a test")
        apl.add("COMMENT", "This is a test line 2")
        apl.set("RA", 3.14159, "decimal degrees")
        apl.set("DEC", 2.71828, "decimal radians")
        correct = [
            ("SIMPLE", True, ""),
            ("BITPIX", -32, ""),
            ("NAXIS", 2, ""),
            ("COMMENT", "This is a test", ""),
            ("COMMENT", "This is a test line 2", ""),
            ("RA", 3.14159, "decimal degrees"),
            ("DEC", 2.71828, "decimal radians"),
        ]
        self.assertEqual(apl.toList(), correct)
        apl.set("NAXIS1", 513, "length of data axis 1")
        correct.append(("NAXIS1", 513, "length of data axis 1"))
        self.assertEqual(apl.toList(), correct)
        apl.set("RA", 1.414)
        correct[5] = ("RA", 1.414, "decimal degrees")
        self.assertEqual(apl.toList(), correct)
        apl.set("DEC", 1.732)
        correct[6] = ("DEC", 1.732, "decimal radians")
        self.assertEqual(apl.toList(), correct)
        apl.set("DEC", -6.28, "")
        correct[6] = ("DEC", -6.28, "")
        self.assertEqual(apl.toList(), correct)
        apl.add("COMMENT", "This is a test line 3", "")
        correct.insert(5, ("COMMENT", "This is a test line 3", ""))
        self.assertEqual(apl.toList(), correct)

        self.checkPickle(apl)

    def testToOrderedDict(self):
        from collections import OrderedDict

        apl = dafBase.PropertyList()
        apl.set("SIMPLE", True)
        apl.set("BITPIX", -32)
        apl.set("NAXIS", 2)
        apl.set("RA", 3.14159)
        apl.set("DEC", 2.71828)
        apl.set("FILTER", None)
        apl.set("COMMENT", "This is a test")
        apl.add("COMMENT", "This is a test line 2")
        correct = OrderedDict([
            ("SIMPLE", True),
            ("BITPIX", -32),
            ("NAXIS", 2),
            ("RA", 3.14159),
            ("DEC", 2.71828),
            ("FILTER", None),
            ("COMMENT", ["This is a test", "This is a test line 2"])
        ])
        self.assertEqual(apl.toOrderedDict(), correct)

        apl.set("NAXIS1", 513)
        correct["NAXIS1"] = 513
        self.assertEqual(apl.toOrderedDict(), correct)
        apl.set("RA", 1.414)
        correct["RA"] = 1.414
        self.assertEqual(apl.toOrderedDict(), correct)
        apl.set("DEC", 1.732)
        correct["DEC"] = 1.732
        self.assertEqual(apl.toOrderedDict(), correct)
        apl.set("DEC", -6.28)
        correct["DEC"] = -6.28
        self.assertEqual(apl.toOrderedDict(), correct)
        apl.add("COMMENT", "This is a test line 3")
        correct["COMMENT"] = correct["COMMENT"] + ["This is a test line 3", ]
        self.assertEqual(apl.toOrderedDict(), correct)

    def testHierarchy(self):
        apl = dafBase.PropertyList()
        apl.set("CURRENT", 49.5)
        apl.set("CURRENT.foo", -32)
        apl.set("CURRENT.bar", 2)
        self.assertEqual(apl.get("CURRENT"), 49.5)
        self.assertEqual(apl.getArray("CURRENT"), [49.5])
        self.assertEqual(apl.getScalar("CURRENT"), 49.5)
        self.assertEqual(apl.get("CURRENT.foo"), -32)
        self.assertEqual(apl.getArray("CURRENT.foo"), [-32])
        self.assertEqual(apl.getScalar("CURRENT.foo"), -32)
        self.assertEqual(apl.get("CURRENT.bar"), 2)
        self.assertEqual(apl.getArray("CURRENT.bar"), [2])
        self.assertEqual(apl.getScalar("CURRENT.bar"), 2)
        self.assertEqual(apl.valueCount(), 3)

        aps = dafBase.PropertySet()
        aps.set("bottom", "x")
        aps.set("sibling", 42)
        apl.set("top", aps)
        self.assertEqual(apl.get("top.bottom"), "x")
        self.assertEqual(apl.getArray("top.bottom"), ["x"])
        self.assertEqual(apl.getScalar("top.bottom"), "x")
        self.assertEqual(apl.get("top.sibling"), 42)
        self.assertEqual(apl.getArray("top.sibling"), [42])
        self.assertEqual(apl.getScalar("top.sibling"), 42)
        self.assertEqual(apl.valueCount(), 5)
        with self.assertRaises(KeyError):
            apl["top"]
        self.assertEqual(apl.toString(),
                         'CURRENT = 49.500000000000\nCURRENT.foo = -32\nCURRENT.bar = 2\n'
                         'top.sibling = 42\ntop.bottom = "x"\n')

        self.checkPickle(apl)

        # Check that a PropertyList (with comment) can go in a PropertySet
        apl.set("INT", 45, "an integer")
        aps = dafBase.PropertySet()
        aps.set("bottom", "x")
        aps.set("apl", apl)
        new = self.checkPickle(aps)
        self.assertIsInstance(new, dafBase.PropertySet)
        self.assertIsInstance(new.getScalar("apl"), dafBase.PropertyList)
        self.assertEqual(new.getScalar("apl").getComment("INT"), "an integer")

    def testCombineHierarchical(self):
        # Test that we can perform a deep copy of a PropertyList containing a
        # hierarchical (contains a '.') key.
        # This was a segfault prior to addressing DM-882.
        pl1 = dafBase.PropertyList()
        pl1.set("a.b", 1)
        pl2 = pl1.deepCopy()  # should not segfault
        self.assertEqual(pl1.get("a.b"), pl2.get("a.b"))
        self.assertEqual(pl1.getArray("a.b"), pl2.getArray("a.b"))
        self.assertEqual(pl1.getScalar("a.b"), pl2.getScalar("a.b"))
        self.checkPickle(pl1)

    def testCopy(self):
        dest = dafBase.PropertyList()
        source = dafBase.PropertyList()
        value1 = [1.5, 3.2]
        source.set("srcItem1", value1)
        dest.copy("destItem1", source, "srcItem1")
        self.assertEqual(dest.get("destItem1"), value1[-1])
        self.assertEqual(dest.getArray("destItem1"), value1)
        self.assertEqual(dest.getScalar("destItem1"), value1[-1])
        self.assertEqual(dest.valueCount(), 2)

        # items are replaced, regardless of type
        dest.set("destItem2", "string value")
        self.assertEqual(dest.valueCount(), 3)
        value2 = [5, -4, 3]
        source.set("srcItem2", value2)
        dest.copy("destItem2", source, "srcItem2")
        self.assertEqual(dest.get("destItem2"), value2[-1])
        self.assertEqual(dest.getArray("destItem2"), value2)
        self.assertEqual(dest.getScalar("destItem2"), value2[-1])
        self.assertEqual(dest.valueCount(), 5)

        # asScalar copies only the last value
        dest.copy("destItem2Scalar", source, "srcItem2", asScalar=True)
        self.assertEqual(dest.get("destItem2Scalar"), value2[-1])
        self.assertEqual(dest.getArray("destItem2Scalar"), [value2[-1]])
        self.assertEqual(dest.getScalar("destItem2Scalar"), value2[-1])
        self.assertEqual(dest.valueCount(), 6)

    def testArrayProperties(self):
        apl = dafBase.PropertyList()
        v = [42, 2008, 1]
        apl.set("ints", v)
        apl.set("int", 365)
        apl.set("ints2", -42)
        apl.add("ints2", -2008)

        self.assertTrue(apl.isArray("ints"))
        self.assertFalse(apl.isArray("int"))
        self.assertTrue(apl.isArray("ints2"))
        self.assertEqual(apl.valueCount("ints"), 3)
        self.assertEqual(apl.valueCount("int"), 1)
        self.assertEqual(apl.valueCount("ints2"), 2)
        self.assertEqual(apl.typeOf("ints"), dafBase.PropertyList.TYPE_Int)
        self.assertEqual(apl.typeOf("int"), dafBase.PropertyList.TYPE_Int)
        self.assertEqual(apl.typeOf("ints2"), dafBase.PropertyList.TYPE_Int)
        self.assertEqual(apl.valueCount(), 6)

    def testHierarchy2(self):
        apl = dafBase.PropertyList()
        aplp = dafBase.PropertyList()

        aplp.set("pre", 1)
        apl.set("apl1", aplp)

        # Python will not see this, aplp is disconnected
        aplp.set("post", 2)
        self.assertFalse(apl.exists("apl1.post"))

        apl.set("int", 42)

        # Setting an empty PropertyList has no effect
        apl.set("apl2", dafBase.PropertyList())
        self.assertFalse(apl.exists("apl2"))

        apl.set("apl2.plus", 10.24)
        apl.set("apl2.minus", -10.24)
        apl.set("apl3.sub1", "foo")
        apl.set("apl3.sub2", "bar")

        self.assertTrue(apl.exists("apl1.pre"))
        self.assertTrue(apl.exists("apl2.plus"))
        self.assertTrue(apl.exists("apl2.minus"))
        self.assertTrue(apl.exists("apl3.sub1"))
        self.assertTrue(apl.exists("apl3.sub2"))

        # Make sure checking a subproperty doesn't create it.
        self.assertFalse(apl.exists("apl2.pre"))
        self.assertFalse(apl.exists("apl2.pre"))
        # Make sure checking an element doesn't create it.
        self.assertFalse(apl.exists("apl4"))
        self.assertFalse(apl.exists("apl4"))
        # Make sure checking a subproperty with a nonexistent parent doesn't
        # create it.
        self.assertFalse(apl.exists("apl4.sub"))
        self.assertFalse(apl.exists("apl4.sub"))
        # Make sure checking a subproperty doesn't create its parent.
        self.assertFalse(apl.exists("apl4"))

    def testvariousThrows(self):
        apl = dafBase.PropertyList()
        apl.set("int", 42)

        # This raises an exception in C++ test but works in Python
        apl.set("int.sub", "foo")

        with self.assertRaises(TypeError):
            apl.getDouble("int")
        with self.assertRaises(LookupError):
            apl.getDouble("double"),
        with self.assertRaises(LookupError):
            apl.getArrayDouble("double")
        with self.assertRaises(LookupError):
            apl.typeOf("double")
        with self.assertRaises(TypeError):
            apl.add("int", 4.2),

        v = [3.14159, 2.71828]
        with self.assertRaises(TypeError):
            apl.add("int", v)
        apl.remove("foo.bar")
        apl.remove("int.sub")

    def testNames(self):
        apl = dafBase.PropertyList()
        apl.set("apl1.pre", 1)
        apl.set("apl1.post", 2)
        apl.set("int", 42)
        apl.set("double", 3.14)
        apl.set("apl2.plus", 10.24)
        apl.set("apl2.minus", -10.24)

        # Hierarchy is always flat
        self.assertEqual(apl.nameCount(), 6)
        self.assertEqual(apl.nameCount(False), 6)

        v = set(apl.names())
        self.assertEqual(len(v), 6)
        self.assertEqual(v, {"double", "int", "apl1.post",
                             "apl1.pre", "apl2.minus", "apl2.plus"})

    def testParamNames(self):
        apl = dafBase.PropertyList()
        apl.set("apl1.pre", 1)
        apl.set("apl1.post", 2)
        apl.set("int", 42)
        apl.set("double", 3.14)
        apl.set("apl2.plus", 10.24)
        apl.set("apl2.minus", -10.24)

        v = set(apl.paramNames())
        self.assertEqual(len(v), 6)
        self.assertEqual(v, {"double", "int", "apl1.post", "apl1.pre",
                             "apl2.minus", "apl2.plus"})

    def testPropertySetNames(self):
        apl = dafBase.PropertyList()
        apl.set("apl1.pre", 1)
        apl.set("apl1.post", 2)
        apl.set("int", 42)
        apl.set("double", 3.14)
        apl.set("apl2.plus", 10.24)
        apl.set("apl2.minus", -10.24)
        apl.set("apl3.sub.subsub", "foo")

        # There are no PropertySets inside flattened PropertyList
        v = set(apl.propertySetNames())
        self.assertEqual(len(v), 0)

    def testGetAs(self):
        apl = dafBase.PropertyList()
        apl.set("bool", True)
        s = 42
        apl.setShort("short", s)
        apl.set("int", 2008)
        apl.set("int64_t", 0xfeeddeadbeef)
        f = 3.14159
        apl.setFloat("float", f)
        d = 2.718281828459045
        apl.setDouble("double", d)
        apl.setString("char*", "foo")
        apl.set("char*2", "foo2")
        apl.set("string", "bar")
        aplp = dafBase.PropertyList()
        aplp.set("bottom", "x")
        apl.set("top", aplp)

        self.assertEqual(apl.getAsBool("bool"), True)
        self.assertEqual(apl.getAsInt("bool"), 1)
        self.assertEqual(apl.getAsInt("short"), 42)
        self.assertEqual(apl.getAsInt("int"), 2008)
        with self.assertRaises(TypeError):
            apl.getAsInt("int64_t")
        self.assertEqual(apl.getAsInt64("bool"), 1)
        self.assertEqual(apl.getAsInt64("short"), 42)
        self.assertEqual(apl.getAsInt64("int"), 2008)
        self.assertEqual(apl.getAsInt64("int64_t"), 0xfeeddeadbeef)
        with self.assertRaises(TypeError):
            apl.getAsInt64("float")
        self.assertEqual(apl.getAsDouble("bool"), 1.0)
        self.assertEqual(apl.getAsDouble("short"), 42.0)
        self.assertEqual(apl.getAsDouble("int"), 2008.0)
        self.assertEqual(apl.getAsDouble("int64_t"), float(0xfeeddeadbeef))
        self.assertAlmostEqual(apl.getAsDouble("float"), 3.14159, places=5)
        self.assertEqual(apl.getAsDouble("double"), 2.718281828459045)
        with self.assertRaises(TypeError):
            apl.getAsDouble("char*")
        self.assertEqual(apl.getAsString("char*"), "foo")
        self.assertEqual(apl.getAsString("char*2"), "foo2")
        self.assertEqual(apl.getAsString("string"), "bar")
        with self.assertRaises(TypeError):
            apl.getAsString("int")
        self.assertEqual(apl.getAsString("top.bottom"), "x")

    def testCombine(self):
        apl = dafBase.PropertyList()
        apl.set("apl1.pre", 1)
        apl.set("apl1.post", 2)
        apl.set("int", 42)
        apl.set("double", 3.14)
        apl.set("apl2.plus", 10.24)
        apl.set("apl2.minus", -10.24)
        apl.set("apl3.sub.subsub", "foo")

        aplp = dafBase.PropertyList()
        aplp.set("apl1.pre", 3)
        aplp.add("apl1.pre", 4)
        aplp.set("int", 2008)
        aplp.set("apl2.foo", "bar")
        aplp.set("apl4.top", "bottom")

        apl.combine(aplp)

        self.assertFalse(apl.isArray("apl1"))
        self.assertTrue(apl.isArray("apl1.pre"))
        self.assertFalse(apl.isArray("apl1.post"))
        self.assertFalse(apl.isArray("apl2"))
        self.assertFalse(apl.isArray("apl2.plus"))
        self.assertFalse(apl.isArray("apl2.minus"))
        self.assertFalse(apl.isArray("apl2.foo"))
        self.assertFalse(apl.isArray("apl3"))
        self.assertFalse(apl.isArray("apl3.sub"))
        self.assertFalse(apl.isArray("apl3.subsub"))
        self.assertFalse(apl.isArray("apl4"))
        self.assertFalse(apl.isArray("apl4.top"))
        self.assertTrue(apl.isArray("int"))
        self.assertFalse(apl.isArray("double"))
        self.assertEqual(apl.valueCount("apl1.pre"), 3)
        self.assertEqual(apl.valueCount("int"), 2)
        v = apl.getArray("apl1.pre")
        self.assertEqual(v, [1, 3, 4])
        v = apl.getArray("int")
        self.assertEqual(v, [42, 2008])
        self.assertEqual(apl.valueCount(), 12)

    def testUpdate(self):
        apl = dafBase.PropertyList()
        apl.set("apl1.pre", 1)
        apl.set("apl1.post", 2)
        apl.set("int", 42)
        apl.set("double", 3.14)
        apl.set("apl2.plus", 10.24)
        apl.set("apl2.minus", -10.24)
        apl.set("apl3.sub.subsub", "foo")

        aplp = dafBase.PropertyList()
        aplp.set("apl1.pre", 3)
        aplp.add("apl1.pre", 4)
        aplp.set("int", 2008)
        aplp.set("apl2.foo", "bar")
        aplp.set("apl4.top", "bottom")

        apl.update(aplp)

        self.assertFalse(apl.isArray("apl1"))
        self.assertTrue(apl.isArray("apl1.pre"))
        self.assertFalse(apl.isArray("apl1.post"))
        self.assertFalse(apl.isArray("apl2"))
        self.assertFalse(apl.isArray("apl2.plus"))
        self.assertFalse(apl.isArray("apl2.minus"))
        self.assertFalse(apl.isArray("apl2.foo"))
        self.assertFalse(apl.isArray("apl3"))
        self.assertFalse(apl.isArray("apl3.sub"))
        self.assertFalse(apl.isArray("apl3.subsub"))
        self.assertFalse(apl.isArray("apl4"))
        self.assertFalse(apl.isArray("apl4.top"))
        self.assertFalse(apl.isArray("int"))
        self.assertFalse(apl.isArray("double"))
        self.assertEqual(apl.valueCount("apl1.pre"), 2)
        self.assertEqual(apl.valueCount("int"), 1)
        v = apl.getArray("apl1.pre")
        self.assertEqual(v, [3, 4])
        v = apl.getArray("int")
        self.assertEqual(v, [2008])
        self.assertEqual(apl.valueCount(), 10)

        apld = {"int": 100, "str": "String", "apl1.foo": 10.5}
        apl.update(apld)
        self.assertEqual(apl["int"], apld["int"])
        self.assertEqual(apl["str"], apld["str"])
        self.assertEqual(apl["apl1.foo"], apld["apl1.foo"])
        self.assertEqual(apl["double"], 3.14)
        self.assertEqual(apl.valueCount(), 12)

    def testCombineThrow(self):
        apl = dafBase.PropertyList()
        apl.set("int", 42)

        aplp = dafBase.PropertyList()
        aplp.set("int", 3.14159)

        with self.assertRaises(TypeError):
            apl.combine(aplp)

        psd = {"bool": True}
        with self.assertRaises(TypeError):
            apl.combine(psd)

    def testremove(self):
        apl = dafBase.PropertyList()
        apl.set("int", 42)
        apl.set("double", 3.14159)
        apl.set("apl1.plus", 1)
        apl.set("apl1.minus", -1)
        apl.set("apl1.zero", 0)
        self.assertEqual(apl.nameCount(False), 5)

        apl.remove("int")
        self.assertFalse(apl.exists("int"))
        self.assertEqual(apl.getAsDouble("double"), 3.14159)
        self.assertEqual(apl.getAsInt("apl1.plus"), 1)
        self.assertEqual(apl.getAsInt("apl1.minus"), -1)
        self.assertEqual(apl.getAsInt("apl1.zero"), 0)
        self.assertEqual(apl.nameCount(False), 4)

        apl.remove("apl1.zero")
        self.assertFalse(apl.exists("int"))
        self.assertEqual(apl.getAsDouble("double"), 3.14159)
        self.assertFalse(apl.exists("apl1.zero"))
        self.assertEqual(apl.getAsInt("apl1.plus"), 1)
        self.assertEqual(apl.getAsInt("apl1.minus"), -1)
        self.assertEqual(apl.nameCount(False), 3)

        # Removing a non-existent key (flattened) has no effect
        self.assertFalse(apl.exists("apl1"))
        apl.remove("apl1")
        self.assertFalse(apl.exists("int"))
        self.assertEqual(apl.getAsDouble("double"), 3.14159)
        self.assertFalse(apl.exists("apl1"))
        self.assertTrue(apl.exists("apl1.plus"))
        self.assertTrue(apl.exists("apl1.minus"))
        self.assertFalse(apl.exists("apl1.zero"))
        self.assertEqual(apl.nameCount(False), 3)

        apl.remove("double")
        self.assertFalse(apl.exists("int"))
        self.assertFalse(apl.exists("double"))
        self.assertFalse(apl.exists("apl1"))
        self.assertTrue(apl.exists("apl1.plus"))
        self.assertTrue(apl.exists("apl1.minus"))
        self.assertFalse(apl.exists("apl1.zero"))
        self.assertEqual(apl.nameCount(False), 2)

        apl.remove("apl1.plus")
        apl.remove("apl1.minus")
        self.assertEqual(apl.nameCount(False), 0)

    def testdeepCopy(self):
        apl = dafBase.PropertyList()
        apl.set("int", 42)
        aplp = dafBase.PropertyList()
        aplp.set("bottom", "x")
        apl.set("top", aplp)

        aplp2 = apl.deepCopy()
        self.assertTrue(aplp2.exists("int"))
        self.assertTrue(aplp2.exists("top.bottom"))
        self.assertEqual(aplp2.getAsInt("int"), 42)
        self.assertEqual(aplp2.getAsString("top.bottom"), "x")
        # Make sure it was indeed a deep copy.
        apl.set("int", 2008)
        apl.set("top.bottom", "z")
        self.assertEqual(apl.getAsInt("int"), 2008)
        self.assertEqual(apl.getAsString("top.bottom"), "z")
        self.assertEqual(aplp2.getAsInt("int"), 42)
        self.assertEqual(aplp2.getAsString("top.bottom"), "x")

    def testToString(self):
        apl = dafBase.PropertyList()
        apl.set("bool", True)
        s = 42
        apl.setShort("short", s)
        apl.set("int", 2008)
        apl.set("int64_t", 0xfeeddeadbeef)
        f = 3.14159
        apl.setFloat("float", f)
        d = 2.718281828459045
        apl.setDouble("double", d)
        apl.setString("char*", "foo")
        apl.set("char*2", "foo2")
        apl.set("string", "bar")
        apl.set("apl1.pre", 1)
        apl.set("apl1.post", 2)
        apl.set("apl2.plus", 10.24)
        apl.set("apl2.minus", -10.24)
        apl.set("apl3.sub.subsub", "foo")
        apl.add("v", 10)
        apl.add("v", 9)
        apl.add("v", 8)

        # Check that the keys returned for this PropertyList match
        # the order they were set
        order = ['bool', 'short', 'int', 'int64_t', 'float', 'double', 'char*', 'char*2',
                 'string', 'apl1.pre', 'apl1.post', 'apl2.plus', 'apl2.minus', 'apl3.sub.subsub', 'v']
        self.assertEqual(apl.getOrderedNames(), order)

        # Argument to toString has no effect for flattened hierarchy
        self.assertEqual(apl.toString(),
                         "bool = 1\n"
                         "short = 42\n"
                         "int = 2008\n"
                         "int64_t = 280297596632815\n"
                         "float = 3.141590\n"
                         "double = 2.7182818284590\n"
                         "char* = \"foo\"\n"
                         "char*2 = \"foo2\"\n"
                         "string = \"bar\"\n"
                         "apl1.pre = 1\n"
                         "apl1.post = 2\n"
                         "apl2.plus = 10.240000000000\n"
                         "apl2.minus = -10.240000000000\n"
                         "apl3.sub.subsub = \"foo\"\n"
                         "v = [ 10, 9, 8 ]\n"
                         )
        self.assertEqual(apl.toString(True),
                         "bool = 1\n"
                         "short = 42\n"
                         "int = 2008\n"
                         "int64_t = 280297596632815\n"
                         "float = 3.141590\n"
                         "double = 2.7182818284590\n"
                         "char* = \"foo\"\n"
                         "char*2 = \"foo2\"\n"
                         "string = \"bar\"\n"
                         "apl1.pre = 1\n"
                         "apl1.post = 2\n"
                         "apl2.plus = 10.240000000000\n"
                         "apl2.minus = -10.240000000000\n"
                         "apl3.sub.subsub = \"foo\"\n"
                         "v = [ 10, 9, 8 ]\n"
                         )


class TestMemory(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()


if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()

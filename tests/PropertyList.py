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

import unittest

import lsst.daf.base as dafBase
import lsst.pex.exceptions as pexExcept

class PropertyListTestCase(unittest.TestCase):
    """A test case for PropertyList."""

    def testConstruct(self):
        apl = dafBase.PropertyList()
        self.assert_(apl is not None)

    def testScalar(self):
        apl = dafBase.PropertyList()
        apl.setBool("bool", True)
        apl.setShort("short", 42)
        apl.setInt("int", 2008)
        apl.setLongLong("int64_t", 0xfeeddeadbeefL)
        apl.setFloat("float", 3.14159)
        apl.setDouble("double", 2.718281828459045)
        apl.set("char*", "foo")
        apl.setString("string", "bar")
        apl.set("int2", 2009)
        apl.set("dt", dafBase.DateTime("20090402T072639.314159265Z"))

        self.assertEqual(apl.typeOf("bool"), dafBase.PropertyList.TYPE_Bool)
        self.assertEqual(apl.getBool("bool"), True)
        self.assertEqual(apl.typeOf("short"), dafBase.PropertyList.TYPE_Short)
        self.assertEqual(apl.getShort("short"), 42)
        self.assertEqual(apl.typeOf("int"), dafBase.PropertyList.TYPE_Int)
        self.assertEqual(apl.getInt("int"), 2008)
        self.assertEqual(apl.typeOf("int64_t"),
                dafBase.PropertyList.TYPE_LongLong)
        self.assertEqual(apl.getLongLong("int64_t"), 0xfeeddeadbeefL)
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
        self.assertEqual(apl.typeOf("dt"), dafBase.PropertyList.TYPE_DateTime)
        self.assertEqual(apl.getDateTime("dt").nsecs(), 1238657233314159265L)

    def testGetDefault(self):
        apl = dafBase.PropertyList()
        apl.setInt("int", 42)
        self.assertEqual(apl.getInt("int"), 42)
        self.assertEqual(apl.getInt("int", 2008), 42)
        self.assertEqual(apl.getInt("foo", 2008), 2008)

    def testExists(self):
        apl = dafBase.PropertyList()
        apl.setInt("int", 42)
        self.assertEqual(apl.exists("int"), True)
        self.assertEqual(apl.exists("foo"), False)

    def testGetVector(self):
        apl = dafBase.PropertyList()
        v = (42, 2008, 1)
        apl.setInt("ints", v)
        apl.setInt("ints2", (10, 9, 8))
        w = apl.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        self.assertEqual(apl.getInt("ints2"), 8)
        self.assertEqual(apl.getArrayInt("ints2"), (10, 9, 8))
        w = apl.get("ints", asArray=True)
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        apl.setInt("int", 999)
        x = apl.get("int")
        self.assertEqual(x, 999)
        x = apl.get("int", asArray=True)
        self.assertEqual(len(x), 1)
        self.assertEqual(x, (999,))

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
        self.assertEqual(apl.getArrayInt("ints2"), (10, 9, 8))

    def testAddScalar(self):
        apl = dafBase.PropertyList()
        v = [42, 2008, 1]
        apl.setInt("ints", v)
        apl.addInt("ints", -999)
        apl.add("other", "foo")
        apl.add("ints", 13)
        w = apl.getArrayInt("ints")
        self.assertEqual(len(w), 5)
        self.assertEqual(v[0], w[0])
        self.assertEqual(v[1], w[1])
        self.assertEqual(v[2], w[2])
        self.assertEqual(w[3], -999)
        self.assertEqual(w[4], 13)
        self.assertEqual(apl.getString("other"), "foo")

    def testDateTimeToString(self):
        apl = dafBase.PropertyList()
        apl.set("dt", dafBase.DateTime("20090402T072639.314159265Z"))
        self.assertEqual(apl.toString(),
                "dt = 2009-04-02T07:26:39.314159265Z\n")

    def testGetScalarThrow(self):
        apl = dafBase.PropertyList()
        apl.setBool("bool", True)
        apl.setShort("short", 42)
        apl.setInt("int", 2008)
        apl.setLongLong("int64_t", 0xfeeddeadbeefL)
        apl.setFloat("float", 3.14159)
        apl.setDouble("double", 2.718281828459045)
        apl.setString("string", "bar")
        self.assertRaises(pexExcept.LsstException, apl.get, "foo")
        self.assertRaises(pexExcept.LsstException, apl.getBool, "short")
        self.assertRaises(pexExcept.LsstException, apl.getBool, "int")
        self.assertRaises(pexExcept.LsstException, apl.getShort, "int")
        self.assertRaises(pexExcept.LsstException, apl.getInt, "short")
        self.assertRaises(pexExcept.LsstException, apl.getInt, "bool")
        self.assertRaises(pexExcept.LsstException, apl.getDouble, "float")
        self.assertRaises(pexExcept.LsstException, apl.getFloat, "double")
        self.assertRaises(pexExcept.LsstException, apl.getString, "int")

    def testAddVector(self):
        apl = dafBase.PropertyList()
        v = [42, 2008, 1]
        apl.set("ints", v)
        apl.add("ints", [-42, -2008, -1])
        self.assertEqual(apl.getArrayInt("ints"),
                (42, 2008, 1, -42, -2008, -1))

    def testComment(self):
        apl = dafBase.PropertyList()
        apl.set("NAXIS", 2, "two-dimensional")
        self.assertEqual(apl.get("NAXIS"), 2)
        self.assertEqual(apl.getComment("NAXIS"), "two-dimensional")
        apl.set("NAXIS", 3, "three-dimensional")
        self.assertEqual(apl.get("NAXIS"), 3)
        self.assertEqual(apl.getComment("NAXIS"), "three-dimensional")

    def testOrder(self):
        apl = dafBase.PropertyList()
        apl.set("SIMPLE", True)
        apl.set("BITPIX", -32)
        apl.set("NAXIS", 2)
        apl.set("RA", 3.14159, "decimal degrees")
        apl.set("DEC", 2.71828, "decimal radians")
        apl.set("COMMENT", "This is a test")
        apl.add("COMMENT", "This is a test line 2")
        correct = [
                ("SIMPLE", True, ""),
                ("BITPIX", -32, ""),
                ("NAXIS", 2, ""),
                ("RA", 3.14159, "decimal degrees"),
                ("DEC", 2.71828, "decimal radians"),
                ("COMMENT", "This is a test", ""),
                ("COMMENT", "This is a test line 2", "")
                ]
        self.assertEqual(apl.toList(), correct)
        apl.set("NAXIS1", 513, "length of data axis 1")
        correct.append(("NAXIS1", 513, "length of data axis 1"))
        self.assertEqual(apl.toList(), correct)
        apl.set("RA", 1.414, inPlace=False)
        del correct[3]
        correct.append(("RA", 1.414, "decimal degrees"))
        self.assertEqual(apl.toList(), correct)
        apl.set("DEC", 1.732)
        correct[3] = ("DEC", 1.732, "decimal radians")
        self.assertEqual(apl.toList(), correct)
        apl.set("DEC", -6.28, "")
        correct[3] = ("DEC", -6.28, "")
        self.assertEqual(apl.toList(), correct)
        apl.add("COMMENT", "This is a test line 3", "")
        correct.insert(6, ("COMMENT", "This is a test line 3", ""))
        self.assertEqual(apl.toList(), correct)
        apl.add("COMMENT", "This is a test line 4", "", inPlace=False)
        correct.append(correct[4])
        correct.append(correct[5])
        correct.append(correct[6])
        correct.append(("COMMENT", "This is a test line 4", ""))
        del correct[4]
        del correct[4]
        del correct[4]
        self.assertEqual(apl.toList(), correct)

    def testHierarchy(self):
        apl = dafBase.PropertyList()
        apl.set("CURRENT", 49.5)
        apl.set("CURRENT.foo", -32)
        apl.set("CURRENT.bar", 2)
        self.assertEqual(apl.get("CURRENT"), 49.5)
        self.assertEqual(apl.get("CURRENT.foo"), -32)
        self.assertEqual(apl.get("CURRENT.bar"), 2)

        aps = dafBase.PropertySet()
        aps.set("bottom", "x")
        aps.set("sibling", 42)
        apl.set("top", aps)
        self.assertEqual(apl.get("top.bottom"), "x")
        self.assertEqual(apl.get("top.sibling"), 42)
        self.assertRaises(pexExcept.LsstException, apl.get, "top")
        self.assertEqual(apl.toString(),
            'CURRENT = 49.500000000000\nCURRENT.foo = -32\nCURRENT.bar = 2\n'
            'top.sibling = 42\ntop.bottom = "x"\n')


if __name__ == '__main__':
    unittest.main()

# BOOST_AUTO_TEST_CASE(arrayProperties) {
#     dafBase::PropertyList apl;
#     std::vector<int> v;
#     v.push_back(42);
#     v.push_back(2008);
#     v.push_back(1);
#     apl.set("ints", v);
#     apl.set("int", 365);
#     apl.set("ints2", -42);
#     apl.add("ints2", -2008);
# 
#     BOOST_CHECK_EQUAL(apl.isArray("ints"), true);
#     BOOST_CHECK_EQUAL(apl.isArray("int"), false);
#     BOOST_CHECK_EQUAL(apl.isArray("ints2"), true);
#     BOOST_CHECK_EQUAL(apl.valueCount("ints"), 3U);
#     BOOST_CHECK_EQUAL(apl.valueCount("int"), 1U);
#     BOOST_CHECK_EQUAL(apl.valueCount("ints2"), 2U);
#     BOOST_CHECK(apl.typeOf("ints") == typeid(int));
#     BOOST_CHECK(apl.typeOf("int") == typeid(int));
#     BOOST_CHECK(apl.typeOf("ints2") == typeid(int));
# }
# 
# BOOST_AUTO_TEST_CASE(hierarchy) {
#     dafBase::PropertyList apl;
#     dafBase::PropertyList::Ptr aplp(new
#     dafBase::PropertyList);
# 
#     aplp->set("pre", 1);
#     apl.set("apl1", aplp);
#     aplp->set("post", 2);
#     apl.set("int", 42);
#     apl.set("apl2", dafBase::PropertyList::Ptr(new
#     dafBase::PropertyList));
#     apl.get<dafBase::PropertyList::Ptr>("apl2")->set("plus", 10.24);
#     apl.set("apl2.minus", -10.24);
#     apl.set("apl3.sub1", "foo");
#     apl.set("apl3.sub2", "bar");
# 
#     BOOST_CHECK(apl.exists("apl1"));
#     BOOST_CHECK(apl.exists("apl2"));
#     BOOST_CHECK(apl.exists("apl3"));
#     BOOST_CHECK(apl.exists("apl1.pre"));
#     BOOST_CHECK(apl.exists("apl1.post"));
#     BOOST_CHECK(apl.exists("apl2.plus"));
#     BOOST_CHECK(apl.exists("apl2.minus"));
#     BOOST_CHECK(apl.exists("apl3.sub1"));
#     BOOST_CHECK(apl.exists("apl3.sub2"));
# 
#     BOOST_CHECK(apl.isPropertyListPtr("apl1"));
#     BOOST_CHECK(apl.isPropertyListPtr("apl2"));
#     BOOST_CHECK(apl.isPropertyListPtr("apl3"));
#     BOOST_CHECK(!apl.isPropertyListPtr("int"));
#     BOOST_CHECK(!apl.isPropertyListPtr("apl1.pre"));
#     BOOST_CHECK(!apl.isPropertyListPtr("apl1.post"));
#     BOOST_CHECK(!apl.isPropertyListPtr("apl2.plus"));
#     BOOST_CHECK(!apl.isPropertyListPtr("apl2.minus"));
#     BOOST_CHECK(!apl.isPropertyListPtr("apl3.sub1"));
#     BOOST_CHECK(!apl.isPropertyListPtr("apl3.sub2"));
# 
#     dafBase::PropertyList::Ptr aplp1 =
#     apl.get<dafBase::PropertyList::Ptr>("apl1");
#     dafBase::PropertyList::Ptr aplp2 =
#     apl.get<dafBase::PropertyList::Ptr>("apl2");
#     dafBase::PropertyList::Ptr aplp3 =
#     apl.get<dafBase::PropertyList::Ptr>("apl3");
#     BOOST_CHECK(aplp1);
#     BOOST_CHECK(aplp2);
#     BOOST_CHECK(aplp3);
#     BOOST_CHECK(aplp1 == aplp);
#     BOOST_CHECK(aplp1->exists("pre"));
#     BOOST_CHECK(aplp1->exists("post"));
#     BOOST_CHECK(aplp2->exists("plus"));
#     BOOST_CHECK(aplp2->exists("minus"));
#     BOOST_CHECK(aplp3->exists("sub1"));
#     BOOST_CHECK(aplp3->exists("sub2"));
#     BOOST_CHECK_EQUAL(aplp1->get<int>("pre"), 1);
#     BOOST_CHECK_EQUAL(aplp1->get<int>("post"), 2);
#     BOOST_CHECK_EQUAL(aplp2->get<double>("plus"), 10.24);
#     BOOST_CHECK_EQUAL(aplp2->get<double>("minus"), -10.24);
#     BOOST_CHECK_EQUAL(aplp3->get<std::string>("sub1"), "foo");
#     BOOST_CHECK_EQUAL(aplp3->get<std::string>("sub2"), "bar");
# 
#     // Make sure checking a subproperty doesn't create it.
#     BOOST_CHECK(!apl.exists("apl2.pre"));
#     BOOST_CHECK(!apl.exists("apl2.pre"));
#     // Make sure checking an element doesn't create it.
#     BOOST_CHECK(!apl.exists("apl4"));
#     BOOST_CHECK(!apl.exists("apl4"));
#     // Make sure checking a subproperty with a nonexistent parent doesn't
#     // create it.
#     BOOST_CHECK(!apl.exists("apl4.sub"));
#     BOOST_CHECK(!apl.exists("apl4.sub"));
#     // Make sure checking a subproperty doesn't create its parent.
#     BOOST_CHECK(!apl.exists("apl4"));
# }
# 
# BOOST_AUTO_TEST_CASE(variousThrows) {
#     dafBase::PropertyList apl;
#     apl.set("int", 42);
#     BOOST_CHECK_THROW(apl.set("int.sub", "foo"),
#                       lsst::pex::exceptions::InvalidParameterException);
#     BOOST_CHECK_THROW(apl.get<double>("int"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(apl.get<double>("double"),
#                       lsst::pex::exceptions::NotFoundException);
#     BOOST_CHECK_THROW(apl.getArray<double>("double"),
#                       lsst::pex::exceptions::NotFoundException);
#     BOOST_CHECK_THROW(apl.typeOf("double"),
#                       lsst::pex::exceptions::NotFoundException);
#     BOOST_CHECK_THROW(apl.add("int", 4.2),
#                       lsst::pex::exceptions::DomainErrorException);
#     std::vector<double> v;
#     v.push_back(3.14159);
#     v.push_back(2.71828);
#     BOOST_CHECK_THROW(apl.add("int", v),
#                       lsst::pex::exceptions::DomainErrorException);
#     BOOST_CHECK_NO_THROW(apl.remove("foo.bar"));
#     BOOST_CHECK_NO_THROW(apl.remove("int.sub"));
# }
# 
# BOOST_AUTO_TEST_CASE(names) {
#     dafBase::PropertyList apl;
#     apl.set("apl1.pre", 1);
#     apl.set("apl1.post", 2);
#     apl.set("int", 42);
#     apl.set("double", 3.14);
#     apl.set("apl2.plus", 10.24);
#     apl.set("apl2.minus", -10.24);
# 
#     BOOST_CHECK_EQUAL(apl.nameCount(), 4U);
#     BOOST_CHECK_EQUAL(apl.nameCount(false), 8U);
# 
#     std::vector<std::string> v = apl.names();
#     BOOST_CHECK_EQUAL(v.size(), 4U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     BOOST_CHECK_EQUAL(v[2], "apl1");
#     BOOST_CHECK_EQUAL(v[3], "apl2");
#     v = apl.names(false);
#     BOOST_CHECK_EQUAL(v.size(), 8U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     BOOST_CHECK_EQUAL(v[2], "apl1");
#     BOOST_CHECK_EQUAL(v[3], "apl1.post");
#     BOOST_CHECK_EQUAL(v[4], "apl1.pre");
#     BOOST_CHECK_EQUAL(v[5], "apl2");
#     BOOST_CHECK_EQUAL(v[6], "apl2.minus");
#     BOOST_CHECK_EQUAL(v[7], "apl2.plus");
# }
# 
# BOOST_AUTO_TEST_CASE(paramNames) {
#     dafBase::PropertyList apl;
#     apl.set("apl1.pre", 1);
#     apl.set("apl1.post", 2);
#     apl.set("int", 42);
#     apl.set("double", 3.14);
#     apl.set("apl2.plus", 10.24);
#     apl.set("apl2.minus", -10.24);
# 
#     std::vector<std::string> v = apl.paramNames();
#     BOOST_CHECK_EQUAL(v.size(), 2U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     v = apl.paramNames(false);
#     BOOST_CHECK_EQUAL(v.size(), 6U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     BOOST_CHECK_EQUAL(v[2], "apl1.post");
#     BOOST_CHECK_EQUAL(v[3], "apl1.pre");
#     BOOST_CHECK_EQUAL(v[4], "apl2.minus");
#     BOOST_CHECK_EQUAL(v[5], "apl2.plus");
# }
# 
# BOOST_AUTO_TEST_CASE(propertySetNames) {
#     dafBase::PropertyList apl;
#     apl.set("apl1.pre", 1);
#     apl.set("apl1.post", 2);
#     apl.set("int", 42);
#     apl.set("double", 3.14);
#     apl.set("apl2.plus", 10.24);
#     apl.set("apl2.minus", -10.24);
#     apl.set("apl3.sub.subsub", "foo");
# 
#     std::vector<std::string> v = apl.propertySetNames();
#     BOOST_CHECK_EQUAL(v.size(), 3U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "apl1");
#     BOOST_CHECK_EQUAL(v[1], "apl2");
#     BOOST_CHECK_EQUAL(v[2], "apl3");
#     v = apl.propertySetNames(false);
#     BOOST_CHECK_EQUAL(v.size(), 4U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "apl1");
#     BOOST_CHECK_EQUAL(v[1], "apl2");
#     BOOST_CHECK_EQUAL(v[2], "apl3");
#     BOOST_CHECK_EQUAL(v[3], "apl3.sub");
# }
# 
# BOOST_AUTO_TEST_CASE(getAs) {
#     dafBase::PropertyList apl;
#     apl.set("bool", true);
#     apl.set("char", 'A');
#     short s = 42;
#     apl.set("short", s);
#     apl.set("int", 2008);
#     apl.set("int64_t", 0xfeeddeadbeefLL);
#     float f = 3.14159;
#     apl.set("float", f);
#     double d = 2.718281828459045;
#     apl.set("double", d);
#     apl.set<std::string>("char*", "foo");
#     apl.set("char*2", "foo2");
#     apl.set("string", std::string("bar"));
#     dafBase::PropertyList::Ptr aplp(new
#     dafBase::PropertyList);
#     aplp->set("bottom", "x");
#     apl.set("top", aplp);
# 
#     BOOST_CHECK_EQUAL(apl.getAsBool("bool"), true);
#     BOOST_CHECK_THROW(apl.getAsBool("char"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(apl.getAsInt("bool"), 1);
#     BOOST_CHECK_EQUAL(apl.getAsInt("char"), static_cast<int>('A'));
#     BOOST_CHECK_EQUAL(apl.getAsInt("short"), 42);
#     BOOST_CHECK_EQUAL(apl.getAsInt("int"), 2008);
#     BOOST_CHECK_THROW(apl.getAsInt("int64_t"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(apl.getAsInt64("bool"), 1LL);
#     BOOST_CHECK_EQUAL(apl.getAsInt64("char"), static_cast<int64_t>('A'));
#     BOOST_CHECK_EQUAL(apl.getAsInt64("short"), 42LL);
#     BOOST_CHECK_EQUAL(apl.getAsInt64("int"), 2008LL);
#     BOOST_CHECK_EQUAL(apl.getAsInt64("int64_t"), 0xfeeddeadbeefLL);
#     BOOST_CHECK_THROW(apl.getAsInt64("float"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(apl.getAsDouble("bool"), 1.0);
#     BOOST_CHECK_EQUAL(apl.getAsDouble("char"), static_cast<double>('A'));
#     BOOST_CHECK_EQUAL(apl.getAsDouble("short"), 42.0);
#     BOOST_CHECK_EQUAL(apl.getAsDouble("int"), 2008.0);
#     BOOST_CHECK_EQUAL(apl.getAsDouble("int64_t"),
#                       static_cast<double>(0xfeeddeadbeefLL));
#     BOOST_CHECK_EQUAL(apl.getAsDouble("float"), 3.14159f);
#     BOOST_CHECK_EQUAL(apl.getAsDouble("double"), 2.718281828459045);
#     BOOST_CHECK_THROW(apl.getAsDouble("char*"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(apl.getAsString("char"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(apl.getAsString("char*"), "foo");
#     BOOST_CHECK_EQUAL(apl.getAsString("char*2"), "foo2");
#     BOOST_CHECK_EQUAL(apl.getAsString("string"), "bar");
#     BOOST_CHECK_THROW(apl.getAsString("int"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(apl.getAsString("top.bottom"), "x");
#     BOOST_CHECK_EQUAL(apl.getAsPropertyListPtr("top"), aplp);
#     BOOST_CHECK_THROW(apl.getAsPropertyListPtr("top.bottom"),
#                       boost::bad_any_cast);
# }
# 
# BOOST_AUTO_TEST_CASE(combine) {
#     dafBase::PropertyList apl;
#     apl.set("apl1.pre", 1);
#     apl.set("apl1.post", 2);
#     apl.set("int", 42);
#     apl.set("double", 3.14);
#     apl.set("apl2.plus", 10.24);
#     apl.set("apl2.minus", -10.24);
#     apl.set("apl3.sub.subsub", "foo");
# 
#     dafBase::PropertyList::Ptr aplp(new
#     dafBase::PropertyList);
#     aplp->set("apl1.pre", 3);
#     aplp->add("apl1.pre", 4);
#     aplp->set("int", 2008);
#     aplp->set("apl2.foo", "bar");
#     aplp->set("apl4.top", "bottom");
# 
#     apl.combine(aplp);
# 
#     BOOST_CHECK(apl.isPropertyListPtr("apl1"));
#     BOOST_CHECK(apl.isPropertyListPtr("apl2"));
#     BOOST_CHECK(apl.isPropertyListPtr("apl3"));
#     BOOST_CHECK(apl.isPropertyListPtr("apl3.sub"));
#     BOOST_CHECK(apl.isPropertyListPtr("apl4"));
#     BOOST_CHECK(!apl.isArray("apl1"));
#     BOOST_CHECK(apl.isArray("apl1.pre"));
#     BOOST_CHECK(!apl.isArray("apl1.post"));
#     BOOST_CHECK(!apl.isArray("apl2"));
#     BOOST_CHECK(!apl.isArray("apl2.plus"));
#     BOOST_CHECK(!apl.isArray("apl2.minus"));
#     BOOST_CHECK(!apl.isArray("apl2.foo"));
#     BOOST_CHECK(!apl.isArray("apl3"));
#     BOOST_CHECK(!apl.isArray("apl3.sub"));
#     BOOST_CHECK(!apl.isArray("apl3.subsub"));
#     BOOST_CHECK(!apl.isArray("apl4"));
#     BOOST_CHECK(!apl.isArray("apl4.top"));
#     BOOST_CHECK(apl.isArray("int"));
#     BOOST_CHECK(!apl.isArray("double"));
#     BOOST_CHECK_EQUAL(apl.valueCount("apl1.pre"), 3U);
#     BOOST_CHECK_EQUAL(apl.valueCount("int"), 2U);
#     std::vector<int> v = apl.getArray<int>("apl1.pre");
#     BOOST_CHECK_EQUAL(v[0], 1);
#     BOOST_CHECK_EQUAL(v[1], 3);
#     BOOST_CHECK_EQUAL(v[2], 4);
#     v = apl.getArray<int>("int");
#     BOOST_CHECK_EQUAL(v[0], 42);
#     BOOST_CHECK_EQUAL(v[1], 2008);
# }
# 
# BOOST_AUTO_TEST_CASE(combineThrow) {
#     dafBase::PropertyList apl;
#     apl.set("int", 42);
# 
#     dafBase::PropertyList::Ptr aplp(new
#     dafBase::PropertyList);
#     aplp->set("int", 3.14159);
# 
#     BOOST_CHECK_THROW(apl.combine(aplp),
#                       lsst::pex::exceptions::DomainErrorException);
# }
# 
# BOOST_AUTO_TEST_CASE(remove) {
#     dafBase::PropertyList apl;
#     apl.set("int", 42);
#     apl.set("double", 3.14159);
#     apl.set("apl1.plus", 1);
#     apl.set("apl1.minus", -1);
#     apl.set("apl1.zero", 0);
#     BOOST_CHECK_EQUAL(apl.nameCount(false), 6U);
# 
#     apl.remove("int");
#     BOOST_CHECK(!apl.exists("int"));
#     BOOST_CHECK_EQUAL(apl.getAsDouble("double"), 3.14159);
#     BOOST_CHECK_EQUAL(apl.getAsInt("apl1.plus"), 1);
#     BOOST_CHECK_EQUAL(apl.getAsInt("apl1.minus"), -1);
#     BOOST_CHECK_EQUAL(apl.getAsInt("apl1.zero"), 0);
#     BOOST_CHECK_EQUAL(apl.nameCount(false), 5U);
# 
#     apl.remove("apl1.zero");
#     BOOST_CHECK(!apl.exists("int"));
#     BOOST_CHECK_EQUAL(apl.getAsDouble("double"), 3.14159);
#     BOOST_CHECK(!apl.exists("apl1.zero"));
#     BOOST_CHECK_EQUAL(apl.getAsInt("apl1.plus"), 1);
#     BOOST_CHECK_EQUAL(apl.getAsInt("apl1.minus"), -1);
#     BOOST_CHECK_EQUAL(apl.nameCount(false), 4U);
# 
#     apl.remove("apl1");
#     BOOST_CHECK(!apl.exists("int"));
#     BOOST_CHECK_EQUAL(apl.getAsDouble("double"), 3.14159);
#     BOOST_CHECK(!apl.exists("apl1"));
#     BOOST_CHECK(!apl.exists("apl1.plus"));
#     BOOST_CHECK(!apl.exists("apl1.minus"));
#     BOOST_CHECK(!apl.exists("apl1.zero"));
#     BOOST_CHECK_EQUAL(apl.nameCount(false), 1U);
# 
#     apl.remove("double");
#     BOOST_CHECK(!apl.exists("int"));
#     BOOST_CHECK(!apl.exists("double"));
#     BOOST_CHECK(!apl.exists("apl1"));
#     BOOST_CHECK(!apl.exists("apl1.plus"));
#     BOOST_CHECK(!apl.exists("apl1.minus"));
#     BOOST_CHECK(!apl.exists("apl1.zero"));
#     BOOST_CHECK_EQUAL(apl.nameCount(false), 0U);
# }
# 
# BOOST_AUTO_TEST_CASE(deepCopy) {
#     dafBase::PropertyList apl;
#     apl.set("int", 42);
#     dafBase::PropertyList::Ptr aplp(new
#     dafBase::PropertyList);
#     aplp->set("bottom", "x");
#     apl.set("top", aplp);
# 
#     dafBase::PropertyList::Ptr aplp2 = apl.deepCopy();
#     BOOST_CHECK(aplp2->exists("int"));
#     BOOST_CHECK(aplp2->exists("top.bottom"));
#     BOOST_CHECK_EQUAL(aplp2->getAsInt("int"), 42);
#     BOOST_CHECK_EQUAL(aplp2->getAsString("top.bottom"), "x");
#     // Make sure it was indeed a deep copy.
#     BOOST_CHECK(aplp2->getAsPropertyListPtr("top") != aplp);
#     apl.set("int", 2008);
#     apl.set("top.bottom", "y");
#     BOOST_CHECK_EQUAL(apl.getAsInt("int"), 2008);
#     BOOST_CHECK_EQUAL(apl.getAsString("top.bottom"), "y");
#     BOOST_CHECK_EQUAL(aplp->getAsString("bottom"), "y");
#     BOOST_CHECK_EQUAL(aplp2->getAsInt("int"), 42);
#     BOOST_CHECK_EQUAL(aplp2->getAsString("top.bottom"), "x");
# }
# 
# BOOST_AUTO_TEST_CASE(toString) {
#     dafBase::PropertyList apl;
#     apl.set("bool", true);
#     apl.set("char", '*');
#     short s = 42;
#     apl.set("short", s);
#     apl.set("int", 2008);
#     apl.set("int64_t", 0xfeeddeadbeefLL);
#     float f = 3.14159;
#     apl.set("float", f);
#     double d = 2.718281828459045;
#     apl.set("double", d);
#     apl.set<std::string>("char*", "foo");
#     apl.set("char*2", "foo2");
#     apl.set("string", std::string("bar"));
#     apl.set("apl1.pre", 1);
#     apl.set("apl1.post", 2);
#     apl.set("apl2.plus", 10.24);
#     apl.set("apl2.minus", -10.24);
#     apl.set("apl3.sub.subsub", "foo");
#     apl.add("v", 10);
#     apl.add("v", 9);
#     apl.add("v", 8);
# 
#     BOOST_CHECK_EQUAL(apl.toString(),
#         "bool = 1\n"
#         "char = '*'\n"
#         "char* = \"foo\"\n"
#         "char*2 = \"foo2\"\n"
#         "double = 2.71828\n"
#         "float = 3.14159\n"
#         "int = 2008\n"
#         "int64_t = 280297596632815\n"
#         "apl1 = {\n"
#         "..post = 2\n"
#         "..pre = 1\n"
#         "}\n"
#         "apl2 = {\n"
#         "..minus = -10.24\n"
#         "..plus = 10.24\n"
#         "}\n"
#         "apl3 = {\n"
#         "..sub = {\n"
#         "....subsub = \"foo\"\n"
#         "..}\n"
#         "}\n"
#         "short = 42\n"
#         "string = \"bar\"\n"
#         "v = [ 10, 9, 8 ]\n"
#         );
#     BOOST_CHECK_EQUAL(apl.toString(true),
#         "bool = 1\n"
#         "char = '*'\n"
#         "char* = \"foo\"\n"
#         "char*2 = \"foo2\"\n"
#         "double = 2.71828\n"
#         "float = 3.14159\n"
#         "int = 2008\n"
#         "int64_t = 280297596632815\n"
#         "apl1 = { ... }\n"
#         "apl2 = { ... }\n"
#         "apl3 = { ... }\n"
#         "short = 42\n"
#         "string = \"bar\"\n"
#         "v = [ 10, 9, 8 ]\n"
#         );
# }

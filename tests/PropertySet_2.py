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

class PropertySetTestCase(unittest.TestCase):
    """A test case for PropertySet."""

    def testConstruct(self):
        ps = dafBase.PropertySet()
        self.assert_(ps is not None)

    def testScalar(self):
        ps = dafBase.PropertySet()
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        ps.setLongLong("int64_t", 0xfeeddeadbeefL)
        ps.setFloat("float", 3.14159)
        ps.setDouble("double", 2.718281828459045)
        ps.set("char*", "foo")
        ps.setString("string", "bar")
        ps.set("int2", 2009)
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z"))

        self.assertEqual(ps.typeOf("bool"), dafBase.PropertySet.TYPE_Bool)
        self.assertEqual(ps.getBool("bool"), True)
        self.assertEqual(ps.typeOf("short"), dafBase.PropertySet.TYPE_Short)
        self.assertEqual(ps.getShort("short"), 42)
        self.assertEqual(ps.typeOf("int"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.getInt("int"), 2008)
        self.assertEqual(ps.typeOf("int64_t"),
                dafBase.PropertySet.TYPE_LongLong)
        self.assertEqual(ps.getLongLong("int64_t"), 0xfeeddeadbeefL)
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
        self.assertEqual(ps.get("int2"), 2009)
        self.assertEqual(ps.typeOf("dt"), dafBase.PropertySet.TYPE_DateTime)
        self.assertEqual(ps.getDateTime("dt").nsecs(), 1238657233314159265L)

    def testGetDefault(self):
        ps = dafBase.PropertySet()
        ps.setInt("int", 42)
        self.assertEqual(ps.getInt("int"), 42)
        self.assertEqual(ps.getInt("int", 2008), 42)
        self.assertEqual(ps.getInt("foo", 2008), 2008)

    def testExists(self):
        ps = dafBase.PropertySet()
        ps.setInt("int", 42)
        self.assertEqual(ps.exists("int"), True)
        self.assertEqual(ps.exists("foo"), False)

    def testGetVector(self):
        ps = dafBase.PropertySet()
        v = (42, 2008, 1)
        ps.setInt("ints", v)
        ps.setInt("ints2", (10, 9, 8))
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        self.assertEqual(ps.getInt("ints2"), 8)
        self.assertEqual(ps.getArrayInt("ints2"), (10, 9, 8))
        w = ps.get("ints", asArray=True)
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        ps.setInt("int", 999)
        x = ps.get("int")
        self.assertEqual(x, 999)
        x = ps.get("int", asArray=True)
        self.assertEqual(len(x), 1)
        self.assertEqual(x, (999,))

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
        self.assertEqual(ps.getArrayInt("ints2"), (10, 9, 8))

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

    def testDateTimeToString(self):
        ps = dafBase.PropertySet()
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z"))
        self.assertEqual(ps.toString(),
                "dt = 2009-04-02T07:26:39.314159265Z\n")

    def testGetScalarThrow(self):
        ps = dafBase.PropertySet()
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        self.assertRaises(pexExcept.LsstException, ps.get, "foo")

    def testSubPS(self):
        ps = dafBase.PropertySet()
        ps1 = dafBase.PropertySet()
        ps1.set("a", 1)
        ps.setPropertySet("b", ps1)
        self.assertEqual(ps.get("b.a"), 1)
        ps.set("c", ps1)
        self.assertEqual(ps.get("c.a"), 1)
        ps.set("c.a", 2)
        self.assertEqual(ps.get("b.a"), 2)
        self.assertEqual(ps.get("b").get("a"), 2)

class FlatTestCase(unittest.TestCase):
    """A test case for flattened PropertySets."""

    def testConstruct(self):
        ps = dafBase.PropertySet(flat=True)
        self.assert_(ps is not None)

    def testScalar(self):
        ps = dafBase.PropertySet(flat=True)
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        ps.setLongLong("int64_t", 0xfeeddeadbeefL)
        ps.setFloat("float", 3.14159)
        ps.setDouble("double", 2.718281828459045)
        ps.set("char*", "foo")
        ps.setString("string", "bar")
        ps.set("int2", 2009)
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z"))

        self.assertEqual(ps.typeOf("bool"), dafBase.PropertySet.TYPE_Bool)
        self.assertEqual(ps.getBool("bool"), True)
        self.assertEqual(ps.typeOf("short"), dafBase.PropertySet.TYPE_Short)
        self.assertEqual(ps.getShort("short"), 42)
        self.assertEqual(ps.typeOf("int"), dafBase.PropertySet.TYPE_Int)
        self.assertEqual(ps.getInt("int"), 2008)
        self.assertEqual(ps.typeOf("int64_t"),
                dafBase.PropertySet.TYPE_LongLong)
        self.assertEqual(ps.getLongLong("int64_t"), 0xfeeddeadbeefL)
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
        self.assertEqual(ps.get("int2"), 2009)
        self.assertEqual(ps.typeOf("dt"), dafBase.PropertySet.TYPE_DateTime)
        self.assertEqual(ps.getDateTime("dt").nsecs(), 1238657233314159265L)

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
        v = (42, 2008, 1)
        ps.setInt("ints", v)
        ps.setInt("ints2", (10, 9, 8))
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        self.assertEqual(ps.getInt("ints2"), 8)
        self.assertEqual(ps.getArrayInt("ints2"), (10, 9, 8))
        w = ps.get("ints", asArray=True)
        self.assertEqual(len(w), 3)
        self.assertEqual(v, w)
        ps.setInt("int", 999)
        x = ps.get("int")
        self.assertEqual(x, 999)
        x = ps.get("int", asArray=True)
        self.assertEqual(len(x), 1)
        self.assertEqual(x, (999,))

    def testGetVector2(self):
        ps = dafBase.PropertySet(flat=True)
        v = [42, 2008, 1]
        ps.setInt("ints", v)
        ps.setInt("ints2", [10, 9, 8])
        w = ps.getArrayInt("ints")
        self.assertEqual(len(w), 3)
        self.assertEqual(v[0], w[0])
        self.assertEqual(v[1], w[1])
        self.assertEqual(v[2], w[2])
        self.assertEqual(ps.getInt("ints2"), 8)
        self.assertEqual(ps.getArrayInt("ints2"), (10, 9, 8))

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
        ps.set("dt", dafBase.DateTime("20090402T072639.314159265Z"))
        self.assertEqual(ps.toString(),
                "dt = 2009-04-02T07:26:39.314159265Z\n")

    def testGetScalarThrow(self):
        ps = dafBase.PropertySet(flat=True)
        ps.setBool("bool", True)
        ps.setShort("short", 42)
        ps.setInt("int", 2008)
        self.assertRaises(pexExcept.LsstException, ps.get, "foo")

    def testSubPS(self):
        ps = dafBase.PropertySet(flat=True)
        ps1 = dafBase.PropertySet()
        ps1.set("a", 1)
        ps1.add("a", 2)
        ps1.set("foo", "bar")
        ps.setPropertySet("b", ps1)
        self.assertEqual(ps.exists("b.a"), True)
        self.assertEqual(ps.get("b.a"), (1, 2))
        self.assertEqual(ps.exists("b"), False)
        self.assertEqual(ps.exists("b.foo"), True)
        self.assertEqual(ps.get("b.foo"), "bar")

        ps.set("b.c", 20)
        self.assertEqual(ps.exists("b.c"), True)
        self.assertEqual(ps.get("b.c"), 20)
        self.assertEqual(ps.exists("b"), False)



if __name__ == '__main__':
    unittest.main()

# BOOST_AUTO_TEST_CASE(getScalarThrow) {
#     dafBase::PropertySet ps;
#     ps.set("bool", true);
#     short s = 42;
#     ps.set("short", s);
#     ps.set("int", 2008);
#     ps.set("int64_t", 0xfeeddeadbeefLL);
#     float f = 3.14159;
#     ps.set("float", f);
#     double d = 2.718281828459045;
#     ps.set("double", d);
#     ps.set<std::string>("char*", "foo");
#     ps.set("char*2", "foo2");
#     ps.set("string", std::string("bar"));
# 
#     BOOST_CHECK_THROW(ps.get<bool>("short"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<bool>("int"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<short>("int"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<int>("short"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<int>("bool"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<unsigned int>("int"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<double>("float"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<float>("double"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<std::string>("int"), boost::bad_any_cast);
# }
# 
# 
# BOOST_AUTO_TEST_CASE(addVector) {
#     dafBase::PropertySet ps;
#     std::vector<int> v;
#     v.push_back(42);
#     v.push_back(2008);
#     v.push_back(1);
#     ps.set("ints", v);
# 
#     std::vector<int> vv;
#     vv.push_back(-42);
#     vv.push_back(-2008);
#     vv.push_back(-1);
#     ps.add("ints", vv);
# 
#     std::vector<int> w = ps.getArray<int>("ints");
#     BOOST_CHECK_EQUAL(w.size(), 6U);
#     for (int i = 0; i < 3; ++i) {
#         BOOST_CHECK_EQUAL(v[i], w[i]);
#         BOOST_CHECK_EQUAL(vv[i], w[i + 3]);
#     }
# }
# 
# BOOST_AUTO_TEST_CASE(arrayProperties) {
#     dafBase::PropertySet ps;
#     std::vector<int> v;
#     v.push_back(42);
#     v.push_back(2008);
#     v.push_back(1);
#     ps.set("ints", v);
#     ps.set("int", 365);
#     ps.set("ints2", -42);
#     ps.add("ints2", -2008);
# 
#     BOOST_CHECK_EQUAL(ps.isArray("ints"), true);
#     BOOST_CHECK_EQUAL(ps.isArray("int"), false);
#     BOOST_CHECK_EQUAL(ps.isArray("ints2"), true);
#     BOOST_CHECK_EQUAL(ps.valueCount("ints"), 3U);
#     BOOST_CHECK_EQUAL(ps.valueCount("int"), 1U);
#     BOOST_CHECK_EQUAL(ps.valueCount("ints2"), 2U);
#     BOOST_CHECK(ps.typeOf("ints") == typeid(int));
#     BOOST_CHECK(ps.typeOf("int") == typeid(int));
#     BOOST_CHECK(ps.typeOf("ints2") == typeid(int));
# }
# 
# BOOST_AUTO_TEST_CASE(hierarchy) {
#     dafBase::PropertySet ps;
#     dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
# 
#     psp->set("pre", 1);
#     ps.set("ps1", psp);
#     psp->set("post", 2);
#     ps.set("int", 42);
#     ps.set("ps2", dafBase::PropertySet::Ptr(new dafBase::PropertySet));
#     ps.get<dafBase::PropertySet::Ptr>("ps2")->set("plus", 10.24);
#     ps.set("ps2.minus", -10.24);
#     ps.set("ps3.sub1", "foo");
#     ps.set("ps3.sub2", "bar");
# 
#     BOOST_CHECK(ps.exists("ps1"));
#     BOOST_CHECK(ps.exists("ps2"));
#     BOOST_CHECK(ps.exists("ps3"));
#     BOOST_CHECK(ps.exists("ps1.pre"));
#     BOOST_CHECK(ps.exists("ps1.post"));
#     BOOST_CHECK(ps.exists("ps2.plus"));
#     BOOST_CHECK(ps.exists("ps2.minus"));
#     BOOST_CHECK(ps.exists("ps3.sub1"));
#     BOOST_CHECK(ps.exists("ps3.sub2"));
# 
#     BOOST_CHECK(ps.isPropertySetPtr("ps1"));
#     BOOST_CHECK(ps.isPropertySetPtr("ps2"));
#     BOOST_CHECK(ps.isPropertySetPtr("ps3"));
#     BOOST_CHECK(!ps.isPropertySetPtr("int"));
#     BOOST_CHECK(!ps.isPropertySetPtr("ps1.pre"));
#     BOOST_CHECK(!ps.isPropertySetPtr("ps1.post"));
#     BOOST_CHECK(!ps.isPropertySetPtr("ps2.plus"));
#     BOOST_CHECK(!ps.isPropertySetPtr("ps2.minus"));
#     BOOST_CHECK(!ps.isPropertySetPtr("ps3.sub1"));
#     BOOST_CHECK(!ps.isPropertySetPtr("ps3.sub2"));
# 
#     dafBase::PropertySet::Ptr psp1 = ps.get<dafBase::PropertySet::Ptr>("ps1");
#     dafBase::PropertySet::Ptr psp2 = ps.get<dafBase::PropertySet::Ptr>("ps2");
#     dafBase::PropertySet::Ptr psp3 = ps.get<dafBase::PropertySet::Ptr>("ps3");
#     BOOST_CHECK(psp1);
#     BOOST_CHECK(psp2);
#     BOOST_CHECK(psp3);
#     BOOST_CHECK(psp1 == psp);
#     BOOST_CHECK(psp1->exists("pre"));
#     BOOST_CHECK(psp1->exists("post"));
#     BOOST_CHECK(psp2->exists("plus"));
#     BOOST_CHECK(psp2->exists("minus"));
#     BOOST_CHECK(psp3->exists("sub1"));
#     BOOST_CHECK(psp3->exists("sub2"));
#     BOOST_CHECK_EQUAL(psp1->get<int>("pre"), 1);
#     BOOST_CHECK_EQUAL(psp1->get<int>("post"), 2);
#     BOOST_CHECK_EQUAL(psp2->get<double>("plus"), 10.24);
#     BOOST_CHECK_EQUAL(psp2->get<double>("minus"), -10.24);
#     BOOST_CHECK_EQUAL(psp3->get<std::string>("sub1"), "foo");
#     BOOST_CHECK_EQUAL(psp3->get<std::string>("sub2"), "bar");
# 
#     // Make sure checking a subproperty doesn't create it.
#     BOOST_CHECK(!ps.exists("ps2.pre"));
#     BOOST_CHECK(!ps.exists("ps2.pre"));
#     // Make sure checking an element doesn't create it.
#     BOOST_CHECK(!ps.exists("ps4"));
#     BOOST_CHECK(!ps.exists("ps4"));
#     // Make sure checking a subproperty with a nonexistent parent doesn't
#     // create it.
#     BOOST_CHECK(!ps.exists("ps4.sub"));
#     BOOST_CHECK(!ps.exists("ps4.sub"));
#     // Make sure checking a subproperty doesn't create its parent.
#     BOOST_CHECK(!ps.exists("ps4"));
# }
# 
# BOOST_AUTO_TEST_CASE(variousThrows) {
#     dafBase::PropertySet ps;
#     ps.set("int", 42);
#     BOOST_CHECK_THROW(ps.set("int.sub", "foo"),
#                       lsst::pex::exceptions::InvalidParameterException);
#     BOOST_CHECK_THROW(ps.get<double>("int"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.get<double>("double"),
#                       lsst::pex::exceptions::NotFoundException);
#     BOOST_CHECK_THROW(ps.getArray<double>("double"),
#                       lsst::pex::exceptions::NotFoundException);
#     BOOST_CHECK_THROW(ps.typeOf("double"),
#                       lsst::pex::exceptions::NotFoundException);
#     BOOST_CHECK_THROW(ps.add("int", 4.2),
#                       lsst::pex::exceptions::DomainErrorException);
#     std::vector<double> v;
#     v.push_back(3.14159);
#     v.push_back(2.71828);
#     BOOST_CHECK_THROW(ps.add("int", v),
#                       lsst::pex::exceptions::DomainErrorException);
#     BOOST_CHECK_NO_THROW(ps.remove("foo.bar"));
#     BOOST_CHECK_NO_THROW(ps.remove("int.sub"));
# }
# 
# BOOST_AUTO_TEST_CASE(names) {
#     dafBase::PropertySet ps;
#     ps.set("ps1.pre", 1);
#     ps.set("ps1.post", 2);
#     ps.set("int", 42);
#     ps.set("double", 3.14);
#     ps.set("ps2.plus", 10.24);
#     ps.set("ps2.minus", -10.24);
# 
#     BOOST_CHECK_EQUAL(ps.nameCount(), 4U);
#     BOOST_CHECK_EQUAL(ps.nameCount(false), 8U);
# 
#     std::vector<std::string> v = ps.names();
#     BOOST_CHECK_EQUAL(v.size(), 4U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     BOOST_CHECK_EQUAL(v[2], "ps1");
#     BOOST_CHECK_EQUAL(v[3], "ps2");
#     v = ps.names(false);
#     BOOST_CHECK_EQUAL(v.size(), 8U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     BOOST_CHECK_EQUAL(v[2], "ps1");
#     BOOST_CHECK_EQUAL(v[3], "ps1.post");
#     BOOST_CHECK_EQUAL(v[4], "ps1.pre");
#     BOOST_CHECK_EQUAL(v[5], "ps2");
#     BOOST_CHECK_EQUAL(v[6], "ps2.minus");
#     BOOST_CHECK_EQUAL(v[7], "ps2.plus");
# }
# 
# BOOST_AUTO_TEST_CASE(paramNames) {
#     dafBase::PropertySet ps;
#     ps.set("ps1.pre", 1);
#     ps.set("ps1.post", 2);
#     ps.set("int", 42);
#     ps.set("double", 3.14);
#     ps.set("ps2.plus", 10.24);
#     ps.set("ps2.minus", -10.24);
# 
#     std::vector<std::string> v = ps.paramNames();
#     BOOST_CHECK_EQUAL(v.size(), 2U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     v = ps.paramNames(false);
#     BOOST_CHECK_EQUAL(v.size(), 6U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "double");
#     BOOST_CHECK_EQUAL(v[1], "int");
#     BOOST_CHECK_EQUAL(v[2], "ps1.post");
#     BOOST_CHECK_EQUAL(v[3], "ps1.pre");
#     BOOST_CHECK_EQUAL(v[4], "ps2.minus");
#     BOOST_CHECK_EQUAL(v[5], "ps2.plus");
# }
# 
# BOOST_AUTO_TEST_CASE(propertySetNames) {
#     dafBase::PropertySet ps;
#     ps.set("ps1.pre", 1);
#     ps.set("ps1.post", 2);
#     ps.set("int", 42);
#     ps.set("double", 3.14);
#     ps.set("ps2.plus", 10.24);
#     ps.set("ps2.minus", -10.24);
#     ps.set("ps3.sub.subsub", "foo");
# 
#     std::vector<std::string> v = ps.propertySetNames();
#     BOOST_CHECK_EQUAL(v.size(), 3U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "ps1");
#     BOOST_CHECK_EQUAL(v[1], "ps2");
#     BOOST_CHECK_EQUAL(v[2], "ps3");
#     v = ps.propertySetNames(false);
#     BOOST_CHECK_EQUAL(v.size(), 4U);
#     std::sort(v.begin(), v.end());
#     BOOST_CHECK_EQUAL(v[0], "ps1");
#     BOOST_CHECK_EQUAL(v[1], "ps2");
#     BOOST_CHECK_EQUAL(v[2], "ps3");
#     BOOST_CHECK_EQUAL(v[3], "ps3.sub");
# }
# 
# BOOST_AUTO_TEST_CASE(getAs) {
#     dafBase::PropertySet ps;
#     ps.set("bool", true);
#     ps.set("char", 'A');
#     short s = 42;
#     ps.set("short", s);
#     ps.set("int", 2008);
#     ps.set("int64_t", 0xfeeddeadbeefLL);
#     float f = 3.14159;
#     ps.set("float", f);
#     double d = 2.718281828459045;
#     ps.set("double", d);
#     ps.set<std::string>("char*", "foo");
#     ps.set("char*2", "foo2");
#     ps.set("string", std::string("bar"));
#     dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
#     psp->set("bottom", "x");
#     ps.set("top", psp);
# 
#     BOOST_CHECK_EQUAL(ps.getAsBool("bool"), true);
#     BOOST_CHECK_THROW(ps.getAsBool("char"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(ps.getAsInt("bool"), 1);
#     BOOST_CHECK_EQUAL(ps.getAsInt("char"), static_cast<int>('A'));
#     BOOST_CHECK_EQUAL(ps.getAsInt("short"), 42);
#     BOOST_CHECK_EQUAL(ps.getAsInt("int"), 2008);
#     BOOST_CHECK_THROW(ps.getAsInt("int64_t"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(ps.getAsInt64("bool"), 1LL);
#     BOOST_CHECK_EQUAL(ps.getAsInt64("char"), static_cast<int64_t>('A'));
#     BOOST_CHECK_EQUAL(ps.getAsInt64("short"), 42LL);
#     BOOST_CHECK_EQUAL(ps.getAsInt64("int"), 2008LL);
#     BOOST_CHECK_EQUAL(ps.getAsInt64("int64_t"), 0xfeeddeadbeefLL);
#     BOOST_CHECK_THROW(ps.getAsInt64("float"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(ps.getAsDouble("bool"), 1.0);
#     BOOST_CHECK_EQUAL(ps.getAsDouble("char"), static_cast<double>('A'));
#     BOOST_CHECK_EQUAL(ps.getAsDouble("short"), 42.0);
#     BOOST_CHECK_EQUAL(ps.getAsDouble("int"), 2008.0);
#     BOOST_CHECK_EQUAL(ps.getAsDouble("int64_t"),
#                       static_cast<double>(0xfeeddeadbeefLL));
#     BOOST_CHECK_EQUAL(ps.getAsDouble("float"), 3.14159f);
#     BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 2.718281828459045);
#     BOOST_CHECK_THROW(ps.getAsDouble("char*"), boost::bad_any_cast);
#     BOOST_CHECK_THROW(ps.getAsString("char"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(ps.getAsString("char*"), "foo");
#     BOOST_CHECK_EQUAL(ps.getAsString("char*2"), "foo2");
#     BOOST_CHECK_EQUAL(ps.getAsString("string"), "bar");
#     BOOST_CHECK_THROW(ps.getAsString("int"), boost::bad_any_cast);
#     BOOST_CHECK_EQUAL(ps.getAsString("top.bottom"), "x");
#     BOOST_CHECK_EQUAL(ps.getAsPropertySetPtr("top"), psp);
#     BOOST_CHECK_THROW(ps.getAsPropertySetPtr("top.bottom"),
#                       boost::bad_any_cast);
# }
# 
# BOOST_AUTO_TEST_CASE(combine) {
#     dafBase::PropertySet ps;
#     ps.set("ps1.pre", 1);
#     ps.set("ps1.post", 2);
#     ps.set("int", 42);
#     ps.set("double", 3.14);
#     ps.set("ps2.plus", 10.24);
#     ps.set("ps2.minus", -10.24);
#     ps.set("ps3.sub.subsub", "foo");
# 
#     dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
#     psp->set("ps1.pre", 3);
#     psp->add("ps1.pre", 4);
#     psp->set("int", 2008);
#     psp->set("ps2.foo", "bar");
#     psp->set("ps4.top", "bottom");
# 
#     ps.combine(psp);
# 
#     BOOST_CHECK(ps.isPropertySetPtr("ps1"));
#     BOOST_CHECK(ps.isPropertySetPtr("ps2"));
#     BOOST_CHECK(ps.isPropertySetPtr("ps3"));
#     BOOST_CHECK(ps.isPropertySetPtr("ps3.sub"));
#     BOOST_CHECK(ps.isPropertySetPtr("ps4"));
#     BOOST_CHECK(!ps.isArray("ps1"));
#     BOOST_CHECK(ps.isArray("ps1.pre"));
#     BOOST_CHECK(!ps.isArray("ps1.post"));
#     BOOST_CHECK(!ps.isArray("ps2"));
#     BOOST_CHECK(!ps.isArray("ps2.plus"));
#     BOOST_CHECK(!ps.isArray("ps2.minus"));
#     BOOST_CHECK(!ps.isArray("ps2.foo"));
#     BOOST_CHECK(!ps.isArray("ps3"));
#     BOOST_CHECK(!ps.isArray("ps3.sub"));
#     BOOST_CHECK(!ps.isArray("ps3.subsub"));
#     BOOST_CHECK(!ps.isArray("ps4"));
#     BOOST_CHECK(!ps.isArray("ps4.top"));
#     BOOST_CHECK(ps.isArray("int"));
#     BOOST_CHECK(!ps.isArray("double"));
#     BOOST_CHECK_EQUAL(ps.valueCount("ps1.pre"), 3U);
#     BOOST_CHECK_EQUAL(ps.valueCount("int"), 2U);
#     std::vector<int> v = ps.getArray<int>("ps1.pre");
#     BOOST_CHECK_EQUAL(v[0], 1);
#     BOOST_CHECK_EQUAL(v[1], 3);
#     BOOST_CHECK_EQUAL(v[2], 4);
#     v = ps.getArray<int>("int");
#     BOOST_CHECK_EQUAL(v[0], 42);
#     BOOST_CHECK_EQUAL(v[1], 2008);
# }
# 
# BOOST_AUTO_TEST_CASE(combineThrow) {
#     dafBase::PropertySet ps;
#     ps.set("int", 42);
# 
#     dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
#     psp->set("int", 3.14159);
# 
#     BOOST_CHECK_THROW(ps.combine(psp),
#                       lsst::pex::exceptions::DomainErrorException);
# }
# 
# BOOST_AUTO_TEST_CASE(remove) {
#     dafBase::PropertySet ps;
#     ps.set("int", 42);
#     ps.set("double", 3.14159);
#     ps.set("ps1.plus", 1);
#     ps.set("ps1.minus", -1);
#     ps.set("ps1.zero", 0);
#     BOOST_CHECK_EQUAL(ps.nameCount(false), 6U);
# 
#     ps.remove("int");
#     BOOST_CHECK(!ps.exists("int"));
#     BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 3.14159);
#     BOOST_CHECK_EQUAL(ps.getAsInt("ps1.plus"), 1);
#     BOOST_CHECK_EQUAL(ps.getAsInt("ps1.minus"), -1);
#     BOOST_CHECK_EQUAL(ps.getAsInt("ps1.zero"), 0);
#     BOOST_CHECK_EQUAL(ps.nameCount(false), 5U);
# 
#     ps.remove("ps1.zero");
#     BOOST_CHECK(!ps.exists("int"));
#     BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 3.14159);
#     BOOST_CHECK(!ps.exists("ps1.zero"));
#     BOOST_CHECK_EQUAL(ps.getAsInt("ps1.plus"), 1);
#     BOOST_CHECK_EQUAL(ps.getAsInt("ps1.minus"), -1);
#     BOOST_CHECK_EQUAL(ps.nameCount(false), 4U);
# 
#     ps.remove("ps1");
#     BOOST_CHECK(!ps.exists("int"));
#     BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 3.14159);
#     BOOST_CHECK(!ps.exists("ps1"));
#     BOOST_CHECK(!ps.exists("ps1.plus"));
#     BOOST_CHECK(!ps.exists("ps1.minus"));
#     BOOST_CHECK(!ps.exists("ps1.zero"));
#     BOOST_CHECK_EQUAL(ps.nameCount(false), 1U);
# 
#     ps.remove("double");
#     BOOST_CHECK(!ps.exists("int"));
#     BOOST_CHECK(!ps.exists("double"));
#     BOOST_CHECK(!ps.exists("ps1"));
#     BOOST_CHECK(!ps.exists("ps1.plus"));
#     BOOST_CHECK(!ps.exists("ps1.minus"));
#     BOOST_CHECK(!ps.exists("ps1.zero"));
#     BOOST_CHECK_EQUAL(ps.nameCount(false), 0U);
# }
# 
# BOOST_AUTO_TEST_CASE(deepCopy) {
#     dafBase::PropertySet ps;
#     ps.set("int", 42);
#     dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
#     psp->set("bottom", "x");
#     ps.set("top", psp);
# 
#     dafBase::PropertySet::Ptr psp2 = ps.deepCopy();
#     BOOST_CHECK(psp2->exists("int"));
#     BOOST_CHECK(psp2->exists("top.bottom"));
#     BOOST_CHECK_EQUAL(psp2->getAsInt("int"), 42);
#     BOOST_CHECK_EQUAL(psp2->getAsString("top.bottom"), "x");
#     // Make sure it was indeed a deep copy.
#     BOOST_CHECK(psp2->getAsPropertySetPtr("top") != psp);
#     ps.set("int", 2008);
#     ps.set("top.bottom", "y");
#     BOOST_CHECK_EQUAL(ps.getAsInt("int"), 2008);
#     BOOST_CHECK_EQUAL(ps.getAsString("top.bottom"), "y");
#     BOOST_CHECK_EQUAL(psp->getAsString("bottom"), "y");
#     BOOST_CHECK_EQUAL(psp2->getAsInt("int"), 42);
#     BOOST_CHECK_EQUAL(psp2->getAsString("top.bottom"), "x");
# }
# 
# BOOST_AUTO_TEST_CASE(toString) {
#     dafBase::PropertySet ps;
#     ps.set("bool", true);
#     ps.set("char", '*');
#     short s = 42;
#     ps.set("short", s);
#     ps.set("int", 2008);
#     ps.set("int64_t", 0xfeeddeadbeefLL);
#     float f = 3.14159;
#     ps.set("float", f);
#     double d = 2.718281828459045;
#     ps.set("double", d);
#     ps.set<std::string>("char*", "foo");
#     ps.set("char*2", "foo2");
#     ps.set("string", std::string("bar"));
#     ps.set("ps1.pre", 1);
#     ps.set("ps1.post", 2);
#     ps.set("ps2.plus", 10.24);
#     ps.set("ps2.minus", -10.24);
#     ps.set("ps3.sub.subsub", "foo");
#     ps.add("v", 10);
#     ps.add("v", 9);
#     ps.add("v", 8);
# 
#     BOOST_CHECK_EQUAL(ps.toString(),
#         "bool = 1\n"
#         "char = '*'\n"
#         "char* = \"foo\"\n"
#         "char*2 = \"foo2\"\n"
#         "double = 2.71828\n"
#         "float = 3.14159\n"
#         "int = 2008\n"
#         "int64_t = 280297596632815\n"
#         "ps1 = {\n"
#         "..post = 2\n"
#         "..pre = 1\n"
#         "}\n"
#         "ps2 = {\n"
#         "..minus = -10.24\n"
#         "..plus = 10.24\n"
#         "}\n"
#         "ps3 = {\n"
#         "..sub = {\n"
#         "....subsub = \"foo\"\n"
#         "..}\n"
#         "}\n"
#         "short = 42\n"
#         "string = \"bar\"\n"
#         "v = [ 10, 9, 8 ]\n"
#         );
#     BOOST_CHECK_EQUAL(ps.toString(true),
#         "bool = 1\n"
#         "char = '*'\n"
#         "char* = \"foo\"\n"
#         "char*2 = \"foo2\"\n"
#         "double = 2.71828\n"
#         "float = 3.14159\n"
#         "int = 2008\n"
#         "int64_t = 280297596632815\n"
#         "ps1 = { ... }\n"
#         "ps2 = { ... }\n"
#         "ps3 = { ... }\n"
#         "short = 42\n"
#         "string = \"bar\"\n"
#         "v = [ 10, 9, 8 ]\n"
#         );
# }

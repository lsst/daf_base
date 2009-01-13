#include "lsst/daf/base/PropertySet.h"

#define BOOST_TEST_MODULE PropertySet_1
#include "boost/test/included/unit_test.hpp"

namespace test = boost::test_tools;
namespace dafBase = lsst::daf::base;

#include <algorithm>

#include "lsst/pex/exceptions/Runtime.h"

BOOST_AUTO_TEST_SUITE(PropertySetSuite)

BOOST_AUTO_TEST_CASE(construct) {
    dafBase::PropertySet ps;
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    BOOST_CHECK_EQUAL(!psp, false);
}

BOOST_AUTO_TEST_CASE(bases) {
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    boost::shared_ptr<dafBase::Persistable> pp =
        boost::dynamic_pointer_cast<dafBase::Persistable, dafBase::PropertySet>(psp);
    BOOST_CHECK_EQUAL(!pp, false);
    boost::shared_ptr<dafBase::Citizen> cp =
        boost::dynamic_pointer_cast<dafBase::Citizen, dafBase::PropertySet>(psp);
    BOOST_CHECK_EQUAL(!cp, false);
}

BOOST_AUTO_TEST_CASE(getScalar) {
    dafBase::PropertySet ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", 0xfeeddeadbeefLL);
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("char*2", "foo2");
    ps.set("string", std::string("bar"));

    BOOST_CHECK_EQUAL(ps.get<bool>("bool"), true);
    BOOST_CHECK_EQUAL(ps.get<char>("char"), '*');
    BOOST_CHECK_EQUAL(ps.get<short>("short"), 42);
    BOOST_CHECK_EQUAL(ps.get<int>("int"), 2008);
    BOOST_CHECK_EQUAL(ps.get<int64_t>("int64_t"), 0xfeeddeadbeefLL);
    BOOST_CHECK_EQUAL(ps.get<float>("float"), 3.14159f);
    BOOST_CHECK_EQUAL(ps.get<double>("double"), 2.718281828459045);
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*"), "foo");
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*2"), "foo2");
    BOOST_CHECK_EQUAL(ps.get<std::string>("string"), "bar");
}

BOOST_AUTO_TEST_CASE(resetScalar) {
    dafBase::PropertySet ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", 0xfeeddeadbeefLL);
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("char*2", "foo2");
    ps.set("string", std::string("bar"));

    BOOST_CHECK_EQUAL(ps.get<bool>("bool"), true);
    BOOST_CHECK_EQUAL(ps.get<char>("char"), '*');
    BOOST_CHECK_EQUAL(ps.get<short>("short"), 42);
    BOOST_CHECK_EQUAL(ps.get<int>("int"), 2008);
    BOOST_CHECK_EQUAL(ps.get<int64_t>("int64_t"), 0xfeeddeadbeefLL);
    BOOST_CHECK_EQUAL(ps.get<float>("float"), 3.14159f);
    BOOST_CHECK_EQUAL(ps.get<double>("double"), 2.718281828459045);
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*"), "foo");
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*2"), "foo2");
    BOOST_CHECK_EQUAL(ps.get<std::string>("string"), "bar");

    ps.set("bool", false);
    ps.set("char", '%');
    s = 2008;
    ps.set("short", s);
    ps.set("int", 42);
    ps.set("int64_t", 0xcafefacadeLL);
    f = 2.71828;
    ps.set("float", f);
    d = 3.1415926535897932;
    ps.set("double", d);
    ps.set<std::string>("char*", "baz");
    ps.set("char*2", "random2");
    ps.set("string", std::string("xyzzy"));

    BOOST_CHECK_EQUAL(ps.get<bool>("bool"), false);
    BOOST_CHECK_EQUAL(ps.get<char>("char"), '%');
    BOOST_CHECK_EQUAL(ps.get<short>("short"), 2008);
    BOOST_CHECK_EQUAL(ps.get<int>("int"), 42);
    BOOST_CHECK_EQUAL(ps.get<int64_t>("int64_t"), 0xcafefacadeLL);
    BOOST_CHECK_EQUAL(ps.get<float>("float"), 2.71828f);
    BOOST_CHECK_EQUAL(ps.get<double>("double"), 3.1415926535897932);
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*"), "baz");
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*2"), "random2");
    BOOST_CHECK_EQUAL(ps.get<std::string>("string"), "xyzzy");
}

BOOST_AUTO_TEST_CASE(getDefault) {
    dafBase::PropertySet ps;
    ps.set("int", 42);

    BOOST_CHECK_EQUAL(ps.get<int>("int"), 42);
    BOOST_CHECK_EQUAL(ps.get<int>("int", 2008), 42);
    BOOST_CHECK_EQUAL(ps.get<int>("foo", 2008), 2008);
}

BOOST_AUTO_TEST_CASE(exists) {
    dafBase::PropertySet ps;
    ps.set("int", 42);
    BOOST_CHECK_EQUAL(ps.exists("int"), true);
    BOOST_CHECK_EQUAL(ps.exists("foo"), false);
}

BOOST_AUTO_TEST_CASE(getScalarThrow) {
    dafBase::PropertySet ps;
    ps.set("bool", true);
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", 0xfeeddeadbeefLL);
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("char*2", "foo2");
    ps.set("string", std::string("bar"));

    BOOST_CHECK_THROW(ps.get<bool>("short"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<bool>("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<short>("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<int>("short"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<int>("bool"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<unsigned int>("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<double>("float"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<float>("double"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<std::string>("int"), dafBase::TypeMismatchException);
}

BOOST_AUTO_TEST_CASE(getVector) {
    dafBase::PropertySet ps;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    ps.set("ints", v);

    std::vector<int> w = ps.getArray<int>("ints");
    BOOST_CHECK_EQUAL(w.size(), 3U);
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_EQUAL(v[i], w[i]);
    }
}

BOOST_AUTO_TEST_CASE(addScalar) {
    dafBase::PropertySet ps;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    ps.set("ints", v);
    ps.add("ints", -999);
    ps.add("other", "foo");

    std::vector<int> w = ps.getArray<int>("ints");
    BOOST_CHECK_EQUAL(w.size(), 4U);
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_EQUAL(v[i], w[i]);
    }
    BOOST_CHECK_EQUAL(w[3], -999);
    BOOST_CHECK_EQUAL(ps.get<std::string>("other"), "foo");
}

BOOST_AUTO_TEST_CASE(addVector) {
    dafBase::PropertySet ps;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    ps.set("ints", v);

    std::vector<int> vv;
    vv.push_back(-42);
    vv.push_back(-2008);
    vv.push_back(-1);
    ps.add("ints", vv);

    std::vector<int> w = ps.getArray<int>("ints");
    BOOST_CHECK_EQUAL(w.size(), 6U);
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_EQUAL(v[i], w[i]);
        BOOST_CHECK_EQUAL(vv[i], w[i + 3]);
    }
}

BOOST_AUTO_TEST_CASE(typeOf) {
    dafBase::PropertySet ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", 0xfeeddeadbeefLL);
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set("char*", "foo");
    ps.set("string", std::string("bar"));

    BOOST_CHECK(ps.typeOf("bool") == typeid(bool));
    BOOST_CHECK(ps.typeOf("char") == typeid(char));
    BOOST_CHECK(ps.typeOf("short") == typeid(short));
    BOOST_CHECK(ps.typeOf("int") == typeid(int));
    BOOST_CHECK(ps.typeOf("int64_t") == typeid(int64_t));
    BOOST_CHECK(ps.typeOf("float") == typeid(float));
    BOOST_CHECK(ps.typeOf("double") == typeid(double));
    BOOST_CHECK(ps.typeOf("char*") == typeid(std::string));
    BOOST_CHECK(ps.typeOf("string") == typeid(std::string));
}

BOOST_AUTO_TEST_CASE(arrayProperties) {
    dafBase::PropertySet ps;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    ps.set("ints", v);
    ps.set("int", 365);
    ps.set("ints2", -42);
    ps.add("ints2", -2008);

    BOOST_CHECK_EQUAL(ps.isArray("ints"), true);
    BOOST_CHECK_EQUAL(ps.isArray("int"), false);
    BOOST_CHECK_EQUAL(ps.isArray("ints2"), true);
    BOOST_CHECK_EQUAL(ps.valueCount("ints"), 3U);
    BOOST_CHECK_EQUAL(ps.valueCount("int"), 1U);
    BOOST_CHECK_EQUAL(ps.valueCount("ints2"), 2U);
    BOOST_CHECK(ps.typeOf("ints") == typeid(int));
    BOOST_CHECK(ps.typeOf("int") == typeid(int));
    BOOST_CHECK(ps.typeOf("ints2") == typeid(int));
}

BOOST_AUTO_TEST_CASE(hierarchy) {
    dafBase::PropertySet ps;
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);

    psp->set("pre", 1);
    ps.set("ps1", psp);
    psp->set("post", 2);
    ps.set("int", 42);
    ps.set("ps2", dafBase::PropertySet::Ptr(new dafBase::PropertySet));
    ps.get<dafBase::PropertySet::Ptr>("ps2")->set("plus", 10.24);
    ps.set("ps2.minus", -10.24);
    ps.set("ps3.sub1", "foo");
    ps.set("ps3.sub2", "bar");

    BOOST_CHECK(ps.exists("ps1"));
    BOOST_CHECK(ps.exists("ps2"));
    BOOST_CHECK(ps.exists("ps3"));
    BOOST_CHECK(ps.exists("ps1.pre"));
    BOOST_CHECK(ps.exists("ps1.post"));
    BOOST_CHECK(ps.exists("ps2.plus"));
    BOOST_CHECK(ps.exists("ps2.minus"));
    BOOST_CHECK(ps.exists("ps3.sub1"));
    BOOST_CHECK(ps.exists("ps3.sub2"));

    BOOST_CHECK(ps.isPropertySetPtr("ps1"));
    BOOST_CHECK(ps.isPropertySetPtr("ps2"));
    BOOST_CHECK(ps.isPropertySetPtr("ps3"));
    BOOST_CHECK(!ps.isPropertySetPtr("int"));
    BOOST_CHECK(!ps.isPropertySetPtr("ps1.pre"));
    BOOST_CHECK(!ps.isPropertySetPtr("ps1.post"));
    BOOST_CHECK(!ps.isPropertySetPtr("ps2.plus"));
    BOOST_CHECK(!ps.isPropertySetPtr("ps2.minus"));
    BOOST_CHECK(!ps.isPropertySetPtr("ps3.sub1"));
    BOOST_CHECK(!ps.isPropertySetPtr("ps3.sub2"));

    dafBase::PropertySet::Ptr psp1 = ps.get<dafBase::PropertySet::Ptr>("ps1");
    dafBase::PropertySet::Ptr psp2 = ps.get<dafBase::PropertySet::Ptr>("ps2");
    dafBase::PropertySet::Ptr psp3 = ps.get<dafBase::PropertySet::Ptr>("ps3");
    BOOST_CHECK(psp1);
    BOOST_CHECK(psp2);
    BOOST_CHECK(psp3);
    BOOST_CHECK(psp1 == psp);
    BOOST_CHECK(psp1->exists("pre"));
    BOOST_CHECK(psp1->exists("post"));
    BOOST_CHECK(psp2->exists("plus"));
    BOOST_CHECK(psp2->exists("minus"));
    BOOST_CHECK(psp3->exists("sub1"));
    BOOST_CHECK(psp3->exists("sub2"));
    BOOST_CHECK_EQUAL(psp1->get<int>("pre"), 1);
    BOOST_CHECK_EQUAL(psp1->get<int>("post"), 2);
    BOOST_CHECK_EQUAL(psp2->get<double>("plus"), 10.24);
    BOOST_CHECK_EQUAL(psp2->get<double>("minus"), -10.24);
    BOOST_CHECK_EQUAL(psp3->get<std::string>("sub1"), "foo");
    BOOST_CHECK_EQUAL(psp3->get<std::string>("sub2"), "bar");

    // Make sure checking a subproperty doesn't create it.
    BOOST_CHECK(!ps.exists("ps2.pre"));
    BOOST_CHECK(!ps.exists("ps2.pre"));
    // Make sure checking an element doesn't create it.
    BOOST_CHECK(!ps.exists("ps4"));
    BOOST_CHECK(!ps.exists("ps4"));
    // Make sure checking a subproperty with a nonexistent parent doesn't
    // create it.
    BOOST_CHECK(!ps.exists("ps4.sub"));
    BOOST_CHECK(!ps.exists("ps4.sub"));
    // Make sure checking a subproperty doesn't create its parent.
    BOOST_CHECK(!ps.exists("ps4"));
}

BOOST_AUTO_TEST_CASE(variousThrows) {
    dafBase::PropertySet ps;
    ps.set("int", 42);
    BOOST_CHECK_THROW(ps.set("int.sub", "foo"),
                      lsst::pex::exceptions::InvalidParameterException);
    BOOST_CHECK_THROW(ps.get<double>("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.get<double>("double"),
                      lsst::pex::exceptions::NotFoundException);
    BOOST_CHECK_THROW(ps.getArray<double>("double"),
                      lsst::pex::exceptions::NotFoundException);
    BOOST_CHECK_THROW(ps.typeOf("double"),
                      lsst::pex::exceptions::NotFoundException);
    BOOST_CHECK_THROW(ps.add("int", 4.2), dafBase::TypeMismatchException);
    std::vector<double> v;
    v.push_back(3.14159);
    v.push_back(2.71828);
    BOOST_CHECK_THROW(ps.add("int", v), dafBase::TypeMismatchException);
    BOOST_CHECK_NO_THROW(ps.remove("foo.bar"));
    BOOST_CHECK_NO_THROW(ps.remove("int.sub"));
}

BOOST_AUTO_TEST_CASE(names) {
    dafBase::PropertySet ps;
    ps.set("ps1.pre", 1);
    ps.set("ps1.post", 2);
    ps.set("int", 42);
    ps.set("double", 3.14);
    ps.set("ps2.plus", 10.24);
    ps.set("ps2.minus", -10.24);

    BOOST_CHECK_EQUAL(ps.nameCount(), 4U);
    BOOST_CHECK_EQUAL(ps.nameCount(false), 8U);

    std::vector<std::string> v = ps.names();
    BOOST_CHECK_EQUAL(v.size(), 4U);
    std::sort(v.begin(), v.end());
    BOOST_CHECK_EQUAL(v[0], "double");
    BOOST_CHECK_EQUAL(v[1], "int");
    BOOST_CHECK_EQUAL(v[2], "ps1");
    BOOST_CHECK_EQUAL(v[3], "ps2");
    v = ps.names(false);
    BOOST_CHECK_EQUAL(v.size(), 8U);
    std::sort(v.begin(), v.end());
    BOOST_CHECK_EQUAL(v[0], "double");
    BOOST_CHECK_EQUAL(v[1], "int");
    BOOST_CHECK_EQUAL(v[2], "ps1");
    BOOST_CHECK_EQUAL(v[3], "ps1.post");
    BOOST_CHECK_EQUAL(v[4], "ps1.pre");
    BOOST_CHECK_EQUAL(v[5], "ps2");
    BOOST_CHECK_EQUAL(v[6], "ps2.minus");
    BOOST_CHECK_EQUAL(v[7], "ps2.plus");
}

BOOST_AUTO_TEST_CASE(paramNames) {
    dafBase::PropertySet ps;
    ps.set("ps1.pre", 1);
    ps.set("ps1.post", 2);
    ps.set("int", 42);
    ps.set("double", 3.14);
    ps.set("ps2.plus", 10.24);
    ps.set("ps2.minus", -10.24);

    std::vector<std::string> v = ps.paramNames();
    BOOST_CHECK_EQUAL(v.size(), 2U);
    std::sort(v.begin(), v.end());
    BOOST_CHECK_EQUAL(v[0], "double");
    BOOST_CHECK_EQUAL(v[1], "int");
    v = ps.paramNames(false);
    BOOST_CHECK_EQUAL(v.size(), 6U);
    std::sort(v.begin(), v.end());
    BOOST_CHECK_EQUAL(v[0], "double");
    BOOST_CHECK_EQUAL(v[1], "int");
    BOOST_CHECK_EQUAL(v[2], "ps1.post");
    BOOST_CHECK_EQUAL(v[3], "ps1.pre");
    BOOST_CHECK_EQUAL(v[4], "ps2.minus");
    BOOST_CHECK_EQUAL(v[5], "ps2.plus");
}

BOOST_AUTO_TEST_CASE(propertySetNames) {
    dafBase::PropertySet ps;
    ps.set("ps1.pre", 1);
    ps.set("ps1.post", 2);
    ps.set("int", 42);
    ps.set("double", 3.14);
    ps.set("ps2.plus", 10.24);
    ps.set("ps2.minus", -10.24);
    ps.set("ps3.sub.subsub", "foo");

    std::vector<std::string> v = ps.propertySetNames();
    BOOST_CHECK_EQUAL(v.size(), 3U);
    std::sort(v.begin(), v.end());
    BOOST_CHECK_EQUAL(v[0], "ps1");
    BOOST_CHECK_EQUAL(v[1], "ps2");
    BOOST_CHECK_EQUAL(v[2], "ps3");
    v = ps.propertySetNames(false);
    BOOST_CHECK_EQUAL(v.size(), 4U);
    std::sort(v.begin(), v.end());
    BOOST_CHECK_EQUAL(v[0], "ps1");
    BOOST_CHECK_EQUAL(v[1], "ps2");
    BOOST_CHECK_EQUAL(v[2], "ps3");
    BOOST_CHECK_EQUAL(v[3], "ps3.sub");
}

BOOST_AUTO_TEST_CASE(getAs) {
    dafBase::PropertySet ps;
    ps.set("bool", true);
    ps.set("char", 'A');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", 0xfeeddeadbeefLL);
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("char*2", "foo2");
    ps.set("string", std::string("bar"));
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    psp->set("bottom", "x");
    ps.set("top", psp);

    BOOST_CHECK_EQUAL(ps.getAsBool("bool"), true);
    BOOST_CHECK_THROW(ps.getAsBool("char"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsInt("bool"), 1);
    BOOST_CHECK_EQUAL(ps.getAsInt("char"), static_cast<int>('A'));
    BOOST_CHECK_EQUAL(ps.getAsInt("short"), 42);
    BOOST_CHECK_EQUAL(ps.getAsInt("int"), 2008);
    BOOST_CHECK_THROW(ps.getAsInt("int64_t"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsInt64("bool"), 1LL);
    BOOST_CHECK_EQUAL(ps.getAsInt64("char"), static_cast<int64_t>('A'));
    BOOST_CHECK_EQUAL(ps.getAsInt64("short"), 42LL);
    BOOST_CHECK_EQUAL(ps.getAsInt64("int"), 2008LL);
    BOOST_CHECK_EQUAL(ps.getAsInt64("int64_t"), 0xfeeddeadbeefLL);
    BOOST_CHECK_THROW(ps.getAsInt64("float"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsDouble("bool"), 1.0);
    BOOST_CHECK_EQUAL(ps.getAsDouble("char"), static_cast<double>('A'));
    BOOST_CHECK_EQUAL(ps.getAsDouble("short"), 42.0);
    BOOST_CHECK_EQUAL(ps.getAsDouble("int"), 2008.0);
    BOOST_CHECK_EQUAL(ps.getAsDouble("int64_t"),
                      static_cast<double>(0xfeeddeadbeefLL));
    BOOST_CHECK_EQUAL(ps.getAsDouble("float"), 3.14159f);
    BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 2.718281828459045);
    BOOST_CHECK_THROW(ps.getAsDouble("char*"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.getAsString("char"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsString("char*"), "foo");
    BOOST_CHECK_EQUAL(ps.getAsString("char*2"), "foo2");
    BOOST_CHECK_EQUAL(ps.getAsString("string"), "bar");
    BOOST_CHECK_THROW(ps.getAsString("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsString("top.bottom"), "x");
    BOOST_CHECK_EQUAL(ps.getAsPropertySetPtr("top"), psp);
    BOOST_CHECK_THROW(ps.getAsPropertySetPtr("top.bottom"),
                      dafBase::TypeMismatchException);
}

BOOST_AUTO_TEST_CASE(combine) {
    dafBase::PropertySet ps;
    ps.set("ps1.pre", 1);
    ps.set("ps1.post", 2);
    ps.set("int", 42);
    ps.set("double", 3.14);
    ps.set("ps2.plus", 10.24);
    ps.set("ps2.minus", -10.24);
    ps.set("ps3.sub.subsub", "foo");

    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    psp->set("ps1.pre", 3);
    psp->add("ps1.pre", 4);
    psp->set("int", 2008);
    psp->set("ps2.foo", "bar");
    psp->set("ps4.top", "bottom");

    ps.combine(psp);

    BOOST_CHECK(ps.isPropertySetPtr("ps1"));
    BOOST_CHECK(ps.isPropertySetPtr("ps2"));
    BOOST_CHECK(ps.isPropertySetPtr("ps3"));
    BOOST_CHECK(ps.isPropertySetPtr("ps3.sub"));
    BOOST_CHECK(ps.isPropertySetPtr("ps4"));
    BOOST_CHECK(!ps.isArray("ps1"));
    BOOST_CHECK(ps.isArray("ps1.pre"));
    BOOST_CHECK(!ps.isArray("ps1.post"));
    BOOST_CHECK(!ps.isArray("ps2"));
    BOOST_CHECK(!ps.isArray("ps2.plus"));
    BOOST_CHECK(!ps.isArray("ps2.minus"));
    BOOST_CHECK(!ps.isArray("ps2.foo"));
    BOOST_CHECK(!ps.isArray("ps3"));
    BOOST_CHECK(!ps.isArray("ps3.sub"));
    BOOST_CHECK(!ps.isArray("ps3.subsub"));
    BOOST_CHECK(!ps.isArray("ps4"));
    BOOST_CHECK(!ps.isArray("ps4.top"));
    BOOST_CHECK(ps.isArray("int"));
    BOOST_CHECK(!ps.isArray("double"));
    BOOST_CHECK_EQUAL(ps.valueCount("ps1.pre"), 3U);
    BOOST_CHECK_EQUAL(ps.valueCount("int"), 2U);
    std::vector<int> v = ps.getArray<int>("ps1.pre");
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 3);
    BOOST_CHECK_EQUAL(v[2], 4);
    v = ps.getArray<int>("int");
    BOOST_CHECK_EQUAL(v[0], 42);
    BOOST_CHECK_EQUAL(v[1], 2008);
}

BOOST_AUTO_TEST_CASE(combineThrow) {
    dafBase::PropertySet ps;
    ps.set("int", 42);

    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    psp->set("int", 3.14159);

    BOOST_CHECK_THROW(ps.combine(psp), dafBase::TypeMismatchException);
}

BOOST_AUTO_TEST_CASE(remove) {
    dafBase::PropertySet ps;
    ps.set("int", 42);
    ps.set("double", 3.14159);
    ps.set("ps1.plus", 1);
    ps.set("ps1.minus", -1);
    ps.set("ps1.zero", 0);
    BOOST_CHECK_EQUAL(ps.nameCount(false), 6U);

    ps.remove("int");
    BOOST_CHECK(!ps.exists("int"));
    BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 3.14159);
    BOOST_CHECK_EQUAL(ps.getAsInt("ps1.plus"), 1);
    BOOST_CHECK_EQUAL(ps.getAsInt("ps1.minus"), -1);
    BOOST_CHECK_EQUAL(ps.getAsInt("ps1.zero"), 0);
    BOOST_CHECK_EQUAL(ps.nameCount(false), 5U);

    ps.remove("ps1.zero");
    BOOST_CHECK(!ps.exists("int"));
    BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 3.14159);
    BOOST_CHECK(!ps.exists("ps1.zero"));
    BOOST_CHECK_EQUAL(ps.getAsInt("ps1.plus"), 1);
    BOOST_CHECK_EQUAL(ps.getAsInt("ps1.minus"), -1);
    BOOST_CHECK_EQUAL(ps.nameCount(false), 4U);

    ps.remove("ps1");
    BOOST_CHECK(!ps.exists("int"));
    BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 3.14159);
    BOOST_CHECK(!ps.exists("ps1"));
    BOOST_CHECK(!ps.exists("ps1.plus"));
    BOOST_CHECK(!ps.exists("ps1.minus"));
    BOOST_CHECK(!ps.exists("ps1.zero"));
    BOOST_CHECK_EQUAL(ps.nameCount(false), 1U);

    ps.remove("double");
    BOOST_CHECK(!ps.exists("int"));
    BOOST_CHECK(!ps.exists("double"));
    BOOST_CHECK(!ps.exists("ps1"));
    BOOST_CHECK(!ps.exists("ps1.plus"));
    BOOST_CHECK(!ps.exists("ps1.minus"));
    BOOST_CHECK(!ps.exists("ps1.zero"));
    BOOST_CHECK_EQUAL(ps.nameCount(false), 0U);
}

BOOST_AUTO_TEST_CASE(deepCopy) {
    dafBase::PropertySet ps;
    ps.set("int", 42);
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    psp->set("bottom", "x");
    ps.set("top", psp);

    dafBase::PropertySet::Ptr psp2 = ps.deepCopy();
    BOOST_CHECK(psp2->exists("int"));
    BOOST_CHECK(psp2->exists("top.bottom"));
    BOOST_CHECK_EQUAL(psp2->getAsInt("int"), 42);
    BOOST_CHECK_EQUAL(psp2->getAsString("top.bottom"), "x");
    // Make sure it was indeed a deep copy.
    BOOST_CHECK(psp2->getAsPropertySetPtr("top") != psp);
    ps.set("int", 2008);
    ps.set("top.bottom", "y");
    BOOST_CHECK_EQUAL(ps.getAsInt("int"), 2008);
    BOOST_CHECK_EQUAL(ps.getAsString("top.bottom"), "y");
    BOOST_CHECK_EQUAL(psp->getAsString("bottom"), "y");
    BOOST_CHECK_EQUAL(psp2->getAsInt("int"), 42);
    BOOST_CHECK_EQUAL(psp2->getAsString("top.bottom"), "x");
}

BOOST_AUTO_TEST_CASE(toString) {
    dafBase::PropertySet ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", 0xfeeddeadbeefLL);
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("char*2", "foo2");
    ps.set("string", std::string("bar"));
    ps.set("ps1.pre", 1);
    ps.set("ps1.post", 2);
    ps.set("ps2.plus", 10.24);
    ps.set("ps2.minus", -10.24);
    ps.set("ps3.sub.subsub", "foo");
    ps.add("v", 10);
    ps.add("v", 9);
    ps.add("v", 8);

    BOOST_CHECK_EQUAL(ps.toString(),
        "bool = 1\n"
        "char = '*'\n"
        "char* = \"foo\"\n"
        "char*2 = \"foo2\"\n"
        "double = 2.71828\n"
        "float = 3.14159\n"
        "int = 2008\n"
        "int64_t = 280297596632815\n"
        "ps1 = {\n"
        "..post = 2\n"
        "..pre = 1\n"
        "}\n"
        "ps2 = {\n"
        "..minus = -10.24\n"
        "..plus = 10.24\n"
        "}\n"
        "ps3 = {\n"
        "..sub = {\n"
        "....subsub = \"foo\"\n"
        "..}\n"
        "}\n"
        "short = 42\n"
        "string = \"bar\"\n"
        "v = [ 10, 9, 8 ]\n"
        );
    BOOST_CHECK_EQUAL(ps.toString(true),
        "bool = 1\n"
        "char = '*'\n"
        "char* = \"foo\"\n"
        "char*2 = \"foo2\"\n"
        "double = 2.71828\n"
        "float = 3.14159\n"
        "int = 2008\n"
        "int64_t = 280297596632815\n"
        "ps1 = { ... }\n"
        "ps2 = { ... }\n"
        "ps3 = { ... }\n"
        "short = 42\n"
        "string = \"bar\"\n"
        "v = [ 10, 9, 8 ]\n"
        );
}

BOOST_AUTO_TEST_CASE(cycle) {
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    psp->set("int", 42);
    psp->set("a.double", 3.14159);
    psp->set("b.c.d", 2008);
    dafBase::PropertySet::Ptr a = psp->getAsPropertySetPtr("a");
    dafBase::PropertySet::Ptr b = psp->getAsPropertySetPtr("b");
    dafBase::PropertySet::Ptr c = psp->getAsPropertySetPtr("b.c");
    BOOST_CHECK_THROW(psp->set("t", psp),
                      pexExcept::InvalidParameterException);
    BOOST_CHECK_THROW(psp->set("a.t", psp),
                      pexExcept::InvalidParameterException);
    BOOST_CHECK_THROW(psp->set("a.t", a),
                      pexExcept::InvalidParameterException);
    psp->set("b.t", psp->getAsPropertySetPtr("a"));
    BOOST_CHECK_EQUAL(a, psp->getAsPropertySetPtr("b.t"));
    BOOST_CHECK_THROW(psp->set("b.c.t", b),
                      pexExcept::InvalidParameterException);
    BOOST_CHECK_THROW(psp->set("b.c.t", c),
                      pexExcept::InvalidParameterException);
    BOOST_CHECK_THROW(a->set("t", psp),
                      pexExcept::InvalidParameterException);
}

BOOST_AUTO_TEST_SUITE_END()

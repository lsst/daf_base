#include "lsst/daf/base/PropertyList.h"

#define BOOST_TEST_MODULE PropertyList_1
#include "boost/test/included/unit_test.hpp"

namespace test = boost::test_tools;
namespace dafBase = lsst::daf::base;

#include <algorithm>

#include "lsst/pex/exceptions/Runtime.h"

#define INT64CONST(x) static_cast<int64_t>(x ## LL)

BOOST_AUTO_TEST_SUITE(PropertyListSuite)

BOOST_AUTO_TEST_CASE(construct) {
    dafBase::PropertyList ps;
    dafBase::PropertyList::Ptr psp(new dafBase::PropertyList);
    BOOST_CHECK_EQUAL(!psp, false);
}

BOOST_AUTO_TEST_CASE(bases) {
    dafBase::PropertyList::Ptr psp(new dafBase::PropertyList);
    boost::shared_ptr<dafBase::Persistable> pp =
        boost::dynamic_pointer_cast<dafBase::Persistable, dafBase::PropertyList>(psp);
    BOOST_CHECK_EQUAL(!pp, false);
    boost::shared_ptr<dafBase::Citizen> cp =
        boost::dynamic_pointer_cast<dafBase::Citizen, dafBase::PropertyList>(psp);
    BOOST_CHECK_EQUAL(!cp, false);
}

BOOST_AUTO_TEST_CASE(getScalar) {
    dafBase::PropertyList ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", INT64CONST(0xfeeddeadbeef));
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
    BOOST_CHECK_EQUAL(ps.get<int64_t>("int64_t"), INT64CONST(0xfeeddeadbeef));
    BOOST_CHECK_EQUAL(ps.get<float>("float"), 3.14159f);
    BOOST_CHECK_EQUAL(ps.get<double>("double"), 2.718281828459045);
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*"), "foo");
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*2"), "foo2");
    BOOST_CHECK_EQUAL(ps.get<std::string>("string"), "bar");
}

BOOST_AUTO_TEST_CASE(resetScalar) {
    dafBase::PropertyList ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", INT64CONST(0xfeeddeadbeef));
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
    BOOST_CHECK_EQUAL(ps.get<int64_t>("int64_t"), INT64CONST(0xfeeddeadbeef));
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
    ps.set("int64_t", INT64CONST(0xcafefacade));
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
    BOOST_CHECK_EQUAL(ps.get<int64_t>("int64_t"), INT64CONST(0xcafefacade));
    BOOST_CHECK_EQUAL(ps.get<float>("float"), 2.71828f);
    BOOST_CHECK_EQUAL(ps.get<double>("double"), 3.1415926535897932);
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*"), "baz");
    BOOST_CHECK_EQUAL(ps.get<std::string>("char*2"), "random2");
    BOOST_CHECK_EQUAL(ps.get<std::string>("string"), "xyzzy");
}

BOOST_AUTO_TEST_CASE(getDefault) {
    dafBase::PropertyList ps;
    ps.set("int", 42);

    BOOST_CHECK_EQUAL(ps.get<int>("int"), 42);
    BOOST_CHECK_EQUAL(ps.get<int>("int", 2008), 42);
    BOOST_CHECK_EQUAL(ps.get<int>("foo", 2008), 2008);
}

BOOST_AUTO_TEST_CASE(exists) {
    dafBase::PropertyList ps;
    ps.set("int", 42);
    BOOST_CHECK_EQUAL(ps.exists("int"), true);
    BOOST_CHECK_EQUAL(ps.exists("foo"), false);
}

BOOST_AUTO_TEST_CASE(getScalarThrow) {
    dafBase::PropertyList ps;
    ps.set("bool", true);
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", INT64CONST(0xfeeddeadbeef));
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
    dafBase::PropertyList ps;
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
    dafBase::PropertyList ps;
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
    dafBase::PropertyList ps;
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
    dafBase::PropertyList ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", INT64CONST(0xfeeddeadbeef));
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
    dafBase::PropertyList ps;
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

BOOST_AUTO_TEST_CASE(variousThrows) {
    dafBase::PropertyList ps;
    ps.set("int", 42);
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
}

BOOST_AUTO_TEST_CASE(names) {
    dafBase::PropertyList ps;
    ps.set("int", 42);
    ps.set("double", 3.14);

    BOOST_CHECK_EQUAL(ps.nameCount(), 2U);

    std::vector<std::string> v = ps.names();
    BOOST_CHECK_EQUAL(v.size(), 2U);
    std::sort(v.begin(), v.end());
    BOOST_CHECK_EQUAL(v[0], "double");
    BOOST_CHECK_EQUAL(v[1], "int");
}

BOOST_AUTO_TEST_CASE(getAs) {
    dafBase::PropertyList ps;
    ps.set("bool", true);
    ps.set("char", 'A');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("char*2", "foo2");
    ps.set("string", std::string("bar"));

    BOOST_CHECK_EQUAL(ps.getAsBool("bool"), true);
    BOOST_CHECK_THROW(ps.getAsBool("char"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsInt("bool"), 1);
    BOOST_CHECK_EQUAL(ps.getAsInt("char"), static_cast<int>('A'));
    BOOST_CHECK_EQUAL(ps.getAsInt("short"), 42);
    BOOST_CHECK_EQUAL(ps.getAsInt("int"), 2008);
    BOOST_CHECK_THROW(ps.getAsInt("int64_t"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsInt64("bool"), INT64CONST(1));
    BOOST_CHECK_EQUAL(ps.getAsInt64("char"), static_cast<int64_t>('A'));
    BOOST_CHECK_EQUAL(ps.getAsInt64("short"), INT64CONST(42));
    BOOST_CHECK_EQUAL(ps.getAsInt64("int"), INT64CONST(2008));
    BOOST_CHECK_EQUAL(ps.getAsInt64("int64_t"), INT64CONST(0xfeeddeadbeef));
    BOOST_CHECK_THROW(ps.getAsInt64("float"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsDouble("bool"), 1.0);
    BOOST_CHECK_EQUAL(ps.getAsDouble("char"), static_cast<double>('A'));
    BOOST_CHECK_EQUAL(ps.getAsDouble("short"), 42.0);
    BOOST_CHECK_EQUAL(ps.getAsDouble("int"), 2008.0);
    BOOST_CHECK_EQUAL(ps.getAsDouble("int64_t"),
                      static_cast<double>(INT64CONST(0xfeeddeadbeef)));
    BOOST_CHECK_EQUAL(ps.getAsDouble("float"), 3.14159f);
    BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 2.718281828459045);
    BOOST_CHECK_THROW(ps.getAsDouble("char*"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(ps.getAsString("char"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(ps.getAsString("char*"), "foo");
    BOOST_CHECK_EQUAL(ps.getAsString("char*2"), "foo2");
    BOOST_CHECK_EQUAL(ps.getAsString("string"), "bar");
    BOOST_CHECK_THROW(ps.getAsString("int"), dafBase::TypeMismatchException);
}

BOOST_AUTO_TEST_CASE(remove) {
    dafBase::PropertyList ps;
    ps.set("int", 42);
    ps.set("double", 3.14159);
    BOOST_CHECK_EQUAL(ps.nameCount(), 2U);

    ps.remove("int");
    BOOST_CHECK(!ps.exists("int"));
    BOOST_CHECK_EQUAL(ps.getAsDouble("double"), 3.14159);
    BOOST_CHECK_EQUAL(ps.nameCount(), 1U);

    ps.remove("double");
    BOOST_CHECK(!ps.exists("int"));
    BOOST_CHECK(!ps.exists("double"));
    BOOST_CHECK(!ps.exists("ps1"));
    BOOST_CHECK_EQUAL(ps.nameCount(), 0U);
}

BOOST_AUTO_TEST_CASE(deepCopy) {
    dafBase::PropertyList ps;
    ps.set("int", 42);

    dafBase::PropertyList::Ptr psp2 = ps.deepCopy();
    BOOST_CHECK(psp2->exists("int"));
    BOOST_CHECK_EQUAL(psp2->getAsInt("int"), 42);
    // Make sure it was indeed a deep copy.
    ps.set("int", 2008);
    BOOST_CHECK_EQUAL(ps.getAsInt("int"), 2008);
    BOOST_CHECK_EQUAL(psp2->getAsInt("int"), 42);
}

BOOST_AUTO_TEST_CASE(toString) {
    dafBase::PropertyList ps;
    ps.set("bool", true);
    ps.set("char", '*');
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("char*2", "foo2");
    ps.set("string", std::string("bar"));
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
        "short = 42\n"
        "string = \"bar\"\n"
        "v = [ 10, 9, 8 ]\n"
        );
}

BOOST_AUTO_TEST_CASE(iterator) {
    dafBase::PropertyList ps;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    ps.set("ints", v);
    ps.set("ints2", -42);
    ps.set("int", 365);
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

    dafBase::PropertyList::const_iterator i = ps.begin();
    BOOST_CHECK(i != ps.end());
    BOOST_CHECK_EQUAL((*i).first, "ints");
    BOOST_CHECK((*i).second.type() == typeid(int));
    BOOST_CHECK_EQUAL(boost::any_cast<int>((*i).second), 42);
    ++i;
    BOOST_CHECK(i != ps.end());
    BOOST_CHECK_EQUAL((*i).first, "ints");
    BOOST_CHECK((*i).second.type() == typeid(int));
    BOOST_CHECK_EQUAL(boost::any_cast<int>((*i).second), 2008);
    ++i;
    BOOST_CHECK(i != ps.end());
    BOOST_CHECK_EQUAL((*i).first, "ints");
    BOOST_CHECK((*i).second.type() == typeid(int));
    BOOST_CHECK_EQUAL(boost::any_cast<int>((*i).second), 1);
    ++i;
    BOOST_CHECK(i != ps.end());
    BOOST_CHECK_EQUAL((*i).first, "ints2");
    BOOST_CHECK((*i).second.type() == typeid(int));
    BOOST_CHECK_EQUAL(boost::any_cast<int>((*i).second), -42);
    ++i;
    BOOST_CHECK(i != ps.end());
    BOOST_CHECK_EQUAL((*i).first, "int");
    BOOST_CHECK((*i).second.type() == typeid(int));
    BOOST_CHECK_EQUAL(boost::any_cast<int>((*i).second), 365);
    ++i;
    BOOST_CHECK(i != ps.end());
    BOOST_CHECK_EQUAL((*i).first, "ints2");
    BOOST_CHECK((*i).second.type() == typeid(int));
    BOOST_CHECK_EQUAL(boost::any_cast<int>((*i).second), -2008);
    ++i;
    BOOST_CHECK(i == ps.end());
}

BOOST_AUTO_TEST_SUITE_END()

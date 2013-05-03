/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
#include "lsst/daf/base/PropertyList.h"

#define BOOST_TEST_MODULE PropertyList
#define BOOST_TEST_DYN_LINK
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#include "boost/test/unit_test.hpp"
#pragma clang diagnostic pop

#include <algorithm>

#include "lsst/pex/exceptions/Runtime.h"

#define INT64CONST(x) static_cast<int64_t>(x ## LL)

namespace test = boost::test_tools;
namespace dafBase = lsst::daf::base;

BOOST_AUTO_TEST_SUITE(PropertyListSuite) /* parasoft-suppress LsstDm-3-2a LsstDm-3-6a LsstDm-4-6 "Boost test harness macros" */

BOOST_AUTO_TEST_CASE(construct) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    dafBase::PropertyList::Ptr plp(new dafBase::PropertyList);
    BOOST_CHECK_EQUAL(!plp, false);
    dafBase::PropertySet::Ptr psp(new dafBase::PropertyList);
    BOOST_CHECK_EQUAL(!psp, false);
}

BOOST_AUTO_TEST_CASE(bases) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList::Ptr plp(new dafBase::PropertyList);
    dafBase::PropertySet::Ptr psp = plp;
    BOOST_CHECK_EQUAL(!plp, false);
    boost::shared_ptr<dafBase::Persistable> pp = plp;
    BOOST_CHECK_EQUAL(!pp, false);
    boost::shared_ptr<dafBase::Citizen> cp = plp;
    BOOST_CHECK_EQUAL(!cp, false);
}

BOOST_AUTO_TEST_CASE(getScalar) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("bool", true);
    pl.set("char", '*');
    short s = 42;
    pl.set("short", s);
    pl.set("int", 2008);
    pl.set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    pl.set("float", f);
    double d = 2.718281828459045;
    pl.set("double", d);
    pl.set<std::string>("char*", "foo");
    pl.set("char*2", "foo2");
    pl.set("string", std::string("bar"));

    BOOST_CHECK_EQUAL(pl.get<bool>("bool"), true);
    BOOST_CHECK_EQUAL(pl.get<char>("char"), '*');
    BOOST_CHECK_EQUAL(pl.get<short>("short"), 42);
    BOOST_CHECK_EQUAL(pl.get<int>("int"), 2008);
    BOOST_CHECK_EQUAL(pl.get<int64_t>("int64_t"), INT64CONST(0xfeeddeadbeef));
    BOOST_CHECK_EQUAL(pl.get<float>("float"), 3.14159f);
    BOOST_CHECK_EQUAL(pl.get<double>("double"), 2.718281828459045);
    BOOST_CHECK_EQUAL(pl.get<std::string>("char*"), "foo");
    BOOST_CHECK_EQUAL(pl.get<std::string>("char*2"), "foo2");
    BOOST_CHECK_EQUAL(pl.get<std::string>("string"), "bar");
}

BOOST_AUTO_TEST_CASE(getScalarPSP) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertySet::Ptr psp(new dafBase::PropertyList);
    psp->set("bool", true);
    psp->set("char", '*');
    short s = 42;
    psp->set("short", s);
    psp->set("int", 2008);
    psp->set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    psp->set("float", f);
    double d = 2.718281828459045;
    psp->set("double", d);
    psp->set<std::string>("char*", "foo");
    psp->set("char*2", "foo2");
    psp->set("string", std::string("bar"));

    BOOST_CHECK_EQUAL(psp->get<bool>("bool"), true);
    BOOST_CHECK_EQUAL(psp->get<char>("char"), '*');
    BOOST_CHECK_EQUAL(psp->get<short>("short"), 42);
    BOOST_CHECK_EQUAL(psp->get<int>("int"), 2008);
    BOOST_CHECK_EQUAL(psp->get<int64_t>("int64_t"), INT64CONST(0xfeeddeadbeef));
    BOOST_CHECK_EQUAL(psp->get<float>("float"), 3.14159f);
    BOOST_CHECK_EQUAL(psp->get<double>("double"), 2.718281828459045);
    BOOST_CHECK_EQUAL(psp->get<std::string>("char*"), "foo");
    BOOST_CHECK_EQUAL(psp->get<std::string>("char*2"), "foo2");
    BOOST_CHECK_EQUAL(psp->get<std::string>("string"), "bar");
}

BOOST_AUTO_TEST_CASE(resetScalar) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("bool", true);
    pl.set("char", '*');
    short s = 42;
    pl.set("short", s);
    pl.set("int", 2008);
    pl.set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    pl.set("float", f);
    double d = 2.718281828459045;
    pl.set("double", d);
    pl.set<std::string>("char*", "foo");
    pl.set("char*2", "foo2");
    pl.set("string", std::string("bar"));

    BOOST_CHECK_EQUAL(pl.get<bool>("bool"), true);
    BOOST_CHECK_EQUAL(pl.get<char>("char"), '*');
    BOOST_CHECK_EQUAL(pl.get<short>("short"), 42);
    BOOST_CHECK_EQUAL(pl.get<int>("int"), 2008);
    BOOST_CHECK_EQUAL(pl.get<int64_t>("int64_t"), INT64CONST(0xfeeddeadbeef));
    BOOST_CHECK_EQUAL(pl.get<float>("float"), 3.14159f);
    BOOST_CHECK_EQUAL(pl.get<double>("double"), 2.718281828459045);
    BOOST_CHECK_EQUAL(pl.get<std::string>("char*"), "foo");
    BOOST_CHECK_EQUAL(pl.get<std::string>("char*2"), "foo2");
    BOOST_CHECK_EQUAL(pl.get<std::string>("string"), "bar");

    pl.set("bool", false);
    pl.set("char", '%');
    s = 2008;
    pl.set("short", s);
    pl.set("int", 42);
    pl.set("int64_t", INT64CONST(0xcafefacade));
    f = 2.71828;
    pl.set("float", f);
    d = 3.1415926535897932;
    pl.set("double", d);
    pl.set<std::string>("char*", "baz");
    pl.set("char*2", "random2");
    pl.set("string", std::string("xyzzy"));

    BOOST_CHECK_EQUAL(pl.get<bool>("bool"), false);
    BOOST_CHECK_EQUAL(pl.get<char>("char"), '%');
    BOOST_CHECK_EQUAL(pl.get<short>("short"), 2008);
    BOOST_CHECK_EQUAL(pl.get<int>("int"), 42);
    BOOST_CHECK_EQUAL(pl.get<int64_t>("int64_t"), INT64CONST(0xcafefacade));
    BOOST_CHECK_EQUAL(pl.get<float>("float"), 2.71828f);
    BOOST_CHECK_EQUAL(pl.get<double>("double"), 3.1415926535897932);
    BOOST_CHECK_EQUAL(pl.get<std::string>("char*"), "baz");
    BOOST_CHECK_EQUAL(pl.get<std::string>("char*2"), "random2");
    BOOST_CHECK_EQUAL(pl.get<std::string>("string"), "xyzzy");
}

BOOST_AUTO_TEST_CASE(getDefault) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("int", 42);

    BOOST_CHECK_EQUAL(pl.get<int>("int"), 42);
    BOOST_CHECK_EQUAL(pl.get<int>("int", 2008), 42);
    BOOST_CHECK_EQUAL(pl.get<int>("foo", 2008), 2008);
}

BOOST_AUTO_TEST_CASE(comments) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("int", 42);
    BOOST_CHECK_EQUAL(pl.get<int>("int"), 42);
    BOOST_CHECK_EQUAL(pl.getComment("int"), "");
    pl.set("int", 31, "test");
    BOOST_CHECK_EQUAL(pl.get<int>("int"), 31);
    BOOST_CHECK_EQUAL(pl.getComment("int"), "test");
    pl.set("int", 20, std::string("test2"));
    BOOST_CHECK_EQUAL(pl.get<int>("int"), 20);
    BOOST_CHECK_EQUAL(pl.getComment("int"), "test2");
    pl.set("int", 9);
    BOOST_CHECK_EQUAL(pl.get<int>("int"), 9);
    BOOST_CHECK_EQUAL(pl.getComment("int"), "test2");
    pl.set("int", -2, "");
    BOOST_CHECK_EQUAL(pl.get<int>("int"), -2);
    BOOST_CHECK_EQUAL(pl.getComment("int"), "");
}

BOOST_AUTO_TEST_CASE(deepCopy) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList::Ptr plp(new dafBase::PropertyList);
    plp->set("int", 31, "test");
    BOOST_CHECK_EQUAL(plp->get<int>("int"), 31);
    dafBase::PropertySet::Ptr psp = plp;
    dafBase::PropertySet::Ptr psp2 = psp->deepCopy();
    BOOST_CHECK_EQUAL(psp2->get<int>("int"), 31);
    dafBase::PropertyList::Ptr plp2 =
        boost::dynamic_pointer_cast<dafBase::PropertyList,
        dafBase::PropertySet>(psp2);
    BOOST_CHECK_EQUAL(!plp2, false);
    BOOST_CHECK_EQUAL(plp2->get<int>("int"), 31);
    BOOST_CHECK_EQUAL(plp2->getComment("int"), "test");
}


BOOST_AUTO_TEST_CASE(exists) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("int", 42);
    BOOST_CHECK_EQUAL(pl.exists("int"), true);
    BOOST_CHECK_EQUAL(pl.exists("foo"), false);
}

BOOST_AUTO_TEST_CASE(getScalarThrow) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("bool", true);
    short s = 42;
    pl.set("short", s);
    pl.set("int", 2008);
    pl.set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    pl.set("float", f);
    double d = 2.718281828459045;
    pl.set("double", d);
    pl.set<std::string>("char*", "foo");
    pl.set("char*2", "foo2");
    pl.set("string", std::string("bar"));

    BOOST_CHECK_THROW(pl.get<bool>("short"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<bool>("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<short>("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<int>("short"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<int>("bool"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<unsigned int>("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<double>("float"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<float>("double"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.get<std::string>("int"), dafBase::TypeMismatchException);
}

BOOST_AUTO_TEST_CASE(getVector) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    pl.set("ints", v);

    std::vector<int> w = pl.getArray<int>("ints");
    BOOST_CHECK_EQUAL(w.size(), 3U);
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_EQUAL(v[i], w[i]);
    }
}

BOOST_AUTO_TEST_CASE(addScalar) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    pl.set("ints", v);
    pl.add("ints", -999);
    pl.add("other", "foo");

    std::vector<int> w = pl.getArray<int>("ints");
    BOOST_CHECK_EQUAL(w.size(), 4U);
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_EQUAL(v[i], w[i]);
    }
    BOOST_CHECK_EQUAL(w[3], -999);
    BOOST_CHECK_EQUAL(pl.get<std::string>("other"), "foo");
}

BOOST_AUTO_TEST_CASE(addVector) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    pl.set("ints", v);

    std::vector<int> vv;
    vv.push_back(-42);
    vv.push_back(-2008);
    vv.push_back(-1);
    pl.add("ints", vv);

    std::vector<int> w = pl.getArray<int>("ints");
    BOOST_CHECK_EQUAL(w.size(), 6U);
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK_EQUAL(v[i], w[i]);
        BOOST_CHECK_EQUAL(vv[i], w[i + 3]);
    }
}

BOOST_AUTO_TEST_CASE(typeOf) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("bool", true);
    pl.set("char", '*');
    short s = 42;
    pl.set("short", s);
    pl.set("int", 2008);
    pl.set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    pl.set("float", f);
    double d = 2.718281828459045;
    pl.set("double", d);
    pl.set("char*", "foo");
    pl.set("string", std::string("bar"));

    BOOST_CHECK(pl.typeOf("bool") == typeid(bool));
    BOOST_CHECK(pl.typeOf("char") == typeid(char));
    BOOST_CHECK(pl.typeOf("short") == typeid(short));
    BOOST_CHECK(pl.typeOf("int") == typeid(int));
    BOOST_CHECK(pl.typeOf("int64_t") == typeid(int64_t));
    BOOST_CHECK(pl.typeOf("float") == typeid(float));
    BOOST_CHECK(pl.typeOf("double") == typeid(double));
    BOOST_CHECK(pl.typeOf("char*") == typeid(std::string));
    BOOST_CHECK(pl.typeOf("string") == typeid(std::string));
}

BOOST_AUTO_TEST_CASE(arrayProperties) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    std::vector<int> v;
    v.push_back(42);
    v.push_back(2008);
    v.push_back(1);
    pl.set("ints", v);
    pl.set("int", 365);
    pl.set("ints2", -42);
    pl.add("ints2", -2008);

    BOOST_CHECK_EQUAL(pl.isArray("ints"), true);
    BOOST_CHECK_EQUAL(pl.isArray("int"), false);
    BOOST_CHECK_EQUAL(pl.isArray("ints2"), true);
    BOOST_CHECK_EQUAL(pl.valueCount("ints"), 3U);
    BOOST_CHECK_EQUAL(pl.valueCount("int"), 1U);
    BOOST_CHECK_EQUAL(pl.valueCount("ints2"), 2U);
    BOOST_CHECK(pl.typeOf("ints") == typeid(int));
    BOOST_CHECK(pl.typeOf("int") == typeid(int));
    BOOST_CHECK(pl.typeOf("ints2") == typeid(int));
}

BOOST_AUTO_TEST_CASE(getAs) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("bool", true);
    pl.set("char", 'A');
    short s = 42;
    pl.set("short", s);
    pl.set("int", 2008);
    pl.set("int64_t", INT64CONST(0xfeeddeadbeef));
    float f = 3.14159;
    pl.set("float", f);
    double d = 2.718281828459045;
    pl.set("double", d);
    pl.set<std::string>("char*", "foo");
    pl.set("char*2", "foo2");
    pl.set("string", std::string("bar"));
    dafBase::PropertySet::Ptr plp(new dafBase::PropertySet);
    plp->set("bottom", "x");
    pl.set("top", plp);

    BOOST_CHECK_EQUAL(pl.getAsBool("bool"), true);
    BOOST_CHECK_THROW(pl.getAsBool("char"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(pl.getAsInt("bool"), 1);
    BOOST_CHECK_EQUAL(pl.getAsInt("char"), static_cast<int>('A'));
    BOOST_CHECK_EQUAL(pl.getAsInt("short"), 42);
    BOOST_CHECK_EQUAL(pl.getAsInt("int"), 2008);
    BOOST_CHECK_THROW(pl.getAsInt("int64_t"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(pl.getAsInt64("bool"), INT64CONST(1));
    BOOST_CHECK_EQUAL(pl.getAsInt64("char"), static_cast<int64_t>('A'));
    BOOST_CHECK_EQUAL(pl.getAsInt64("short"), INT64CONST(42));
    BOOST_CHECK_EQUAL(pl.getAsInt64("int"), INT64CONST(2008));
    BOOST_CHECK_EQUAL(pl.getAsInt64("int64_t"), INT64CONST(0xfeeddeadbeef));
    BOOST_CHECK_THROW(pl.getAsInt64("float"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(pl.getAsDouble("bool"), 1.0);
    BOOST_CHECK_EQUAL(pl.getAsDouble("char"), static_cast<double>('A'));
    BOOST_CHECK_EQUAL(pl.getAsDouble("short"), 42.0);
    BOOST_CHECK_EQUAL(pl.getAsDouble("int"), 2008.0);
    BOOST_CHECK_EQUAL(pl.getAsDouble("int64_t"),
                      static_cast<double>(INT64CONST(0xfeeddeadbeef)));
    BOOST_CHECK_EQUAL(pl.getAsDouble("float"), 3.14159f);
    BOOST_CHECK_EQUAL(pl.getAsDouble("double"), 2.718281828459045);
    BOOST_CHECK_THROW(pl.getAsDouble("char*"), dafBase::TypeMismatchException);
    BOOST_CHECK_THROW(pl.getAsString("char"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(pl.getAsString("char*"), "foo");
    BOOST_CHECK_EQUAL(pl.getAsString("char*2"), "foo2");
    BOOST_CHECK_EQUAL(pl.getAsString("string"), "bar");
    BOOST_CHECK_THROW(pl.getAsString("int"), dafBase::TypeMismatchException);
    BOOST_CHECK_EQUAL(pl.getAsString("top.bottom"), "x");
    BOOST_CHECK_THROW(pl.getAsPropertySetPtr("top"), pexExcept::NotFoundException);
}

BOOST_AUTO_TEST_CASE(combineThrow) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList pl;
    pl.set("int", 42);

    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    psp->set("int", 3.14159);

    BOOST_CHECK_THROW(pl.combine(psp), dafBase::TypeMismatchException);
}

BOOST_AUTO_TEST_CASE(combineAsPS) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    dafBase::PropertyList::Ptr plp1(new dafBase::PropertyList);
    dafBase::PropertyList::Ptr plp2(new dafBase::PropertyList);
    plp1->set("int", 42, "comment");
    plp2->set("float", 3.14159, "stuff");
    dafBase::PropertySet::Ptr psp =
        boost::static_pointer_cast<dafBase::PropertySet,
        dafBase::PropertyList>(plp1);
    psp.get()->set("foo", 36);
    psp.get()->combine(plp2);
    dafBase::PropertyList::Ptr newPlp =
        boost::dynamic_pointer_cast<dafBase::PropertyList,
        dafBase::PropertySet>(psp);
    BOOST_CHECK_EQUAL(!newPlp, false);
    BOOST_CHECK_EQUAL(newPlp->get<int>("int"), 42);
    BOOST_CHECK_EQUAL(newPlp->get<int>("foo"), 36);
    BOOST_CHECK_EQUAL(newPlp->get<double>("float"), 3.14159);
    BOOST_CHECK_EQUAL(newPlp->getComment("int"), "comment");
    BOOST_CHECK_EQUAL(newPlp->getComment("float"), "stuff");
}


BOOST_AUTO_TEST_SUITE_END()

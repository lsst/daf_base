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

#include "lsst/daf/base/DateTime.h"

#define BOOST_TEST_MODULE DateTime_1
#define BOOST_TEST_DYN_LINK
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#include "boost/test/unit_test.hpp"
#pragma clang diagnostic pop

#include "lsst/cpputils/tests.h"
#include "lsst/pex/exceptions.h"

namespace test = boost::test_tools;
using lsst::daf::base::DateTime;

BOOST_AUTO_TEST_SUITE(DateTimeSuite)

BOOST_AUTO_TEST_CASE(Gmtime) {
    DateTime dt("20090402T072639.314159265Z", DateTime::UTC);
    struct tm t(dt.gmtime(DateTime::UTC));
    BOOST_CHECK_EQUAL(t.tm_sec, 39);
    BOOST_CHECK_EQUAL(t.tm_min, 26);
    BOOST_CHECK_EQUAL(t.tm_hour, 7);
    BOOST_CHECK_EQUAL(t.tm_mday, 2);
    BOOST_CHECK_EQUAL(t.tm_mon, 4 - 1);
    BOOST_CHECK_EQUAL(t.tm_year, 2009 - 1900);
    BOOST_CHECK_EQUAL(t.tm_wday, 4);
    BOOST_CHECK_EQUAL(t.tm_yday, 31 + 28 + 31 + 2 - 1);
    BOOST_CHECK_EQUAL(t.tm_isdst, 0);
}

BOOST_AUTO_TEST_CASE(Timespec) {
    DateTime dt("20090402T072639.314159265Z", DateTime::UTC);
    struct timespec ts(dt.timespec(DateTime::UTC));
    BOOST_CHECK_EQUAL(ts.tv_sec, 1238657199);
    BOOST_CHECK_EQUAL(ts.tv_nsec, 314159265);
}

BOOST_AUTO_TEST_CASE(Timeval) {
    DateTime dt("20090402T072639.314159265Z", DateTime::UTC);
    struct timeval tv(dt.timeval(DateTime::UTC));
    BOOST_CHECK_EQUAL(tv.tv_sec, 1238657199);
    BOOST_CHECK_EQUAL(tv.tv_usec, 314159);
}

BOOST_AUTO_TEST_CASE(Throw) {
//
#if 0
    // Date too far in the future
    BOOST_CHECK_THROW(DateTime(40587.0 + 106752.0),
                      lsst::pex::exceptions::DomainError);
    // Date too far in the past
    BOOST_CHECK_THROW(DateTime(40587.0 - 106752.0),
                      lsst::pex::exceptions::DomainError);
#endif
    // Date before UTC->TAI conversion is valid
    BOOST_CHECK_THROW(DateTime(-500000000 * 1000000000LL, DateTime::UTC), lsst::pex::exceptions::DomainError);
    // Date before UTC->TAI conversion is valid and too far in the past for
    // 32-bit Unix mktime()
    BOOST_CHECK_THROW(DateTime("1901-01-01T12:34:56Z", DateTime::UTC), lsst::pex::exceptions::DomainError);
    if (sizeof(time_t) == 4) {
        // Date too far in the past for Unix mktime()
        BOOST_CHECK_THROW(DateTime(1901, 1, 1, 12, 34, 56), lsst::pex::exceptions::DomainError);
        // Date too far in the future for Unix mktime()
        BOOST_CHECK_THROW(DateTime(2039, 1, 1, 12, 34, 56), lsst::pex::exceptions::DomainError);
        BOOST_CHECK_THROW(DateTime("2039-01-01T12:34:56Z", DateTime::UTC),
                          lsst::pex::exceptions::DomainError);
    }
}

BOOST_AUTO_TEST_CASE(Hash) {
    lsst::cpputils::assertValidHash<DateTime>();

    DateTime date1("20090402T072639.314159265Z", DateTime::UTC);
    lsst::cpputils::assertHashesEqual(date1, DateTime(date1.nsecs()));
}

BOOST_AUTO_TEST_SUITE_END()

#include "lsst/daf/base/DateTime.h"

#define BOOST_TEST_MODULE DateTime_1
#include "boost/test/included/unit_test.hpp"

#include "lsst/pex/exceptions.h"

namespace test = boost::test_tools;
using lsst::daf::base::DateTime;

BOOST_AUTO_TEST_SUITE(DateTimeSuite) /* parasoft-suppress LsstDm-3-2a LsstDm-3-6a LsstDm-4-6 "Boost test harness macros" */

BOOST_AUTO_TEST_CASE(Gmtime) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-2a LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    DateTime dt("20090402T072639.314159265Z");
    struct tm t(dt.gmtime());
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

BOOST_AUTO_TEST_CASE(Timespec) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-2a LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    DateTime dt("20090402T072639.314159265Z");
    struct timespec ts(dt.timespec());
    BOOST_CHECK_EQUAL(ts.tv_sec, 1238657199);
    BOOST_CHECK_EQUAL(ts.tv_nsec, 314159265);
}

BOOST_AUTO_TEST_CASE(Timeval) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-2a LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    DateTime dt("20090402T072639.314159265Z");
    struct timeval tv(dt.timeval());
    BOOST_CHECK_EQUAL(tv.tv_sec, 1238657199);
    BOOST_CHECK_EQUAL(tv.tv_usec, 314159);
}

BOOST_AUTO_TEST_CASE(Throw) { /* parasoft-suppress LsstDm-3-1 LsstDm-3-2a LsstDm-3-4a LsstDm-5-25 LsstDm-4-6 "Boost test harness macros" */
    // Date too far in the future
    BOOST_CHECK_THROW(DateTime(40587.0 + 106752.0),
                      lsst::pex::exceptions::DomainErrorException);
    // Date too far in the past
    BOOST_CHECK_THROW(DateTime(40587.0 - 106752.0),
                      lsst::pex::exceptions::DomainErrorException);
    // Date before UTC->TAI conversion is valid
    BOOST_CHECK_THROW(DateTime(-500000000 * 1000000000LL, DateTime::UTC),
                      lsst::pex::exceptions::DomainErrorException);
    // Date before UTC->TAI conversion is valid and too far in the past for
    // 32-bit Unix mktime()
    BOOST_CHECK_THROW(DateTime("1901-01-01T12:34:56Z"),
                      lsst::pex::exceptions::DomainErrorException);
    if (sizeof(time_t) == 4) {
        // Date too far in the past for Unix mktime()
        BOOST_CHECK_THROW(DateTime(1901, 1, 1, 12, 34, 56),
                          lsst::pex::exceptions::DomainErrorException);
        // Date too far in the future for Unix mktime()
        BOOST_CHECK_THROW(DateTime(2039, 1, 1, 12, 34, 56),
                          lsst::pex::exceptions::DomainErrorException);
        BOOST_CHECK_THROW(DateTime("2039-01-01T12:34:56Z"),
                          lsst::pex::exceptions::DomainErrorException);
    }

}

BOOST_AUTO_TEST_SUITE_END()

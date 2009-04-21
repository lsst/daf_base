#include "lsst/daf/base/DateTime.h"

#define BOOST_TEST_MODULE DateTime_1
#include "boost/test/included/unit_test.hpp"

namespace test = boost::test_tools;
using lsst::daf::base::DateTime;

BOOST_AUTO_TEST_SUITE(DateTimeSuite)

BOOST_AUTO_TEST_CASE(Gmtime) {
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

BOOST_AUTO_TEST_CASE(Timespec) {
    DateTime dt("20090402T072639.314159265Z");
    struct timespec ts(dt.timespec());
    BOOST_CHECK_EQUAL(ts.tv_sec, 1238657199);
    BOOST_CHECK_EQUAL(ts.tv_nsec, 314159265);
}

BOOST_AUTO_TEST_CASE(Timeval) {
    DateTime dt("20090402T072639.314159265Z");
    struct timeval tv(dt.timeval());
    BOOST_CHECK_EQUAL(tv.tv_sec, 1238657199);
    BOOST_CHECK_EQUAL(tv.tv_usec, 314159);
}

BOOST_AUTO_TEST_SUITE_END()

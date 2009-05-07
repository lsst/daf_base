// -*- lsst-c++ -*-


/** \file
 * \brief Implementation of DateTime class.
 *
 * \author $Author: ktlim $
 * \version $Revision: 2151 $
 * \date $Date$
 *
 * Contact: Kian-Tat Lim (ktl@slac.stanford.edu)
 *
 * \ingroup daf_base
 */

#ifndef __GNUC__
#  define __attribute__(x) /*NOTHING*/
#endif
static char const* SVNid __attribute__((unused)) = "$Id$";

#include "lsst/daf/base/DateTime.h"

#include "boost/format.hpp"
#include "boost/regex.hpp"
#include <vector>

#include "lsst/pex/exceptions.h"

namespace dafBase = lsst::daf::base;

/// Epoch = 1970 JAN  1 00:00:00 = JD 2440587.5 = MJD 40587.0
static double const MJD_TO_JD = 2400000.5;
static double const EPOCH_IN_MJD = 40587.0;

/// Nanoseconds per day.
static double const NSEC_PER_DAY = 86.4e12;

/// Nanoseconds per day/second as a long long.
static long long const LL_NSEC_PER_SEC = 1000000000LL;
static long long const LL_NSEC_PER_DAY = 86400 * LL_NSEC_PER_SEC;

/// Maximum number of days expressible as signed 64-bit nanoseconds.
/// 2^64 / 2 / 1e9 / 86400
static double const MAX_DAYS = 106751.99;

/* Leap second table as string.
 *
 * Source: http://maia.usno.navy.mil/ser7/tai-utc.dat
 */
static std::string leapString =
"\
1961 JAN  1 =JD 2437300.5  TAI-UTC=   1.4228180 S + (MJD - 37300.) X 0.001296 S\n\
1961 AUG  1 =JD 2437512.5  TAI-UTC=   1.3728180 S + (MJD - 37300.) X 0.001296 S\n\
1962 JAN  1 =JD 2437665.5  TAI-UTC=   1.8458580 S + (MJD - 37665.) X 0.0011232S\n\
1963 NOV  1 =JD 2438334.5  TAI-UTC=   1.9458580 S + (MJD - 37665.) X 0.0011232S\n\
1964 JAN  1 =JD 2438395.5  TAI-UTC=   3.2401300 S + (MJD - 38761.) X 0.001296 S\n\
1964 APR  1 =JD 2438486.5  TAI-UTC=   3.3401300 S + (MJD - 38761.) X 0.001296 S\n\
1964 SEP  1 =JD 2438639.5  TAI-UTC=   3.4401300 S + (MJD - 38761.) X 0.001296 S\n\
1965 JAN  1 =JD 2438761.5  TAI-UTC=   3.5401300 S + (MJD - 38761.) X 0.001296 S\n\
1965 MAR  1 =JD 2438820.5  TAI-UTC=   3.6401300 S + (MJD - 38761.) X 0.001296 S\n\
1965 JUL  1 =JD 2438942.5  TAI-UTC=   3.7401300 S + (MJD - 38761.) X 0.001296 S\n\
1965 SEP  1 =JD 2439004.5  TAI-UTC=   3.8401300 S + (MJD - 38761.) X 0.001296 S\n\
1966 JAN  1 =JD 2439126.5  TAI-UTC=   4.3131700 S + (MJD - 39126.) X 0.002592 S\n\
1968 FEB  1 =JD 2439887.5  TAI-UTC=   4.2131700 S + (MJD - 39126.) X 0.002592 S\n\
1972 JAN  1 =JD 2441317.5  TAI-UTC=  10.0       S + (MJD - 41317.) X 0.0      S\n\
1972 JUL  1 =JD 2441499.5  TAI-UTC=  11.0       S + (MJD - 41317.) X 0.0      S\n\
1973 JAN  1 =JD 2441683.5  TAI-UTC=  12.0       S + (MJD - 41317.) X 0.0      S\n\
1974 JAN  1 =JD 2442048.5  TAI-UTC=  13.0       S + (MJD - 41317.) X 0.0      S\n\
1975 JAN  1 =JD 2442413.5  TAI-UTC=  14.0       S + (MJD - 41317.) X 0.0      S\n\
1976 JAN  1 =JD 2442778.5  TAI-UTC=  15.0       S + (MJD - 41317.) X 0.0      S\n\
1977 JAN  1 =JD 2443144.5  TAI-UTC=  16.0       S + (MJD - 41317.) X 0.0      S\n\
1978 JAN  1 =JD 2443509.5  TAI-UTC=  17.0       S + (MJD - 41317.) X 0.0      S\n\
1979 JAN  1 =JD 2443874.5  TAI-UTC=  18.0       S + (MJD - 41317.) X 0.0      S\n\
1980 JAN  1 =JD 2444239.5  TAI-UTC=  19.0       S + (MJD - 41317.) X 0.0      S\n\
1981 JUL  1 =JD 2444786.5  TAI-UTC=  20.0       S + (MJD - 41317.) X 0.0      S\n\
1982 JUL  1 =JD 2445151.5  TAI-UTC=  21.0       S + (MJD - 41317.) X 0.0      S\n\
1983 JUL  1 =JD 2445516.5  TAI-UTC=  22.0       S + (MJD - 41317.) X 0.0      S\n\
1985 JUL  1 =JD 2446247.5  TAI-UTC=  23.0       S + (MJD - 41317.) X 0.0      S\n\
1988 JAN  1 =JD 2447161.5  TAI-UTC=  24.0       S + (MJD - 41317.) X 0.0      S\n\
1990 JAN  1 =JD 2447892.5  TAI-UTC=  25.0       S + (MJD - 41317.) X 0.0      S\n\
1991 JAN  1 =JD 2448257.5  TAI-UTC=  26.0       S + (MJD - 41317.) X 0.0      S\n\
1992 JUL  1 =JD 2448804.5  TAI-UTC=  27.0       S + (MJD - 41317.) X 0.0      S\n\
1993 JUL  1 =JD 2449169.5  TAI-UTC=  28.0       S + (MJD - 41317.) X 0.0      S\n\
1994 JUL  1 =JD 2449534.5  TAI-UTC=  29.0       S + (MJD - 41317.) X 0.0      S\n\
1996 JAN  1 =JD 2450083.5  TAI-UTC=  30.0       S + (MJD - 41317.) X 0.0      S\n\
1997 JUL  1 =JD 2450630.5  TAI-UTC=  31.0       S + (MJD - 41317.) X 0.0      S\n\
1999 JAN  1 =JD 2451179.5  TAI-UTC=  32.0       S + (MJD - 41317.) X 0.0      S\n\
2006 JAN  1 =JD 2453736.5  TAI-UTC=  33.0       S + (MJD - 41317.) X 0.0      S\n\
2009 JAN  1 =JD 2454832.5  TAI-UTC=  34.0       S + (MJD - 41317.) X 0.0      S\n\
";

/// Leap second descriptor.
struct Leap {
    long long whenUtc; ///< UTC nanosecs of change
    long long whenTai; ///< TAI nanosecs of change
    double offset; ///< TAI - UTC
    double mjdRef; ///< Intercept for MJD interpolation
    double drift; ///< Slope of MJD interpolation
};

class LeapTable : public std::vector<Leap> {
public:
    LeapTable(void);
};

static LeapTable leapSecTable;

LeapTable::LeapTable(void) {
    dafBase::DateTime::initializeLeapSeconds(leapString);
}

/** Convert UTC time to TAI time.
 * \param[in] nsecs Number of nanoseconds since the epoch in UTC
 * \return Number of nanoseconds since the epoch in TAI
 */
static long long utcToTai(long long nsecs) {
    size_t i;
    for (i = 0; i < leapSecTable.size(); ++i) {
        if (nsecs < leapSecTable[i].whenUtc) break;
    }
    if (i == 0) {
        throw LSST_EXCEPT(
            lsst::pex::exceptions::DomainErrorException,
            (boost::format(
                    "DateTime value too early for UTC-TAI conversion: %1%"
                    ) % nsecs).str());
    }
    Leap const& l(leapSecTable[i - 1]);
    double mjd = static_cast<double>(nsecs) / NSEC_PER_DAY + EPOCH_IN_MJD;
    double leapSecs = l.offset + (mjd - l.mjdRef) * l.drift;
    long long leapNSecs = static_cast<long long>(leapSecs * 1.0e9 + 0.5);
    return nsecs + leapNSecs;
}

/** Convert TAI time to UTC time.
 * \param[in] nsecs Number of nanoseconds since the epoch in TAI
 * \return Number of nanoseconds since the epoch in UTC
 */
static long long taiToUtc(long long nsecs) {
    size_t i;
    for (i = 0; i < leapSecTable.size(); ++i) {
        if (nsecs < leapSecTable[i].whenTai) break;
    }
    if (i == 0) {
        throw LSST_EXCEPT(
            lsst::pex::exceptions::DomainErrorException,
            (boost::format(
                    "DateTime value too early for TAI-UTC conversion: %1%"
                    ) % nsecs).str());
    }
    Leap const& l(leapSecTable[i - 1]);
    double taiSecs = nsecs / 1.0e9;
    double leapSecs = taiSecs -
        (taiSecs - l.offset - l.drift * (EPOCH_IN_MJD - l.mjdRef)) /
        (1.0 + l.drift * 1.0e9 / NSEC_PER_DAY);
    long long leapNSecs = static_cast<long long>(leapSecs * 1.0e9 + 0.5);
    return nsecs - leapNSecs;
}



/** Constructor.
 * \param[in] nsecs Number of nanoseconds since the epoch.
 * \param[in] scale Timescale of input (TAI or UTC, default TAI).
 */
dafBase::DateTime::DateTime(long long nsecs, Timescale scale) : _nsecs(nsecs) {
    if (scale == UTC) {
        _nsecs = utcToTai(_nsecs);
    }
}

/** Constructor.
 * \param[in] mjd Modified Julian Day.
 * \param[in] scale Timescale of input (TAI or UTC, default TAI).
 */
dafBase::DateTime::DateTime(double mjd, Timescale scale) {
    if (mjd > EPOCH_IN_MJD + MAX_DAYS) {
        throw LSST_EXCEPT(
            lsst::pex::exceptions::DomainErrorException,
            (boost::format("MJD too far in the future: %1%") % mjd).str());
    }
    if (mjd < EPOCH_IN_MJD - MAX_DAYS) {
        throw LSST_EXCEPT(
            lsst::pex::exceptions::DomainErrorException,
            (boost::format("MJD too far in the past: %1%") % mjd).str());
    }
    _nsecs = static_cast<long long>((mjd - EPOCH_IN_MJD) * NSEC_PER_DAY);
    if (scale == UTC) {
        _nsecs = utcToTai(_nsecs);
    }
}

/** Constructor.
 * \param[in] year Year number.
 * \param[in] month Month number (Jan = 1).
 * \param[in] day Day number (1 to 31).
 * \param[in] hr Hour number (0 to 23).
 * \param[in] min Minute number (0 to 59).
 * \param[in] sec Second number (0 to 60).
 * \param[in] scale Timescale of input (TAI or UTC, default TAI).
 */
dafBase::DateTime::DateTime(int year, int month, int day,
                            int hr, int min, int sec, Timescale scale) {
    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hr;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_isdst = 0;
    tm.tm_gmtoff = 0;

    // Convert to seconds since the epoch, correcting to UTC.
    time_t secs = mktime(&tm);
    if (secs == -1) {
        throw LSST_EXCEPT(
            lsst::pex::exceptions::DomainErrorException,
            (boost::format("Unconvertible date: %04d-%02d-%02dT%02d:%02d:%02d")
             % year % month % day % hr % min % sec).str());
    }

    secs -= ::timezone;

    _nsecs = secs * LL_NSEC_PER_SEC;
    if (scale == UTC) {
        _nsecs = utcToTai(_nsecs);
    }
}

/** Constructor.  Accepts a restricted subset of ISO8601:
  * yyyy-mm-ddThh:mm:ss.nnnnnnnnnZ where the - and : separators are optional,
  * the fractional seconds are also optional, and the decimal point may be a
  * comma.
 * \param[in] iso8601 ISO8601 representation of date and time.  Must be UTC.
 */
dafBase::DateTime::DateTime(std::string const& iso8601) {
    boost::regex re("(\\d{4})-?(\\d{2})-?(\\d{2})" "T"
                    "(\\d{2}):?(\\d{2}):?(\\d{2})" "([.,]\\d*)?" "Z");
    boost::smatch matches;
    if (!regex_match(iso8601, matches, re)) {
        throw LSST_EXCEPT(lsst::pex::exceptions::DomainErrorException,
                          "Not in acceptable ISO8601 format: " + iso8601);
    }
    DateTime dt(atoi(matches.str(1).c_str()), atoi(matches.str(2).c_str()),
                atoi(matches.str(3).c_str()), atoi(matches.str(4).c_str()),
                atoi(matches.str(5).c_str()), atoi(matches.str(6).c_str()),
                UTC);
    _nsecs = dt._nsecs;
    if (matches[7].matched) {
        _nsecs += atoi(matches.str(7).c_str() + 1);
    }
}


/** Accessor.
 * \return Number of nanoseconds since the epoch in UTC or TAI.
 */
long long dafBase::DateTime::nsecs(Timescale scale) const {
    if (scale == TAI) {
        return _nsecs;
    }
    else {
        return taiToUtc(_nsecs);
    }
}

/** Convert to Modified Julian Day.
 * \param[in] scale Desired timescale (TAI or UTC, default TAI).
 * \return The Modified Julian Day corresponding to the time.
 */
double dafBase::DateTime::mjd(Timescale scale) const {
    if (scale == TAI) {
        return static_cast<double>(_nsecs) / NSEC_PER_DAY + EPOCH_IN_MJD;
    }
    else {
        return static_cast<double>(taiToUtc(_nsecs)) / NSEC_PER_DAY +
            EPOCH_IN_MJD;
    }
}

/** Convert to struct tm.
 * \return Structure with decoded time in UTC.
 */
struct tm dafBase::DateTime::gmtime(void) const {
    struct tm gmt;
    time_t secs = static_cast<time_t>(taiToUtc(_nsecs) / LL_NSEC_PER_SEC);
    gmtime_r(&secs, &gmt);
    return gmt;
}

/** Convert to struct timespec.
 * \return Structure with UTC time in seconds and nanoseconds.
 */
struct timespec dafBase::DateTime::timespec(void) const {
    struct timespec ts;
    long long nsecs = taiToUtc(_nsecs);
    ts.tv_sec = static_cast<time_t>(nsecs / LL_NSEC_PER_SEC);
    ts.tv_nsec = static_cast<int>(nsecs % LL_NSEC_PER_SEC);
    return ts;
}

/** Convert time to struct timeval.
 * \return Structure with time in seconds and microseconds.
 */
struct timeval dafBase::DateTime::timeval(void) const {
    struct timeval tv;
    long long nsecs = taiToUtc(_nsecs);
    tv.tv_sec = static_cast<time_t>(nsecs / LL_NSEC_PER_SEC);
    tv.tv_usec = static_cast<int>((nsecs % LL_NSEC_PER_SEC) / 1000);
    return tv;
}

/** Accessor.
 * \return ISO8601-formatted string representation.
 */
std::string dafBase::DateTime::toString(void) const {
    struct tm gmt(this->gmtime());
    return (boost::format("%04d-%02d-%02dT%02d:%02d:%02d.%09dZ") %
            (gmt.tm_year + 1900) % (gmt.tm_mon + 1) % gmt.tm_mday %
            gmt.tm_hour % gmt.tm_min % gmt.tm_sec %
            (taiToUtc(_nsecs) % LL_NSEC_PER_SEC)).str();
}

/** Return current time as a DateTime.
  * \return DateTime representing the current time.
  */
dafBase::DateTime dafBase::DateTime::now(void) {
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret != 0) {
        throw LSST_EXCEPT(lsst::pex::exceptions::RuntimeErrorException,
                          "Unable to get current time");
    }
    long long nsecs = tv.tv_sec * LL_NSEC_PER_SEC + tv.tv_usec * 1000LL;
    return DateTime(nsecs, DateTime::UTC);
}

/** Initialize leap second table.
  * \param leapString Leap second table from USNO as a single multiline string.
  */
void dafBase::DateTime::initializeLeapSeconds(std::string const& leapString) {
    Leap l;
    leapSecTable.clear();
    boost::regex re("^\\d{4}.*?=JD\\s*([\\d.]+)\\s+TAI-UTC=\\s+([\\d.]+)\\s+S"
                    " \\+ \\(MJD - ([\\d.]+)\\) X ([\\d.]+)\\s*S$");
    for (boost::cregex_iterator i = make_regex_iterator(leapString.c_str(), re);
         i != boost::cregex_iterator(); ++i) {
        double mjdUtc = strtod((*i)[1].first, 0) - MJD_TO_JD;
        l.offset = strtod((*i)[2].first, 0);
        l.mjdRef = strtod((*i)[3].first, 0);
        l.drift = strtod((*i)[4].first, 0);
        l.whenUtc = static_cast<long long>(
            (mjdUtc - EPOCH_IN_MJD) * NSEC_PER_DAY);
        l.whenTai = l.whenUtc + static_cast<long long>(
            1.0e9 * (l.offset + (mjdUtc - l.mjdRef) * l.drift));
        leapSecTable.push_back(l);
    }
}

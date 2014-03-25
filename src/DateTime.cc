// -*- lsst-c++ -*-

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

#include <limits>
#include <cmath>

#include "lsst/daf/base/DateTime.h"

#include "boost/format.hpp"
#include "boost/regex.hpp"
#include <vector>

#include "lsst/pex/exceptions.h"

namespace dafBase = lsst::daf::base;
namespace pexEx   = lsst::pex::exceptions;

// Epoch = 1970 JAN  1 00:00:00 = JD 2440587.5 = MJD 40587.0
static double const MJD_TO_JD = 2400000.5;
static double const EPOCH_IN_MJD = 40587.0;
static double const JD2000 = 2451544.50;

/// Nanoseconds per day.
static double const NSEC_PER_DAY = 86.4e12;

/// Nanoseconds per day/second as a long long.
static long long const LL_NSEC_PER_SEC = 1000000000LL;
// static long long const LL_NSEC_PER_DAY = 86400 * LL_NSEC_PER_SEC;

// Maximum number of days expressible as signed 64-bit nanoseconds.
// 2^64 / 2 / 1e9 / 86400
// NOTE: long long nsecs will wrap:
// -- earliest date representable = sep 21, 1677 00:00:00
// -- latest date representable   = apr 12, 2262 00:00:00
static double const MAX_DAYS = 106751.99;


#ifdef CAL_TO_JD
static double const HOURS_PER_DAY = 24.0;
static double const MIN_PER_DAY   = 1440.0;
static double const SEC_PER_DAY   = 86400.0;
#endif

// Difference between Terrestrial Time and TAI.
static double const TT_MINUS_TAI_NSECS = 32184000000LL;

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
2012 JUL  1 =JD 2456109.5  TAI-UTC=  35.0       S + (MJD - 41317.) X 0.0      S\n\
";

// Anonymous namespace for structures, classes, and formerly file-static
// functions.
namespace {

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

LeapTable leapSecTable;

LeapTable::LeapTable(void) {
    dafBase::DateTime::initializeLeapSeconds(leapString);
}

/** Convert UTC time to TAI time.
 * \param[in] nsecs Number of nanoseconds since the epoch in UTC
 * \return Number of nanoseconds since the epoch in TAI
 */
template<typename NsType>
NsType utcToTai(NsType nsecs) {
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
    NsType leapNSecs = static_cast<NsType>(leapSecs * 1.0e9 + 0.5);
    return nsecs + leapNSecs;
}

/** Convert TAI time to UTC time.
 * \param[in] nsecs Number of nanoseconds since the epoch in TAI
 * \return Number of nanoseconds since the epoch in UTC
 */
template<typename NsType>
NsType taiToUtc(NsType nsecs) {
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
    double mjd = static_cast<double>(nsecs) / NSEC_PER_DAY + EPOCH_IN_MJD;
    double leapSecs = l.offset + (mjd - l.mjdRef) * l.drift;
    // Correct for TAI MJD vs. UTC MJD.
    leapSecs /= 1.0 + l.drift * 1.0e9 / NSEC_PER_DAY;
    NsType leapNSecs = static_cast<NsType>(leapSecs * 1.0e9 + 0.5);
    return nsecs - leapNSecs;
}


#ifdef CAL_TO_JD
/**
 * A function to compute the Julian day from a calendar date.
 * \param[in] year Year number.
 * \param[in] month Month number (Jan = 1).
 * \param[in] day Day number (1 to 31).
 * \param[in] hr Hour number (0 to 23).
 * \param[in] min Minute number (0 to 59).
 * \param[in] sec Second number (0 to 60).
 *
 */
double calendarToJd(int year, int month, int day, int hour, int min, double sec) {
    if ( month <= 2 ) {
    	year -= 1;
    	month += 12;
    }
    int a = int(year/100);
    int b =  2 - a + int(a/4); 

    int yy = 1582, mm = 10; //, d = 4;
    if (year < yy || (year == yy && month < mm) || (year == yy && month == mm && day <= 4)) {
        b = 0;
    }

    double jd = static_cast<int>(365.25*(year + 4716)) +
        static_cast<int>(30.6001*(month + 1)) + day + b - 1524.5;
    jd += hour/HOURS_PER_DAY + min/MIN_PER_DAY + sec/SEC_PER_DAY;

    return jd;
}

#endif // CAL_TO_JD
} // end anonymous namespace


/**
 * @brief a function to convert MJD to interal nsecs
 * @param[in] mjd The Modified Julian Day
 * @param[in] scale The time scale (TAI, or UTC)
 *
 */
void dafBase::DateTime::setNsecsFromMjd(double mjd, Timescale scale) {

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
    } else if (scale == TT) {
        _nsecs -= TT_MINUS_TAI_NSECS;
    }

}
    
/**
 * @brief a function to convert JD to internal nsecs
 * @param[in] jd The Julian Day
 * @param[in] scale The time scale (TAI, or UTC)
 */
void dafBase::DateTime::setNsecsFromJd(double jd, Timescale scale) {
    setNsecsFromMjd(jd - MJD_TO_JD, scale);
}
    
/**
 * @brief a function to convert epoch to internal nsecs
 * @param[in] epoch The Julian epoch
 * @param[in] scale The time scale (TAI, or UTC)
 */
void dafBase::DateTime::setNsecsFromEpoch(double epoch, Timescale scale) {
    setNsecsFromMjd(365.25*(epoch - 2000.0) + JD2000 - MJD_TO_JD, scale);
}
    


/** Constructor.
 * \param[in] nsecs Number of nanoseconds since the epoch.
 * \param[in] scale Timescale of input (TAI or UTC, default TAI).
 */
dafBase::DateTime::DateTime(long long nsecs, Timescale scale) : _nsecs(nsecs) {
    if (scale == UTC) {
        _nsecs = utcToTai(_nsecs);
    } else if (scale == TT) {
        _nsecs -= TT_MINUS_TAI_NSECS;
    }
}

/** Constructor.
 * \param[in] date Date.
 * \param[in] system The requested date system (JD, MJD, or Julian epoch)
 * \param[in] scale Timescale of input (TAI or UTC, default TAI).
 */
dafBase::DateTime::DateTime(double date, DateSystem system, Timescale scale) {
    switch (system) {
      case MJD:
        setNsecsFromMjd(date, scale);
        break;
      case JD:
        setNsecsFromJd(date, scale);
        break;
      case EPOCH:
        setNsecsFromEpoch(date, scale);
        break;
      default:
        throw LSST_EXCEPT(pexEx::InvalidParameterException, "DateSystem must be MJD, JD, or EPOCH.");
        break;
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
    
    // long long nsecs will blow out beyond sep 21, 1677 0:00:00, and apr 12 2262 00:00:00
    // (refering to the values of EPOCH_IN_MJD +/- MAX_DAYS ... exceeds 64 bits.)
    // However, a tm struct is only 32 bits, and saturates at:
    //    low end - Dec 13 1901, 20:45:52
    //    hi end  - Jan 19 2038, 03:14:07
    
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
    } else if (scale == TT) {
        _nsecs -= TT_MINUS_TAI_NSECS;
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
                    "(\\d{2}):?(\\d{2}):?(\\d{2})" "([.,](\\d*))?" "Z");
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
        std::string frac = matches.str(8);
        int places = frac.size();
        if (places > 9) { // truncate fractional nanosec
            frac.erase(9);
        }
        int value = atoi(frac.c_str());
        while (places < 9) {
            value *= 10;
            ++places;
        }
        _nsecs += value;
    }
}


/** Generic Accessor
 * @return the date in the required system, for the requested scale
 * @param[in] system The type of date requested (JD, MJD, or EPOCH)
 * @param[in] scale The time scale (UTC, or TAI)
 *
 * @note The NSECS can't be requested here as they're in long long form.
 *       A factory could be constructed, but it's more trouble than it's worth at this point.
 */
double dafBase::DateTime::get(DateSystem system, Timescale scale) const {
    switch (system) {
      case MJD:
        return _getMjd(scale);
        break;
      case JD:
        return _getJd(scale);
        break;
      case EPOCH:
        return _getEpoch(scale);
        break;
      default:
        throw LSST_EXCEPT(pexEx::InvalidParameterException,
                          "DateSystem must be MJD, JD, or EPOCH.");
        break;
    }
}


/** Accessor.
 * \return Number of nanoseconds since the epoch in UTC or TAI.
 */
long long dafBase::DateTime::nsecs(Timescale scale) const {
    if (scale == TAI) {
        return _nsecs;
    } else if (scale == TT) {
        return _nsecs + TT_MINUS_TAI_NSECS;
    } else {
        return taiToUtc(_nsecs);
    }
}


/** Convert to Modified Julian Day.
 * \param[in] scale Desired timescale (TAI or UTC, default TAI).
 * \return The Modified Julian Day corresponding to the time.
 */
double dafBase::DateTime::_getMjd(Timescale scale) const {

    double nsecs;
    if (scale == TAI) {
        nsecs = static_cast<double>(_nsecs);
    } else if (scale == TT) {
        nsecs = static_cast<double>(_nsecs) + TT_MINUS_TAI_NSECS;
    } else {
        nsecs = static_cast<double>(taiToUtc(_nsecs));
    }
    return nsecs / NSEC_PER_DAY + EPOCH_IN_MJD;
}


/** Convert to Julian Day.
 * \param[in] scale Desired timescale (TAI or UTC, default TAI).
 * \return The Julian Day corresponding to the time.
 */
double dafBase::DateTime::_getJd(Timescale scale) const {
    return _getMjd(scale) + MJD_TO_JD;
}

/** Convert to Julian Epoch.
 * \param[in] scale Desired timescale (TAI or UTC, default TAI).
 * \return The Julian Epoch corresponding to the time.
 */
double dafBase::DateTime::_getEpoch(Timescale scale) const {
    return 2000.0 + (_getJd(scale) - JD2000)/365.25;
}




/** Convert to struct tm.  Truncate fractional seconds.
 * \return Structure with decoded time in UTC.
 */
struct tm dafBase::DateTime::gmtime(void) const {
    struct tm gmt;
    long long nsecs = taiToUtc(_nsecs);
    // Round to negative infinity
    long long frac = nsecs % LL_NSEC_PER_SEC;
    if (nsecs < 0 && frac < 0) {
        nsecs -= LL_NSEC_PER_SEC + frac;
    }
    else {
        nsecs -= frac;
    }
    time_t secs = static_cast<time_t>(nsecs / LL_NSEC_PER_SEC);
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
 * \return Structure with UTC time in seconds and microseconds.
 */
struct timeval dafBase::DateTime::timeval(void) const {
    struct timeval tv;
    long long nsecs = taiToUtc(_nsecs);
    tv.tv_sec = static_cast<time_t>(nsecs / LL_NSEC_PER_SEC);
    tv.tv_usec = static_cast<int>((nsecs % LL_NSEC_PER_SEC) / 1000);
    return tv;
}

/** Accessor.
 * \return ISO8601-formatted string representation.  Always UTC.
 */
std::string dafBase::DateTime::toString(void) const {
    struct tm gmt(this->gmtime());
    long long nsecs = taiToUtc(_nsecs) % LL_NSEC_PER_SEC;
    if (nsecs < 0) {
        nsecs += LL_NSEC_PER_SEC;
    }
    return (boost::format("%04d-%02d-%02dT%02d:%02d:%02d.%09dZ") %
            (gmt.tm_year + 1900) % (gmt.tm_mon + 1) % gmt.tm_mday %
            gmt.tm_hour % gmt.tm_min % gmt.tm_sec % nsecs).str();
}

/** Equality operator.
 * \return True if both DateTimes have the same nanosecond representation.
 */
bool dafBase::DateTime::operator==(DateTime const& rhs) const {
    return _nsecs == rhs._nsecs;
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

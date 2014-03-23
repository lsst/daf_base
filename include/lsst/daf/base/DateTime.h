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
 
#ifndef LSST_DAF_BASE_DATETIME_H
#define LSST_DAF_BASE_DATETIME_H

/** @file
  * @ingroup daf_base
  *
  * @brief Interface for DateTime class
  *
  * @author Kian-Tat Lim (ktl@slac.stanford.edu)
  * @version $Revision$
  * @date $Date$
  */

/** @class lsst::daf::base::DateTime
  * @brief Class for handling dates/times, including MJD, UTC, and TAI.
  *
  * @ingroup daf_base
  */

#include <ctime>
#include <sys/time.h>
#include <string>

// Forward declaration of the boost::serialization::access class.
namespace boost {
namespace serialization {
    class access;
}} // namespace boost::serialization

namespace lsst {
namespace daf {
namespace base {

class DateTime {
public:
    enum Timescale { TAI, UTC, TT };
    enum DateSystem { JD, MJD, EPOCH };
    explicit DateTime(long long nsecs = 0LL, Timescale scale = TAI);
    explicit DateTime(double date, DateSystem system = MJD, Timescale scale = TAI);
    DateTime(int year, int month, int day, int hr, int min, int sec,
             Timescale scale = TAI);
    explicit DateTime(std::string const& iso8601);

    long long nsecs(Timescale scale = TAI) const;
    double mjd(Timescale scale = TAI) const {  // deprecated, please use get(MJD)
        return _getMjd(scale);
    }
    double get(DateSystem system = MJD, Timescale scale = TAI) const;
    std::string toString(void) const;

    struct tm gmtime(void) const; // Always UTC
    struct timespec timespec(void) const; // Always UTC
    struct timeval timeval(void) const; // Always UTC

    bool operator==(DateTime const& rhs) const;

    static DateTime now(void);

    static void initializeLeapSeconds(std::string const& leapString);

private:
    long long _nsecs;  ///< Nanoseconds since Unix epoch

    double _getMjd(Timescale scale) const;
    double _getJd(Timescale scale) const;
    double _getEpoch(Timescale scale) const;

    void setNsecsFromMjd(double mjd, Timescale scale);
    void setNsecsFromJd(double jd, Timescale scale);
    void setNsecsFromEpoch(double epoch, Timescale scale);
    
    friend class boost::serialization::access;
    /** Serialize DateTime to/from a Boost archive.
      * @param[in,out] ar   Archive to access.
      * @param[in] version  Version of class serializer.
      */
    template <class Archive> void serialize(Archive ar, int const version) {
        ar & _nsecs;
    }

};

}}} // namespace lsst::daf::base

#endif

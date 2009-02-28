// -*- lsst-c++ -*-
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
    enum Timescale { TAI, UTC };
    explicit DateTime(long long nsecs = 0LL, Timescale scale = TAI);
    explicit DateTime(double mjd, Timescale scale = TAI);
    DateTime(int year, int month, int day, int hr, int min, int sec,
             Timescale scale = TAI);

    long long nsecs(Timescale scale = TAI) const;
    double mjd(Timescale scale = TAI) const;

    struct tm gmtime(void) const; // Always UTC
    struct timespec timespec(void) const; // Always UTC
    struct timeval timeval(void) const; // Always UTC

    static void initializeLeapSeconds(std::string const& leapString);

    friend class boost::serialization::access;
    /** Serialize DateTime to/from a Boost archive.
      * @param[in,out] ar   Archive to access.
      * @param[in] version  Version of class serializer.
      */
    template <class Archive> void serialize(Archive ar, int const version) {
        ar & _nsecs;
    };

private:
    long long _nsecs;
        ///< Nanoseconds since Unix epoch
};

}}} // namespace lsst::daf::base

#endif

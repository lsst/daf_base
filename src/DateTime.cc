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

#include "boost/python.hpp"
#define LSST_DAF_BASE_DATETIME_INTERNAL 1
#include "lsst/daf/base/DateTime.h"

#include "lsst/pex/exceptions.h"

namespace dafBase = lsst::daf::base;
namespace pexEx   = lsst::pex::exceptions;

/// Nanoseconds per day/second as a long long.
static long long const LL_NSEC_PER_SEC = 1000000000LL;

std::string _getStringFromPyExc( boost::python::error_already_set const &e ) {
  using namespace boost::python;
  PyObject * extype, * value, * traceback;
  std::string errstr;
  PyErr_Fetch(&extype, &value, &traceback);
  if (!extype) return errstr;

  object o_extype(handle<>(borrowed(extype)));
  object o_value(handle<>(borrowed(value)));
  object o_traceback(handle<>(borrowed(traceback)));

  object mod_traceback = import("traceback");
  object lines = mod_traceback.attr("format_exception")(
    o_extype, o_value, o_traceback);

  for (int i = 0; i < len(lines); ++i)
    errstr += extract<std::string>(lines[i])();

  PyErr_Clear();
  return errstr;
}

// We have caught a python exception. This needs to be rethrown
// as a C++ exception. We really need to look at the Python exception
// type and pick the corresponding PEX exception.
// Pex exceptions: Domain. InvalidParameter, Io, Length, Logic, Memory,
// NotFound, OutOfRange, Overflow, Range, Runtime, Timeout, Type, Underflow.
//
// If we don't recognize it throw a RuntimeError. For now we just do that.
//
// One problem is that the traceback reports this function rather than the
// caller. Obviously we also lose the Python exception details.

void _translatePyException( boost::python::error_already_set const &e ) {
  using namespace boost::python;
  PyObject * extype, * value, * traceback;
  std::string errstr;
  PyErr_Fetch(&extype, &value, &traceback);

  // We have no real idea how we got here
  if (!extype) return;

  object o_extype(handle<>(borrowed(extype)));
  object o_value(handle<>(borrowed(value)));

  object mod_traceback = import("traceback");
  object lines = mod_traceback.attr("format_exception_only")(
    o_extype, o_value);

  for (int i = 0; i < len(lines); ++i)
    errstr += extract<std::string>(lines[i])();

  PyErr_Clear();
  throw LSST_EXCEPT(pexEx::RuntimeError, errstr);
}


boost::python::object getPyClass(void) {
  static boost::python::object cls;
  if (!cls) {
    // Do not need the initialize if we are python calling c++ calling python
    // Py_Initialize();
    cls = boost::python::import("lsstx.DateTime");
  }
  return cls;
}

/** Constructor.
 *  No arguments. Assumes 0 nanosec.
 *  Do not use. A fudge to fool SWIG. SWIG does not expect default constructors
 *  to ever throw an exception so it does not trap exceptions. Other constructors
 *  are trapped though.
 */
dafBase::DateTime::DateTime() {
    boost::python::object cls = getPyClass();
    _pyself = cls.attr("DateTime")(0LL);
}

/** Constructor.
 * \param[in] nsecs Number of nanoseconds since the epoch.
 * \param[in] scale Timescale of input (TAI or UTC, default TAI). X
 */
dafBase::DateTime::DateTime(long long nsecs, Timescale scale) {
  std::string errstr;
  try {
    boost::python::object cls = getPyClass();
    _pyself = cls.attr("DateTime")(nsecs, (int)scale);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in DateTime(nsecs, scale)" << std::endl;
    _translatePyException(e);
  }
}

/** Constructor.
 * \param[in] date Date.
 * \param[in] system The requested date system (JD, MJD, or Julian epoch)
 * \param[in] scale Timescale of input (TAI or UTC, default TAI).
 */
dafBase::DateTime::DateTime(double date, DateSystem system, Timescale scale) {
  try {
    boost::python::object cls = getPyClass();
    _pyself = cls.attr("DateTime")(date, (int)system, (int)scale);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in DateTime(mjd, system, scale)" << std::endl;
    _translatePyException(e);
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
  try {
    boost::python::object cls = getPyClass();
    _pyself = cls.attr("DateTime")(year, month, day, hr, min, sec, (int)scale);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in DateTime(y, m, d, h, m, s, scale)" << std::endl;
    _translatePyException(e);
  }

}

/** Constructor.  Accepts a restricted subset of ISO8601:
  * yyyy-mm-ddThh:mm:ss.nnnnnnnnnZ where the - and : separators are optional,
  * the fractional seconds are also optional, and the decimal point may be a
  * comma.
 * \param[in] iso8601 ISO8601 representation of date and time.  Must be UTC.
 */
dafBase::DateTime::DateTime(std::string const& iso8601) {
   try {
    boost::python::object cls = getPyClass();
    _pyself = cls.attr("DateTime")(iso8601);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in DateTime(iso8601)" << std::endl;
    _translatePyException(e);
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

  try {
    boost::python::object retval = _pyself.attr("get")((int)system, (int)scale);
    return boost::python::extract<double>(retval);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in get()" << std::endl;
    _translatePyException(e);
  }
}


/** Accessor.
 * \return Number of nanoseconds since the epoch in UTC or TAI.
 */
long long dafBase::DateTime::nsecs(Timescale scale) const {
  try {
    boost::python::object retval = _pyself.attr("nsecs")((int)scale);
    return boost::python::extract<long long>(retval);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in nsecs()" << std::endl;
    _translatePyException(e);
  }
}

/** Convert to struct tm.  Truncate fractional seconds.
 * \return Structure with decoded time in UTC.
 */
struct tm dafBase::DateTime::gmtime(void) const {
    struct tm gmt;
    time_t secs = 0;
    throw LSST_EXCEPT(pexEx::RuntimeError, "gmtime not yet implemented");
    gmtime_r(&secs, &gmt);
    return gmt;
}

/** Convert to struct timespec.
 * \return Structure with UTC time in seconds and nanoseconds.
 */
struct timespec dafBase::DateTime::timespec(void) const {
    struct timespec ts;
    throw LSST_EXCEPT(pexEx::RuntimeError, "timespec not yet implemented");
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    return ts;
}

/** Convert time to struct timeval.
 * \return Structure with UTC time in seconds and microseconds.
 */
struct timeval dafBase::DateTime::timeval(void) const {
    struct timeval tv;
    throw LSST_EXCEPT(pexEx::RuntimeError, "timeval not yet implemented");
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return tv;
}

/** Accessor.
 * \return ISO8601-formatted string representation.  Always UTC.
 */
std::string dafBase::DateTime::toString(void) const {
  try {
    boost::python::object retval = _pyself.attr("__str__")();
    return boost::python::extract<std::string>(retval);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in toString()" << std::endl;
    _translatePyException(e);
  }
}

/** Equality operator.
 * \return True if both DateTimes have the same nanosecond representation.
 */
bool dafBase::DateTime::operator==(DateTime const& rhs) const {
  try {
    boost::python::object retval = _pyself.attr("__eq__")(rhs._pyself);
    return boost::python::extract<bool>(retval);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in __eq__()" << std::endl;
    _translatePyException(e);
  }
}

/** Non-Equality operator.
 * \return False if both DateTimes have the same nanosecond representation.
 */
bool dafBase::DateTime::operator!=(DateTime const& rhs) const {
  try {
    boost::python::object retval = _pyself.attr("__ne__")(rhs._pyself);
    return boost::python::extract<bool>(retval);
  } catch (boost::python::error_already_set const &e) {
    std::cout << "Caught python error in __ne__()" << std::endl;
    _translatePyException(e);
  }
}

/** Return current time as a DateTime.
  * \return DateTime representing the current time.
  */
dafBase::DateTime dafBase::DateTime::now(void) {
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret != 0) {
        throw LSST_EXCEPT(lsst::pex::exceptions::RuntimeError,
                          "Unable to get current time");
    }
    long long nsecs = tv.tv_sec * LL_NSEC_PER_SEC + tv.tv_usec * 1000LL;
    return DateTime(nsecs, DateTime::UTC);
}

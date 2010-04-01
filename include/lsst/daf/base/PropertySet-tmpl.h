// -*- lsst-c++ -*-

/** @file
  * @ingroup daf_base
  *
  * @brief PropertySet template definitions
  *
  * @version $Revision$
  * @date $Date$
  *
  * Contact: Kian-Tat Lim (ktl@slac.stanford.edu)
  */
#include "lsst/daf/base/PropertySet.h"

namespace dafBase = lsst::daf::base;
namespace pexExcept = lsst::pex::exceptions;

using namespace std;

/// @cond
// Explicit template instantiations are not well understood by doxygen.

#define PROPERTYSET_INSTANTIATE(t) \
    template t dafBase::PropertySet::get<t>(string const& name) const; \
    template t dafBase::PropertySet::get<t>(string const& name, t const& defaultValue) const; \
    template vector<t> dafBase::PropertySet::getArray<t>(string const& name) const; \
    template void dafBase::PropertySet::set<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::set<t>(string const& name, vector<t> const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, vector<t> const& value);


///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

// The following throw an exception if the type does not match exactly.

/** Get the last value for a property name (possibly hierarchical).
  * Note that the type must be explicitly specified for this template:
  * @code int i = propertySet.get<int>("foo") @endcode
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Last value set or added.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value does not match desired type.
  */
template <typename T>
T dafBase::PropertySet::get(string const& name) const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    try {
        return boost::any_cast<T>(i->second->back());
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    // not reached
    return boost::any_cast<T>(i->second->back());
}

/** Get the last value for a property name (possibly hierarchical).
  * Returns the provided @a defaultValue if the property does not exist.
  * Note that the type must be explicitly specified for this template:
  * @code int i = propertySet.get<int>("foo", 42) @endcode
  * @param[in] name Property name to examine, possibly hierarchical.
  * @param[in] defaultValue Default value to return if property does not exist.
  * @return Last value set or added.
  * @throws TypeMismatchException Value does not match desired type.
  */
template <typename T>
T dafBase::PropertySet::get(string const& name, T const& defaultValue) const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        return defaultValue;
    }
    try {
        return boost::any_cast<T>(i->second->back());
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    // not reached
    return boost::any_cast<T>(i->second->back());
}

/** Get the vector of values for a property name (possibly hierarchical).
  * Note that the type must be explicitly specified for this template:
  * @code vector<int> v = propertySet.getArray<int>("foo") @endcode
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Vector of values.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value does not match desired type.
  */
template <typename T>
vector<T> dafBase::PropertySet::getArray(string const& name) const {
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    vector<T> v;
    for (vector<boost::any>::const_iterator j = i->second->begin();
         j != i->second->end(); ++j) {
        try {
            v.push_back(boost::any_cast<T>(*j));
        }
        catch (boost::bad_any_cast) {
            throw LSST_EXCEPT(TypeMismatchException, name);
        }
    }
    return v;
}

///////////////////////////////////////////////////////////////////////////////
// Modifiers
///////////////////////////////////////////////////////////////////////////////

/** Replace all values for a property name (possibly hierarchical) with a new
  * value.
  * @param[in] name Property name to set, possibly hierarchical.
  * @param[in] value Value to set.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
template <typename T>
void dafBase::PropertySet::set(std::string const& name, T const& value) {
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->push_back(value);
    _findOrInsert(name, vp);
}

/** Replace all values for a property name (possibly hierarchical) with a
  * vector of new values.
  * @param[in] name Property name to set, possibly hierarchical.
  * @param[in] value Vector of values to set.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
template <typename T>
void dafBase::PropertySet::set(std::string const& name,
                               vector<T> const& value) {
    if (value.empty()) return;
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->insert(vp->end(), value.begin(), value.end());
    _findOrInsert(name, vp);
}

/** Appends a single value to the vector of values for a property name
  * (possibly hierarchical).  Sets the value if the property does not exist.
  * @param[in] name Property name to append to, possibly hierarchical.
  * @param[in] value Value to append.
  * @throws TypeMismatchException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
template <typename T>
void dafBase::PropertySet::add(std::string const& name, T const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(TypeMismatchException,
                              name + " has mismatched type");
        }
        i->second->push_back(value);
    }
}

/** Appends a vector of values to the vector of values for a property name
  * (possibly hierarchical).  Sets the values if the property does not exist.
  * @param[in] name Property name to append to, possibly hierarchical.
  * @param[in] value Vector of values to append.
  * @throws TypeMismatchException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  * @note
  * May only partially add the vector if an exception occurs.
  */
template <typename T>
void dafBase::PropertySet::add(std::string const& name,
                               vector<T> const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(TypeMismatchException,
                              name + " has mismatched type");
        }
        i->second->insert(i->second->end(), value.begin(), value.end());
    }
}

/// @endcond

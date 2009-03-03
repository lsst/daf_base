// -*- lsst-c++ -*-

/** @file
  * @ingroup daf_base
  *
  * @brief Implementation for PropertyList class
  *
  * @version $Revision$
  * @date $Date$
  *
  * Contact: Kian-Tat Lim (ktl@slac.stanford.edu)
  */

#ifndef __GNUC__
#  define __attribute__(x) /*NOTHING*/
#endif
static char const* SVNid __attribute__((unused)) = "$Id$";

#include "lsst/daf/base/PropertyList.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "lsst/pex/exceptions/Runtime.h"
#include "lsst/daf/base/DateTime.h"

namespace dafBase = lsst::daf::base;
namespace pexExcept = lsst::pex::exceptions;

using namespace std;

/** Constructor.
  */
dafBase::PropertyList::PropertyList(void) : Citizen(typeid(*this)) {
}

/** Destructor.
  */
dafBase::PropertyList::~PropertyList(void) {
}

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

/** Copy the PropertyList and all of its contents.
  * @return PropertyList::Ptr pointing to the new copy.
  */
dafBase::PropertyList::Ptr dafBase::PropertyList::deepCopy(void) const {
    Ptr n(new PropertyList);
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        boost::shared_ptr< vector<boost::any> > vp(
            new vector<boost::any>(*(i->second)));
        n->_map[i->first] = vp;
        n->_insertOrder = _insertOrder;
    }
    return n;
}

/** Get the number of names in the PropertyList.
  * @return Number of names.
  */
size_t dafBase::PropertyList::nameCount(void) const {
    return _map.size();
}

/** Get the names in the PropertyList.
  * @return Vector of names.
  */
vector<string> dafBase::PropertyList::names(void) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        v.push_back(i->first);
    }
    return v;
}

/** Determine if a name exists.
  * @param[in] name Property name to examine.
  * @return true if property exists.
  */
bool dafBase::PropertyList::exists(std::string const& name) const {
    return _map.find(name) != _map.end();
}

/** Determine if a name has multiple values.
  * @param[in] name Property name to examine.
  * @return true if property exists and has more than one value.
  */
bool dafBase::PropertyList::isArray(std::string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
    return i != _map.end() && i->second->size() > 1U;
}

/** Get number of values for a property name.
  * @param[in] name Property name to examine.
  * @return Number of values for that property.  0 if it doesn't exist.
  */
size_t dafBase::PropertyList::valueCount(std::string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
    if (i == _map.end()) return 0;
    return i->second->size();
}

/** Get the type of values for a property name.
  * @param[in] name Property name to examine.
  * @return Type of values for that property.
  * @throws NotFoundException Property does not exist.
  */
type_info const& dafBase::PropertyList::typeOf(std::string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    return i->second->back().type();
}

// The following throw an exception if the type does not match exactly.

/** Get the last value for a property name.
  * Note that the type must be explicitly specified for this template:
  * @code int i = propertyList.get<int>("foo") @endcode
  * @param[in] name Property name to examine.
  * @return Last value set or added.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value does not match desired type.
  */
template <typename T>
T dafBase::PropertyList::get(string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
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

/** Get the last value for a property name.
  * Returns the provided @a defaultValue if the property does not exist.
  * Note that the type must be explicitly specified for this template:
  * @code int i = propertyList.get<int>("foo", 42) @endcode
  * @param[in] name Property name to examine.
  * @param[in] defaultValue Default value to return if property does not exist.
  * @return Last value set or added.
  * @throws TypeMismatchException Value does not match desired type.
  */
template <typename T>
T dafBase::PropertyList::get(string const& name, T const& defaultValue) const {
    AnyMap::const_iterator i = _map.find(name);
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

/** Get the vector of values for a property name.
  * Note that the type must be explicitly specified for this template:
  * @code vector<int> v = propertyList.getArray<int>("foo") @endcode
  * @param[in] name Property name to examine.
  * @return Vector of values.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value does not match desired type.
  */
template <typename T>
vector<T> dafBase::PropertyList::getArray(string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
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

// The following throw an exception if the conversion is inappropriate.

/** Get the last value for a bool property name.
  * @param[in] name Property name to examine.
  * @return Value as a bool.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value is not a bool.
  */
bool dafBase::PropertyList::getAsBool(std::string const& name) const {
    return get<bool>(name);
}

/** Get the last value for a bool/char/short/int property name.
  * Unsigned int properties are not acceptable, but unsigned
  * versions of smaller types are.
  * @param[in] name Property name to examine.
  * @return Value as an int.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value cannot be converted to int.
  */
int dafBase::PropertyList::getAsInt(std::string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) return boost::any_cast<bool>(v);
    if (t == typeid(char)) return boost::any_cast<char>(v);
    if (t == typeid(signed char)) return boost::any_cast<signed char>(v);
    if (t == typeid(unsigned char)) return boost::any_cast<unsigned char>(v);
    if (t == typeid(short)) return boost::any_cast<short>(v);
    if (t == typeid(unsigned short)) return boost::any_cast<unsigned short>(v);
    try {
        return boost::any_cast<int>(v);
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    // not reached
    return boost::any_cast<int>(v);
}

/** Get the last value for a bool/char/short/int/int64_t property name.
  * Unsigned int64_t properties are not acceptable,
  * but unsigned versions of smaller types are, except possibly unsigned long,
  * depending on compiler.
  * @param[in] name Property name to examine.
  * @return Value as an int64_t.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value cannot be converted to int64_t.
  */
int64_t dafBase::PropertyList::getAsInt64(std::string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) return boost::any_cast<bool>(v);
    if (t == typeid(char)) return boost::any_cast<char>(v);
    if (t == typeid(signed char)) return boost::any_cast<signed char>(v);
    if (t == typeid(unsigned char)) return boost::any_cast<unsigned char>(v);
    if (t == typeid(short)) return boost::any_cast<short>(v);
    if (t == typeid(unsigned short)) return boost::any_cast<unsigned short>(v);
    if (t == typeid(int)) return boost::any_cast<int>(v);
    if (t == typeid(unsigned int)) return boost::any_cast<unsigned int>(v);
    if (t == typeid(long)) return boost::any_cast<long>(v);
    try {
        return boost::any_cast<int64_t>(v);
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    // not reached
    return boost::any_cast<int64_t>(v);
}

/** Get the last value for any arithmetic property name.
  * @param[in] name Property name to examine.
  * @return Value as a double.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value cannot be converted to double.
  */
double dafBase::PropertyList::getAsDouble(std::string const& name) const {
    AnyMap::const_iterator i = _map.find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) return boost::any_cast<bool>(v);
    if (t == typeid(char)) return boost::any_cast<char>(v);
    if (t == typeid(signed char)) return boost::any_cast<signed char>(v);
    if (t == typeid(unsigned char)) return boost::any_cast<unsigned char>(v);
    if (t == typeid(short)) return boost::any_cast<short>(v);
    if (t == typeid(unsigned short)) return boost::any_cast<unsigned short>(v);
    if (t == typeid(int)) return boost::any_cast<int>(v);
    if (t == typeid(unsigned int)) return boost::any_cast<unsigned int>(v);
    if (t == typeid(long)) return boost::any_cast<long>(v);
    if (t == typeid(unsigned long)) return boost::any_cast<unsigned long>(v);
    if (t == typeid(long long)) return boost::any_cast<long long>(v);
    if (t == typeid(unsigned long long)) return boost::any_cast<unsigned long long>(v);
    if (t == typeid(float)) return boost::any_cast<float>(v);
    try {
        return boost::any_cast<double>(v);
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    // not reached
    return boost::any_cast<double>(v);
}

/** Get the last value for a string property name.
  * Note that properties set with <tt>char const*</tt> can be retrieved as
  * strings using this method.
  * @param[in] name Property name to examine.
  * @return String value.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value is not a string.
  */
std::string dafBase::PropertyList::getAsString(std::string const& name) const {
    return get<string>(name);
}

void fitsHeader(ostringstream& s,
                std::string const& name, boost::any const& value,
                type_info const& type) {
    s << boost::format("%-8s") % name << " = ";
    if (type == typeid(bool)) {
        s << (boost::any_cast<bool>(value) ? "T" : "F");
    }
}

std::string dafBase::PropertyList::toFitsHeaders(void) const {
    ostringstream s;
    typedef std::tr1::unordered_map<std::string, int> PosMap;
    PosMap posMap;
    for (list<string>::const_iterator i = _insertOrder.begin();
         i != _insertOrder.end(); ++i) {
        PosMap::iterator p = posMap.find(*i);
        int pos;
        if (p == posMap.end()) {
            pos = 0;
        }
        else {
            pos = p->second;
        }
        AnyMap::const_iterator j = _map.find(*i);
        boost::any const& v = j->second->at(pos);
        type_info const& t = v.type();
        fitsHeader(s, *i, v, t);
    }
    return s.str();
}

/** Generate a string representation of the PropertyList.
  * Use this for debugging, not for serialization/persistence.
  * @return String representation of the PropertyList.
  */
std::string dafBase::PropertyList::toString(void) const {
    ostringstream s;
    vector<string> nv = names();
    sort(nv.begin(), nv.end());
    for (vector<string>::const_iterator i = nv.begin(); i != nv.end(); ++i) {
        AnyMap::const_iterator j = _map.find(*i);
        s << j->first << " = ";
        boost::shared_ptr< vector<boost::any> > vp = j->second;
        if (vp->size() > 1) s << "[ ";
        type_info const& t = vp->back().type();
        for (vector<boost::any>::const_iterator k = vp->begin();
             k != vp->end(); ++k) {
            if (k != vp->begin()) s << ", ";
            boost::any const& v(*k);
            if (t == typeid(bool)) s << boost::any_cast<bool>(v);
            else if (t == typeid(char)) s << '\'' << boost::any_cast<char>(v) << '\'';
            else if (t == typeid(signed char)) s << '\'' << boost::any_cast<signed char>(v) << '\'';
            else if (t == typeid(unsigned char)) s << '\'' << boost::any_cast<unsigned char>(v) << '\'';
            else if (t == typeid(short)) s << boost::any_cast<short>(v);
            else if (t == typeid(unsigned short)) s << boost::any_cast<unsigned short>(v);
            else if (t == typeid(int)) s << boost::any_cast<int>(v);
            else if (t == typeid(unsigned int)) s << boost::any_cast<unsigned int>(v);
            else if (t == typeid(long)) s << boost::any_cast<long>(v);
            else if (t == typeid(unsigned long)) s << boost::any_cast<unsigned long>(v);
            else if (t == typeid(long long)) s << boost::any_cast<long long>(v);
            else if (t == typeid(unsigned long long)) s << boost::any_cast<unsigned long long>(v);
            else if (t == typeid(float)) s << boost::any_cast<float>(v);
            else if (t == typeid(double)) s << boost::any_cast<double>(v);
            else if (t == typeid(string)) s << '"' << boost::any_cast<string>(v) << '"';
            else s << "<Unknown>";
        }
        if (j->second->size() > 1) s << " ]";
        s << endl;
    }
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////
// Modifiers
///////////////////////////////////////////////////////////////////////////////

/** Replace all values for a property name with a new value.
  * @param[in] name Property name to set.
  * @param[in] value Value to set.
  */
template <typename T>
void dafBase::PropertyList::set(std::string const& name, T const& value) {
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->push_back(value);
    _insertOrder.erase(name);
    _map[name] = vp;
    _insertOrder.push_back(name);
}

/** Replace all values for a property name with a vector of new values.
  * @param[in] name Property name to set.
  * @param[in] value Vector of values to set.
  */
template <typename T>
void dafBase::PropertyList::set(std::string const& name,
                               vector<T> const& value) {
    if (value.empty()) return;
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    _insertOrder.erase(name);
    for (vector<T>::const_iterator i = value.begin(); i != value.end(); ++i) {
        vp.push_back(*i);
        _insertOrder.push_back(name);
    }
    _map[name] = vp;
}

/** Replace all values for a property name with a string value.
  * @param[in] name Property name to set.
  * @param[in] value Character string (converted to \c std::string ).
  */
void dafBase::PropertyList::set(std::string const& name, char const* value) {
    set(name, string(value));
}

/** Appends a single value to the vector of values for a property name.
  * Sets the value if the property does not exist.
  * @param[in] name Property name to append to.
  * @param[in] value Value to append.
  * @throws TypeMismatchException Type does not match existing values.
  */
template <typename T>
void dafBase::PropertyList::add(std::string const& name, T const& value) {
    AnyMap::iterator i = _map.find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(TypeMismatchException,
                              name + " has mismatched type");
        }
        i->second->push_back(value);
        _insertOrder.push_back(name);
    }
}

/** Appends a vector of values to the vector of values for a property name.
  * Sets the values if the property does not exist.
  * @param[in] name Property name to append to.
  * @param[in] value Vector of values to append.
  * @throws TypeMismatchException Type does not match existing values.
  * @note
  * May only partially add the vector if an exception occurs.
  */
template <typename T>
void dafBase::PropertyList::add(std::string const& name,
                               vector<T> const& value) {
    AnyMap::iterator i = _map.find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(TypeMismatchException,
                              name + " has mismatched type");
        }
        for (vector<T>::const_iterator j = value.begin();
             j != value.end(); ++j) {
            i->second->push_back(*j);
            _insertOrder.push_back(name);
        }
    }
}

/** Appends a <tt>char const*</tt> value to the vector of values for a
  * property name.  Sets the value if the property does not exist.
  * @param[in] name Property name to append to.
  * @param[in] value Character string value to append.
  * @throws TypeMismatchException Type does not match existing values.
  */
void dafBase::PropertyList::add(std::string const& name, char const* value) {
   add(name, string(value));
}

/** Removes all values for a property name.  Does nothing if the property does
  * not exist.
  * @param[in] name Property name to remove.
  */
void dafBase::PropertyList::remove(std::string const& name) {
    _map.erase(name);
    _insertOrder.erase(name);
}

///////////////////////////////////////////////////////////////////////////////
// Explicit template instantiations
///////////////////////////////////////////////////////////////////////////////

/// @cond
// Explicit template instantiations are not well understood by doxygen.

#define INSTANTIATE(t) \
    template t dafBase::PropertyList::get<t>(string const& name) const; \
    template t dafBase::PropertyList::get<t>(string const& name, t const& defaultValue) const; \
    template vector<t> dafBase::PropertyList::getArray<t>(string const& name) const; \
    template void dafBase::PropertyList::set<t>(string const& name, t const& value); \
    template void dafBase::PropertyList::set<t>(string const& name, vector<t> const& value); \
    template void dafBase::PropertyList::add<t>(string const& name, t const& value); \
    template void dafBase::PropertyList::add<t>(string const& name, vector<t> const& value);

INSTANTIATE(bool)
INSTANTIATE(char)
INSTANTIATE(signed char)
INSTANTIATE(unsigned char)
INSTANTIATE(short)
INSTANTIATE(unsigned short)
INSTANTIATE(int)
INSTANTIATE(unsigned int)
INSTANTIATE(long)
INSTANTIATE(unsigned long)
INSTANTIATE(long long)
INSTANTIATE(unsigned long long)
INSTANTIATE(float)
INSTANTIATE(double)
INSTANTIATE(string)
INSTANTIATE(dafBase::DateTime)

/// @endcond

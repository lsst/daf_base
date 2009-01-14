// -*- lsst-c++ -*-

/** @file
  * @ingroup daf_base
  *
  * @brief Implementation for PropertySet class
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

#include "lsst/daf/base/PropertySet.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "lsst/pex/exceptions/Runtime.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/detail/AnyVal.h"

namespace dafBase = lsst::daf::base;
namespace pexExcept = lsst::pex::exceptions;

using namespace std;

/** Constructor.
  */
dafBase::PropertySet::PropertySet(void) : Citizen(typeid(*this)) {
}

/** Destructor.
  */
dafBase::PropertySet::~PropertySet(void) {
}

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

/** Copy the PropertySet and all of its contents.
  * @return PropertySet::Ptr pointing to the new copy.
  */
dafBase::PropertySet::Ptr dafBase::PropertySet::deepCopy(void) const {
    Ptr n(new PropertySet);
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second.type() == typeid(Ptr)) {
            n->set(i->first, i->second.getPtr()->deepCopy());
        }
        else {
            n->_map[i->first] = i->second;
        }
    }
    return n;
}

/** Get the number of names in the PropertySet, optionally including those in
  * subproperties.  The name of the subproperty is counted in that case.
  * @param[in] topLevelOnly true (default) = don't include subproperties.
  * @return Number of names.
  */
size_t dafBase::PropertySet::nameCount(bool topLevelOnly) const {
    int n = 0;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        ++n;
        if (!topLevelOnly && i->second.type() == typeid(Ptr)) {
            n += i->second.getPtr()->nameCount(false);
        }
    }
    return n;
}

/** Get the names in the PropertySet, optionally including those in
  * subproperties.  The name of the subproperty is included in that case.
  * Hierarchical subproperty names are separated by a period.
  * @param[in] topLevelOnly true (default) = don't include subproperties.
  * @return Vector of names.
  */
vector<string> dafBase::PropertySet::names(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        v.push_back(i->first);
        if (!topLevelOnly && i->second.type() == typeid(Ptr)) {
            Ptr p = i->second.getPtr();
            vector<string> w = p->names(false);
            for (vector<string>::const_iterator k = w.begin();
                 k != w.end(); ++k) {
                v.push_back(i->first + "." + *k);
            }
        }
    }
    return v;
}

/** Get the names of parameters (non-subproperties) in the PropertySet,
  * optionally including those in subproperties.  Hierarchical subproperty
  * names are separated by a period.
  * @param[in] topLevelOnly true (default) = don't include subproperties.
  * @return Vector of parameter names.
  */
vector<string>
dafBase::PropertySet::paramNames(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second.type() == typeid(Ptr)) {
            Ptr p = i->second.getPtr();
            if (!topLevelOnly) {
                vector<string> w = p->paramNames(false);
                for (vector<string>::const_iterator k = w.begin();
                     k != w.end(); ++k) {
                    v.push_back(i->first + "." + *k);
                }
            }
        }
        else {
            v.push_back(i->first);
        }
    }
    return v;
}

/** Get the names of subproperties in the PropertySet, optionally including
  * those in subproperties.  Hierarchical subproperty names are separated by a
  * period.
  * @param[in] topLevelOnly true (default) = don't include subproperties.
  * @return Vector of subproperty names.
  */
vector<string>
dafBase::PropertySet::propertySetNames(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second.type() == typeid(Ptr)) {
            v.push_back(i->first);
            Ptr p = i->second.getPtr();
            if (!topLevelOnly) {
                vector<string> w = p->propertySetNames(false);
                for (vector<string>::const_iterator k = w.begin();
                     k != w.end(); ++k) {
                    v.push_back(i->first + "." + *k);
                }
            }
        }
    }
    return v;
}

/** Determine if a name (possibly hierarchical) exists.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return true if property exists.
  */
bool dafBase::PropertySet::exists(std::string const& name) const {
    return find(name) != _map.end();
}

/** Determine if a name (possibly hierarchical) has multiple values.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return true if property exists and has more than one value.
  */
bool dafBase::PropertySet::isArray(std::string const& name) const {
    AnyMap::const_iterator i = find(name);
    return i != _map.end() && i->second.size() > 1U;
}

/** Determine if a name (possibly hierarchical) is a subproperty.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return true if property exists and its values are PropertySet::Ptrs.
  */
bool dafBase::PropertySet::isPropertySetPtr(std::string const& name) const {
    AnyMap::const_iterator i = find(name);
    return i != _map.end() && i->second.type() == typeid(Ptr);
}

/** Get number of values for a property name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Number of values for that property.  0 if it doesn't exist.
  */
size_t dafBase::PropertySet::valueCount(std::string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) return 0;
    return i->second.size();
}

/** Get the type of values for a property name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Type of values for that property.
  * @throws NotFoundException Property does not exist.
  */
type_info const& dafBase::PropertySet::typeOf(std::string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    return i->second.type();
}

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
T dafBase::PropertySet::get(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    if (i->second.type() != typeid(T)) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    return i->second.getLast<T>();
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
T dafBase::PropertySet::get(string const& name, T const& defaultValue) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        return defaultValue;
    }
    if (i->second.type() != typeid(T)) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    return i->second.getLast<T>();
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
vector<T> const& dafBase::PropertySet::getArray(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    if (i->second.type() != typeid(T)) {
        throw LSST_EXCEPT(TypeMismatchException, name);
    }
    return i->second.getVec<T>();
}

// The following throw an exception if the conversion is inappropriate.

/** Get the last value for a bool property name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as a bool.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value is not a bool.
  */
bool dafBase::PropertySet::getAsBool(std::string const& name) const {
    return get<bool>(name);
}

/** Get the last value for a bool/char/short/int property name (possibly
  * hierarchical).  Unsigned int properties are not acceptable, but unsigned
  * versions of smaller types are.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as an int.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value cannot be converted to int.
  */
int dafBase::PropertySet::getAsInt(std::string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    return i->second.getAsInt(name);
}

/** Get the last value for a bool/char/short/int/int64_t property name
  * (possibly hierarchical).  Unsigned int64_t properties are not acceptable,
  * but unsigned versions of smaller types are, except possibly unsigned long,
  * depending on compiler.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as an int64_t.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value cannot be converted to int64_t.
  */
int64_t dafBase::PropertySet::getAsInt64(std::string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    return i->second.getAsInt64(name);
}

/** Get the last value for any arithmetic property name (possibly
  * hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as a double.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value cannot be converted to double.
  */
double dafBase::PropertySet::getAsDouble(std::string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundException, name + " not found");
    }
    return i->second.getAsDouble(name);
}

/** Get the last value for a string property name (possibly hierarchical).
  * Note that properties set with <tt>char const*</tt> can be retrieved as
  * strings using this method.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return String value.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value is not a string.
  */
std::string dafBase::PropertySet::getAsString(std::string const& name) const {
    return get<string>(name);
}

/** Get the last value for a subproperty name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return PropertySet::Ptr value.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value is not a PropertySet::Ptr.
  */
dafBase::PropertySet::Ptr
dafBase::PropertySet::getAsPropertySetPtr(std::string const& name) const {
    return get<Ptr>(name);
}

/** Get the last value for a Persistable name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Persistable::Ptr value.
  * @throws NotFoundException Property does not exist.
  * @throws TypeMismatchException Value is not a Persistable::Ptr.
  */
dafBase::Persistable::Ptr
dafBase::PropertySet::getAsPersistablePtr(std::string const& name) const {
    return get<Persistable::Ptr>(name);
}

/** Generate a string representation of the PropertySet.
  * Use this for debugging, not for serialization/persistence.
  * @param[in] topLevelOnly false (default) = do include subproperties.
  * @param[in] indent String to indent lines by (default none).
  * @return String representation of the PropertySet.
  */
std::string dafBase::PropertySet::toString(bool topLevelOnly,
                                           std::string const& indent) const {
    ostringstream s;
    vector<string> nv = names();
    sort(nv.begin(), nv.end());
    for (vector<string>::const_iterator i = nv.begin(); i != nv.end(); ++i) {
        AnyMap::const_iterator j = _map.find(*i);
        s << indent << j->first << " = ";
        if (j->second.type() == typeid(Ptr)) {
            if (topLevelOnly) {
                s << "{ ... }";
            }
            else {
                Ptr p = j->second.getPtr();
                s << '{' << endl;
                s << p->toString(false, indent + "..");
                s << indent << '}';
            }
        }
        else {
            j->second.toStream(s);
        }
        s << endl;
    }
    return s.str();
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
    vector<T> v;
    v.push_back(value);
    findOrInsert(name, detail::AnyVal(v));
}

template<>
void dafBase::PropertySet::set<dafBase::PropertySet::Ptr>(
    std::string const& name, PropertySet::Ptr const& value) {
    findOrInsert(name, detail::AnyVal(value));
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
    findOrInsert(name, detail::AnyVal(value));
}

/** Replace all values for a property name (possibly hierarchical) with a
  * string value.
  * @param[in] name Property name to set, possibly hierarchical.
  * @param[in] value Character string (converted to \c std::string ).
  */
void dafBase::PropertySet::set(std::string const& name, char const* value) {
    set(name, string(value));
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
    AnyMap::iterator i = find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second.type() != typeid(T)) {
            throw LSST_EXCEPT(TypeMismatchException,
                              name + " has mismatched type");
        }
        i->second.getVec<T>().push_back(value);
    }
}

template<> void dafBase::PropertySet::add<dafBase::PropertySet::Ptr>(
    std::string const& name, dafBase::PropertySet::Ptr const& value) {
    throw LSST_EXCEPT(TypeMismatchException,
                      "Cannot add() to PropertySet " + name);
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
    AnyMap::iterator i = find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second.type() != typeid(T)) {
            throw LSST_EXCEPT(TypeMismatchException,
                              name + " has mismatched type");
        }
        std::vector<T>& v(i->second.getVec<T>());
        v.insert(v.end(), value.begin(), value.end());
    }
}

/** Appends a <tt>char const*</tt> value to the vector of values for a
  * property name (possibly hierarchical).  Sets the value if the property
  * does not exist.
  * @param[in] name Property name to append to, possibly hierarchical.
  * @param[in] value Character string value to append.
  * @throws TypeMismatchException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
void dafBase::PropertySet::add(std::string const& name, char const* value) {
   add(name, string(value));
}

/** Appends all value vectors from the \a source to their corresponding
  * properties.  Sets values if a property does not exist.
  * @param[in] source PropertySet::Ptr for the source PropertySet.
  * @throws TypeMismatchException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  * @note
  * May only partially combine the PropertySets if an exception occurs.
  */
void dafBase::PropertySet::combine(Ptr const source) {
    if (source.get() == 0) {
        return;
    }
    vector<string> names = source->paramNames(false);
    for (vector<string>::const_iterator i = names.begin();
         i != names.end(); ++i) {
        AnyMap::const_iterator sj = source->find(*i);
        AnyMap::iterator dj = find(*i);
        if (dj == _map.end()) {
            findOrInsert(*i, sj->second);
        }
        else {
            if (sj->second.type() != dj->second.type()) {
                throw LSST_EXCEPT(TypeMismatchException,
                                  *i + " has mismatched type");
            }
            // Check for cycles
            if (sj->second.type() == typeid(Ptr)) {
                cycleCheck(sj->second.getPtr(), *i);
            }
            dj->second.combine(sj->second);
        }
    }
}

/** Removes all values for a property name (possibly hierarchical).  Does
  * nothing if the property does not exist.
  * @param[in] name Property name to remove, possibly hierarchical.
  */
void dafBase::PropertySet::remove(std::string const& name) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        _map.erase(name);
        return;
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second.type() != typeid(Ptr)) {
        return;
    }
    string suffix(name, i + 1);
    j->second.getPtr()->remove(suffix);
}

///////////////////////////////////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////////////////////////////////

/** Finds the property name (possibly hierarchical).
  * @param[in] name Property name to find, possibly hierarchical.
  * @return unordered_map::iterator to the property or end() if nonexistent.
  */
dafBase::PropertySet::AnyMap::iterator
dafBase::PropertySet::find(std::string const& name) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second.type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = j->second.getPtr();
    string suffix(name, i + 1);
    AnyMap::iterator x = p->find(suffix);
    if (x == p->_map.end()) {
        return _map.end();
    }
    return x;
}

/** Finds the property name (possibly hierarchical).  Const version.
  * @param[in] name Property name to find, possibly hierarchical.
  * @return unordered_map::const_iterator to the property or end().
  */
dafBase::PropertySet::AnyMap::const_iterator
dafBase::PropertySet::find(std::string const& name) const {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::const_iterator j = _map.find(prefix);
    if (j == _map.end() || j->second.type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = j->second.getPtr();
    string suffix(name, i + 1);
    AnyMap::const_iterator x = p->find(suffix);
    if (x == p->_map.end()) {
        return _map.end();
    }
    return x;
}

/** Finds the property name (possibly hierarchical) and sets or replaces its
  * value with the given vector of values.
  * @param[in] name Property name to find, possibly hierarchical.
  * @param[in] vp shared_ptr to vector of values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
void dafBase::PropertySet::findOrInsert(std::string const& name,
                                        detail::AnyVal const& any) {
    // Check for cycles
    if (any.type() == typeid(Ptr)) {
        cycleCheck(any.getPtr(), name);
    }

    string::size_type i = name.find('.');
    if (i == name.npos) {
        _map[name] = any;
        return;
    }
    string prefix(name, 0, i);
    string suffix(name, i + 1);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end()) {
        PropertySet::Ptr pp(new PropertySet);
        pp->findOrInsert(suffix, any);
        _map[prefix] = detail::AnyVal(pp);
        return;
    }
    else if (j->second.type() != typeid(Ptr)) {
        throw LSST_EXCEPT(pexExcept::InvalidParameterException,
                          prefix +
                          " exists but does not contain PropertySet::Ptrs");
    }
    j->second.getPtr()->findOrInsert(suffix, any);
}

void dafBase::PropertySet::cycleCheck(Ptr const& p,
                                      std::string const& name) {
    if (p.get() == this) {
        throw LSST_EXCEPT(pexExcept::InvalidParameterException,
                          name + " would cause a cycle");
    }
    vector<string> sets = p->propertySetNames(false);
    for (vector<string>::const_iterator i = sets.begin();
         i != sets.end(); ++i) {
        if (p->getAsPropertySetPtr(*i).get() == this) {
            throw LSST_EXCEPT(pexExcept::InvalidParameterException,
                              name + " would cause a cycle");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Explicit template instantiations
///////////////////////////////////////////////////////////////////////////////

/// @cond
// Explicit template instantiations are not well understood by doxygen.

#define INSTANTIATE(t) \
    template t dafBase::PropertySet::get<t>(string const& name) const; \
    template t dafBase::PropertySet::get<t>(string const& name, t const& defaultValue) const; \
    template vector<t> const& dafBase::PropertySet::getArray<t>(string const& name) const; \
    template void dafBase::PropertySet::set<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::set<t>(string const& name, vector<t> const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, vector<t> const& value);

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
INSTANTIATE(dafBase::Persistable::Ptr)
INSTANTIATE(dafBase::DateTime)

/// @endcond

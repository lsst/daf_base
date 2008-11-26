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

namespace dafBase = lsst::daf::base;

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
        if (i->second->back().type() == typeid(Ptr)) {
            for (vector<boost::any>::const_iterator j =
                 i->second->begin(); j != i->second->end(); ++j) {
                n->add(i->first, boost::any_cast<Ptr>(*j)->deepCopy());
            }
        }
        else {
            boost::shared_ptr< vector<boost::any> > vp(
                new vector<boost::any>(*(i->second)));
            n->_map[i->first] = vp;
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
        if (!topLevelOnly && i->second->back().type() == typeid(Ptr)) {
            n += boost::any_cast<Ptr>(i->second->back())->nameCount(false);
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
        if (!topLevelOnly && i->second->back().type() == typeid(Ptr)) {
            vector<string> w =
                boost::any_cast<Ptr>(i->second->back())->names(false);
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
        if (i->second->back().type() == typeid(Ptr)) {
            if (!topLevelOnly) {
                vector<string> w =
                    boost::any_cast<Ptr>(i->second->back())->paramNames(false);
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
        if (i->second->back().type() == typeid(Ptr)) {
            v.push_back(i->first);
            if (!topLevelOnly) {
                vector<string> w = boost::any_cast<Ptr>(i->second->back())->
                    propertySetNames(false);
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
bool dafBase::PropertySet::exists(string const& name) const {
    return find(name) != _map.end();
}

/** Determine if a name (possibly hierarchical) has multiple values.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return true if property exists and has more than one value.
  */
bool dafBase::PropertySet::isArray(string const& name) const {
    AnyMap::const_iterator i = find(name);
    return i != _map.end() && i->second->size() > 1U;
}

/** Determine if a name (possibly hierarchical) is a subproperty.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return true if property exists and its values are PropertySet::Ptrs.
  */
bool dafBase::PropertySet::isPropertySetPtr(string const& name) const {
    AnyMap::const_iterator i = find(name);
    return i != _map.end() && i->second->back().type() == typeid(Ptr);
}

/** Get number of values for a property name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Number of values for that property.  0 if it doesn't exist.
  */
size_t dafBase::PropertySet::valueCount(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) return 0;
    return i->second->size();
}

/** Get the type of values for a property name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Type of values for that property.
  * @throws NotFoundException Property does not exist.
  */
type_info const& dafBase::PropertySet::typeOf(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    return i->second->back().type();
}

// The following throw an exception if the type does not match exactly.

/** Get the last value for a property name (possibly hierarchical).
  * Note that the type must be explicitly specified for this template:
  * @code int i = propertySet.get<int>("foo") @endcode
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Last value set or added.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value does not match desired type.
  */
template <typename T>
T dafBase::PropertySet::get(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    return boost::any_cast<T>(i->second->back());
}

/** Get the last value for a property name (possibly hierarchical).
  * Returns the provided @a defaultValue if the property does not exist.
  * Note that the type must be explicitly specified for this template:
  * @code int i = propertySet.get<int>("foo", 42) @endcode
  * @param[in] name Property name to examine, possibly hierarchical.
  * @param[in] defaultValue Default value to return if property does not exist.
  * @return Last value set or added.
  * @throws boost::bad_any_cast Value does not match desired type.
  */
template <typename T>
T dafBase::PropertySet::get(string const& name, T const& defaultValue) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        return defaultValue;
    }
    return boost::any_cast<T>(i->second->back());
}

/** Get the vector of values for a property name (possibly hierarchical).
  * Note that the type must be explicitly specified for this template:
  * @code vector<int> v = propertySet.getArray<int>("foo") @endcode
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Vector of values.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value does not match desired type.
  */
template <typename T>
vector<T> dafBase::PropertySet::getArray(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    vector<T> v;
    for (vector<boost::any>::const_iterator j = i->second->begin();
         j != i->second->end(); ++j) {
        v.push_back(boost::any_cast<T>(*j));
    }
    return v;
}

// The following throw an exception if the conversion is inappropriate.

/** Get the last value for a bool property name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as a bool.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value is not a bool.
  */
bool dafBase::PropertySet::getAsBool(string const& name) const {
    return get<bool>(name);
}

/** Get the last value for a bool/char/short/int property name (possibly
  * hierarchical).  Unsigned int properties are not acceptable, but unsigned
  * versions of smaller types are.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as an int.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value cannot be converted to int.
  */
int dafBase::PropertySet::getAsInt(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) return boost::any_cast<bool>(v);
    if (t == typeid(char)) return boost::any_cast<char>(v);
    if (t == typeid(signed char)) return boost::any_cast<signed char>(v);
    if (t == typeid(unsigned char)) return boost::any_cast<unsigned char>(v);
    if (t == typeid(short)) return boost::any_cast<short>(v);
    if (t == typeid(unsigned short)) return boost::any_cast<unsigned short>(v);
    return boost::any_cast<int>(v);
}

/** Get the last value for a bool/char/short/int/int64_t property name
  * (possibly hierarchical).  Unsigned int properties are not acceptable, but
  * unsigned versions of smaller types are, except possibly unsigned long,
  * depending on compiler.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as an int64_t.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value cannot be converted to int64_t.
  */
int64_t dafBase::PropertySet::getAsInt64(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
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
    return boost::any_cast<int64_t>(v);
}

/** Get the last value for any arithmetic property name (possibly
  * hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Value as a double.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value cannot be converted to double.
  */
double dafBase::PropertySet::getAsDouble(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
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
    return boost::any_cast<double>(v);
}

/** Get the last value for a string property name (possibly hierarchical).
  * Note that properties set with <tt>char const*</tt> can be retrieved as
  * strings using this method.
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return String value.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value is not a string.
  */
string dafBase::PropertySet::getAsString(string const& name) const {
    return get<string>(name);
}

/** Get the last value for a subproperty name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return PropertySet::Ptr value.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value is not a PropertySet::Ptr.
  */
dafBase::PropertySet::Ptr
dafBase::PropertySet::getAsPropertySetPtr(string const& name) const {
    return get<Ptr>(name);
}

/** Get the last value for a Persistable name (possibly hierarchical).
  * @param[in] name Property name to examine, possibly hierarchical.
  * @return Persistable::Ptr value.
  * @throws NotFoundException Property does not exist.
  * @throws boost::bad_any_cast Value is not a Persistable::Ptr.
  */
dafBase::Persistable::Ptr
dafBase::PropertySet::getAsPersistablePtr(string const& name) const {
    return get<Persistable::Ptr>(name);
}

/** Generate a string representation of the PropertySet.
  * Use this for debugging, not for serialization/persistence.
  * @param[in] topLevelOnly false (default) = do include subproperties.
  * @param[in] indent String to indent lines by (default none).
  * @return String representation of the PropertySet.
  */
string dafBase::PropertySet::toString(bool topLevelOnly,
                                      string const& indent) const {
    ostringstream s;
    vector<string> nv = names();
    sort(nv.begin(), nv.end());
    for (vector<string>::const_iterator i = nv.begin(); i != nv.end(); ++i) {
        AnyMap::const_iterator j = _map.find(*i);
        s << indent << j->first << " = ";
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
            else if (t == typeid(Ptr) && !topLevelOnly) {
                s << '{' << std::endl;
                s << boost::any_cast<Ptr>(*k)->toString(false, indent + "..");
                s << indent << '}';
            }
            else if (t == typeid(Persistable::Ptr)) s << "<Persistable>";
            else s << "<Unknown>";
        }
        if (j->second->size() > 1) s << " ]";
        s << std::endl;
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
void dafBase::PropertySet::set(string const& name, T const& value) {
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->push_back(value);
    findOrInsert(name, vp);
}

/** Replace all values for a property name (possibly hierarchical) with a
  * vector of new values.
  * @param[in] name Property name to set, possibly hierarchical.
  * @param[in] value Vector of values to set.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
template <typename T>
void dafBase::PropertySet::set(string const& name, vector<T> const& value) {
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->insert(vp->end(), value.begin(), value.end());
    findOrInsert(name, vp);
}

/** Replace all values for a property name (possibly hierarchical) with a
  * string value.
  * @param[in] name Property name to set, possibly hierarchical.
  * @param[in] value Character string (converted to \c std::string ).
  */
void dafBase::PropertySet::set(string const& name, char const* value) {
    set(name, string(value));
}

/** Appends a single value to the vector of values for a property name
  * (possibly hierarchical).  Sets the value if the property does not exist.
  * @param[in] name Property name to append to, possibly hierarchical.
  * @param[in] value Value to append.
  * @throws DomainErrorException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
template <typename T>
void dafBase::PropertySet::add(string const& name, T const& value) {
    AnyMap::iterator i = find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw runtime_error(name + " mismatched type");
        }
        i->second->push_back(value);
    }
}

/** Appends a vector of values to the vector of values for a property name
  * (possibly hierarchical).  Sets the values if the property does not exist.
  * @param[in] name Property name to append to, possibly hierarchical.
  * @param[in] value Vector of values to append.
  * @throws DomainErrorException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  * @note
  * May only partially add the vector if an exception occurs.
  */
template <typename T>
void dafBase::PropertySet::add(string const& name, vector<T> const& value) {
    AnyMap::iterator i = find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw runtime_error(name + " mismatched type");
        }
        i->second->insert(i->second->end(), value.begin(), value.end());
    }
}

/** Appends a <tt>char const*</tt> value to the vector of values for a
  * property name (possibly hierarchical).  Sets the value if the property
  * does not exist.
  * @param[in] name Property name to append to, possibly hierarchical.
  * @param[in] value Character string value to append.
  * @throws DomainErrorException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
void dafBase::PropertySet::add(string const& name, char const* value) {
   add(name, string(value));
}

/** Appends all value vectors from the \a source to their corresponding
  * properties.  Sets values if a property does not exist.
  * @param[in] source PropertySet::Ptr for the source PropertySet.
  * @throws DomainErrorException Type does not match existing values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  * @note
  * May only partially combine the PropertySets if an exception occurs.
  */
void dafBase::PropertySet::combine(Ptr const source) {
    vector<string> names = source->paramNames(false);
    for (vector<string>::const_iterator i = names.begin();
         i != names.end(); ++i) {
        AnyMap::const_iterator sj = source->find(*i);
        AnyMap::const_iterator dj = find(*i);
        if (dj == _map.end()) {
            boost::shared_ptr< vector<boost::any> > vp(
                new vector<boost::any>(*(sj->second)));
            findOrInsert(*i, vp);
        }
        else {
            if (sj->second->back().type() != dj->second->back().type()) {
                throw runtime_error(*i + " mismatched type");
            }
            dj->second->insert(dj->second->end(),
                               sj->second->begin(), sj->second->end());
        }
    }
}

/** Removes all values for a property name (possibly hierarchical).  Does
  * nothing if the property does not exist.
  * @param[in] name Property name to remove, possibly hierarchical.
  */
void dafBase::PropertySet::remove(string const& name) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        _map.erase(name);
        return;
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return;
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    string suffix(name, i + 1);
    p->remove(suffix);
}

///////////////////////////////////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////////////////////////////////

/** Finds the property name (possibly hierarchical).
  * @param[in] name Property name to find, possibly hierarchical.
  * @return unordered_map::iterator to the property or end() if nonexistent.
  * @note
  * Assumes that end() is the same for all unordered_maps.
  */
dafBase::PropertySet::AnyMap::iterator
dafBase::PropertySet::find(string const& name) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    string suffix(name, i + 1);
    return p->find(suffix);
}

/** Finds the property name (possibly hierarchical).  Const version.
  * @param[in] name Property name to find, possibly hierarchical.
  * @return unordered_map::const_iterator to the property or end().
  * @note
  * Assumes that end() is the same for all unordered_maps.
  */
dafBase::PropertySet::AnyMap::const_iterator
dafBase::PropertySet::find(string const& name) const {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::const_iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    string suffix(name, i + 1);
    return p->find(suffix);
}

/** Finds the property name (possibly hierarchical) and sets or replaces its
  * value with the given vector of values.
  * @param[in] name Property name to find, possibly hierarchical.
  * @param[in] vp shared_ptr to vector of values.
  * @throws InvalidParameterException Hierarchical name uses non-PropertySet.
  */
void dafBase::PropertySet::findOrInsert(
    string const& name, boost::shared_ptr< vector<boost::any> > vp) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        _map[name] = vp;
        return;
    }
    string prefix(name, 0, i);
    string suffix(name, i + 1);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end()) {
        PropertySet::Ptr pp(new PropertySet);
        pp->findOrInsert(suffix, vp);
        boost::shared_ptr< vector<boost::any> > temp(new vector<boost::any>);
        temp->push_back(pp);
        _map[prefix] = temp;
        return;
    }
    else if (j->second->back().type() != typeid(Ptr)) {
        throw runtime_error(prefix + " exists and is not PropertySet::Ptr");
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    p->findOrInsert(suffix, vp);
}

///////////////////////////////////////////////////////////////////////////////
// Explicit template instantiations
///////////////////////////////////////////////////////////////////////////////

/// @cond
// Explicit template instantiations are not well understood by doxygen.

#define INSTANTIATE(t) \
    template t dafBase::PropertySet::get<t>(string const& name) const; \
    template t dafBase::PropertySet::get<t>(string const& name, t const& defaultValue) const; \
    template vector<t> dafBase::PropertySet::getArray<t>(string const& name) const; \
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
INSTANTIATE(dafBase::PropertySet::Ptr)
INSTANTIATE(dafBase::Persistable::Ptr)

/// @endcond

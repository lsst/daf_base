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
 * GNU General Public License for more detailTypeErrors.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 

#include "lsst/daf/base/PropertySet.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/pex/exceptions/Runtime.h"
#include "lsst/daf/base/DateTime.h"

namespace dafBase = lsst::daf::base;
namespace pexExcept = lsst::pex::exceptions;

using namespace std;

namespace {

/**
 * Append the contents of a vector<T> to a vector<boost::any>
 *
 * This method exists because vector<boost::any>.insert mis-behaves for vector of bool,
 * resulting in a vector with elements that are a strange type.
 */
template <typename T>
void _append(std::vector<boost::any>& dest, std::vector<T> const& src) {
    dest.reserve(dest.size() + src.size());
    for (const T& val : src) {
        dest.push_back(static_cast<T>(val));
    }
}

}  // namespace

dafBase::PropertySet::PropertySet(bool flat) : Citizen(typeid(*this)), _flat(flat) {
}

dafBase::PropertySet::~PropertySet(void) {
}

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

dafBase::PropertySet::Ptr dafBase::PropertySet::deepCopy(void) const {
    Ptr n(new PropertySet(_flat));
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second->back().type() == typeid(Ptr)) {
            for (vector<boost::any>::const_iterator j =
                 i->second->begin(); j != i->second->end(); ++j) {
                Ptr p = boost::any_cast<Ptr>(*j);
                if (p.get() == 0) {
                    n->add(i->first, Ptr());
                } else {
                    n->add(i->first, p->deepCopy());
                }
            }
        } else {
            std::shared_ptr< vector<boost::any> > vp(
                new vector<boost::any>(*(i->second)));
            n->_map[i->first] = vp;
        }
    }
    return n;
}

size_t dafBase::PropertySet::nameCount(bool topLevelOnly) const {
    int n = 0;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        ++n;
        if (!topLevelOnly && i->second->back().type() == typeid(Ptr)) {
            Ptr p = boost::any_cast<Ptr>(i->second->back());
            if (p.get() != 0) {
                n += p->nameCount(false);
            }
        }
    }
    return n;
}

vector<string> dafBase::PropertySet::names(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        v.push_back(i->first);
        if (!topLevelOnly && i->second->back().type() == typeid(Ptr)) {
            Ptr p = boost::any_cast<Ptr>(i->second->back());
            if (p.get() != 0) {
                vector<string> w = p->names(false);
                for (vector<string>::const_iterator k = w.begin();
                     k != w.end(); ++k) {
                    v.push_back(i->first + "." + *k);
                }
            }
        }
    }
    return v;
}

vector<string>
dafBase::PropertySet::paramNames(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second->back().type() == typeid(Ptr)) {
            Ptr p = boost::any_cast<Ptr>(i->second->back());
            if (p.get() != 0 && !topLevelOnly) {
                vector<string> w = p->paramNames(false);
                for (vector<string>::const_iterator k = w.begin();
                     k != w.end(); ++k) {
                    v.push_back(i->first + "." + *k);
                }
            }
        } else {
            v.push_back(i->first);
        }
    }
    return v;
}

vector<string>
dafBase::PropertySet::propertySetNames(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second->back().type() == typeid(Ptr)) {
            v.push_back(i->first);
            Ptr p = boost::any_cast<Ptr>(i->second->back());
            if (p.get() != 0 && !topLevelOnly) {
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

bool dafBase::PropertySet::exists(std::string const& name) const {
    return _find(name) != _map.end();
}

bool dafBase::PropertySet::isArray(std::string const& name) const {
    AnyMap::const_iterator i = _find(name);
    return i != _map.end() && i->second->size() > 1U;
}

bool dafBase::PropertySet::isPropertySetPtr(std::string const& name) const {
    AnyMap::const_iterator i = _find(name);
    return i != _map.end() && i->second->back().type() == typeid(Ptr);
}

size_t dafBase::PropertySet::valueCount(std::string const& name) const {
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) return 0;
    return i->second->size();
}

type_info const& dafBase::PropertySet::typeOf(std::string const& name) const {
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundError, name + " not found");
    }
    return i->second->back().type();
}

// The following throw an exception if the type does not match exactly.

template <typename T>
T dafBase::PropertySet::get(string const& name) const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundError, name + " not found");
    }
    try {
        return boost::any_cast<T>(i->second->back());
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pexExcept::TypeError, name);
    }
    // not reached
    return boost::any_cast<T>(i->second->back());
}

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
        throw LSST_EXCEPT(pexExcept::TypeError, name);
    }
    // not reached
    return boost::any_cast<T>(i->second->back());
}

template <typename T>
vector<T> dafBase::PropertySet::getArray(string const& name) const {
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundError, name + " not found");
    }
    vector<T> v;
    for (vector<boost::any>::const_iterator j = i->second->begin();
         j != i->second->end(); ++j) {
        try {
            v.push_back(boost::any_cast<T>(*j));
        }
        catch (boost::bad_any_cast) {
            throw LSST_EXCEPT(pexExcept::TypeError, name);
        }
    }
    return v;
}

// The following throw an exception if the conversion is inappropriate.

bool dafBase::PropertySet::getAsBool(std::string const& name) const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "for symmetry with other types" */
    return get<bool>(name);
}

int dafBase::PropertySet::getAsInt(std::string const& name) const {
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundError, name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) {
        return boost::any_cast<bool>(v);
    } else if (t == typeid(char)) {
        return boost::any_cast<char>(v);
    } else if (t == typeid(signed char)) {
        return boost::any_cast<signed char>(v);
    } else if (t == typeid(unsigned char)) {
        return boost::any_cast<unsigned char>(v);
    } else if (t == typeid(short)) {
        return boost::any_cast<short>(v);
    } else if (t == typeid(unsigned short)) {
        return boost::any_cast<unsigned short>(v);
    }
    try {
        return boost::any_cast<int>(v);
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pexExcept::TypeError, name);
    }
    // not reached
    return boost::any_cast<int>(v);
}

int64_t dafBase::PropertySet::getAsInt64(std::string const& name) const {
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundError, name + " not found");
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
    if (t == typeid(long long)) return boost::any_cast<long long>(v);
    try {
        return boost::any_cast<int64_t>(v);
    }
    catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pexExcept::TypeError, name);
    }
    // not reached
    return boost::any_cast<int64_t>(v);
}

double dafBase::PropertySet::getAsDouble(std::string const& name) const {
    AnyMap::const_iterator i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pexExcept::NotFoundError, name + " not found");
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
        throw LSST_EXCEPT(pexExcept::TypeError, name);
    }
    // not reached
    return boost::any_cast<double>(v);
}

std::string dafBase::PropertySet::getAsString(std::string const& name) const {
    return get<string>(name);
}

dafBase::PropertySet::Ptr
dafBase::PropertySet::getAsPropertySetPtr(std::string const& name) const {
    return get<Ptr>(name);
}

dafBase::Persistable::Ptr
dafBase::PropertySet::getAsPersistablePtr(std::string const& name) const {
    return get<Persistable::Ptr>(name);
}

std::string dafBase::PropertySet::toString(bool topLevelOnly,
                                           std::string const& indent) const {
    ostringstream s;
    vector<string> nv = names();
    sort(nv.begin(), nv.end());
    for (vector<string>::const_iterator i = nv.begin(); i != nv.end(); ++i) {
        std::shared_ptr< vector<boost::any> > vp = _map.find(*i)->second;
        type_info const& t = vp->back().type();
        if (t == typeid(Ptr)) {
            s << indent << *i << " = ";
            if (topLevelOnly) {
                s << "{ ... }";
            } else {
                Ptr p = boost::any_cast<Ptr>(vp->back());
                if (p.get() == 0) {
                    s << "{ NULL }";
                } else {
                    s << '{' << endl;
                    s << p->toString(false, indent + "..");
                    s << indent << '}';
                }
            }
            s << endl;
        }
        else {
            s << indent << _format(*i);
        }
    }
    return s.str();
}

std::string dafBase::PropertySet::_format(std::string const& name) const {
    ostringstream s;
    s << std::showpoint; // Always show a decimal point for floats
    AnyMap::const_iterator j = _map.find(name);
    s << j->first << " = ";
    std::shared_ptr< vector<boost::any> > vp = j->second;
    if (vp->size() > 1) {
        s << "[ ";
    }
    type_info const& t = vp->back().type();
    for (vector<boost::any>::const_iterator k = vp->begin();
         k != vp->end(); ++k) {
        if (k != vp->begin()) {
            s << ", ";
        }
        boost::any const& v(*k);
        if (t == typeid(bool)) {
            s << boost::any_cast<bool>(v);
        } else if (t == typeid(char)) {
            s << '\'' << boost::any_cast<char>(v) << '\'';
        } else if (t == typeid(signed char)) {
            s << '\'' << boost::any_cast<signed char>(v) << '\'';
        } else if (t == typeid(unsigned char)) {
            s << '\'' << boost::any_cast<unsigned char>(v) << '\'';
        } else if (t == typeid(short)) {
            s << boost::any_cast<short>(v);
        } else if (t == typeid(unsigned short)) {
            s << boost::any_cast<unsigned short>(v);
        } else if (t == typeid(int)) {
            s << boost::any_cast<int>(v);
        } else if (t == typeid(unsigned int)) {
            s << boost::any_cast<unsigned int>(v);
        } else if (t == typeid(long)) {
            s << boost::any_cast<long>(v);
        } else if (t == typeid(unsigned long)) {
            s << boost::any_cast<unsigned long>(v);
        } else if (t == typeid(long long)) {
            s << boost::any_cast<long long>(v);
        } else if (t == typeid(unsigned long long)) {
            s << boost::any_cast<unsigned long long>(v);
        } else if (t == typeid(float)) {
            s << std::setprecision(7) << boost::any_cast<float>(v);
        } else if (t == typeid(double)) {
            s << std::setprecision(14) << boost::any_cast<double>(v);
        } else if (t == typeid(string)) {
            s << '"' << boost::any_cast<string>(v) << '"';
        } else if (t == typeid(dafBase::DateTime)) {
            s << boost::any_cast<dafBase::DateTime>(v).toString(dafBase::DateTime::UTC);
        } else if (t == typeid(Ptr)) {
            s << "{ ... }";
        } else if (t == typeid(Persistable::Ptr)) {
            s << "<Persistable>";
        } else {
            s << "<Unknown>";
        }
    }
    if (j->second->size() > 1) {
        s << " ]";
    }
    s << endl;
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////
// Modifiers
///////////////////////////////////////////////////////////////////////////////

template <typename T>
void dafBase::PropertySet::set(std::string const& name, T const& value) {
    std::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->push_back(value);
    _set(name, vp);
}

template <typename T>
void dafBase::PropertySet::set(std::string const& name,
                               vector<T> const& value) {
    if (value.empty()) return;
    std::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    _append(*vp, value);
    _set(name, vp);
}

void dafBase::PropertySet::set(std::string const& name, char const* value) {
    set(name, string(value));
}

template <typename T>
void dafBase::PropertySet::add(std::string const& name, T const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(pexExcept::TypeError,
                              name + " has mismatched type");
        }
        i->second->push_back(value);
    }
}

// Specialize for Ptrs to check for cycles.
template <> void dafBase::PropertySet::add<dafBase::PropertySet::Ptr>(
    std::string const& name, Ptr const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(Ptr)) {
            throw LSST_EXCEPT(pexExcept::TypeError,
                              name + " has mismatched type");
        }
        _cycleCheckPtr(value, name);
        i->second->push_back(value);
    }
}

template <typename T>
void dafBase::PropertySet::add(std::string const& name,
                               vector<T> const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(pexExcept::TypeError,
                              name + " has mismatched type");
        }
        _append(*(i->second), value);
    }
}

// Specialize for Ptrs to check for cycles.
template<> void dafBase::PropertySet::add<dafBase::PropertySet::Ptr>(
    std::string const& name, vector<Ptr> const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->back().type() != typeid(Ptr)) {
            throw LSST_EXCEPT(pexExcept::TypeError,
                              name + " has mismatched type");
        }
        _cycleCheckPtrVec(value, name);
        _append(*(i->second), value);
    }
}

void dafBase::PropertySet::add(std::string const& name, char const* value) {
    add(name, string(value));
}

void dafBase::PropertySet::copy(std::string const& dest,
                                ConstPtr source, std::string const& name,
                                bool asScalar) {
    if (source.get() == 0) {
        throw LSST_EXCEPT(pexExcept::InvalidParameterError,
                          "Missing source");
    }
    AnyMap::const_iterator sj = source->_find(name);
    if (sj == source->_map.end()) {
        throw LSST_EXCEPT(pexExcept::InvalidParameterError,
                          name + " not in source");
    }
    remove(dest);
    if (asScalar) {
        auto vp = std::make_shared<vector<boost::any>>();
        vp->push_back(sj->second->back());
        _set(dest, vp);
    } else {
        auto vp = std::make_shared<vector<boost::any>>(*(sj->second));
        _set(dest, vp);
    }
}

void dafBase::PropertySet::combine(ConstPtr source) {
    if (source.get() == 0) {
        return;
    }
    vector<string> names = source->paramNames(false);
    for (vector<string>::const_iterator i = names.begin();
         i != names.end(); ++i) {
        AnyMap::const_iterator sp = source->_find(*i);
        _add(*i, sp->second);
    }
}

void dafBase::PropertySet::remove(std::string const& name) {
    string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        _map.erase(name);
        return;
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return;
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() != 0) {
        string suffix(name, i + 1);
        p->remove(suffix);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////////////////////////////////

dafBase::PropertySet::AnyMap::iterator
dafBase::PropertySet::_find(std::string const& name) {
    string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() == 0) {
        return _map.end();
    }
    string suffix(name, i + 1);
    AnyMap::iterator x = p->_find(suffix);
    if (x == p->_map.end()) {
        return _map.end();
    }
    return x;
}

dafBase::PropertySet::AnyMap::const_iterator
dafBase::PropertySet::_find(std::string const& name) const {
    string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::const_iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() == 0) {
        return _map.end();
    }
    string suffix(name, i + 1);
    AnyMap::const_iterator x = p->_find(suffix);
    if (x == p->_map.end()) {
        return _map.end();
    }
    return x;
}

void dafBase::PropertySet::_set(
    std::string const& name, std::shared_ptr< std::vector<boost::any> > vp) {
    _findOrInsert(name, vp);
}

void dafBase::PropertySet::_add(
    std::string const& name, std::shared_ptr< std::vector<boost::any> > vp) {

    AnyMap::const_iterator dp = _find(name);
    if (dp == _map.end()) {
        _set(name, vp);
    }
    else {
        if (vp->back().type() != dp->second->back().type()) {
            throw LSST_EXCEPT(pexExcept::TypeError,
                              name + " has mismatched type");
        }
        // Check for cycles
        if (vp->back().type() == typeid(Ptr)) {
            _cycleCheckAnyVec(*vp, name);
        }
        _append(*(dp->second), *vp);
    }
}

void dafBase::PropertySet::_findOrInsert(
    std::string const& name, std::shared_ptr< std::vector<boost::any> > vp) {
    if (vp->back().type() == typeid(Ptr)) {
        if (_flat) {
            Ptr source = boost::any_cast<Ptr>(vp->back());
            vector<string> names = source->paramNames(false);
            for (vector<string>::const_iterator i = names.begin();
                 i != names.end(); ++i) {
                AnyMap::const_iterator sp = source->_find(*i);
                _add(name + "." + *i, sp->second);
            }
            return;
        }

        // Check for cycles
        _cycleCheckAnyVec(*vp, name);
    }

    string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        _map[name] = vp;
        return;
    }
    string prefix(name, 0, i);
    string suffix(name, i + 1);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end()) {
        PropertySet::Ptr pp(new PropertySet);
        pp->_findOrInsert(suffix, vp);
        std::shared_ptr< vector<boost::any> > temp(new vector<boost::any>);
        temp->push_back(pp);
        _map[prefix] = temp;
        return;
    }
    else if (j->second->back().type() != typeid(Ptr)) {
        throw LSST_EXCEPT(pexExcept::InvalidParameterError,
                          prefix +
                          " exists but does not contain PropertySet::Ptrs");
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() == 0) {
        throw LSST_EXCEPT(pexExcept::InvalidParameterError,
                          prefix +
                          " exists but contains null PropertySet::Ptr");
    }
    p->_findOrInsert(suffix, vp);
}

void dafBase::PropertySet::_cycleCheckPtrVec(std::vector<Ptr> const& v,
                                         std::string const& name) {
    for (vector<Ptr>::const_iterator i = v.begin(); i != v.end(); ++i) {
        _cycleCheckPtr(*i, name);
    }
}

void dafBase::PropertySet::_cycleCheckAnyVec(std::vector<boost::any> const& v,
                                         std::string const& name) {
    for (vector<boost::any>::const_iterator i = v.begin(); i != v.end(); ++i) {
        _cycleCheckPtr(boost::any_cast<Ptr>(*i), name);
    }
}

void dafBase::PropertySet::_cycleCheckPtr(Ptr const& v,
                                         std::string const& name) {
    if (v.get() == this) {
        throw LSST_EXCEPT(pexExcept::InvalidParameterError,
                          name + " would cause a cycle");
    }
    vector<string> sets = v->propertySetNames(false);
    for (vector<string>::const_iterator i = sets.begin();
         i != sets.end(); ++i) {
        if (v->getAsPropertySetPtr(*i).get() == this) {
            throw LSST_EXCEPT(pexExcept::InvalidParameterError,
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
    template vector<t> dafBase::PropertySet::getArray<t>(string const& name) const; \
    template void dafBase::PropertySet::set<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::set<t>(string const& name, vector<t> const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, vector<t> const& value);

#define INSTANTIATE_PROPERTY_SET(t) \
    template t dafBase::PropertySet::get<t>(string const& name) const; \
    template t dafBase::PropertySet::get<t>(string const& name, t const& defaultValue) const; \
    template vector<t> dafBase::PropertySet::getArray<t>(string const& name) const; \
    template void dafBase::PropertySet::set<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::set<t>(string const& name, vector<t> const& value);

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
INSTANTIATE_PROPERTY_SET(dafBase::PropertySet::Ptr)
INSTANTIATE(dafBase::Persistable::Ptr)
INSTANTIATE(dafBase::DateTime)

/// @endcond

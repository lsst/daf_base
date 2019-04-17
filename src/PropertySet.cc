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
 * GNU General Public License for more errors.
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

namespace lsst {
namespace daf {
namespace base {

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

PropertySet::PropertySet(bool flat) : _flat(flat) {}

PropertySet::~PropertySet() noexcept = default;

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

PropertySet::Ptr PropertySet::deepCopy() const {
    Ptr n(new PropertySet(_flat));
    for (auto const& elt : _map) {
        if (elt.second->back().type() == typeid(Ptr)) {
            for (auto const& j : *elt.second) {
                Ptr p = boost::any_cast<Ptr>(j);
                if (p.get() == 0) {
                    n->add(elt.first, Ptr());
                } else {
                    n->add(elt.first, p->deepCopy());
                }
            }
        } else {
            std::shared_ptr<std::vector<boost::any>> vp(new std::vector<boost::any>(*(elt.second)));
            n->_map[elt.first] = vp;
        }
    }
    return n;
}

size_t PropertySet::nameCount(bool topLevelOnly) const {
    int n = 0;
    for (auto const& elt : _map) {
        ++n;
        if (!topLevelOnly && elt.second->back().type() == typeid(Ptr)) {
            Ptr p = boost::any_cast<Ptr>(elt.second->back());
            if (p.get() != 0) {
                n += p->nameCount(false);
            }
        }
    }
    return n;
}

std::vector<std::string> PropertySet::names(bool topLevelOnly) const {
    std::vector<std::string> v;
    for (auto const& elt : _map) {
        v.push_back(elt.first);
        if (!topLevelOnly && elt.second->back().type() == typeid(Ptr)) {
            Ptr p = boost::any_cast<Ptr>(elt.second->back());
            if (p.get() != 0) {
                std::vector<std::string> w = p->names(false);
                for (auto const& k : w) {
                    v.push_back(elt.first + "." + k);
                }
            }
        }
    }
    return v;
}

std::vector<std::string> PropertySet::paramNames(bool topLevelOnly) const {
    std::vector<std::string> v;
    for (auto const& elt : _map) {
        if (elt.second->back().type() == typeid(Ptr)) {
            Ptr p = boost::any_cast<Ptr>(elt.second->back());
            if (p.get() != 0 && !topLevelOnly) {
                std::vector<std::string> w = p->paramNames(false);
                for (auto const& k : w) {
                    v.push_back(elt.first + "." + k);
                }
            }
        } else {
            v.push_back(elt.first);
        }
    }
    return v;
}

std::vector<std::string> PropertySet::propertySetNames(bool topLevelOnly) const {
    std::vector<std::string> v;
    for (auto const& elt : _map) {
        if (elt.second->back().type() == typeid(Ptr)) {
            v.push_back(elt.first);
            Ptr p = boost::any_cast<Ptr>(elt.second->back());
            if (p.get() != 0 && !topLevelOnly) {
                std::vector<std::string> w = p->propertySetNames(false);
                for (auto const& k : w) {
                    v.push_back(elt.first + "." + k);
                }
            }
        }
    }
    return v;
}

bool PropertySet::exists(std::string const& name) const { return _find(name) != _map.end(); }

bool PropertySet::isArray(std::string const& name) const {
    auto const i = _find(name);
    return i != _map.end() && i->second->size() > 1U;
}

bool PropertySet::isPropertySetPtr(std::string const& name) const {
    auto const i = _find(name);
    return i != _map.end() && i->second->back().type() == typeid(Ptr);
}

bool PropertySet::isUndefined(std::string const& name) const {
    auto const i = _find(name);
    return i != _map.end() && i->second->back().type() == typeid(nullptr);
}

size_t PropertySet::valueCount(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) return 0;
    return i->second->size();
}

std::type_info const& PropertySet::typeOf(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    return i->second->back().type();
}

template <typename T>
std::type_info const& PropertySet::typeOfT() {
    return typeid(T);
}

// The following throw an exception if the type does not match exactly.

template <typename T>
T PropertySet::get(std::string const& name)
        const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    try {
        return boost::any_cast<T>(i->second->back());
    } catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pex::exceptions::TypeError, name);
    }
    // not reached
    return boost::any_cast<T>(i->second->back());
}

template <typename T>
T PropertySet::get(std::string const& name, T const& defaultValue)
        const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    auto const i = _find(name);
    if (i == _map.end()) {
        return defaultValue;
    }
    try {
        return boost::any_cast<T>(i->second->back());
    } catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pex::exceptions::TypeError, name);
    }
    // not reached
    return boost::any_cast<T>(i->second->back());
}

template <typename T>
std::vector<T> PropertySet::getArray(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    std::vector<T> v;
    for (auto const& j : *(i->second)) {
        try {
            v.push_back(boost::any_cast<T>(j));
        } catch (boost::bad_any_cast) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name);
        }
    }
    return v;
}

// The following throw an exception if the conversion is inappropriate.

bool PropertySet::getAsBool(std::string const& name)
        const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "for symmetry with other types" */
    return get<bool>(name);
}

int PropertySet::getAsInt(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    boost::any v = i->second->back();
    std::type_info const& t = v.type();
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
    } catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pex::exceptions::TypeError, name);
    }
    // not reached
    return boost::any_cast<int>(v);
}

int64_t PropertySet::getAsInt64(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    boost::any v = i->second->back();
    std::type_info const& t = v.type();
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
    } catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pex::exceptions::TypeError, name);
    }
    // not reached
    return boost::any_cast<int64_t>(v);
}

double PropertySet::getAsDouble(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    boost::any v = i->second->back();
    std::type_info const& t = v.type();
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
    } catch (boost::bad_any_cast) {
        throw LSST_EXCEPT(pex::exceptions::TypeError, name);
    }
    // not reached
    return boost::any_cast<double>(v);
}

std::string PropertySet::getAsString(std::string const& name) const { return get<std::string>(name); }

PropertySet::Ptr PropertySet::getAsPropertySetPtr(std::string const& name) const { return get<Ptr>(name); }

Persistable::Ptr PropertySet::getAsPersistablePtr(std::string const& name) const {
    return get<Persistable::Ptr>(name);
}

std::string PropertySet::toString(bool topLevelOnly, std::string const& indent) const {
    std::ostringstream s;
    std::vector<std::string> nv = names();
    sort(nv.begin(), nv.end());
    for (auto const& i : nv) {
        std::shared_ptr<std::vector<boost::any>> vp = _map.find(i)->second;
        std::type_info const& t = vp->back().type();
        if (t == typeid(Ptr)) {
            s << indent << i << " = ";
            if (topLevelOnly) {
                s << "{ ... }";
            } else {
                Ptr p = boost::any_cast<Ptr>(vp->back());
                if (p.get() == 0) {
                    s << "{ NULL }";
                } else {
                    s << '{' << std::endl;
                    s << p->toString(false, indent + "..");
                    s << indent << '}';
                }
            }
            s << std::endl;
        } else {
            s << indent << _format(i);
        }
    }
    return s.str();
}

std::string PropertySet::_format(std::string const& name) const {
    std::ostringstream s;
    s << std::showpoint;  // Always show a decimal point for floats
    auto const j = _map.find(name);
    s << j->first << " = ";
    std::shared_ptr<std::vector<boost::any>> vp = j->second;
    if (vp->size() > 1) {
        s << "[ ";
    }
    std::type_info const& t = vp->back().type();
    bool isFirst = true;
    for (auto const& k : *vp) {
        if (isFirst) {
            isFirst = false;
        } else {
            s << ", ";
        }
        boost::any const& v(k);
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
        } else if (t == typeid(std::string)) {
            s << '"' << boost::any_cast<std::string>(v) << '"';
        } else if (t == typeid(DateTime)) {
            s << boost::any_cast<DateTime>(v).toString(DateTime::UTC);
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
    s << std::endl;
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////
// Modifiers
///////////////////////////////////////////////////////////////////////////////

template <typename T>
void PropertySet::set(std::string const& name, T const& value) {
    std::shared_ptr<std::vector<boost::any>> vp(new std::vector<boost::any>);
    vp->push_back(value);
    _set(name, vp);
}

template <typename T>
void PropertySet::set(std::string const& name, std::vector<T> const& value) {
    if (value.empty()) return;
    std::shared_ptr<std::vector<boost::any>> vp(new std::vector<boost::any>);
    _append(*vp, value);
    _set(name, vp);
}

void PropertySet::set(std::string const& name, char const* value) { set(name, std::string(value)); }

template <typename T>
void PropertySet::add(std::string const& name, T const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    } else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name + " has mismatched type");
        }
        i->second->push_back(value);
    }
}

// Specialize for Ptrs to check for cycles.
template <>
void PropertySet::add<PropertySet::Ptr>(std::string const& name, Ptr const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    } else {
        if (i->second->back().type() != typeid(Ptr)) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name + " has mismatched type");
        }
        _cycleCheckPtr(value, name);
        i->second->push_back(value);
    }
}

template <typename T>
void PropertySet::add(std::string const& name, std::vector<T> const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    } else {
        if (i->second->back().type() != typeid(T)) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name + " has mismatched type");
        }
        _append(*(i->second), value);
    }
}

// Specialize for Ptrs to check for cycles.
template <>
void PropertySet::add<PropertySet::Ptr>(std::string const& name, std::vector<Ptr> const& value) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    } else {
        if (i->second->back().type() != typeid(Ptr)) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name + " has mismatched type");
        }
        _cycleCheckPtrVec(value, name);
        _append(*(i->second), value);
    }
}

void PropertySet::add(std::string const& name, char const* value) { add(name, std::string(value)); }

void PropertySet::copy(std::string const& dest, ConstPtr source, std::string const& name, bool asScalar) {
    if (source.get() == 0) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError, "Missing source");
    }
    auto const sj = source->_find(name);
    if (sj == source->_map.end()) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError, name + " not in source");
    }
    remove(dest);
    if (asScalar) {
        auto vp = std::make_shared<std::vector<boost::any>>();
        vp->push_back(sj->second->back());
        _set(dest, vp);
    } else {
        auto vp = std::make_shared<std::vector<boost::any>>(*(sj->second));
        _set(dest, vp);
    }
}

void PropertySet::combine(ConstPtr source) {
    if (source.get() == 0) {
        return;
    }
    std::vector<std::string> names = source->paramNames(false);
    for (auto const& name : names) {
        auto const sp = source->_find(name);
        _add(name, sp->second);
    }
}

void PropertySet::remove(std::string const& name) {
    std::string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        _map.erase(name);
        return;
    }
    std::string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return;
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() != 0) {
        std::string suffix(name, i + 1);
        p->remove(suffix);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////////////////////////////////

PropertySet::AnyMap::iterator PropertySet::_find(std::string const& name) {
    std::string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        return _map.find(name);
    }
    std::string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() == 0) {
        return _map.end();
    }
    std::string suffix(name, i + 1);
    AnyMap::iterator x = p->_find(suffix);
    if (x == p->_map.end()) {
        return _map.end();
    }
    return x;
}

PropertySet::AnyMap::const_iterator PropertySet::_find(std::string const& name) const {
    std::string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        return _map.find(name);
    }
    std::string prefix(name, 0, i);
    auto const j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() == 0) {
        return _map.end();
    }
    std::string suffix(name, i + 1);
    auto const x = p->_find(suffix);
    if (x == p->_map.end()) {
        return _map.end();
    }
    return x;
}

void PropertySet::_set(std::string const& name, std::shared_ptr<std::vector<boost::any>> vp) {
    _findOrInsert(name, vp);
}

void PropertySet::_add(std::string const& name, std::shared_ptr<std::vector<boost::any>> vp) {
    auto const dp = _find(name);
    if (dp == _map.end()) {
        _set(name, vp);
    } else {
        if (vp->back().type() != dp->second->back().type()) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name + " has mismatched type");
        }
        // Check for cycles
        if (vp->back().type() == typeid(Ptr)) {
            _cycleCheckAnyVec(*vp, name);
        }
        _append(*(dp->second), *vp);
    }
}

void PropertySet::_findOrInsert(std::string const& name, std::shared_ptr<std::vector<boost::any>> vp) {
    if (vp->back().type() == typeid(Ptr)) {
        if (_flat) {
            Ptr source = boost::any_cast<Ptr>(vp->back());
            std::vector<std::string> names = source->paramNames(false);
            for (auto const& i : names) {
                auto const sp = source->_find(i);
                _add(name + "." + i, sp->second);
            }
            return;
        }

        // Check for cycles
        _cycleCheckAnyVec(*vp, name);
    }

    std::string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        _map[name] = vp;
        return;
    }
    std::string prefix(name, 0, i);
    std::string suffix(name, i + 1);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end()) {
        PropertySet::Ptr pp(new PropertySet);
        pp->_findOrInsert(suffix, vp);
        std::shared_ptr<std::vector<boost::any>> temp(new std::vector<boost::any>);
        temp->push_back(pp);
        _map[prefix] = temp;
        return;
    } else if (j->second->back().type() != typeid(Ptr)) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError,
                          prefix + " exists but does not contain PropertySet::Ptrs");
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    if (p.get() == 0) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError,
                          prefix + " exists but contains null PropertySet::Ptr");
    }
    p->_findOrInsert(suffix, vp);
}

void PropertySet::_cycleCheckPtrVec(std::vector<Ptr> const& v, std::string const& name) {
    for (auto const& i : v) {
        _cycleCheckPtr(i, name);
    }
}

void PropertySet::_cycleCheckAnyVec(std::vector<boost::any> const& v, std::string const& name) {
    for (auto const& i : v) {
        _cycleCheckPtr(boost::any_cast<Ptr>(i), name);
    }
}

void PropertySet::_cycleCheckPtr(Ptr const& v, std::string const& name) {
    if (v.get() == this) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError, name + " would cause a cycle");
    }
    std::vector<std::string> sets = v->propertySetNames(false);
    for (auto const& i : sets) {
        if (v->getAsPropertySetPtr(i).get() == this) {
            throw LSST_EXCEPT(pex::exceptions::InvalidParameterError, name + " would cause a cycle");
        }
    }
}

    ///////////////////////////////////////////////////////////////////////////////
    // Explicit template instantiations
    ///////////////////////////////////////////////////////////////////////////////

    /// @cond
    // Explicit template instantiations are not well understood by doxygen.

#define INSTANTIATE(t)                                                                       \
    template std::type_info const& PropertySet::typeOfT<t>();                                \
    template t PropertySet::get<t>(std::string const& name) const;                           \
    template t PropertySet::get<t>(std::string const& name, t const& defaultValue) const;    \
    template std::vector<t> PropertySet::getArray<t>(std::string const& name) const;         \
    template void PropertySet::set<t>(std::string const& name, t const& value);              \
    template void PropertySet::set<t>(std::string const& name, std::vector<t> const& value); \
    template void PropertySet::add<t>(std::string const& name, t const& value);              \
    template void PropertySet::add<t>(std::string const& name, std::vector<t> const& value);

#define INSTANTIATE_PROPERTY_SET(t)                                                       \
    template std::type_info const& PropertySet::typeOfT<t>();                             \
    template t PropertySet::get<t>(std::string const& name) const;                        \
    template t PropertySet::get<t>(std::string const& name, t const& defaultValue) const; \
    template std::vector<t> PropertySet::getArray<t>(std::string const& name) const;      \
    template void PropertySet::set<t>(std::string const& name, t const& value);           \
    template void PropertySet::set<t>(std::string const& name, std::vector<t> const& value);

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
INSTANTIATE(std::nullptr_t)
INSTANTIATE(std::string)
INSTANTIATE_PROPERTY_SET(PropertySet::Ptr)
INSTANTIATE(Persistable::Ptr)
INSTANTIATE(DateTime)

}  // namespace base
}  // namespace daf
}  // namespace lsst

/// @endcond

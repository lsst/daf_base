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
#include <any>

#include "lsst/pex/exceptions/Runtime.h"
#include "lsst/daf/base/DateTime.h"

namespace lsst {
namespace daf {
namespace base {

namespace {

/**
 * Append the contents of a vector<T> to a vector<std::any>
 *
 * This method exists because vector<std::any>.insert mis-behaves for vector of bool,
 * resulting in a vector with elements that are a strange type.
 */
template <typename T>
void _append(std::vector<std::any>& dest, std::vector<T> const& src) {
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

std::shared_ptr<PropertySet> PropertySet::deepCopy() const {
    auto n = std::make_shared<PropertySet>(_flat);
    for (auto const& elt : _map) {
        if (elt.second->back().type() == typeid(std::shared_ptr<PropertySet>)) {
            for (auto const& j : *elt.second) {
                auto p = std::any_cast<std::shared_ptr<PropertySet>>(j);
                if (p.get() == 0) {
                    n->add(elt.first, std::shared_ptr<PropertySet>());
                } else {
                    n->add(elt.first, p->deepCopy());
                }
            }
        } else {
            std::shared_ptr<std::vector<std::any>> vp(new std::vector<std::any>(*(elt.second)));
            n->_map[elt.first] = vp;
        }
    }
    return n;
}

size_t PropertySet::nameCount(bool topLevelOnly) const {
    int n = 0;
    for (auto const& elt : _map) {
        ++n;
        if (!topLevelOnly && elt.second->back().type() == typeid(std::shared_ptr<PropertySet>)) {
            auto p = std::any_cast<std::shared_ptr<PropertySet>>(elt.second->back());
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
        if (!topLevelOnly && elt.second->back().type() == typeid(std::shared_ptr<PropertySet>)) {
            auto p = std::any_cast<std::shared_ptr<PropertySet>>(elt.second->back());
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
        if (elt.second->back().type() == typeid(std::shared_ptr<PropertySet>)) {
            auto p = std::any_cast<std::shared_ptr<PropertySet>>(elt.second->back());
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
        if (elt.second->back().type() == typeid(std::shared_ptr<PropertySet>)) {
            v.push_back(elt.first);
            auto p = std::any_cast<std::shared_ptr<PropertySet>>(elt.second->back());
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

bool PropertySet::isPropertySet(std::string const& name) const {
    auto const i = _find(name);
    return i != _map.end() && i->second->back().type() == typeid(std::shared_ptr<PropertySet>);
}

bool PropertySet::isPropertySetPtr(std::string const& name) const {
    return isPropertySet(name);
}

bool PropertySet::isUndefined(std::string const& name) const {
    auto const i = _find(name);
    return i != _map.end() && i->second->back().type() == typeid(nullptr);
}

size_t PropertySet::valueCount() const {
    size_t sum = 0;
    for (auto const& name : paramNames(false)) {
        sum += valueCount(name);
    }
    return sum;
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
        return std::any_cast<T>(i->second->back());
    } catch (std::bad_any_cast &) {
        std::throw_with_nested(LSST_EXCEPT(pex::exceptions::TypeError, name));
    }
    // not reached
    return std::any_cast<T>(i->second->back());
}

template <typename T>
T PropertySet::get(std::string const& name, T const& defaultValue)
        const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    auto const i = _find(name);
    if (i == _map.end()) {
        return defaultValue;
    }
    try {
        return std::any_cast<T>(i->second->back());
    } catch (std::bad_any_cast &) {
        std::throw_with_nested(LSST_EXCEPT(pex::exceptions::TypeError, name));
    }
    // not reached
    return std::any_cast<T>(i->second->back());
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
            v.push_back(std::any_cast<T>(j));
        } catch (std::bad_any_cast &) {
            std::throw_with_nested(LSST_EXCEPT(pex::exceptions::TypeError, name));
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
    std::any v = i->second->back();
    std::type_info const& t = v.type();
    if (t == typeid(bool)) {
        return std::any_cast<bool>(v);
    } else if (t == typeid(char)) {
        return std::any_cast<char>(v);
    } else if (t == typeid(signed char)) {
        return std::any_cast<signed char>(v);
    } else if (t == typeid(unsigned char)) {
        return std::any_cast<unsigned char>(v);
    } else if (t == typeid(short)) {
        return std::any_cast<short>(v);
    } else if (t == typeid(unsigned short)) {
        return std::any_cast<unsigned short>(v);
    }
    try {
        return std::any_cast<int>(v);
    } catch (std::bad_any_cast &) {
        std::throw_with_nested(LSST_EXCEPT(pex::exceptions::TypeError, name));
    }
    // not reached
    return std::any_cast<int>(v);
}

int64_t PropertySet::getAsInt64(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    std::any v = i->second->back();
    std::type_info const& t = v.type();
    if (t == typeid(bool)) return std::any_cast<bool>(v);
    if (t == typeid(char)) return std::any_cast<char>(v);
    if (t == typeid(signed char)) return std::any_cast<signed char>(v);
    if (t == typeid(unsigned char)) return std::any_cast<unsigned char>(v);
    if (t == typeid(short)) return std::any_cast<short>(v);
    if (t == typeid(unsigned short)) return std::any_cast<unsigned short>(v);
    if (t == typeid(int)) return std::any_cast<int>(v);
    if (t == typeid(unsigned int)) return std::any_cast<unsigned int>(v);
    if (t == typeid(long)) return std::any_cast<long>(v);
    if (t == typeid(long long)) return std::any_cast<long long>(v);
    try {
        return std::any_cast<int64_t>(v);
    } catch (std::bad_any_cast &) {
        std::throw_with_nested(LSST_EXCEPT(pex::exceptions::TypeError, name));
    }
    // not reached
    return std::any_cast<int64_t>(v);
}

uint64_t PropertySet::getAsUInt64(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    std::any v = i->second->back();
    std::type_info const& t = v.type();
    if (t == typeid(bool)) return std::any_cast<bool>(v);
    if (t == typeid(char)) return std::any_cast<char>(v);
    if (t == typeid(signed char)) return std::any_cast<signed char>(v);
    if (t == typeid(unsigned char)) return std::any_cast<unsigned char>(v);
    if (t == typeid(short)) return std::any_cast<short>(v);
    if (t == typeid(unsigned short)) return std::any_cast<unsigned short>(v);
    if (t == typeid(int)) return std::any_cast<int>(v);
    if (t == typeid(unsigned int)) return std::any_cast<unsigned int>(v);
    if (t == typeid(long)) return std::any_cast<long>(v);
    if (t == typeid(long long)) return std::any_cast<long long>(v);
    if (t == typeid(unsigned long long)) return std::any_cast<unsigned long long>(v);
    try {
        return std::any_cast<uint64_t>(v);
    } catch (std::bad_any_cast &) {
        std::throw_with_nested(LSST_EXCEPT(pex::exceptions::TypeError, name));
    }
    // not reached
    return std::any_cast<uint64_t>(v);
}

double PropertySet::getAsDouble(std::string const& name) const {
    auto const i = _find(name);
    if (i == _map.end()) {
        throw LSST_EXCEPT(pex::exceptions::NotFoundError, name + " not found");
    }
    std::any v = i->second->back();
    std::type_info const& t = v.type();
    if (t == typeid(bool)) return std::any_cast<bool>(v);
    if (t == typeid(char)) return std::any_cast<char>(v);
    if (t == typeid(signed char)) return std::any_cast<signed char>(v);
    if (t == typeid(unsigned char)) return std::any_cast<unsigned char>(v);
    if (t == typeid(short)) return std::any_cast<short>(v);
    if (t == typeid(unsigned short)) return std::any_cast<unsigned short>(v);
    if (t == typeid(int)) return std::any_cast<int>(v);
    if (t == typeid(unsigned int)) return std::any_cast<unsigned int>(v);
    if (t == typeid(long)) return std::any_cast<long>(v);
    if (t == typeid(unsigned long)) return std::any_cast<unsigned long>(v);
    if (t == typeid(long long)) return std::any_cast<long long>(v);
    if (t == typeid(unsigned long long)) return std::any_cast<unsigned long long>(v);
    if (t == typeid(float)) return std::any_cast<float>(v);
    try {
        return std::any_cast<double>(v);
    } catch (std::bad_any_cast &) {
        std::throw_with_nested(LSST_EXCEPT(pex::exceptions::TypeError, name));
    }
    // not reached
    return std::any_cast<double>(v);
}

std::string PropertySet::getAsString(std::string const& name) const { return get<std::string>(name); }

std::shared_ptr<PropertySet> PropertySet::getAsPropertySet(std::string const& name) const {
    return get<std::shared_ptr<PropertySet>>(name);
}

std::shared_ptr<PropertySet> PropertySet::getAsPropertySetPtr(std::string const& name) const {
    return getAsPropertySet(name);
}

Persistable::Ptr PropertySet::getAsPersistablePtr(std::string const& name) const {
    return get<Persistable::Ptr>(name);
}

std::string PropertySet::toString(bool topLevelOnly, std::string const& indent) const {
    std::ostringstream s;
    std::vector<std::string> nv = names();
    sort(nv.begin(), nv.end());
    for (auto const& i : nv) {
        std::shared_ptr<std::vector<std::any>> vp = _map.find(i)->second;
        std::type_info const& t = vp->back().type();
        if (t == typeid(std::shared_ptr<PropertySet>)) {
            s << indent << i << " = ";
            if (topLevelOnly) {
                s << "{ ... }";
            } else {
                auto p = std::any_cast<std::shared_ptr<PropertySet>>(vp->back());
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
    std::shared_ptr<std::vector<std::any>> vp = j->second;
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
        std::any const& v(k);
        if (t == typeid(bool)) {
            s << std::any_cast<bool>(v);
        } else if (t == typeid(char)) {
            s << '\'' << std::any_cast<char>(v) << '\'';
        } else if (t == typeid(signed char)) {
            s << '\'' << std::any_cast<signed char>(v) << '\'';
        } else if (t == typeid(unsigned char)) {
            s << '\'' << std::any_cast<unsigned char>(v) << '\'';
        } else if (t == typeid(short)) {
            s << std::any_cast<short>(v);
        } else if (t == typeid(unsigned short)) {
            s << std::any_cast<unsigned short>(v);
        } else if (t == typeid(int)) {
            s << std::any_cast<int>(v);
        } else if (t == typeid(unsigned int)) {
            s << std::any_cast<unsigned int>(v);
        } else if (t == typeid(long)) {
            s << std::any_cast<long>(v);
        } else if (t == typeid(unsigned long)) {
            s << std::any_cast<unsigned long>(v);
        } else if (t == typeid(long long)) {
            s << std::any_cast<long long>(v);
        } else if (t == typeid(unsigned long long)) {
            s << std::any_cast<unsigned long long>(v);
        } else if (t == typeid(float)) {
            s << std::setprecision(7) << std::any_cast<float>(v);
        } else if (t == typeid(double)) {
            s << std::setprecision(14) << std::any_cast<double>(v);
        } else if (t == typeid(std::string)) {
            s << '"' << std::any_cast<std::string>(v) << '"';
        } else if (t == typeid(DateTime)) {
            s << std::any_cast<DateTime>(v).toString(DateTime::UTC);
        } else if (t == typeid(std::shared_ptr<PropertySet>)) {
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
    std::shared_ptr<std::vector<std::any>> vp(new std::vector<std::any>);
    vp->push_back(value);
    _set(name, vp);
}

template <typename T>
void PropertySet::set(std::string const& name, std::vector<T> const& value) {
    if (value.empty()) return;
    std::shared_ptr<std::vector<std::any>> vp(new std::vector<std::any>);
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

// Specialize for shared_ptr to check for cycles.
template <>
void PropertySet::add<std::shared_ptr<PropertySet>>(
    std::string const& name,
    std::shared_ptr<PropertySet> const& value
) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    } else {
        if (i->second->back().type() != typeid(std::shared_ptr<PropertySet>)) {
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

// Specialize for shared_ptr to check for cycles.
template <>
void PropertySet::add<std::shared_ptr<PropertySet>>(
    std::string const& name,
    std::vector<std::shared_ptr<PropertySet>> const& value
) {
    AnyMap::iterator i = _find(name);
    if (i == _map.end()) {
        set(name, value);
    } else {
        if (i->second->back().type() != typeid(std::shared_ptr<PropertySet>)) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name + " has mismatched type");
        }
        _cycleCheckPtrVec(value, name);
        _append(*(i->second), value);
    }
}

void PropertySet::add(std::string const& name, char const* value) { add(name, std::string(value)); }

void PropertySet::copy(
    std::string const& dest,
    PropertySet const& source,
    std::string const& name,
    bool asScalar
) {
    auto const sj = source._find(name);
    if (sj == source._map.end()) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError, name + " not in source");
    }
    remove(dest);
    if (asScalar) {
        auto vp = std::make_shared<std::vector<std::any>>();
        vp->push_back(sj->second->back());
        _set(dest, vp);
    } else {
        auto vp = std::make_shared<std::vector<std::any>>(*(sj->second));
        _set(dest, vp);
    }
}

void PropertySet::copy(std::string const& dest, std::shared_ptr<PropertySet const> source,
                       std::string const& name, bool asScalar) {
    if (!source) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError, "Missing source");
    }
    copy(dest, *source, name, asScalar);
}

void PropertySet::combine(PropertySet const & source) {
    std::vector<std::string> names = source.paramNames(false);
    for (auto const& name : names) {
        auto const sp = source._find(name);
        _add(name, sp->second);
    }
}

void PropertySet::combine(std::shared_ptr<PropertySet const> source) {
    if (!source) {
        return;
    }
    combine(*source);
}

void PropertySet::remove(std::string const& name) {
    std::string::size_type i = name.find('.');
    if (_flat || i == name.npos) {
        _map.erase(name);
        return;
    }
    std::string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(std::shared_ptr<PropertySet>)) {
        return;
    }
    auto p = std::any_cast<std::shared_ptr<PropertySet>>(j->second->back());
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
    if (j == _map.end() || j->second->back().type() != typeid(std::shared_ptr<PropertySet>)) {
        return _map.end();
    }
    auto p = std::any_cast<std::shared_ptr<PropertySet>>(j->second->back());
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
    if (j == _map.end() || j->second->back().type() != typeid(std::shared_ptr<PropertySet>)) {
        return _map.end();
    }
    auto p = std::any_cast<std::shared_ptr<PropertySet>>(j->second->back());
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

void PropertySet::_set(std::string const& name, std::shared_ptr<std::vector<std::any>> vp) {
    _findOrInsert(name, vp);
}

void PropertySet::_add(std::string const& name, std::shared_ptr<std::vector<std::any>> vp) {
    auto const dp = _find(name);
    if (dp == _map.end()) {
        _set(name, vp);
    } else {
        if (vp->back().type() != dp->second->back().type()) {
            throw LSST_EXCEPT(pex::exceptions::TypeError, name + " has mismatched type");
        }
        // Check for cycles
        if (vp->back().type() == typeid(std::shared_ptr<PropertySet>)) {
            _cycleCheckAnyVec(*vp, name);
        }
        _append(*(dp->second), *vp);
    }
}

void PropertySet::_findOrInsert(std::string const& name, std::shared_ptr<std::vector<std::any>> vp) {
    if (vp->back().type() == typeid(std::shared_ptr<PropertySet>)) {
        if (_flat) {
            auto source = std::any_cast<std::shared_ptr<PropertySet>>(vp->back());
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
        auto pp = std::make_shared<PropertySet>();
        pp->_findOrInsert(suffix, vp);
        std::shared_ptr<std::vector<std::any>> temp(new std::vector<std::any>);
        temp->push_back(pp);
        _map[prefix] = temp;
        return;
    } else if (j->second->back().type() != typeid(std::shared_ptr<PropertySet>)) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError,
                          prefix + " exists but does not contain PropertySets");
    }
    auto p = std::any_cast<std::shared_ptr<PropertySet>>(j->second->back());
    if (p.get() == 0) {
        throw LSST_EXCEPT(pex::exceptions::InvalidParameterError,
                          prefix + " exists but contains null PropertySets");
    }
    p->_findOrInsert(suffix, vp);
}

void PropertySet::_cycleCheckPtrVec(
    std::vector<std::shared_ptr<PropertySet>> const& v,
    std::string const& name
) {
    for (auto const& i : v) {
        _cycleCheckPtr(i, name);
    }
}

void PropertySet::_cycleCheckAnyVec(std::vector<std::any> const& v, std::string const& name) {
    for (auto const& i : v) {
        _cycleCheckPtr(std::any_cast<std::shared_ptr<PropertySet>>(i), name);
    }
}

void PropertySet::_cycleCheckPtr(std::shared_ptr<PropertySet> const & v, std::string const& name) {
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
INSTANTIATE_PROPERTY_SET(std::shared_ptr<PropertySet>)
INSTANTIATE(Persistable::Ptr)
INSTANTIATE(DateTime)

}  // namespace base
}  // namespace daf
}  // namespace lsst

/// @endcond

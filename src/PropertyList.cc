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

#include "lsst/daf/base/PropertyList.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/daf/base/DateTime.h"

namespace lsst {
namespace daf {
namespace base {

/** Constructor.
 */
PropertyList::PropertyList() : _properties(true) {}

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<PropertyList> PropertyList::deepCopy() const {
    auto n = std::make_shared<PropertyList>();
    n->_properties.combine(*_properties.deepCopy());
    n->_order = _order;
    n->_comments = _comments;
    return n;
}


// The following throw an exception if the type does not match exactly.

template <typename T>
T PropertyList::get(std::string const& name) const {
    return _properties.get<T>(name);
}

template <typename T>
T PropertyList::get(std::string const& name, T const& defaultValue) const {
    return _properties.get<T>(name, defaultValue);
}

template <typename T>
std::vector<T> PropertyList::getArray(std::string const& name) const {
    return _properties.getArray<T>(name);
}

std::string const& PropertyList::getComment(std::string const& name) const {
    return _comments.find(name)->second;
}

std::vector<std::string> PropertyList::getOrderedNames() const {
    std::vector<std::string> v;
    for (auto const& name : _order) {
        v.push_back(name);
    }
    return v;
}

std::list<std::string>::const_iterator PropertyList::begin() const { return _order.begin(); }

std::list<std::string>::const_iterator PropertyList::end() const { return _order.end(); }

std::string PropertyList::toString(std::string const& indent) const {
    std::ostringstream s;
    for (auto const& name : _order) {
        s << _properties.format(name);
        std::string const& comment = _comments.find(name)->second;
        if (comment.size()) {
            s << "// " << comment << std::endl;
        }
    }
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////
// Modifiers
///////////////////////////////////////////////////////////////////////////////

#define LSST_PROPERTYLIST_CHECK_INVARIANTS() \
    assert(_properties.nameCount(true) == _order.size()); \
    assert(_properties.nameCount(true) == _comments.size()); \
    assert(_properties.nameCount(true) == _properties.nameCount(false))

///////////////////////////////////////////////////////////////////////////////
// Normal versions of set/add with placement control

template <typename T>
void PropertyList::set(std::string const& name, T const& value) {
    _properties.set(name, value);
    _addDefaultCommentAndOrder(name);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::set(std::string const& name, PropertySet const& value) {
    _properties.set(name, value.deepCopy());
    _comments.erase(name);
    _order.remove(name);
    for (auto const & k : value.paramNames(false)) {
        _addDefaultCommentAndOrder(name + "." + k);
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::set(std::string const& name, PropertyList const& value) {
    _properties.set(name, value._properties.deepCopy());
    _comments.erase(name);
    _order.remove(name);
    for (auto const& k : value._properties.paramNames(false)) {
        _commentOrderFix(name + "." + k, value.getComment(k));
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::set(std::string const& name, char const* value) { set(name, std::string(value)); }

template <typename T>
void PropertyList::set(std::string const& name, std::vector<T> const& value) {
    _properties.set(name, value);
    _addDefaultCommentAndOrder(name);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

template <typename T>
void PropertyList::add(std::string const& name, T const& value) {
    if (_properties.exists(name)) {
        _properties.add(name, value);
    } else {
        set(name, value);
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::add(std::string const& name, char const* value) { add(name, std::string(value)); }

template <typename T>
void PropertyList::add(std::string const& name, std::vector<T> const& value) {
    if (_properties.exists(name)) {
        _properties.add(name, value);
    } else {
        set(name, value);
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

///////////////////////////////////////////////////////////////////////////////
// Commented versions of set/add

template <typename T>
void PropertyList::set(std::string const& name, T const& value, std::string const& comment) {
    _properties.set(name, value);
    _commentOrderFix(name, comment);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::set(std::string const& name, char const* value, std::string const& comment) {
    set(name, std::string(value), comment);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

template <typename T>
void PropertyList::set(std::string const& name, std::vector<T> const& value, std::string const& comment) {
    _properties.set(name, value);
    _commentOrderFix(name, comment);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}
template <typename T>
void PropertyList::add(std::string const& name, T const& value, std::string const& comment) {
    if (_properties.exists(name)) {
        _properties.add(name, value);
        _commentOrderFix(name, comment);
    } else {
        set(name, value, comment);
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::add(std::string const& name, char const* value, std::string const& comment) {
    add(name, std::string(value), comment);
}

template <typename T>
void PropertyList::add(std::string const& name, std::vector<T> const& value, std::string const& comment) {
    if (_properties.exists(name)) {
        _properties.add(name, value);
        _commentOrderFix(name, comment);
    } else {
        set(name, value, comment);
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

///////////////////////////////////////////////////////////////////////////////
// Other modifiers

void PropertyList::copy(std::string const& dest, PropertySet const & source,
                        std::string const& name, bool asScalar) {
    _properties.copy(dest, source, name, asScalar);
    _addDefaultCommentAndOrder(dest);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::copy(std::string const& dest, PropertyList const & source,
                        std::string const& name, bool asScalar) {
    copy(dest, source._properties, name, asScalar);
    _commentOrderFix(dest, source._comments.find(name)->second);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::combine(PropertySet const & source) {
    std::list<std::string> newOrder;
    auto const paramNames = source.paramNames(false);
    for (auto const& name : paramNames) {
        bool present = _comments.find(name) != _comments.end();
        if (!present) {
            newOrder.push_back(name);
        }
    }
    _properties.combine(source);
    _order.swap(newOrder);
    for (auto const& name : paramNames) {
        auto iter = _comments.find(name);
        if (iter == _comments.end()) {
            _comments[name] = std::string();
        }
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::combine(PropertyList const & source) {
    std::list<std::string> newOrder(_order);
    for (auto const& name : source) {
        bool present = _comments.find(name) != _comments.end();
        if (!present) {
            newOrder.push_back(name);
        }
    }
    _properties.combine(source._properties);
    _order.swap(newOrder);
    for (auto const& name : source) {
        _comments[name] = source._comments.find(name)->second;
    }
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

void PropertyList::remove(std::string const& name) {
    _properties.remove(name);
    _comments.erase(name);
    _order.remove(name);
    LSST_PROPERTYLIST_CHECK_INVARIANTS();
}

///////////////////////////////////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////////////////////////////////

void PropertyList::_commentOrderFix(std::string const& name, std::string const& comment) {
    auto i = _comments.find(name);
    if (i == _comments.end()) {
        _comments[name] = comment;
        _order.push_back(name);
    } else {
        i->second = comment;
    }
}

void PropertyList::_addDefaultCommentAndOrder(std::string const & name) {
    if (_comments.find(name) == _comments.end()) {
        _comments[name] = std::string();
        _order.push_back(name);
    }
}




///////////////////////////////////////////////////////////////////////////////
// Explicit template instantiations
///////////////////////////////////////////////////////////////////////////////

/// @cond
// Explicit template instantiations are not well understood by doxygen.

#define INSTANTIATE(t)                                                                                       \
    template t PropertyList::get<t>(std::string const& name) const;                                          \
    template t PropertyList::get<t>(std::string const& name, t const& defaultValue) const;                   \
    template std::vector<t> PropertyList::getArray<t>(std::string const& name) const;                        \
    template void PropertyList::set<t>(std::string const& name, t const& value);                             \
    template void PropertyList::set<t>(std::string const& name, std::vector<t> const& value);                \
    template void PropertyList::add<t>(std::string const& name, t const& value);                             \
    template void PropertyList::add<t>(std::string const& name, std::vector<t> const& value);                \
    template void PropertyList::set<t>(std::string const& name, t const& value, std::string const& comment); \
    template void PropertyList::set<t>(std::string const& name, std::vector<t> const& value,                 \
                                       std::string const& comment);                                          \
    template void PropertyList::add<t>(std::string const& name, t const& value, std::string const& comment); \
    template void PropertyList::add<t>(std::string const& name, std::vector<t> const& value,                 \
                                       std::string const& comment);                                          \
    template void PropertyList::set<t>(std::string const& name, t const& value, char const* comment);        \
    template void PropertyList::set<t>(std::string const& name, std::vector<t> const& value,                 \
                                       char const* comment);                                                 \
    template void PropertyList::add<t>(std::string const& name, t const& value, char const* comment);        \
    template void PropertyList::add<t>(std::string const& name, std::vector<t> const& value,                 \
                                       char const* comment);

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
INSTANTIATE(std::string)
INSTANTIATE(Persistable::Ptr)
INSTANTIATE(DateTime)

/// @endcond

}  // namespace base
}  // namespace daf
}  // namespace lsst

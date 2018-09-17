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
PropertyList::PropertyList() : PropertySet(true) {}

/** Destructor.
 */
PropertyList::~PropertyList() noexcept = default;

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<PropertySet> PropertyList::_deepCopy() const {
    Ptr n(new PropertyList);
    n->PropertySet::combine(this->PropertySet::_deepCopy());
    n->_order = _order;
    n->_comments = _comments;
    return n;
}

// The following throw an exception if the type does not match exactly.

template <typename T>
T PropertyList::get(std::string const& name)
        const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    return PropertySet::get<T>(name);
}

template <typename T>
T PropertyList::get(std::string const& name, T const& defaultValue)
        const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    return PropertySet::get<T>(name, defaultValue);
}

template <typename T>
std::vector<T> PropertyList::getArray(std::string const& name) const {
    return PropertySet::getArray<T>(name);
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

std::string PropertyList::toString(bool topLevelOnly, std::string const& indent) const {
    std::ostringstream s;
    for (auto const& name : _order) {
        s << _format(name);
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

///////////////////////////////////////////////////////////////////////////////
// Normal versions of set/add with placement control

template <typename T>
void PropertyList::set(std::string const& name, T const& value) {
    PropertySet::set(name, value);
}

void PropertyList::set(std::string const& name, PropertySet::Ptr const& value) {
    Ptr pl = std::dynamic_pointer_cast<PropertyList, PropertySet>(value);
    PropertySet::set(name, value);
    _comments.erase(name);
    _order.remove(name);
    std::vector<std::string> paramNames = value->paramNames(false);
    if (pl) {
        for (auto const& paramName : paramNames) {
            _commentOrderFix(name + "." + paramName, pl->getComment(paramName));
        }
    }
}

void PropertyList::set(std::string const& name, char const* value) { set(name, std::string(value)); }

template <typename T>
void PropertyList::set(std::string const& name, std::vector<T> const& value) {
    PropertySet::set(name, value);
}

template <typename T>
void PropertyList::add(std::string const& name, T const& value) {
    PropertySet::add(name, value);
}

void PropertyList::add(std::string const& name, char const* value) { add(name, std::string(value)); }

template <typename T>
void PropertyList::add(std::string const& name, std::vector<T> const& value) {
    PropertySet::add(name, value);
}

///////////////////////////////////////////////////////////////////////////////
// Commented versions of set/add

template <typename T>
void PropertyList::set(std::string const& name, T const& value, std::string const& comment) {
    PropertySet::set(name, value);
    _commentOrderFix(name, comment);
}

void PropertyList::set(std::string const& name, char const* value, std::string const& comment) {
    set(name, std::string(value), comment);
}

template <typename T>
void PropertyList::set(std::string const& name, std::vector<T> const& value, std::string const& comment) {
    PropertySet::set(name, value);
    _commentOrderFix(name, comment);
}
template <typename T>
void PropertyList::add(std::string const& name, T const& value, std::string const& comment) {
    PropertySet::add(name, value);
    _commentOrderFix(name, comment);
}

void PropertyList::add(std::string const& name, char const* value, std::string const& comment) {
    add(name, std::string(value), comment);
}

template <typename T>
void PropertyList::add(std::string const& name, std::vector<T> const& value, std::string const& comment) {
    PropertySet::add(name, value);
    _commentOrderFix(name, comment);
}

///////////////////////////////////////////////////////////////////////////////
// Other modifiers

void PropertyList::copy(std::string const& dest, PropertySet::ConstPtr source, std::string const& name,
                        bool asScalar) {
    PropertySet::copy(dest, source, name, asScalar);
    ConstPtr pl = std::dynamic_pointer_cast<PropertyList const, PropertySet const>(source);
    if (pl) {
        _comments[name] = pl->_comments.find(name)->second;
    }
}

void PropertyList::combine(PropertySet::ConstPtr source) {
    ConstPtr pl = std::dynamic_pointer_cast<PropertyList const, PropertySet const>(source);
    std::list<std::string> newOrder;
    if (pl) {
        newOrder = _order;
        for (auto const& name : *pl) {
            bool present = _comments.find(name) != _comments.end();
            if (!present) {
                newOrder.push_back(name);
            }
        }
    }
    PropertySet::combine(source);
    if (pl) {
        _order = newOrder;
        for (auto const& name : *pl) {
            _comments[name] = pl->_comments.find(name)->second;
        }
    }
}

void PropertyList::remove(std::string const& name) {
    PropertySet::remove(name);
    _comments.erase(name);
    _order.remove(name);
}

///////////////////////////////////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////////////////////////////////

void PropertyList::_set(std::string const& name, std::shared_ptr<std::vector<boost::any> > vp) {
    PropertySet::_set(name, vp);
    if (_comments.find(name) == _comments.end()) {
        _comments.insert(std::make_pair(name, std::string()));
        _order.push_back(name);
    }
}

void PropertyList::_moveToEnd(std::string const& name) {
    _order.remove(name);
    _order.push_back(name);
}

void PropertyList::_commentOrderFix(std::string const& name, std::string const& comment) {
    _comments[name] = comment;
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

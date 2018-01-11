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

#include "lsst/pex/exceptions/Runtime.h"
#include "lsst/daf/base/DateTime.h"

namespace pexExcept = lsst::pex::exceptions;

using namespace std;

namespace lsst {
namespace daf {
namespace base {

/** Constructor.
  */
PropertyList::PropertyList(void) : PropertySet(true) {
}

/** Destructor.
  */
PropertyList::~PropertyList(void) {
}

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

PropertySet::Ptr PropertyList::deepCopy(void) const {
    Ptr n(new PropertyList);
    n->PropertySet::combine(this->PropertySet::deepCopy());
    n->_order = _order;
    n->_comments = _comments;
    return n;
}

// The following throw an exception if the type does not match exactly.

template <typename T>
T PropertyList::get(string const& name) const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    return PropertySet::get<T>(name);
}

template <typename T>
T PropertyList::get(string const& name, T const& defaultValue) const { /* parasoft-suppress LsstDm-3-4a LsstDm-4-6 "allow template over bool" */
    return PropertySet::get<T>(name, defaultValue);
}

template <typename T>
vector<T> PropertyList::getArray(string const& name) const {
    return PropertySet::getArray<T>(name);
}

std::string const& PropertyList::getComment(
    std::string const& name) const {
    return _comments.find(name)->second;
}

std::vector<std::string> PropertyList::getOrderedNames(void) const {
    std::vector<std::string> v;
    for (std::list<std::string>::const_iterator i = _order.begin();
         i != _order.end(); ++i) {
        v.push_back(*i);
    }
    return v;
}

std::list<std::string>::const_iterator
PropertyList::begin(void) const {
    return _order.begin();
}

std::list<std::string>::const_iterator
PropertyList::end(void) const {
    return _order.end();
}

std::string PropertyList::toString(bool topLevelOnly,
                                           std::string const& indent) const {
    ostringstream s;
    for (std::list<std::string>::const_iterator i = _order.begin();
         i != _order.end(); ++i) {
        s << _format(*i);
        std::string const& comment = _comments.find(*i)->second;
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
void PropertyList::set(
    std::string const& name, T const& value) {
    PropertySet::set(name, value);
}

void PropertyList::set(
    std::string const& name, PropertySet::Ptr const& value) {
    Ptr pl = std::dynamic_pointer_cast<PropertyList, PropertySet>(value);
    PropertySet::set(name, value);
    _comments.erase(name);
    _order.remove(name);
    vector<string> names = value->paramNames(false);
    for (vector<string>::const_iterator i = names.begin();
         i != names.end(); ++i) {
        if (pl) {
            _commentOrderFix(name + "." + *i, pl->getComment(*i));
        }
    }
}

void PropertyList::set(
    std::string const& name, char const* value) {
    set(name, string(value));
}

template <typename T>
void PropertyList::set(
    std::string const& name, vector<T> const& value) {
    PropertySet::set(name, value);
}

template <typename T>
void PropertyList::add(
    std::string const& name, T const& value) {
    PropertySet::add(name, value);
}

void PropertyList::add(
    std::string const& name, char const* value) {
    add(name, string(value));
}

template <typename T>
void PropertyList::add(
    std::string const& name, vector<T> const& value) {
    PropertySet::add(name, value);
}


///////////////////////////////////////////////////////////////////////////////
// Commented versions of set/add

template <typename T>
void PropertyList::set(
    std::string const& name, T const& value,
    std::string const& comment) {
    PropertySet::set(name, value);
    _commentOrderFix(name, comment);
}

void PropertyList::set(
    std::string const& name, char const* value,
    std::string const& comment) {
    set(name, string(value), comment);
}

template <typename T>
void PropertyList::set(
    std::string const& name, vector<T> const& value,
    std::string const& comment) {
    PropertySet::set(name, value);
    _commentOrderFix(name, comment);
}
template <typename T>
void PropertyList::add(
    std::string const& name, T const& value,
    std::string const& comment) {
    PropertySet::add(name, value);
    _commentOrderFix(name, comment);
}

void PropertyList::add(
    std::string const& name, char const* value,
    std::string const& comment) {
    add(name, string(value), comment);
}

template <typename T>
void PropertyList::add(
    std::string const& name, vector<T> const& value,
    std::string const& comment) {
    PropertySet::add(name, value);
    _commentOrderFix(name, comment);
}


///////////////////////////////////////////////////////////////////////////////
// Other modifiers

void PropertyList::copy(
    std::string const& dest, PropertySet::ConstPtr source,
    std::string const& name) {
    PropertySet::copy(dest, source, name);
    ConstPtr pl =
        std::dynamic_pointer_cast<PropertyList const, PropertySet const>(
            source);
    if (pl) {
        _comments[name] = pl->_comments.find(name)->second;
    }
}

void PropertyList::combine(PropertySet::ConstPtr source) {
    ConstPtr pl =
        std::dynamic_pointer_cast<PropertyList const, PropertySet const>(
            source);
    std::list<std::string> newOrder;
    if (pl) {
        newOrder = _order;
        for (std::list<std::string>::const_iterator i = pl->begin();
             i != pl->end(); ++i) {
            bool present = _comments.find(*i) != _comments.end();
            if (!present) {
                newOrder.push_back(*i);
            }
        }
    }
    PropertySet::combine(source);
    if (pl) {
        _order = newOrder;
        for (std::list<std::string>::const_iterator i = pl->begin();
             i != pl->end(); ++i) {
            _comments[*i] = pl->_comments.find(*i)->second;
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

void PropertyList::_set(std::string const& name,
          std::shared_ptr< std::vector<boost::any> > vp) {
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

void PropertyList::_commentOrderFix(
    std::string const& name, std::string const& comment) {
    _comments[name] = comment;
}

///////////////////////////////////////////////////////////////////////////////
// Explicit template instantiations
///////////////////////////////////////////////////////////////////////////////

/// @cond
// Explicit template instantiations are not well understood by doxygen.

#define INSTANTIATE(t) \
    template t PropertyList::get<t>(string const& name) const; \
    template t PropertyList::get<t>(string const& name, t const& defaultValue) const; \
    template vector<t> PropertyList::getArray<t>(string const& name) const; \
    template void PropertyList::set<t>(string const& name, t const& value); \
    template void PropertyList::set<t>(string const& name, vector<t> const& value); \
    template void PropertyList::add<t>(string const& name, t const& value); \
    template void PropertyList::add<t>(string const& name, vector<t> const& value); \
    template void PropertyList::set<t>(string const& name, t const& value, string const& comment); \
    template void PropertyList::set<t>(string const& name, vector<t> const& value, string const& comment); \
    template void PropertyList::add<t>(string const& name, t const& value, string const& comment); \
    template void PropertyList::add<t>(string const& name, vector<t> const& value, string const& comment); \
    template void PropertyList::set<t>(string const& name, t const& value, char const* comment); \
    template void PropertyList::set<t>(string const& name, vector<t> const& value, char const* comment); \
    template void PropertyList::add<t>(string const& name, t const& value, char const* comment); \
    template void PropertyList::add<t>(string const& name, vector<t> const& value, char const* comment);

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
INSTANTIATE(Persistable::Ptr)
INSTANTIATE(DateTime)

/// @endcond

} } } // namespace lsst::daf::base

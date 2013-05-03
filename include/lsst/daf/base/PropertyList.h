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
 
#ifndef LSST_DAF_BASE_PROPERTYLIST
#define LSST_DAF_BASE_PROPERTYLIST

/** @file
  * @ingroup daf_base
  *
  * @brief Interface for PropertyList class
  *
  * @version $Revision$
  * @date $Date$
  */

/** @class lsst::daf::base::PropertyList
  * @brief Class for storing ordered metadata with comments.
  *
  * This class stores key/value pairs like PropertySet, of which it is a
  * subclass.  The difference is that the PropertyList maintains the order of
  * the key/value pairs according to how they were inserted.  By default,
  * replacing the value of an existing key does not change its ordering.  In
  * addition, a comment string may be associated with each key/value pair.
  *
  * The main motivating use case for PropertyList is FITS headers.
  *
  * PropertyLists are not truly hierarchical, although they accept dotted paths
  * as keys.  If a PropertySet or PropertyList is added as a value to a
  * PropertyList, the hierarchical pathnames are flattened into the resulting
  * PropertyList.
  *
  * @ingroup daf_base
  */

#include <lsst/tr1/unordered_map.h>
#include <list>
#include <string>
#include <typeinfo>
#include <vector>

#include "boost/any.hpp"
#include "boost/shared_ptr.hpp"
#include "lsst/daf/base/PropertySet.h"

namespace lsst {
namespace daf {

namespace persistence {
    class PropertyListFormatter;
} // namespace lsst::daf::persistence


namespace base {

#if defined(__ICC)
#pragma warning (push)
#pragma warning (disable: 444)
#endif

class PropertyList : public PropertySet {
public:
// Typedefs
    typedef boost::shared_ptr<PropertyList> Ptr;
    typedef boost::shared_ptr<PropertyList const> ConstPtr;

// Constructors
    PropertyList(void);
    virtual ~PropertyList(void);

// Accessors
    virtual PropertySet::Ptr deepCopy(void) const;
    // Returns a PropertySet::Ptr pointing to a new deep copy of the
    // PropertyList.

    template <typename T> T get(std::string const& name) const;
        // Note that the type must be explicitly specified for this template:
        // int i = propertyList.get<int>("foo");
    template <typename T>
        T get(std::string const& name, T const& defaultValue) const;
        // Returns the provided default value if the name does not exist.
    template <typename T>
        std::vector<T> getArray(std::string const& name) const;

    std::string const& getComment(std::string const& name) const;
    std::vector<std::string> getOrderedNames(void) const;

    std::list<std::string>::const_iterator begin(void) const;
    std::list<std::string>::const_iterator end(void) const;
    
    // Use this for debugging, not for serialization/persistence.
    virtual std::string toString(bool topLevelOnly = false,
                    std::string const& indent = "") const;

// Modifiers
    template <typename T> void set(
        std::string const& name, T const& value, bool inPlace=true);
    void set(
        std::string const& name, PropertySet::Ptr const& value,
        bool inPlace=true);
    template <typename T> void set(
        std::string const& name, std::vector<T> const& value,
        bool inPlace=true);
    void set(
        std::string const& name, char const* value, bool inPlace=true);
    template <typename T> void add(
        std::string const& name, T const& value, bool inPlace=true);
    template <typename T> void add(
        std::string const& name, std::vector<T> const& value,
        bool inPlace=true);
    void add(
        std::string const& name, char const* value, bool inPlace=true);

    template <typename T> void set(
        std::string const& name, T const& value,
        std::string const& comment, bool inPlace=true);
    template <typename T> void set(
        std::string const& name, std::vector<T> const& value,
        std::string const& comment, bool inPlace=true);
    void set(
        std::string const& name, char const* value,
        std::string const& comment, bool inPlace=true);
    template <typename T> void add(
        std::string const& name, T const& value,
        std::string const& comment, bool inPlace=true);
    template <typename T> void add(
        std::string const& name, std::vector<T> const& value,
        std::string const& comment, bool inPlace=true);
    void add(
        std::string const& name, char const* value,
        std::string const& comment, bool inPlace=true);

    template <typename T> void set(
        std::string const& name, T const& value,
        char const* comment, bool inPlace=true) {
        set(name, value, std::string(comment), inPlace);
    }
    template <typename T> void set(
        std::string const& name, std::vector<T> const& value,
        char const* comment, bool inPlace=true) {
        set(name, value, std::string(comment), inPlace);
    }
    void set(
        std::string const& name, char const* value,
        char const* comment, bool inPlace=true) {
        set(name, value, std::string(comment), inPlace);
    }
    template <typename T> void add(
        std::string const& name, T const& value,
        char const* comment, bool inPlace=true) {
        add(name, value, std::string(comment), inPlace);
    }
    template <typename T> void add(
        std::string const& name, std::vector<T> const& value,
        char const* comment, bool inPlace=true) {
        add(name, value, std::string(comment), inPlace);
    }
    void add(
        std::string const& name, char const* value,
        char const* comment, bool inPlace=true) {
        add(name, value, std::string(comment), inPlace);
    }

    virtual void copy(std::string const& dest, PropertySet::ConstPtr source,
                      std::string const& name) {
        copy(dest, source, name, true);
    }
    virtual void combine(PropertySet::ConstPtr source) {
        combine(source, true);
    }

    virtual void copy(std::string const& dest, PropertySet::ConstPtr source,
                      std::string const& name, bool inPlace);
    virtual void combine(PropertySet::ConstPtr source, bool inPlace);

    virtual void remove(std::string const& name);

private:
    LSST_PERSIST_FORMATTER(lsst::daf::persistence::PropertyListFormatter)

    typedef std::tr1::unordered_map<std::string, std::string> CommentMap;

    virtual void _set(std::string const& name,
                      boost::shared_ptr< std::vector<boost::any> > vp);
    virtual void _moveToEnd(std::string const& name);
    virtual void _commentOrderFix(
        std::string const& name, std::string const& comment, bool inPlace);

    CommentMap _comments;
    std::list<std::string> _order;
};

#if defined(__ICC)
#pragma warning (pop)
#endif    

}}} // namespace lsst::daf::base

#endif

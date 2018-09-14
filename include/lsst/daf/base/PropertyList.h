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

#include <list>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "boost/any.hpp"

#include "lsst/base.h"
#include "lsst/daf/base/PropertySet.h"

namespace lsst {
namespace daf {
namespace base {

#if defined(__ICC)
#pragma warning(push)
#pragma warning(disable : 444)
#endif

class LSST_EXPORT PropertyList : public PropertySet {
public:
    // Typedefs
    typedef std::shared_ptr<PropertyList> Ptr;
    typedef std::shared_ptr<PropertyList const> ConstPtr;

    /// Construct an empty PropertyList
    PropertyList(void);
    /// Destructor
    virtual ~PropertyList(void);

    // Accessors

    /**
     * Make a deep copy of the PropertyList and all of its contents.
     *
     * @return PropertyList::Ptr pointing to the new copy.
     */
    virtual PropertySet::Ptr deepCopy(void) const;

    // I can't make copydoc work for this so...
    /**
     * Get the last value for a property name (possibly hierarchical).
     *
     * Note that the type must be explicitly specified for this template:
     * @code int i = propertySet.get<int>("foo") @endcode
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Last value set or added.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value does not match desired type.
     */
    template <typename T>
    T get(std::string const& name) const;

    // I can't make copydoc work for this so...
    /**
     * Get the last value for a property name (possibly hierarchical);
     * return the provided @a defaultValue if the property does not exist.
     *
     * Note that the type must be explicitly specified for this template:
     * @code int i = propertySet.get<int>("foo", 42) @endcode
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @param[in] defaultValue Default value to return if property does not exist.
     * @return Last value set or added.
     * @throws TypeError Value does not match desired type.
     */
    template <typename T>
    T get(std::string const& name, T const& defaultValue) const;

    /// @copydoc PropertySet::getArray()
    template <typename T>
    std::vector<T> getArray(std::string const& name) const;

    /**
     * Get the comment for a string property name (possibly hierarchical).
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Comment string.
     * @throws NotFoundError Property does not exist.
     */
    std::string const& getComment(std::string const& name) const;

    /// Get the list of property names, in the order they were added
    std::vector<std::string> getOrderedNames(void) const;

    /// Begin iterator over the list of property names, in the order they were added
    std::list<std::string>::const_iterator begin(void) const;

    /// End iterator over the list of property names, in the order they were added
    std::list<std::string>::const_iterator end(void) const;

    /// @copydoc PropertySet::toString()
    virtual std::string toString(bool topLevelOnly = false, std::string const& indent = "") const;

    // Modifiers

    /// @copydoc PropertySet::set(std::string const &, T const &)
    template <typename T>
    void set(std::string const& name, T const& value);

    /**
     * Replace all values for a property name (possibly hierarchical) with a new
     * PropertySet.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Value to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    void set(std::string const& name, PropertySet::Ptr const& value);

    /// @copydoc PropertySet::set(std::string const&, std::vector<T> const&)
    template <typename T>
    void set(std::string const& name, std::vector<T> const& value);

    /// @copydoc PropertySet::set(std::string const &, char const*)
    void set(std::string const& name, char const* value);

    /// @copydoc PropertySet::add(std::string const&, T const&)
    template <typename T>
    void add(std::string const& name, T const& value);

    /// @copydoc PropertySet::add(std::string const&, std::vector<T> const&)
    template <typename T>
    void add(std::string const& name, std::vector<T> const& value);

    /// @copydoc PropertySet::add(std::string const&, char const*)
    void add(std::string const& name, char const* value);

    /**
     * Version of set scalar value that accepts a comment.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Value to set.
     * @param[in] comment Comment to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    template <typename T>
    void set(std::string const& name, T const& value, std::string const& comment);

    /**
     * Version of set vector value that accepts a comment.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Vector value to set.
     * @param[in] comment Comment to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    template <typename T>
    void set(std::string const& name, std::vector<T> const& value, std::string const& comment);

    /**
     * Version of set char* value that accepts a comment.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Character string value to set.
     * @param[in] comment Comment to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    void set(std::string const& name, char const* value, std::string const& comment);

    /**
     * Version of add scalar value that accepts a comment.
     *
     * @param[in] name Property name to append to, possibly hierarchical.
     * @param[in] value Value to add.
     * @param[in] comment Comment to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    template <typename T>
    void add(std::string const& name, T const& value, std::string const& comment);

    /**
     * Version of add vector value that accepts a comment.
     *
     * @param[in] name Property name to append to, possibly hierarchical.
     * @param[in] value Character string value to add.
     * @param[in] comment Comment to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    template <typename T>
    void add(std::string const& name, std::vector<T> const& value, std::string const& comment);

    /**
     * Version of add char* value that accepts a comment.
     *
     * @param[in] name Property name to append to, possibly hierarchical.
     * @param[in] value Character string value to add.
     * @param[in] comment Comment to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    void add(std::string const& name, char const* value, std::string const& comment);

    /// @copydoc PropertyList::set(std::string const&, T const&, std::string const&)
    template <typename T>
    void set(std::string const& name, T const& value, char const* comment) {
        set(name, value, std::string(comment));
    }

    /// @copydoc PropertyList::set(std::string const&, std::vector<T> const&, std::string const&)
    template <typename T>
    void set(std::string const& name, std::vector<T> const& value, char const* comment) {
        set(name, value, std::string(comment));
    }

    /// @copydoc PropertyList::set(std::string const&, char const*, std::string const&)
    void set(std::string const& name, char const* value, char const* comment) {
        set(name, value, std::string(comment));
    }

    /// @copydoc PropertyList::add(std::string const&, T const&, std::string const&)
    template <typename T>
    void add(std::string const& name, T const& value, char const* comment) {
        add(name, value, std::string(comment));
    }
    /// @copydoc PropertyList::add(std::string const&, std::vector<T> const&, std::string const&)
    template <typename T>
    void add(std::string const& name, std::vector<T> const& value, char const* comment) {
        add(name, value, std::string(comment));
    }

    /// @copydoc PropertyList::add(std::string const&, char const*, std::string const&)
    void add(std::string const& name, char const* value, char const* comment) {
        add(name, value, std::string(comment));
    }

    /// @copydoc PropertySet::copy
    virtual void copy(std::string const& dest, PropertySet::ConstPtr source, std::string const& name,
                      bool asScalar = false);

    /// @copydoc PropertySet::combine
    virtual void combine(PropertySet::ConstPtr source);

    /// @copydoc PropertySet::remove
    virtual void remove(std::string const& name);

private:

    typedef std::unordered_map<std::string, std::string> CommentMap;

    virtual void _set(std::string const& name, std::shared_ptr<std::vector<boost::any> > vp);
    virtual void _moveToEnd(std::string const& name);
    virtual void _commentOrderFix(std::string const& name, std::string const& comment);

    CommentMap _comments;
    std::list<std::string> _order;
};

#if defined(__ICC)
#pragma warning(pop)
#endif
}
}  // namespace daf
}  // namespace lsst

#endif

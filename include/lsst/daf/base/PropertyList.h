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

class LSST_EXPORT PropertyList final {
public:
    // Typedefs
    typedef std::shared_ptr<PropertyList> Ptr;
    typedef std::shared_ptr<PropertyList const> ConstPtr;

    /// Construct an empty PropertyList
    PropertyList();

    ///@{
    /**
     * Copy construction and assignment.
     */
    PropertyList(PropertyList const &) = default;
    PropertyList& operator=(PropertyList const &) = default;
    ///@}

    ///@{
    /**
     * Move construction and assignment.
     */
    PropertyList(PropertyList&&) noexcept = default;
    PropertyList& operator=(PropertyList&&) noexcept = default;
    //@}

    /**
     * Convert to a PropertySet.
     *
     * The returned object will have been constructed with flat=True.
     */
    explicit operator PropertySet () const { return _properties; }

    ~PropertyList() noexcept = default;

    // Accessors

    /// Get the number of names in the PropertyList,
    size_t nameCount() const { return _properties.nameCount(); }

    /// Get the number of names in the PropertyList.
    std::vector<std::string> names() const { return _properties.names(); }

    /**
     * Make a deep copy of the PropertyList and all of its contents.
     *
     * @return shared_ptr pointing to the new copy.
     */
    std::shared_ptr<PropertyList> deepCopy() const;

    /// @copydoc PropertySet::exists
    bool exists(std::string const& name) const { return _properties.exists(name); }

    /// @copydoc PropertySet::isArray
    bool isArray(std::string const& name) const { return _properties.isArray(name); }

    /// @copydoc PropertySet::valueCount
    size_t valueCount(std::string const& name) const { return _properties.valueCount(name); }

    /// @copydoc PropertySet::typeOf
    std::type_info const& typeOf(std::string const& name) const { return _properties.typeOf(name); }

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

    /// @copydoc PropertySet::getAsBool
    bool getAsBool(std::string const& name) const { return _properties.getAsBool(name); }

    /// @copydoc PropertySet::getAsInt
    int getAsInt(std::string const& name) const { return _properties.getAsInt(name); }

    /// @copydoc PropertySet::getAsInt64
    int64_t getAsInt64(std::string const& name) const { return _properties.getAsInt64(name); }

    /// @copydoc PropertySet::getAsDouble
    double getAsDouble(std::string const& name) const { return _properties.getAsDouble(name); }

    /// @copydoc PropertySet::getAsString
    std::string getAsString(std::string const& name) const { return _properties.getAsString(name); }

    /// @copydoc PropertySet::getAsPersistablePtr
    Persistable::Ptr getAsPersistablePtr(std::string const& name) const {
        return _properties.getAsPersistablePtr(name);
    }

    /**
     * Get the comment for a string property name (possibly hierarchical).
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Comment string.
     * @throws NotFoundError Property does not exist.
     */
    std::string const& getComment(std::string const& name) const;

    /// Get the list of property names, in the order they were added
    std::vector<std::string> getOrderedNames() const;

    /// Begin iterator over the list of property names, in the order they were added
    std::list<std::string>::const_iterator begin() const;

    /// End iterator over the list of property names, in the order they were added
    std::list<std::string>::const_iterator end() const;

    /**
     * Generate a string representation of the PropertyList.
     *
     * Use this for debugging, not for serialization/persistence.
     *
     * @param[in] indent String to indent lines by (default none).
     * @return String representation of the PropertySet.
     */
    std::string toString(std::string const& indent = "") const;

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
    void set(std::string const& name, PropertySet const& value);

    /// Deprecated shared_ptr overload of set().
    [[deprecated("Pass by const reference, not by shared_ptr.")]]
    void set(std::string const& name, std::shared_ptr<PropertySet> const & value) {
        set(name, *value);
    }

    /// Deprecated shared_ptr overload of set().
    [[deprecated("Pass by const reference, not by shared_ptr.")]]
    void set(std::string const& name, std::shared_ptr<PropertySet const> const & value) {
        set(name, *value);
    }

    /**
     * Replace all values for a property name (possibly hierarchical) with a new
     * PropertyList.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Value to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    void set(std::string const& name, PropertyList const& value);

    /// Deprecated shared_ptr overload of set().
    [[deprecated("Pass by const reference, not by shared_ptr.")]]
    void set(std::string const& name, std::shared_ptr<PropertyList> const & value) {
        set(name, *value);
    }

    /// Deprecated shared_ptr overload of set().
    [[deprecated("Pass by const reference, not by shared_ptr.")]]
    void set(std::string const& name, std::shared_ptr<PropertyList const> const & value) {
        set(name, *value);
    }

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

    /**
     * Replace a single value vector in the destination with one from the
     * \a source.
     *
     * @param[in] dest Destination property name.
     * @param[in] source the source PropertySet.
     * @param[in] name Property name to extract.
     * @param[in] asScalar If true copy the item as a scalar by ignoring all but the last value
     *                     (which is the value returned by get<T>(name))
     * @throws TypeError Type does not match existing values.
     * @throws InvalidParameterError Name does not exist in source.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet or PropertyList.
     */
    void copy(std::string const& dest, PropertySet const & source, std::string const& name,
              bool asScalar = false);

    /**
     * Replace a single value vector in the destination with one from the
     * \a source.
     *
     * @param[in] dest Destination property name.
     * @param[in] source the source PropertyList.
     * @param[in] name Property name to extract.
     * @param[in] asScalar If true copy the item as a scalar by ignoring all but the last value
     *                     (which is the value returned by get<T>(name))
     * @throws TypeError Type does not match existing values.
     * @throws InvalidParameterError Name does not exist in source.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet or PropertyList.
     */
    void copy(std::string const& dest, PropertyList const & source, std::string const& name,
              bool asScalar = false);

    /**
     * Append all value vectors from the \a source to their corresponding
     * properties.  Sets values if a property does not exist.
     *
     * If a property already exists then the types of the existing value(s)
     * must match the type of the value(s) in \a source.
     *
     * @param[in] source the source PropertyList.
     * @throws TypeError Type does not match existing values for an item.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     *
     * @warning May only partially combine the PropertySets if an exception occurs.
     */
    void combine(PropertySet const & source);

    /**
     * Append all value vectors from the \a source to their corresponding
     * properties.  Sets values if a property does not exist.
     *
     * If a property already exists then the types of the existing value(s)
     * must match the type of the value(s) in \a source.
     *
     * @param[in] source the source PropertyList.
     * @throws TypeError Type does not match existing values for an item.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     *
     * @warning May only partially combine the PropertySets if an exception occurs.
     */
    void combine(PropertyList const & source);

    /// @copydoc PropertySet::remove
    void remove(std::string const& name);

private:

    typedef std::unordered_map<std::string, std::string> CommentMap;

    void _commentOrderFix(std::string const& name, std::string const& comment);

    void _addDefaultCommentAndOrder(std::string const & name);

    PropertySet _properties;
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

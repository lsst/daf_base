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

#ifndef LSST_DAF_BASE_PROPERTYSET
#define LSST_DAF_BASE_PROPERTYSET

/** @class lsst::daf::base::PropertySet
 * @brief Class for storing generic metadata.
 *
 * This class stores key/value pairs, like a Python dictionary but in C++.
 * Keys are always C++ strings.  Values can be C++ primitive data types,
 * strings, lsst::daf::base::DateTime objects, and lsst::daf::base::Persistable
 * subclasses (although the latter is currently discouraged).  Values can also
 * be vectors of these items.
 *
 * PropertySets are hierarchical; values within a PropertySet that is contained
 * within another PropertySet can be addressed using dotted paths ("a.b.c").
 * If "flat=true" is specified to the constructor, the PropertySet still takes
 * dotted paths but is not actually hierarchical in structure.  This is used to
 * support PropertyList.
 *
 * @ingroup daf_base
 */

#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "boost/any.hpp"

#include "lsst/base.h"
#include "lsst/daf/base/Persistable.h"
#include "lsst/pex/exceptions.h"

namespace lsst {
namespace daf {
namespace base {

#if defined(__ICC)
#pragma warning(push)
#pragma warning(disable : 444)
#endif

class LSST_EXPORT PropertySet {
public:
    // Typedefs
    typedef std::shared_ptr<PropertySet> Ptr;
    typedef std::shared_ptr<PropertySet const> ConstPtr;

    /**
     * Construct an empty PropertySet
     *
     * @param[in] flat false (default) = flatten hierarchy by ignoring dots in names
     */
    explicit PropertySet(bool flat = false);

    /// Destructor
    virtual ~PropertySet() noexcept;

    // No copying
    PropertySet(const PropertySet&) = delete;
    PropertySet& operator=(const PropertySet&) = delete;

    // No moving
    PropertySet(PropertySet&&) = delete;
    PropertySet& operator=(PropertySet&&) = delete;

    // Accessors

    /**
     * Make a deep copy of the PropertySet and all of its contents.
     *
     * @return PropertySet::Ptr pointing to the new copy.
     */
    virtual Ptr deepCopy() const;

    /**
     * Get the number of names in the PropertySet, optionally including those in subproperties.
     *
     * @param[in] topLevelOnly If true (default) omit names from subproperties and names of subproperties.
     * @return Number of names.
     */
    size_t nameCount(bool topLevelOnly = true) const;

    /**
     * Get the names in the PropertySet, optionally including those in subproperties.
     *
     * @param[in] topLevelOnly  If true (default) omit names from subproperties and names of subproperties.
     *                          If false subproperty names are separated by a dot, e.g. "subname.subitem1"
     * @return Vector of names.
     */
    std::vector<std::string> names(bool topLevelOnly = true) const;

    /**
     * A variant of @ref names that excludes the names of subproperties.
     */
    std::vector<std::string> paramNames(bool topLevelOnly = true) const;

    /**
     * A variant of @ref names that only returns the names of subproperties.
     */
    std::vector<std::string> propertySetNames(bool topLevelOnly = true) const;

    /**
     * Determine if a name (possibly hierarchical) exists.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return true if property exists.
     */
    bool exists(std::string const& name) const;

    /**
     * Determine if a name (possibly hierarchical) has multiple values.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return true if property exists and has more than one value.
     */
    bool isArray(std::string const& name) const;

    /**
     * Determine if a name (possibly hierarchical) is a subproperty.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return true if property exists and its values are PropertySet::Ptrs.
     */
    bool isPropertySetPtr(std::string const& name) const;

    /**
     * Determine if a name (possibly hierarchical) has a defined value.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return true if property exists and its values are undefined.
     */
    bool isUndefined(std::string const& name) const;

    /**
     * Get the number of values in the entire PropertySet, counting each
     * element of a vector.
     *
     * @return Number of values.
     */
    size_t valueCount() const;

    /**
     * Get the number of values for a property name (possibly hierarchical).
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Number of values for that property.  0 if it doesn't exist.
     */
    size_t valueCount(std::string const& name) const;

    /**
     * Get the type of values for a property name (possibly hierarchical).
     *
     * If the value is an array this returns the type of the elements.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Type of values for that property.
     * @throws NotFoundError Property does not exist.
     */
    std::type_info const& typeOf(std::string const& name) const;

    /**
     * Get type info for the specified class
     */
     // Implemented in the .cc file to work around symbol visiblity issues on macOS
     // e.g. https://github.com/pybind/pybind11/issues/1503
    template <typename T>
    static std::type_info const& typeOfT();

    // The following throw an exception if the type does not match exactly.

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

    /**
     * Get the vector of values for a property name (possibly hierarchical).
     *
     * Note that the type must be explicitly specified for this template:
     * @code vector<int> v = propertySet.getArray<int>("foo") @endcode
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Vector of values.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value does not match desired type.
     */
    template <typename T>
    std::vector<T> getArray(std::string const& name) const;

    // The following throw an exception if the conversion is inappropriate.

    /**
     * Get the last value for a bool property name (possibly hierarchical).
     *
     * Only bool properties are supported.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Value as a bool.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value is not a bool.
     */
    bool getAsBool(std::string const& name) const;

    /**
     * Get the last value for a bool/char/short/int property name (possibly
     * hierarchical). Unsigned int properties are not acceptable.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Value as an int.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value cannot be converted to int.
     */
    int getAsInt(std::string const& name) const;

    /**
     * Get the last value for a bool/char/short/int/int64_t property name
     * (possibly hierarchical).  Unsigned int64_t properties are not acceptable,
     * but unsigned versions of smaller types are, except possibly unsigned long,
     * depending on compiler.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Value as an int64_t.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value cannot be converted to int64_t.
     */
    int64_t getAsInt64(std::string const& name) const;

    /**
     * Get the last value for an bool/char/short/int/int64_t property name
     * (possibly hierarchical).
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Value as an uint64_t.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value cannot be converted to uint64_t.
     */
    uint64_t getAsUInt64(std::string const& name) const;

    /**
     * Get the last value for any arithmetic property name (possibly
     * hierarchical).
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Value as a double.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value cannot be converted to double.
     */
    double getAsDouble(std::string const& name) const;

    /**
     * Get the last value for a string property name (possibly hierarchical).
     *
     * Only strings values are supported, though this does include properties
     * set with <tt>char const*</tt>.
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return String value.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value is not a string.
     */
    std::string getAsString(std::string const& name) const;

    /**
     * Get the last value for a subproperty name (possibly hierarchical).
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return PropertySet::Ptr value.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value is not a PropertySet::Ptr.
     */
    PropertySet::Ptr getAsPropertySetPtr(std::string const& name) const;

    /**
     * Get the last value for a Persistable name (possibly hierarchical).
     *
     * @param[in] name Property name to examine, possibly hierarchical.
     * @return Persistable::Ptr value.
     * @throws NotFoundError Property does not exist.
     * @throws TypeError Value is not a Persistable::Ptr.
     */
    Persistable::Ptr getAsPersistablePtr(std::string const& name) const;

    /**
     * Generate a string representation of the PropertySet.
     *
     * Use this for debugging, not for serialization/persistence.
     *
     * @param[in] topLevelOnly false (default) = do include subproperties.
     * @param[in] indent String to indent lines by (default none).
     * @return String representation of the PropertySet.
     */
    virtual std::string toString(bool topLevelOnly = false, std::string const& indent = "") const;

    // Modifiers

    /**
     * Replace all values for a property name (possibly hierarchical) with a new
     * scalar value.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Value to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    template <typename T>
    void set(std::string const& name, T const& value);

    /**
     * Replace all values for a property name (possibly hierarchical) with a
     * vector of new values.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Vector of values to set.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    template <typename T>
    void set(std::string const& name, std::vector<T> const& value);

    /**
     * Replace all values for a property name (possibly hierarchical) with a
     * string value.
     *
     * @param[in] name Property name to set, possibly hierarchical.
     * @param[in] value Character string (converted to \c std::string ).
     */
    void set(std::string const& name, char const* value);

    /**
     * Append a single value to the vector of values for a property name
     * (possibly hierarchical).  Sets the value if the property does not exist.
     *
     * @param[in] name Property name to append to, possibly hierarchical.
     * @param[in] value Value to append.
     * @throws TypeError Type does not match existing values.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    template <typename T>
    void add(std::string const& name, T const& value);

    /**
     * Append a vector of values to the vector of values for a property name
     * (possibly hierarchical).  Sets the values if the property does not exist.
     *
     * @param[in] name Property name to append to, possibly hierarchical.
     * @param[in] value Vector of values to append.
     * @throws TypeError Type does not match existing values.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     * @note
     * May only partially add the vector if an exception occurs.
     */
    template <typename T>
    void add(std::string const& name, std::vector<T> const& value);

    /**
     * Append a <tt>char const*</tt> value to the vector of values for a
     * property name (possibly hierarchical).  Sets the value if the property
     * does not exist.
     *
     * @param[in] name Property name to append to, possibly hierarchical.
     * @param[in] value Character string value to append.
     * @throws TypeError Type does not match existing values.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    void add(std::string const& name, char const* value);

    /**
     * Replace a single value vector in the destination with one from the
     * \a source.
     *
     * @param[in] dest Destination property name.
     * @param[in] source PropertySet::Ptr for the source PropertySet.
     * @param[in] name Property name to extract.
     * @param[in] asScalar If true copy the item as a scalar by ignoring all but the last value
     *                     (which is the value returned by get<T>(name))
     * @throws TypeError Type does not match existing values.
     * @throws InvalidParameterError Name does not exist in source.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    virtual void copy(std::string const& dest, ConstPtr source, std::string const& name,
                      bool asScalar = false);

    /**
     * Append all value vectors from the \a source to their corresponding
     * properties.  Sets values if a property does not exist.
     *
     * If a property already exists then the types of the existing value(s)
     * must match the type of the value(s) in \a source.
     *
     * @param[in] source PropertySet::Ptr for the source PropertySet.
     * @throws TypeError Type does not match existing values for an item.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     *
     * @warning May only partially combine the PropertySets if an exception occurs.
     */
    virtual void combine(ConstPtr source);

    /**
     * Remove all values for a property name (possibly hierarchical).  Does
     * nothing if the property does not exist.
     *
     * @param[in] name Property name to remove, possibly hierarchical.
     */
    virtual void remove(std::string const& name);

protected:
    /*
     * Find the property name (possibly hierarchical) and set or replace its
     * value with the given vector of values.  Hook for subclass overrides of
     * top-level setting.
     *
     * @param[in] name Property name to find, possibly hierarchical.
     * @param[in] vp shared_ptr to vector of values.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    virtual void _set(std::string const& name, std::shared_ptr<std::vector<boost::any> > vp);

    /*
     * Find the property name (possibly hierarchical) and append or set its
     * value with the given vector of values.
     *
     * @param[in] name Property name to find, possibly hierarchical.
     * @param[in] vp shared_ptr to vector of values.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    virtual void _add(std::string const& name, std::shared_ptr<std::vector<boost::any> > vp);

    // Format a value in human-readable form; called by toString
    virtual std::string _format(std::string const& name) const;

private:

    typedef std::unordered_map<std::string, std::shared_ptr<std::vector<boost::any> > > AnyMap;

    /*
     * Find the property name (possibly hierarchical).
     *
     * @param[in] name Property name to find, possibly hierarchical.
     * @return unordered_map::iterator to the property or end() if nonexistent.
     */
    AnyMap::iterator _find(std::string const& name);

    /*
     * Find the property name (possibly hierarchical).  Const version.
     *
     * @param[in] name Property name to find, possibly hierarchical.
     * @return unordered_map::const_iterator to the property or end().
     */
    AnyMap::const_iterator _find(std::string const& name) const;

    /*
     * Find the property name (possibly hierarchical) and set or replace its
     * value with the given vector of values.
     *
     * @param[in] name Property name to find, possibly hierarchical.
     * @param[in] vp shared_ptr to vector of values.
     * @throws InvalidParameterError Hierarchical name uses non-PropertySet.
     */
    virtual void _findOrInsert(std::string const& name, std::shared_ptr<std::vector<boost::any> > vp);
    void _cycleCheckPtrVec(std::vector<Ptr> const& v, std::string const& name);
    void _cycleCheckAnyVec(std::vector<boost::any> const& v, std::string const& name);
    void _cycleCheckPtr(Ptr const& v, std::string const& name);

    AnyMap _map;
    bool _flat;
};

#if defined(__ICC)
#pragma warning(pop)
#endif

template <>
void PropertySet::add<PropertySet::Ptr>(std::string const& name, Ptr const& value);
template <>
void PropertySet::add<PropertySet::Ptr>(std::string const& name, std::vector<Ptr> const& value);
}
}  // namespace daf
}  // namespace lsst

#endif

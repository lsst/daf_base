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

/** @file
  * @ingroup daf_base
  *
  * @brief Interface for PropertySet class
  *
  * @version $Revision$
  * @date $Date$
  */

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

#include <lsst/tr1/unordered_map.h>
#include <string>
#include <typeinfo>
#include <vector>

#include "boost/any.hpp"
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/Persistable.h"
#include "lsst/pex/exceptions.h"

namespace lsst {
namespace daf {

namespace persistence {
    class PropertySetFormatter;
} // namespace lsst::daf::persistence


namespace base {

#if defined(__ICC)
#pragma warning (push)
#pragma warning (disable: 444)
#endif

class PropertySet :
    public Persistable, public Citizen
#ifndef SWIG
    , public boost::noncopyable
#endif
    {
public:
// Typedefs
    typedef boost::shared_ptr<PropertySet> Ptr;
    typedef boost::shared_ptr<PropertySet const> ConstPtr;

// Constructors
    explicit PropertySet(bool flat=false);
    virtual ~PropertySet(void);

// Accessors
    virtual Ptr deepCopy(void) const;
    // Returns a PropertySet::Ptr pointing to a new deep copy.

    size_t nameCount(bool topLevelOnly = true) const;
    std::vector<std::string> names(bool topLevelOnly = true) const;
    std::vector<std::string> paramNames(bool topLevelOnly = true) const;
    std::vector<std::string> propertySetNames(bool topLevelOnly = true) const;

    bool exists(std::string const& name) const;
    bool isArray(std::string const& name) const;
    bool isPropertySetPtr(std::string const& name) const;

    size_t valueCount(std::string const& name) const;
    std::type_info const& typeOf(std::string const& name) const;
        // This returns typeof(vector::value_type), not the type of the value
        // vector itself.

    // The following throw an exception if the type does not match exactly.
    template <typename T> T get(std::string const& name) const;
        // Note that the type must be explicitly specified for this template:
        // int i = propertySet.get<int>("foo");
    template <typename T>
        T get(std::string const& name, T const& defaultValue) const;
        // Returns the provided default value if the name does not exist.
    template <typename T>
        std::vector<T> getArray(std::string const& name) const;

    // The following throw an exception if the conversion is inappropriate.
    bool getAsBool(std::string const& name) const;      // for bools only
    int getAsInt(std::string const& name) const;        // bool/char/short/int
    int64_t getAsInt64(std::string const& name) const;  // above + int64_t
    double getAsDouble(std::string const& name) const;  // + float, double
    std::string getAsString(std::string const& name) const; // for strings only
    PropertySet::Ptr getAsPropertySetPtr(std::string const& name) const;
    Persistable::Ptr getAsPersistablePtr(std::string const& name) const;

    // Use this for debugging, not for serialization/persistence.
    virtual std::string toString(bool topLevelOnly = false,
                    std::string const& indent = "") const;

// Modifiers
    template <typename T> void set(std::string const& name, T const& value);
    template <typename T> void set(std::string const& name,
                                   std::vector<T> const& value);
    void set(std::string const& name, char const* value);
    template <typename T> void add(std::string const& name, T const& value);
    template <typename T> void add(std::string const& name,
                                   std::vector<T> const& value);
    void add(std::string const& name, char const* value);

    virtual void copy(std::string const& dest, ConstPtr source,
                      std::string const& name);
    virtual void combine(ConstPtr source);
        // All vectors from the source are add()ed to the destination with the
        // same names.  Types must match.

    virtual void remove(std::string const& name);

protected:
    virtual void _set(std::string const& name,
                      boost::shared_ptr< std::vector<boost::any> > vp);
    virtual void _add(std::string const& name,
                      boost::shared_ptr< std::vector<boost::any> > vp);
    virtual std::string _format(std::string const& name) const;

private:
    LSST_PERSIST_FORMATTER(lsst::daf::persistence::PropertySetFormatter)

    typedef std::tr1::unordered_map<std::string,
            boost::shared_ptr< std::vector<boost::any> > > AnyMap;

    AnyMap::iterator _find(std::string const& name);
    AnyMap::const_iterator _find(std::string const& name) const;
    virtual void _findOrInsert(std::string const& name,
                      boost::shared_ptr< std::vector<boost::any> > vp);
    void _cycleCheckPtrVec(std::vector<Ptr> const& v, std::string const& name);
    void _cycleCheckAnyVec(std::vector<boost::any> const& v,
                          std::string const& name);
    void _cycleCheckPtr(Ptr const& v, std::string const& name);

    AnyMap _map;
    bool _flat;
};

#if defined(__ICC)
#pragma warning (pop)
#endif    

template<> void PropertySet::add<PropertySet::Ptr>(
    std::string const& name, Ptr const& value);
template<> void PropertySet::add<PropertySet::Ptr>(
    std::string const& name, std::vector<Ptr> const& value);

LSST_EXCEPTION_TYPE(TypeMismatchException,
                    lsst::pex::exceptions::LogicErrorException,
                    lsst::daf::base::TypeMismatchException)

}}} // namespace lsst::daf::base

#endif

// -*- lsst-c++ -*-
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
  * @brief Class for storing ordered generic metadata.
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
    class PropertyListFormatter;
} // namespace lsst::daf::persistence


namespace base {

class PropertyList :
    public Persistable, public Citizen, public boost::noncopyable {
public:
// Typedefs
    typedef boost::shared_ptr<PropertyList> Ptr;

// Constructors
    PropertyList(void);
    virtual ~PropertyList(void);

// Accessors
    Ptr deepCopy(void) const;
    // Returns a PropertyList::Ptr pointing to a new deep copy.

    size_t nameCount(void) const;
    std::vector<std::string> names(void) const;

    bool exists(std::string const& name) const;
    bool isArray(std::string const& name) const;

    size_t valueCount(std::string const& name) const;
    std::type_info const& typeOf(std::string const& name) const;
        // This returns typeof(vector::value_type), not the type of the value
        // vector itself.

    // The following throw an exception if the type does not match exactly.
    template <typename T> T get(std::string const& name) const;
        // Note that the type must be explicitly specified for this template:
        // int i = propertyList.get<int>("foo");
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

    std::string toFitsHeaders(void) const;

    // Use this for debugging, not for serialization/persistence.
    std::string toString(void) const;

// Modifiers
    template <typename T> void set(std::string const& name, T const& value);
    template <typename T> void set(std::string const& name,
                                   std::vector<T> const& value);
    void set(std::string const& name, char const* value);
    template <typename T> void add(std::string const& name, T const& value);
    template <typename T> void add(std::string const& name,
                                   std::vector<T> const& value);
    void add(std::string const& name, char const* value);

    void remove(std::string const& name);

private:
    LSST_PERSIST_FORMATTER(lsst::daf::persistence::PropertyListFormatter);

    typedef std::tr1::unordered_map<std::string,
            boost::shared_ptr< std::vector<boost::any> > > AnyMap;

    AnyMap _map;
    std::list<std::string> _insertOrder;
};

LSST_EXCEPTION_TYPE(TypeMismatchException,
                    lsst::pex::exceptions::LogicErrorException,
                    lsst::daf::base::TypeMismatchException)

}}} // namespace lsst::daf::base

#endif

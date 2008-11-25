// -*- lsst-c++ -*-
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
  * @ingroup daf_base
  */

#include <string>
#include <lsst/tr1/unordered_map.h>
#include <typeinfo>
#include <vector>

#include "boost/any.hpp"
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/Persistable.h"

namespace lsst {
namespace daf {

namespace persistence {
    class PropertySetFormatter;
} // namespace lsst::daf::persistence


namespace base {

using namespace std;

class PropertySet :
    public Persistable, public Citizen, public boost::noncopyable {
public:
// Typedefs
    typedef boost::shared_ptr<PropertySet> Ptr;

// Constructors
    PropertySet(void);
    virtual ~PropertySet(void);

// Accessors
    Ptr deepCopy(void) const;  // Returns a PropertySet::Ptr to a new deep copy.

    size_t nameCount(bool topLevelOnly = true) const;
    vector<string> names(bool topLevelOnly = true) const;
    vector<string> paramNames(bool topLevelOnly = true) const;
    vector<string> propertySetNames(bool topLevelOnly = true) const;

    bool exists(string const& name) const;
    bool isArray(string const& name) const;
    bool isPropertySetPtr(string const& name) const;

    size_t valueCount(string const& name) const;
    std::type_info const& typeOf(string const& name) const;
        // This returns typeof(vector::value_type), not the type of the value
        // vector itself.

    // The following throw an exception if the type does not match exactly.
    template <typename T> T get(string const& name) const;
        // Note that the type must be explicitly specified for this template:
        // int i = propertySet.get<int>("foo");
    template <typename T> T get(string const& name,
                                T const& defaultValue) const;
        // Returns the provided default value if the name does not exist.
    template <typename T> vector<T> const& getArray(string const& name) const;

    // The following throw an exception if the conversion is inappropriate.
    bool getAsBool(string const& name) const;      // for bools only
    int getAsInt(string const& name) const;        // bool, char, short, int
    int64_t getAsInt64(string const& name) const;  // above plus int64_t
    double getAsDouble(string const& name) const;  // above plus float, double
    string getAsString(string const& name) const;  // for strings only
    PropertySet::Ptr getAsPropertySetPtr(string const& name) const;
    Persistable::Ptr getAsPersistablePtr(string const& name) const;

    // Use this for debugging, not for serialization/persistence.
    string toString(bool topLevelOnly = false,
                    string const& indent = "") const;

// Modifiers
    template <typename T> void set(string const& name, T const& value);
    template <typename T> void set(string const& name, vector<T> const& value);
    template <typename T> void add(string const& name, T const& value);
    template <typename T> void add(string const& name, vector<T> const& value);
    void combine(Ptr const source);
        // All vectors from the source are add()ed to the destination with the
        // same names.  Types must match.

    void remove(string const& name);

private:
    LSST_PERSIST_FORMATTER(lsst::daf::persistence::PropertySetFormatter);

    typedef tr1::unordered_map<string, boost::any> AnyMap;
    AnyMap _map;
};

}}} // namespace lsst::daf::base

#endif

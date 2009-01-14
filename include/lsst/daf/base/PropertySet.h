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

#include <lsst/tr1/unordered_map.h>
#include <ostream>
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

LSST_EXCEPTION_TYPE(TypeMismatchException,
                    lsst::pex::exceptions::LogicErrorException,
                    lsst::daf::base::TypeMismatchException)

namespace detail {

struct ValBase {
    virtual ~ValBase(void) {};
    virtual ValBase* clone(void) = 0;
    virtual std::size_t size(void) const = 0;
    virtual std::type_info const& type(void) const = 0;
    virtual int getAsInt(std::string const& name) const {
        throw LSST_EXCEPT(TypeMismatchException, name);
    };
    virtual int64_t getAsInt64(std::string const& name) const {
        throw LSST_EXCEPT(TypeMismatchException, name);
    };
    virtual double getAsDouble(std::string const& name) const {
        throw LSST_EXCEPT(TypeMismatchException, name);
    };
    virtual void toStream(std::ostream& s) const = 0;
};

template <typename T> struct Val : ValBase {
    typedef std::vector<T> ValueType;
    explicit Val(ValueType const& value) : _value(value) { };
    virtual ValBase* clone(void) { return new Val(_value); };
    virtual std::size_t size(void) const { return _value.size(); };
    virtual std::type_info const& type(void) const { return typeid(T); };
    virtual int getAsInt(std::string const& name) const {
        T const& t = _value.back();
        if (static_cast<T>(static_cast<int>(t)) == t) return t;
        throw LSST_EXCEPT(TypeMismatchException, name);
    };
    virtual int64_t getAsInt64(std::string const& name) const {
        T const& t = _value.back();
        if (static_cast<T>(static_cast<int64_t>(t)) == t) return t;
        throw LSST_EXCEPT(TypeMismatchException, name);
    };
    virtual double getAsDouble(std::string const& name) const {
        T const& t = _value.back();
        if (static_cast<T>(static_cast<double>(t)) == t) return t;
        throw LSST_EXCEPT(TypeMismatchException, name);
    };
    virtual void toStream(std::ostream& s) const {
        if (_value.size() > 1) s << "[ ";
        for (ValueType::const_iterator k = _value.begin();
             k != _value.end(); ++k) {
            if (k != _value.begin()) s << ", ";
            s << *k;
        }
        if (_value.size() > 1) s << " ]";
    };

    ValueType _value;
};

struct AnyVal {
    AnyVal(void) : _value(0) { };
    AnyVal(AnyVal const& any) :
        _value((any._value == 0) ? 0 : any._value->clone()) { };
    template <typename T> explicit AnyVal(T const& value) :
        _value(new Val<T>(value)) { };
    ~AnyVal(void) {
        delete _value;
        _value = 0;
    };
    AnyVal& operator=(AnyVal const& any) {
        delete _value;
        _value = (any._value == 0) ? 0 : any._value->clone();
        return *this;
    };

    std::size_t size(void) const { return _value->size(); };
    std::type_info const& type(void) const { return _value->type(); };

    template <typename T> std::vector<T> const& getVec(void) const {
        assert(_value != 0);
        assert(typeid(T) == _value->type());
        return (static_cast<Val<T>*>(_value))->_value;
    };
    template <typename T> T const& getLast(void) const {
        assert(_value != 0);
        assert(typeid(T) == _value->type());
        return (static_cast<Val<T>*>(_value))->_value.back();
    };

    int getAsInt(std::string const& name) const {
        assert(_value != 0);
        return _value->getAsInt(name);
    };
    int64_t getAsInt64(std::string const& name) const {
        assert(_value != 0);
        return _value->getAsInt64(name);
    };
    double getAsDouble(std::string const& name) const {
        assert(_value != 0);
        return _value->getAsDouble(name);
    };
    void toStream(std::ostream& s) const {
        assert(_value != 0);
        return _value->toStream(s);
    };

    ValBase* _value;
};

template <typename T> std::vector<T> const& anyValVec(AnyVal const& any) {
    assert(typeid(T) == any._value->type());
    return (static_cast<Val<T>*>(any._value))->_value;
};

} // namespace detail

class PropertySet :
    public Persistable, public Citizen, public boost::noncopyable {
public:
// Typedefs
    typedef boost::shared_ptr<PropertySet> Ptr;

// Constructors
    PropertySet(void);
    virtual ~PropertySet(void);

// Accessors
    Ptr deepCopy(void) const;
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
    std::string toString(bool topLevelOnly = false,
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

    void combine(Ptr const source);
        // All vectors from the source are add()ed to the destination with the
        // same names.  Types must match.

    void remove(std::string const& name);

private:
    LSST_PERSIST_FORMATTER(lsst::daf::persistence::PropertySetFormatter);

    typedef std::tr1::unordered_map<std::string, detail::AnyVal> AnyMap;

    AnyMap::iterator find(std::string const& name);
    AnyMap::const_iterator find(std::string const& name) const;
    void findOrInsert(std::string const& name, detail::AnyVal vp);
    void cycleCheckPtrVec(std::vector<Ptr> const& v, std::string const& name);
    void cycleCheckAnyVec(std::vector<boost::any> const& v,
                          std::string const& name);
    void cycleCheckPtr(Ptr const& v, std::string const& name);

    AnyMap _map;
};

template<> void PropertySet::add<PropertySet::Ptr>(
    std::string const& name, Ptr const& value);
template<> void PropertySet::add<PropertySet::Ptr>(
    std::string const& name, std::vector<Ptr> const& value);

namespace detail {

std::ostream& operator<<(std::ostream& s, Persistable::Ptr p) {
    return s << "<Persistable>";
}

template<> struct Val<PropertySet::Ptr> : ValBase {
    typedef PropertySet::Ptr ValueType;
    explicit Val(ValueType const& value) : _value(value) { };
    virtual ValBase* clone(void) { return new Val(_value); };
    virtual std::size_t size(void) const { return 1; };
    virtual std::type_info const& type(void) const {
        return typeid(ValueType);
    };
    virtual void toStream(std::ostream& s) const { s << "<PropertySet>"; };

    ValueType _value;
};

template<>
PropertySet::Ptr const& AnyVal::getLast<PropertySet::Ptr>(void) const {
    assert(typeid(PropertySet::Ptr) == _value->type());
    return (static_cast<Val<PropertySet::Ptr>*>(_value))->_value;
}

} // namespace detail

}}} // namespace lsst::daf::base

#endif

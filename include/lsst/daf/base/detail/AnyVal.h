// -*- lsst-c++ -*-
#ifndef LSST_DAF_BASE_DETAIL_ANYVAL
#define LSST_DAF_BASE_DETAIL_ANYVAL

/** @file
  * @ingroup daf_base
  *
  * @brief Interface and inline implementation for detail::AnyVal class
  *
  * @version $Revision$
  * @date $Date$
  */

/** @class lsst::daf::base::detail::AnyVal
  * @brief Internal implementation class for storing vectors or
  * PropertySet::Ptrs.
  *
  * @ingroup daf_base
  */

#include <ostream>
#include <string>
#include <typeinfo>
#include <vector>

#include "boost/shared_ptr.hpp"
#include "lsst/daf/base/Persistable.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"

namespace lsst {
namespace daf {
namespace base {
namespace detail {

std::ostream& operator<<(std::ostream& s, Persistable::Ptr p) {
    return s << "<Persistable>";
}

class ValBase {
public:
    virtual ~ValBase(void) {};
    virtual ValBase* clone(void) const = 0;
    virtual std::size_t size(void) const = 0;
    virtual std::type_info const& type(void) const = 0;
    virtual int getAsInt(std::string const& name) const = 0;
    virtual int64_t getAsInt64(std::string const& name) const = 0;
    virtual double getAsDouble(std::string const& name) const = 0;
    virtual void toStream(std::ostream& s) const = 0;
    virtual void combine(AnyVal const& any) = 0;
};

template <typename T> class Val : public ValBase {
public:
    typedef std::vector<T> ValueType;
    explicit Val(ValueType const& value);
    virtual ValBase* clone(void) const;
    virtual std::size_t size(void) const;
    virtual std::type_info const& type(void) const;
    virtual int getAsInt(std::string const& name) const;
    virtual int64_t getAsInt64(std::string const& name) const;
    virtual double getAsDouble(std::string const& name) const;
    virtual void toStream(std::ostream& s) const;
    virtual void combine(AnyVal const& any);
    virtual ValueType const& getVec(void) const;
    virtual ValueType& getVec(void) ;
    virtual T getLast(void) const;

private:
    ValueType _value;
};

template<> class Val<PropertySet::Ptr> : public ValBase {
public:
    typedef PropertySet::Ptr ValueType;
    explicit Val(ValueType const& value);
    virtual ValBase* clone(void) const;
    virtual std::size_t size(void) const;
    virtual std::type_info const& type(void) const;
    virtual int getAsInt(std::string const& name) const;
    virtual int64_t getAsInt64(std::string const& name) const;
    virtual double getAsDouble(std::string const& name) const;
    virtual void toStream(std::ostream& s) const;
    virtual void combine(AnyVal const& any);
    virtual ValueType const& getPtr(void) const;

private:
    ValueType _value;
};

class AnyVal {
public:
    AnyVal(void) : _value(0) { };
    AnyVal(AnyVal const& any) :
        _value((any._value == 0) ? 0 : any._value->clone()) { };
    explicit AnyVal(PropertySet::Ptr const& value) :
        _value(new Val<PropertySet::Ptr>(value)) { };
    template <typename T> explicit AnyVal(std::vector<T> const& value) :
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
        return (static_cast<Val<T>*>(_value))->getVec();
    };
    template <typename T> std::vector<T>& getVec(void) {
        assert(_value != 0);
        assert(typeid(T) == _value->type());
        return (static_cast<Val<T>*>(_value))->getVec();
    };
    template <typename T> T getLast(void) const {
        assert(_value != 0);
        assert(typeid(T) == _value->type());
        return (static_cast<Val<T>*>(_value))->getLast();
    };
    PropertySet::Ptr const& getPtr(void) const {
        assert(_value != 0);
        assert(typeid(PropertySet::Ptr) == _value->type());
        return (static_cast< Val<PropertySet::Ptr>* >(_value))->getPtr();
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
    void combine(AnyVal const& any) {
        _value->combine(any);
    };

private:
    ValBase* _value;
};

///////////////////////////////////////////////////////////////////////////////
// Implementation for Val<T>
///////////////////////////////////////////////////////////////////////////////

template <typename T> Val<T>::Val(ValueType const& value) : _value(value) {
}

template <typename T> ValBase* Val<T>::clone(void) const {
    return new Val(_value);
}

template <typename T> std::size_t Val<T>::size(void) const {
    return _value.size();
}

template <typename T> std::type_info const& Val<T>::type(void) const {
    return typeid(T);
}

template <typename T> int Val<T>::getAsInt(std::string const& name) const {
    throw LSST_EXCEPT(TypeMismatchException, name);
}
template<> int Val<bool>::getAsInt(std::string const& name) const {
    return _value.back();
}
template<> int Val<char>::getAsInt(std::string const& name) const {
    return _value.back();
}
template<> int Val<signed char>::getAsInt(std::string const& name) const {
    return _value.back();
}
template<> int Val<unsigned char>::getAsInt(std::string const& name) const {
    return _value.back();
}
template<> int Val<short>::getAsInt(std::string const& name) const {
    return _value.back();
}
template<> int Val<unsigned short>::getAsInt(std::string const& name) const {
    return _value.back();
}
template<> int Val<int>::getAsInt(std::string const& name) const {
    return _value.back();
}

template <typename T> int64_t Val<T>::getAsInt64(std::string const& name) const {
    throw LSST_EXCEPT(TypeMismatchException, name);
}
template<> int64_t Val<bool>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<char>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<signed char>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<unsigned char>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<short>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<unsigned short>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<int>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<unsigned int>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<long>::getAsInt64(std::string const& name) const {
    return _value.back();
}
template<> int64_t Val<int64_t>::getAsInt64(std::string const& name) const {
    return _value.back();
}

template <typename T> double Val<T>::getAsDouble(std::string const& name) const {
    throw LSST_EXCEPT(TypeMismatchException, name);
}
template<> double Val<bool>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<char>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<signed char>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<unsigned char>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<short>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<unsigned short>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<int>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<unsigned int>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<long>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<unsigned long>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<int64_t>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<uint64_t>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<float>::getAsDouble(std::string const& name) const {
    return _value.back();
}
template<> double Val<double>::getAsDouble(std::string const& name) const {
    return _value.back();
}

template <typename T> void Val<T>::toStream(std::ostream& s) const {
    if (_value.size() > 1) s << "[ ";
    for (typename ValueType::const_iterator k = _value.begin();
         k != _value.end(); ++k) {
        if (k != _value.begin()) s << ", ";
        s << *k;
    }
    if (_value.size() > 1) s << " ]";
}

template<> void Val<char>::toStream(std::ostream& s) const {
    if (_value.size() > 1) s << "[ ";
    for (ValueType::const_iterator k = _value.begin(); k != _value.end(); ++k) {
        if (k != _value.begin()) s << ", ";
        s << '\'' << *k << '\'';
    }
    if (_value.size() > 1) s << " ]";
}
template<> void Val<signed char>::toStream(std::ostream& s) const {
    if (_value.size() > 1) s << "[ ";
    for (ValueType::const_iterator k = _value.begin(); k != _value.end(); ++k) {
        if (k != _value.begin()) s << ", ";
        s << '\'' << *k << '\'';
    }
    if (_value.size() > 1) s << " ]";
}
template<> void Val<unsigned char>::toStream(std::ostream& s) const {
    if (_value.size() > 1) s << "[ ";
    for (ValueType::const_iterator k = _value.begin(); k != _value.end(); ++k) {
        if (k != _value.begin()) s << ", ";
        s << '\'' << *k << '\'';
    }
    if (_value.size() > 1) s << " ]";
}

template<> void Val<std::string>::toStream(std::ostream& s) const {
    if (_value.size() > 1) s << "[ ";
    for (ValueType::const_iterator k = _value.begin(); k != _value.end(); ++k) {
        if (k != _value.begin()) s << ", ";
        s << '"' << *k << '"';
    }
    if (_value.size() > 1) s << " ]";
}

template <typename T> void Val<T>::combine(AnyVal const& any) {
    if (any.type() != typeid(T)) {
        throw LSST_EXCEPT(TypeMismatchException, "in combine");
    }
    std::vector<T> const& v(any.getVec<T>());
    _value.insert(_value.end(), v.begin(), v.end());
}

template <typename T> std::vector<T> const& Val<T>::getVec(void) const {
    return _value;
}

template <typename T> std::vector<T>& Val<T>::getVec(void) {
    return _value;
}

template <typename T> T Val<T>::getLast(void) const {
    return _value.back();
}

///////////////////////////////////////////////////////////////////////////////
// Implementation of specialization of Val<T> for PropertySet::Ptr
///////////////////////////////////////////////////////////////////////////////

Val<PropertySet::Ptr>::Val(ValueType const& value) : _value(value) {
}

ValBase* Val<PropertySet::Ptr>::clone(void) const {
    return new Val(_value);
}

std::size_t Val<PropertySet::Ptr>::size(void) const {
    return 1;
}

std::type_info const& Val<PropertySet::Ptr>::type(void) const {
    return typeid(ValueType);
}

int Val<PropertySet::Ptr>::getAsInt(std::string const& name) const {
    throw LSST_EXCEPT(TypeMismatchException, name);
}
int64_t Val<PropertySet::Ptr>::getAsInt64(std::string const& name) const {
    throw LSST_EXCEPT(TypeMismatchException, name);
}
double Val<PropertySet::Ptr>::getAsDouble(std::string const& name) const {
    throw LSST_EXCEPT(TypeMismatchException, name);
}

void Val<PropertySet::Ptr>::toStream(std::ostream& s) const {
    s << "<PropertySet>";
}

void Val<PropertySet::Ptr>::combine(AnyVal const& any) {
    throw LSST_EXCEPT(TypeMismatchException, "in combine");
}

PropertySet::Ptr const& Val<PropertySet::Ptr>::getPtr(void) const {
    return _value;
}

///////////////////////////////////////////////////////////////////////////////
// Specializations of AnyVal methods for PropertySet::Ptr
///////////////////////////////////////////////////////////////////////////////

template<> std::vector<PropertySet::Ptr> const&
AnyVal::getVec<PropertySet::Ptr>(void) const {
    throw LSST_EXCEPT(TypeMismatchException, "in getVec");
}

template<> std::vector<PropertySet::Ptr>&
AnyVal::getVec<PropertySet::Ptr>(void) {
    throw LSST_EXCEPT(TypeMismatchException, "in getVec");
}

template<> PropertySet::Ptr AnyVal::getLast<PropertySet::Ptr>(void) const {
    assert(_value != 0);
    assert(typeid(PropertySet::Ptr) == _value->type());
    return (static_cast<Val<PropertySet::Ptr>*>(_value))->getPtr();
}

}}}} // namespace lsst::daf::base::detail

#endif

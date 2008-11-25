/** @file
  * @ingroup daf_base
  *
  * @brief Implementation for PropertySet class
  *
  * @version $Revision$
  * @date $Date$
  */

#include "lsst/daf/base/PropertySet.h"

namespace dafBase = lsst::daf::base;

using namespace std;

dafBase::PropertySet::PropertySet(void) : Citizen(typeid(*this)) {
}

dafBase::PropertySet::~PropertySet(void) {
}

// Accessors

// Returns a PropertySet::Ptr to a new deep copy.
dafBase::PropertySet::Ptr dafBase::PropertySet::deepCopy(void) const {
    Ptr n(new PropertySet);
    return n;
}

size_t dafBase::PropertySet::nameCount(bool topLevelOnly) const {
    return 0;
}

vector<string> dafBase::PropertySet::names(bool topLevelOnly) const {
    vector<string> v;
    return v;
}

vector<string>
dafBase::PropertySet::paramNames(bool topLevelOnly) const {
    vector<string> v;
    return v;
}

vector<string>
dafBase::PropertySet::propertySetNames(bool topLevelOnly) const {
    vector<string> v;
    return v;
}

bool dafBase::PropertySet::exists(string const& name) const {
    return false;
}

bool dafBase::PropertySet::isArray(string const& name) const {
    return false;
}

bool dafBase::PropertySet::isPropertySetPtr(string const& name) const {
    return false;
}


size_t dafBase::PropertySet::valueCount(string const& name) const {
    return 0;
}

std::type_info const& dafBase::PropertySet::typeOf(string const& name) const {
    return typeid(*this);
}
        // This returns typeid(vector::value_type), not the type of the value
        // vector itself.

// The following throw an exception if the type does not match exactly.
template <typename T>
T const& dafBase::PropertySet::get(string const& name) const {
    static T t;
    return t;
}
// Note that the type must be explicitly specified for this template:
// int i = propertySet.get<int>("foo")

// Returns the provided default value if the name does not exist.
template <typename T>
T const& dafBase::PropertySet::get(string const& name,
                                   T const& defaultValue) const {
    return defaultValue;
}

template <typename T>
vector<T> const& dafBase::PropertySet::getArray(string const& name) const {
    static vector<T> v;
    return v;
}

// The following throw an exception if the conversion is inappropriate.
bool dafBase::PropertySet::getAsBool(string const& name) const {
    return false;
}      // for bools only

int dafBase::PropertySet::getAsInt(string const& name) const {
    return 0;
}        // bool, char, short, int

int64_t dafBase::PropertySet::getAsInt64(string const& name) const {
    return 0LL;
}  // above plus int64_t

double dafBase::PropertySet::getAsDouble(string const& name) const {
    return 0.0;
}  // above plus float, double

string dafBase::PropertySet::getAsString(string const& name) const {
    string s;
    return s;
}  // for strings only

dafBase::PropertySet::Ptr
dafBase::PropertySet::getAsPropertySetPtr(string const& name) const {
    return PropertySet::Ptr();
}

dafBase::Persistable::Ptr
dafBase::PropertySet::getAsPersistablePtr(string const& name) const {
    return Persistable::Ptr();
}

// Use this for debugging, not for serialization/persistence.
string dafBase::PropertySet::toString(bool topLevelOnly,
                                      string const& indent) const {
    string s;
    return s;
}

// Modifiers
template <typename T>
void dafBase::PropertySet::set(string const& name, T const& value) {
}

template <typename T>
void dafBase::PropertySet::set(string const& name, vector<T> const& value) {
}

template <typename T>
void dafBase::PropertySet::add(string const& name, T const& value) {
}

template <typename T>
void dafBase::PropertySet::add(string const& name, vector<T> const& value) {
}

// All vectors from the source are add()ed to the destination with the
// same names.  Types must match.
void dafBase::PropertySet::combine(Ptr const source) {
}

void dafBase::PropertySet::remove(string const& name) {
}


template bool const& dafBase::PropertySet::get<bool>(string const& name) const;
template short const& dafBase::PropertySet::get<short>(string const& name) const;
template int const& dafBase::PropertySet::get<int>(string const& name) const;
template long const& dafBase::PropertySet::get<long>(string const& name) const;
template long long const& dafBase::PropertySet::get<long long>(string const& name) const;
template float const& dafBase::PropertySet::get<float>(string const& name) const;
template double const& dafBase::PropertySet::get<double>(string const& name) const;
template string const& dafBase::PropertySet::get<string>(string const& name) const;
template dafBase::PropertySet::Ptr const& dafBase::PropertySet::get<dafBase::PropertySet::Ptr>(string const& name) const;
template dafBase::Persistable::Ptr const& dafBase::PropertySet::get<dafBase::Persistable::Ptr>(string const& name) const;

template bool const& dafBase::PropertySet::get<bool>(string const& name, bool const& defaultValue) const;
template short const& dafBase::PropertySet::get<short>(string const& name, short const& defaultValue) const;
template int const& dafBase::PropertySet::get<int>(string const& name, int const& defaultValue) const;
template long const& dafBase::PropertySet::get<long>(string const& name, long const& defaultValue) const;
template long long const& dafBase::PropertySet::get<long long>(string const& name, long long const& defaultValue) const;
template float const& dafBase::PropertySet::get<float>(string const& name, float const& defaultValue) const;
template double const& dafBase::PropertySet::get<double>(string const& name, double const& defaultValue) const;
template string const& dafBase::PropertySet::get<string>(string const& name, string const& defaultValue) const;
template dafBase::PropertySet::Ptr const& dafBase::PropertySet::get<dafBase::PropertySet::Ptr>(string const& name, PropertySet::Ptr const& defaultValue) const;
template dafBase::Persistable::Ptr const& dafBase::PropertySet::get<dafBase::Persistable::Ptr>(string const& name, Persistable::Ptr const& defaultValue) const;

template vector<bool> const& dafBase::PropertySet::getArray<bool>(string const& name) const;
template vector<short> const& dafBase::PropertySet::getArray<short>(string const& name) const;
template vector<int> const& dafBase::PropertySet::getArray<int>(string const& name) const;
template vector<long> const& dafBase::PropertySet::getArray<long>(string const& name) const;
template vector<long long> const& dafBase::PropertySet::getArray<long long>(string const& name) const;
template vector<float> const& dafBase::PropertySet::getArray<float>(string const& name) const;
template vector<double> const& dafBase::PropertySet::getArray<double>(string const& name) const;
template vector<string> const& dafBase::PropertySet::getArray<string>(string const& name) const;
template vector<dafBase::PropertySet::Ptr> const& dafBase::PropertySet::getArray<dafBase::PropertySet::Ptr>(string const& name) const;
template vector<dafBase::Persistable::Ptr> const& dafBase::PropertySet::getArray<dafBase::Persistable::Ptr>(string const& name) const;

template void dafBase::PropertySet::set<bool>(string const& name, bool const& value);
template void dafBase::PropertySet::set<short>(string const& name, short const& value);
template void dafBase::PropertySet::set<int>(string const& name, int const& value);
template void dafBase::PropertySet::set<long>(string const& name, long const& value);
template void dafBase::PropertySet::set<long long>(string const& name, long long const& value);
template void dafBase::PropertySet::set<float>(string const& name, float const& value);
template void dafBase::PropertySet::set<double>(string const& name, double const& value);
template void dafBase::PropertySet::set<string>(string const& name, string const& value);
template void dafBase::PropertySet::set<dafBase::PropertySet::Ptr>(string const& name, PropertySet::Ptr const& value);
template void dafBase::PropertySet::set<dafBase::Persistable::Ptr>(string const& name, Persistable::Ptr const& value);

template void dafBase::PropertySet::set<bool>(string const& name, vector<bool> const& value);
template void dafBase::PropertySet::set<short>(string const& name, vector<short> const& value);
template void dafBase::PropertySet::set<int>(string const& name, vector<int> const& value);
template void dafBase::PropertySet::set<long>(string const& name, vector<long> const& value);
template void dafBase::PropertySet::set<long long>(string const& name, vector<long long> const& value);
template void dafBase::PropertySet::set<float>(string const& name, vector<float> const& value);
template void dafBase::PropertySet::set<double>(string const& name, vector<double> const& value);
template void dafBase::PropertySet::set<string>(string const& name, vector<string> const& value);
template void dafBase::PropertySet::set<dafBase::PropertySet::Ptr>(string const& name, vector<PropertySet::Ptr> const& value);
template void dafBase::PropertySet::set<dafBase::Persistable::Ptr>(string const& name, vector<Persistable::Ptr> const& value);

template void dafBase::PropertySet::add<bool>(string const& name, bool const& value);
template void dafBase::PropertySet::add<short>(string const& name, short const& value);
template void dafBase::PropertySet::add<int>(string const& name, int const& value);
template void dafBase::PropertySet::add<long>(string const& name, long const& value);
template void dafBase::PropertySet::add<long long>(string const& name, long long const& value);
template void dafBase::PropertySet::add<float>(string const& name, float const& value);
template void dafBase::PropertySet::add<double>(string const& name, double const& value);
template void dafBase::PropertySet::add<string>(string const& name, string const& value);
template void dafBase::PropertySet::add<dafBase::PropertySet::Ptr>(string const& name, PropertySet::Ptr const& value);
template void dafBase::PropertySet::add<dafBase::Persistable::Ptr>(string const& name, Persistable::Ptr const& value);

template void dafBase::PropertySet::add<bool>(string const& name, vector<bool> const& value);
template void dafBase::PropertySet::add<short>(string const& name, vector<short> const& value);
template void dafBase::PropertySet::add<int>(string const& name, vector<int> const& value);
template void dafBase::PropertySet::add<long>(string const& name, vector<long> const& value);
template void dafBase::PropertySet::add<long long>(string const& name, vector<long long> const& value);
template void dafBase::PropertySet::add<float>(string const& name, vector<float> const& value);
template void dafBase::PropertySet::add<double>(string const& name, vector<double> const& value);
template void dafBase::PropertySet::add<string>(string const& name, vector<string> const& value);
template void dafBase::PropertySet::add<dafBase::PropertySet::Ptr>(string const& name, vector<PropertySet::Ptr> const& value);
template void dafBase::PropertySet::add<dafBase::Persistable::Ptr>(string const& name, vector<Persistable::Ptr> const& value);

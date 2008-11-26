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

#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace std;

dafBase::PropertySet::PropertySet(void) : Citizen(typeid(*this)) {
}

dafBase::PropertySet::~PropertySet(void) {
}

///////////////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////////////

// Returns a PropertySet::Ptr to a new deep copy.
dafBase::PropertySet::Ptr dafBase::PropertySet::deepCopy(void) const {
    Ptr n(new PropertySet);
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second->back().type() == typeid(Ptr)) {
            for (vector<boost::any>::const_iterator j =
                 i->second->begin(); j != i->second->end(); ++j) {
                n->add(i->first, boost::any_cast<Ptr>(*j)->deepCopy());
            }
        }
        else {
            boost::shared_ptr< vector<boost::any> > vp(
                new vector<boost::any>(*(i->second)));
            n->_map[i->first] = vp;
        }
    }
    return n;
}

size_t dafBase::PropertySet::nameCount(bool topLevelOnly) const {
    int n = 0;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        ++n;
        if (!topLevelOnly && i->second->back().type() == typeid(Ptr)) {
            n += boost::any_cast<Ptr>(i->second->back())->nameCount(false);
        }
    }
    return n;
}

vector<string> dafBase::PropertySet::names(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        v.push_back(i->first);
        if (!topLevelOnly && i->second->back().type() == typeid(Ptr)) {
            vector<string> w =
                boost::any_cast<Ptr>(i->second->back())->names(false);
            for (vector<string>::const_iterator k = w.begin();
                 k != w.end(); ++k) {
                v.push_back(i->first + "." + *k);
            }
        }
    }
    return v;
}

vector<string>
dafBase::PropertySet::paramNames(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second->back().type() == typeid(Ptr)) {
            if (!topLevelOnly) {
                vector<string> w =
                    boost::any_cast<Ptr>(i->second->back())->paramNames(false);
                for (vector<string>::const_iterator k = w.begin();
                     k != w.end(); ++k) {
                    v.push_back(i->first + "." + *k);
                }
            }
        }
        else {
            v.push_back(i->first);
        }
    }
    return v;
}

vector<string>
dafBase::PropertySet::propertySetNames(bool topLevelOnly) const {
    vector<string> v;
    for (AnyMap::const_iterator i = _map.begin(); i != _map.end(); ++i) {
        if (i->second->back().type() == typeid(Ptr)) {
            v.push_back(i->first);
            if (!topLevelOnly) {
                vector<string> w = boost::any_cast<Ptr>(i->second->back())->
                    propertySetNames(false);
                for (vector<string>::const_iterator k = w.begin();
                     k != w.end(); ++k) {
                    v.push_back(i->first + "." + *k);
                }
            }
        }
    }
    return v;
}

bool dafBase::PropertySet::exists(string const& name) const {
    return find(name) != _map.end();
}

bool dafBase::PropertySet::isArray(string const& name) const {
    AnyMap::const_iterator i = find(name);
    return i != _map.end() && i->second->size() > 1U;
}

bool dafBase::PropertySet::isPropertySetPtr(string const& name) const {
    AnyMap::const_iterator i = find(name);
    return i != _map.end() && i->second->back().type() == typeid(Ptr);
}


size_t dafBase::PropertySet::valueCount(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) return 0;
    return i->second->size();
}

// This returns typeid(vector::value_type), not the type of the value
// vector itself.
type_info const& dafBase::PropertySet::typeOf(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    return i->second->back().type();
}

// The following throw an exception if the type does not match exactly.

// Note that the type must be explicitly specified for this template:
// int i = propertySet.get<int>("foo")
template <typename T>
T dafBase::PropertySet::get(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    return boost::any_cast<T>(i->second->back());
}

// Returns the provided default value if the name does not exist.
template <typename T>
T dafBase::PropertySet::get(string const& name, T const& defaultValue) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        return defaultValue;
    }
    return boost::any_cast<T>(i->second->back());
}

template <typename T>
vector<T> dafBase::PropertySet::getArray(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    vector<T> v;
    for (vector<boost::any>::const_iterator j = i->second->begin();
         j != i->second->end(); ++j) {
        v.push_back(boost::any_cast<T>(*j));
    }
    return v;
}

// The following throw an exception if the conversion is inappropriate.

// for bools only
bool dafBase::PropertySet::getAsBool(string const& name) const {
    return get<bool>(name);
}

// bool, char, short, int
int dafBase::PropertySet::getAsInt(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) return boost::any_cast<bool>(v);
    if (t == typeid(char)) return boost::any_cast<char>(v);
    if (t == typeid(unsigned char)) return boost::any_cast<unsigned char>(v);
    if (t == typeid(short)) return boost::any_cast<short>(v);
    if (t == typeid(unsigned short)) return boost::any_cast<unsigned short>(v);
    return boost::any_cast<int>(v);
}

// above plus int64_t
int64_t dafBase::PropertySet::getAsInt64(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) return boost::any_cast<bool>(v);
    if (t == typeid(char)) return boost::any_cast<char>(v);
    if (t == typeid(unsigned char)) return boost::any_cast<unsigned char>(v);
    if (t == typeid(short)) return boost::any_cast<short>(v);
    if (t == typeid(unsigned short)) return boost::any_cast<unsigned short>(v);
    if (t == typeid(int)) return boost::any_cast<int>(v);
    if (t == typeid(unsigned int)) return boost::any_cast<unsigned int>(v);
    if (t == typeid(long)) return boost::any_cast<long>(v);
    return boost::any_cast<int64_t>(v);
}

// above plus float, double
double dafBase::PropertySet::getAsDouble(string const& name) const {
    AnyMap::const_iterator i = find(name);
    if (i == _map.end()) {
        throw runtime_error(name + " not found");
    }
    boost::any v = i->second->back();
    type_info const& t = v.type();
    if (t == typeid(bool)) return boost::any_cast<bool>(v);
    if (t == typeid(char)) return boost::any_cast<char>(v);
    if (t == typeid(unsigned char)) return boost::any_cast<unsigned char>(v);
    if (t == typeid(short)) return boost::any_cast<short>(v);
    if (t == typeid(unsigned short)) return boost::any_cast<unsigned short>(v);
    if (t == typeid(int)) return boost::any_cast<int>(v);
    if (t == typeid(unsigned int)) return boost::any_cast<unsigned int>(v);
    if (t == typeid(long)) return boost::any_cast<long>(v);
    if (t == typeid(unsigned long)) return boost::any_cast<unsigned long>(v);
    if (t == typeid(long long)) return boost::any_cast<long long>(v);
    if (t == typeid(unsigned long long)) return boost::any_cast<unsigned long long>(v);
    if (t == typeid(float)) return boost::any_cast<float>(v);
    return boost::any_cast<double>(v);
}

// for strings only
string dafBase::PropertySet::getAsString(string const& name) const {
    return get<string>(name);
}

dafBase::PropertySet::Ptr
dafBase::PropertySet::getAsPropertySetPtr(string const& name) const {
    return get<Ptr>(name);
}

dafBase::Persistable::Ptr
dafBase::PropertySet::getAsPersistablePtr(string const& name) const {
    return get<Persistable::Ptr>(name);
}

// Use this for debugging, not for serialization/persistence.
string dafBase::PropertySet::toString(bool topLevelOnly,
                                      string const& indent) const {
    ostringstream s;
    vector<string> nv = names();
    sort(nv.begin(), nv.end());
    for (vector<string>::const_iterator i = nv.begin(); i != nv.end(); ++i) {
        AnyMap::const_iterator j = _map.find(*i);
        s << indent << j->first << " = ";
        boost::shared_ptr< vector<boost::any> > vp = j->second;
        if (vp->size() > 1) s << "[ ";
        type_info const& t = vp->back().type();
        for (vector<boost::any>::const_iterator k = vp->begin();
             k != vp->end(); ++k) {
            if (k != vp->begin()) s << ", ";
            boost::any const& v(*k);
            if (t == typeid(bool)) s << boost::any_cast<bool>(v);
            else if (t == typeid(char)) s <<  boost::any_cast<char>(v);
            else if (t == typeid(unsigned char)) s << boost::any_cast<unsigned char>(v);
            else if (t == typeid(short)) s << boost::any_cast<short>(v);
            else if (t == typeid(unsigned short)) s << boost::any_cast<unsigned short>(v);
            else if (t == typeid(int)) s << boost::any_cast<int>(v);
            else if (t == typeid(unsigned int)) s << boost::any_cast<unsigned int>(v);
            else if (t == typeid(long)) s << boost::any_cast<long>(v);
            else if (t == typeid(unsigned long)) s << boost::any_cast<unsigned long>(v);
            else if (t == typeid(long long)) s << boost::any_cast<long long>(v);
            else if (t == typeid(unsigned long long)) s << boost::any_cast<unsigned long long>(v);
            else if (t == typeid(float)) s << boost::any_cast<float>(v);
            else if (t == typeid(double)) s << boost::any_cast<double>(v);
            else if (t == typeid(string)) s << '"' << boost::any_cast<string>(v) << '"';
            else if (t == typeid(Ptr) && !topLevelOnly) {
                s << '{' << std::endl;
                s << boost::any_cast<Ptr>(*k)->toString(false, indent + "..");
                s << indent << '}';
            }
            else if (t == typeid(Persistable::Ptr)) s << "<Persistable>";
            else s << "<Unknown>";
        }
        if (j->second->size() > 1) s << " ]";
        s << std::endl;
    }
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////
// Modifiers
///////////////////////////////////////////////////////////////////////////////

template <typename T>
void dafBase::PropertySet::set(string const& name, T const& value) {
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->push_back(value);
    findOrInsert(name, vp);
}

template <typename T>
void dafBase::PropertySet::set(string const& name, vector<T> const& value) {
    boost::shared_ptr< vector<boost::any> > vp(new vector<boost::any>);
    vp->insert(vp->end(), value.begin(), value.end());
    findOrInsert(name, vp);
}

void dafBase::PropertySet::set(string const& name, char const* value) {
    set(name, string(value));
}

template <typename T>
void dafBase::PropertySet::add(string const& name, T const& value) {
    AnyMap::iterator i = find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->at(0).type() != typeid(T)) {
            throw runtime_error(name + " mismatched type");
        }
        i->second->push_back(value);
    }
}

template <typename T>
void dafBase::PropertySet::add(string const& name, vector<T> const& value) {
    AnyMap::iterator i = find(name);
    if (i == _map.end()) {
        set(name, value);
    }
    else {
        if (i->second->at(0).type() != typeid(T)) {
            throw runtime_error(name + " mismatched type");
        }
        i->second->insert(i->second->end(), value.begin(), value.end());
    }
}

void dafBase::PropertySet::add(string const& name, char const* value) {
   add(name, string(value));
}

// All vectors from the source are add()ed to the destination with the
// same names.  Types must match.
void dafBase::PropertySet::combine(Ptr const source) {
    vector<string> names = source->paramNames(false);
    for (vector<string>::const_iterator i = names.begin();
         i != names.end(); ++i) {
        AnyMap::const_iterator sj = source->find(*i);
        AnyMap::const_iterator dj = find(*i);
        if (dj == _map.end()) {
            boost::shared_ptr< vector<boost::any> > vp(
                new vector<boost::any>(*(sj->second)));
            findOrInsert(*i, vp);
        }
        else {
            if (sj->second->back().type() != dj->second->back().type()) {
                throw boost::bad_any_cast();
            }
            dj->second->insert(dj->second->end(),
                               sj->second->begin(), sj->second->end());
        }
    }
}

void dafBase::PropertySet::remove(string const& name) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        _map.erase(name);
        return;
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return;
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    string suffix(name, i + 1);
    p->remove(suffix);
}

///////////////////////////////////////////////////////////////////////////////
// Private member functions
///////////////////////////////////////////////////////////////////////////////

dafBase::PropertySet::AnyMap::iterator
dafBase::PropertySet::find(string const& name) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    string suffix(name, i + 1);
    return p->find(suffix);
}

dafBase::PropertySet::AnyMap::const_iterator
dafBase::PropertySet::find(string const& name) const {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        return _map.find(name);
    }
    string prefix(name, 0, i);
    AnyMap::const_iterator j = _map.find(prefix);
    if (j == _map.end() || j->second->back().type() != typeid(Ptr)) {
        return _map.end();
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    string suffix(name, i + 1);
    return p->find(suffix);
}

void dafBase::PropertySet::findOrInsert(
    string const& name, boost::shared_ptr< vector<boost::any> > vp) {
    string::size_type i = name.find('.');
    if (i == name.npos) {
        _map[name] = vp;
        return;
    }
    string prefix(name, 0, i);
    AnyMap::iterator j = _map.find(prefix);
    if (j == _map.end()) {
        boost::shared_ptr< vector<boost::any> > temp(new vector<boost::any>);
        temp->push_back(PropertySet::Ptr(new PropertySet));
        _map[prefix] = temp;
        j = _map.find(prefix);
    }
    else if (j->second->back().type() != typeid(Ptr)) {
        throw runtime_error(prefix + " exists and is not PropertySet::Ptr");
    }
    Ptr p = boost::any_cast<Ptr>(j->second->back());
    string suffix(name, i + 1);
    p->findOrInsert(suffix, vp);
}

///////////////////////////////////////////////////////////////////////////////
// Explicit template instantiations
///////////////////////////////////////////////////////////////////////////////

/// @cond
// Explicit template instantiations are not well understood by doxygen.

#define INSTANTIATE(t) \
    template t dafBase::PropertySet::get<t>(string const& name) const; \
    template t dafBase::PropertySet::get<t>(string const& name, t const& defaultValue) const; \
    template vector<t> dafBase::PropertySet::getArray<t>(string const& name) const; \
    template void dafBase::PropertySet::set<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::set<t>(string const& name, vector<t> const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, t const& value); \
    template void dafBase::PropertySet::add<t>(string const& name, vector<t> const& value);

INSTANTIATE(bool)
INSTANTIATE(short)
INSTANTIATE(unsigned short)
INSTANTIATE(int)
INSTANTIATE(unsigned int)
INSTANTIATE(long)
INSTANTIATE(unsigned long)
INSTANTIATE(long long)
INSTANTIATE(unsigned long long)
INSTANTIATE(float)
INSTANTIATE(double)
INSTANTIATE(string)
INSTANTIATE(dafBase::PropertySet::Ptr)
INSTANTIATE(dafBase::Persistable::Ptr)

/// @endcond

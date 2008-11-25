#include "lsst/daf/base/PropertySet.h"

#define BOOST_TEST_MODULE Exception_1
#include "boost/test/included/unit_test.hpp"

namespace test = boost::test_tools;
namespace dafBase = lsst::daf::base;

BOOST_AUTO_TEST_SUITE(PropertySetSuite)

BOOST_AUTO_TEST_CASE(construct) {
    dafBase::PropertySet ps;
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    BOOST_CHECK_EQUAL(!psp, false);
}

BOOST_AUTO_TEST_CASE(bases) {
    dafBase::PropertySet::Ptr psp(new dafBase::PropertySet);
    boost::shared_ptr<dafBase::Persistable> pp =
        boost::dynamic_pointer_cast<dafBase::Persistable, dafBase::PropertySet>(psp);
    BOOST_CHECK_EQUAL(!pp, false);
    boost::shared_ptr<dafBase::Citizen> cp =
        boost::dynamic_pointer_cast<dafBase::Citizen, dafBase::PropertySet>(psp);
    BOOST_CHECK_EQUAL(!cp, false);
}

BOOST_AUTO_TEST_CASE(setScalar) {
    dafBase::PropertySet ps;
    ps.set("bool", true);
    short s = 42;
    ps.set("short", s);
    ps.set("int", 2008);
    ps.set("int64_t", 0xfeeddeadbeefLL);
    float f = 3.14159;
    ps.set("float", f);
    double d = 2.718281828459045;
    ps.set("double", d);
    ps.set<std::string>("char*", "foo");
    ps.set("string", std::string("bar"));
}

/*
    template <typename T> void set(string const& name, vector<T> const& value);
    template <typename T> void add(string const& name, T const& value);
    template <typename T> void add(string const& name, vector<T> const& value);
    void combine(Ptr const source);
        // All vectors from the source are add()ed to the destination with the
        // same names.  Types must match.

    void remove(string const& name);

    Ptr deepCopy(void) const;  // Returns a PropertySet::Ptr to a new deep copy.

    size_t nameCount(bool topLevelOnly = true) const;
    vector<string> names(bool topLevelOnly = true) const;
    vector<string> paramNames(bool topLevelOnly = true) const;
    vector<string> propertySetNames(bool topLevelOnly = true) const;

    bool exists(string const& name) const;
    bool isArray(string const& name) const;
    bool isPropertySetPtr(string const& name) const;

    size_t valueCount(string const& name) const;
    type_info typeOf(string const& name) const;
        // This returns typeof(vector::value_type), not the type of the value
        // vector itself.

    // The following throw an exception if the type does not match exactly.
    template <typename T> T const& get(string const& name) const;
        // Note that the type must be explicitly specified for this template:
        // int i = propertySet.get<int>("foo");
    template <typename T> T const& get(string const& name,
                                       T const& default) const;
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

*/

BOOST_AUTO_TEST_SUITE_END()

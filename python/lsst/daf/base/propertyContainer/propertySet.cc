#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <string>
#include <typeinfo>

#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertyList.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace daf {
namespace base {
namespace {

template <typename T, typename C>
void declareAccessors(C& cls, std::string const& name) {
    const std::string getName = "get" + name;
    cls.def(getName.c_str(), (T (PropertySet::*)(std::string const&) const) & PropertySet::get<T>, "name"_a);
    cls.def(getName.c_str(), (T (PropertySet::*)(std::string const&, T const&) const) & PropertySet::get<T>,
            "name"_a, "defaultValue"_a);

    const std::string getArrayName = "getArray" + name;
    cls.def(getArrayName.c_str(),
            (std::vector<T> (PropertySet::*)(std::string const&) const) & PropertySet::getArray<T>, "name"_a);

    const std::string setName = "set" + name;
    cls.def(setName.c_str(), (void (PropertySet::*)(std::string const&, T const&)) & PropertySet::set<T>,
            "name"_a, "value"_a);
    cls.def(setName.c_str(),
            (void (PropertySet::*)(std::string const&, std::vector<T> const&)) & PropertySet::set<T>,
            "name"_a, "value"_a);

    const std::string addName = "add" + name;
    cls.def(addName.c_str(), (void (PropertySet::*)(std::string const&, T const&)) & PropertySet::add<T>,
            "name"_a, "value"_a);
    cls.def(addName.c_str(),
            (void (PropertySet::*)(std::string const&, std::vector<T> const&)) & PropertySet::add<T>,
            "name"_a, "value"_a);

    const std::string typeName = "TYPE_" + name;
    cls.attr(typeName.c_str()) = py::cast(PropertySet::typeOfT<T>(), py::return_value_policy::reference);
}

}  // <anonymous>

PYBIND11_MODULE(propertySet, mod) {
    py::module::import("lsst.daf.base.persistable");

    py::class_<std::type_info>(mod, "TypeInfo")
            .def("__eq__",
                 [](std::type_info const& self, std::type_info const& other) { return self == other; })
            .def("__ne__",
                 [](std::type_info const& self, std::type_info const& other) { return self != other; })
            .def("name", &std::type_info::name)
            .def("__hash__", &std::type_info::hash_code);

    py::class_<PropertySet, std::shared_ptr<PropertySet>> cls(mod, "PropertySet");

    cls.def(py::init<bool>(), "flat"_a = false);
    cls.def(py::init<PropertyList const &>());  // n.b. implemented as conversion operator on PropertyList

    cls.def("deepCopy", &PropertySet::deepCopy);
    cls.def("nameCount", &PropertySet::nameCount, "topLevelOnly"_a = true);
    cls.def("names", &PropertySet::names, "topLevelOnly"_a = true);
    cls.def("paramNames", &PropertySet::paramNames, "topLevelOnly"_a = true);
    cls.def("propertySetNames", &PropertySet::propertySetNames, "topLevelOnly"_a = true);
    cls.def("exists", &PropertySet::exists);
    cls.def("isArray", &PropertySet::isArray);
    cls.def("isPropertySetPtr", &PropertySet::isPropertySetPtr);
    cls.def("valueCount", &PropertySet::valueCount);
    cls.def("typeOf", &PropertySet::typeOf, py::return_value_policy::reference);
    cls.def("toString", &PropertySet::toString, "topLevelOnly"_a = false, "indent"_a = "");
    cls.def("copy",
            py::overload_cast<std::string const &, PropertySet const &, std::string const &, bool>(
                &PropertySet::copy
            ),
            "dest"_a, "source"_a, "name"_a, "asScalar"_a=false);
    cls.def("combine", py::overload_cast<PropertySet const &>(&PropertySet::combine));
    cls.def("remove", &PropertySet::remove);
    cls.def("getAsBool", &PropertySet::getAsBool);
    cls.def("getAsInt", &PropertySet::getAsInt);
    cls.def("getAsInt64", &PropertySet::getAsInt64);
    cls.def("getAsDouble", &PropertySet::getAsDouble);
    cls.def("getAsString", &PropertySet::getAsString);
    cls.def("getAsPropertySetPtr", &PropertySet::getAsPropertySetPtr);
    cls.def("getAsPersistablePtr", &PropertySet::getAsPersistablePtr);

    declareAccessors<bool>(cls, "Bool");
    declareAccessors<short>(cls, "Short");
    declareAccessors<int>(cls, "Int");
    declareAccessors<long>(cls, "Long");
    declareAccessors<long long>(cls, "LongLong");
    declareAccessors<float>(cls, "Float");
    declareAccessors<double>(cls, "Double");
    declareAccessors<std::string>(cls, "String");
    declareAccessors<DateTime>(cls, "DateTime");
    declareAccessors<std::shared_ptr<PropertySet>>(cls, "PropertySet");
}

}  // base
}  // daf
}  // lsst

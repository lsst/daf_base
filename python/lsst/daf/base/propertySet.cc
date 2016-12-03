#include <string>
#include <typeinfo>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/daf/base/DateTime.h"

using namespace lsst::daf::base;

namespace py = pybind11;

template <typename T, typename C> void addAccessors(C &cls, std::string const& name) {
    const std::string getName = "get" + name;
    cls.def(getName.c_str(), (T (PropertySet::*)(std::string const&) const) &PropertySet::get<T>,
            py::arg("name"));
    cls.def(getName.c_str(), (T (PropertySet::*)(std::string const&, T const&) const) &PropertySet::get<T>,
            py::arg("name"), py::arg("defaultValue"));

    const std::string getArrayName = "getArray" + name;
    cls.def(getArrayName.c_str(), (std::vector<T> (PropertySet::*)(std::string const&) const) &PropertySet::getArray<T>,
            py::arg("name"));

    const std::string setName = "set" + name;
    cls.def(setName.c_str(), (void (PropertySet::*)(std::string const&, T const&)) &PropertySet::set<T>,
            py::arg("name"), py::arg("value"));
    cls.def(setName.c_str(), (void (PropertySet::*)(std::string const&, std::vector<T> const&)) &PropertySet::set<T>,
            py::arg("name"), py::arg("value"));

    const std::string addName = "add" + name;
    cls.def(addName.c_str(), (void (PropertySet::*)(std::string const&, T const&)) &PropertySet::add<T>,
            py::arg("name"), py::arg("value"));
    cls.def(addName.c_str(), (void (PropertySet::*)(std::string const&, std::vector<T> const&)) &PropertySet::add<T>,
            py::arg("name"), py::arg("value"));

    const std::string typeName = "TYPE_" + name;
    cls.attr(typeName.c_str()) = py::cast(typeid(T), py::return_value_policy::reference);
}

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_propertySet) {
    py::module mod("_propertySet", "Access to the classes from the daf_base propertySet library");

    py::class_<std::type_info>(mod, "TypeInfo")
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::class_<PropertySet, std::shared_ptr<PropertySet>, Persistable, Citizen> cls(mod, "PropertySet");

    cls.def(py::init<bool>(), py::arg("flat")=false);

    cls.def("deepCopy", &PropertySet::deepCopy);
    cls.def("nameCount", &PropertySet::nameCount, py::arg("topLevelOnly")=true);
    cls.def("names", &PropertySet::names, py::arg("topLevelOnly")=true);
    cls.def("paramNames", &PropertySet::paramNames, py::arg("topLevelOnly")=true);
    cls.def("propertySetNames", &PropertySet::propertySetNames, py::arg("topLevelOnly")=true);
    cls.def("exists", &PropertySet::exists);
    cls.def("isArray", &PropertySet::isArray);
    cls.def("isPropertySetPtr", &PropertySet::isPropertySetPtr);
    cls.def("valueCount", &PropertySet::valueCount);
    cls.def("typeOf", &PropertySet::typeOf, py::return_value_policy::reference);
    cls.def("toString", &PropertySet::toString, py::arg("topLevelOnly")=false, py::arg("indent")="");
    cls.def("copy", &PropertySet::copy);
    cls.def("combine", &PropertySet::combine);
    cls.def("remove", &PropertySet::remove);

    cls.def("getAsPropertySetPtr", &PropertySet::getAsPropertySetPtr);

    addAccessors<bool>(cls, "Bool");
    addAccessors<short>(cls, "Short");
    addAccessors<int>(cls, "Int");
    addAccessors<long>(cls, "Long");
    addAccessors<long long>(cls, "LongLong");
    addAccessors<float>(cls, "Float");
    addAccessors<double>(cls, "Double");
    addAccessors<std::string>(cls, "String");
    addAccessors<DateTime>(cls, "DateTime");
    addAccessors<std::shared_ptr<PropertySet>>(cls, "PropertySet");

    return mod.ptr();
}


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/PropertyList.h"
#include "lsst/daf/base/DateTime.h"

using namespace lsst::daf::base;

namespace py = pybind11;

template <typename T, typename C> void addAccessors(C &cls, std::string const& name) {
    const std::string getName = "get" + name;
    cls.def(getName.c_str(), (T (PropertyList::*)(std::string const&) const) &PropertyList::get<T>,
            py::arg("name"));
    cls.def(getName.c_str(), (T (PropertyList::*)(std::string const&, T const&) const) &PropertyList::get<T>,
            py::arg("name"), py::arg("defaultValue"));

    const std::string getArrayName = "getArray" + name;
    cls.def(getArrayName.c_str(), (std::vector<T> (PropertyList::*)(std::string const&) const) &PropertyList::getArray<T>,
            py::arg("name"));

    const std::string setName = "set" + name;
    cls.def(setName.c_str(), (void (PropertyList::*)(std::string const&, T const&)) &PropertyList::set<T>);
    cls.def(setName.c_str(), (void (PropertyList::*)(std::string const&, std::vector<T> const&)) &PropertyList::set<T>);
    cls.def(setName.c_str(), (void (PropertyList::*)(std::string const&, T const&, std::string const&)) &PropertyList::set<T>);
    cls.def(setName.c_str(), (void (PropertyList::*)(std::string const&, std::vector<T> const&, std::string const&)) &PropertyList::set<T>);

    const std::string addName = "add" + name;
    cls.def(addName.c_str(), (void (PropertyList::*)(std::string const&, T const&)) &PropertyList::add<T>);
    cls.def(addName.c_str(), (void (PropertyList::*)(std::string const&, std::vector<T> const&)) &PropertyList::add<T>);
    cls.def(addName.c_str(), (void (PropertyList::*)(std::string const&, T const&, std::string const&)) &PropertyList::add<T>);
    cls.def(addName.c_str(), (void (PropertyList::*)(std::string const&, std::vector<T> const&, std::string const&)) &PropertyList::add<T>);

    const std::string typeName = "TYPE_" + name;
    cls.attr(typeName.c_str()) = py::cast(typeid(T), py::return_value_policy::reference);
}

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_propertyList) {
    py::module mod("_propertyList", "Access to the classes from the daf_base propertyList library");

    py::class_<PropertyList, std::shared_ptr<PropertyList>, PropertySet, Citizen> cls(mod, "PropertyList");

    cls.def(py::init<>());

    cls.def("getComment", &PropertyList::getComment);
    cls.def("getOrderedNames", &PropertyList::getOrderedNames);
    cls.def("deepCopy", [](PropertyList &pl){return std::static_pointer_cast<PropertySet>(pl.deepCopy());});

    /* __getstate__ and __setstate__ implement support for pickling (protocol version 2)
     * 
     * They are most easily implemented in Python because the container can hold many different types.
     * However, implementing __setstate__ in Python leads pickle to create a new instance by calling
     * object.__new__(PropertyList, *args) which bypasses the pybind11 memory allocation step and hence
     * leads to segfaults. Thus, __setstate__ first calls the C++ constructor and then calls back to
     * Python to do the remaining initialization. Note that __getstate__ is mainly implemented in the
     * same way for clarity, but not strictly needed (I think).
     */
    cls.def("__getstate__", [](const PropertyList &pl) -> py::object {
        static py::object module{PyImport_ImportModule("lsst.daf.base.baseLib"), true};
        if (!module.ptr()) {
            throw py::error_already_set();
        } else {
            static py::object func{PyObject_GetAttrString(module.ptr(), "getstate"), true};
            if (!func.ptr()) {
                throw py::error_already_set();
            } else {
                py::object self = py::cast(pl);
                py::object result{PyObject_CallFunctionObjArgs(func.ptr(), self.ptr(), NULL), false};
                return result;
            }
        }
        return py::none{};
    });
    cls.def("__setstate__", [](PropertyList &pl, py::object state) {
        /* Invoke the in-place constructor. Note that this is needed even
           when the object just has a trivial default constructor */
        new (&pl) PropertyList();

        static py::object module{PyImport_ImportModule("lsst.daf.base.baseLib"), true};
        if (!module.ptr()) {
            throw py::error_already_set();
        } else {
            static py::object func{PyObject_GetAttrString(module.ptr(), "setstate"), true};
            if (!func.ptr()) {
                throw py::error_already_set();
            } else {
                py::object self = py::cast(pl);
                py::object result{PyObject_CallFunctionObjArgs(func.ptr(), self.ptr(), state.ptr(), NULL), false};
            }
        }
    });

    addAccessors<bool>(cls, "Bool");
    addAccessors<short>(cls, "Short");
    addAccessors<int>(cls, "Int");
    addAccessors<long>(cls, "Long");
    addAccessors<long long>(cls, "LongLong");
    addAccessors<float>(cls, "Float");
    addAccessors<double>(cls, "Double");
    addAccessors<std::string>(cls, "String");
    addAccessors<DateTime>(cls, "DateTime");

    cls.def("setPropertySet", (void (PropertyList::*)(std::string const&, PropertySet::Ptr const&)) &PropertyList::set);

    return mod.ptr();
}


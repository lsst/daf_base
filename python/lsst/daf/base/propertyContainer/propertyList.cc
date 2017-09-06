#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/PropertyList.h"
#include "lsst/daf/base/DateTime.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace daf {
namespace base {
namespace {

template <typename T, typename C>
void declareAccessors(C& cls, std::string const& name) {
    const std::string getName = "get" + name;
    cls.def(getName.c_str(), (T (PropertyList::*)(std::string const&) const) & PropertyList::get<T>,
            "name"_a);
    cls.def(getName.c_str(), (T (PropertyList::*)(std::string const&, T const&) const) & PropertyList::get<T>,
            "name"_a, "defaultValue"_a);

    // Warning: __len__ is ambiguous so do not attempt to define it. It could return
    // the number of unique names or the number of entries (e.g. as returned by toList,
    // a pure Python method). C++ begin and end iterate over unique names, but users often
    // view PropertyList as a representation of a FITS header. When in doubt, refuse to guess.

    const std::string getArrayName = "getArray" + name;
    cls.def(getArrayName.c_str(),
            (std::vector<T> (PropertyList::*)(std::string const&) const) & PropertyList::getArray<T>,
            "name"_a);

    const std::string setName = "set" + name;
    cls.def(setName.c_str(), (void (PropertyList::*)(std::string const&, T const&)) & PropertyList::set<T>);
    cls.def(setName.c_str(),
            (void (PropertyList::*)(std::string const&, std::vector<T> const&)) & PropertyList::set<T>);
    cls.def(setName.c_str(), (void (PropertyList::*)(std::string const&, T const&, std::string const&)) &
                                     PropertyList::set<T>);
    cls.def(setName.c_str(),
            (void (PropertyList::*)(std::string const&, std::vector<T> const&, std::string const&)) &
                    PropertyList::set<T>);

    const std::string addName = "add" + name;
    cls.def(addName.c_str(), (void (PropertyList::*)(std::string const&, T const&)) & PropertyList::add<T>);
    cls.def(addName.c_str(),
            (void (PropertyList::*)(std::string const&, std::vector<T> const&)) & PropertyList::add<T>);
    cls.def(addName.c_str(), (void (PropertyList::*)(std::string const&, T const&, std::string const&)) &
                                     PropertyList::add<T>);
    cls.def(addName.c_str(),
            (void (PropertyList::*)(std::string const&, std::vector<T> const&, std::string const&)) &
                    PropertyList::add<T>);

    const std::string typeName = "TYPE_" + name;
    cls.attr(typeName.c_str()) = py::cast(typeid(T), py::return_value_policy::reference);
}

}  // <anonymous>

PYBIND11_PLUGIN(propertyList) {
    py::module::import("lsst.daf.base.persistable");

    py::module mod("propertyList");

    py::class_<PropertyList, std::shared_ptr<PropertyList>, PropertySet, Citizen> cls(mod, "PropertyList");

    cls.def(py::init<>());

    cls.def("getComment", &PropertyList::getComment);
    cls.def("getOrderedNames", &PropertyList::getOrderedNames);
    cls.def("deepCopy",
            [](PropertyList const& self) { return std::static_pointer_cast<PropertySet>(self.deepCopy()); });

    /* __getstate__ and __setstate__ implement support for pickling (protocol version 2)
     *
     * They are most easily implemented in Python because the container can hold many different types.
     * However, implementing __setstate__ in Python leads pickle to create a new instance by calling
     * object.__new__(PropertyList, *args) which bypasses the pybind11 memory allocation step and hence
     * leads to segfaults. Thus, __setstate__ first calls the C++ constructor and then calls back to
     * Python to do the remaining initialization. Note that __getstate__ is mainly implemented in the
     * same way for clarity, but not strictly needed (I think).
     */
    cls.def("__getstate__", [](PropertyList const& self) -> py::object {
        auto module =
                py::reinterpret_borrow<py::object>(PyImport_ImportModule("lsst.daf.base.propertyContainer"));
        if (!module.ptr()) {
            throw py::error_already_set();
        } else {
            auto func = py::reinterpret_borrow<py::object>(PyObject_GetAttrString(module.ptr(), "getstate"));
            if (!func.ptr()) {
                throw py::error_already_set();
            } else {
                auto pySelf = py::cast(self);
                auto result = py::reinterpret_steal<py::object>(
                        PyObject_CallFunctionObjArgs(func.ptr(), pySelf.ptr(), NULL));
                return result;
            }
        }
        return py::none{};
    });
    cls.def("__setstate__", [](PropertyList& self, py::object state) {
        /* Invoke the in-place constructor. Note that this is needed even
           when the object just has a trivial default constructor */
        new (&self) PropertyList();

        auto module =
                py::reinterpret_borrow<py::object>(PyImport_ImportModule("lsst.daf.base.propertyContainer"));
        if (!module.ptr()) {
            throw py::error_already_set();
        } else {
            auto func = py::reinterpret_borrow<py::object>(PyObject_GetAttrString(module.ptr(), "setstate"));
            if (!func.ptr()) {
                throw py::error_already_set();
            } else {
                auto pySelf = py::cast(self, py::return_value_policy::reference);
                auto result = py::reinterpret_steal<py::object>(
                        PyObject_CallFunctionObjArgs(func.ptr(), pySelf.ptr(), state.ptr(), NULL));
            }
        }
    });

    declareAccessors<bool>(cls, "Bool");
    declareAccessors<short>(cls, "Short");
    declareAccessors<int>(cls, "Int");
    declareAccessors<long>(cls, "Long");
    declareAccessors<long long>(cls, "LongLong");
    declareAccessors<float>(cls, "Float");
    declareAccessors<double>(cls, "Double");
    declareAccessors<std::string>(cls, "String");
    declareAccessors<DateTime>(cls, "DateTime");

    cls.def("setPropertySet",
            (void (PropertyList::*)(std::string const&, PropertySet::Ptr const&)) & PropertyList::set);

    return mod.ptr();
}

}  // base
}  // daf
}  // lsst

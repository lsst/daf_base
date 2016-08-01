#include <string>

#include <pybind11/pybind11.h>

#include "lsst/daf/base/Persistable.h"

using namespace lsst::daf::base;

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(MyType, std::shared_ptr<MyType>);

PYBIND11_PLUGIN(_persistable) {
    py::module mod("_persistable", "Access to the classes from the daf_base persistable library");

    py::class_<Persistable, std::shared_ptr<Persistable>> cls(mod, "Persistable");

    return mod.ptr();
}


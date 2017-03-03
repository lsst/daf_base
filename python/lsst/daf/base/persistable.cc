#include "pybind11/pybind11.h"

#include <string>

#include "lsst/daf/base/Persistable.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace daf {
namespace base {

PYBIND11_PLUGIN(persistable) {
    py::module mod("persistable", "Access to the classes from the daf_base persistable library");

    py::class_<Persistable, std::shared_ptr<Persistable>> cls(mod, "Persistable");

    return mod.ptr();
}

}  // base
}  // daf
}  // lsst


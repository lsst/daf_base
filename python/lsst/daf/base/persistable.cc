#include "pybind11/pybind11.h"

#include <string>

#include "lsst/daf/base/Persistable.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace daf {
namespace base {

PYBIND11_MODULE(persistable, mod) {
    py::class_<Persistable, std::shared_ptr<Persistable>> cls(mod, "Persistable");
}

}  // base
}  // daf
}  // lsst

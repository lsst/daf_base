#include "pybind11/pybind11.h"
#include "lsst/daf/base/Persistable.h"

#include "lsst/cpputils/python.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace daf {
namespace base {
void wrapPersistable(lsst::cpputils::python::WrapperCollection &wrappers) {
    using PyPersistable = py::class_<Persistable, std::shared_ptr<Persistable>>;
    wrappers.wrapType(PyPersistable(wrappers.module,  "Persistable"), [] (auto &mod, auto &cls) {
    });
}

}  // base
}  // daf
}  // lsst

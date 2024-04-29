#include "lsst/daf/base/Persistable.h"

#include "lsst/cpputils/python.h"

namespace nb = nanobind;
using namespace nanobind::literals;

namespace lsst {
namespace daf {
namespace base {
void wrapPersistable(lsst::cpputils::python::WrapperCollection &wrappers) {
    using PyPersistable = nb::class_<Persistable>;
    wrappers.wrapType(PyPersistable(wrappers.module,  "Persistable"), [] (auto &mod, auto &cls) {
    });
}

}  // base
}  // daf
}  // lsst

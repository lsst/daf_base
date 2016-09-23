#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "lsst/daf/base/Citizen.h"

using namespace lsst::daf::base;

namespace py = pybind11;

PYBIND11_PLUGIN(_citizen) {
    py::module mod("_citizen", "Access to the classes from the daf_base citizen library");

    py::class_<Citizen> cls(mod, "Citizen");

    cls.def_static("census", (int (*)(int, Citizen::memId)) &Citizen::census);
    /* It is strange to expose cout to Python, so instead make a new wrapper */
    cls.def_static("census", [](Citizen::memId id) { Citizen::census(std::cout, id); });
    cls.def_static("getNextMemId", &Citizen::getNextMemId);
    cls.def_static("setNewCallbackId", &Citizen::setNewCallbackId);
    cls.def_static("setDeleteCallbackId", &Citizen::setDeleteCallbackId);

    return mod.ptr();
}


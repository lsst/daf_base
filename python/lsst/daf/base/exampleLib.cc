#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "lsst/daf/base/DateTime.h"

using namespace lsst::daf::base;

namespace py = pybind11;

PYBIND11_PLUGIN(_exampleLib) {
    py::module mod("_exampleLib", "example baseLib module with pybind11");

    py::class_<DateTime> cls(mod, "DateTime");

    py::enum_<DateTime::Timescale>(cls, "Timescale")
        .value("TAI", DateTime::Timescale::TAI)
        .value("UTC", DateTime::Timescale::UTC)
        .value("TT", DateTime::Timescale::TT)
        .export_values();

    py::enum_<DateTime::DateSystem>(cls, "DateSystem")
        .value("JD", DateTime::DateSystem::JD)
        .value("MJD", DateTime::DateSystem::MJD)
        .value("EPOCH", DateTime::DateSystem::EPOCH)
        .export_values();

    cls.def(py::init<long long, DateTime::Timescale>(),
            py::arg("nsecs") = 0LL, py::arg("scale") = DateTime::TAI)
        .def(py::init<long long, DateTime::Timescale>())
        .def(py::init<double, DateTime::DateSystem, DateTime::Timescale>())
        .def(py::init<int, int, int, int, int, int, DateTime::Timescale>())
        .def(py::init<const std::string &>())
        .def("nsecs", &DateTime::nsecs, py::arg("scale") = DateTime::TAI)
        .def("get", &DateTime::get)
        .def("toString", &DateTime::toString)
        .def("gmtime", &DateTime::gmtime)
        .def("timespec", &DateTime::timespec)
        .def("timeval", &DateTime::timeval)
        .def_static("now", &DateTime::now)
        .def_static("initializeLeapSeconds", &DateTime::initializeLeapSeconds)
        .def("__repr__", &DateTime::toString)
        .def(py::self == py::self);

    return mod.ptr();
}


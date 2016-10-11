#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "lsst/daf/base/DateTime.h"

using namespace lsst::daf::base;

namespace py = pybind11;

// invalid_nsecs is odr used but has an in-class initializer
constexpr long long DateTime::invalid_nsecs;

PYBIND11_PLUGIN(_dateTime) {
    py::module mod("_dateTime", "Access to the classes from the daf_base dateTime library");

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

    cls.def(py::init<>())
        .def_readonly_static("invalid_nsecs", &DateTime::invalid_nsecs)
        .def(py::init<long long, DateTime::Timescale>(),
             py::arg("nsecs"), py::arg("scale") = DateTime::Timescale::TAI)
        .def(py::init<double, DateTime::DateSystem, DateTime::Timescale>(),
             py::arg("date"), py::arg("system")=DateTime::DateSystem::MJD, py::arg("scale")=DateTime::Timescale::TAI)
        .def(py::init<int, int, int, int, int, int, DateTime::Timescale>())
        .def(py::init<const std::string &, DateTime::Timescale>())
        .def("nsecs", &DateTime::nsecs, py::arg("scale") = DateTime::Timescale::TAI)
        .def("get", &DateTime::get,
            py::arg("system") = DateTime::DateSystem::MJD,
            py::arg("scale") = DateTime::Timescale::TAI)
        .def("toString", &DateTime::toString)
        .def("gmtime", &DateTime::gmtime)
        .def("timespec", &DateTime::timespec)
        .def("timeval", &DateTime::timeval)
        .def("isValid", &DateTime::isValid)
        .def_static("now", &DateTime::now)
        .def_static("initializeLeapSeconds", &DateTime::initializeLeapSeconds)
        .def(py::self == py::self)
        // Support pickling
        .def("__getstate__", [](const DateTime &d) {
            return py::make_tuple(d.nsecs());
        })
        .def("__setstate__", [](DateTime &d, py::tuple t) {
            if (t.size() != 1)
                throw std::runtime_error("Invalid state!");

            /* Invoke the in-place constructor. Note that this is needed even
               when the object just has a trivial default constructor */
            new (&d) DateTime(t[0].cast<long long>());
        });

    return mod.ptr();
}


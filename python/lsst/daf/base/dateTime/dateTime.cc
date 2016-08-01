#include "pybind11/pybind11.h"

#include "lsst/daf/base/DateTime.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace daf {
namespace base {

PYBIND11_PLUGIN(dateTime) {
    py::module mod("dateTime");

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
            .def(py::init<long long, DateTime::Timescale>(), "nsecs"_a, "scale"_a = DateTime::Timescale::TAI)
            .def(py::init<double, DateTime::DateSystem, DateTime::Timescale>(), "date"_a,
                 "system"_a = DateTime::DateSystem::MJD, "scale"_a = DateTime::Timescale::TAI)
            .def(py::init<int, int, int, int, int, int, DateTime::Timescale>())
            .def(py::init<const std::string &, DateTime::Timescale>())
            .def("nsecs", &DateTime::nsecs, "scale"_a = DateTime::Timescale::TAI)
            .def("get", &DateTime::get, "system"_a = DateTime::DateSystem::MJD,
                 "scale"_a = DateTime::Timescale::TAI)
            .def("toString", &DateTime::toString)
            .def("gmtime", &DateTime::gmtime)
            .def("timespec", &DateTime::timespec)
            .def("timeval", &DateTime::timeval)
            .def("isValid", &DateTime::isValid)
            .def_static("now", &DateTime::now)
            .def_static("initializeLeapSeconds", &DateTime::initializeLeapSeconds)
            .def("__eq__", [](DateTime const &self, DateTime const &other) { return self == other; },
                 py::is_operator());

    return mod.ptr();
}

}  // base
}  // daf
}  // lsst

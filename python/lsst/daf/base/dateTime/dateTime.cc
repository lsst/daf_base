#include "pybind11/pybind11.h"
#include "lsst/cpputils/python.h"

#include "lsst/daf/base/DateTime.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace daf {
namespace base {
void wrapDateTime(lsst::cpputils::python::WrapperCollection &wrappers)  {
     auto clsDef = wrappers.wrapType(py::class_<DateTime>(wrappers.module, "DateTime"), [](auto &mod, auto &cls) {
         cls.def(py::init<>());
         cls.def_readonly_static("invalid_nsecs", &DateTime::invalid_nsecs);
         cls.def(py::init<long long, DateTime::Timescale>(), "nsecs"_a, "scale"_a = DateTime::Timescale::TAI);
         cls.def(py::init<double, DateTime::DateSystem, DateTime::Timescale>(), "date"_a,
                 "system"_a = DateTime::DateSystem::MJD, "scale"_a = DateTime::Timescale::TAI);
         cls.def(py::init<int, int, int, int, int, int, DateTime::Timescale>());
         cls.def(py::init<const std::string &, DateTime::Timescale>());
         cls.def("nsecs", &DateTime::nsecs, "scale"_a = DateTime::Timescale::TAI);
         cls.def("get", &DateTime::get, "system"_a = DateTime::DateSystem::MJD,
                 "scale"_a = DateTime::Timescale::TAI);
         cls.def("toString", &DateTime::toString);
         cls.def("gmtime", &DateTime::gmtime);
         cls.def("timespec", &DateTime::timespec);
         cls.def("timeval", &DateTime::timeval);
         cls.def("isValid", &DateTime::isValid);
         cls.def_static("now", &DateTime::now);
         cls.def_static("initializeLeapSeconds", &DateTime::initializeLeapSeconds);
         cls.def("__eq__", [](DateTime const &self, DateTime const &other) { return self == other; },
                 py::is_operator());
     });

     wrappers.wrapType(py::enum_<DateTime::Timescale>(clsDef, "Timescale"), [](auto &mod, auto &enm) {
         enm.value("TAI", DateTime::Timescale::TAI);
         enm.value("UTC", DateTime::Timescale::UTC);
         enm.value("TT", DateTime::Timescale::TT);
         enm.export_values();
     });

     wrappers.wrapType(py::enum_<DateTime::DateSystem>(clsDef, "DateSystem"), [](auto &mod, auto &enm) {
        enm.value("JD", DateTime::DateSystem::JD);
        enm.value("MJD", DateTime::DateSystem::MJD);
        enm.value("EPOCH", DateTime::DateSystem::EPOCH);
        enm.export_values();
     });

}

}  // base
}  // daf
}  // lsst

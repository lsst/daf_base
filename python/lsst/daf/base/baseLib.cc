#include <boost/python.hpp>
#include <boost/python/to_python/copy_to_tuple.hpp> // actually defined in utils package (for now)
#include <boost/python/from_python/container.hpp> // actually defined in utils package (for now)x
#include <sstream>

#include "lsst/daf/base.h"
#include "lsst/pex/exceptions/python.h"

namespace bp = boost::python;

namespace lsst { namespace daf { namespace base {

struct PyCitizen {

    // Only wrap std::ostream overload of census - the std::vector one cannot safely be wrapped.
    static bp::str census(Citizen::memId startingMemId) {
        std::ostringstream oss;
        Citizen::census(oss, startingMemId);
        return bp::str(oss.str());
    }

    static void declare() {
        bp::class_< Citizen, boost::shared_ptr<Citizen> >("Citizen", bp::no_init)
            .def("repr", &Citizen::repr)
            .def("markPersistent", &Citizen::markPersistent)
            .def("census", &PyCitizen::census, (bp::arg("startingMemId")=0))
            .staticmethod("census")
            .def("hasBeenCorrupted", &Citizen::hasBeenCorrupted)
            .staticmethod("hasBeenCorrupted")
            .def("getId", &Citizen::getId)
            .def("getNextMemId", &Citizen::getNextMemId)
            .staticmethod("getNextMemId")
            .def("init", &Citizen::init)
            .staticmethod("init")
            ;
    }

};

struct PyDateTime {

    static void declare() {
        // Unfortunately, we can't declare these after declaring DateTime, so they end up in the
        // wrong scope.  I'd consider it a minor Boost.Python bug, but the workaround is easy.
        bp::class_< DateTime, boost::shared_ptr<DateTime> > pyDateTime(
            "DateTime", bp::init<std::string const &>(bp::arg("iso8601"))
        );
        {
            bp::scope inDateTime(pyDateTime);
            bp::enum_<DateTime::Timescale>("Timescale")
                .value("TAI", DateTime::TAI)
                .value("UTC", DateTime::UTC)
                .value("TT", DateTime::TT)
                ;
            bp::enum_<DateTime::DateSystem>("DateSystem")
            .value("JD", DateTime::JD)
            .value("MJD", DateTime::MJD)
            .value("EPOCH", DateTime::EPOCH)
            ;
        }
        pyDateTime
            .def(bp::init<double, DateTime::DateSystem, DateTime::Timescale>(
                     (bp::arg("date"), bp::arg("system")=DateTime::MJD, bp::arg("scale")=DateTime::TAI)))
            .def(bp::init<long long, DateTime::Timescale>(
                     (bp::arg("nsecs")=0LL, bp::arg("scale")=DateTime::TAI)))
            .def(bp::init<int,int,int,int,int,int,DateTime::Timescale>(
                     (bp::arg("year"), bp::arg("month"), bp::arg("day"), 
                      bp::arg("hr"), bp::arg("min"), bp::arg("sec"), bp::arg("scale")=DateTime::TAI)))
            .def("nsecs", &DateTime::nsecs, (bp::arg("scale")=DateTime::TAI))
            .def("mjd", &DateTime::mjd, (bp::arg("scale")=DateTime::TAI))
            .def("get", &DateTime::get, (bp::arg("system")=DateTime::MJD, bp::arg("scale")=DateTime::TAI))
            .def("toString", &DateTime::toString)
            .def("__str__", &DateTime::toString)
            .def("now", &DateTime::now)
            .staticmethod("now")
            .def("initializeLeapSeconds", &DateTime::initializeLeapSeconds)
            .staticmethod("initializeLeapSeconds")
            .setattr("TAI", DateTime::TAI)
            .setattr("UTC", DateTime::UTC)
            .setattr("TT", DateTime::TT)
            .setattr("JD", DateTime::JD)
            .setattr("MJD", DateTime::MJD)
            .setattr("EPOCH", DateTime::EPOCH)
            ;
    }

};

struct PyPersistable {

    static void declare() {
        bp::class_< Persistable, boost::shared_ptr<Persistable> >("Persistable", bp::no_init);
    }

};

struct PyPropertySet {

    typedef bp::class_< PropertySet, PropertySet::Ptr, bp::bases<Persistable,Citizen>, 
                        boost::noncopyable> Wrapper;

    template <typename T>
    static void addAccessors(Wrapper & cls, std::string const & name) {
        typedef T (PropertySet::*DefaultGetter)(std::string const &, T const &) const;
        typedef T (PropertySet::*BasicGetter)(std::string const &) const;
        typedef void (PropertySet::*VectorAccessor)(std::string const &, std::vector<T> const &);
        typedef void (PropertySet::*ScalarAccessor)(std::string const &, T const &);
        cls.def(("get" + name).c_str(), (BasicGetter)&PropertySet::get<T>, bp::arg("name"));
        cls.def(("get" + name).c_str(), (DefaultGetter)&PropertySet::get<T>,
                (bp::arg("name"), bp::arg("default")));
        cls.def(("set" + name).c_str(), (VectorAccessor)&PropertySet::set<T>, 
                (bp::arg("name"), bp::arg("value")));
        cls.def(("set" + name).c_str(), (ScalarAccessor)&PropertySet::set<T>, 
                (bp::arg("name"), bp::arg("value")));
        cls.def(("add" + name).c_str(), (VectorAccessor)&PropertySet::add<T>,
                (bp::arg("name"), bp::arg("value")));
        cls.def(("add" + name).c_str(), (ScalarAccessor)&PropertySet::add<T>,
                (bp::arg("name"), bp::arg("value")));
        cls.def(("getArray" + name).c_str(), &PropertySet::getArray<T>, 
                (bp::arg("name"), bp::arg("value")), bp::return_value_policy<bp::copy_to_tuple>());
        cls.setattr(("TYPE_" + name).c_str(), typeid(T).name());
    }

    static bp::str typeOf(PropertySet const & self, std::string const & name) {
        return bp::str(self.typeOf(name).name());
    }

    static void declare() {

        bp::container_from_python_sequence< std::vector<bool> >::declare();
        bp::container_from_python_sequence< std::vector<short> >::declare();
        bp::container_from_python_sequence< std::vector<int> >::declare();
        bp::container_from_python_sequence< std::vector<long> >::declare();
        bp::container_from_python_sequence< std::vector<long long> >::declare();
        bp::container_from_python_sequence< std::vector<float> >::declare();
        bp::container_from_python_sequence< std::vector<double> >::declare();
        bp::container_from_python_sequence< std::vector<std::string> >::declare();
        bp::container_from_python_sequence< std::vector<DateTime> >::declare();

        Wrapper wrapper("PropertySet");
        wrapper
            .def("deepCopy", &PropertySet::deepCopy)
            .def("nameCount", &PropertySet::nameCount, (bp::arg("topLevelOnly")=true))
            .def("names", &PropertySet::names, (bp::arg("topLevelOnly")=true),
                 bp::return_value_policy<bp::copy_to_list>())
            .def("paramNames", &PropertySet::paramNames, (bp::arg("topLevelOnly")=true),
                 bp::return_value_policy<bp::copy_to_list>())
            .def("propertySetNames", &PropertySet::propertySetNames, (bp::arg("topLevelOnly")=true),
                 bp::return_value_policy<bp::copy_to_list>())
            .def("exists", &PropertySet::exists, bp::arg("name"))
            .def("isArray", &PropertySet::isArray, bp::arg("name"))
            .def("isPropertySetPtr", &PropertySet::isPropertySetPtr, bp::arg("name"))
            .def("valueCount", &PropertySet::valueCount, bp::arg("name"))
            .def("typeOf", &PyPropertySet::typeOf, bp::arg("name"))
            .def("getAsBool", &PropertySet::getAsBool, bp::arg("name"))
            .def("getAsInt", &PropertySet::getAsInt, bp::arg("name"))
            .def("getAsDouble", &PropertySet::getAsDouble, bp::arg("name"))
            .def("getAsString", &PropertySet::getAsString, bp::arg("name"))
            .def("getAsPropertySetPtr", &PropertySet::getAsPropertySetPtr, bp::arg("name"))
            .def("getAsPersistablePtr", &PropertySet::getAsPersistablePtr, bp::arg("name"))
            .def("toString", &PropertySet::toString, (bp::arg("topLevelOnly")=false,bp::arg("indent")=""))
            .def("copy", &PropertySet::copy, (bp::arg("dest"), bp::arg("source"), bp::arg("name")))
            .def("combine", &PropertySet::combine, bp::arg("source"))
            .def("remove", &PropertySet::remove, bp::arg("name"))
            ;
        addAccessors<bool>(wrapper, "Bool");
        addAccessors<short>(wrapper, "Short");
        addAccessors<int>(wrapper, "Int");
        addAccessors<long>(wrapper, "Long");
        addAccessors<long long>(wrapper, "LongLong");
        addAccessors<float>(wrapper, "Float");
        addAccessors<double>(wrapper, "Double");
        addAccessors<std::string>(wrapper, "String");
        addAccessors<DateTime>(wrapper, "DateTime");
    }

};

}}}

BOOST_PYTHON_MODULE(baseLib) {
    bp::import("lsst.pex.exceptions");
    lsst::daf::base::PyCitizen::declare();
    lsst::daf::base::PyDateTime::declare();
    lsst::daf::base::PyPersistable::declare();
    lsst::daf::base::PyPropertySet::declare();
    lsst::pex::exceptions::PyException::subclass<lsst::daf::base::TypeMismatchException,
        lsst::pex::exceptions::LogicErrorException>("TypeMismatchException");
}

// -*- lsst-c++ -*-
%define daf_base_DOCSTRING
"
Access to the classes from the daf_base library
"
%enddef

%feature("autodoc", "1");
%module(package="lsst.daf.base", docstring=daf_base_DOCSTRING) baseLib

%{
#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/Persistable.h"
#include "lsst/daf/base/PropertySet.h"
%}

#define NO_SWIG_LSST_EXCEPTIONS

%include "lsst/p_lsstSwig.i"

SWIG_SHARED_PTR(Persistable, lsst::daf::base::Persistable)
SWIG_SHARED_PTR_DERIVED(PropertySet, lsst::daf::base::Persistable, lsst::daf::base::PropertySet)

class lsst::daf::base::Citizen;

%template(vectorCitizen) std::vector<lsst::daf::base::Citizen *>;

// Swig versions 1.3.33 - 1.3.36 have problems with std::vector<lsst::daf::base::Citizen const *>,
// so replace Citizen::census() with a function that casts to something swig understands.
%extend lsst::daf::base::Citizen {
    static std::vector<lsst::daf::base::Citizen *> const * census() {
        return reinterpret_cast<std::vector<lsst::daf::base::Citizen *> const *>(
                lsst::daf::base::Citizen::census());
    }
    %ignore census();
}

%template(VectorB) std::vector<bool>;
%template(VectorI) std::vector<int>;
%template(VectorD) std::vector<double>;
%template(VectorS) std::vector<std::string>;

%include "lsst/daf/base/Citizen.h"
%include "lsst/daf/base/DateTime.h"
%include "lsst/daf/base/Persistable.h"
%include "lsst/daf/base/PropertySet.h"

%template(setB) lsst::daf::base::PropertySet::set<bool>;
%template(setI) lsst::daf::base::PropertySet::set<int>;
%template(setD) lsst::daf::base::PropertySet::set<double>;
%template(setS) lsst::daf::base::PropertySet::set<std::string>;

%template(addB) lsst::daf::base::PropertySet::add<bool>;
%template(addI) lsst::daf::base::PropertySet::add<int>;
%template(addD) lsst::daf::base::PropertySet::add<double>;
%template(addS) lsst::daf::base::PropertySet::add<std::string>;

%template(getArrayB) lsst::daf::base::PropertySet::getArray<bool>;
%template(getArrayI) lsst::daf::base::PropertySet::getArray<int>;
%template(getArrayD) lsst::daf::base::PropertySet::getArray<double>;
%template(getArrayS) lsst::daf::base::PropertySet::getArray<std::string>;

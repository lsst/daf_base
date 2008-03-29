// -*- lsst-c++ -*-
%define daf_base_DOCSTRING
"
Access to the classes from the daf_base library
"
%enddef

%feature("autodoc", "1");
%module(package="lsst.daf.base", docstring=daf_base_DOCSTRING) base

%{
#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/Persistable.h"
%}

%inline %{
namespace lsst { namespace daf { namespace base { } } }
    
using namespace lsst::daf::base;
%}

%init %{
%}

%include "p_lsstSwig.i"
%template(vectorCitizen) std::vector<Citizen *>;
//
// Swig 1.3.33 has problems with std::vector<Citizen const *>, so
// we fake things here.  It's a fake, hence the C-style cast
//
%inline %{
    std::vector<Citizen *> * Citizen_census_for_swig() {
        return (std::vector<Citizen *> *)Citizen::census();
    }
%}
%include "lsst/daf/base/Citizen.h"
%include "lsst/daf/base/DateTime.h"
%include "lsst/daf/base/Persistable.h"
%include "DataProperty.i"

/******************************************************************************/
// Local Variables: ***
// eval: (setq indent-tabs-mode nil) ***
// End: ***

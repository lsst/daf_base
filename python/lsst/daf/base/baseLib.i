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

%include "lsst/p_lsstSwig.i"

%lsst_exceptions()
%import "lsst/pex/exceptions/exceptionsLib.i"

SWIG_SHARED_PTR(Persistable, lsst::daf::base::Persistable)
SWIG_SHARED_PTR_DERIVED(PropertySet, lsst::daf::base::Persistable, lsst::daf::base::PropertySet)

%include "persistenceMacros.i"
%lsst_persistable(lsst::daf::base::PropertySet);

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

// This has to come before PropertySet.h
%define VectorAddType(type, typeName)
    %template(Vector ## typeName) std::vector<type>;
%enddef

%ignore lsst::daf::base::PropertySet::set(std::string const&, char const*);
%ignore lsst::daf::base::PropertySet::add(std::string const&, char const*);
%ignore lsst::daf::base::PropertySet::getAsInt64(std::string const&) const;

VectorAddType(bool, Bool)
VectorAddType(short, Short)
VectorAddType(int, Int)
VectorAddType(long, Long)
VectorAddType(long long, LongLong)
VectorAddType(float, Float)
VectorAddType(double, Double)
VectorAddType(std::string, String)
VectorAddType(lsst::daf::base::DateTime, DateTime)

SWIG_SHARED_PTR(Citizen, lsst::daf::base::Citizen);

%include "lsst/daf/base/Citizen.h"
%include "lsst/daf/base/DateTime.h"
%include "lsst/daf/base/Persistable.h"
%include "lsst/daf/base/PropertySet.h"

// This has to come after PropertySet.h
%define PropertySetAddType(type, typeName)
    %template(set ## typeName) lsst::daf::base::PropertySet::set<type>;
    %template(add ## typeName) lsst::daf::base::PropertySet::add<type>;
    %template(get ## typeName) lsst::daf::base::PropertySet::get<type>;
    %template(getArray ## typeName) lsst::daf::base::PropertySet::getArray<type>;
    %extend lsst::daf::base::PropertySet {
static std::type_info const TYPE_ ## typeName = typeid(type);
}
%enddef

PropertySetAddType(bool, Bool)
PropertySetAddType(short, Short)
PropertySetAddType(int, Int)
PropertySetAddType(long, Long)
PropertySetAddType(long long, LongLong)
PropertySetAddType(float, Float)
PropertySetAddType(double, Double)
PropertySetAddType(std::string, String)
PropertySetAddType(lsst::daf::base::DateTime, DateTime)

%pythoncode {
def getPSValue(self, name):
    """
    Extract a single Python value of unknown type from a PropertySet by
    trying each Python-compatible type in turn until no exception is raised.
    """
    if not self.exists(name):
    	raise lsst.pex.exceptions.LsstException, name + " not found"

    try:
        value = self.getArrayString(name)
        return value[0] if len(value) == 1 else value
    except:
        pass
    try:
        return self.getAsPropertySetPtr(name)
    except:
        pass
    try:
        return self.getAsPersistablePtr(name)
    except:
        pass
    try:
        value = self.getArrayBool(name)
        return value[0] if len(value) == 1 else value
    except:
        pass
    try:
        value = self.getArrayInt(name)
        return value[0] if len(value) == 1 else value
    except:
        pass
    try:
        value = self.getArrayLong(name)
        return value[0] if len(value) == 1 else value
    except:
        pass
    try:
        value = self.getArrayLongLong(name)
        return value[0] if len(value) == 1 else value
    except:
        pass
    try:
        value = self.getArrayDouble(name)
        return value[0] if len(value) == 1 else value
    except:
        pass
    try:
        value = self.getArrayDateTime(name)
        return value[0] if len(value) == 1 else value
    except:
        pass
    raise lsst.pex.exceptions.LsstException, \
        'Unknown PropertySet value type for ' + name

def setPSValue(self, name, value):
    """
    Set a value in a PropertySet from a single Python value of unknown type.
    """
    if isinstance(value, bool):
        self.setBool(name, value)
    elif isinstance(value, int):
	self.setInt(name, value)
    elif isinstance(value, long):
        self.setLongLong(name, value)
    elif isinstance(value, float):
        self.setDouble(name, value)
    elif isinstance(value, str):
        self.setString(name, value)
    elif isinstance(value, lsst.daf.base.DateTime):
        self.setDateTime(name, value)
    else:
        raise lsst.pex.exceptions.LsstException, \
            'Unknown value type for %s: %s' % (name, type(name))

def addPSValue(self, name, value):
    """
    Add a value to a PropertySet from a single Python value of unknown type.
    """
    if isinstance(value, bool):
        self.addBool(name, value)
    elif isinstance(value, int):
	self.addInt(name, value)
    elif isinstance(value, long):
        self.addLongLong(name, value)
    elif isinstance(value, float):
        self.addDouble(name, value)
    elif isinstance(value, str):
        self.addString(name, value)
    elif isinstance(value, lsst.daf.base.DateTime):
        self.addDateTime(name, value)
    else:
        raise lsst.pex.exceptions.LsstException, \
            'Unknown value type for %s: %s' % (name, type(name))

PropertySet.get = getPSValue
PropertySet.set = setPSValue
PropertySet.add = addPSValue
}

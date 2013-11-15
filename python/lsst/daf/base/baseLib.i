// -*- lsst-c++ -*-

/*
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 *
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program.  If not,
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */

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
#include "lsst/daf/base/PropertyList.h"
%}

%include "lsst/p_lsstSwig.i"

%lsst_exceptions()
%import "lsst/pex/exceptions/exceptionsLib.i"

%shared_ptr(lsst::daf::base::Persistable);
%shared_ptr(lsst::daf::base::PropertySet);
%shared_ptr(lsst::daf::base::PropertyList);

%include "persistenceMacros.i"
%lsst_persistable(lsst::daf::base::PropertySet);
%lsst_persistable(lsst::daf::base::PropertyList);

// This has to come before PropertySet.h
%define VectorAddType(type, typeName)
    %template(Vector ## typeName) std::vector<type>;
%enddef

%ignore lsst::daf::base::PropertySet::set(std::string const&, char const*);
%ignore lsst::daf::base::PropertySet::add(std::string const&, char const*);
%ignore lsst::daf::base::PropertySet::getAsInt64(std::string const&) const;
%ignore lsst::daf::base::PropertyList::set(std::string const&, char const*);
%ignore lsst::daf::base::PropertyList::add(std::string const&, char const*);
%ignore lsst::daf::base::PropertyList::getAsInt64(std::string const&) const;

VectorAddType(bool, Bool)
VectorAddType(short, Short)
VectorAddType(int, Int)
VectorAddType(long, Long)
VectorAddType(long long, LongLong)
VectorAddType(float, Float)
VectorAddType(double, Double)
VectorAddType(std::string, String)
VectorAddType(lsst::daf::base::DateTime, DateTime)

%shared_ptr(lsst::daf::base::Citizen);
%ignore lsst::daf::base::Citizen::operator=;

%include "lsst/daf/base/Citizen.h"
%include "lsst/daf/base/DateTime.h"
%include "lsst/daf/base/Persistable.h"
%include "lsst/daf/base/PropertySet.h"
%include "lsst/daf/base/PropertyList.h"

%extend lsst::daf::base::DateTime {
    %pythoncode %{
        def toPython(self, timescale=None):
            """Convert a DateTime to Python's datetime

            @param timescale  Timescale for resultant datetime
            """
            import datetime
            nsecs = self.nsecs(timescale) if timescale is not None else self.nsecs()
            return datetime.datetime.utcfromtimestamp(nsecs/10**9)
        def __reduce__(self):
            return self.__class__, (self.nsecs(),)
        def __eq__(self, other):
            return self.nsecs() == other.nsecs()
    %}
}

%template(vectorCitizen) std::vector<lsst::daf::base::Citizen const *>;

// This has to come after PropertySet.h
%define PropertySetAddType(type, typeName)
    %template(set ## typeName) lsst::daf::base::PropertySet::set<type >;
    %template(add ## typeName) lsst::daf::base::PropertySet::add<type >;
    %template(get ## typeName) lsst::daf::base::PropertySet::get<type >;
    %template(getArray ## typeName) lsst::daf::base::PropertySet::getArray<type >;
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
PropertySetAddType(boost::shared_ptr<lsst::daf::base::PropertySet>, PropertySet)

%pythoncode {
def _propertyContainerElementTypeName(container, name):
    """Return name of the type of a particular element"""
    t = container.typeOf(name)
    for checkType in ("Bool", "Short", "Int", "Long", "LongLong", "Float", "Double", "String", "DateTime"):
        if t == getattr(container, "TYPE_" + checkType):
            return checkType
    return None

def _propertyContainerGet(container, name, asArray=False):
    """Extract a single Python value of unknown type"""
    if not container.exists(name):
        raise lsst.pex.exceptions.LsstException, name + " not found"

    elemType = _propertyContainerElementTypeName(container, name)
    if elemType:
        value = getattr(container, "getArray" + elemType)(name)
        return value[0] if len(value) == 1 and not asArray else value

    try:
        return container.getAsPropertyListPtr(name)
    except:
        pass
    if container.typeOf(name) == container.TYPE_PropertySet:
        return container.getAsPropertySetPtr(name)
    try:
        return container.getAsPersistablePtr(name)
    except:
        pass
    raise lsst.pex.exceptions.LsstException('Unknown PropertySet value type for ' + name)

def _propertyContainerSet(container, name, value, typeMenu, *args):
    """Set a single Python value of unknown type"""
    if hasattr(value, "__iter__"):
        exemplar = value[0]
    else:
        exemplar = value

    t = type(exemplar)
    if t in typeMenu:
        return getattr(container, "set" + typeMenu[t])(name, value, *args)
    # Allow for subclasses
    for checkType in typeMenu:
        if isinstance(exemplar, checkType):
            return getattr(container, "set" + typeMenu[checkType])(name, value, *args)
    raise lsst.pex.exceptions.LsstException("Unknown value type for %s: %s" % (name, t))

def _propertyContainerAdd(container, name, value, typeMenu, *args):
    """Add a single Python value of unknown type"""
    if hasattr(value, "__iter__"):
        exemplar = value[0]
    else:
        exemplar = value

    t = type(exemplar)
    if t in typeMenu:
        return getattr(container, "add" + typeMenu[t])(name, value, *args)
    # Allow for subclasses
    for checkType in typeMenu:
        if isinstance(exemplar, checkType):
            return getattr(container, "add" + typeMenu[checkType])(name, value, *args)
    raise lsst.pex.exceptions.LsstException("Unknown value type for %s: %s" % (name, t))


# Mapping of type to method names
_PS_typeMenu = {bool: "Bool",
                int: "Int",
                long: "LongLong",
                float: "Double",
                str: "String",
                DateTime: "DateTime",
                PropertySet: "PropertySet",
                PropertyList: "PropertySet",
                }

def _PS_getValue(self, name, asArray=False):
    return _propertyContainerGet(self, name, asArray)
def _PS_setValue(self, name, value):
    return _propertyContainerSet(self, name, value, _PS_typeMenu)
def _PS_addValue(self, name, value):
    return _propertyContainerAdd(self, name, value, _PS_typeMenu)

PropertySet.get = _PS_getValue
PropertySet.set = _PS_setValue
PropertySet.add = _PS_addValue
del _PS_getValue
del _PS_setValue
del _PS_addValue
}

// This has to come after PropertyList.h
%define PropertyListAddType(type, typeName)
    %template(set ## typeName) lsst::daf::base::PropertyList::set<type>;
    %template(add ## typeName) lsst::daf::base::PropertyList::add<type>;
    %template(get ## typeName) lsst::daf::base::PropertyList::get<type>;
    %template(getArray ## typeName) lsst::daf::base::PropertyList::getArray<type>;
    %extend lsst::daf::base::PropertyList {
static std::type_info const TYPE_ ## typeName = typeid(type);
void setPropertySet(
    std::string const& name, PropertySet::Ptr const& value,
    bool inPlace=true) {
    $self->set(name, value, inPlace);
}
}
%enddef

PropertyListAddType(bool, Bool)
PropertyListAddType(short, Short)
PropertyListAddType(int, Int)
PropertyListAddType(long, Long)
PropertyListAddType(long long, LongLong)
PropertyListAddType(float, Float)
PropertyListAddType(double, Double)
PropertyListAddType(std::string, String)
PropertyListAddType(lsst::daf::base::DateTime, DateTime)

%extend lsst::daf::base::PropertyList {
    %pythoncode {
        def __len__(self):
            return self.size()
        def __getstate__(self):
            return [(name, _propertyContainerElementTypeName(self, name), self.get(name),
                     self.getComment(name)) for name in self.getOrderedNames()]
        def __setstate__(self, state):
            self.__init__()
            for name, elemType, value, comment in state:
                getattr(self, "set" + elemType)(name, value, comment)
}
}

%pythoncode {
# Mapping of type to method names
_PL_typeMenu = {bool: "Bool",
                int: "Int",
                long: "LongLong",
                float: "Double",
                str: "String",
                DateTime: "DateTime",
                PropertySet: "PropertySet",
                PropertyList: "PropertySet",
                }

def _PL_getValue(self, name, asArray=False):
    return _propertyContainerGet(self, name, asArray)
def _PL_setValue(self, name, value, comment=None, inPlace=True):
    args = []
    if comment is not None:
        args.append(comment)
    args.append(inPlace)
    return _propertyContainerSet(self, name, value, _PL_typeMenu, *args)
def _PL_addValue(self, name, value, comment=None, inPlace=True):
    args = []
    if comment is not None:
        args.append(comment)
    args.append(inPlace)
    return _propertyContainerAdd(self, name, value, _PL_typeMenu, *args)

PropertyList.get = _PL_getValue
PropertyList.set = _PL_setValue
PropertyList.add = _PL_addValue
del _PL_getValue
del _PL_setValue
del _PL_addValue

def _PL_toList(self):
    orderedNames = self.getOrderedNames()
    ret = []
    for name in orderedNames:
        if self.isArray(name):
            values = self.get(name)
            for v in values:
                ret.append((name, v, self.getComment(name)))
        else:
            ret.append((name, self.get(name), self.getComment(name)))
    return ret

PropertyList.toList = _PL_toList
del _PL_toList
}



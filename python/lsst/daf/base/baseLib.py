# 
# LSST Data Management System
#
# Copyright 2008-2016  AURA/LSST.
# 
# This product includes software developed by the
# LSST Project (http://www.lsst.org/).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the LSST License Statement and 
# the GNU General Public License along with this program.  If not, 
# see <https://www.lsstcorp.org/LegalNotices/>.
#

from __future__ import absolute_import
from past.builtins import long

from .citizen import *
from .dateTime import *
from ._persistable import *
from ._propertySet import *
from ._propertyList import *

import lsst.pex.exceptions

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
        raise lsst.pex.exceptions.NotFoundError(name + " not found")

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
    raise lsst.pex.exceptions.TypeError('Unknown PropertySet value type for ' + name)

import numbers
def _guessIntegerType(container, name, value):
    """Given an existing container and name, determine the type
    that should be used for the supplied value. The supplied value
    is assumed to be a scalar.

    On Python 3 all ints are LongLong but we need to be able to store them
    in Int containers if that is what is being used (testing for truncation).
    Int is assumed to mean 32bit integer (2147483647 to -2147483648).

    If there is no pre-existing value we have to decide what to do. For now
    we pick Int if the value is less than maxsize.

    Returns None if the value supplied is a bool or not an integral value.
    """
    useType = None
    maxInt = 2147483647
    minInt = -2147483648

    # We do not want to convert bool to int so let the system work that
    # out itself
    if isinstance(value, bool):
        return useType

    if isinstance(value, numbers.Integral):
        try:
            containerType = _propertyContainerElementTypeName(container, name)
        except lsst.pex.exceptions.NotFoundError:
            # nothing in the container so choose based on size. Safe option is to
            # always use LongLong
            if value <= maxInt and value >= minInt:
                useType = "Int"
            else:
                useType = "LongLong"
        else:
            if containerType == "Int":
                # Always use an Int even if we know it won't fit. The later
                # code will trigger OverflowError if appropriate. Setting the
                # type to LongLong here will trigger a TypeError instead so it's
                # best to trigger a predictable OverflowError.
                useType = "Int"
            elif containerType == "LongLong":
                useType = "LongLong"
    return useType

def _propertyContainerSet(container, name, value, typeMenu, *args):
    """Set a single Python value of unknown type"""
    if hasattr(value, "__iter__") and not isinstance(value, str):
        exemplar = value[0]
    else:
        exemplar = value

    t = type(exemplar)
    setType = _guessIntegerType(container, name, exemplar)

    if setType is not None or t in typeMenu:
        if setType is None:
            setType = typeMenu[t]
        return getattr(container, "set" + setType)(name, value, *args)
    # Allow for subclasses
    for checkType in typeMenu:
        if isinstance(exemplar, checkType):
            return getattr(container, "set" + typeMenu[checkType])(name, value, *args)
    raise lsst.pex.exceptions.TypeError("Unknown value type for %s: %s" % (name, t))

def _propertyContainerAdd(container, name, value, typeMenu, *args):
    """Add a single Python value of unknown type"""
    if hasattr(value, "__iter__"):
        exemplar = value[0]
    else:
        exemplar = value

    t = type(exemplar)
    addType = _guessIntegerType(container, name, exemplar)

    if addType is not None or t in typeMenu:
        if addType is None:
            addType = typeMenu[t]
        return getattr(container, "add" + addType)(name, value, *args)
    # Allow for subclasses
    for checkType in typeMenu:
        if isinstance(exemplar, checkType):
            return getattr(container, "add" + typeMenu[checkType])(name, value, *args)
    raise lsst.pex.exceptions.TypeError("Unknown value type for %s: %s" % (name, t))


# Mapping of type to method names
_PS_typeMenu = {bool: "Bool",
                long: "LongLong",
                int: "Int", # overwrites long on Python 3.x
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

def _PS_toDict(self):
    """Returns a (possibly nested) dictionary with all properties.
    """

    d = {}
    for name in self.names():
        v = self.get(name)

        if isinstance(v, PropertySet):
            d[name] = PropertySet.toDict(v)
        else:
            d[name] = v
    return d

PropertySet.get = _PS_getValue
PropertySet.set = _PS_setValue
PropertySet.add = _PS_addValue
PropertySet.toDict = _PS_toDict
del _PS_getValue
del _PS_setValue
del _PS_addValue
del _PS_toDict

def _PL__len__(self):
    return self.size()

def getstate(self):
    return [(name, _propertyContainerElementTypeName(self, name), self.get(name),
        self.getComment(name)) for name in self.getOrderedNames()]

def setstate(self, state):
    for name, elemType, value, comment in state:
        getattr(self, "set" + elemType)(name, value, comment)
PropertyList.__len__ = _PL__len__
del _PL__len__

# Mapping of type to method names
_PL_typeMenu = {bool: "Bool",
                long: "LongLong",
                int: "Int", # overwrites long on Python 3.x
                float: "Double",
                str: "String",
                DateTime: "DateTime",
                PropertySet: "PropertySet",
                PropertyList: "PropertySet",
                }

def _PL_getValue(self, name, asArray=False):
    return _propertyContainerGet(self, name, asArray)
def _PL_setValue(self, name, value, comment=None):
    args = []
    if comment is not None:
        args.append(comment)
    return _propertyContainerSet(self, name, value, _PL_typeMenu, *args)
def _PL_addValue(self, name, value, comment=None):
    args = []
    if comment is not None:
        args.append(comment)
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

def _PL_toOrderedDict(self):
    """Return an ordered dictionary with all properties in the order that
    they were inserted.
    """
    from collections import OrderedDict

    d = OrderedDict()
    for name in self.getOrderedNames():
        d[name] = self.get(name)
    return d

PropertyList.toList = _PL_toList
PropertyList.toOrderedDict = _PL_toOrderedDict
del _PL_toList
del _PL_toOrderedDict


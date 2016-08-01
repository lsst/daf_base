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
    raise lsst.pex.exceptions.TypeError("Unknown value type for %s: %s" % (name, t))

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
    raise lsst.pex.exceptions.TypeError("Unknown value type for %s: %s" % (name, t))

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


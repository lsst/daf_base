from baseLib import *
import lsst.pex.exceptions

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

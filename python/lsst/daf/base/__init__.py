from . import baseLib
import lsst.bputils
import lsst.pex.exceptions

lsst.bputils.rescope(baseLib, globals(), names=("Citizen", "DateTime", "Persistable"))

@lsst.bputils.extend(baseLib.PropertySet)
class PropertySet:

    def get(self, name):
        """
        Extract a single Python value of unknown type from a PropertySet by
        trying each Python-compatible type in turn until no exception is raised.
        """
        if not self.exists(name):
            raise lsst.pex.exceptions.Exception, name + " not found"

        t = self.typeOf(name)
        if t == self.TYPE_Bool:
            value = self.getArrayBool(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Short:
            value = self.getArrayShort(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Int:
            value = self.getArrayInt(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Long:
            value = self.getArrayLong(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_LongLong:
            value = self.getArrayLongLong(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Float:
            value = self.getArrayFloat(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Double:
            value = self.getArrayDouble(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_String:
            value = self.getArrayString(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_DateTime:
            value = self.getArrayDateTime(name)
            return value[0] if len(value) == 1 else value
        try:
            return self.getAsPropertySetPtr(name)
        except:
            pass
        try:
            return self.getAsPersistablePtr(name)
        except:
            pass
        raise lsst.pex.exceptions.Exception, \
            'Unknown PropertySet value type for ' + name

    def set(self, name, value):
        """
        Set a value in a PropertySet from a single Python value of unknown type.
        """
        if hasattr(value, "__iter__"):
            exemplar = value[0]
        else:
            exemplar = value
        if isinstance(exemplar, bool):
            self.setBool(name, value)
        elif isinstance(exemplar, int):
            self.setInt(name, value)
        elif isinstance(exemplar, long):
            self.setLongLong(name, value)
        elif isinstance(exemplar, float):
            self.setDouble(name, value)
        elif isinstance(exemplar, str):
            self.setString(name, value)
        elif isinstance(exemplar, lsst.daf.base.DateTime):
            self.setDateTime(name, value)
        else:
            raise lsst.pex.exceptions.Exception, \
                'Unknown value type for %s: %s' % (name, type(value))

    def add(self, name, value):
        """
        Add a value to a PropertySet from a single Python value of unknown type.
        """
        if hasattr(value, "__iter__"):
            exemplar = value[0]
        else:
            exemplar = value
        if isinstance(exemplar, bool):
            self.addBool(name, value)
        elif isinstance(exemplar, int):
            self.addInt(name, value)
        elif isinstance(exemplar, long):
            self.addLongLong(name, value)
        elif isinstance(exemplar, float):
            self.addDouble(name, value)
        elif isinstance(exemplar, str):
            self.addString(name, value)
        elif isinstance(exemplar, lsst.daf.base.DateTime):
            self.addDateTime(name, value)
        else:
            raise lsst.pex.exceptions.Exception, \
                'Unknown value type for %s: %s' % (name, type(exemplar))

@lsst.bputils.extend(baseLib.PropertyList)
class PropertyList:

    def get(self, name):
        """
        Extract a single Python value of unknown type from a PropertyList by
        trying each Python-compatible type in turn until no exception is raised.
        """
        if not self.exists(name):
            raise lsst.pex.exceptions.Exception, name + " not found"

        t = self.typeOf(name)
        if t == self.TYPE_Bool:
            value = self.getArrayBool(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Short:
            value = self.getArrayShort(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Int:
            value = self.getArrayInt(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Long:
            value = self.getArrayLong(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_LongLong:
            value = self.getArrayLongLong(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Float:
            value = self.getArrayFloat(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_Double:
            value = self.getArrayDouble(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_String:
            value = self.getArrayString(name)
            return value[0] if len(value) == 1 else value
        elif t == self.TYPE_DateTime:
            value = self.getArrayDateTime(name)
            return value[0] if len(value) == 1 else value
        try:
            return self.getAsPropertyListPtr(name)
        except:
            pass
        try:
            return self.getAsPersistablePtr(name)
        except:
            pass
        raise lsst.pex.exceptions.Exception, \
            'Unknown PropertyList value type for ' + name

    def set(self, name, value, comment=None, inPlace=True):
        """
        List a value in a PropertyList from a single Python value of unknown type.
        """
        if hasattr(value, "__iter__"):
            exemplar = value[0]
        else:
            exemplar = value
        if comment is None:
            if isinstance(exemplar, bool):
                self.setBool(name, value, inPlace)
            elif isinstance(exemplar, int):
                self.setInt(name, value, inPlace)
            elif isinstance(exemplar, long):
                self.setLongLong(name, value, inPlace)
            elif isinstance(exemplar, float):
                self.setDouble(name, value, inPlace)
            elif isinstance(exemplar, str):
                self.setString(name, value, inPlace)
            elif isinstance(exemplar, lsst.daf.base.DateTime):
                self.setDateTime(name, value, inPlace)
            else:
                raise lsst.pex.exceptions.Exception, \
                    'Unknown value type for %s: %s' % (name, type(value))
        else:
            if isinstance(exemplar, bool):
                self.setBool(name, value, comment, inPlace)
            elif isinstance(exemplar, int):
                self.setInt(name, value, comment, inPlace)
            elif isinstance(exemplar, long):
                self.setLongLong(name, value, comment, inPlace)
            elif isinstance(exemplar, float):
                self.setDouble(name, value, comment, inPlace)
            elif isinstance(exemplar, str):
                self.setString(name, value, comment, inPlace)
            elif isinstance(exemplar, lsst.daf.base.DateTime):
                self.setDateTime(name, value, comment, inPlace)
            else:
                raise lsst.pex.exceptions.Exception, \
                    'Unknown value type for %s: %s' % (name, type(value))

    def add(self, name, value, comment=None, inPlace=True):
        """
        Add a value to a PropertyList from a single Python value of unknown type.
        """
        if hasattr(value, "__iter__"):
            exemplar = value[0]
        else:
            exemplar = value
        if comment is None:
            if isinstance(exemplar, bool):
                self.addBool(name, value, inPlace)
            elif isinstance(exemplar, int):
                self.addInt(name, value, inPlace)
            elif isinstance(exemplar, long):
                self.addLongLong(name, value, inPlace)
            elif isinstance(exemplar, float):
                self.addDouble(name, value, inPlace)
            elif isinstance(exemplar, str):
                self.addString(name, value, inPlace)
            elif isinstance(exemplar, lsst.daf.base.DateTime):
                self.addDateTime(name, value, inPlace)
            else:
                raise lsst.pex.exceptions.Exception, \
                    'Unknown value type for %s: %s' % (name, type(value))
        else:
            if isinstance(exemplar, bool):
                self.addBool(name, value, comment, inPlace)
            elif isinstance(exemplar, int):
                self.addInt(name, value, comment, inPlace)
            elif isinstance(exemplar, long):
                self.addLongLong(name, value, comment, inPlace)
            elif isinstance(exemplar, float):
                self.addDouble(name, value, comment, inPlace)
            elif isinstance(exemplar, str):
                self.addString(name, value, comment, inPlace)
            elif isinstance(exemplar, lsst.daf.base.DateTime):
                self.addDateTime(name, value, comment, inPlace)
            else:
                raise lsst.pex.exceptions.Exception, \
                    'Unknown value type for %s: %s' % (name, type(value))

    def toList(self):
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

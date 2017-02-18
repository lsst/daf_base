from __future__ import absolute_import

from ._dateTime import DateTime

def DateTime_toPython(self, timescale=None):
    """Convert a DateTime to Python's datetime

    @param timescale  Timescale for resultant datetime
    """
    import datetime
    nsecs = self.nsecs(timescale) if timescale is not None else self.nsecs()
    return datetime.datetime.utcfromtimestamp(nsecs/10**9)
DateTime.toPython = DateTime_toPython
del DateTime_toPython

def DateTime__repr__(self):
    if self.isValid():
        return "DateTime(\"{}\", TAI)".format(self.toString(DateTime.TAI))
    else:
        return "DateTime()"

DateTime.__repr__ = DateTime__repr__
del DateTime__repr__

def DateTime__reduce__(self): 
    return (DateTime, (self.nsecs(), )) 

DateTime.__reduce__ = DateTime__reduce__
del DateTime__reduce__


from __future__ import absolute_import

__all__ = []

from lsst.utils import continueClass

from .dateTime import DateTime

@continueClass
class DateTime:
    def toPython(self, timescale=None):
        """Convert a DateTime to Python's datetime
    
        @param timescale  Timescale for resultant datetime
        """
        import datetime
        nsecs = self.nsecs(timescale) if timescale is not None else self.nsecs()
        return datetime.datetime.utcfromtimestamp(nsecs/10**9)
    
    def __repr__(self):
        if self.isValid():
            return "DateTime(\"{}\", TAI)".format(self.toString(DateTime.TAI))
        else:
            return "DateTime()"
    
    def __reduce__(self): 
        return (DateTime, (self.nsecs(), )) 


from _dateTime import DateTime

def toPython(self, timescale=None):
    """Convert a DateTime to Python's datetime

    @param timescale  Timescale for resultant datetime
    """
    import datetime
    nsecs = self.nsecs(timescale) if timescale is not None else self.nsecs()
    return datetime.datetime.utcfromtimestamp(nsecs/10**9)

DateTime.toPython = toPython

#
# LSST Data Management System
#
# Copyright 2008-2017  AURA/LSST.
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
# see <http://www.lsstcorp.org/LegalNotices/>.
#

from __future__ import absolute_import, division, print_function

__all__ = []

from lsst.utils import continueClass

from .dateTime import DateTime


@continueClass  # noqa F811
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

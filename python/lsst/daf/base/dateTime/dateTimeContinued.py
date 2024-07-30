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

__all__ = ["DateTime"]

from lsst.utils import continueClass

from .._dafBaseLib import DateTime


@continueClass  # noqa: F811 (FIXME: remove for py 3.8+)
class DateTime:  # noqa: F811

    def toPython(self, timescale=None):
        """Convert a DateTime to Python's datetime

        Parameters
        ----------
        timescale : `dateTime.DateTime.Timescale`, optional
            Timescale for resultant datetime.

        Returns
        -------
        datetime : `datetime.datetime`
            The resultant Python `datetime.datetime` object.

        Raises
        ------
        ValueError
            Raised if the DateTime is invalid (uninitialized).
        """
        import datetime
        if not self.isValid():
            raise RuntimeError("DateTime not valid")
        nsecs = self.nsecs(timescale) if timescale is not None else self.nsecs()
        return datetime.datetime.utcfromtimestamp(nsecs/10**9)

    def toAstropy(self, timescale=None):
        """Convert a DateTime to an astropy Time.

        Returns
        -------
        time : `astropy.time.Time`
            This date as an astropy MJD/TAI time.

        Raises
        ------
        ValueError
            Raised if the DateTime is invalid (uninitialized).
        """
        # So that astropy is not an import-time dependency of daf_base.
        import astropy.time
        return astropy.time.Time(self.get(system=DateTime.MJD, scale=DateTime.TAI), scale="tai", format="mjd")

    def __repr__(self):
        if self.isValid():
            return "DateTime(\"{}\", TAI)".format(self.toString(DateTime.TAI))
        else:
            return "DateTime()"

    def __reduce__(self):
        return (DateTime, (self.nsecs(), ))

    def __lt__(self, other):
        if not isinstance(other, type(self)):
            return NotImplemented
        # Invalid date has very negative nsecs().
        return self.nsecs() < other.nsecs()

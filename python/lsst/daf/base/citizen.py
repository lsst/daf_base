from __future__ import print_function
from builtins import range
import re
import lsst.daf.base as dafBase


def setCallbacks(new=None, delete=None, both=False):
    """Set the callback IDs for the `Citizen`.

    These callbacks can be used within :command:`gdb`::

        break defaultNewCallback
        break defaultDeleteCallback

    Enable these by default by including the above lines in a ``~/.gdbinit``
    file.

    You can retrieve a `Citizen`\ 's object's signature with
    `lsst.daf.base.Citizen.repr`.

    Parameters
    ----------
    new : `obj`, optional
        Callback ID for creation.
    delete : `obj`, optional
        Callback ID for deletion.
    both : `bool`, optional
        If `True`, set both `new` and `delete` to the same value.
    """

    if both:
        if new:
            if delete and new != delete:
                raise RuntimeError("You may not specify new, delete, and both")
            delete = new
        else:
            new = delete

    if new:
        dafBase.Citizen.setNewCallbackId(new)
    if delete:
        dafBase.Citizen.setDeleteCallbackId(delete)


def mortal(memId0=0, nleakPrintMax=20, first=True, showTypes=None):
    """Print leaked memory blocks.

    You can get the next memory ID to be allocated with ``mortal("set")``,
    e.g.::

       memId0 = mortal("set")
       # ...
       mortal(memId0)

    Parameters
    ----------
    memId0 : `int`
        Only consider blocks allocated after this memory ID.
    nleakPrintMax : `int`
        Maximum number of leaks to print. Negative `nleakPrintMax` means
        unlimited printouts.
    first : `bool`
        Print the first `nleakPrintMax` blocks. If `False`, print the last
        blocks.
    showTypes : `str`
        Only print objects matching this regex. If `showTypes` starts
        with ``!``, objects that *don't* match are printed.

    See also
    --------
    :meth:`dafBase.Citizen.census` : Provides the entire list, allowing
        you to imlement custom filtering.
    """

    if memId0 == 'set':
        return dafBase.Citizen.getNextMemId()

    nleak = dafBase.Citizen.census(0, memId0)
    if nleak != 0:
        print("%d Objects leaked" % dafBase.Citizen.census(0, memId0))

        census = dafBase.Citizen.census()
        census = [census[i].repr() for i in range(len(census))]  # using [i] for some swiggy reason
        if showTypes:
            if showTypes[0] == '!':
                invert = True           # invert the matching logic
                showTypes = showTypes[1:]
            else:
                invert = False

            _census, census = census, []
            for c in _census:
                memId, addr, dtype = c.split()
                memId = int(memId[:-1])

                if \
                        (not invert and re.search(showTypes, dtype)) or \
                        (invert and not re.search(showTypes, dtype)):
                    census.append(c)

            nleak = len(census)
            print("%d leaked objects match" % nleak)

        if nleakPrintMax <= 0 or nleak <= nleakPrintMax:
            for c in census:
                memId, addr, type = c.split()
                memId = int(memId[:-1])
                if memId >= memId0:
                    print(c)
        else:
            print("...")
            for i in range(nleakPrintMax - 1, -1, -1):
                print(census[i])

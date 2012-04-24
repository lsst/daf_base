import re
import lsst.daf.base as dafBase

def setCallbacks(new=None, delete=None, both=False):
    """Set the callback IDs for the Citizen; if both is true, set both new and delete to the same value

You probably want to chant the following to gdb:
   break defaultNewCallback
   break defaultDeleteCallback

You might want to put this in your .gdbinit file.

You can retrieve a citizen's signature from python with obj.repr()
    """

    if both:
        if new:
            if delete and new != delete:
                raise RuntimeError, "You may not specify new, delete, and both"
            delete = new
        else:
            new = delete

    if new:
        dafBase.Citizen.setNewCallbackId(new)
    if delete:
        dafBase.Citizen.setDeleteCallbackId(delete)

def mortal(memId0=0, nleakPrintMax=20, first=True, showTypes=None):
    """Print leaked memory blocks
    @param memId0 Only consider blocks allocated after this memId
    @param nleakPrintMax Maximum number of leaks to print; <= 0 means unlimited
    @param first Print the first nleakPrintMax blocks; if False print the last blocks.
    @param showTypes Only print objects matching this regex (if starts with !, print objects that don't match)

    If you want finer control than nleakPrintMax/first provide, use
    dafBase.Citizen.census() to get the entire list

You can get the next memId to be allocated with mortal("set"), e.g.
    memId0 = mortal("set")
    # work work work
    mortal(memId0)
    """
    
    if memId0 == 'set':
        return dafBase.Citizen.getNextMemId()

    nleak = dafBase.Citizen.census(0, memId0)
    if nleak != 0:
        print "%d Objects leaked" % dafBase.Citizen.census(0, memId0)

        census = dafBase.Citizen.census()
        census = [census[i].repr() for i in range(len(census))] # using [i] for some swiggy reason
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
            print "%d leaked objects match" % nleak
            
        if nleakPrintMax <= 0 or nleak <= nleakPrintMax:
            for c in census:
                memId, addr, type = c.split()
                memId = int(memId[:-1])
                if memId >= memId0:
                    print c
        else:
            print "..."
            for i in range(nleakPrintMax - 1, -1, -1):
                print census[i]

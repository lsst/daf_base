import lsst.daf.base as dafBase

def setCallbacks(new=None, delete=None, both=False):
    """Set the callback IDs for the Citizen; if both is true, set both new and delete to the same value

You probably want to chant the following to gdb:
   break 'lsst::daf::base::defaultNewCallback(lsst::daf::base::Citizen const*)' 
   break 'lsst::daf::base::defaultDeleteCallback(lsst::daf::base::Citizen const*)' 

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

def mortal(memId0=0, nleakPrintMax=20):
    """Print leaked memory blocks
    @param memId0 Only consider blocks allocated after this memId
    @param nleakPrintMax Maximum number of leaks to print; 0 => unlimited

You can get the next memId to be allocated with mortal("set"), e.g.
    memId0 = mortal("set")
    # work work work
    mortal(memId0)
    """
    
    if memId0 == 'set':
        return dafBase.Citizen.getNextMemId()

    nleak = dafBase.Citizen.census(0, memId0)
    if nleak != 0:
        print "\n%d Objects leaked:" % dafBase.Citizen.census(0, memId0)

        if nleak <= nleakPrintMax:
            print dafBase.Citizen.census(dafBase.cout, memId0)
        else:
            census = dafBase.Citizen.census()
            print "..."
            for i in range(nleakPrintMax - 1, -1, -1):
                print census[i].repr()

        print "Leaked %d blocks" % dafBase.Citizen.census(0, memId0)

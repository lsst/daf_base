// -*- LSST-C++ -*-
//! \file
//! \brief Implementation of Citizen

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <ctype.h>
#include "lsst/daf/base/Citizen.h"
#include "lsst/pex/exceptions.h"
#include "lsst/utils/Demangle.h"

namespace lsst {
namespace daf {
namespace base {

//! Called once when the memory system is being initialised
//
// \brief A class that is instantiated once during startup
//
// The main purpose of CitizenInit is to provide a place to set
// breakpoints to setup memory debugging; see discussion on trac
//
class CitizenInit {
public:
    CitizenInit() : _dummy(1) {
    }
private:
    volatile int _dummy;
};
    
CitizenInit one;
//
// Con/Destructors
//
Citizen::Citizen(const std::type_info &type) :
    _sentinel(magicSentinel),
    _typeName(type.name()) {
    _CitizenId = _nextMemId()++;
    activeCitizens()[_CitizenId] = this;

    if (_CitizenId == _newId) {
        _newId += _newCallback(this);
    }
}

Citizen::Citizen(Citizen const & citizen) :
    _sentinel(magicSentinel),
    _typeName(citizen._typeName) {
    _CitizenId = _nextMemId()++;
    activeCitizens()[_CitizenId] = this;

    if (_CitizenId == _newId) {
        _newId += _newCallback(this);
    }
}

Citizen::~Citizen() {
    if (_CitizenId == _deleteId) {
        _deleteId += _deleteCallback(this);
    }

    (void)_checkCorruption();
    _sentinel = 0x0000dead;             // In case we have a dangling pointer
    size_t nActive = activeCitizens().erase(_CitizenId);
    if (nActive > 1 || (nActive == 0 && permanentCitizens().erase(_CitizenId) != 1)) {
        (void)_corruptionCallback(this);
    }
}

//! Called once when the memory system is being initialised
//
// The main purpose of this routine is as a place to set
// breakpoints to setup memory debugging; see discussion on trac
//
int Citizen::init() {
    volatile int dummy = 1;
    return dummy;
}

/******************************************************************************/
//
// Return (some) private state
//
//! Return the Citizen's ID
Citizen::memId Citizen::getId() const {
    return _CitizenId;
}

//! Return the memId of the next object to be allocated
Citizen::memId Citizen::getNextMemId() {
    return _nextMemId();
}

//! Return the memId of the next object to be allocated
Citizen::memId& Citizen::_nextMemId() {
    static memId next = Citizen::init();
    return next;
}

//! Return a string representation of a Citizen
//
std::string Citizen::repr() const {
    return boost::str(boost::format("%d: %08x %s")
                      % _CitizenId
                      % this
                      % lsst::utils::demangleType(_typeName)
                     );
}

//! Mark a Citizen as persistent and not destroyed until process end.
void Citizen::markPersistent(void) {
    activeCitizens().erase(_CitizenId);
    permanentCitizens()[_CitizenId] = this;
}

//! \name Census
//! Provide a list of current Citizens
//@{
//
//
//! How many active Citizens are there?
//
int Citizen::census(
    int,                                //<! the int argument allows overloading
    memId startingMemId                 //!< Don't print Citizens with lower IDs
    ) {
    if (startingMemId == 0) {              // easy
        return activeCitizens().size();
    }

    int n = 0;
    for (table::iterator cur = activeCitizens().begin();
         cur != activeCitizens().end(); cur++) {
        if (cur->second->_CitizenId >= startingMemId) {
            n++;
        }
    }

    return n;    
}
//
//! Print a list of all active Citizens to stream
//
void Citizen::census(
    std::ostream &stream,               //!< stream to print to
    memId startingMemId                 //!< Don't print Citizens with lower IDs
    ) {
    for (table::iterator cur = activeCitizens().begin();
         cur != activeCitizens().end(); cur++) {
        if (cur->second->_CitizenId >= startingMemId) {
            stream << cur->second->repr() << "\n";
        }
    }
}
//
//! Return a (newly allocated) std::vector of active Citizens
//
//! You are responsible for deleting it; or you can say
//!    boost::scoped_ptr<const std::vector<const Citizen *> >
//!					leaks(Citizen::census());
//! and not bother
//
const std::vector<const Citizen *> *Citizen::census() {
    std::vector<const Citizen *> *vec =
        new std::vector<const Citizen *>(0);
    vec->reserve(activeCitizens().size());

    for (table::iterator cur = activeCitizens().begin();
         cur != activeCitizens().end(); cur++) {
        vec->push_back(dynamic_cast<const Citizen *>(cur->second));
    }
        
    return vec;
}
//@}

//! Check for corruption
//! Return true if the block is corrupted, but
//! only after calling the corruptionCallback
bool Citizen::_checkCorruption() const {
    if (_sentinel == static_cast<int>(magicSentinel)) {
        return false;
    }

    (void)_corruptionCallback(this);
    return true;
}

//! Check all allocated blocks for corruption
bool Citizen::checkCorruption() {
    for (table::iterator cur = activeCitizens().begin();
         cur != activeCitizens().end(); cur++) {
        if (cur->second->_checkCorruption()) {
            return true;
        }
    }
    for (table::iterator cur = permanentCitizens().begin();
         cur != permanentCitizens().end(); cur++) {
        if (cur->second->_checkCorruption()) {
            return true;
        }
    }

    return false;
}

//! \name callbackIDs
//! Set callback Ids. The old Id is returned
//@{
//
//! Call the NewCallback when block is allocated
Citizen::memId Citizen::setNewCallbackId(
    Citizen::memId id                   //!< Desired ID
    ) {
    Citizen::memId oldId = _newId;
    _newId = id;

    return oldId;
}

//! Call the current DeleteCallback when block is deleted
Citizen::memId Citizen::setDeleteCallbackId(
    Citizen::memId id                   //!< Desired ID
    ) {
    Citizen::memId oldId = _deleteId;
    _deleteId = id;

    return oldId;
}
//@}

//! \name callbacks
//! Set the New/Delete callback functions; in each case
//! the previously installed callback is returned. These
//! callback functions return a value which is Added to
//! the previously registered id.
//!
//! The default callback functions are called
//! default{New,Delete}Callback; you may want to set a break
//! point in these callbacks from your favourite debugger
//

//@{
//! Set the NewCallback function

Citizen::memCallback Citizen::setNewCallback(
    Citizen::memCallback func //! The new function to be called when a designated block is allocated
    ) {
    Citizen::memCallback old = _newCallback;
    _newCallback = func;

    return old;
}

//! Set the DeleteCallback function
Citizen::memCallback Citizen::setDeleteCallback(
    Citizen::memCallback func           //!< function be called when desired block is deleted
    ) {
    Citizen::memCallback old = _deleteCallback;
    _deleteCallback = func;

    return old;
}
    
//! Set the CorruptionCallback function
Citizen::memCallback Citizen::setCorruptionCallback(
	Citizen::memCallback func //!< function be called when block is found to be corrupted
                                                   ) {
    Citizen::memCallback old = _corruptionCallback;
    _corruptionCallback = func;

    return old;
}
    
//! Default callbacks.
//!
//! Note that these may well be the target of debugger breakpoints, so e.g. dId
//! may well be changed behind our back
//@{
//! Default NewCallback
Citizen::memId defaultNewCallback(const Citizen *ptr //!< Just-allocated Citizen
                                 ) {
    static int dId = 0;             // how much to incr memId
    std::cerr << boost::format("Allocating memId %s\n") % ptr->repr();

    return dId;
}

//! Default DeleteCallback
Citizen::memId defaultDeleteCallback(const Citizen *ptr //!< About-to-be deleted Citizen
                                    ) {
    static int dId = 0;             // how much to incr memId
    std::cerr << boost::format("Deleting memId %s\n") % ptr->repr();

    return dId;
}

//! Default CorruptionCallback
Citizen::memId defaultCorruptionCallback(const Citizen *ptr //!< About-to-be deleted Citizen
                              ) {
    throw LSST_EXCEPT(lsst::pex::exceptions::MemoryException,
                      str(boost::format("Citizen \"%s\" is corrupted") % ptr->repr()));

    return ptr->getId();                // NOTREACHED
}

Citizen::table& Citizen::activeCitizens(void) {
    static Citizen::table* _activeCitizens = new Citizen::table;
    return *_activeCitizens;
}

Citizen::table& Citizen::permanentCitizens(void) {
    static Citizen::table* _permanentCitizens = new Citizen::table;
    return *_permanentCitizens;
}

//@}
//
// Initialise static members
//
Citizen::memId Citizen::_newId = 0;
Citizen::memId Citizen::_deleteId = 0;

Citizen::memCallback Citizen::_newCallback = defaultNewCallback;
Citizen::memCallback Citizen::_deleteCallback = defaultDeleteCallback;
Citizen::memCallback Citizen::_corruptionCallback = defaultCorruptionCallback;

}}} // namespace lsst::daf::base

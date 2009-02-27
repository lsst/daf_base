// -*- lsst-c++ -*-

/** \file
 * \brief Implementation of Persistable base class
 *
 * \author $Author$
 * \version $Revision$
 * \date $Date$
 *
 * Contact: Kian-Tat Lim (ktl@slac.stanford.edu)
 *
 * \ingroup daf_base
 */

#ifndef __GNUC__
#  define __attribute__(x) /*NOTHING*/
#endif
static char const* SVNid __attribute__((unused)) = "$Id$";

#include "lsst/daf/base/Persistable.h"

namespace lsst {
namespace daf {
namespace base {

/** Default constructor
 */
Persistable::Persistable(void) {
}

/** Destructor
 */
Persistable::~Persistable(void) {
}

}}} // namespace lsst::daf::base

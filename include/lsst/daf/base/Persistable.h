// -*- lsst-c++ -*-
#ifndef LSST_DAF_BASE_PERSISTABLE_H
#define LSST_DAF_BASE_PERSISTABLE_H

/** @file
  * @ingroup daf_base
  *
  * @brief Interface for Persistable base class
  *
  * @author Kian-Tat Lim (ktl@slac.stanford.edu)
  * @version $Revision$
  * @date $Date$
  */

/** @class lsst::daf::base::Persistable
  * @brief Base class for all persistable classes.
  *
  * This class must be a public base class for all persistable classes.  It
  * provides the persist() method used to store an instance of the persistable
  * class in a logical location.  It also provides a macro used to connect the
  * persistable class with its Formatter subclass and to delegate
  * @c boost::serialization persistence to that subclass.
  *
  * @ingroup daf_base
  */

#include <boost/shared_ptr.hpp>
#include <typeinfo>

namespace lsst {
namespace daf {
namespace base {

// Forward declarations.
class Persistable;

} // namespace lsst::daf::base

namespace persistence {
// Template global function used to connect with boost::serialization.
// Definition is in daf/persistence/FormatterImpl.h, but is not needed by
// Persistable clients, only Formatter subclasses.
template <class Form, class Archive>
    void delegateSerialize(Archive& ar, unsigned int const version,
                           lsst::daf::base::Persistable* persistable);

} // namespace lsst::daf::persistence

namespace base {

class Persistable {
public:
    typedef boost::shared_ptr<Persistable> Ptr;

    Persistable(void);
    virtual ~Persistable(void);

    /** Template boost::serialization function for the base class.  Doesn't
      * actually do anything, since there is no data associated with the base.
      * Inline to allow expansion wherever needed.
      */
    template <class Archive>
    void serialize(Archive& ar, unsigned int const version) { };
};

/** Macro used to connect the persistable class with the Formatter and
  * @c boost::serialization
  *
  * Provides friend access to @c boost::serialization and the Formatter
  * subclass.  Also provides a template serialization function (inline, so
  * templates can be expanded by @c g++ in the "Borland" style).
  *
  * @param formatter Name of formatter class with template parameters, if any.
  */
#define LSST_PERSIST_FORMATTER(formatter...) \
    friend class boost::serialization::access; \
    friend class formatter; \
    template <class Archive> \
    void serialize(Archive& ar, unsigned int const version) { \
        lsst::daf::persistence::delegateSerialize<formatter, Archive>(ar, version, this); \
    };

}}} // namespace lsst::mwi::base

// Forward declaration of the boost::serialization::access class.
namespace boost {
namespace serialization {
class access;
}} // namespace boost::serialization

#endif

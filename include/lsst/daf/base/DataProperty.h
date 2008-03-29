// -*- lsst-c++ -*-
/**
  * \class DataProperty
  *
  * \ingroup daf_base
  *
  * \author Jeff Bartels
  * 
  * Contact: jeffbartels@usa.net
  * 
  * Created: 29-Jun-2007 3:05:07 PM
  */

/*
 $Author::                                                                 $
 $Rev::                                                                    $
 $Date::                                                                   $
 $Id::                                                                     $
 */ 

#ifndef LSST_DAF_BASE_DATAPROPERTY_H
#define LSST_DAF_BASE_DATAPROPERTY_H

#include <string>
#include <list> 
#include <set>

#include <boost/any.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
// #include <boost/serialization/export.hpp>

#include "lsst/daf/base/Citizen.h"
#include "lsst/daf/base/Persistable.h"

namespace lsst {
namespace daf {

namespace data {
    class DataPropertyFormatter;
} // namespace lsst::daf::data

namespace base {


/** 
  * 
  * A DataProperty encapsulates a name, value pair. 
  *
  * Since the value of a given property object may be a collection of 
  * DataProperty objects, a DataProperty may either act as a 
  * a node in a heirarchy(tree) of DataProperties, or as a leaf in such a tree.
  * This characteristic is initially established at construction time by passing
  * either a ContainerType value (to create a node) or a boost::any type value
  * (to create a leaf). The overloaded setValue member function also provides
  * a way to mutate this characteristic of a DataProperty object.
  * 
  * Operations which are appropriate on node-valued instances will throw
  * exceptions when called on a leaf-valued instance.
  *
  * \note
  * For node-type objects, the collection of children DataProperty objects is 
  * ordered by sequence of insertion (with one caveat as explained below). This ordering cannot 
  * be changed. Access to the content of the collection of children is via iterators (the 
  * collection is never returned directly by any method of this interface). 
  * When a child is added to such an object's collection it is always appended to the end of 
  * the collection, thus any returned iterator will always sequence from first to last 
  * element in order of insertion. Note that elements can be removed from the 
  * collection ( see deleteAll() ), however this does not change the fundamental ordering of
  * the collection. One caveat is that since the node-type characteristic is established by
  * constructing a DataProperty object or setting its value using a ContainerType instance, 
  * the content and ordering of that instance is left completely intact so the ordering of this
  * content is unknown. Any subsequent addProperty operations on the instance will append 
  * collection members as previously described.
  * 
  * \note    
  * Note the find and search operations on DataProperty implement access to 
  * nested properties using a simple String criterion constructed of labels
  * (i.e. "A.B.C" to access grandchild "C" of "A"), or by using regular expressions
  * to find nested properties using pattern matching (i.e. "^C$" to find "C" 
  * somewhere underneath "A").
  * 
  */
class DataProperty : public Persistable, public Citizen
{

public:
    typedef boost::shared_ptr<DataProperty> PtrType;
    typedef std::list<PtrType> ContainerType;
    typedef std::list<PtrType>::const_iterator ContainerIteratorType;
    typedef std::set<std::string> nameSetType;
    typedef std::pair<ContainerIteratorType, ContainerIteratorType> iteratorRangeType;
    
    DataProperty(std::string name, boost::any value = boost::any());
    DataProperty(std::string name, ContainerType& value );
    DataProperty(const DataProperty& orig);
    DataProperty& operator= (const DataProperty& rhs ); 
    virtual ~DataProperty();

    const std::string& getName() const;
    const boost::any& getValue() const;

    void setName(const std::string name);
    void setValue(const boost::any& value);
    void setValue(const ContainerType& value);

    iteratorRangeType getChildren() const;
    void addProperty(const DataProperty& dp);
    void addProperty(PtrType const& prop);
    void addChildren(PtrType const& prop);

    PtrType findUnique( const std::string& criteria, const bool deep = true ) const;
    iteratorRangeType findAll( const std::string& criteria, const bool deep = true ) const;
    iteratorRangeType searchAll( const std::string& regexSpec, const bool deep = true ) const;
    nameSetType findNames( const std::string& regexSpec ) const;

    void deleteAll( const std::string& regexSpec, const bool deep = true );
    
    const bool isNode() const;

    const std::string toString(
        const std::string& prefix = "", bool deep = false ) const;

private:
    LSST_PERSIST_FORMATTER(lsst::daf::data::DataPropertyFormatter);

    typedef std::vector<std::string> stringVectorType;

    DataProperty();
    
    void _cloneValue( const DataProperty& orig );
    void _eraseChildren();
    void _cloneCollection( const DataProperty& source );

    nameSetType& _findDescendantNames(
        nameSetType& ret,
        const boost::regex& spec,
        const std::string& nameRoot ) const ;
    
    PtrType _recurseFindDescendant(
        stringVectorType::const_iterator nameSegs,
        stringVectorType::const_iterator end_of_nameSegs ) const ;

    void _recurseFindAllDescendants(
        ContainerType& results,
        stringVectorType::const_iterator nameSegs,
        stringVectorType::const_iterator end_of_nameSegs,
        bool recurse ) const;
    void _recurseSearchDescendantNames(
        ContainerType& results,
        const boost::regex& spec, 
        const std::string& nameRoot,
        bool recurse ) const;

    void _recurseDeleteDescendants( const boost::regex& spec, bool recurse );

    void _resetFindSearchResult() const;

    std::string _name;
    boost::any _value;

    ContainerType _collectionValue;
    
    bool _isANode;

    mutable ContainerType _findSearchResult;
};

}}} // namespace lsst::daf::base

#endif // LSST_DAF_BASE_DATAPROPERTY_H

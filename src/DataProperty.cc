// -*- lsst-c++ -*-
/**
  * \file DataProperty.cc
  *
  * \ingroup daf_base
  *
  * \author Jeff Bartels
  *
  * Contact: jeffbartels@usa.net
  *
  */

/*
 $Author::                                                                 $
 $Rev::                                                                    $
 $Date::                                                                   $
 $Id::                                                                     $
 */ 

#include <sstream>
#include <string>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/format.hpp>

#include "lsst/daf/base/DataProperty.h"

using namespace std;

int const EXEC_TRACE = 20;
static void execTrace( string s, int level = EXEC_TRACE){
    // Cannot use Trace since move to daf/base.  Do nothing for now.
}


namespace lsst {
namespace daf {
namespace base {

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS, COPY OPERATORS, DESTRUCTORS
//////////////////////////////////////////////////////////////////////////////


/** Default constructor for a new DataProperty object.
  */
DataProperty::DataProperty() : Citizen(typeid(this)), _isANode(false) {
    execTrace("Enter DataProperty::DataProperty()");
    
    setName("");
    setValue(boost::any());
    
    execTrace( boost::str( 
        boost::format("Exit DataProperty::DataProperty() : %s") 
            % this->toString()));
}
    
/** Construct a new DataProperty object.
  *
  * \param name The name of the object. Note that any '.' characters in the
  *               name are disallowed and will be replaced with '@' characters
  * \param value The value of the DataProperty (defaults to an empty boost::any
  */
DataProperty::DataProperty( std::string name, boost::any value )
    : Citizen(typeid(this)), _isANode(false) 
{
    execTrace("Enter DataProperty::DataProperty(name, value)");
    
    setName(name);
    setValue(value);
    
    execTrace( boost::str( 
        boost::format("Exit DataProperty::DataProperty(name,value) : %s") 
            % this->toString()));
}


/** Construct a new DataProperty object as a node.
  * 
  * \param name The name of the object. Note that any '.' characters in the
  *               name are disallowed and will be replaced with '@' characters
  * \param value A collection of DataProperty::PtrType
  */
DataProperty::DataProperty(
    std::string name, DataProperty::ContainerType& value )
        : Citizen(typeid(this)), _isANode(false) {
    execTrace("Enter DataProperty::DataProperty(name, collectionValue)");
    
    setName(name);
    setValue(value);
    
    execTrace( boost::str( 
        boost::format("Exit DataProperty::DataProperty(name, collectionValue) : %s") 
            % this->toString()));
}


    
/**DataProperty copy constructor.
  *
  * Is an clone operation. The resulting DataProperty object will be completely
  * independent of the source object, including all descendants, if there are
  * any.
  *
  * \param orig A reference to the DataProperty to clone.
  */
DataProperty::DataProperty(const DataProperty& orig) 
        : Citizen(typeid(this)), _name(orig._name), _isANode(false) {
    execTrace("Enter DataProperty::DataProperty(DataProperty&)");
    
    _cloneValue(orig);
    
    execTrace( boost::str( 
        boost::format("Exit DataProperty::DataProperty(DataProperty&) : %s") 
            % this->toString()));
}


/** DataProperty assignment operator
  * 
  * Implements DataProperty assignment semantics for the lhs of
  * an assignment statement involving two DataProperty objects. 
  * Overwrites the content of the lhs and then clones the content of the rhs
  * object into the lhs. 
  *
  * \note If the lhs' value is a collection of DataProperty (i.e. is a node),
  * the contents of that collection will be erased (per the semantics of 
  * std::list::erase prior to cloning. This may cause the collected 
  * instances to be destroyed per boost::shared_ptr semantics)
  *
  * \note The original base Citizen of the lhs operand is left intact
  */
DataProperty& DataProperty::operator= (const DataProperty& rhs ) 
{
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::operator= (const DataProperty&) : %s") 
                % this->toString()));

    this->_name = std::string(rhs._name);

    _cloneValue(rhs);

    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::operator= (const DataProperty&) : %s") 
                % this->toString()));
    return *this;
}


/** DataProperty destructor
  * 
  * \note If the lhs' value is a collection of DataProperty (i.e. is a node),
  * the contents of that collection will be erased (per the semantics of 
  * std::list::erase. This may cause the collected instances to be destroyed 
  * per boost::shared_ptr semantics)
  */
DataProperty::~DataProperty(){
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::~DataProperty() : %s") 
                % this->toString()));
    if( this->_isANode ) {
        _eraseChildren();
    }
    execTrace( "Exit DataProperty::~DataProperty()" );
}


/** Return a const copy of the DataProperty's name
  */
const std::string& DataProperty::getName() const {
    return _name;
}


/** Return the boost::any value attribute of this object. 
  *
  * \note The value may be tested for specific type by performing a 
  * boost::any_cast<>() on the returned reference value within a 
  * try-catch block.
  * 
  * \note If the object is a node, then the boost::any value returned will be empty
  */
const boost::any& DataProperty::getValue() const { 
    return _value;
}


/** Set the name attribute of the DataProperty object
  *
  * \param name The name of the object. Note that any '.' characters in the
  *               name are disallowed and will be replaced with '@' characters
  */
void DataProperty::setName(const std::string name) {
    // enforce no '.' characters in the DataProperty name
    _name = name;
    if( _name.find_first_of(".",0) ) {
        boost::algorithm::replace_all(_name,".","@");
    }
}

/** Set the value of the DataProperty object. 
  *
  * \param value A boost::any object containing the value
  *
  * \note Will mutate a node DataProperty object into a leaf
  *
  * \note If the value of the object is currently a collection of DataProperty, then 
  * the contents of the collection will be erased.  
  */
void DataProperty::setValue(const boost::any& value) {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::setValue (boost::any& value) : %s") 
                % this->toString()));

    if( this->_isANode ) {
        _eraseChildren();
        _collectionValue = ContainerType();
    }
    _value = value;
    _isANode = false;

    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::setValue (boost::any& value) : %s") 
                % this->toString()));
}


/**
  * Sets the value of the DataProperty object by creating a copy of the given
  * collection and its contents.  If the object is currently
  * a node, then the contents of the aggregated collection will first be erased.  
  *
  * \param value A boost::any object containing the value
  *
  * \note Will mutate a leaf DataProperty object into a node 
  *
  * \note If the value of the object is currently a collection of DataProperty, then 
  * the contents of the collection will be erased.  
  */
void DataProperty::setValue(const DataProperty::ContainerType& value) {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::setValue (ContainerType& value) : %s") 
                % this->toString()));

    if( this->_isANode ) {
        _eraseChildren();
    }
    _collectionValue = ContainerType(value);
    _isANode = true;

    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::setValue (ContainerType& value) : %s") 
                % this->toString()));
}


/** Return an iterator to the collection of children properties owned by this object. 
 * 
 * \return an iteratorRangeType containing a begin/end const_iterator pair
 *         to allow iteration over the child proerties of this object.
 *
 * \note The number of items in the range of iteration can be tested by calling the 
         std::distance(range.first, range.second) using the .first and .second members of
         the pair.
 * \throw Throws std::runtime_error if this object is not a node
 *
 */
DataProperty::iteratorRangeType DataProperty::getChildren() const { 
    if( _isANode == false )
        throw std::runtime_error( "DataProperty object is not a node" );
    return std::make_pair( _collectionValue.begin(), _collectionValue.end() );
}


/** Add a copy of the given object to this object as a child property.
  *
  * \param dp A reference to a DataProperty object
  *
  * \note To insure that the new child object will not be inadvertantly destroyed,
  *       a copy of the object is created and the copy stored as the child.
  *
  * \return Returns false if this object is not a node (i.e. its value is not a 
  *         DataProperty::propertyCollectionType)
  * \throw Throws std::runtime_error if this object is not a node
  *
  */
void DataProperty::addProperty(const DataProperty& dp) {
    if( _isANode == false )
        throw std::runtime_error( "DataProperty object is not a node" );
    else {
        PtrType newItem( new DataProperty(dp) );
        _collectionValue.push_back(newItem);
    }
}


/** Adds the given object to this object as a child property.
  *
  * \param prop A reference to a boost::shared_ptr containing a DataProperty object
  *
  * \return Returns false if this object is not a node (i.e. its value is not a 
  *         DataProperty::propertyCollectionType)
  * \throw Throws std::runtime_error if this object is not a node
  *
  */
void DataProperty::addProperty(DataProperty::PtrType const& prop) {
    if( _isANode == false )
        throw std::runtime_error( "DataProperty object is not a node" );
    else {
        _collectionValue.push_back(prop);
    }
}

/** Adds the children of the given DataProperty to this DataProperty,
 * replacing the values of any that already exist.
 * \param prop A reference to a boost::shared_ptr containing a DataProperty object
  * \throw Throws std::runtime_error if this object is not a node
 */
void DataProperty::addChildren(DataProperty::PtrType const& prop) {
    if (_isANode == false) {
        throw std::runtime_error( "DataProperty object is not a node" );
    }
    DataProperty::iteratorRangeType range = prop->getChildren();
    for (DataProperty::ContainerIteratorType iter = range.first;
         iter != range.second; iter++) {
        DataProperty::PtrType dp;
        try {
            dp = findUnique((*iter)->getName());
        }
        catch (...) {
        }
        if (dp) {
            dp->setValue((*iter)->getValue());
        }
        else {
            addProperty(*iter);
        }
    }
}

/** Returns a DataProperty object that exists beneath the current object at a
 * location given by the search criteria string.
 * 
 * \param criteria A string containing one or more names, separated by dot '.'
 *                 characters (i.e. "Parent.Child.Grandchild" ). See notes.
*
 * \param deep A boolean flag that controls whether or not the operation will
 *             recurse beneath the level of this object to children nodes. Defaults
 *             to true. If true, recursion will proceed as a depth-first 
 *             traversal tree of nodes descending from this object, if false,
 *             will only iterate over the children of this object.
 *
  *
 * \return A PtrType containing a reference to the object found. If none
 *         is found matching the given search criteria, then an empty PtrType
 *         is returned.
 * 
 * \throw Throws std::runtime_error if this object is not a node
 *
 * \throw Throws std::runtime_error if the given name occurs more than once 
 *        (i.e. multiple children of one node having the same name)
 *
 * \note The string is parsed by splitting it on '.' characters into a list of names. 
 * The list of names is processed in sequence from beginning to end by recursively
 * examining the descendants of this object. 
 * If the deep parameter is set to false, an object having name(1) must exist
 * as a direct descendant of this object. If that is true, then an object having
 * name(2) must exist as a direct descendant of object.name(1), and so on. 
 * If the deep parameter is set to true, then the method will recurse through the tree
 * of children under this object in a depth-first traversal, if necessary, to find
 * and an indirect descendant (i.e. great-grandchild) that satisfies the search
 * criteria.
 * In order for the find operation to succeed, the list of names in the search criteria
 * must be exhausted
 * with successful matches. The operation fails if at any point in the search a
 * name segment cannot be found. If the current object has no descendants, then 
 * the result is guaranteed to be an empty value, otherwise, an empty value may 
 * be returned if the search fails.
 */
DataProperty::PtrType DataProperty::findUnique( 
                            const std::string& criteria, const bool deep ) const {
                                            
    PtrType ret = PtrType();
    
    if( _isANode == false )
        throw std::runtime_error( "DataProperty object is not a node" );

    this->findAll( criteria, deep );
    if( _findSearchResult.size() > 1 )
        throw std::runtime_error( 
            boost::str( boost::format( "Name %s is not unique ") % criteria ));
    else {
        ContainerIteratorType first = _findSearchResult.begin();
        if( first != _findSearchResult.end() )
            ret = *(first);
        return ret;
    }
}


/** Returns a set of descendant DataProperty given by the search criteria string.
 * 
 * \param criteria A string containing one or more names, separated by dot '.'
 *                 characters (i.e. "Parent.Child.Grandchild..." ). See notes.
 *
 * \param deep A boolean flag that controls whether or not the operation will
 *             recurse beneath the level of this object to children nodes. Defaults
 *             to true. If true, recursion will proceed as a depth-first 
 *             traversal tree of nodes descending from this object, if false,
 *             will only iterate over the children of this object. In the former case
 *             the method will find indirect descendants (i.e. GrandChild.GreatGrandchild),
 *             in the latter only a direct descendant will be found.
 *
 * \return A iteratorRangeType containing a pair of iterators to access the list 
 *         of found objects. If none
 *         is found matching the given search criteria, then the first member
 *         of the pair is equal to the second.
 * 
 * \throw Throws std::runtime_error if this object is not a node
 *
 * \note See findUnique() for information regarding the criteria string.
 * \note The number of objects found by this method can be determined by testing
 *       the iterator range with std::distance, i.e. std::distance(range.first,range.second).
 * \note If a DataProperty object only aggregates unique names, a call to this
 *       method with a given criteria is functionally equivalent to a call to 
 *       findUnique, except that the result must be accessed through the
 *       iterator pair in the returned value.
 * \note Non-sensical calling arguments are possible: i.e. criteria specifies
 *       grandchildren of this object, and deep is set to false.
 *       
 */
DataProperty::iteratorRangeType DataProperty::findAll( 
                               const std::string& criteria, const bool deep ) const {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::findAll (\"%s\") : %s") 
                % criteria % this->toString()));

    if( _isANode == false )
        throw std::runtime_error( "DataProperty object is not a node" );
    else
        _resetFindSearchResult();
    
    if( criteria.length() > 0 ) {
        stringVectorType names;
        boost::algorithm::split( names, criteria, boost::algorithm::is_any_of(".") );
        _recurseFindAllDescendants( _findSearchResult, names.begin(), names.end(), deep );
    }

    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::findAll (\"%s\") : %s") 
                % criteria % this->toString()));
    
    return std::make_pair( _findSearchResult.begin(), _findSearchResult.end() );
}


/** Returns the set of DataProperty objects that exists as children of this 
 *  object or descendants of the children, whose names match the pattern given
 *  by the regular expression.
 * 
 * \param regexSpec A string containing a boost::regex-compliant regular
 *                  expression.
 *
 * \param deep A boolean flag that controls whether or not the operation will
 *             recurse beneath the level of this object to children nodes. Defaults
 *             to true. If true, recursion will proceed as a depth-first 
 *             traversal tree of nodes descending from this object, if false,
 *             will only iterate over the children of this object.
 *
 * \return A iteratorRangeType containing a pair of iterators to access the list 
 *         of found objects. If none
 *         is found matching the given search criteria, then the first member
 *         of the pair is equal to the second.
 * 
 * \throw Throws std::runtime_error if this object is not a node
 *
 * \note The number of objects found by this method can be determined by testing
 *       the iterator range with std::distance, 
 *       i.e. std::distance(range.first,range.second).
 *
 * \note This implementation uses boost::regex_search. Accordingly, if it is 
 *       desired to perform anchored searches, regexSpec must include explicit 
 *       "^" and "$" characters.
 */
DataProperty::iteratorRangeType DataProperty::searchAll( 
                                    const std::string& regexSpec, const bool deep ) const {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::searchAll (\"%s\") : %s") 
                % regexSpec % this->toString()));

    if( _isANode == false )
        throw std::runtime_error( "DataProperty object is not a node" );
    else
        _resetFindSearchResult();
        
    boost::regex ex;
    
    try {
        ex.assign(regexSpec);
    }
    catch(std::exception& e)
    {
        throw std::runtime_error( 
            boost::str( 
                boost::format("DataProperty::findNames - bad regular expression \"%s\"") 
                    % regexSpec ));
    }
    
    _recurseSearchDescendantNames( _findSearchResult, ex, "", deep );
    
    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::searchAll (\"%s\") : %s") 
                % regexSpec % this->toString()));

    return std::make_pair( _findSearchResult.begin(), _findSearchResult.end() );
}


/** Remove descendant DataProperties from this node.
  * Attempts to remove the named descendant data property from this DataProperty object,
  * as given by the name criteria. 
  *
  * \param regexSpec A string containing a boost::regex-compliant regular
  *                  expression giving the name(s) of children (or descendant)
  *                  properties to remove
  *
  * \param deep A boolean flag that controls whether or not the operation will
  *             recurse beneath the level of this object to children nodes. Defaults
  *             to true. If true, recursion will proceed as a depth-first 
  *             traversal tree of nodes descending from this object, if false,
  *             will only iterate over the children of this object.
  *
  *
  * \throw Throws an exception as per boost::regex::assign if the given regular expression
  *        string is invalid.
  * \throw Throws std::runtime_error if this object is not a node
  *
  * \note This implementation uses boost::regex_search. Accordingly, if it is 
  *       desired to perform anchored searches, regexSpec must include explicit 
  *       "^" and "$" characters.
  */
void DataProperty::deleteAll( const std::string& regexSpec, const bool deep ) {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::deleteAll(\"%s\") : %s") 
                % regexSpec % this->toString()));

    if( _isANode == false ) {
        throw std::runtime_error( "DataProperty object is not a node" );
    }
    
    boost::regex ex;

    try {
        ex.assign(regexSpec);
    }
    catch(std::exception& e)
    {
        throw std::runtime_error( 
            boost::str( 
                boost::format("DataProperty::deleteAll - bad regular expression \"%s\"") 
                    % regexSpec ));
    }

    _recurseDeleteDescendants( ex, deep );

    execTrace( boost::str( 
        boost::format( "Exit DataProperty::deleteAll : %s") % this->toString()));
}


/** Find all descendants of this object having a name that matches the given 
 * pattern.
 *
 * Performs a depth-first traversal of the tree of DataProperty objects beneath
 * this object, looking for objects having a name that matches the given string.
 * Since the input string is treated as a regular expression (see boost::regex),
 * wildcards are allowed. 
 * For each successful match, a string is added to the returned
 * collection that is a compound name giving the "path" to that object beneath
 * this one. Each of the items in this list can be used in subsequent calls to the
 * find() operation.
 *
 * \param regexSpec A string containing a boost::regex-compliant regular
 *                  expression.
 *
 * \return A nameSetType containing unique (possibly compound) names of 
 *         object(s) found. 
 *         If none is found matching the given search criteria, then an empty 
 *         nameSetType is returned.
 *
 * \throw Throws std::runtime_error if the given regular expression is bad
 *
 * \note This implementation uses boost::regex_search. Accordingly, if it is 
 *       desired to perform anchored searches, regexSpec must include explicit 
 *       "^" and "$" characters.
 * \note Since DataProperties may aggregate multiple children with the same
 *       names, when using the members of the returned set to perform
 *       findUnique calls, exceptions may be raised (see findUnique()). The
 *       safest technique is to call findAll with values returned from the set.
 */
DataProperty::nameSetType DataProperty::findNames(const std::string& regexSpec) const {
   execTrace( boost::str( 
        boost::format("DataProperty::findNames(%s) [%s]") 
            % regexSpec  % this->toString()));

    nameSetType ret;
    boost::regex ex;
    
    try {
        ex.assign(regexSpec);
    }
    catch(std::exception& e)
    {
        throw std::runtime_error( 
            boost::str( 
                boost::format("DataProperty::findNames - bad regular expression \"%s\"") 
                    % regexSpec ));
    }

    ret = _findDescendantNames( ret, ex, "" );

    execTrace( boost::str( 
        boost::format("Exit DataProperty::findNames(%s)") % regexSpec ));

    return ret;
}


/** Returns true if this object has a value that is a collection of
 * DataProperty (ContainerType), otherwise returns false.
 */
const bool DataProperty::isNode() const {
    return _isANode;
}


/** Return a string representation of this object
  * 
  * \param prefix A string to prepend to the returned string (defaults to "").
  * \param deep A boolean. If true, then the representation will include
  *             this object and ALL of its descendants. If false, will 
  *             not include any descendants in the returned string. Defaults to
  *             false.
  */
    
const std::string DataProperty::toString( 
        const std::string& prefix, bool deep ) const 
{
    std::ostringstream sout;
    std::string valueString;

    sout << prefix << Citizen::repr() << " [" << _name << ", ";
    
    if( isNode() == true ) {
        sout << " (children) ]" ;
    
        if( deep == true ) {
            ContainerIteratorType iter =  _collectionValue.begin();
            ContainerIteratorType endIter = _collectionValue.end();
            
            for( ; iter != endIter; iter++ ) {
                PtrType dpp = *iter;
                sout << std::endl << dpp->toString(prefix+"    ", deep);
            }
        }
    } else {
        try
        {
            std::ostringstream sValue;
            if (_value.type() == typeid(int)) {
                sValue << boost::any_cast<const int>(_value);
            } else if (_value.type() == typeid(std::string)) {
                sValue << boost::any_cast<const std::string>(_value);
            } else if (_value.type() == typeid(float)) {
                sValue << boost::any_cast<const float>(_value);
            } else if (_value.type() == typeid(double)) {
                sValue << boost::any_cast<const double>(_value);
            } else if (_value.type() == typeid(bool)) {
                sValue << boost::any_cast<const bool>(_value);
            } else if (_value.type() == typeid(int64_t)) {
                sValue << boost::any_cast<const int64_t>(_value);
            } else {
                sValue << string("...");
            }
            valueString = sValue.str();
        }
        catch( ... )
        {
            valueString = "";
        }

        sout << valueString + "]";
    }

    return sout.str();
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////


/*
 * Creates a copy of the value property of the given object. If the given
 * object is a node (value is a ContainerType), then all of the 
 * descendants will be recursively copied from the given object to this
 * object - the result will be two exact copies of the given object's value
 * (no shared descendant pointers)
 */
void DataProperty::_cloneValue( const DataProperty& orig ) {
    if( this->_isANode ) {
        this->_eraseChildren();
    }

    if( orig._isANode ) {
        this->_cloneCollection(orig);
        this->_value = boost::any();
        this->_isANode = true;
    } else {
        this->_value = orig._value;
        this->_collectionValue = ContainerType();
        this->_isANode = false;
    }
}


/*
 * Invoke std::list::erase on all elements of this object's children collection
 *
 * NOTE: These are boost::shared_ptr objects wrapping raw DataProperty*
 *       so the erase should result in proper garbage collection if the
 *       reference count on an individual DataProperty* goes to 0.
 */
void DataProperty::_eraseChildren() {
    execTrace( "Enter DataProperty::_eraseChildren()", EXEC_TRACE+5 );
    if( this->_isANode ) {
        if( _collectionValue.size() > 0 ) {
            _collectionValue.erase(
                _collectionValue.begin(),_collectionValue.end());
        }
    }

    execTrace( "Exit DataProperty::_eraseChildren()", EXEC_TRACE+5 );
}

/*
 * Clone all items in the given collection into a new collection
 */
void DataProperty::_cloneCollection( const DataProperty& source) {
    //
    // Iterate over the source object's collection of Items
    // and create copies of them into this object
    //
    execTrace( "Enter DataProperty::_cloneCollection()", EXEC_TRACE+5 );

    _collectionValue = ContainerType();

    ContainerType::const_iterator iter = source._collectionValue.begin();
    ContainerType::const_iterator endIter = source._collectionValue.end();
    
    for(; iter != endIter; iter++ ) {
        execTrace( "... cloning : " + (*iter)->toString(), EXEC_TRACE+10 );
        DataProperty::PtrType newObj( new DataProperty( *(*iter) ));
        _collectionValue.push_back(newObj);
    }

    execTrace( "Exit DataProperty::_cloneCollection()", EXEC_TRACE+5 );
}


/*
 * Scan the collection of child DataProperty objects looking for names that
 * match the given regular expression. For each match, add the name matched to
 * the stringVectorType ret with the given nameRoot prepended (this satisfies
 * the requirements imposed by DataProperty::findNames(). 
 *
 * For each item in the collection, call this method recursively (produces a
 * depth-first traversal of the tree object DataProperties.
 */
DataProperty::nameSetType& DataProperty::_findDescendantNames(
    nameSetType& ret,
    const boost::regex& spec,
    const string& nameRoot ) const {
    execTrace( boost::str( 
        boost::format("Enter DataProperty::_findDescendantNames() [%s]") 
            % this->toString()), EXEC_TRACE+5 );

    if ( this->_isANode ) {
        ContainerIteratorType iter =  _collectionValue.begin();
        ContainerIteratorType endIter = _collectionValue.end();

        for( ; iter != endIter; iter++ ) {
            PtrType prop = *iter;
            execTrace( "... checking \"" + prop->_name 
                      + "\" against \"" + spec.str() + "\"", EXEC_TRACE+10 );
    
            if (boost::regex_search(prop->_name, spec)) {
               ret.insert( nameRoot + prop->_name );
            }
            ret = (*iter)->_findDescendantNames( 
                ret, spec, nameRoot + prop->_name + "." );
        }

    }

    execTrace( "Exit DataProperty::_findDescendantNames()", EXEC_TRACE+5  );

    return ret;
}


/*
 * Searches the collection of descendant properties of this object looking
 * for an item having the name stored in the first item of the given vector
 * of names. If an exact match is found, then the function recurses to the 
 * descendants of the matching item using the remainder of the vector (i.e.
 * items 2 .. n.
 *
 * Returns the descendant item that is found at a name path completely 
 * conforming to the names given in the input vector. For example, if the 
 * vector contains "A","B","C" then this function will return the object
 * having name "C" that is a descendant of object "B" that is a descendant
 * of object "A" that is a descendant of this object, conforming to a name 
 * path of "A.B.C"
 */
DataProperty::PtrType DataProperty::_recurseFindDescendant(
    stringVectorType::const_iterator nameSegs,
    stringVectorType::const_iterator end_of_nameSegs ) const {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::findDescendant(%s) : %s") 
                % *nameSegs % this->toString()), EXEC_TRACE+5 );
                                        
    ContainerIteratorType iter =  _collectionValue.begin();
    ContainerIteratorType endIter = _collectionValue.end();
    PtrType ret;
    
    for( ; iter != endIter; iter++ ) {
        PtrType item = (*iter);
        if( item->_name == *nameSegs ) {
            ++nameSegs;
            if( nameSegs != end_of_nameSegs ) {
                ret = item->_recurseFindDescendant( nameSegs, end_of_nameSegs );
            } else {
                ret = item;
            }
            break;
        }
    }

    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::findDescendant(...) : %s") 
                % this->toString()), EXEC_TRACE+5 );
    
    return ret;
}


void DataProperty::_recurseFindAllDescendants(  
                ContainerType& results,
                    stringVectorType::const_iterator nameSegs,  
                        stringVectorType::const_iterator endOf_nameSegs,
                            bool recurse ) const {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::_recurseFindAllDescendants(%s) : %s") 
                % *nameSegs % this->toString()), EXEC_TRACE+5 );

    ContainerIteratorType iter =  _collectionValue.begin();
    ContainerIteratorType endIter = _collectionValue.end();

    if( std::distance( nameSegs, endOf_nameSegs) > 1 ) {
        for( ; iter != endIter; iter++ ) {
            PtrType item = (*iter);
            if( item->_isANode == true ) {
                if( item->_name == *nameSegs ) {
                    // probe deeper
                    stringVectorType::const_iterator nextNameSeg = nameSegs;
                    nextNameSeg++;
                    item->_recurseFindAllDescendants( 
                            results, nextNameSeg, endOf_nameSegs, false );
                } else if( recurse == true  ) {
                    item->_recurseFindAllDescendants( 
                            results, nameSegs, endOf_nameSegs, recurse );
                }
            }
        }
    } else if( std::distance( nameSegs, endOf_nameSegs) == 1 ) {
        for( ; iter != endIter; iter++ ) {
            PtrType item = (*iter);
            if( item->_name == *nameSegs ) {
                results.push_back( item );
            }
            if( recurse == true && item->_isANode == true ) {
                item->_recurseFindAllDescendants( 
                        results, nameSegs, endOf_nameSegs, recurse );
            }
        }
    }
    
    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::_recurseFindAllDescendants(...) : %s") 
                % this->toString()), EXEC_TRACE+5 );
    
    return;
}


void DataProperty::_recurseSearchDescendantNames(
            ContainerType& results,
                const boost::regex& spec, 
                    const std::string& nameRoot,
                        bool recurse ) const {
    execTrace( boost::str( 
        boost::format("Enter DataProperty::_recurseSearchDescendantNames() [%s]") 
            % this->toString()), EXEC_TRACE+5 );

                                        
    if( this->_isANode == false )
        return;

    ContainerIteratorType iter =  _collectionValue.begin();
    ContainerIteratorType endIter = _collectionValue.end();

    for( ; iter != endIter; iter++ ) {
        PtrType prop = *iter;
        execTrace( "... checking \"" + prop->_name 
                  + "\" against expression \"" + spec.str() + "\"", 
                    EXEC_TRACE+10 );

        if (boost::regex_search(prop->_name, spec))
           results.push_back( prop );
        if( prop->_isANode == true && recurse == true )
            prop->_recurseSearchDescendantNames(
                results, spec, nameRoot + prop->_name + ".", recurse );
    }

    execTrace( "Exit DataProperty::_recurseSearchDescendantNames(...)", EXEC_TRACE+5 );
    
    return;
}


void DataProperty::_recurseDeleteDescendants( const boost::regex& spec, bool recurse ) {
    execTrace( boost::str( 
        boost::format( 
            "Enter DataProperty::_recurseDeleteDescendants(%s) : %s") 
                % spec.str() % this->toString()), EXEC_TRACE+5 );
                                        
    if( this->_isANode == false )
        return;

    ContainerType::iterator iter =  _collectionValue.begin();
    ContainerType::iterator endIter = _collectionValue.end();

    while( iter != endIter ) {
        PtrType prop = *iter;
        execTrace( "... checking \"" + prop->_name 
                  + "\" against expression \"" + spec.str() + "\"", 
                    EXEC_TRACE+10 );

        if (boost::regex_search(prop->_name, spec)) {
            execTrace( "... erasing \"" + prop->_name, EXEC_TRACE+15 );
           iter = _collectionValue.erase( iter );
        }
        else
            iter++;
            
        if( prop->_isANode == true && recurse == true )
            prop->_recurseDeleteDescendants( spec, recurse );
    }
                

    execTrace( boost::str( 
        boost::format( 
            "Exit DataProperty::deleteDescendant(...) : %s") 
                % this->toString()), EXEC_TRACE+5 );
}

void DataProperty::_resetFindSearchResult() const {
    _findSearchResult = ContainerType();
}

}}} // namespace lsst::daf::base

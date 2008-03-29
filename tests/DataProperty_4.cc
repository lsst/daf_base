// 
// File:   DataProperty_3.cc
// Author: jeff
//
// Created on May 17, 2007, 8:31 AM
//

#include <iostream>
#include <list>
#include <string>

#include <boost/format.hpp>

using namespace std;

#include "lsst/daf/base.h"

using lsst::daf::base::DataProperty;
using lsst::daf::base::Citizen;

void Trace(std::string const&, int, std::string const& text) {
    std::cerr << text << std::endl;
}

void testFindUnique()
{
    Trace("testFindUnique",5,"Building tree");
    DataProperty::PtrType root(new DataProperty("root"));
    DataProperty::PtrType sub(new DataProperty("sub"));
    DataProperty::PtrType ssub(new DataProperty("ssub"));
    root->addProperty( sub );
    root->addProperty(DataProperty::PtrType(new DataProperty("a",string("root.a"))));
    sub->addProperty(DataProperty::PtrType(new DataProperty("a",string("root.sub.a"))));
    sub->addProperty(ssub);
    ssub->addProperty(DataProperty::PtrType(new DataProperty("b",string("root.sub.ssub.b"))));

    Trace("testFindUnique",10,
            boost::str(boost::format( "Resulting tree:\n%s" ) % root->toString("               ",true) ));

    Trace("testFindUnique",10, "Test root->findUnique(\"sub.a\")" );
    Trace("testFindUnique",10,            
            boost::str(boost::format( "root->findUnique(\"sub.a\") : found %s" )  % root->findUnique("sub.a")->toString() ));

    Trace("testFindUnique",10, "Test root->findUnique(\"ssub.b\")" );
    Trace("testFindUnique",10,            
            boost::str(boost::format( "root->findUnique(\"ssub.b\") : found %s" )  % root->findUnique("ssub.b")->toString() ));

    DataProperty::PtrType found;
    Trace("testFindUnique",10, "Test root->findUnique(\"ssub.b\", false)" );
    found = root->findUnique("ssub.b",false);
    if( found != 0 )
       Trace("testFindUnique",10,            
            boost::str(boost::format( "Error: root->findUnique(\"ssub.b\", false) : found %s" )  % found->toString() ));
    else
       Trace("testFindUnique",10, "Success: root->findUnique(\"ssub.b\", false) : found nothing as expected");
     
    Trace("testFindUnique",10, "Test root->findAll on \"a\"" );
    DataProperty::iteratorRangeType all = root->findAll("a");
    DataProperty::ContainerIteratorType iter = all.first;
    
    for(; iter != all.second; iter++ )
    {
        found = *iter;
        Trace("testFindUnique",10,            
            boost::str(boost::format( "root->findAll(\"a\") : found %s" )  % found->toString() ));
    }
    
    Trace("testFindUnique",10, "Test root->findUnique on \"a\"" );
    try {
        found = root->findUnique("a");                
        Trace("testFindUnique",10,            
            boost::str(boost::format( "Error: root->findUnique(\"a\") : found %s" )  % found->toString() ));
            throw( std::runtime_error("DataProperty::findUnique does not fail as expected") );
    }
    catch ( std::runtime_error e) {
        Trace("testFindUnique",10, "root->findUnique(\"a\") fails as expected");
    }
    
    Trace("testFindUnique",5,"Inserting tree underneath node \"top\"");
    DataProperty::PtrType top(new DataProperty("top"));
    top->addProperty(root);                
    Trace("testFindUnique",10, "Test top->findUnique on \"root.sub.b\" (should not find anything)" );
     found = top->findUnique("root.sub.b");
    if( found != 0 )                
        Trace("testFindUnique",10,            
            boost::str(boost::format( "Error: top->findUnique(\"root.sub.b\") : found %s" )  % found->toString() ));
    else            
        Trace("testFindUnique",10, "Success: top->findUnique(\"root.sub.b\") : found nothing" );

    Trace("testFindUnique",5,"Done");
}


int main(int argc, char** argv) {
    int exitVal = 0;

    Trace("DataProperty_3",1,"Testing findUnique functionality");
    testFindUnique();
    Trace("DataProperty_3",1,"Done testing findUnique functionality");
    
    //
    // Check for memory leaks
    //
    if (Citizen::census(0) == 0) 
    {
         cerr << "No leaks detected" << endl;
         exitVal = EXIT_SUCCESS;
    } 
    else 
    {
         cerr << "Leaked memory blocks:" << endl;
         Citizen::census(cerr);
         exitVal = ~EXIT_SUCCESS;
    }
    
    return exitVal;
}


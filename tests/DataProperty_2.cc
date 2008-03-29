// 
// File:   DataProperty_2.cc
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

//
// 
//
void testCopyAndAssignment()
{
    Trace("testCopyAndAssignment",1,"Creating DataProperty dp1");
    DataProperty dp1("dp1",string("dp1 value"));

    Trace("testCopyAndAssignment",1,"Creating DataProperty dp2");
    DataProperty dp2("dp2", string("prop2_value"));

    Trace("testCopyAndAssignment",1,"Creating DataProperty dp2_asssign");
    DataProperty dp2_assign("dp2_assign", string("prop3_value"));
    
    Trace("testCopyAndAssignment",1,"Creating DataProperty dp1_copy(dp1)");
    DataProperty dp1_copy(dp1);

    Trace("testCopyAndAssignment",1,"Assigning DataProperty dp2_assign = dp2");
    Trace("testCopyAndAssignment",5,"Before assignment:\n" + dp2_assign.toString(string(10,' '), true) );
    dp2_assign = dp2;
    Trace("testCopyAndAssignment",5,"After assignment:\n"  + dp2_assign.toString(string(10,' '), true) );

    Trace("testCopyAndAssignment",1,"Creating and destroying DataProperty dp3");
    DataProperty* dp3 = new DataProperty("dp3");
    delete dp3;
    
    Trace("testCopyAndAssignment",1,"Creating DataProperty::PtrType dp4");
    DataProperty::PtrType dp4( new DataProperty("dp4CopyConstructor") );
    
    Trace("testCopyAndAssignment",1,"dp1, dp2, dp4, dp1_copy and dp2_assign going out of scope");
}

void testAddAndFind()
{
    Trace("testAddAndFind",1,"Creating node DataProperty dp1");
    DataProperty::PtrType dp1( new DataProperty("dp1"));

    Trace("testAddAndFind",1,"Creating DataProperty dp2");
    DataProperty::PtrType dp2( new DataProperty("dp2", string("prop2_value")));

    Trace("testAddAndFind",1,"Creating DataProperty dp3");
    DataProperty::PtrType dp3( new DataProperty("dp3"));
    
    Trace("testAddAndFind",1,"Creating DataProperty dp4");
    DataProperty dp4( "dp4" );
    
    Trace("testAddAndFind",1,"Adding DataProperty dp2 to dp1");
    Trace("testAddAndFind",5,
        boost::str( boost::format("dp2 use count before insert: %d") % dp2.use_count() ));
    DataProperty::iteratorRangeType range = dp1->getChildren();
    Trace("testAddAndFind",5,
        boost::str( boost::format("dp1 children count before insert: %d") 
            % std::distance(range.first, range.second) ));
    dp1->addProperty(dp2);
    range = dp1->getChildren();
    Trace("testAddAndFind",5,
        boost::str( boost::format("dp1 children count after insert: %d") 
            % std::distance(range.first, range.second) ));
    Trace("testAddAndFind",5,
        boost::str( boost::format("dp2 use count after insert: %d") % dp2.use_count() ));

    Trace("testAddAndFind",1,"Adding DataProperty dp4 to dp1");
    dp1->addProperty(dp4);
    
    Trace("testAddAndFind",1,"Find dp2 in dp1");
    DataProperty::PtrType found = dp1->findUnique( string("dp2") );
    Trace("testAddAndFind",5,
        boost::str( boost::format("found %s") % found->toString() ));

    Trace("testAddAndFind",1,"Create dp1_copy from dp1");
    DataProperty dp1_copy(*(dp1.get()));
    
    Trace("testAddAndFind",1,"Adding DataProperty dp3 to dp1");
    Trace("testAddAndFind",5,
        boost::str( boost::format("dp3 use count before insert: %d") % dp3.use_count() ));
    
    dp1->addProperty(dp3);
    Trace("testAddAndFind",5,
        boost::str( boost::format("dp3 use count after insert: %d") % dp3.use_count() ));
    
    Trace("testAddAndFind",5, "dp1 contents: \n" + dp1->toString(string(10,' '),true) );
    Trace("testAddAndFind",5, "dp1_copy contents: \n" + dp1_copy.toString(string(10,' '),true) );
    
    {
        Trace("testAddAndFind",1,"test dp1->findNames(\".*\")");
        DataProperty::nameSetType names = dp1->findNames( ".*" );
        for( DataProperty::nameSetType::iterator iter = names.begin(); 
             iter != names.end(); 
             iter++){
            Trace("testAddAndFind",5,
                boost::str(boost::format( "... found name '%s'" ) % *iter ));
            DataProperty::PtrType obj = dp1->findUnique(*iter);
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... object %s" ) % obj->toString(string(15,' ')) ));
        }
    }
    
    {
        Trace("testAddAndFind",1,"test dp1->searchAll(\".*\")");
        DataProperty::iteratorRangeType result = dp1->searchAll( ".*" );
        for( DataProperty::ContainerType::const_iterator iter = result.first; 
             iter != result.second; 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found %S" ) % (*iter)->toString() ));
        }
    }
    
    {
        Trace("testAddAndFind",1,"test dp1->findNames(\"[23]$\")");
        DataProperty::nameSetType names = dp1->findNames( "[23]$" );
        for( DataProperty::nameSetType::iterator iter = names.begin(); 
             iter != names.end(); 
             iter++){
            Trace("testAddAndFind",5,
                boost::str(boost::format( "... found name '%s'" ) % *iter ));
            DataProperty::PtrType obj = dp1->findUnique(*iter);
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... object %s" ) % obj->toString(string(15,' ')) ));
        }
    }
    
    {
        Trace("testAddAndFind",1,"test dp1->searchAll(\"[23]$\")");
        DataProperty::iteratorRangeType result = dp1->searchAll( "[23]$" );
        for( DataProperty::ContainerType::const_iterator iter = result.first; 
             iter != result.second; 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found %S" ) % (*iter)->toString() ));
        }
    }
    
    {
        Trace("testAddAndFind",1,"test dp1->findNames(\"^dp\")");
        DataProperty::nameSetType names = dp1->findNames( "^dp" );
        for( DataProperty::nameSetType::iterator iter = names.begin(); 
             iter != names.end(); 
             iter++){
            Trace("testAddAndFind",5,
            boost::str(boost::format( "... found name '%s'" ) % *iter ));
            DataProperty::PtrType obj = dp1->findUnique(*iter);
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... object %s" ) % obj->toString(string(15,' ')) ));
        }
    }
    
    {
        Trace("testAddAndFind",1,"test dp1->searchAll(\"p\")");
        DataProperty::iteratorRangeType result = dp1->searchAll( "p" );
        for( DataProperty::ContainerType::const_iterator iter = result.first; 
             iter != result.second; 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found %S" ) % (*iter)->toString() ));
        }
    }

    {
        Trace("testAddAndFind",1,"test dp1->findNames(\"p\")");
        DataProperty::nameSetType names = dp1->findNames( "p" );
        for( DataProperty::nameSetType::iterator iter = names.begin(); 
             iter != names.end(); 
             iter++){
            Trace("testAddAndFind",5,
                boost::str(boost::format( "... found name '%s'" ) % *iter ));
            DataProperty::PtrType obj = dp1->findUnique(*iter);
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... object %s" ) % obj->toString(string(15,' ')) ));
        }
    }

    Trace("testAddAndFind",1,"dp1, dp2, dp3 going out of scope");
}

void testAddFindDelete()
{
    Trace("testAddFindDelete",5,"Building tree");
    //    DataProperty::nameSetType names = dp1->findNames( ".*" );

    DataProperty::PtrType A(new DataProperty("A"));
    {
        DataProperty::PtrType B(new DataProperty("B"));
        DataProperty::PtrType C(new DataProperty("C"));
        DataProperty::PtrType D(new DataProperty("D"));
        DataProperty::PtrType E1(new DataProperty("E"));
        DataProperty::PtrType F1(new DataProperty("F"));
        DataProperty::PtrType G1(new DataProperty("G"));
        DataProperty::PtrType E2(new DataProperty("E"));
        DataProperty::PtrType F2(new DataProperty("F"));
        DataProperty::PtrType G2(new DataProperty("G"));
        DataProperty::PtrType H(new DataProperty("H"));
        DataProperty::PtrType I(new DataProperty("I"));
        DataProperty::PtrType W(new DataProperty("W"));
        DataProperty::PtrType X(new DataProperty("X"));
        DataProperty::PtrType Y(new DataProperty("Y"));

        A->addProperty(B); 
        B->addProperty(E1); B->addProperty(F1); B->addProperty(G1);
        F1->addProperty(W); F1->addProperty(X); 

        A->addProperty(C); 
        C->addProperty(E2); C->addProperty(F2); 
        F2->addProperty(Y); 

        A->addProperty(D);
        D->addProperty(G2); D->addProperty(H); D->addProperty(I);
    }    
    Trace("testAddFindDelete",5,"Done building tree :");
    Trace("testAddFindDelete", 10, A->toString("          ",true));

 
    Trace("testAddAndFind2",5,"Find All names in tree with findNames");
    {
        DataProperty::nameSetType names = A->findNames("[A-Z]*");
        for( DataProperty::nameSetType::iterator iter = names.begin(); 
             iter != names.end(); 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found name '%s'" ) % *iter ));
        }
    }
    Trace("testAddAndFind2",5,"Done");

    Trace("testAddAndFind2",5,"Find All names in tree with searchAll");
    {
        DataProperty::iteratorRangeType result = A->searchAll("^.*$");
        for( DataProperty::ContainerType::const_iterator iter = result.first;
             iter != result.second; 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found object '%s'" ) % (*iter)->toString() ));
        }
    }
    Trace("testAddAndFind2",5,"Done");

    Trace("testAddAndFind2",5,"Find All named \"E\" or \"F\" in tree");
    {
        DataProperty::iteratorRangeType result = A->searchAll("^[EF]$");
        for( DataProperty::ContainerType::const_iterator iter = result.first;
             iter != result.second; 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found object '%s'" ) % (*iter)->toString() ));
        }
    }
    Trace("testAddAndFind2",5,"Done");
    
    Trace("testAddAndFind2",5,"Find Everything under node \"B.F\" with findNames");
    {
        DataProperty::PtrType BF = A->findUnique("B.F");
        DataProperty::nameSetType names = BF->findNames(string("^.*$"));
        for( DataProperty::nameSetType::iterator iter = names.begin(); 
             iter != names.end(); 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found name '%s'" ) % *iter ));
            Trace("testAddAndFind",15, A->findUnique("B.F." + *iter)->toString(string(20,' ')) );
        }
    }
    Trace("testAddAndFind2",5,"Done");

    Trace("testAddAndFind2",5,"Find Everything under node \"B.F\" with searchAll");
    {
        DataProperty::PtrType BF = A->findUnique("B.F");
        DataProperty::iteratorRangeType result = BF->searchAll(string("^.*$"));
        for( DataProperty::ContainerType::const_iterator iter = result.first;
             iter != result.second; 
             iter++){
            Trace("testAddAndFind",10,
                boost::str(boost::format( "... found name '%s'" ) % (*iter)->getName() ));
            Trace("testAddAndFind",15, 
                A->findUnique("B.F." + ((*iter)->getName()))->toString(string(20,' ')) );
        }
    }
    Trace("testAddAndFind2",5,"Done");


    Trace("testAddFindDelete",5,"Attempt deleteAll with an invalid name specification");
    {
        Trace("testAddFindDelete",10,
            boost::str(boost::format( "Starting tree:\n%s" ) % A->toString("          ",true) ));
        A->deleteAll("BAD");
        Trace("testAddFindDelete",10,
            boost::str(boost::format( "Resulting tree:\n%s" ) % A->toString("          ",true) ));
    }
    Trace("testAddFindDelete",5,"Done");


    Trace("testAddFindDelete",5,"Attempt deleteAll for all descendants of \"B.F\"");
    {
        Trace("testAddFindDelete",10,
            boost::str(boost::format( "Starting tree:\n%s" ) % A->toString("          ",true) ));
        A->findUnique("B.F")->deleteAll("^.*$");
        Trace("testAddFindDelete",10,
            boost::str(boost::format( "Resulting tree:\n%s" ) % A->toString("          ",true) ));
    }
    Trace("testAddFindDelete",5,"Done");

    Trace("testAddFindDelete",5,"Attempt deleteAll on branch at \"D\"");
    {
        Trace("testAddFindDelete",10,
            boost::str(boost::format( "Starting tree:\n%s" ) % A->toString("          ",true) ));
        A->deleteAll("D");
        Trace("testAddFindDelete",10,
            boost::str(boost::format( "Resulting tree:\n%s" ) % A->toString("          ",true) ));
    }
    Trace("testAddFindDelete",5,"Done");

}


void testRegexFind()
{
    Trace("testRegexFind",5,"Building tree");
    DataProperty::PtrType root(new DataProperty("root"));
    root->addProperty(DataProperty::PtrType(new DataProperty("name1",string("value1"))));
    root->addProperty(DataProperty::PtrType(new DataProperty("name2",2)));
    root->addProperty(DataProperty::PtrType(new DataProperty("name2",4)));
    root->addProperty(DataProperty::PtrType(new DataProperty("name3",string("Foo()"))));

    Trace("testRegexFind",10,
            boost::str(boost::format( "Resulting tree:\n%s" ) % root->toString("               ",true) ));
            
    Trace("testRegexFind",10,            
            boost::str(boost::format( "Testing find(\"name1\") : found %s" ) 
                % root->findUnique("name1")->toString() ));

    Trace("testRegexFind",10,"Testing findNames(\"name2\")");
    DataProperty::iteratorRangeType result = root->searchAll("name2");

    Trace("testRegexFind",15,
        boost::str( boost::format( "... found %d names:" ) 
            % std::distance( result.first, result.second ) ));
    
    for( DataProperty::ContainerType::const_iterator iter = result.first;
         iter != result.second; 
         iter++){
        Trace("testRegexFind",17, (*iter)->toString() );
    }

    Trace("testRegexFind",5,"Done");
}


int main(int argc, char** argv) {
    int exitVal = 0;

    Trace("DataProperty_2",1,"Testing copy and assignment functionality");
    testCopyAndAssignment();
    Trace("DataProperty_2",1,"Done testing copy and assignment functionality");

    Trace("DataProperty_2",1,"Testing add and find functionality");
    testAddAndFind();
    Trace("DataProperty_2",1,"Done testing add and find functionality");
    
    Trace("DataProperty_2",1,"Testing findNames and deleteDescendant functionality");

    testAddFindDelete();

    Trace("DataProperty_2",1,"Done testing findNames and deleteDescendant functionality");

    Trace("DataProperty_2",1,"Testing regex find functionality");
    testRegexFind()    ;
    Trace("DataProperty_2",1,"Done testing regex find functionality");
    
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


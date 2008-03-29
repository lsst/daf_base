// -*- lsst-c++ -*-
#include "lsst/daf/base/DataProperty.h"
#include "lsst/daf/base/Citizen.h"
#include "lsst/utils/Utils.h"

using namespace std;
using namespace lsst::daf::base;

class Foo {
     int gurp;
     std::string murp;
     double durp;
};

void test() {
     DataProperty::PtrType root(new DataProperty("root"));

     DataProperty::PtrType prop1(new DataProperty("name1", std::string("value1")));
     DataProperty::PtrType prop2(new DataProperty("name2", 2));
     DataProperty::PtrType prop2a(new DataProperty("name2", 4));
     

     root->addProperty(prop1);
     root->addProperty(prop2);

     Foo foo1;
     DataProperty::PtrType prop3(new DataProperty("name3", foo1));
     root->addProperty(prop3);

     root->addProperty(prop2a);

     std::cout << "findAll(\"name2\"): " << std::endl;
     DataProperty::iteratorRangeType result = root->findAll("name2");
     DataProperty::ContainerType::const_iterator iter = result.first;
     if( std::distance(result.first,result.second) != 2 )
        std::cout << "    Error: findAll found " <<  std::distance(result.first,result.second) << " items" << std::endl; 
     else {
        for( ; iter != result.second; iter++ )
           std::cout << "    Found item: " << (*iter)->toString() << std::endl;
     }

     DataProperty::PtrType dpPtr;
     try {
        dpPtr = root->findUnique( "name2" );
        std::cout << "Error: findUnique did not throw exception with \"name2\"" << std::endl; 
     } catch( ... ) {
        std::cout << "Success: findUnique threw exception with \"name2\"" << std::endl; 
     }
     
     dpPtr = root->findUnique("name1");
     std::cout << dpPtr->toString("\t", true) << std::endl;
     dpPtr = root->findUnique("name3");
     std::cout << dpPtr->toString("\t", true) << std::endl;

     // Try nested property list
     
     DataProperty::PtrType nested(new DataProperty("nested"));

     DataProperty::PtrType nprop1(new DataProperty("name1n", std::string("value1")));
     DataProperty::PtrType nprop2(new DataProperty("name2n", 2));
     

     nested->addProperty(nprop1);
     nested->addProperty(nprop2);

     root->addProperty(nested);

     std::cout << "root contents:"  << std::endl << root->toString("", true) << std::endl;

     // Check copy constructor

     DataProperty::PtrType rootCopy(new DataProperty(*root));

     // Explicitly destroy root

     root.reset();

     std::cout << "Explicit destruction done" << std::endl;

     // Check that rootCopy is still OK...

     std::cout << "rootCopy contents:" << rootCopy->toString("\t", true) << std::endl;
     
}     

void test2()
{
    boost::any foo = lsst::utils::stringToAny("-1234");
    boost::any foo2 = lsst::utils::stringToAny("1.234e-1");
    boost::any foo3 = lsst::utils::stringToAny("'This is a Fits string'");
    
    DataProperty::PtrType fooProp(new DataProperty("foo", foo));
    DataProperty::PtrType fooProp2(new DataProperty("foo2", foo2));
    DataProperty::PtrType fooProp3(new DataProperty("foo3", foo3));
    std::cout << fooProp->toString() << std::endl;
    std::cout << fooProp2->toString() << std::endl;
    std::cout << fooProp3->toString() << std::endl;
}

int main(int argc, char** argv) {
    int verbosity = 100;
    int exitVal = 0;
    
    test();

    test2();

     //
     // Check for memory leaks
     //
     if (Citizen::census(0) == 0) {
         cerr << "No leaks detected" << endl;
         exitVal = EXIT_SUCCESS;
     } else {
         cerr << "Leaked memory blocks:" << endl;
         Citizen::census(cerr);
         exitVal = ~EXIT_SUCCESS;
     }
     
     return exitVal;
}

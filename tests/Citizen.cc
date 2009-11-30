#include <iostream>
#include <stdexcept>

#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "lsst/pex/exceptions.h"
#include "lsst/daf/base/Citizen.h"

#define BOOST_TEST_MODULE Citizen
#include "boost/test/included/unit_test.hpp"

class Shoe : public lsst::daf::base::Citizen {
public:
    Shoe(int i=0) : Citizen(typeid(this)), _i(i) { }
    ~Shoe() { }
private:
    int _i;
};

class MyClass : public lsst::daf::base::Citizen {
public:
    explicit MyClass(char const* typeName=0);
private:
    boost::scoped_ptr<int> ptr;         // no need to track this alloc
};

MyClass::MyClass(char const* typeName) :
    Citizen(typeid(this)), ptr(new int) {
    *ptr = 0;
}

using namespace lsst::daf::base;

/************************************************************************************************************/

BOOST_AUTO_TEST_SUITE(CitizenSuite)

MyClass *foo() {
    boost::scoped_ptr<Shoe> x(new Shoe(1));
    MyClass *my_instance = new MyClass();

    BOOST_CHECK_EQUAL(Citizen::census(0), 5);

    return my_instance;
}

BOOST_AUTO_TEST_CASE(all) {
    Shoe x;
    const Citizen::memId firstId = Citizen::getNextMemId();
        // after allocating x
    
    boost::scoped_ptr<Shoe> y(new Shoe);
    boost::scoped_ptr<Shoe> z(new Shoe(10));
    
    MyClass *mine = foo();

    boost::scoped_ptr<const std::vector<const Citizen *> > leaks(Citizen::census());
    BOOST_CHECK_EQUAL(leaks->end() - leaks->begin(), 4);
    BOOST_CHECK_EQUAL(Citizen::census(0), 4);
    BOOST_CHECK_EQUAL(Citizen::census(0, firstId), 3);

    x.markPersistent(); // x isn't going to be deleted until main exists,
                        // so don't list as a leak
    BOOST_CHECK_EQUAL(Citizen::census(0, firstId), 3);

    z.reset();                          // i.e. delete pointed-to object
    delete mine;

    ((int *)y.get())[0] = 0;            // deliberately corrupt the block
    BOOST_CHECK_THROW((void)Citizen::hasBeenCorrupted(),
                      lsst::pex::exceptions::MemoryException);
    ((int *)y.get())[0] = 0xdeadbeef;   // uncorrupt the block

    y.reset();
    Citizen::census(std::cout, firstId);
    BOOST_CHECK_EQUAL(Citizen::census(0, firstId), 0);
}

BOOST_AUTO_TEST_SUITE_END()


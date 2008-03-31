import pdb                              # we may want to say pdb.set_trace()
import unittest
import lsst.utils.tests as tests
from lsst.daf.base import DataProperty

try:
    type(verbose)
except NameError:
    verbose = 0

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

class DataPropertyTestCase(unittest.TestCase):
    """A test case for DataProperty"""
    def setUp(self):
        self.root = DataProperty.createPropertyNode("root")

        self.values = {}; props = []
        
        n = "name1"; self.values[n] = "value1"
        props += [DataProperty(n, self.values[n])]

        n = "name2"; self.values[n] = 2
        props += [DataProperty(n, self.values[n])]
        props += [DataProperty(n, 2*self.values[n])] # add with different value

        n = "name3"
        if False:                           # this won't work, as I don't
                                            # have boost::any working from python
            class Foo:
                __slots__ = ["gurp", "murp", "durp"]

            self.values[n] = Foo()
        else:
            self.values[n] = "Foo()"
        props += [DataProperty(n, self.values[n])]

        for prop in props:
            self.root.addProperty(prop)

    def tearDown(self):
        del self.root
        self.root = None

    def testName1(self):
        """Check "name1", a string valued DataProperty"""
        
        n = "name1"
        dpPtr = self.root.findUnique(n)
        assert dpPtr.get() != None, "Failed to find %s" % n
        self.assertEqual(dpPtr.getValueString(), self.values[n])

    def testDataPtrType(self):
        """Check that the getValueXXX routines get the right types"""
        dpPtr = self.root.findUnique("name1")
        self.assertRaises(RuntimeError, dpPtr.getValueInt)

        range = self.root.searchAll("name2")

        self.assertRaises(RuntimeError, range.value().getValueString)
        range.advance(1)
        self.assertRaises(RuntimeError, range.value().getValueString)

    def testName2(self):
        """Check "name2", an int valued DataProperty with two definitions (2 and 4)"""
        
        n = "name2"

        range = self.root.searchAll(n)
        dp = [i for i in range]
        
        self.assertEqual(len(dp), 2, "Failed to find both occurrences of %s" % n)
        
        self.assertEqual(2*dp[0].getValueInt(), dp[1].getValueInt(), "Lookup correct values for name2")

    def testName3(self):
        """Check name3, which (should have) a non-{int,string} type"""
        n = "name3"
        dpPtr = self.root.findUnique(n)
        assert dpPtr.get() != None, "Failed to find %s" % n

    def testUndefined(self):
        """Check that we can't find a data property that isn't defined"""
        dpPtr = self.root.findUnique("undefined")
        assert dpPtr.get() == None, "Found non-existent DataProperty"

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

class NestedDataPropertyTestCase(unittest.TestCase):
    """A test case for nested DataProperty"""
    def setUp(self):
        self.root = DataProperty.createPropertyNode("root")

        nested = DataProperty.createPropertyNode("nested")

        self.values = {}; props = []
           
        n = "name1"; self.values[n] = "value1"
        props += [DataProperty(n, self.values[n])]

        n = "name2"; self.values[n] = 2
        props += [DataProperty(n, self.values[n])]

        for prop in props:
            nested.addProperty(prop)

        self.root.addProperty(nested)
        
    def tearDown(self):
        del self.root
        self.root = None

    def testCopyConstructor(self):
        """Check copy constructor"""
    
        rootCopy = DataProperty(self.root.get())

        # Explicitly destroy root
        del self.root; self.root = None
    
        # Check that rootCopy is still OK...
        assert rootCopy.toString() != None, "rootCopy is mangled"

    def testNested(self):
        """Extract root node"""
        contents = self.root.getChildren()

        self.assertEqual(contents.value().getName(), "nested")
        self.assertEqual(len([1 for i in contents]), 1)

    def testNested2(self):
        """Extract nested contents"""
        contents = self.root.getChildren()

        n = "name1"
        prop = contents.value()
        dpPtr = prop.findUnique(n)
        assert dpPtr.get() != None, "failed to find %s" % n
        self.assertEqual(dpPtr.getValueString(), self.values[n])

        n = "name2"
        range = contents.value().findAll(n)
        dpPtr = range.value()
        assert dpPtr.get() != None, "failed to find %s" % n
        self.assertEqual(dpPtr.getValueInt(), self.values[n])

    def testFindNames(self):
        """Test findNames"""

        names = self.root.findNames(r"^.ame")
        self.assertEqual(len(names), 2, "Two matches; name1 and name2")

    def testRegex(self):
        """Find DataProperty using boost::regex"""
        contents = self.root.getChildren()

        prop = contents.value()
        for dpPtr in prop.searchAll("^name[0-9]+"):
            n = dpPtr.getName()

            getValue = (dpPtr.getValueString if (n == "name1") else dpPtr.getValueInt)
            self.assertEqual(getValue(), self.values[n])

#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

def suite():
    """Returns a suite containing all the test cases in this module."""
    tests.init()

    suites = []
    suites += unittest.makeSuite(DataPropertyTestCase)
    suites += unittest.makeSuite(NestedDataPropertyTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

if __name__ == "__main__":
    tests.run(suite())

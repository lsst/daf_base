import unittest
import lsst.utils.tests


class UtilsBinaryTester(lsst.utils.tests.ExecutablesTestCase):
    pass

UtilsBinaryTester.create_executable_tests(__file__)

if __name__ == "__main__":
    unittest.main()

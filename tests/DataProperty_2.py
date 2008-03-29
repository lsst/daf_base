#!/usr/bin/env python

# Test the type handling of the DataProperty class.

from lsst.daf.base import DataProperty

# Python bools are stored as C++ ints.
boolIn = True
boolDP = DataProperty("bool", boolIn)
boolOut = boolDP.getValueInt()
assert boolIn == boolOut

# Ints can be stored and retrieved.
smallIn = 42
smallDP = DataProperty("small", smallIn)
smallOut = smallDP.getValueInt()
assert smallIn == smallOut

# Python longs are stored as C++ int64_t's.
bigIn = 42839284859382948L
bigDP = DataProperty("big", bigIn)
bigOut = bigDP.getValueInt64()
assert bigIn == bigOut

# Python floats are stored as C++ doubles.
doubleIn = 2.718281828459045
doubleDP = DataProperty("double", doubleIn)
doubleOut = doubleDP.getValueDouble()
assert doubleIn == doubleOut

# Python strings are stored as C++ std::strings.
stringIn = "This is only a test"
stringDP = DataProperty("string", stringIn)
stringOut = stringDP.getValueString()
assert stringIn == stringOut

# Cannot test getValueBool() or getValueFloat() from Python only as values of
# these types cannot be set from Python.  (Bool is stored as int; float is
# stored as double.)

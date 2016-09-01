############################################################################################################################################
lsst.daf.base --- Low-level data structures, including memory-management helpers (Citizen), mappings (PropertySet, PropertyList), & DateTime
############################################################################################################################################

``lsst.daf.base`` is a low-level package used by the data access framework.
It includes memory-management helpers (``Citizen``), data structures (``PropertySet`` and ``PropertyList``) and ``DateTime``.

Included in EUPS distributions
   ``lsst_apps``, ``lsst_distrib``, and ``lsst_ci``.

GitHub repository
   https://github.com/lsst/daf_base

Python API Reference
====================

.. automodapi:: lsst.daf.base
   :no-inheritance-diagram:
   :skip: long, Citizen_census, Citizen_getNextMemId, Citizen_hasBeenCorrupted, Citizen_init, Citizen_setCorruptionCallback, Citizen_setDeleteCallback, Citizen_setDeleteCallbackId, Citizen_setNewCallback, Citizen_setNewCallbackId, Citizen_swigregister, DateTime_initializeLeapSeconds, DateTime_now, DateTime_swigregister, Persistable_swigregister, PropertyList_cast, PropertyList_swigConvert, PropertyList_swigregister, PropertySet_swigConvert, PropertySet_swigregister, SwigPyIterator_swigregister, VectorBool_swigregister, VectorDateTime_swigregister, VectorDouble_swigregister, VectorFloat_swigregister, VectorInt_swigregister, VectorLongLong_swigregister, VectorLong_swigregister, VectorShort_swigregister, VectorString_swigregister, endl, ends, flush, ios_base_swigregister, ios_base_sync_with_stdio, ios_base_xalloc, ios_swigregister, iostream_swigregister, istream_swigregister, ostream_swigregister, type_info_swigregister, vectorCitizen_swigregister, SwigPyIterator, VectorBool, VectorDateTime, VectorDouble, VectorFloat, VectorInt, VectorLong, VectorLongLong, VectorShort, VectorString, ios, ios_base, iostream, istream, ostream, type_info, vectorCitizen

.. automodapi:: lsst.daf.base.citizen

C++ API Reference
=================

.. doxygenindex::
   :project: daf_base

.. .

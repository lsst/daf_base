// -*- lsst-c++ -*-

// Provides dynamic down casts from boost::shared_ptr<lsst::daf::base::Persistable>
%define %lsst_persistable(CppType...)

    %extend CppType {
        static boost::shared_ptr<CppType > swigConvert(boost::shared_ptr<lsst::daf::base::Persistable> const & ptr) {
            return boost::dynamic_pointer_cast<CppType >(ptr);
        }
    }

%enddef


# -*- python -*-
#
# Setup our environment
#
import os.path
import lsst.scons.SConsUtils as scons

env = scons.makeEnv("daf_base",
                    r"$HeadURL$",
                    scons.ConfigureDependentProducts("daf_base"))
env.Help("""
LSST Data Access Framework base package
""")

env.Append(CXXFLAGS="-std=c++0x")

###############################################################################
# Boilerplate below here

#
# Build/install things
#

for d in Split("lib python/lsst/daf/base doc examples tests"):
    SConscript(os.path.join(d, "SConscript"))

env['IgnoreFiles'] = r"(~$|\.pyc$|^\.svn$|\.o$)"

env.InstallLSST(env['prefix'], ["doc", "include", 'etc', "lib", "python", "ups"])

scons.CleanTree(r"*~ core *.so *.os *.o")

#
# Build TAGS files
#
files = scons.filesToTag()
if files:
    env.Command("TAGS", files, "etags -o $TARGET $SOURCES")

env.Declare()

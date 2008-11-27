# -*- python -*-
#
# Setup our environment
#
import glob, os.path, re
import lsst.SConsUtils as scons

dependencies = ["boost", "python", "utils"]

env = scons.makeEnv("daf_base",
                    r"$HeadURL$",
                    [["boost", "boost/shared_ptr.hpp"],
                     ["python", "Python.h"],
                     ["utils", "lsst/utils/Utils.h", "utils:C++"],
                     ["pex_exceptions", "lsst/pex/exceptions/Runtime.h", "pex_exceptions:C++"]
                    ])

env.libs["daf_base"] += env.getlibs(" ".join(dependencies))

#
# Build/install things
#
for d in Split("examples lib tests python/lsst/daf/base doc"):
    SConscript(os.path.join(d, "SConscript"))

env['IgnoreFiles'] = r"(~$|\.pyc$|^\.svn$|\.o$)"

Alias("install", [env.Install(env['prefix'], "python"),
                  env.Install(env['prefix'], "include"),
                  env.Install(env['prefix'], "lib"),
                  env.InstallAs(os.path.join(env['prefix'], "doc", "doxygen"),
                                os.path.join("doc", "htmlDir")),
                  env.InstallEups(env['prefix'] + "/ups", glob.glob("ups/*.table"))])

scons.CleanTree(r"*~ core *.so *.os *.o")

#
# Build TAGS files
#
files = scons.filesToTag()
if files:
    env.Command("TAGS", files, "etags -o $TARGET $SOURCES")

env.Declare()
env.Help("""
LSST Data Access Framework base package
""")

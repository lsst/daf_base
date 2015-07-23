# -*- python -*-
from lsst.sconsUtils import scripts, env, utils

scripts.BasicSConstruct.initialize(packageName = "daf_base")

# We need to know how to link against libpython and we have to link
# against the correct libpython. We assume that "python-config" in the path
# is related to the python we are going to use to run the code.

# We can not use the python that we are running scons with as it may be
# a completely different python.

includes = utils.runExternal("python-config --includes", fatal=True,
                        msg="Determining Python link environment")
libs = utils.runExternal("python-config --libs", fatal=True)
prefix = utils.runExternal("python-config --prefix", fatal=True)

# If the wrong libpython is loaded at run time (ie a libpython that is not
# associated with the python executable that launched the process) everything
# will fail with an error such as: "dynamic module not initialized properly"
ldflags = "-L{}/lib {} -lboost_python".format(prefix.strip(), libs.strip())

env.Append(LINKFLAGS = [ldflags.split()])
env.Append(CPPFLAGS = [includes.strip().split()])

scripts.BasicSConstruct.finish()

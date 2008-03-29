# Test findUnique in hierarchies from Python

import lsst.daf.base as B

dp = B.DataProperty("root")
sub = B.DataProperty("sub")
dp.addProperty(sub)

dp.addProperty(B.DataProperty("a", 3))
dp.addProperty(B.DataProperty("x", 4))

sub.addProperty(B.DataProperty("a", 1))
sub.addProperty(B.DataProperty("b", 2))

x = dp.findUnique("sub")
assert x.getName() == "sub"
x = dp.findUnique("b")
assert x.getValueInt() == 2
x = dp.findUnique("x")
assert x.getValueInt() == 4
x = dp.findUnique("sub.a")
assert x.getValueInt() == 1
x = dp.findUnique("sub.b")
assert x.getValueInt() == 2
x = sub.findUnique("a")
assert x.getValueInt() == 1
x = sub.findUnique("b")
assert x.getValueInt() == 2

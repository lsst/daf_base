# Test findUnique in hierarchies from Python

import lsst.mwi.data as D

dp = D.SupportFactory.createPropertyNode("root")
sub = D.SupportFactory.createPropertyNode("sub")
dp.addProperty(sub)

dp.addProperty(D.SupportFactory.createLeafProperty("a", 3))
dp.addProperty(D.SupportFactory.createLeafProperty("x", 4))

sub.addProperty(D.SupportFactory.createLeafProperty("a", 1))
sub.addProperty(D.SupportFactory.createLeafProperty("b", 2))

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

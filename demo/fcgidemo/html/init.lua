local slist = require "slist"
local schain = require "schain"
local fcgi = require "fcgi"

--shared list settings below:
slpoolid = 1
slname = "slist"
sllen = 16288
--shared chain settings below:
scpoolid = 1
scname = "test"
sclen = 130288

slist.link(scpoolid, sllen, slname)
slist.select(scpoolid)
schain.link(slpoolid, sclen, scname)
schain.select(slpoolid)

--slist.writei(0, 0, 100) --set TID counter start from 100
local slist = require "slist"
local schain = require "schain" 

--shared list settings below:
slpoolid = 1
slname = "slist"
sllog = "slist.log"
sldmp = "./slist.dmp"
maxTID = 1000
slblen = 8
slauxlen = 8
sllen = 16288
--shared chain settings below:
scpoolid = 1
scname = "test"
scblen = 256
scmaxid = 500
sclog = "log.txt"
scdmp = "sc.dmp"
sclen = 130288
--sys settings below:
localcpath = "./local/%d"

--[[
topic\nthread
slist's structure:
TID-counter-flag-CID-ACCESS
TID start form 1, TID 0 used as flags
TID 0:
TID_counter

flag--->4byte
flag:0--->status
]]

--status flags below:

TID_counter = 1

function init()
	slist.create(slblen, slauxlen, maxTID, slname, sllog)
	schain.create(scblen, scmaxid, scname, sclog)
	link()
	print("\tDone init, now you can exit to set the CONF in this script\
	if not set the CONF, will run with the default parameters to link \
	the shm and OPS, that may cause error, exit now? (yes/no)")
	if io.read() == "yes" then os.exit() end
end

function link()
	slist.link(scpoolid, sllen, slname)
	slist.select(scpoolid)
	schain.link(slpoolid, sclen, scname)
	schain.select(slpoolid)
	TID_counter = slist.geti(0, 0)
end

function drop()
	slist.select(slpoolid)
	slist.remove(sllen, slname)
	schain.select(scpoolid)
	schain.remove(sclen, scname)
end

function dump()
	slist.dump(sllen, sldmp)
	schain.dump(sclen, scname)
end

function _load()
	print("load slist length =", slist.load(sldmp))
	print("load schain length =", schain.load(sclen, scname))
	link()
end

function add_thread(TID, access, text)
	if slist.geti(TID, 4) ~= 0 then
		print("error TID exsit")
		return -1
	end
	id = schain.available()
	io.write("Using schain ID = ", id,"\n")
	if id < 0 then
		print("error datachain not available")
		return -2
	end
	status = schain.write(id, text)
	if status < 0 then
		print("error datachain no space")
		schain.del(id)
		return -3
	end
	slist.writei(0, 0, TID)	--update TID_counter at slist
	slist.writeb(TID, 4, 1)	--set flag status 1 means schain
	slist.writeauxi(TID, 0, id)	--record schain id
	slist.writeauxi(TID, 4, access)	--set access
	return 0
end

function add_to_HDD(TID, access, text)
	if slist.geti(TID, 4) ~= 0 then
		print("error TID exsit")
		return -1
	end
	file = string.format(localcpath, TID)
	fp = io.open(file, "w+")
	fp:write(topic)
	io.close(fp)
	slist.writei(0, 0, TID)	--update TID_counter at slist
	slist.writeb(TID, 4, 2)	--set flag status 2 means local HDD
	slist.writeauxi(TID, 0, 0)	--set 0, schain id no use for HDD data
	slist.writeauxi(TID, 4, access)	--set access
end

function local_read(tid)
	file = string.format(localcpath, tid)
	fp = io.open(file, "r")
	str = fp:read()
	io.close(fp)
	return str
end

function read_thread(TID, access)
	if slist.getauxi(TID, 4) ~= access then
		return "error access"
	end
	status = slist.getb(TID, 4)
	if status == 1 then
		print("read from schain")
		return schain.read(slist.getauxi(TID, 0))
	elseif status == 2 then
		print("read local file from HDD")
		return local_read(TID)
	end
end

function show_menu()
	print("\n\nPlease input command:")
	print("command:")
	print("new --write a new topic")
	print("read --read a new topic")
	print("load --load slist and schain")
	print("dump --dump slist and schain")
	print("exit --exit from this session\n")
end

function demo_new()
	print("\n\ninput your access:")
	access = io.read()
	print("input your topic")
	topic = io.read()
	TID_counter = TID_counter + 1
	status = add_thread(TID_counter, tonumber(access), topic)
	if status == 0 then
		print("Add new success at TID =", TID_counter)
	else
		add_to_HDD(TID_counter, tonumber(access), topic)
		print("Write to HDD TID=", TID_counter)
	end
end

function demo_read()
	print("input your access:")
	access = io.read()
	print("input TID you want")
	tid = io.read()
	print(read_thread(tonumber(tid), tonumber(access)))
end

--main program start below:

print("Run init? (yes/no)")
if io.read() == "yes" then init() end
link()  --link the slist and the schain

while 1 > 0 do
	show_menu()
	cmd = io.read()
	if cmd == "new" then
		demo_new()
	elseif cmd == "read" then
		demo_read()
	elseif cmd == "load" then
		_load()
	elseif cmd == "dump" then
		dump()
	elseif cmd == "exit" then
		break
	end
end
--print(add_thread(1, 0, "hello-new thread by lua"))
--print(read_thread(1, 0))
print("Demo end, remove shm? (yes/no)")
if io.read() == "yes" then drop() end
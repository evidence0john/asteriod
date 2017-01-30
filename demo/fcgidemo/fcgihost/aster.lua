local fcgi = require "fcgi"
local slist = require "slist"
local schain = require "schain" 

home_path = "/usr/share/nginx/html/"
localcpath = "/usr/share/nginx/html/local/%d"
TID_TREE = "all.tid"

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
	fp:write(text)
	io.close(fp)
	slist.writei(0, 0, TID)	--update TID_counter at slist
	slist.writeb(TID, 4, 2)	--set flag status 2 means local HDD
	slist.writeauxi(TID, 0, 0)	--set 0, schain id no use for HDD data
	slist.writeauxi(TID, 4, access)	--set access
end

function local_read(tid)
	file = string.format(localcpath, tid)
	fp = io.open(file, "r")
	str = fp:read("a")
	io.close(fp)
	return str
end

function read_thread(TID, access)
	if slist.getauxi(TID, 4) ~= access then
		return "error"
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

function demo_new(access, topic)
	TID_counter = slist.geti(0, 0)
	TID_counter = TID_counter + 1
	status = add_thread(TID_counter, access, topic)
	if status == 0 then
		fcgi.echo("成功发表主题 并缓存 数据库记录 TID =", TID_counter)
	else
		add_to_HDD(TID_counter, access, topic)
		fcgi.echo("成功发表主题 因为缓存不足而写入到磁盘 数据库记录 TID=", TID_counter)
	end
	return TID_counter
end

function demo_read(access, tid)
	rstr = read_thread(tonumber(tid), tonumber(access))
	if rstr ~= "error" then
		fcgi.echo(rstr)
	else
		fcgi.echo(static_page("./res-static/req.html"))
	end
end

function static_page(fname)
	file = home_path..fname
	fp = io.open(file, "r")
	str = fp:read("a")
	io.close(fp)
	return str
end

function wtidtree(title, tid)
	file = home_path..TID_TREE
	fp = io.open(file, "a")
	rec = "<a href=\"host.cgi?read.lua="..tostring(tid).."\">"..title.. "</a><br/>\n"
	str = fp:write(rec)
	io.close(fp)
	return rec
end

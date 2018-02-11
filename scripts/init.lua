----------------settings----------------
package.path = '/home/evide/asteroid/lua-libs/?.lua'
Global = {}
Global.script_home = '/home/evide/asteroid/scripts/'
Global.spath = '/home/evide/asteroid/res-static/'
---------------initialize---------------
fcgi = require "fcgi"
ltls = require "ltls"
require "luaoo"
require "traverse"
--fcgi lib
print = fcgi.print
echo = fcgi.echo
Raw_POST = fcgi.Raw_POST
query = fcgi.query
_env = fcgi.env
toint32 = fcgi.toint32
toint64 = fcgi.toint64
int32 = fcgi.int32
int64 = fcgi.int64
time33 = fcgi.time33
md5 = fcgi.md5
urldecoder = fcgi.urldecoder
tagRemove = fcgi.tagRemove
--
Page_counter = {}

setmetatable(Page_counter, 
		{
			['__index'] = 
			function (t, k)
				return 0
			end
		})
function counter_inc(name)
	Page_counter[name] = Page_counter[name] + 1
end

function counter_dec(name)
	Page_counter[name] = Page_counter[name] - 1
end

function counter_set(name, num)
	Page_counter[name] = num
end

function counter_get(name, num)
	return Page_counter[name]
end

function __Accept__()
	echo("Content-type:text/html\r\n\r\n")
end

function load_small_file(file)
	fp = io.open(file, "r")
	buf = fp:read("a")
	io.close(fp)
	return buf
end

function getPostByName(post, name, ctype, Boundary, dcbuflen)
	if ctype == 0 then
		gap = "&"
		left = 38
		right = 61
	elseif ctype == 1 then
		gap = "\r\n"
		left = 10
		right = 61
	elseif ctype == 2 then
		fstr = 'name=\"'..name..'\"'
		local i, j = string.find(post, fstr)
		if i then
			i = i + #fstr + 4
			j = string.find(post, Boundary, i)
			return string.sub(post, i, j - 3)
		else
			return nil
		end
	end
	pointer = 0
	::find:: do
		index = string.find(post, name, pointer)
		if index == nil then
			return nil
		end
		if index == 1 then
			if string.byte(post, index + string.len(name)) ~= right then
				pointer = index + string.len(name)
				goto find
			end
		else
			if string.byte(post, index -1) ~= left or
					string.byte(post, index + string.len(name)) ~= right then
				pointer = index + string.len(name)
				goto find
			end
		end
	end
	endl = string.find(post, gap, index)
	if endl == nil then
		endl = -1
	else
		endl = endl -1
	end
	if ctype == 0 then
		return urldecoder(string.sub(post, index + string.len(name) + 1, endl), dcbuflen)
	else
		return string.sub(post, index + string.len(name) + 1, endl)
	end
end

--application/x-www-form-urlencoded 0
--text/plain 1
--multipart/form-data 2


Accept = 
{
	['__class_type__'] = 'Accept',
	--1048576
	['buflen'] = 1048576,
	['postlen'] = 0,
	['postbuf'] = '',
	['status'] = 0,
	['postBoundary'] = '',
	['postBLEN'] = 0,
	['enctype'] = '',
	['ip'] = '',
	['script'] = '',
	['port'] = '',
	['name'] = '',
	['__init__'] = 
	function (self, conf)
		if conf then
			if conf.buflen then
				self.buflen = conf.buflen
			end
		end
		self.postlen = fcgi.postlen()
		self.postbuf = Raw_POST(self.buflen)
		self.ip = fcgi.getip()
		self.port = fcgi.env("SERVER_PORT")
		self.name = fcgi.env("SERVER_NAME")
		self.script = fcgi.env('SCRIPT_NAME')
		local ctypebuf = fcgi.env("CONTENT_TYPE")
		if ctypebuf == "text/plain" then
			self.enctype = 1
		else
			self.enctype = 0
		end
		if string.sub(ctypebuf, 1, 5) == 'multi' then
			self.enctype = 2
			local s = string.find(ctypebuf, '=') + 1
			--Protocol?
			self.postBoundary = '--'..string.sub(ctypebuf, s)
			self.postBLEN = #self.postBoundary 
		end
	end,
	['filename'] =
	function(self)
		local i, j = string.find(self.postbuf, "filename=\"[%a%d%p%s]*\"")
		local filename = ''
		if i then
			return string.sub(self.postbuf, i + 10, j - 1)
		else
			return nil
		end
	end,
	['tofile'] = 
	function(self, filename, mod)
		if filename == nil then
			return nil
		end
		local io_mod = 'w+'
		if mod then
			io_mod = mod
		end
		fp = io.open(filename, io_mod)
		if fp == nil then
			return nil
		end
		local s, e = string.find(self.postbuf, '\n\r')
		--XuanXue Constant
		s = s + 2
		local filelen = self.postlen - s - self.postBLEN - 6
		local curlen = self.buflen - s
		local remlen = filelen - curlen
		file = string.sub(self.postbuf, s + 1, s + filelen)
		fp:write(file)
		if remlen > 0 then
			local t = remlen // self.buflen
			local v = remlen % self.buflen
			for i = 1, t do
				str = Raw_POST(self.buflen)
				fp:write(str)
			end
			str = Raw_POST(v)
			fp:write(str)
		end
		fp:close()
		return filelen
	end,
	['getPostByName'] =
	function(self, name, dcbuflen)
		if self.enctype == 2 then
			return getPostByName(self.postbuf, name, self.enctype, self.postBoundary, dcbuflen)
		else
			return getPostByName(self.postbuf, name, self.enctype, self.postBoundary, dcbuflen)
		end
	end,
	['Query'] =
	function()
		return query()
	end,
	['info'] = 
	function(self)
		print('Sever Protocol = ', fcgi.env('SERVER_PROTOCOL'))
		print('Script = ', self.script)
		print('Client IP = ', self.ip)
		print('Post length = ', self.postlen)
		print('Post enctype = ', self.enctype)
		if self.enctype == 2 then
			print('Boundary = ', self.postBoundary)
		end
		print('Buffer length = ', self.buflen)
		print('Sever Port = ', self.port)
		print('Sever Name = ', self.name)
		print('Sever Software = ', fcgi.env('SERVER_SOFTWARE'))
		print('CurLua Status Mem= ', collectgarbage('count'),' kb')
	end
}

Simple_buffer = {
	['__class_type__'] = 'Simple_buffer',
	['article'] = {},
	['head'] = {},
	['tls_id'] = {},
	['counter'] = 0,
	--33554432 -> 32MB
	['capacity'] = 3355443,
	['reset'] =
	function (self)
		self.article = nil
		self.counter = 0
	end,
	['set_capacity'] =
	function (self, capacity)
		self.capacity = capacity
	end,
	['add'] =
	function (self, head, body, id)
		self.counter = self.counter + #body
		self.counter = self.counter + #head
		if self.counter > self.capacity then
			self.counter = self.counter - #body
			self.counter = self.counter - #head
			return 0
		end
		table.insert(self.article, body)
		table.insert(self.head, head)
		table.insert(self.tls_id, id)
		return #self.article
	end,
	['remove'] =
	function (self, id)
		self.counter = self.counter - #self.article[id]
		self.counter = self.counter - #self.head[id]
		table.remove(self.article[id])
		table.remove(self.head[id])
		tls_id = self.tls_id[id]
		table.remove(self.tls_id[id])
		return tls_id
	end,
	['info'] =
	function (self)
		print(self.counter)
	end
}

function OpenDiskSource(fname, mod)
	umod = 'r+'
	if mod then
		umod = mod
	end
	fp = io.open(fname, umod)
	if fp == nil then
		fp = io.open(fname, 'w+')
	end
	fp:seek('end')
	return fp
end

function StringToDisk(fp, str)
	pos = fp:seek()
	fp:write(str)
	fp:flush()
	return pos
end

----------------------------------------------------
require("article_mod")
----------------------------------------------------

dofile(Global.script_home..'usr_init.lua')
dofile(Global.script_home..'setarticle.lua')

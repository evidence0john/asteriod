--[[
Initialization file for asteroid http server local lua state
]]
print('local lua State init')
l = require('lmodule')
echo = l.b_echo
print = function (...)
	echo(...)
	echo('<br>')
end
--flush buffer
flush = l.b_flush
--clear buffer
clear = l.b_clear
--print string to stderr
log = l.stderr_log
--Http request environment
env = {}
--Returns full http request message
env.raw = l.raw
--Returns http message body
env.body = l.body
--Returns http URI
env.uri = l.uri
--Returns content length
env.content_len = l.content_len
--Returns request method
env.method = l.method
--Returns request query_string
env.query = l.query
--Returns request http protocol
env.proto = l.proto
--Returns host info
env.host = l.host
--Returns user agent
env.ua = l.ua
--Returns Accept-Encoding
env.encoding = l.encoding
--Returns Accept-Language 
env.lang = l.lang
--Returns Accept
env.accept = l.accept
--Returns Origin, it shows where the client post from
env.origin = l.origin
--Returns Content type
env.content_type = l.content_type
--Boundary if post multipart/form-data
env.boundary =
function ()
	local xtype = env.content_type()
	if not xtype then
		return nil
	end
	local pos = string.find(xtype, 'boundary')
	if pos then
		return string.sub(xtype, pos + 9, -2) -- 9 is string.len('boundary=')
	else
		return nil 
	end
end
--[[
application/x-www-form-urlencoded 0
text/plain 1
multipart/form-data 2
]]
--Returns enctype
env.enctype =
function ()
	if env.method() == 'GET' then
		return 0
	end
	local ctype = env.content_type()
	if not ctype then
		return nil
	end
	if string.find(ctype, 'urlencoded') then
		return 0
	elseif string.find(ctype, 'plain') then
		return 1
	elseif string.find(ctype, 'multi') then
		return 2
	else
		return nil --unsupport
	end
end
------------------------------------------------
--------------------  Util  --------------------
------------------------------------------------
--getPostByName(body, name[, boundary])
--getPostFileName(body, name, boundary)
--savePostFile(body, name, boundary, path[, fname])
--md5(str)
--load_file(file)

md5 = l.md5

function _getPostByNameM(body, name, boundary) --multi
	local b = '--'..boundary
	local h, l = string.find(body, b)
	while h do
		local e = string.find(body, '\n\r', h)
		if not e then
			return nil
		end --reach end
		local o = string.sub(body, h, e)
		if string.find(o, '\"[ \t]*'..name..'[ \t]*\"') then
			local p = string.find(body, b, e)
			if not p then
				return nil --'body msg error'
			end
			return string.sub(body, e + 3, p - 3) --offset
		else
			h, l = string.find(body, b, e)
		end
	end
	return nil --'nofound'..name
end

function _getPostByNameT(body, name) --text
	local s, e = string.find(body, '[ \t]*'..name..'[ \t]*=')
	if not s then
		return nil
	end
	local x = string.find(body, '\r\n', e)
	return string.sub(body, e + 1, x - 1)
end

function _getPostByNameU(body, name) --urlencoded
	local s, e = string.find(body, '[ \t]*'..name..'[ \t]*=')
	if not s then
		return nil
	end
	local x = string.find(body, '&', e)
	return string.sub(body, e + 1, x - 1)
end

function getPostByName(body, name, boundary)
	local enctype = env.enctype()
	if enctype == 0 then
		return _getPostByNameU(body, name)
	elseif enctype == 1 then
		return _getPostByNameT(body, name)
	elseif enctype == 2 then
		if boundary then
			return _getPostByNameM(body, name, boundary)
		else
			return nil
		end
	else
		return nil --unsupport
	end
end

function getPostFileName(body, name, boundary) --multi
	local b = '--'..boundary
	local h, l = string.find(body, b)
	while h do
		local e = string.find(body, '\n\r', h)
		if not e then
			return nil
		end --reach end
		local o = string.sub(body, h, e)
		if string.find(o, '\"[ \t]*'..name..'[ \t]*\"') then
			local filename = string.sub(o, string.find(o, 'filename=.+\"'))
			filename = string.sub(filename, #'filename="' + 1, #filename - 1)
			return filename
		else
			h, l = string.find(body, b, e)
		end
	end
	return nil --'nofound'..name
end

function savePostFile(body, name, boundary, path, fname)
	local b = '--'..boundary
	local h, l = string.find(body, b)
	while h do
		local e = string.find(body, '\n\r', h)
		if not e then
			return nil
		end --reach end
		local o = string.sub(body, h, e)
		if string.find(o, '\"[ \t]*'..name..'[ \t]*\"') then
			local filename = string.sub(o, string.find(o, 'filename=.+\"'))
			filename = string.sub(filename, #'filename="' + 1, #filename - 1)
			local p = string.find(body, b, e)
			if not p then
				return nil --'body msg error'
			end
			local file = string.sub(body, e + 3, p - 3)
			if not fname then
				path = path..filename
			else
				path = path..fname
			end
			local fp = io.open(path, 'w+')
			if not fp then
				return nil
			end
			fp:write(file)
			fp:close()
			return filename
		else
			h, l = string.find(body, b, e)
		end
	end
	return nil --'nofound'..name
end

function load_file(file)
	local fp = io.open(file, "r")
	if not fp then
		return nil
	end
	local buf = fp:read("a")
	io.close(fp)
	return buf
end

--[[function __Accept__( ... )
	-- body
end]]

--[[function __Finished__()
	flush()
end]]


--[[
------------------------------------------------
-------------------  Global  -------------------
------------------------------------------------
Global = {}

--Global.HtmlHome = ''

Global.UserCookie =
{
	['uname'] = ''
}

Global.UserList =
{
	['uname'] = '0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C'
}
------------------------------------------------
--------------------- Extra --------------------
------------------------------------------------
function isFileExist(filename)
	local fp = io.open(filename, 'r')
	if fp then
		io.close(fp)
		return true
	else
		return false
	end
end

ltls = require('ltls')
ltfms = require('ltfms')

if isFileExist('article_list') then
	Global.ArticleList = ltls.import('article_list')
else
	Global.ArticleList = ltls.new(8192)
end

Global.ArticleTfms = ltfms.load_tfms('testFs')
]]
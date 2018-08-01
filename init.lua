--[[
Initialization file for asteroid http server local lua state
]]
print('local lua State init')
l = require('lmodule')
echo = l.b_echo
print = function(...)
	echo(...)
	echo('<br>')
end
flush = l.b_flush
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

--[[function __Accept__( ... )
	-- body
end]]

--[[function __Finished__()
	flush()
end]]
local index = new(Accept)
---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------

local query = index:Query()
if #query == 0 then
	print('<center><img src=\"asteroid.jpg\" style=\"width:30%;\"></center><br>')
	print('<center>访问错误</center>')
else
	local up, ul = string.find(query, '&')
	local uname = string.sub(query, 1, up - 1)
	local cookie = string.sub(query, up + 1)
	if Global.usr_cookie[uname] == cookie then
		Global.usr_cookie[uname] = nil
	end
	echo(load_small_file(Global.spath..'logout.html'))
end
---------------foot---------------
print(string.rep('<br>', 3))
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------
local cmd = new(Accept)
local usrcmd = cmd:getPostByName('cmd')
local transf = io.open(Global.spath..'tmp.html', 'w+')
if string.find(usrcmd, 'sudo') then
	transf:write('Permission denied')
else
	os.execute(usrcmd..' >'..Global.spath..'raw.html')
	local rawf = io.open(Global.spath..'raw.html', 'r')
	local buf = rawf:read('a')
	local trans = string.gsub(buf, '\n', '</br>')
	transf:write(trans)
	rawf:close()
end
transf:close()
fcgi.echo('<script>window.location.href=\'show.lua\'</script>')

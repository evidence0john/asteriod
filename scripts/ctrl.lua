local post = new(Accept)
---------------head---------------
--echo(load_small_file(Global.spath..'head.html'))
if Global.Ctrl_LED == nil then
	Global.Ctrl_LED = {}
	for i=1,10 do
		Global.Ctrl_LED[i] = 0
	end
end

if Global.ctrl_serial_fp == nil then
	Global.ctrl_serial_fp = io.open('/dev/ttyUSB1', 'ab')
end

local setLED =
function (Id, Val, fp)
	Id = Id - 1
	--print(Id, Val)
	local BinS = string.char(Val)..string.char(Id)
	if fp then
		fp:write(BinS)
		fp:flush()
	end
	return BinS
end
---------------head---------------
if post.postlen == 0 then
	--print('User Info Error')
	echo('<input id = \"sId\" style=\"display: none\" value=\"1\">')
	echo('<input id = \"sVal\" style=\"display: none\"value=\"'..Global.Ctrl_LED[1]..'\">')
else
	--print(post.postbuf)
	local reqId = tonumber(post:getPostByName('led_Id'))
	local reqVal = tonumber(post:getPostByName('led_Val'))
	if post:getPostByName('IdChange') ~= 'update' then
		Global.Ctrl_LED[reqId] = reqVal
	end
	if post:getPostByName('Close') == 'true' then
		for i=1,4 do
			Global.Ctrl_LED[i] = 0
			setLED(i, Global.Ctrl_LED[i], Global.ctrl_serial_fp)
		end
	end
	echo('<input id = \"sId\" style=\"display: none\" value=\"', reqId, '\">')
	echo('<input id = \"sVal\" style=\"display: none\"value=\"'..Global.Ctrl_LED[reqId]..'\">')
	setLED(reqId, Global.Ctrl_LED[reqId], Global.ctrl_serial_fp)
end
echo(load_small_file(Global.spath..'ctrl.html'))
for i,v in ipairs(Global.Ctrl_LED) do
	if i <= 4 then
		print('LED ID = ', i, '&nbsp&nbsp&nbsp Bright Vlaue = ', v)
	end
end
---------------foot---------------
--echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------

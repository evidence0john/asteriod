Global.usr_list = {
	['evi'] = '7d6d121fd6142feb09b7028732b6e84f'
}
Global.usr_cookie = {}
Usr_set_cookie = 
function (uname, key)
	cookie = md5(uname..key..tostring(os.clock()))
	Global.usr_cookie[uname] = cookie
	return cookie
end

Usr_del_cookie = 
function (uname)
	Global.usr_cookie[uname] = nil
end

PrintInfo2Page =
function (id, str)
	print('<input type=\"text\" id=\"'..id..'\" value=\"'..str..'\"style=\"display: none\">')
end

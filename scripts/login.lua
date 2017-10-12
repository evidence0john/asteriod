local index = new(Accept)

---------------head---------------
echo(load_small_file(Global.spath..'head.html'))
---------------head---------------
if index.postlen == 0 then
	echo(load_small_file(Global.spath..'login.html'))
else
	local User = index:getPostByName('uname')
	local Passwd = index:getPostByName('passwd')
	local cookie = ''
	if Global.usr_list[User] then
		if Passwd == Global.usr_list[User] then
			cookie = Usr_set_cookie(User, Passwd)
			PrintInfo2Page('cookie', cookie)
			PrintInfo2Page('uname', User)
			echo(load_small_file(Global.spath..'login_success.html'))
		else
			print('<center><img src=\"asteroid.jpg\" style=\"width:30%;\"></center><br>')
			print('<center>登录失败 Wrong Password</center>')
		end
		
	else
		print('<center><img src=\"asteroid.jpg\" style=\"width:30%;\"></center><br>')
		print('<center>登录失败 User \"'..User..'\" Not Exist</center>')
	end
end
---------------foot---------------
print(string.rep('<br>', 3))
echo(load_small_file(Global.spath..'foot.html'))
---------------foot---------------
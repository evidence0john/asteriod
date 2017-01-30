local fcgi = require "fcgi"
require "aster"
--fcgi.echo("<meta charset=\"UTF-8\">")

fcgi.inc_counter()

fcgi.echo(static_page("./res-static/head.html"))

tpost = fcgi.tpost()
access_raw = string.sub(tpost, 8, string.find(tpost, "\n") - 2)
access = fcgi.calc_access(access_raw)
--fcgi.echo("<p>", access_raw, " access code = ", tostring(access), "</p>")
post = string.sub(tpost, string.find(tpost, "\n") + 6)
title = string.sub(post, 0, 256)
pos = string.find(title, "\n")
if pos ~= 0 then
	title = string.sub(title, 0, pos)
end
if access == 0 then
	fcgi.echo("<p>成功发送一个公开主题</p>")
	else
		fcgi.echo("<p>成功发送一个有权限的主题</p>")
		fcgi.echo("<p>牢记你设置的权限码：", access_raw, "</p>")
		fcgi.echo("<p>数据库记录权限码：", tostring(access), "</p>")
end
fcgi.echo("<p>标题 <em>", wtidtree(title, demo_new(access, post)), "</em></p>", "</br>")

fcgi.echo("</br></br><h3>你的 IP ：", fcgi.getip(), "</br>lua 页面请求计数 ：", 
		string.format("%d", fcgi.get_counter()), "<h3>")
fcgi.echo(static_page("./res-static/foot.html"))
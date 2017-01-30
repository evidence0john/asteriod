local fcgi = require "fcgi"
require "aster"

fcgi.inc_counter()

fcgi.echo(static_page("./res-static/head.html"))
access = 0
tid = fcgi.query()
tid = string.sub(tid, 10)
if fcgi.postlen() ~= 0 then
	tpost = fcgi.tpost()
	tpost = string.sub(tpost, 8)
	ret = string.sub(tpost, string.len(tpost) - 1)
	ffc =  tonumber(string.byte(ret, 1))
	if ffc == 13  or ffc == 10then
		tpost = string.sub(tpost, 0, string.len(tpost) - 2)
	end
	--fcgi.echo("-", tpost, "-", tonumber(string.byte(ret, 1)), "-")
	access = fcgi.calc_access(tpost)
end
if string.len(tid) == 0 then
	fcgi.echo("<center><h3><a href=\"host.cgi?post.lua\">创建一个新的主题</a></h3></center>")
	fcgi.echo("<p>已经存在的主题（按照发布时间降序排列，新发布的主题请向下滑）：</p>")
	--fcgi.echo("<p></p>")
	fcgi.echo(static_page("all.tid"))
	else
		fcgi.echo("<center><h3><a href=\"host.cgi?read.lua\">返回上页</a></h3></center>")
		demo_read(access, tonumber(tid))
end

fcgi.echo("</br></br><h3>你的 IP ：", fcgi.getip(), "</br>lua 页面请求计数 ：", 
		string.format("%d", fcgi.get_counter()), "<h3>")
fcgi.echo(static_page("./res-static/foot.html"))
local fcgi = require "fcgi"
require "aster"
fcgi.inc_counter()
fcgi.echo(static_page("./res-static/head.html"))
fcgi.echo(static_page("./res-static/main.html"))
fcgi.echo("<h3>你的 IP ：", fcgi.getip(), "</br>lua 页面请求计数 ：", 
		string.format("%d", fcgi.get_counter()), "<h3>")
fcgi.echo(static_page("./res-static/foot.html"))
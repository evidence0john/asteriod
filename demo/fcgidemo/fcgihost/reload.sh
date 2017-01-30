killall host.cgi
spawn-fcgi -a 127.0.0.1 -p 9000 -f /usr/share/nginx/html/host.cgi

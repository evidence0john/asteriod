cc = gcc
link = -lfcgi -llua
host.cgi: ./host.c ./load_conf.c
	$(cc) -o host.cgi ./host.c ./load_conf.c $(link)

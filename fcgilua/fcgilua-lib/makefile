cc = gcc
op = -shared -fPIC
link = -lfcgi -llua
fcgi.so: ./fcgi.c
	$(cc) $(op) $(link) -o fcgi.so ./fcgi.c

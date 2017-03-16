cc = gcc
op = -shared -fPIC
link = -lfcgi -llua
fcgi.so: ./fcgi.c
	$(cc) $(op) -o fcgi.so ./fcgi.c $(link)

cc = gcc
op = -shared -fPIC
link = -lfcgi -llua
fcgi.so: ./fcgi.c ./md5.c
	$(cc) $(op) -o fcgi.so ./fcgi.c ./md5.c $(link)

cc = gcc
op = -shared -fPIC
link = -llua
md5lua.so: ./md5lua.c
	$(cc) $(op) $(link) -o md5lua.so ./md5lua.c ../../tools/md5/md5.c

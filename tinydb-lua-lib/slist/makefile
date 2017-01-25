cc = gcc
op = -shared -fPIC
#link = -llua -lsharedlist -lswaplist
link = -llua -ltinydb
slist.so: ./slist.c
	$(cc) $(op) $(link) -o slist.so ./slist.c

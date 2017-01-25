cc = gcc
cppc = g++
op = -shared -fPIC
sources = simple_shm.c
objs = tinylist.o swaplist.o tinychain.o sharedlist.o sharedchain.o

tinydb : libtinydb.so clean
.PHONY : tinydb
libtinydb.so : $(objs)
	$(cc) $(op) -o libtinydb.so $(objs) $(sources)
tinylist.o : tinylist.c
	$(cc) $(op) -c tinylist.c
swaplist.o : ./swaplist.c
	$(cc) $(op) -c swaplist.c
tinychain.o : tinychain.cpp
	$(cppc) $(op) -c tinychain.cpp
sharedlist.o : sharedlist.c
	$(cc) $(op) -c sharedlist.c
sharedchain.o : sharedchain.cpp
	$(cppc) $(op) -c sharedchain.cpp
clean:
	find . -name "*.o"  | xargs rm -f

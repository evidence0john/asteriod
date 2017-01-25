cc = g++
op = -shared -fPIC
#link = -llua -lsharedchain
link = -llua -ltinydb
schain.so: ./schain.cpp
	$(cc) $(op) $(link) -o schain.so ./schain.cpp

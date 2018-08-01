cc = gcc

demo.so: example_module.c
	$(cc) -o demo.so example_module.c mongoose.c -shared -fPIC

lmodule.so: lmodule.c
	$(cc) -o lmodule.so lmodule.c mongoose.c -llua -shared -fPIC

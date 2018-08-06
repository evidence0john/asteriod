cc = gcc

demo.so: example_module.c
	$(cc) -o demo.so example_module.c mongoose.c -shared -fPIC

lmodule.so: lmodule.c
	$(cc) -o lmodule.so lmodule.c mongoose.c -llua -shared -fPIC

asteroid: asteroid.c handler.h handler.c lstate.c lstate.h mongoose.c mongoose.h load_conf.c
	$(cc) -o asteroid asteroid.c handler.c lstate.c mongoose.c load_conf.c -llua -ldl #-Wall
#default = make all
.PHONY: all

all: lmodule.so demo.so asteroid

clean:
	find . -name "*.o"  | xargs rm -f

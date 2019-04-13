compile_static_lib:
	@gcc -c lib1.c
	@ar crs liblib1.a lib1.o
	@rm lib1.o
compile_shared_lib:
	@gcc -c -fpic lib1.c
	@gcc -shared -o liblib1.so lib1.o
	@rm lib1.o
all:
	make compile_static_lib
	make compile_shared_lib
compile_static_tests:
	@gcc -c lib1.c
	@ar crs liblib1.a lib1.o
	@rm lib1.o
	gcc test.c -o test_static -L. -llib1 -Wall
	@rm liblib1.a
compile_static_tests:
	@gcc -c lib1.c
	@ar crs liblib1.a lib1.o
	@rm lib1.o
	gcc test.c -o test_static -L. -llib1 -Wall
	@rm liblib1.a
compile_shared_tests:
	@gcc -c -fpic lib1.c
	@gcc -shared -o liblib1.so lib1.o
	@rm lib1.o
	gcc -L. -Wl,-rpath=. -o test_shared test.c -llib1 -Wall
compile_dynamic_tests:
	@gcc -c -fpic lib1.c
	@gcc -shared -o liblib1.so lib1.o
	@rm lib1.o
	gcc -o test_dynamic test_dynamic.c -L. -ldl -Wall
run_tests:
	@echo "static library tests:\n" >> results3a.txt
	@./test_static 10000 5000 1 2 aaa 3 >> results3a.txt
	@./test_static 10000 5000 1 2 aaa 4 >> results3a.txt
	@echo "shared library tests:\n" >> results3a.txt
	@./test_shared 10000 5000 1 2 aaa 3 >> results3a.txt
	@./test_shared 10000 5000 1 2 aaa 4 >> results3a.txt
	@echo "dynamic library tests:\n" >> results3a.txt
	@./test_dynamic 10000 5000 1 2 aaa 3 >> results3a.txt
	@./test_dynamic 10000 5000 1 2 aaa 4 >> results3a.txt
all:
	make compile_static_tests
	make compile_shared_tests
	make compile_dynamic_tests

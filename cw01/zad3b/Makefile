compile_optimized_static_tests:
	@gcc -c lib1.c
	@ar crs liblib1.a lib1.o
	@rm lib1.o
	@gcc test.c -o test_static_O1 -L. -llib1 -w -O1
	@gcc test.c -o test_static_O2 -L. -llib1 -w -O2
	@gcc test.c -o test_static_Os -L. -llib1 -w -Os
	@rm liblib1.a
compile_optimized_shared_tests:
	@gcc -c -fpic lib1.c
	@gcc -shared -o liblib1.so lib1.o
	@rm lib1.o
	@gcc -L. -Wl,-rpath=. -o test_shared_O1 test.c -llib1 -O1 -w
	@gcc -L. -Wl,-rpath=. -o test_shared_O2 test.c -llib1 -O2 -w
	@gcc -L. -Wl,-rpath=. -o test_shared_Os test.c -llib1 -Os -w
compile_optimized_dynamic_tests:
	@gcc -c -fpic lib1.c
	@gcc -shared -o liblib1.so lib1.o
	@rm lib1.o
	@gcc -o test_dynamic_O1 test_dynamic.c -L. -ldl -O1 -w
	@gcc -o test_dynamic_O2 test_dynamic.c -L. -ldl -O2 -w
	@gcc -o test_dynamic_Os test_dynamic.c -L. -ldl -Os -w
run_optimized_tests:
	@echo "static O1 library tests:\n" >> results3b.txt
	@./test_static_O1 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_static_O1 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "static O2 library tests:\n" >> results3b.txt
	@./test_static_O2 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_static_O2 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "static Os library tests:\n" >> results3b.txt
	@./test_static_Os 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_static_Os 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "shared O1 library tests:\n" >> results3b.txt
	@./test_shared_O1 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_shared_O1 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "shared O2 library tests:\n" >> results3b.txt
	@./test_shared_O2 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_shared_O2 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "shared Os library tests:\n" >> results3b.txt
	@./test_shared_Os 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_shared_Os 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "dynamic O1 library tests:\n" >> results3b.txt
	@./test_dynamic_O1 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_dynamic_O1 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "dynamic O2 library tests:\n" >> results3b.txt
	@./test_dynamic_O2 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_dynamic_O2 10000 5000 1 2 aaa 4 >> results3b.txt
	@echo "dynamic Os library tests:\n" >> results3b.txt
	@./test_dynamic_Os 10000 5000 1 2 aaa 3 >> results3b.txt
	@./test_dynamic_Os 10000 5000 1 2 aaa 4 >> results3b.txt
all:
	make compile_optimized_static_tests
	make compile_optimized_shared_tests
	make compile_optimized_dynamic_tests

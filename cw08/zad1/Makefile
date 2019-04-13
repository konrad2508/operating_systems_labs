compile:
	gcc main.c -o main -Wall -lm -lpthread
tests:
	./main 1 test.pgm k3.flt out.pgm >> Times.txt
	./main 2 test.pgm k3.flt out.pgm >> Times.txt
	./main 4 test.pgm k3.flt out.pgm >> Times.txt
	./main 8 test.pgm k3.flt out.pgm >> Times.txt
	./main 1 test.pgm k5.flt out.pgm >> Times.txt
	./main 2 test.pgm k5.flt out.pgm >> Times.txt
	./main 4 test.pgm k5.flt out.pgm >> Times.txt
	./main 8 test.pgm k5.flt out.pgm >> Times.txt
	./main 1 test.pgm k10.flt out.pgm >> Times.txt
	./main 2 test.pgm k10.flt out.pgm >> Times.txt
	./main 4 test.pgm k10.flt out.pgm >> Times.txt
	./main 8 test.pgm k10.flt out.pgm >> Times.txt
clean:
	rm main
	rm out.pgm

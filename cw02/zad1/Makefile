compile:
	@gcc main.c -o main
tests:
	gcc main.c -o main
	touch a
	touch b
	./main generate file_4 2000 4
	cp file_4 file_4_cp
	./main sort file_4_cp 2000 4 sys >> wyniki.txt
	./main copy file_4_cp a 2000 4 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 2000 4 lib >> wyniki.txt
	./main copy file_4 b 2000 4 lib >> wyniki.txt
	rm file_4
	rm b
	touch a
	touch b
	./main generate file_4 4000 4
	cp file_4 file_4_cp
	./main sort file_4_cp 4000 4 sys >> wyniki.txt
	./main copy file_4_cp a 4000 4 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 4000 4 lib >> wyniki.txt
	./main copy file_4 b 4000 4 lib >> wyniki.txt
	rm file_4
	rm b
	touch a
	touch b
	./main generate file_4 2000 512
	cp file_4 file_4_cp
	./main sort file_4_cp 2000 512 sys >> wyniki.txt
	./main copy file_4_cp a 2000 512 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 2000 512 lib >> wyniki.txt
	./main copy file_4 b 2000 512 lib >> wyniki.txt
	rm file_4
	rm b
	touch a
	touch b
	./main generate file_4 4000 512
	cp file_4 file_4_cp
	./main sort file_4_cp 4000 512 sys >> wyniki.txt
	./main copy file_4_cp a 4000 512 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 4000 512 lib >> wyniki.txt
	./main copy file_4 b 4000 512 lib >> wyniki.txt
	rm file_4
	rm b
	touch a
	touch b
	./main generate file_4 2000 4096
	cp file_4 file_4_cp
	./main sort file_4_cp 2000 4096 sys >> wyniki.txt
	./main copy file_4_cp a 2000 4096 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 2000 4096 lib >> wyniki.txt
	./main copy file_4 b 2000 4096 lib >> wyniki.txt
	rm file_4
	rm b
	touch a
	touch b
	./main generate file_4 4000 4096
	cp file_4 file_4_cp
	./main sort file_4_cp 4000 4096 sys >> wyniki.txt
	./main copy file_4_cp a 4000 4096 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 4000 4096 lib >> wyniki.txt
	./main copy file_4 b 4000 4096 lib >> wyniki.txt
	rm file_4
	rm b
	touch a
	touch b
	./main generate file_4 2000 8192
	cp file_4 file_4_cp
	./main sort file_4_cp 2000 8192 sys >> wyniki.txt
	./main copy file_4_cp a 2000 8192 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 2000 8192 lib >> wyniki.txt
	./main copy file_4 b 2000 8192 lib >> wyniki.txt
	rm file_4
	rm b
	touch a
	touch b
	./main generate file_4 4000 8192
	cp file_4 file_4_cp
	./main sort file_4_cp 4000 8192 sys >> wyniki.txt
	./main copy file_4_cp a 4000 8192 sys >> wyniki.txt
	rm file_4_cp
	rm a
	./main sort file_4 4000 8192 lib >> wyniki.txt
	./main copy file_4 b 4000 8192 lib >> wyniki.txt
	rm file_4
	rm b
	rm main
all: memchunk_test

memchunk_test: memchunk.o memchunk_test.o
	gcc -m32 -o $@ $^

%.o: %.c
	gcc -m32 -c -o $@ $^

clean:
	rm -rf *.o memchunk_test
all: test_syscall2


test_syscall2: test_syscall2.o
	gcc -g --std=gnu89 test_syscall2.o -o test_syscall2

test_syscall2.o: test_syscall2.c processinfo.h
		gcc -g --std=gnu89 -c test_syscall2.c

clean:
	rm -f *.o
	rm test_syscall2




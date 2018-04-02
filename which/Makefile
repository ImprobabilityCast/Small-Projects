CFLAGS = -Wall -Os

which.exe: which.o
	gcc $(CFLAGS) -o which.exe which.o

which.o: which.c
	gcc $(CFLAGS) -c which.c

.PHONY: clean

clean:
	rm -vf which.o
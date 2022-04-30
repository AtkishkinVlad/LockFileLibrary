all: locker

CFLAGS=-c -W -Wall -O3 
#CFLAGS=-c -W -Wall -g3 

locker: main.o
	gcc -o locker main.o -lm

main.o: main.c
	gcc ${CFLAGS} main.c

.PHONY: all clean

clean:
	rm -f *.o locker

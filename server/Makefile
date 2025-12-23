CC :=gcc
all:aesdsocket.c
	$(CC) -c aesdsocket.c -o aesdsocket.o
	$(CC) -c signal.c -o signal.o

	$(CC) aesdsocket.o signal.o -o aesdsocket

clean:
	rm -f aesdsocket *.o
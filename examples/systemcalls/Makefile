CC :=$(CROSS_CPMPILE)gcc
default:systemcalls.c
	$(CC) -c systemcalls.c -o systemcalls.o
	$(CC) systemcalls.o -o systemcalls

clean:
	rm -f systemcalls *.o
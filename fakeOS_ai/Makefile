CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = shell

ALL: $(TARGET)

$(TARGET): syscalls.h str.h shell.o str.o libkernel.a
	$(CC) -o $(TARGET) shell.o str.o -L. -lkernel

libkernel.a: kernel.h syscalls.h syscalls.o url.h url.o process.o mem.o network.o
	ar rcs libkernel.a syscalls.o process.o mem.o network.o url.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	/bin/rm -f shell libkernel.a *.o

# .PHONY: clean
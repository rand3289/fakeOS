CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = shell
OBJS = shell.o str.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

shell.o: shell.c syscalls.h str.h
	$(CC) $(CFLAGS) -c shell.c

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c

clean:
	rm -f $(OBJS) $(TARGET)

# .PHONY: clean
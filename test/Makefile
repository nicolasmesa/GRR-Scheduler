CC := arm-none-linux-gnueabi-gcc
LD := arm-none-linux-gnueabi-gcc
CFLAGS := -g -Wall -Werror -static
LDFLAGS := -static

OBJECTS := syscall.o

all: syscall


: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f test 
	rm -f test.o

.PHONY: clean

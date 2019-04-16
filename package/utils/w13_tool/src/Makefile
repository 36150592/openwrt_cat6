all: w13_tool

CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread


SRC= $(wildcard *.c)

#INC= $(patsubst %.c, %.h, $(SRC))

OBJS= $(patsubst %.c, %.o, $(SRC))

%.o: %.c #$(INC)
#	echo $(INC)
	$(CC) $< -c -o $@  $(CFLAGS)

w13_tool: $(OBJS)
	$(CC)  $^ $(LDFLAGS) -o $@ -lpthread -luci



.PHONY: clean

clean:
	rm -f w13_tool *.o
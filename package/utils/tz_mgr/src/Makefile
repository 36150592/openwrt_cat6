all: tz_mgr

CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread


SRC= $(wildcard *.c)

#INC= $(patsubst %.c, %.h, $(SRC))

OBJS= $(patsubst %.c, %.o, $(SRC))

%.o: %.c #$(INC)
#	echo $(INC)
	$(CC) $< -c -o $@  $(CFLAGS)

tz_mgr: $(OBJS)
	$(CC)  $^ $(LDFLAGS) -o $@ -lpthread -luci



.PHONY: clean

clean:
	rm -f tz_mgr *.o
all: tz_upgrade_client

CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread


SRC= $(wildcard *.c)

#INC= $(patsubst %.c, %.h, $(SRC))

OBJS= $(patsubst %.c, %.o, $(SRC))

%.o: %.c #$(INC)
#	echo $(INC)
	$(CC) $< -c -o $@  $(CFLAGS)

tz_upgrade_client: $(OBJS)
	$(CC)  $^ $(LDFLAGS) -o $@ -lpthread



.PHONY: clean

clean:
	rm -f tz_upgrade_client *.o
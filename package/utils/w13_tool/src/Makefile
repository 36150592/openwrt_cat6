all: w13_tool tz_export w13_write_mac

CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread -luci


SRC= $(wildcard *.c)

#INC= $(patsubst %.c, %.h, $(SRC))

OBJS= Cshell.o w13_tool.o

%.o: %.c #$(INC)
#	echo $(INC)
	$(CC) $< -c -o $@  $(CFLAGS)

w13_tool: $(OBJS)
	$(CC)  $^ $(LDFLAGS) -o $@ 
tz_export: tz_export.o
	$(CC)  $^ $(LDFLAGS) -o $@ 
w13_write_mac: Cshell.o w13_write_mac.o
	$(CC)  $^ $(LDFLAGS) -o $@ 


.PHONY: clean

clean:
	rm -f w13_tool tz_export w13_write_mac *.o
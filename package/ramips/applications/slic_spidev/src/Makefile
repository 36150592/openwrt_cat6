CC=gcc
CFLAGS += -c  -Wall -Werror -I.
LDFLAGS += -ldl -lpthread

OBJS = main.o tz_base.o tz_socket.o

EXEC=slic_spidev

all:$(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 
	
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm *.o -f
	rm slic_spidev -f

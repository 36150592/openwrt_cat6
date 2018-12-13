CC=gcc
#CFLAGS += -c 
CFLAGS += -c  -Wall -Werror 
LDFLAGS += -ldl -lpthread

OBJS = main.o 
all:sendat

sendat: $(OBJS)
	$(CC)  $(INC) -o $@ $(OBJS) $(LDFLAGS) 
	
clean:
	rm *.o -f
	rm sendat -f

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

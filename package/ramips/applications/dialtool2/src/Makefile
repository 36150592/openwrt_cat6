CC=gcc
#CFLAGS += -c 
#CFLAGS += -c  -Wall -Werror 
CFLAGS += -c -g  -Os  -Wall -Werror -ffunction-sections -fdata-sections 
LDFLAGS += -ldl -lpthread -Wl,--gc-sections

ifdef P61_DIALTOOL_FLAG
CFLAGS += -DP61_DIALTOOL_FLAG
endif

OBJS = main.o lt10.o bm806.o ec20.o bm906.o common.o logger.o option.o queue.o serial.o sim7000.o a9500.o bm916.o

ifeq ($(TOZED_HW_VERSION), TZ7.823.317)
OBJS += boardtype_317.o
else ifeq ($(TOZED_HW_VERSION), TZ7.823.324)
OBJS += boardtype_324.o
else
OBJS += boardtype_306A.o
endif

all:dialtool_new

dialtool_new : $(OBJS)
	$(CC)  $(INC) -g -o $@ $(OBJS) $(LDFLAGS) 
	
clean:
	rm *.o -f
	rm dialtool -rf

%.o: %.c *.h
	$(CC) $(CFLAGS) $< -o $@

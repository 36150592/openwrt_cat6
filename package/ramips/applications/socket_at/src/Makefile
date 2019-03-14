#BINFILE = fastboot

SOCKET_AT_BINFILE = socket_at


#PIPE_BINFILE = pipe_data
#TEST_BINFILE = modemtest


CFLAGS += -Wall
CFLAGS += -I$(LC_GENERL_INCLUDE_DIR)

ifeq ($(IS_BIG_ENDIAN), 1)
CFLAGS += -D_BIG_ENDIAN_=1
else
CFLAGS += -D_BIG_ENDIAN_=0
endif

#LDFLAGS += -L$(LC_LIB_DIR)
LDFLAGS += -lpthread 
SRCS += socket_at.c

SOCKET_AT_SRCS = socket_at.c	

#PIPE_SRCS += pipe_data.c
#TEST_SRCS += modemtest.c mops.c

OBJS = $(SRCS:%.c=%.o)
SOCKET_AT_OBJS = $(SOCKET_AT_SRCS:%.c=%.o)
#PIPE_OBJS = $(PIPE_SRCS:%.c=%.o)
#TEST_OBJS = $(TEST_SRCS:%.c=%.o)

all:$(BINFILE) $(SOCKET_AT_BINFILE) 
#$(PIPE_BINFILE) $(TEST_BINFILE)

prep:

$(BINFILE): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) 

$(SOCKET_AT_BINFILE):$(SOCKET_AT_OBJS)
#	$(CC) $^ -o $@ $(LDFLAGS) 
	$(CC) $^ -o $@ $(LDFLAGS) 
#-lz    

#$(PIPE_BINFILE): $(PIPE_OBJS)
#	$(CC) $^ -o $@ $(LDFLAGS)

#$(TEST_BINFILE): $(TEST_OBJS)
#	$(CC) $^ -o $@ $(LDFLAGS) -lmu_api -lz

clean:
	rm -f *.o $(BINFILE) $(SOCKET_AT_BINFILE) $(PIPE_BINFILE) $(TEST_BINFILE)
#install:
	#cp -f $(PIPE_BINFILE) $(INSTALL_ROOT)/usr/sbin/
#	cp $(PIPE_BINFILE) $(INSTALL_ROOT)
#	cp $(TEST_BINFILE) ${IMAGEPATH}
#	cp $(BINFILE) ${IMAGEPATH}
#	cp $(SOCKET_AT_BINFILE) ${IMAGEPATH}


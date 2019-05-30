CC=gcc
CFLAGS += -c -g -fpic -Os  -Wall -Werror -ffunction-sections -fdata-sections 
LUATZLIB_LDFLAGS += -L./ -llua
SO_CFLAGS += -shared -ldl -lpthread

LUATZLIB_SO_OBJS = luatzlib.o sendat.o
LUATZLIB_INC += luatzlib.h

LUATZLIB_SO = luatzlib.so

all:tzlib

tzlib: $(LUATZLIB_SO_OBJS)
	$(CC)   $(LUATZLIB_INC)  $(LUATZLIB_SO_OBJS)  $(SO_CFLAGS) $(LUATZLIB_LDFLAGS)   -g -o $(LUATZLIB_SO)

clean:
	rm *.o -f
	rm $(LUATZLIB_SO_OBJS) $(LUATZLIB_SO) -rf

%.o: %.c *.h
	$(CC) $(CFLAGS) $< -o $@

EXEC    = lanrecord

OBJS    = main.o macrecord.o quota.o utility.o
CFLAGS += -g
LDLIBS += -lpthread

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $(OBJS) 


romfs:
	cp $(EXEC) $(INSTALL_DIR)/bin

adb:
	adb push $(EXEC) /tozed/bin/

clean:
	rm -f $(EXEC) *.elf *.gdb *.o


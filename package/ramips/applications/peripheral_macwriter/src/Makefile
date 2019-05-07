EXEC=peripheral_macwriter
SRC=peripheral_macwriter.c cmdlib.c des.c des3.c utility.c s21.c

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o


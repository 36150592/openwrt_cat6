EXEC=tozed_tool
SRC=update_config.c utility.c web_user_setting.c wifi_ssid_pwd.c main.c

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o


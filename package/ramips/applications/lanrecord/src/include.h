#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include "macrecord.h"
#include "quota.h"
#include "type.h"
#include "utility.h"
#include <libiptc/libiptc.h>


#define CURRENT_LAN_LIST "/tmp/.current_lan_list"
#define HISTORY_LAN_LIST "/etc/history_lan_list"
#define RESTORE_LAN_LIST_SIGN "/tmp/.restore_lan_list"


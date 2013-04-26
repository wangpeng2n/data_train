#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <string.h>
#include <mysql/mysql.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

struct nic_rate {
    char name[32];
    char speed[32];
};

struct bond_rate {
    char name[32];
    char speed[32];
};

struct nic_info {
    int bond;
    int nic;
    char total[32];
    char time[16];
    char hostname[32];
    struct nic_rate nic_rate[16];
    struct bond_rate bond_rate[16];
};

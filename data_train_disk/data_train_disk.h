
#ifndef INCLUDE_FILE
#define INCLUDE_FILE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <string.h>
#include <mysql/mysql.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXINTERFACES 16

struct disk_info {
    int id;
    char name[128];
    char mount_path[512];
    char total[16];
    char used[16];
    char hostname[36];
    char seat[8];
};

struct disk{
    int num;
    struct disk_info disk[24];
};

#endif

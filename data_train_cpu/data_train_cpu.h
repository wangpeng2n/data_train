#ifndef __DATA_TRAINDATA_CPU_H__
#define __DATA_TRAINDATA_CPU_H__

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
#include <time.h>

#define ONE_MB (1024 * 1024)
#define INSERT_VOLUME "insert into cpu_info(hostname,usedcpu,hosttime) values"

struct mysql_occupy
{
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char insert[256];
	char query[150];
};

struct occupy        
{
  char name[100];      
  unsigned int user;  
  unsigned int nice;  
  unsigned int system;
  unsigned int idle;  
};

typedef struct cpu_occupy_
{
	float cpu_used;
}cpu_occ;

struct node_occupy
{
	char pc_name[100];
	int cpu_count;
	float global_cpu;
	cpu_occ  cpu_num[8];
	int fdisk_count;
	int mysql_id;
	char mysql_ip[16];
	char pc_time[50];
};


float g_cpu_used;           
int cpu_num;                
void cal_occupy(struct occupy *, struct occupy *);  
void get_occupy(struct occupy *);
void cal_mem();
void cal_fdisk();
void cal_cpu(struct node_occupy *);



#endif

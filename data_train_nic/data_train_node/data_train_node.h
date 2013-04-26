#ifndef __NETEM_CMD_GUARD_H__
#define __NETEM_CMD_GUARD_H__

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

#define ETH_NAME	"eth0"
#define ONE_MB (1024 * 1024)
#define SELECT_NODE "select nodename from node_info"
#define INSERT_VOLUME "insert into node_info(id,nodename,ip,hosttime,cpunum,uesdmemory,totalmemory,disknum,status) values"
#define UP_VOLUME "update node_info"
#define JUDGE_BOND "/etc/sysconfig/network-scripts/ifcfg-bond0"

struct mysql_occupy
{
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char insert[150];
	char up[150];
	char query[150];
};

struct occupy        
{
  char name[20];      
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
	char pc_name[20];
	char pc_ip[20];
	char pc_time[32];
	int cpu_count;
	float global_cpu;
	long long global_mem;
	long long  mem_free;
	cpu_occ  cpu_num[8];
	int fdisk_count;
	float fdisk_szie;
	float fdisk_free;
	int mysql_id;
	char mysql_ip[16];
};

float g_cpu_used;           
int cpu_num;                
void cal_occupy(struct occupy *, struct occupy *);  
void get_occupy(struct occupy *);
void cal_mem();
void cal_fdisk();
void cal_cpu(struct node_occupy *);

#endif

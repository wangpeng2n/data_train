/********************************************************
 * Copyright (C) 2011 by Wise Tone						          *
 * wangleiyf@initdream.com									            *
 *														                          *
 * Project Name: 									                      *
 * Module Name: 							                          *	
 * Version: 1.0											                    *	
 ********************************************************/
#include "data_train_node.h"

static int judge_bond = 0;

void cal_occupy (struct occupy *o, struct occupy *n)
{
  double od, nd,id,sd,scale;   
  od = (double) (o->user + o->nice + o->system +o->idle);
  nd = (double) (n->user + n->nice + n->system +n->idle);
  scale = 100.0 / (float)(nd-od);      
  id = (double) (n->user - o->user);    
  sd = (double) (n->system - o->system);
  g_cpu_used = ((sd+id)*100.0)/(nd-od); 
}

void get_occupy (struct occupy *o) 
{
  FILE *fd = NULL;        
  int n = 0;           
  char buff[1024];  

  if( ! (fd = fopen ("/proc/stat", "r")))
	  return ;

  fgets (buff, sizeof(buff), fd); 
  for(n=0;n<cpu_num;n++)          
  {
    fgets (buff, sizeof(buff),fd);
    sscanf (buff, "%s %u %u %u %u", o[n].name, &o[n].user, &o[n].nice,&o[n].system, &o[n].idle);
  }
  fclose(fd);    
} 

void cal_cpu(struct node_occupy *node_connect)
{
//    struct occupy ocpu[10];   
        //  struct occupy ncpu[10];   
    
    cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
    node_connect->cpu_count = cpu_num;
   
#if 0
   sleep(1);                               
	  get_occupy(ocpu);                       
	  sleep(1);                                
	  get_occupy(ncpu); 
	  
	  for (i=0; i<cpu_num; i++)               
	  {
	     cal_occupy(&ocpu[i], &ncpu[i]);
	     node_connect->cpu_num[i].cpu_used = g_cpu_used;
	     if(g_cpu_used > cpu_used)
	     	  cpu_used = g_cpu_used;
	  }
   //if( cpu_used < 100.0 )	
//    	printf("global cpu used %f% \n",cpu_used);
	node_connect->global_cpu = cpu_used;
    /*else
	printf("global cpu used %f% \n",cpu_used/cpu_num);*/
#endif

}

void cal_mem( struct node_occupy *node_connect )
{ 
    long num_pages,free_pages,page_size,num_procs;
    long long  mem,free_mem;

    num_procs = sysconf (_SC_NPROCESSORS_CONF);

    page_size = sysconf (_SC_PAGESIZE);

    num_pages = sysconf (_SC_PHYS_PAGES);

    free_pages = sysconf (_SC_AVPHYS_PAGES);

    mem = (long long) ((long long)num_pages * (long long)page_size);
    mem /= ONE_MB;

    free_mem = (long long)free_pages * (long long)page_size;
    free_mem /= ONE_MB;

    node_connect->global_mem = mem;
    node_connect->mem_free = free_mem;
}

int convert_unit(char *cont) {
    long int sum;
    int n, num;

    n = strlen(cont);
    num = atoi(cont);
    
    switch(cont[n-1]) {
        case 'T':
            sum = 1024*1024*num;
            break;
        case 'G':
            sum = 1024*num;
            break;
        case 'M':
            sum = num;
            break;
        default:
            sum = 0;
            break;
    }

    sprintf(cont, "%lu", sum);
    
    return 0;
}


void cal_fdisk( struct node_occupy *node_connect )
{
	FILE * fp = NULL;
	int i = 0;
	node_connect->fdisk_count = 0;
	node_connect->fdisk_szie = 0.0;
	node_connect->fdisk_free = 0.0;

	char name[128], used[16], path[128], total[128];
	char  buffer[512], tmp[128];

	if( !(fp = popen("df -h|sed -n \'4,$p\'","r")) ){
		printf("popen null\n");
	}

	while( fgets(buffer, 500, fp) )
	{
		i = sscanf(buffer, "%s %s %s %s %s %s",name, total, used, tmp, tmp, path);
		        if ( i != 6)
				continue;

			node_connect->fdisk_szie ++;

			node_connect->fdisk_free += convert_unit(used);

			node_connect->fdisk_count++;
	}
}

#if 0
void get_local_ip( char *ip_data )
{
	int sock;
	struct sockaddr_in sin;
	struct ifreq ifr;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock == -1 ) {
		perror("socket");
		return ;
	}

	strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;

	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0){
		perror("ioctal");
		return ;
	}
	
	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	memcpy(ip_data,inet_ntoa(sin.sin_addr),strlen(inet_ntoa(sin.sin_addr)));
	
	close(sock);
}
#endif

int ip_to_int(char *ip_data)
{
	int tmp,num;

	sscanf(ip_data, "%d.%d.%d.%d",&tmp, &tmp, &tmp, &num);
	
	return num;
}

int judge_local_bond( struct node_occupy *node )
{
	FILE * fd = NULL;
        //int ip_i = 0, node_i = 0 ;
	if( !(fd = fopen("/etc/sysconfig/network-scripts/ifcfg-bond0","r"))  )
		return 1;
	else{
		char bond_data[50];
		memset(bond_data,0,50);

		while( (fgets(bond_data, 100, fd )) )
		{
			if( !memcmp(bond_data,"IPADDR",6) )
				sscanf(bond_data,"%*[^=]=%s",node->pc_ip);
		}
		fclose(fd);
		node->mysql_id = ip_to_int(node->pc_ip);
		printf("###### %s\n",node->pc_ip);
		return 0;
	}
}

int get_local_ip(char *ip) {
    FILE *fp;
    char line[256], ipaddr[128], tmp[128];

    if ((fp=fopen("/etc/hosts", "r")) == NULL)
        return 1;

    while(fgets(line, 255, fp)) {
        if(sscanf(line, "%s %s %s", ipaddr, tmp, tmp) == 3) {
            strcpy(ip, ipaddr);
            fclose(fp);
            return 0;
        }
    }
    strcpy(ip, "Unknown");
    fclose(fp);
    
    return 1;
} 

void cal_host( struct node_occupy *node )
{
	char ip_data[20];
	memset(ip_data,0,20);

	gethostname(node->pc_name,20);

	if( judge_bond )
	{
		get_local_ip(ip_data);
		memset(node->pc_ip,0,20);
		memcpy(node->pc_ip,ip_data,strlen(ip_data));
	}

	node->mysql_id = ip_to_int(node->pc_ip);
}

void cal_pctime(struct node_occupy *node_time)
{
	time_t timep;
	struct tm *p;
	FILE *fp = NULL;

	memset(node_time->pc_time,0,32);

	if((fp=fopen("/var/run/initstored.pid", "r")) != NULL){		
	time (&timep);
	p = localtime(&timep);
	sprintf(node_time->pc_time,"%04d%02d%02d%02d%02d%02d",(p->tm_year+1900),
							      (p->tm_mon+1),
							      (p->tm_mday),
							      (p->tm_hour),
							      ( p->tm_min), 
							      ( p->tm_sec)
							      );
	fclose(fp);
	}
	else{
		
		memcpy(node_time->pc_time,"0",1);
	}
}

//#if 0
void display_cal( struct node_occupy node )
{
	int i = 0;
	printf("-----------------------------------------------------------\n");
	printf("hostname		:	%s\n",node.pc_name);
	printf("ip			:	%s\n",node.pc_ip);
	printf("time			:	%s\n",node.pc_time);
	for(i = 0; i < node.cpu_count; i++)
	{
		printf( "cpu used		:	 %5.2f\n",node.cpu_num[i].cpu_used );
	}

	printf("global cpu used 	:	 %5.2f\n",node.global_cpu );
	printf("mem			:	   %lld\n", node.global_mem );
	printf("free mem 		: 	   %lld\n",node.mem_free );
	printf("fdisk total		:	   %d\n",node.fdisk_count );
	printf("fdisk size		:	 %5.2fM\n",node.fdisk_szie );
	printf("fdisk free		:	 %5.2fM\n",node.fdisk_free);
}
//#endif

void mysql_occupy_init( struct mysql_occupy *mysql_in,struct node_occupy *node )
{
	mysql_init( &mysql_in->mysql );

	if( !mysql_real_connect(&mysql_in->mysql,node->mysql_ip, "root", "syst3m","db1",0,NULL,0) )
	{
		fprintf(stderr, "Failed to connect to database: Error: %s\\n", mysql_error(&mysql_in->mysql));
	}else
		fprintf(stderr, "connected to Mysql successfully!\n");
}

void mysql_occupy_close( struct mysql_occupy *mysql_cl )
{
	mysql_close(&mysql_cl->mysql);
}

void mysql_insert(struct mysql_occupy *mysql_insert,struct node_occupy *node_data)
{
	int flag = 0; 
	sprintf(mysql_insert->insert,"%s ('%d','%s','%s','%s','%d','%d','%d','%d','%d')",
								      INSERT_VOLUME,
								      node_data->mysql_id,
								      node_data->pc_name,
							  	      node_data->pc_ip,
								      node_data->pc_time,
								      node_data->cpu_count,
								      (int)node_data->global_mem - (int)node_data->mem_free,
								      (int)node_data->global_mem,
								      (int)node_data->fdisk_szie,
								      0
								      );

	flag = mysql_real_query(&mysql_insert->mysql, mysql_insert->insert,(unsigned int)strlen(mysql_insert->insert));
	if(flag){
		printf("mysql insert ERROR\n");
	}
}

int mysql_occupy_delete( struct mysql_occupy *mysql_del, struct node_occupy *node_data )
{
	int flag = 0;
	char delete[100];
	memset(delete,0,100);

	sprintf(delete,"delete from node_info where nodename='%s'",node_data->pc_name);
	 
	flag = mysql_real_query(&mysql_del->mysql, delete,(unsigned int)strlen(delete));
	if(flag){
		printf("delete Error\n");
		return 0;
	}
	return 1;
}

void cal_mysql( struct node_occupy *node_data )
{
	struct mysql_occupy mysql_occupy;

	mysql_occupy_init(&mysql_occupy,node_data);

	if( mysql_occupy_delete(&mysql_occupy,node_data) )
		mysql_insert(&mysql_occupy, node_data);

	mysql_occupy_close(&mysql_occupy);
}

int local_mysql_ip(struct node_occupy *node_mysql_ip, int agrc, char *agrv[])
{
	if( (agrc <2) && ( NULL == agrv[1]) )
		return 1;
	memcpy(node_mysql_ip->mysql_ip,agrv[1],strlen(agrv[1]));

	return 0;
}

int main( int agrc,char *agrv[] )                 
{
	struct node_occupy node_data;

	if(local_mysql_ip(&node_data,agrc,agrv))
	{
		return -1;
	}
	
	judge_bond = judge_local_bond( &node_data );
 
	cal_cpu( &node_data );

    	cal_mem( &node_data );

    	cal_fdisk( &node_data);

	cal_host( &node_data );

	cal_pctime( &node_data );

	display_cal( node_data );

	cal_mysql( &node_data );

	return 0;
}

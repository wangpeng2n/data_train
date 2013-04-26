#include "data_train_cpu.h"

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
    struct occupy ocpu[10];   
    struct occupy ncpu[10];   
    int i = 0;
    float cpu_used = 0.0;
    
    cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
    node_connect->cpu_count = cpu_num;
   
    usleep(500000);                               
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
	node_connect->global_cpu = cpu_used;
}

void local_pc_time(struct node_occupy *node_time)
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p = localtime(&timep);
	sprintf(node_time->pc_time,"%04d%02d%02d%02d%02d%02d",(p->tm_year+1900),
							      (p->tm_mon+1),
							      (p->tm_mday),
							      (p->tm_hour),
							      ( p->tm_min), 
							      ( p->tm_sec)
							      );
}

void local_pc_host( struct node_occupy *node_host )
{
	char ip_data[100];
	memset(ip_data,0,100);
	memset(node_host->pc_name,0,100);
	gethostname(ip_data,100);
	memcpy(node_host->pc_name,ip_data,strlen(ip_data));
}

int local_mysql_ip(struct node_occupy *node_mysql_ip, int agrc, char *agrv[])
{
	if( (agrc <2) && ( NULL == agrv[1]) )
		return 1;
	strcpy(node_mysql_ip->mysql_ip,agrv[1]);
	return 0;
}

void mysql_occupy_init( struct mysql_occupy *mysql_in,struct node_occupy *node )
{
	mysql_init( &mysql_in->mysql );
	if( !mysql_real_connect( &mysql_in->mysql,node->mysql_ip, "root", "syst3m","db1",0,NULL,0 ) ) {
		fprintf(stderr, "Failed to connect to database: Error: %s\\n", mysql_error(&mysql_in->mysql));
	}
}

void mysql_insert(struct mysql_occupy *mysql_insert,struct node_occupy *node_data)
{
	int flag = 0;
	sprintf(mysql_insert->insert,"%s ('%s','%d','%s')",INSERT_VOLUME,
						   node_data->pc_name,
						   (int)node_data->global_cpu,
						   node_data->pc_time);
	flag = mysql_real_query(&mysql_insert->mysql, mysql_insert->insert,(unsigned int)strlen(mysql_insert->insert));
	if(flag){
		printf("mysql insert ERROR\n");
	}
}

void mysql_occupy_close(struct mysql_occupy *mysql_cl)
{
	mysql_close(&mysql_cl->mysql);
}

void cal_mysql( struct node_occupy *node_data )
{
	struct mysql_occupy mysql_occupy;

	mysql_occupy_init(&mysql_occupy,node_data);

	mysql_insert(&mysql_occupy, node_data);

	mysql_occupy_close(&mysql_occupy);
}

int main(int agrc,char *agrv[])
{
	struct node_occupy node;

	if(local_mysql_ip(&node,agrc,agrv)) {
		return -1;
	}

	local_pc_time(&node);

	local_pc_host(&node);

	cal_cpu(&node);

	cal_mysql( &node );

	return 0;
}

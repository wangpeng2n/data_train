#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <unistd.h>

#define BUFFER_DATA 1024*16
#define MYSQL_VOLNAME_COUNT 100
#define SELECT_VOLUME "select volname from volume_info"
#define MAX_ID "select id from volume_info"
#define INSERT_VOLUME "insert into volume_info(id,volname,usedspace,voltype,totalspace) values"
#define UP_VOLUME "update volume_info"

struct fdisk_occupy
{
	char  fdisk_online;
	float fdisk_total;
	float fdisk_free;
	char  name[5];
};

struct occupy
{
	char volume_name[10];
	char volume_type[10];
	int fdisk_num;
	float fdisk_glabl;
	float fdisk_used;
	struct fdisk_occupy fdisk_name[24];
};

struct mysql_occupy
{
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char mysql_ip[16];
	char insert[400];
	char up[150];
	char query[150];
};

int loacl_volume_on(char *buffer, char *ob)
{
	char *compare_name = NULL;
	int count = 0;

	compare_name = strstr(buffer,"\n");

	while( *compare_name != '/' )
	{
		compare_name = compare_name - 1;
		count ++;
	}
	
	memcpy( ob,compare_name+1,count-1);
	
	return count-1;
}

int fdisk_display_type(char *compare_buffer)
{
	if( strstr(compare_buffer,"MB") )
		return 1;
	if( strstr(compare_buffer,"GB"))
		return 1024;
	if( strstr(compare_buffer,"TB") )
		return 1024*1024;

	return 0;
}

void cal_volume_fdisk( char *fdisk_buffer, struct occupy *fdisk_occupy )
{
	char *compare_buffer = NULL;
	char fdisk_objective[5];
	int fdisk_name = 0;
	fdisk_occupy->fdisk_num = 0;
	
	compare_buffer = fdisk_buffer;

	while ( ( compare_buffer =  strstr( compare_buffer,"Brick                :") ) )
	{	
		int reg = 0;
		fdisk_occupy->fdisk_num++;
		reg = loacl_volume_on( compare_buffer, fdisk_objective );

		memcpy(fdisk_occupy->fdisk_name[fdisk_name].name,fdisk_objective,reg);
		memset(fdisk_objective,0,5);

		if( (compare_buffer =  strstr( compare_buffer, "Online               : ")) )
		{
			fdisk_occupy->fdisk_name[fdisk_name].fdisk_online = *( compare_buffer + 23 );
		}
		if( (compare_buffer =  strstr( compare_buffer, "Disk Space Free      : ")) )
		{
			fdisk_occupy->fdisk_name[fdisk_name].fdisk_free = atof(compare_buffer + 23) * fdisk_display_type(compare_buffer);
		}
		if( (compare_buffer =  strstr( compare_buffer, "Total Disk Space     : ")) )
		{
			fdisk_occupy->fdisk_name[fdisk_name].fdisk_total = atof(compare_buffer + 23) * fdisk_display_type(compare_buffer);
		}
		fdisk_name++;
	}
}

void loacl_occupy_fdisk( char *volume_name , struct occupy *fdisk )
{
	char comm_buffer[BUFFER_DATA] ;
	char buffer[BUFFER_DATA];
	FILE *fd = NULL ;
	memset(buffer,0,BUFFER_DATA);

	sprintf(comm_buffer, "initstore volume status %s detail", volume_name );
	if(!(fd = popen(comm_buffer,"r"))){
		printf("initstore volume  detail errot");
		return ;
	}
	fread(buffer,BUFFER_DATA,sizeof(char),fd);

	pclose(fd);

	cal_volume_fdisk(buffer,fdisk);
}

void cal_volume_name( char *buffer, struct occupy *fdisk_data )
{
	char *compare_buffer = NULL;

	compare_buffer = strstr(buffer,"\n");
	memcpy(fdisk_data->volume_name,buffer,compare_buffer-buffer);

	loacl_occupy_fdisk(fdisk_data->volume_name,fdisk_data);
}

void cal_volume_type( char *buffer, struct occupy *fdisk_data )
{
	if( 0 == memcmp("Distribute-Rep",buffer,12))
		memcpy(fdisk_data->volume_type,"Rep",3);
	else
		if(0 == memcmp("Distribute",buffer,8))
			memcpy(fdisk_data->volume_type,"Dis",3);
		else
			memcpy(fdisk_data->volume_type,"Str",3);
}

void fdisk_compute_used(struct occupy *fdisk_use)
{
	int i = 0;
	float summitor = 0.0;
	
	for(i = 0; i < fdisk_use->fdisk_num; i++)
	{
		summitor = fdisk_use->fdisk_name[i].fdisk_total + summitor;
	}

	fdisk_use->fdisk_glabl = summitor;

	summitor = 0;

	for(i = 0; i < fdisk_use->fdisk_num; i++)
	{
		summitor = fdisk_use->fdisk_name[i].fdisk_free + summitor;
	}

	fdisk_use->fdisk_used = fdisk_use->fdisk_glabl - summitor;
}

void mysql_occupy_init( struct mysql_occupy *mysql_in )
{
	mysql_init( &mysql_in->mysql );

	if( !mysql_real_connect(&mysql_in->mysql, mysql_in->mysql_ip, "root", "syst3m","db1",0,NULL,0) )
	{
		fprintf(stderr, "Failed to connect to database: Error: %s\\n", mysql_error(&mysql_in->mysql));
	}else
		fprintf(stderr, "connected to Mysql successfully!\n");
}

void mysql_occupy_close( struct mysql_occupy *mysql_cl )
{
	mysql_close(&mysql_cl->mysql);
}

void mysql_insert( struct mysql_occupy *mysql_in, struct occupy *insert_fisk, int volume_count )
{
	int id = 0,flag = 0; 
	char volname_data[MYSQL_VOLNAME_COUNT];
	memset(volname_data,0,MYSQL_VOLNAME_COUNT);
	

	sprintf(mysql_in->insert,"%s",INSERT_VOLUME);

	for( id = 0; id < volume_count; id++ )
	{
		sprintf(volname_data, "('%d','%s','%d','%s','%d')",id+1,
							          insert_fisk[id].volume_name, 
							          (int)insert_fisk[id].fdisk_used, 
						                  insert_fisk[id].volume_type,
								  (int)insert_fisk[id].fdisk_glabl);
		strcat(mysql_in->insert,volname_data);

		if( id != volume_count -1 )
			strcat(mysql_in->insert,",");
		memset(volname_data,0,MYSQL_VOLNAME_COUNT);

	}

	flag = mysql_real_query(&mysql_in->mysql, mysql_in->insert,(unsigned int)strlen(mysql_in->insert));
	if( flag ){
		printf("insert Error\n");
	}
}

void mysql_occupy_delete( struct mysql_occupy *mysql_del )
{

	int flag = 0;
	char *delete_data = "delete from volume_info where id < 10 ";

	flag = mysql_real_query(&mysql_del->mysql, delete_data,(unsigned int)strlen(delete_data));
	if(flag){
		printf("delete Error\n");
	}
}

void connect_mysql( struct occupy *insert_fisk ,int volume_count, char *agrv_ip )
{
	struct mysql_occupy mysql_occupy;

	strcpy(mysql_occupy.mysql_ip,agrv_ip);

    	mysql_occupy_init(&mysql_occupy);

	mysql_occupy_delete(&mysql_occupy);

	mysql_insert(&mysql_occupy, insert_fisk, volume_count);

	mysql_occupy_close(&mysql_occupy);
}

//#if 0
void dispaly_fdisk_occucpy( struct occupy *dispaly_fisk ,int volume_count)
{
	int i = 0,j  = 0;

	for( i = 0; i < volume_count; i++ )
	{
		printf("------------------------------------------\n");
		printf("volume name 		:	%s\n", dispaly_fisk[i].volume_name );
		printf("volume type 		:	%s\n", dispaly_fisk[i].volume_type );
		printf("fdsik count 		:	%d\n", dispaly_fisk[i].fdisk_num );
		printf("fdisk tatol		:	%.2fMB\n", dispaly_fisk[i].fdisk_glabl);
		printf("fdisk used		:	%.2fMB\n", dispaly_fisk[i].fdisk_used);
		printf("volume disk usage	:	%3.2f\n",
               (dispaly_fisk[i].fdisk_used / dispaly_fisk[i].fdisk_glabl)*100 );

		for(j =0; j < dispaly_fisk[i].fdisk_num; j ++ )
		{
			printf("==========================================\n");
			printf("fdisk name[%d]		:	%s\n", j+1,dispaly_fisk[i].fdisk_name[j].name );
			printf("fdisk online  		:	%c\n", dispaly_fisk[i].fdisk_name[j].fdisk_online);
			printf("fdisk total   		:	%f\n",dispaly_fisk[i].fdisk_name[j].fdisk_total);
			printf("fdisk_free    		:	%f\n",dispaly_fisk[i].fdisk_name[j].fdisk_free);
		}
		printf("------------------------------------------\n\n\n");
	}
}
//#endif

int main( int agrc,char *agrv[] )                 
{
	FILE *fd = NULL ;
	char buffer[256] ;
	struct occupy fdisk_occupy[20];
	int volume_i= 0;

	if(agrc < 2){
		printf("mm 192.168.1.180\n");
		return -1;
	}
	
	if( !(fd = popen("initstore volume info","r"))){
		printf("initstore volume info null");
		return -1;
	}

	while( (fgets(buffer, 256, fd )) ){
		if( strstr(buffer,"Volume Name:")){
			cal_volume_name(buffer+13,&fdisk_occupy[volume_i]);
		}
		if(strstr(buffer,"Type: ")){
			cal_volume_type(buffer+6,&fdisk_occupy[volume_i]);
			volume_i++;
		}
		memset(buffer,0,256);
		fdisk_compute_used(&fdisk_occupy[volume_i-1]);
	}
	pclose(fd);

	dispaly_fdisk_occucpy( fdisk_occupy, volume_i );
	connect_mysql( fdisk_occupy, volume_i, agrv[1]);

	return 0;
}

#include "data_train_nic.h"

#define LOG_FILE "/var/log/initstorefs/sar_info"

int write_log(const char * info)
{
    FILE *fp = fopen(LOG_FILE, "a+");
    if (fp == NULL)
		return 1;

    int num;
    time_t now;
    struct tm  *timenow;
    char str[1024], date[128];

    time(&now);
    timenow = localtime(&now);
    sprintf(date, "%s", asctime(timenow));
    num = strlen(date);
    date[num-1] = '\0';
    
    sprintf(str, "%s  ----(CLNT)PID:%d---%s.\n", date, getpid(), info);
	fwrite(str, strlen(str), 1, fp);
	fclose(fp);

	return 0;
}

long  count_nic_speed(struct nic_rate nic_rate[], int n) {
    int i = 0;
    long  sum = 0, tmp;

    while(i < n) {
        tmp = atol(nic_rate[i].speed);
        sum = sum + tmp;
        i++;
    }

    return sum;
}

long  count_bond_speed(struct bond_rate bond_rate[], int n) {
    int i = 0;
    long  sum = 0, tmp;

    while(i < n) {
        tmp = atol(bond_rate[i].speed);
        sum = sum + tmp;
        i++;
    }

    return sum;
}

int count_node_speed(struct nic_info *nic) {
    char tmp[128];
    
    if (nic->bond != 0)
        sprintf(tmp, "%ld", count_bond_speed(nic->bond_rate, nic->bond));
    else
        sprintf(tmp, "%ld", count_nic_speed(nic->nic_rate, nic->nic));
    strcpy(nic->total, tmp);
    
    return 0;
}

long  number_plus(char num[][16], int n) {
    int i = 0;
    long  sum = 0, tmp;

    while(i < n) {
        tmp = atol(num[i]);
        sum = sum + tmp;
        i++;
    }

    return sum;
}

void local_pc_time(struct nic_info *nic)
{
	time_t timep;
	struct tm *p;
    
	time (&timep);
	p = localtime(&timep);
	sprintf(nic->time,"%04d%02d%02d%02d%02d%02d",(p->tm_year+1900),
							      (p->tm_mon+1),
							      (p->tm_mday),
							      (p->tm_hour),
							      ( p->tm_min), 
							      ( p->tm_sec)
							      );
}


int print(struct nic_info *nic) {
    int i = 0;

    while(i < nic->nic) {
        printf("nic name:%s speed:%s.\n",
               nic->nic_rate[i].name, nic->nic_rate[i].speed);
        i++;
    }

    i = 0;
    
    while(i < nic->bond) {
        printf("bond name:%s speed:%s.\n",
               nic->bond_rate[i].name, nic->bond_rate[i].speed);
        i++;
    }
    
    printf("hostname:%s.\ntime:%s.\ntotal:%s.\n",
           nic->hostname, nic->time, nic->total);
    return 0;
    
}


int get_nic_speed(struct nic_info *nic) {
    FILE *fp;
    char tmp[128], line[512], name[16];
    char str[7][16];

    local_pc_time(nic);
    gethostname(nic->hostname, 31);
    if ((fp=(popen("sar -n DEV 2 1", "r"))) == NULL) {
        write_log("###############can't open sar############################.");
        return 1;
    }
    
    nic->bond = 0;
    nic->nic = 0;
    write_log("###############haha  open sar############################.");
    while(fgets(line, 511, fp)) {
            write_log(line);
        if ( (strncasecmp(line, "average:", 8) != 0)&&( strncmp( line, "平均时间:",strlen("平均时间:")) != 0 ) )
            continue;
        
        if (sscanf(line, "%s %s %s %s %s %s %s %s %s",
                   tmp, name, str[0], str[1], str[2], str[3], str[4], str[5], str[6]) != 9)
            continue;

        sprintf(tmp, "%ld", number_plus(str, 7));
        if (strncasecmp(name, "bond", 4) == 0) {
            strcpy(nic->bond_rate[nic->bond].name, name);
            strcpy(nic->bond_rate[nic->bond].speed, tmp);
            nic->bond++;
        }else if (strncasecmp(name, "eth", 3) == 0) {
            strcpy(nic->nic_rate[nic->nic].name, name);
            strcpy(nic->nic_rate[nic->nic].speed, tmp);
            nic->nic++;
        } 
    }
    write_log(line);
    pclose(fp);
    
    count_node_speed(nic);
        //print(nic);
    
    return 0;
}

int get_ip_num(const char *ip){
    int tmp, num;

    sscanf(ip, "%d.%d.%d.%d",
           &tmp, &tmp, &tmp, &num);

    return num;
}

int update_mysql(const char *sql_ip) {
    int res;
    char cmd[2048];
    MYSQL conn_prt;
    struct nic_info nic;
    
    get_nic_speed(&nic);
    print(&nic);
    mysql_init(&conn_prt);
        
	if( !mysql_real_connect(&conn_prt, sql_ip, "root", "syst3m","db1",0,NULL,0) ) {
		fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&conn_prt));
        return 1;
    }
	else
		fprintf(stderr, "connected to Mysql successfully!\n");

    sprintf(cmd, "insert into nic_info(hostname, speed, hosttime)values(\'%s\',\'%s\',\'%s\')",
            nic.hostname, nic.total, nic.time);
    
    if(!(res=mysql_query(&conn_prt, cmd))) {
        fprintf(stderr, "Inserted %lu rows.\n", (unsigned long)mysql_affected_rows(&conn_prt));
    }
        else
            fprintf(stderr, "Insert error %d: %s.\n", mysql_errno(&conn_prt), mysql_error(&conn_prt));

    mysql_close(&conn_prt);
    
    return 0;
}


int main(int argc, char *argv[]) {
    update_mysql(argv[1]);
    return 0;
}

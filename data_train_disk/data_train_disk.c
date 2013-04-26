
#include "data_train_disk.h"

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

int get_disk_info(struct disk *disk) {
    FILE *fp;
    int i, n ,num, id;
    char *str, buffer[512], tmp[128];
    char name[128], used[16], path[128], total[128];

    if( !(fp = popen("df -h|sed -n \'4,$p\'","r")) ){
		printf("popen null\n");
		return 1;
	}

    i = 0;
    n = 1;
    id = 1;
    disk->num = 0;
    while(fgets(buffer, 500, fp)) {
        num = sscanf(buffer, "%s %s %s %s %s %s",
                     name, total, used, tmp, tmp, path);
        if (num != 6)
            continue;

        if (strncasecmp(name, "/dev/sd", 7) != 0 ||
            strncasecmp(path, "/export/", 8) != 0)
            continue;
        
        convert_unit(total);
        convert_unit(used);
        gethostname(disk->disk[i].hostname, 36);

        str = name + 5;
        strcpy(disk->disk[i].name, str);
        strcpy(disk->disk[i].total, total);
        strcpy(disk->disk[i].used, used);
        strcpy(disk->disk[i].mount_path, path);
        i++;
        
        if (i%3 != 0)  {
            sprintf(disk->disk[i-1].seat, "%d%d", id, n);
            n++;
        } else {
            sprintf(disk->disk[i-1].seat, "%d%d", id, 3);
            n = 1;
            id++;
        }
    }
    disk->num = i;
    pclose(fp);
    
    return 0;
}

int get_local_ip(char *ip) {
    FILE *fp;
    char line[256], ipaddr[128], tmp[128];

    if ((fp=fopen("/etc/hosts", "r")) == NULL)
        return 1;

    while(fgets(line, 255, fp)) {
        if(sscanf(line, "%s %s %s", ipaddr, tmp, tmp) == 3 &&
           strcasecmp(tmp, "localhost") == 0) {
            strcpy(ip, ipaddr);
            fclose(fp);
            return 0;
        }
    }
    strcpy(ip, "127.0.0.1");
    fclose(fp);
    
    return 1;
} 


int print(struct disk *disk) {
    int i = 0;

    while(i < disk->num) {
        printf("seat:%s  host:%s  %s total:%sMB used:%sMB export: %s.\n",
               disk->disk[i].seat, disk->disk[i].hostname, disk->disk[i].name, disk->disk[i].total, disk->disk[i].used, disk->disk[i].mount_path);
        i++;
    }
    return 0;
}

int get_ip_num(const char *ip){
    int tmp, num;

    sscanf(ip, "%d.%d.%d.%d",
           &tmp, &tmp, &tmp, &num);

    return num;
}

int update_mysql(const char *sql_ip) {
    int res, i, ipnum, id;
    char cmd[2048];
    struct disk disk;
    MYSQL conn_prt;
    char ip[36], tmp[36], values[256];
    
    get_local_ip(ip);
    ipnum = get_ip_num(ip);
    
    get_disk_info(&disk);
    mysql_init(&conn_prt);
    
    
	if( !mysql_real_connect(&conn_prt, sql_ip, "root", "syst3m","db1",0,NULL,0) ) {
		fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&conn_prt));
        return 1;
    }
	else
		fprintf(stderr, "connected to Mysql successfully!\n");
    
    sprintf(cmd, "delete from disk_info where nodename='%s'", disk.disk[0].hostname);
    if((res=mysql_query(&conn_prt, cmd)))
        fprintf(stderr, "Delete error %d: %s.\n", mysql_errno(&conn_prt), mysql_error(&conn_prt));

    fprintf(stderr, "delete %lu rows.\n", (unsigned long)mysql_affected_rows(&conn_prt));
    
    i = 0;
    strcpy(cmd, "insert into disk_info(id, coord, diskname,diskpath,diskspace,diskusedspace,nodename,status) values");
    while(i < disk.num) {
        sprintf(tmp ,"%d%d", ipnum, i+1);
        id = atoi(tmp);
        sprintf(values, "(%d, \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\', 0),",
                id, disk.disk[i].seat, disk.disk[i].name, disk.disk[i].mount_path, disk.disk[i].total, disk.disk[i].used, disk.disk[i].hostname);
        strcat(cmd, values);
        i++;
    }

    i = strlen(cmd);
    cmd[i-1] = '\0';

        //printf("\n\ncmd:%s.\n\n",cmd);
    if(!(res=mysql_query(&conn_prt, cmd))) {
        fprintf(stderr, "Inserted %lu rows.\n", (unsigned long)mysql_affected_rows(&conn_prt));
    }
        else
            fprintf(stderr, "Insert error %d: %s.\n", mysql_errno(&conn_prt), mysql_error(&conn_prt));
    
    mysql_close(&conn_prt);
        //print(&disk);
    
    return 0;
}

int main(int argc, char *argv[]) {
        //const char ip[] = "192.168.1.180";
    update_mysql(argv[1]);
    return 0;
}

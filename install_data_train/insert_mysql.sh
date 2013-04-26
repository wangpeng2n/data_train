#!/bin/bash
HOSTNAME="192.168.135.165"
PORT="3306"
USERNAME="root"
PASSWORD="syst3m"
DBNAME="db1"
TABLENAME="disk_info"

create_db_sql="create database ${DBNAME};use db1;"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} -e "${create_db_sql}"

create_table_sql="CREATE TABLE disk_info (  id INTEGER NOT NULL ,diskname varchar(24) , diskpath varchar(24) , diskspace INTEGER ,diskusedspace INTEGER , nodename varchar(24) , status INTEGER,coord  varchar(5), PRIMARY KEY (id))"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} ${DBNAME} -e "${create_table_sql}"

create_table_sql="CREATE TABLE cpdisk_info (  id INTEGER NOT NULL ,diskname varchar(24) , diskpath varchar(24) , diskspace INTEGER ,diskusedspace INTEGER , nodenam varchar(24) , status INTEGER,ccord  varchar(5), PRIMARY KEY (id))"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} ${DBNAME} -e "${create_table_sql}"

create_table_sql="CREATE TABLE node_info (id INTEGER NOT NULL,nodename varchar(20),ip varchar(15) ,hosttime varchar(32), cpunum INTEGER ,uesdmemory INTEGER ,totalmemory INTEGER ,disknum INTEGER ,status INTEGER, PRIMARY KEY (id))"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} ${DBNAME} -e "${create_table_sql}"

create_table_sql="CREATE TABLE cpnode_info (id INTEGER NOT NULL,nodename varchar(20),ip varchar(15) ,hosttime varchar(32), cpunum INTEGER ,uesdmemory INTEGER ,totalmemory INTEGER ,disknum INTEGER ,status INTEGER, PRIMARY KEY (id))"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} ${DBNAME} -e "${create_table_sql}"

create_table_sql="CREATE TABLE volume_info (id INTEGER NOT NULL ,volname varchar(20),usedspace INTEGER ,voltype varchar(10) ,totalspace INTEGER ,PRIMARY KEY (id))"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} ${DBNAME} -e "${create_table_sql}"

create_table_sql="CREATE TABLE cpu_info (  id INT AUTO_INCREMENT , hostname varchar(24) , usedcpu INTEGER, hosttime varchar(32), PRIMARY KEY (id))"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} ${DBNAME} -e "${create_table_sql}"

create_table_sql="CREATE TABLE nic_info (id INT AUTO_INCREMENT , hostname varchar(24) , speed INTEGER, hosttime varchar(32), PRIMARY KEY (id))"
mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} ${DBNAME} -e "${create_table_sql}"

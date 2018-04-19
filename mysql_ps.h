#ifndef __MYSQL_PS_H__
#define __MYSQL_PS_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int utils_free(void **p);
void *utils_malloc(int size);

int mysql_conn_initer(void **conn, char* host, char *username, char *password, char *database);
int mysql_disconn(void *conn);
int mysql_stmt_initer(void* conn, void **stmt);
int mysql_stmt_prepare_bind(void *stmt, void **parambind, int *paramcount, void **resultbind, int *resultcount, const char *sql);
int mysql_bind_map(void *bind, int bindsize, int pos, char *data, int datalen, unsigned long int *len);
int mysql_bind_map2(void *bind, int bindsize, int pos, char **data, int datalen);
int mysql_stmt_query_record(void *stmt, void *parambind, void *resultbind);
int mysql_stmt_query_column(void *stmt, void *bind, int index, char *buf, int len);
int mysql_stmt_query_column2(void *stmt, void *bind, int index, char **buf);
int mysql_clear_stmt(void *stmt);

#endif // __MYSQL_PS_H__

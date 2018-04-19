#include "mysql_ps.h"
#include <mysql/mysql.h>
#include <mysql/errmsg.h>

int utils_free(void **p)
{
	if (!p || *p == NULL) {
		return -1;
	}
	free(*p);
	*p = NULL;
	return 0;
}

void *utils_malloc(int size)
{
	void *p = NULL;

	if (size <= 0) {
		return NULL;
	}
	p = malloc(size);
	if (p) {
		memset(p, 0, size);
	}
	return p;
}

int mysql_conn_initer(void **conn, char* host, char *username, char *password, char *database)
{
    void * conn_buf = NULL;

    if (!conn || !host || !username || !password || !database) {
        printf("mysql execute param is invalid\n");
		return -1;
    }
    if (!(conn_buf = mysql_init(NULL))) {
        printf("mysql_init() failed:%s\n", mysql_error(conn_buf));
        return -1;
    }

    if(!mysql_real_connect(conn_buf, host, username, password, database, 0, NULL, 0)) {
        printf("mysql_real_connect() failed:%s\n", mysql_error(conn_buf));
        mysql_close(conn_buf);
    }
    *conn = conn_buf;

    return 0;
}

int mysql_disconn(void *conn)
{
    if (conn) {
        mysql_close(conn);
        return 0;
    }
    return -1;
}

int mysql_stmt_initer(void* conn, void **stmt)
{
	void *stmt_buf = NULL;

	if (!conn) {
		printf("mysql execute param is invalid\n");
		return -1;
	}

	if (!(stmt_buf = mysql_stmt_init(conn))) {
		printf("mysql_stmt_init() failed:%s\n", mysql_stmt_error(stmt_buf));
		return -1;
	}
	*stmt = stmt_buf;

	return 0;
}

int mysql_stmt_prepare_bind(void *stmt, void **parambind, int *paramcount, void **resultbind, int *resultcount, const char *sql)
{
	void	*parambind_buf = NULL;
	void	*resultbind_buf = NULL;
	int		paramcount_tmp = 0;
	int		resultcount_tmp = 0;
	int		bind_size = 0;

	if (!stmt || !sql) {
		printf("mysql execute param is invalid\n");
		return -1;
	}

	if (mysql_stmt_prepare(stmt, sql, strlen(sql))) {
        printf("mysql stmt prepare failed(%s)\n", mysql_stmt_error(stmt));
		return -1;
    }

	if ((paramcount_tmp = mysql_stmt_param_count(stmt)) < 0) {
		printf("mysql_stmt_param_count() failed:%s\n", mysql_stmt_error(stmt));
		return -1;
	}
	bind_size		= paramcount_tmp * sizeof(MYSQL_BIND);
	parambind_buf	= malloc(bind_size);
	if (!parambind_buf) {
		printf("malloc memory failed:line %d\n", __LINE__);
		return -1;
	}
	memset(parambind_buf, 0, bind_size);

	if ((resultcount_tmp = mysql_stmt_field_count(stmt)) < 0) {
		printf("mysql_stmt_field_count() failed:%s\n", mysql_stmt_error(stmt));
		return -1;
	}
	bind_size		= resultcount_tmp * sizeof(MYSQL_BIND);
	resultbind_buf	= malloc(bind_size);
	if (!resultbind_buf) {
		printf("malloc memory failed:line %d\n", __LINE__);
		free(parambind_buf);
		return -1;
	}
	memset(resultbind_buf, 0, bind_size);

	*parambind	= parambind_buf;
	*resultbind	= resultbind_buf;
	*paramcount	= paramcount_tmp;
	*resultcount= resultcount_tmp;

	return 0;
}

int mysql_bind_map(void *bind, int bindsize, int pos, char *data, int datalen, unsigned long int *len)
{
	if (!bind || datalen < 0 || pos >= bindsize || pos < 0 || !data|| !len) {
		return -1;
	}
	len[pos] = strlen(data);

    ((MYSQL_BIND*)bind)[pos].buffer_type	= MYSQL_TYPE_VAR_STRING;
    ((MYSQL_BIND*)bind)[pos].buffer			= (char *)data;
    ((MYSQL_BIND*)bind)[pos].buffer_length	= datalen;
    ((MYSQL_BIND*)bind)[pos].is_null		= 0;
    ((MYSQL_BIND*)bind)[pos].length			= &len[pos];

	return 0;
}

int mysql_bind_map2(void *bind, int bindsize, int pos, char **data, int datalen)
{
	if (!bind || datalen < 0 || pos >= bindsize || pos < 0) {
		return -1;
	}
	*data = (char*)malloc(datalen);
	if (!*data) {
		return -1;
	}

	((MYSQL_BIND*)bind)[pos].buffer_type	= MYSQL_TYPE_VAR_STRING;
    ((MYSQL_BIND*)bind)[pos].buffer			= (char *)*data;
    ((MYSQL_BIND*)bind)[pos].buffer_length	= datalen;
    ((MYSQL_BIND*)bind)[pos].is_null		= 0;

	return 0;
}


int mysql_stmt_query_record(void *stmt, void *parambind, void *resultbind)
{
	int err = 0;

	if (!stmt || !resultbind) {
		return -1;
	}
	if (parambind && mysql_stmt_bind_param(stmt, parambind)) {
		printf("mysql_stmt_bind_param() failed:\n%s\n", mysql_stmt_error(stmt));
		return -1;
	}

	if (mysql_stmt_execute(stmt)) {
		printf("mysql_stmt_execute() failed:\n%s\n", mysql_stmt_error(stmt));
		return -1;
	}
	if (mysql_stmt_bind_result(stmt, resultbind)) {
		printf("mysql_stmt_bind_result() failed:\n%s\n", mysql_stmt_error(stmt));
		return -1;
	}
	if (mysql_stmt_store_result(stmt)) {
		printf("mysql_stmt_bind_result() failed:\n%s\n", mysql_stmt_error(stmt));
		return -1;
	}

	if (err = mysql_stmt_fetch(stmt)) {
		printf("mysql_stmt_fetch() failed[%d]:\n%s\n", err, mysql_stmt_error(stmt));
		return -1;
	}

	return 0;
}

int mysql_stmt_query_column(void *stmt, void *bind, int index, char *buf, int len)
{

	if (!stmt || !bind || index < 0 || !buf || len <= 0) {
		printf("sql query data fail, param is invalid\n");
		return -1;
	}
    snprintf(buf, len, "%s", !mysql_stmt_fetch_column(stmt, &((MYSQL_BIND *)bind)[index], index, 0) ? (char *)((MYSQL_BIND *)bind)[index].buffer : "");

	return 0;
}

int mysql_stmt_query_column2(void *stmt, void *bind, int index, char **buf)
{
	int len = 0;

	if (!stmt || !bind || index < 0) {
		printf("sql query data fail, param is invalid\n");
		return -1;
	}

    if(mysql_stmt_fetch_column(stmt, &((MYSQL_BIND *)bind)[index], index, 0)) {
		printf("mysql_stmt_fetch_column failed(%d)\n", __LINE__);
		return -1;
	}
	len		= strlen(((MYSQL_BIND *)bind)[index].buffer) + 1;
	*buf	= (char *)malloc(len);
	if (!*buf) {
		printf("mysql_stmt_query_column failed(%d)\n", __LINE__);
		return -1;
	}
	snprintf(*buf, len, "%s", (char *)((MYSQL_BIND *)bind)[index].buffer);

	return 0;
}

int mysql_clear_stmt(void *stmt)
{
	if (stmt) {
		mysql_stmt_close(stmt);
	}
	return 0;
}

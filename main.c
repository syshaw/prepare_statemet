#include "mysql_ps.h"

struct DataNode{
    char id[16];
    char name[16];
    char *content;
};

int prepare_statement(struct DataNode** info, char *paramid, char *paramname)
{
    void *conn						= NULL;
    void *stmt						= NULL;
    void *parambind					= NULL;
    void *resultbind				= NULL;
	int paramcount					= 0;
	int resultcount					= 0;
	unsigned long int *paramlen		= NULL;
	unsigned long int *resultlen	= NULL;
	int ret							= -1;
	char sql[64]					= {0};
	char id[16]						= {0};
	char name[16]					= {0};
	char bindid[16]					= {0};
	char bindname[16]				= {0};
	struct DataNode *pinfo			= NULL;
	struct DataNode bindinfo		= {0};

	if (!sql) {
		return -1;
	}
    if (mysql_conn_initer(&conn, "localhost", "username", "password", "database")) {
		return -2;
    }
    if (mysql_stmt_initer(conn, &stmt)) {
		goto end;
		return -3;
    }
	snprintf(sql, sizeof(sql), "select id,name,cont from test where id=? and name=?");
	if (mysql_stmt_prepare_bind(stmt, &parambind, &paramcount, &resultbind, &resultcount, sql) != 0) {
		ret = -4;
		goto end;
	}

	/*绑定参数*/
	if (paramcount) {
		paramlen = utils_malloc(paramcount * sizeof(int));
		if (!paramlen) {
			ret = -5;
			goto end;
		}
		mysql_bind_map(parambind, paramcount, 0, paramid, strlen(paramid), paramlen);
		mysql_bind_map(parambind, paramcount, 1, paramname, strlen(paramname), paramlen);
	}

	resultlen = utils_malloc(resultcount * sizeof(int));
	if (!resultlen) {
		ret = -6;
		goto end;
	}

	/*绑定结果*/
	mysql_bind_map(resultbind, resultcount, 0, bindinfo.id, sizeof(bindinfo.id), resultlen);
	mysql_bind_map(resultbind, resultcount, 1, bindinfo.name, sizeof(bindinfo.name), resultlen);
	mysql_bind_map2(resultbind, resultcount, 2, &bindinfo.content, 64);

	if (mysql_stmt_query_record(stmt, parambind, resultbind) < 0) {
		ret = -7;
		goto end;
	}

	pinfo = utils_malloc(sizeof(struct DataNode));
	if (!pinfo) {
		ret = -8;
		goto end;
	}

	/*获取查询结果*/
	mysql_stmt_query_column(stmt, resultbind, 0, pinfo->id, sizeof(pinfo->id));
	mysql_stmt_query_column(stmt, resultbind, 1, pinfo->name, sizeof(pinfo->name));
	mysql_stmt_query_column2(stmt, resultbind, 2, &pinfo->content);

	*info = pinfo;
	ret = 0;
end:
	/*预处理内存释放*/
	utils_free((void **)&paramlen);
	utils_free((void **)&resultlen);
	utils_free((void **)&parambind);
	utils_free((void **)&resultbind);
	utils_free((void **)&bindinfo.content);
	mysql_clear_stmt(stmt);
	mysql_disconn(conn);

	return ret;
}

int main()
{
    struct DataNode *node	= NULL;
	int ret					= -1;
    if (!(ret = prepare_statement(&node, "1", "name_1"))) {
        printf("id\t:%s\nname\t:%s\ncontent\t:%s\n", node->id, node->name, node->content);
        utils_free((void**)&node->content);
        utils_free((void**)&node);
    } else {
    	printf("return failed %d\n", ret);
    }

    return 0;
}

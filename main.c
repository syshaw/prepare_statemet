#include "mysql_ps.h"

#define HOST		"localhost"
#define USER		"username"
#define PASSWORD	"password"
#define DATABASE	"database"

struct DataNode{
    char id[16];
    char name[16];
    char *content;
};

/**read**/
int prepare_statement_read(struct DataNode** info, char *paramid, char *paramname)
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

	if (!paramid || !paramname) {
		return -1;
	}
	if (mysql_conn_initer(&conn, HOST, USER, PASSWORD, DATABASE)) {
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
	mysql_bind_map2(resultbind, resultcount, 2, &bindinfo.content, 128);

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

/**insert**/
int prepare_statement_insert(struct DataNode* info)
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


	if (!info) {
		return -1;
	}
	if (mysql_conn_initer(&conn, HOST, USER, PASSWORD, DATABASE)) {
		return -2;
	}
	if (mysql_stmt_initer(conn, &stmt)) {
		goto end;
		return -3;
	}
	snprintf(sql, sizeof(sql), "insert into test (id,name,cont)values(?,?,?)");
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
		mysql_bind_map(parambind, paramcount, 0, info->id, strlen(info->id), paramlen);
		mysql_bind_map(parambind, paramcount, 1, info->name, strlen(info->name), paramlen);
		mysql_bind_map(parambind, paramcount, 2, info->content, strlen(info->content), paramlen);
	}

	if (mysql_stmt_query_record(stmt, parambind, resultbind) < 0) {
		ret = -6;
		goto end;
	}

	ret = 0;
end:
	/*预处理内存释放*/
	utils_free((void **)&paramlen);
	utils_free((void **)&resultlen);
	utils_free((void **)&parambind);
	utils_free((void **)&resultbind);
	mysql_clear_stmt(stmt);
	mysql_disconn(conn);

	return ret;
}

/**update**/
int prepare_statement_update(struct DataNode* info)
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


	if (!info) {
		return -1;
	}
	if (mysql_conn_initer(&conn, HOST, USER, PASSWORD, DATABASE)) {
		return -2;
	}
	if (mysql_stmt_initer(conn, &stmt)) {
		goto end;
		return -3;
	}
	snprintf(sql, sizeof(sql), "update test set name=?,cont=? where id = ?");
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

		mysql_bind_map(parambind, paramcount, 0, info->name, strlen(info->name), paramlen);
		mysql_bind_map(parambind, paramcount, 1, info->content, strlen(info->content), paramlen);
		mysql_bind_map(parambind, paramcount, 2, info->id, strlen(info->id), paramlen);
	}

	if (mysql_stmt_query_record(stmt, parambind, resultbind) < 0) {
		ret = -6;
		goto end;
	}

	ret = 0;
end:
	/*预处理内存释放*/
	utils_free((void **)&paramlen);
	utils_free((void **)&resultlen);
	utils_free((void **)&parambind);
	utils_free((void **)&resultbind);
	mysql_clear_stmt(stmt);
	mysql_disconn(conn);

	return ret;
}

/**delete**/
int prepare_statement_delete(struct DataNode* info)
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


	if (!info) {
		return -1;
	}
	if (mysql_conn_initer(&conn, HOST, USER, PASSWORD, DATABASE)) {
		return -2;
	}
	if (mysql_stmt_initer(conn, &stmt)) {
		goto end;
		return -3;
	}
	snprintf(sql, sizeof(sql), "delete from test where id = ?");
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
		mysql_bind_map(parambind, paramcount, 0, info->id, strlen(info->id), paramlen);
	}

	if (mysql_stmt_query_record(stmt, parambind, resultbind) < 0) {
		ret = -6;
		goto end;
	}

	ret = 0;
end:
	/*预处理内存释放*/
	utils_free((void **)&paramlen);
	utils_free((void **)&resultlen);
	utils_free((void **)&parambind);
	utils_free((void **)&resultbind);
	mysql_clear_stmt(stmt);
	mysql_disconn(conn);

	return ret;
}

int main()
{
	struct DataNode *node	= NULL;
	struct DataNode node2	= {0};
	char *content			= "im syshaw, i like ananimation by japanese, but i dont like japanese who was politician";
	int len					= 0;
	int ret					= -1;

	/***read***/
	if (!(ret = prepare_statement_read(&node, "11", "mobile"))) {
		printf("read success...............!\n");
		printf("id\t:%s\nname\t:%s\ncontent\t:%s\n", node->id, node->name, node->content);
		utils_free((void**)&node->content);
		utils_free((void**)&node);
	} else {
		printf("read failed %d\n", ret);
	}

#if 0
	/***insert***/
    snprintf(node2.id, sizeof(node2.id), "%s", "10086");
    snprintf(node2.name, sizeof(node2.name), "%s", "mobile");
    len = strlen(content) + 1;
    node2.content = utils_malloc(len);
    if (!node2.content) {
		printf("malloc failed\n");
		exit(-1);
    }
	snprintf(node2.content, len, "%s", content);

	if (!(ret = prepare_statement_insert(&node2))) {
		printf("insert success...............!\n");
	} else {
		printf("insert failed %d\n", ret);
	}

	/***update***/
	snprintf(node2.id, sizeof(node2.id), "%s", "2");
	if (!(ret = prepare_statement_update(&node2))) {
		printf("update success...............!\n");
		utils_free((void**)&node2.content);
	} else {
		printf("update failed %d\n", ret);
	}

	/***delete***/
	snprintf(node2.id, sizeof(node2.id), "%s", "4");
	if (!(ret = prepare_statement_delete(&node2))) {
		printf("delete success...............!\n");
		utils_free((void**)&node2.content);
	} else {
		printf("delete failed %d\n", ret);
	}
#endif

	return 0;
}

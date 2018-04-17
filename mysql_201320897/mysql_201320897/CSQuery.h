#pragma once
#include <mysql.h>

class CSQuery
{
private:
	MYSQL *m_pConnection, conn;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
public:
	bool connect();
public:
	CSQuery();
	~CSQuery();
};


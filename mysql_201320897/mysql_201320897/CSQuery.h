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
	bool Progress();
	void Print();
private:
	int Query(MYSQL *_pConnection, char *queryString = nullptr);
public:
	CSQuery();
	~CSQuery();
};


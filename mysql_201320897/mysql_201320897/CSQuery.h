#pragma once
#include <mysql.h>
enum E_TABLE
{
	DEPARTMENT = 0,
	EMPLOYEE,
	DELP_LOCATIONS,
	PROJECT,
	WORKS_ON,
	DEPENDENT,
};

class CSQuery
{
private:
	MYSQL *m_pConnection, conn;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
public:
	bool connect();
	bool Progress();
	bool Print();
private:
	int Query(MYSQL *_pConnection, char *queryString = nullptr);
	bool PrintHelper(int count, char *_string, E_TABLE myTable);
public:
	CSQuery();
	~CSQuery();
};


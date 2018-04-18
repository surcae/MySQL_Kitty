#include "stdafx.h"
#include "CSQuery.h"
#include <stdio.h>

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "COMPANYX"

#define QUERY(String) Query(this->m_pConnection, String) != 0 
#define QUERY_ERROR fprintf(stderr, "Mysql query error: %s", mysql_error(&conn))

bool CSQuery::connect() {
	// 초기화, 연결 수행
	mysql_init(m_pConnection);
	
	m_pConnection = mysql_real_connect(&conn, DB_HOST, 
		DB_USER, DB_PASS, NULL, 3306, (char *)NULL, 0);

	printf("MySQL client version: %s\n============================\n", mysql_get_client_info());

	// 연결 확인
	if(m_pConnection)
		return true;
	else
	{
		fprintf(stderr, "Mysql connection error: %s", mysql_error(&conn));
		return false;
	}
}

bool CSQuery::Progress() {
	//TODO: SQL 쿼리문을 수행한다. 스키마 생성, 릴레이션 생성, 튜플 삽입
	if (QUERY("CREATE SCHEMA IF NOT EXISTS `COMPANY` DEFAULT CHARACTER SET utf8;"))
	{
		QUERY_ERROR;
		return false;
	}

	return true;
}
void CSQuery::Print() {

}

int CSQuery::Query(MYSQL * _pConnection, char * queryString) {
	return mysql_query(_pConnection, queryString);
}


CSQuery::CSQuery() : m_pConnection(NULL)
{
}


CSQuery::~CSQuery()
{
}

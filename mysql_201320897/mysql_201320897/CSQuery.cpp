#include "stdafx.h"
#include "CSQuery.h"

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "COMPANYX"

bool CSQuery::connect() {
	// �ʱ�ȭ, ���� ����
	mysql_init(m_pConnection);
	
	m_pConnection = mysql_real_connect(&conn, DB_HOST, 
		DB_USER, DB_PASS, DB_NAME, 5959, (char *)NULL, 0);

	// ���� Ȯ��
	if(!m_pConnection)
		return true;
	else // ���� ����
	{
		fprintf(stderr, "Mysql connection error: %s", mysql_error(&conn));
		return false;
	}
}


CSQuery::CSQuery() : m_pConnection(NULL)
{
}


CSQuery::~CSQuery()
{
}

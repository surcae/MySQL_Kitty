#include "stdafx.h"
#include "CSQuery.h"
#include <stdio.h>

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "root"
#define PORT 3306

#define QUOTE(...) #__VA_ARGS__
#define QUERY_ERROR fprintf(stderr, "Mysql query error: %s\n", mysql_error(&conn))
#define QUERY_BEHAVIOR(String) Query(this->m_pConnection, String) != 0 
#define QUERY(String) if( QUERY_BEHAVIOR(String) ) { \
				      QUERY_ERROR;           \
					  return false;          \
					  }                      \

bool CSQuery::connect() {
	// �ʱ�ȭ, ���� ����
	mysql_init(m_pConnection);
	
	m_pConnection = mysql_real_connect(&conn, DB_HOST, 
		DB_USER, DB_PASS, NULL, PORT, (char *)NULL, 0);

	printf("MySQL client version: %s\n============================\n", mysql_get_client_info());

	// ���� Ȯ��
	if(m_pConnection)
		return true;
	else
	{
		fprintf(stderr, "Mysql connection error: %s\n", mysql_error(&conn));
		return false;
	}
}

bool CSQuery::Progress() {
	/*
		[ Progress ]
	1. ���(��Ű��) ����
	2. ���̺� ����
	3. �ܷ�Ű ����(ALTER)
	4. Ʃ�� ����
	
	# MySQL Server�� �ִ� DB�� �ش� Ŭ���� �Ҹ��ڿ��� DROP ���� ������ Database �� ���·� ���� ��Ŵ.
	# ������ �״�� �����ϰ� ������ �Ҹ��� ���� �ּ� ó���ϱ� �ٶ�
	*/
	
	//	[ Ư¡ ]
	//SQL �������� �����Ѵ�. ��Ű�� ����, �����̼� ����, Ʃ�� ����
	//QUERY(""); �⺻ ������ (��ũ�� ����)
	//�������� ��ũ��ġ���� Forwarding �ؼ� Script �ؽ�Ʈ ����, �ٿ��ֱ���
	
	//	[ Ư�̻��� ]
	/* CREATE TABLE �� �� ���� ���̺��� ��������� �ʾҴµ� 
	Constraint Foreign Key ������ ���� �ٸ� ���̺��� ��������� ���� ���¶� ������ �Ұ����ϴ�.
	�ϴ� ���̺��� �⺻ ��Ҹ� ���������� ��� ���̺��� �����ϰ� ���߿� ALTER TABLE �������� ... �ؼ� �ܷ�Ű ������ ������.
	*/
	
	// DB�� �̹� ������ �����ϰ� ���� �����Ѵ�.
	QUERY("DROP DATABASE IF EXISTS COMPANYX;");

	// Database ���� Name: COMPANYX
	QUERY("CREATE DATABASE COMPANYX DEFAULT CHARACTER SET utf8 ;");
	printf("�����ͺ��̽� �����Ϸ�\n");

	// Database Ȱ��ȭ
	QUERY("USE COMPANYX;");
	printf("�����ͺ��̽� Ȱ��ȭ��\n");

	// DEPARTMENT ���̺� ����
	QUERY(QUOTE(CREATE TABLE DEPARTMENT (
		`Dname` VARCHAR(15) NOT NULL,
		`Dnumber` INT NOT NULL,
		`Mgr_ssn` CHAR(9) NOT NULL,
		`Mgr_start_date` DATE NULL,
		PRIMARY KEY(`Dnumber`),
			UNIQUE INDEX `Dname_UNIQUE` (`Dname` ASC),
			INDEX `fk_Mgr_ssn_idx` (`Mgr_ssn` ASC)) ENGINE = InnoDB;
		));
	printf("DEPARTMENT ���̺� �����Ϸ�\n");
	

	
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
	// DB �״�� ������� �� �Ʒ� ������ �ּ�ǥ���� ��.
	mysql_query(m_pConnection, "DROP DATABASE IF EXISTS COMPANYX");
	
	// ���� ����
	mysql_close(m_pConnection);
}

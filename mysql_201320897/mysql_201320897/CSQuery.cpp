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
				      QUERY_ERROR;					 \
					  return false;					 \
					  }								 \

bool CSQuery::connect() {
	// �ʱ�ȭ, ���� ����
	mysql_init(m_pConnection);

	m_pConnection = mysql_real_connect(&conn, DB_HOST,
		DB_USER, DB_PASS, NULL, PORT, (char *)NULL, 0);

	printf("MySQL client version: %s\n============================\n", mysql_get_client_info());

	// ���� Ȯ��
	if (m_pConnection)
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
	0. �ɼ� ����
	1. ���(��Ű��) ����
	2. ���̺� ����
	3. �ܷ�Ű ����(ALTER)
	4. Ʃ�� ����
	5. �ɼ� ����

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
	printf("�����ͺ��̽� ��������Ȯ�οϷ�");

	// Database ���� Name: COMPANYX
	QUERY("CREATE DATABASE COMPANYX DEFAULT CHARACTER SET utf8 ;");
	printf("�����ͺ��̽� �����Ϸ�\n");

	// Database Ȱ��ȭ
	QUERY("USE COMPANYX;");
	printf("�����ͺ��̽� Ȱ��ȭ��\n\n");

	#pragma region ���̺� ���� �ڵ�
	// ���̺� ����
	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPARTMENT`(
		`Dname` VARCHAR(15) NOT NULL,
		`Dnumber` INT NOT NULL,
		`Mgr_ssn` CHAR(9) NOT NULL,
		`Mgr_start_date` DATE NULL,
		PRIMARY KEY(`Dnumber`),
			UNIQUE INDEX `Dname_UNIQUE` (`Dname` ASC),
			INDEX `fk_Mgr_ssn_idx` (`Mgr_ssn` ASC)) ENGINE = InnoDB;
	));
	printf("DEPARTMENT ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`EMPLOYEE` (
		`Fname` VARCHAR(15) NOT NULL,
		`Minit` CHAR NULL,
		`Lname` VARCHAR(15) NOT NULL,
		`Ssn` CHAR(9) NOT NULL,
		`Bdate` DATE NULL,
		`Address` VARCHAR(30) NULL,
		`Sex` CHAR NULL,
		`Salary` DECIMAL(10, 2) NULL,
		`Super_ssn` CHAR(9) NULL,
		`Dno` INT NOT NULL,
		PRIMARY KEY(`Ssn`),
			INDEX `fk_Super_ssn_idx` (`Super_ssn` ASC),
			INDEX `fk_Dno_idx` (`Dno` ASC)) ENGINE = InnoDB;
	));
	printf("EMPLOYEE ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPT_LOCATIONS` (
		`Dnumber` INT NOT NULL,
		`Dlocation` VARCHAR(15) NOT NULL,
		PRIMARY KEY(`Dnumber`, `Dlocation`)) ENGINE = InnoDB;
	));
	printf("DELP_LOCATIONS ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`PROJECT` (
		`Pname` VARCHAR(15) NOT NULL,
		`Pnumber` INT NOT NULL,
		`Plocation` VARCHAR(15) NULL,
		`Dnum` INT NOT NULL,
		PRIMARY KEY(`Pnumber`),
			UNIQUE INDEX `Pname_UNIQUE` (`Pname` ASC),
			INDEX `fk_Dnum_idx` (`Dnum` ASC)) ENGINE = InnoDB;
	));
	printf("PROJECT ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`WORKS_ON` (
		`Essn` CHAR(9) NOT NULL,
		`Pno` INT NOT NULL,
		`Hours` DECIMAL(3, 1) NOT NULL,
		PRIMARY KEY(`Essn`, `Pno`),
			INDEX `fk_Pno_idx` (`Pno` ASC))	ENGINE = InnoDB;
	));
	printf("WORKS_ON ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPENDENT` (
		`Essn` CHAR(9) NOT NULL,
		`Dependent_name` VARCHAR(15) NOT NULL,
		`Sex` CHAR NULL,
		`Bdate` DATE NULL,
		`Relationship` VARCHAR(8) NULL,
		PRIMARY KEY(`Essn`, `Dependent_name`)) ENGINE = InnoDB;
	));
	printf("DEPENDENT ���̺� �����Ϸ�\n\n");
#pragma endregion
	#pragma region �������� �߰� �ڵ�
	// �ܷ�Ű �������� �߰�
	// ALTER TABLE ���̺��
	// ADD [CONSTRAINT �������Ǹ� ...]
	QUERY(QUOTE(ALTER TABLE DEPARTMENT ADD 
		CONSTRAINT `fk_Mgr_ssn`
		FOREIGN KEY(`Mgr_ssn`)
			REFERENCES `COMPANYX`.`EMPLOYEE` (`Ssn`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("DEPARTMENT ��������(�ܷ�Ű) ����Ϸ�\n");

	QUERY(QUOTE(ALTER TABLE EMPLOYEE ADD 
		CONSTRAINT `fk_Super_ssn`
		FOREIGN KEY(`Super_ssn`)
			REFERENCES `COMPANYX`.`EMPLOYEE` (`Ssn`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	QUERY(QUOTE(ALTER TABLE EMPLOYEE ADD
		CONSTRAINT `fk_Dno`
		FOREIGN KEY(`Dno`)
			REFERENCES `COMPANYX`.`DEPARTMENT` (`Dnumber`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("EMPLOYEE ��������(�ܷ�Ű) ����Ϸ�\n");

	QUERY(QUOTE(ALTER TABLE DEPT_LOCATIONS ADD 
		CONSTRAINT `fk_Dnumber`
		FOREIGN KEY(`Dnumber`)
			REFERENCES `COMPANYX`.`DEPARTMENT` (`Dnumber`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("DEPT_LOCATIONS ��������(�ܷ�Ű) ����Ϸ�\n");

	QUERY(QUOTE(ALTER TABLE PROJECT ADD 
		CONSTRAINT `fk_Dnum`
		FOREIGN KEY(`Dnum`)
			REFERENCES `COMPANYX`.`DEPARTMENT` (`Dnumber`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("PROJECT ��������(�ܷ�Ű) ����Ϸ�\n");

	QUERY(QUOTE(ALTER TABLE WORKS_ON ADD
		CONSTRAINT `fk_Essn`
		FOREIGN KEY(`Essn`)
			REFERENCES `COMPANYX`.`EMPLOYEE` (`Ssn`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	QUERY(QUOTE(ALTER TABLE WORKS_ON ADD
				CONSTRAINT `fk_Pno`
				FOREIGN KEY(`Pno`)
					REFERENCES `COMPANYX`.`PROJECT` (`Pnumber`)
						ON DELETE NO ACTION
						ON UPDATE NO ACTION));
	printf("WORKS_ON ��������(�ܷ�Ű) ����Ϸ�\n");

	QUERY(QUOTE(ALTER TABLE DEPENDENT ADD
		CONSTRAINT `fk_Essn2`
		FOREIGN KEY(`Essn`)
			REFERENCES `COMPANYX`.`EMPLOYEE` (`Ssn`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("DEPENDENT ��������(�ܷ�Ű) ����Ϸ�\n\n");
#pragma endregion

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

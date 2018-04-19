#include "stdafx.h"
#include "CSQuery.h"
#include <stdio.h>

// ������ ��ǻ���� ��� �ּ� Ǯ��
//#define MYCOM 

// ����
#define DB_HOST "127.0.0.1"
// ���� ��ǻ�Ϳ�
#define DB_USER "root"
#define DB_PASS "root"
#define PORT 4306

// ��ũ��
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
	#ifdef MYCOM
	m_pConnection = mysql_real_connect(&conn, DB_HOST,
		DB_USER, DB_PASS, NULL, PORT, (char *)NULL, 0);

	printf("MySQL client version: %s\n", mysql_get_client_info());
	printf("============================\n");
#endif
	#ifndef MYCOM
	char ID[30], PW[30];
	int port_number = NULL;

	printf("MySQL client version: %s\n", mysql_get_client_info());
	printf("USER ID : "); scanf_s("%s", ID, (unsigned int)sizeof(ID));
	printf("PASSWORD: "); scanf_s("%s", PW, (unsigned int)sizeof(PW));
	printf("PORT NUMBER: "); scanf_s("%4d", &port_number);
	printf("============================\n");

	m_pConnection = mysql_real_connect(&conn, DB_HOST,
		ID, PW, NULL, (unsigned int)port_number, (char *)NULL, 0);
#endif
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
	�̷� ��� ��Ű���� �� �߸� ¥���� �ִٰ� ������ �ܷ�Ű ������ ���߿� �ؼ� �ذ��� ���� �ִ�.
	�ٸ� ������δ� ���� ���̺�(n:m ó��)�� ���� �����ϴ� ����� �ִ�. (�̷��� ���̺��� �þ���� ������ �������� ���õ� �ٷ� �ٷ� ����)
	*/

	// DB�� �̹� ������ �����ϰ� ���� �����Ѵ�.
	QUERY("DROP DATABASE IF EXISTS COMPANYX;");
	printf("�����ͺ��̽� ��������Ȯ�οϷ�\n");

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
			INDEX `fk_Mgr_ssn_idx` (`Mgr_ssn` ASC)); //ENGINE = InnoDB;
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
			INDEX `fk_Dno_idx` (`Dno` ASC)); //ENGINE = InnoDB;
	));
	printf("EMPLOYEE ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPT_LOCATIONS` (
		`Dnumber` INT NOT NULL,
		`Dlocation` VARCHAR(15) NOT NULL,
		PRIMARY KEY(`Dnumber`, `Dlocation`)); //ENGINE = InnoDB;
	));
	printf("DELP_LOCATIONS ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`PROJECT` (
		`Pname` VARCHAR(15) NOT NULL,
		`Pnumber` INT NOT NULL,
		`Plocation` VARCHAR(15) NULL,
		`Dnum` INT NOT NULL,
		PRIMARY KEY(`Pnumber`),
			UNIQUE INDEX `Pname_UNIQUE` (`Pname` ASC),
			INDEX `fk_Dnum_idx` (`Dnum` ASC)); //ENGINE = InnoDB;
	));
	printf("PROJECT ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`WORKS_ON` (
		`Essn` CHAR(9) NOT NULL,
		`Pno` INT NOT NULL,
		`Hours` DECIMAL(3, 1) NOT NULL,
		PRIMARY KEY(`Essn`, `Pno`),
			INDEX `fk_Pno_idx` (`Pno` ASC)); //ENGINE = InnoDB;
	));
	printf("WORKS_ON ���̺� �����Ϸ�\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPENDENT` (
		`Essn` CHAR(9) NOT NULL,
		`Dependent_name` VARCHAR(15) NOT NULL,
		`Sex` CHAR NULL,
		`Bdate` DATE NULL,
		`Relationship` VARCHAR(8) NULL,
		PRIMARY KEY(`Essn`, `Dependent_name`)); //ENGINE = InnoDB;
	));
	printf("DEPENDENT ���̺� �����Ϸ�\n\n");
#pragma endregion
	#pragma region �������� �߰� �ڵ�
	// �ܷ�Ű �������� �߰�
	// ALTER TABLE ���̺��
	// ADD [CONSTRAINT �������Ǹ� ...]
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
	#pragma region Ʃ�� ���� �ڵ�
	// Ʃ�õ� ����
	// ���� ���Ἲ�� ��Ű�� ���� ���� �⺻Ű���� ���� �ְ� ���߿� ������ �κ��� �־��ش�
	
	QUERY("INSERT INTO `COMPANYX`.`DEPARTMENT` (`Dname`, `Dnumber`, `Mgr_ssn`, `Mgr_start_date`) VALUES('Research', 5, '333445555', '1988-05-22');");
	QUERY("INSERT INTO `COMPANYX`.`DEPARTMENT` (`Dname`, `Dnumber`, `Mgr_ssn`, `Mgr_start_date`) VALUES('Administration', 4, '987654321', '1995-01-01');");
	QUERY("INSERT INTO `COMPANYX`.`DEPARTMENT` (`Dname`, `Dnumber`, `Mgr_ssn`, `Mgr_start_date`) VALUES('Headquarters', 1, '888665555', '1981-06-19');");
	printf("DEPARTMENT Ʃ�� ���ԿϷ�\n");

	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('John', 'B', 'Smith', '123456789', '1965-01-09', '731 Fondren, Houston, TX', 'M', 30000, '333445555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Franklin', 'T', 'Wong', '333445555', '1955-12-08', '638 Voss, Houston, TX', 'M', 40000, '888665555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Alicia', 'J', 'Zelaya', '999887777', '1968-01-19', '3321 Castle, Spring, TX', 'F', 25000, '987654321', 4);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Jennifer', 'S', 'Wallace', '987654321', '1941-06-20', '291 Berry, Bellaire, TX', 'F', 43000, '888665555', 4);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Ramesh', 'K', 'Narayan', '666884444', '1962-09-15', '975 Fire Oak, Humble, TX', 'M', 38000, '333445555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Joyce', 'A', 'English', '453453453', '1972-07-31', '5631 Rice, Houston, TX', 'F', 25000, '333445555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Ahmad', 'V', 'Jabbar', '987987987', '1969-03-29', '980 Dallas, Houston, TX', 'M', 25000, '987654321', 4);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('James', 'E', 'Borg', '888665555', '1937-11-10', '450 Stone, Houston, TX', 'M', 55000, NULL, 1);");
	printf("EMPLOYEE Ʃ�� ���ԿϷ�\n\n");
	
	// ��ȣ ���� ���� �ذ��� ���ؼ� ���⼭ �������� �߰���
	printf("��ȣ���� ���� �ذ��� ���� �������� ����\n");

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
	printf("EMPLOYEE ��������(�ܷ�Ű) ����Ϸ�\n\n");

	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(1, 'Houston');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(4, 'Stafford');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(5, 'Bellaire');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(5, 'Sugarland');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(5, 'Houston');");
	printf("DEPT_LOCATIONS Ʃ�� ���ԿϷ�\n");

	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('ProductX', 1, 'Bellaire', 5);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('ProductY', 2, 'Sugarland', 5);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('ProductZ', 3, 'Houston', 5);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('Computerization', 10, 'Stafford', 4);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('Reorganization', 20, 'Houston', 1);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('Newbenefits', 30, 'Stafford', 4);");
	printf("PROJECT Ʃ�� ���ԿϷ�\n");

	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('123456789', 1, 32.5);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('123456789', 2, 7.5);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('666884444', 3, 40.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('453453453', 1, 20.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('453453453', 2, 20.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('333445555', 2, 10.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('333445555', 3, 10.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('333445555', 10, 10.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('333445555', 20, 10.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('999887777', 30, 30.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('999887777', 10, 10.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('987987987', 10, 35.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('987987987', 30, 5.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('987654321', 30, 20.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('987654321', 20, 15.0);");
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('888665555', 20, 10);"); // Hours�� 10�� �־ �������ּ��� from ����.
	printf("WORKS_ON Ʃ�� ���ԿϷ�\n");

	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('333445555', 'Alice', 'F', '1986-04-05', 'Daughter');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('333445555', 'Theodore', 'M', '1983-10-25', 'Son');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('333445555', 'Joy', 'F', '1958-05-03', 'Spouse');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('987654321', 'Abner', 'M', '1942-02-28', 'Spouse');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('123456789', 'Michael', 'M', '1988-01-04', 'Son');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('123456789', 'Alice', 'F', '1988-12-30', 'Daughter');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('123456789', 'Elizabeth', 'F', '1967-05-05', 'Spouse');");
	printf("DEPENDENT Ʃ�� ���ԿϷ�\n\n");

	printf("============================\n");
	printf("������ ����\n");
#pragma endregion

	return true;
}
bool CSQuery::Print() {
	// �������� �䱸�ϴ� ������ ���ǳ�Ʈ�� �ִ� ���������� �����ϰ� ����� �����ִ� �Լ�
	#pragma region ����1
	// 1. SELECT * FROM <Relation name>;
	printf("###ù��° ����###\n");
	if (!SelectAllFromTable("SELECT * FROM DEPARTMENT;", DEPARTMENT))
		return false;
	if (!SelectAllFromTable("SELECT * FROM EMPLOYEE;", EMPLOYEE))
		return false;
	if (!SelectAllFromTable("SELECT * FROM DEPT_LOCATIONS;", DEPT_LOCATIONS))
		return false;
	if (!SelectAllFromTable("SELECT * FROM PROJECT;", PROJECT))
		return false;
	if (!SelectAllFromTable("SELECT * FROM WORKS_ON;", WORKS_ON))
		return false;
	if (!SelectAllFromTable("SELECT * FROM DEPENDENT;", DEPENDENT))
		return false;
	printf("\n");
#pragma endregion
	#pragma region ����2
	// 2. ���ǳ�Ʈ Chapter 4�� page 26 & 27�� �ִ� ����0, 1, 2
	// 2-1 ����0
	printf("###�ι�° ����###\n");
	printf("%s\n", "SELECT Bdate, Address FROM EMPLOYEE WHERE Fname='John' AND Minit='B' AND Lname='Smith';");
	QUERY("SELECT Bdate, Address FROM EMPLOYEE WHERE Fname='John' AND Minit='B' AND Lname='Smith';");
	sql_result = mysql_store_result(m_pConnection); // ����

	printf("%15s %35s\n", "Bdate", "Address");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s %35s\n", sql_row[0], sql_row[1]);
	}

	// 2-2 ����1
	printf("%s\n", "SELECT Fname, Lname, Address FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research' AND Dnumber=Dno;");
	QUERY("SELECT Fname, Lname, Address FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research' AND Dnumber=Dno;");
	sql_result = mysql_store_result(m_pConnection); // ����

	printf("%10s %10s %35s\n", "Fname", "Lname", "Address");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%10s %10s %35s\n", sql_row[0], sql_row[1], sql_row[2]);
	}

	// 2-3 ����2
	printf("%s\n", "SELECT Pnumber, Dnum, Lname, Address, Bdate FROM PROJECT, DEPARTMENT, EMPLOYEE WHERE Dnum=Dnumber AND Mgr_ssn=Ssn AND Plocation='Stafford';");
	QUERY("SELECT Pnumber, Dnum, Lname, Address, Bdate FROM PROJECT, DEPARTMENT, EMPLOYEE WHERE Dnum=Dnumber AND Mgr_ssn=Ssn AND Plocation='Stafford';");
	sql_result = mysql_store_result(m_pConnection); // ����

	printf("%10s %5s %10s %35s %15s\n", "Pnumber", "Dnum", "Lname", "Address", "Bdate");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%10s %5s %10s %35s %15s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
	}
	printf("\n");
#pragma endregion
	#pragma region ����3
	// 3. ���ǳ�Ʈ Chapter 4�� page 35(33��)�� �ִ� ����4
	printf("###����° ����###\n");
	printf("%s\n%s\n%s\n",
		"(SELECT DISTINCT Pnumber FROM PROJECT, DEPARTMENT, EMPLOYEE WHERE Dnum=Dnumber AND Mgr_ssn=Ssn AND Lname='Smith')",
		"UNION",
		"(SELECT DISTINCT Pnumber FROM PROJECT, WORKS_ON, EMPLOYEE WHERE Pnumber=Pno AND Essn=Ssn AND Lname='Smith');"
	);
	QUERY(QUOTE(
		(SELECT DISTINCT Pnumber FROM PROJECT, DEPARTMENT, EMPLOYEE WHERE Dnum = Dnumber AND Mgr_ssn = Ssn AND Lname = 'Smith')
		UNION
		(SELECT DISTINCT Pnumber FROM PROJECT, WORKS_ON, EMPLOYEE WHERE Pnumber = Pno AND Essn = Ssn AND Lname = 'Smith');
	));
	sql_result = mysql_store_result(m_pConnection); // ����
	printf("%6s\n", "Pnumber");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%6s\n", sql_row[0]);
	}
	printf("\n");
#pragma endregion
	#pragma region ����4
	// 4. ���ǳ�Ʈ Chapter 5�� page 20�� �ִ� ����20, 21, 22�� page 22�� �ִ� ����28 - from ���� "Join"�� ������� ����
	// 4-1 ����20
	printf("###�׹�° ����###(Join ���X)\n");
	printf("%s\n", "SELECT SUM(Salary), MAX(Salary), MIN(Salary), AVG(Salary) FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research';");
	QUERY("SELECT SUM(Salary), MAX(Salary), MIN(Salary), AVG(Salary) FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research';");
	sql_result = mysql_store_result(m_pConnection); // ����
	printf("%15s %15s %15s %15s\n", "SUM(Salary)", "MAX(Salary)", "MIN(Salary)", "AVG(Salary)");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s %15s %15s %15s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
	}

	// 4-2 ����21
	printf("%s\n", "SELECT COUNT(*) FROM EMPLOYEE;");
	QUERY("SELECT COUNT(*) FROM EMPLOYEE;");
	sql_result = mysql_store_result(m_pConnection); // ����
	printf("%15s\n", "COUNT(*)");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s\n", sql_row[0]);
	}

	// 4-3 ����22
	printf("%s\n", "SELECT COUNT(*) FROM EMPLOYEE, DEPARTMENT WHERE DNO=DNUMBER AND DNAME='Research';");
	QUERY("SELECT COUNT(*) FROM EMPLOYEE, DEPARTMENT WHERE DNO=DNUMBER AND DNAME='Research';");
	sql_result = mysql_store_result(m_pConnection); // ����
	printf("%15s\n", "COUNT(*)");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s\n", sql_row[0]);
	}

	// 4-4 ����28
	printf("%s\n", QUOTE(
		SELECT Dnumber, COUNT(*) FROM DEPARTMENT, EMPLOYEE WHERE Dnumber = Dno AND Salary > 40000 AND
		(SELECT Dno FROM EMPLOYEE GROUP BY Dno HAVING COUNT(*) > 5);
	));
	QUERY(QUOTE(
		SELECT Dnumber, COUNT(*) FROM DEPARTMENT, EMPLOYEE WHERE Dnumber = Dno AND Salary > 40000 AND
		(SELECT Dno FROM EMPLOYEE GROUP BY Dno HAVING COUNT(*) > 5);
	));
	sql_result = mysql_store_result(m_pConnection); // ����
	printf("%15s %15s\n", "Dnumber", "COUNT(*)");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s %15s\n", sql_row[0], sql_row[1]);
	}
	printf("\n");
#pragma endregion
	return true;
}

int CSQuery::Query(MYSQL * _pConnection, char * queryString) {
	return mysql_query(_pConnection, queryString);
}

bool CSQuery::SelectAllFromTable(char *_string, E_TABLE myTable)
{
	// �̷��� ¥�� ������ ��¹��� �Ǿ� ���߿� Column �߰�, �����Ǹ� ������ �����������. �ſ� ��ȿ�����̶� db�κ��� column ����, ������ ũ�⸦ �޾Ƽ� �ڵ�ȭ ��ų �ʿ䰡 ����.
	// for Select * from Table;
	printf("%s\n", _string);
	QUERY(_string); // ������ return false;
	sql_result = mysql_store_result(m_pConnection); // ����

	/*
	enum E_TABLE
	{
		DEPARTMENT = 0,
		EMPLOYEE,
		DEPT_LOCATIONS,
		PROJECT,
		WORKS_ON,
		DEPENDENT,
	};
	*/

	// Columns ��� ���
	switch (myTable) {
	case DEPARTMENT:
		printf("%20s %10s %15s %15s\n"
			, "Dname", "Dnumber", "Mgr_ssn", "Mgr_start_date");
		break;
	case EMPLOYEE:
		printf("%10s %6s %10s %12s %13s %30s %5s %10s %15s %5s\n"
			, "Fname", "Minit", "Lname", "Ssn", "Bdate", "Address", "Sex", "Salary", "Super_ssn", "Dno");
		break;
	case DEPT_LOCATIONS:
		printf("%5s %15s\n"
			, "Dnumber", "Dlocation");
		break;
	case PROJECT:
		printf("%20s %10s %15s %5s\n"
			, "Pname", "Pnumber", "Plocation", "Dnum");
		break;
	case WORKS_ON:
		printf("%15s %5s %10s\n"
			, "Essn", "Pno", "Hours");
		break;
	case DEPENDENT:
		printf("%15s %15s %5s %15s %15s\n"
			, "Essn", "Dependent_name", "Sex", "Bdate", "Relationship");
		break;
	}

	// 
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		switch (myTable) {
		case DEPARTMENT:
			printf("%20s %10s %15s %15s\n"
				, sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
			break;
		case EMPLOYEE:
			printf("%10s %6s %10s %12s %13s %30s %5s %10s %15s %5s\n"
				, sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5]
				, sql_row[6], sql_row[7], sql_row[8], sql_row[9]);
			break;
		case DEPT_LOCATIONS:
			printf("%5s %15s\n"
				, sql_row[0], sql_row[1]);
			break;
		case PROJECT:
			printf("%20s %10s %15s %5s\n"
				, sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
			break;
		case WORKS_ON:
			printf("%15s %5s %10s\n"
				, sql_row[0], sql_row[1], sql_row[2]);
			break;
		case DEPENDENT:
			printf("%15s %15s %5s %15s %15s\n"
				, sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
			break;
		}
	}

	return true;
}


CSQuery::CSQuery() : m_pConnection(nullptr)
{
}


CSQuery::~CSQuery()
{
	// DB �״�� ������� �� �Ʒ� ������ �ּ�ǥ���� ��.
	mysql_query(m_pConnection, "DROP DATABASE IF EXISTS COMPANYX");

	// ���� ����
	mysql_close(m_pConnection);
}

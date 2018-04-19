#include "stdafx.h"
#include "CSQuery.h"
#include <stdio.h>

// 제작자 컴퓨터일 경우 주석 풀기
//#define MYCOM 

// 공용
#define DB_HOST "127.0.0.1"
// 본인 컴퓨터용
#define DB_USER "root"
#define DB_PASS "root"
#define PORT 4306

// 매크로
#define QUOTE(...) #__VA_ARGS__
#define QUERY_ERROR fprintf(stderr, "Mysql query error: %s\n", mysql_error(&conn))
#define QUERY_BEHAVIOR(String) Query(this->m_pConnection, String) != 0 
#define QUERY(String) if( QUERY_BEHAVIOR(String) ) { \
				      QUERY_ERROR;					 \
					  return false;					 \
					  }								 \

bool CSQuery::connect() {
	// 초기화, 연결 수행
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
	// 연결 확인
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
	1. 디비(스키마) 생성
	2. 테이블 삽입
	3. 외래키 설정(ALTER)
	4. 튜플 삽입

	# MySQL Server에 있는 DB는 해당 클래스 소멸자에서 DROP 구문 날려서 Database 원 상태로 복귀 시킴.
	# 데이터 그대로 유지하고 싶으면 소멸자 내용 주석 처리하기 바람
	*/

	//	[ 특징 ]
	//SQL 쿼리문을 수행한다. 스키마 생성, 릴레이션 생성, 튜플 삽입
	//QUERY(""); 기본 쿼리문 (매크로 참고)
	//쿼리문은 워크벤치에서 Forwarding 해서 Script 텍스트 복사, 붙여넣기함

	//	[ 특이사항 ]
	/* CREATE TABLE 할 때 먼저 테이블이 만들어지지 않았는데
	Constraint Foreign Key 셋팅이 들어가면 다른 테이블이 만들어지지 않은 상태라서 접근이 불가능하다.
	일단 테이블의 기본 요소만 만들어놓은후 모든 테이블을 셋팅하고 나중에 ALTER TABLE 제약조건 ... 해서 외래키 설정을 해주자.
	이런 경우 스키마가 좀 잘못 짜여져 있다고 하지만 외래키 셋팅을 나중에 해서 해결할 수도 있다.
	다른 방법으로는 따로 테이블(n:m 처럼)을 만들어서 저장하는 방법이 있다. (이러면 테이블이 늘어나지만 쿼리가 빨라지고 셋팅도 바로 바로 가능)
	*/

	// DB가 이미 있으면 삭제하고 새로 시작한다.
	QUERY("DROP DATABASE IF EXISTS COMPANYX;");
	printf("데이터베이스 생성유무확인완료\n");

	// Database 생성 Name: COMPANYX
	QUERY("CREATE DATABASE COMPANYX DEFAULT CHARACTER SET utf8 ;");
	printf("데이터베이스 생성완료\n");

	// Database 활성화
	QUERY("USE COMPANYX;");
	printf("데이터베이스 활성화됨\n\n");

	#pragma region 테이블 생성 코드
	// 테이블 생성
	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPARTMENT`(
		`Dname` VARCHAR(15) NOT NULL,
		`Dnumber` INT NOT NULL,
		`Mgr_ssn` CHAR(9) NOT NULL,
		`Mgr_start_date` DATE NULL,
		PRIMARY KEY(`Dnumber`),
			UNIQUE INDEX `Dname_UNIQUE` (`Dname` ASC),
			INDEX `fk_Mgr_ssn_idx` (`Mgr_ssn` ASC)); //ENGINE = InnoDB;
	));
	printf("DEPARTMENT 테이블 생성완료\n");

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
	printf("EMPLOYEE 테이블 생성완료\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPT_LOCATIONS` (
		`Dnumber` INT NOT NULL,
		`Dlocation` VARCHAR(15) NOT NULL,
		PRIMARY KEY(`Dnumber`, `Dlocation`)); //ENGINE = InnoDB;
	));
	printf("DELP_LOCATIONS 테이블 생성완료\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`PROJECT` (
		`Pname` VARCHAR(15) NOT NULL,
		`Pnumber` INT NOT NULL,
		`Plocation` VARCHAR(15) NULL,
		`Dnum` INT NOT NULL,
		PRIMARY KEY(`Pnumber`),
			UNIQUE INDEX `Pname_UNIQUE` (`Pname` ASC),
			INDEX `fk_Dnum_idx` (`Dnum` ASC)); //ENGINE = InnoDB;
	));
	printf("PROJECT 테이블 생성완료\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`WORKS_ON` (
		`Essn` CHAR(9) NOT NULL,
		`Pno` INT NOT NULL,
		`Hours` DECIMAL(3, 1) NOT NULL,
		PRIMARY KEY(`Essn`, `Pno`),
			INDEX `fk_Pno_idx` (`Pno` ASC)); //ENGINE = InnoDB;
	));
	printf("WORKS_ON 테이블 생성완료\n");

	QUERY(QUOTE(CREATE TABLE IF NOT EXISTS `COMPANYX`.`DEPENDENT` (
		`Essn` CHAR(9) NOT NULL,
		`Dependent_name` VARCHAR(15) NOT NULL,
		`Sex` CHAR NULL,
		`Bdate` DATE NULL,
		`Relationship` VARCHAR(8) NULL,
		PRIMARY KEY(`Essn`, `Dependent_name`)); //ENGINE = InnoDB;
	));
	printf("DEPENDENT 테이블 생성완료\n\n");
#pragma endregion
	#pragma region 제약조건 추가 코드
	// 외래키 제약조건 추가
	// ALTER TABLE 테이블명
	// ADD [CONSTRAINT 제약조건명 ...]
	QUERY(QUOTE(ALTER TABLE DEPT_LOCATIONS ADD 
		CONSTRAINT `fk_Dnumber`
		FOREIGN KEY(`Dnumber`)
			REFERENCES `COMPANYX`.`DEPARTMENT` (`Dnumber`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("DEPT_LOCATIONS 제약조건(외래키) 연결완료\n");

	QUERY(QUOTE(ALTER TABLE PROJECT ADD 
		CONSTRAINT `fk_Dnum`
		FOREIGN KEY(`Dnum`)
			REFERENCES `COMPANYX`.`DEPARTMENT` (`Dnumber`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("PROJECT 제약조건(외래키) 연결완료\n");

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
	printf("WORKS_ON 제약조건(외래키) 연결완료\n");

	QUERY(QUOTE(ALTER TABLE DEPENDENT ADD
		CONSTRAINT `fk_Essn2`
		FOREIGN KEY(`Essn`)
			REFERENCES `COMPANYX`.`EMPLOYEE` (`Ssn`)
				ON DELETE NO ACTION
				ON UPDATE NO ACTION));
	printf("DEPENDENT 제약조건(외래키) 연결완료\n\n");
#pragma endregion
	#pragma region 튜플 삽입 코드
	// 튜플들 삽입
	// 참조 무결성을 지키기 위해 먼저 기본키들을 먼저 넣고 나중에 부족한 부분을 넣어준다
	
	QUERY("INSERT INTO `COMPANYX`.`DEPARTMENT` (`Dname`, `Dnumber`, `Mgr_ssn`, `Mgr_start_date`) VALUES('Research', 5, '333445555', '1988-05-22');");
	QUERY("INSERT INTO `COMPANYX`.`DEPARTMENT` (`Dname`, `Dnumber`, `Mgr_ssn`, `Mgr_start_date`) VALUES('Administration', 4, '987654321', '1995-01-01');");
	QUERY("INSERT INTO `COMPANYX`.`DEPARTMENT` (`Dname`, `Dnumber`, `Mgr_ssn`, `Mgr_start_date`) VALUES('Headquarters', 1, '888665555', '1981-06-19');");
	printf("DEPARTMENT 튜플 삽입완료\n");

	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('John', 'B', 'Smith', '123456789', '1965-01-09', '731 Fondren, Houston, TX', 'M', 30000, '333445555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Franklin', 'T', 'Wong', '333445555', '1955-12-08', '638 Voss, Houston, TX', 'M', 40000, '888665555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Alicia', 'J', 'Zelaya', '999887777', '1968-01-19', '3321 Castle, Spring, TX', 'F', 25000, '987654321', 4);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Jennifer', 'S', 'Wallace', '987654321', '1941-06-20', '291 Berry, Bellaire, TX', 'F', 43000, '888665555', 4);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Ramesh', 'K', 'Narayan', '666884444', '1962-09-15', '975 Fire Oak, Humble, TX', 'M', 38000, '333445555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Joyce', 'A', 'English', '453453453', '1972-07-31', '5631 Rice, Houston, TX', 'F', 25000, '333445555', 5);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('Ahmad', 'V', 'Jabbar', '987987987', '1969-03-29', '980 Dallas, Houston, TX', 'M', 25000, '987654321', 4);");
	QUERY("INSERT INTO `COMPANYX`.`EMPLOYEE` (`Fname`, `Minit`, `Lname`, `Ssn`, `Bdate`, `Address`, `Sex`, `Salary`, `Super_ssn`, `Dno`) VALUES('James', 'E', 'Borg', '888665555', '1937-11-10', '450 Stone, Houston, TX', 'M', 55000, NULL, 1);");
	printf("EMPLOYEE 튜플 삽입완료\n\n");
	
	// 상호 참조 문제 해결을 위해서 여기서 제약조건 추가함
	printf("상호참조 문제 해결을 위해 제약조건 연결\n");

	QUERY(QUOTE(ALTER TABLE DEPARTMENT ADD
	CONSTRAINT `fk_Mgr_ssn`
	FOREIGN KEY(`Mgr_ssn`)
	REFERENCES `COMPANYX`.`EMPLOYEE` (`Ssn`)
	ON DELETE NO ACTION
	ON UPDATE NO ACTION));
	printf("DEPARTMENT 제약조건(외래키) 연결완료\n");

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
	printf("EMPLOYEE 제약조건(외래키) 연결완료\n\n");

	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(1, 'Houston');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(4, 'Stafford');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(5, 'Bellaire');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(5, 'Sugarland');");
	QUERY("INSERT INTO `COMPANYX`.`DEPT_LOCATIONS` (`Dnumber`, `Dlocation`) VALUES(5, 'Houston');");
	printf("DEPT_LOCATIONS 튜플 삽입완료\n");

	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('ProductX', 1, 'Bellaire', 5);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('ProductY', 2, 'Sugarland', 5);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('ProductZ', 3, 'Houston', 5);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('Computerization', 10, 'Stafford', 4);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('Reorganization', 20, 'Houston', 1);");
	QUERY("INSERT INTO `COMPANYX`.`PROJECT` (`Pname`, `Pnumber`, `Plocation`, `Dnum`) VALUES('Newbenefits', 30, 'Stafford', 4);");
	printf("PROJECT 튜플 삽입완료\n");

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
	QUERY("INSERT INTO `COMPANYX`.`WORKS_ON` (`Essn`, `Pno`, `Hours`) VALUES('888665555', 20, 10);"); // Hours에 10을 넣어서 진행해주세요 from 조교.
	printf("WORKS_ON 튜플 삽입완료\n");

	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('333445555', 'Alice', 'F', '1986-04-05', 'Daughter');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('333445555', 'Theodore', 'M', '1983-10-25', 'Son');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('333445555', 'Joy', 'F', '1958-05-03', 'Spouse');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('987654321', 'Abner', 'M', '1942-02-28', 'Spouse');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('123456789', 'Michael', 'M', '1988-01-04', 'Son');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('123456789', 'Alice', 'F', '1988-12-30', 'Daughter');");
	QUERY("INSERT INTO `COMPANYX`.`DEPENDENT` (`Essn`, `Dependent_name`, `Sex`, `Bdate`, `Relationship`) VALUES('123456789', 'Elizabeth', 'F', '1967-05-05', 'Spouse');");
	printf("DEPENDENT 튜플 삽입완료\n\n");

	printf("============================\n");
	printf("쿼리문 수행\n");
#pragma endregion

	return true;
}
bool CSQuery::Print() {
	// 과제에서 요구하는 쿼리와 강의노트에 있는 쿼리문들을 실행하고 결과를 보여주는 함수
	#pragma region 질의1
	// 1. SELECT * FROM <Relation name>;
	printf("###첫번째 쿼리###\n");
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
	#pragma region 질의2
	// 2. 강의노트 Chapter 4의 page 26 & 27에 있는 쿼리0, 1, 2
	// 2-1 쿼리0
	printf("###두번째 쿼리###\n");
	printf("%s\n", "SELECT Bdate, Address FROM EMPLOYEE WHERE Fname='John' AND Minit='B' AND Lname='Smith';");
	QUERY("SELECT Bdate, Address FROM EMPLOYEE WHERE Fname='John' AND Minit='B' AND Lname='Smith';");
	sql_result = mysql_store_result(m_pConnection); // 저장

	printf("%15s %35s\n", "Bdate", "Address");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s %35s\n", sql_row[0], sql_row[1]);
	}

	// 2-2 쿼리1
	printf("%s\n", "SELECT Fname, Lname, Address FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research' AND Dnumber=Dno;");
	QUERY("SELECT Fname, Lname, Address FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research' AND Dnumber=Dno;");
	sql_result = mysql_store_result(m_pConnection); // 저장

	printf("%10s %10s %35s\n", "Fname", "Lname", "Address");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%10s %10s %35s\n", sql_row[0], sql_row[1], sql_row[2]);
	}

	// 2-3 쿼리2
	printf("%s\n", "SELECT Pnumber, Dnum, Lname, Address, Bdate FROM PROJECT, DEPARTMENT, EMPLOYEE WHERE Dnum=Dnumber AND Mgr_ssn=Ssn AND Plocation='Stafford';");
	QUERY("SELECT Pnumber, Dnum, Lname, Address, Bdate FROM PROJECT, DEPARTMENT, EMPLOYEE WHERE Dnum=Dnumber AND Mgr_ssn=Ssn AND Plocation='Stafford';");
	sql_result = mysql_store_result(m_pConnection); // 저장

	printf("%10s %5s %10s %35s %15s\n", "Pnumber", "Dnum", "Lname", "Address", "Bdate");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%10s %5s %10s %35s %15s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
	}
	printf("\n");
#pragma endregion
	#pragma region 질의3
	// 3. 강의노트 Chapter 4의 page 35(33쪽)에 있는 쿼리4
	printf("###세번째 쿼리###\n");
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
	sql_result = mysql_store_result(m_pConnection); // 저장
	printf("%6s\n", "Pnumber");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%6s\n", sql_row[0]);
	}
	printf("\n");
#pragma endregion
	#pragma region 질의4
	// 4. 강의노트 Chapter 5의 page 20에 있는 쿼리20, 21, 22와 page 22에 있는 쿼리28 - from 절에 "Join"을 사용하지 않음
	// 4-1 쿼리20
	printf("###네번째 쿼리###(Join 사용X)\n");
	printf("%s\n", "SELECT SUM(Salary), MAX(Salary), MIN(Salary), AVG(Salary) FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research';");
	QUERY("SELECT SUM(Salary), MAX(Salary), MIN(Salary), AVG(Salary) FROM EMPLOYEE, DEPARTMENT WHERE Dname='Research';");
	sql_result = mysql_store_result(m_pConnection); // 저장
	printf("%15s %15s %15s %15s\n", "SUM(Salary)", "MAX(Salary)", "MIN(Salary)", "AVG(Salary)");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s %15s %15s %15s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
	}

	// 4-2 쿼리21
	printf("%s\n", "SELECT COUNT(*) FROM EMPLOYEE;");
	QUERY("SELECT COUNT(*) FROM EMPLOYEE;");
	sql_result = mysql_store_result(m_pConnection); // 저장
	printf("%15s\n", "COUNT(*)");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s\n", sql_row[0]);
	}

	// 4-3 쿼리22
	printf("%s\n", "SELECT COUNT(*) FROM EMPLOYEE, DEPARTMENT WHERE DNO=DNUMBER AND DNAME='Research';");
	QUERY("SELECT COUNT(*) FROM EMPLOYEE, DEPARTMENT WHERE DNO=DNUMBER AND DNAME='Research';");
	sql_result = mysql_store_result(m_pConnection); // 저장
	printf("%15s\n", "COUNT(*)");
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		printf("%15s\n", sql_row[0]);
	}

	// 4-4 쿼리28
	printf("%s\n", QUOTE(
		SELECT Dnumber, COUNT(*) FROM DEPARTMENT, EMPLOYEE WHERE Dnumber = Dno AND Salary > 40000 AND
		(SELECT Dno FROM EMPLOYEE GROUP BY Dno HAVING COUNT(*) > 5);
	));
	QUERY(QUOTE(
		SELECT Dnumber, COUNT(*) FROM DEPARTMENT, EMPLOYEE WHERE Dnumber = Dno AND Salary > 40000 AND
		(SELECT Dno FROM EMPLOYEE GROUP BY Dno HAVING COUNT(*) > 5);
	));
	sql_result = mysql_store_result(m_pConnection); // 저장
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
	// 이렇게 짜면 정적인 출력문이 되어 나중에 Column 추가, 삭제되면 일일히 수정해줘야함. 매우 비효율적이라서 db로부터 column 갯수, 도메인 크기를 받아서 자동화 시킬 필요가 있음.
	// for Select * from Table;
	printf("%s\n", _string);
	QUERY(_string); // 에러시 return false;
	sql_result = mysql_store_result(m_pConnection); // 저장

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

	// Columns 목록 출력
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
	// DB 그대로 남기려면 이 아래 문장을 주석표시할 것.
	mysql_query(m_pConnection, "DROP DATABASE IF EXISTS COMPANYX");

	// 연결 종료
	mysql_close(m_pConnection);
}

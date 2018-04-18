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
	// 초기화, 연결 수행
	mysql_init(m_pConnection);
	
	m_pConnection = mysql_real_connect(&conn, DB_HOST, 
		DB_USER, DB_PASS, NULL, PORT, (char *)NULL, 0);

	printf("MySQL client version: %s\n============================\n", mysql_get_client_info());

	// 연결 확인
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
	*/
	
	// DB가 이미 있으면 삭제하고 새로 시작한다.
	QUERY("DROP DATABASE IF EXISTS COMPANYX;");

	// Database 생성 Name: COMPANYX
	QUERY("CREATE DATABASE COMPANYX DEFAULT CHARACTER SET utf8 ;");
	printf("데이터베이스 생성완료\n");

	// Database 활성화
	QUERY("USE COMPANYX;");
	printf("데이터베이스 활성화됨\n");

	// DEPARTMENT 테이블 생성
	QUERY(QUOTE(CREATE TABLE DEPARTMENT (
		`Dname` VARCHAR(15) NOT NULL,
		`Dnumber` INT NOT NULL,
		`Mgr_ssn` CHAR(9) NOT NULL,
		`Mgr_start_date` DATE NULL,
		PRIMARY KEY(`Dnumber`),
			UNIQUE INDEX `Dname_UNIQUE` (`Dname` ASC),
			INDEX `fk_Mgr_ssn_idx` (`Mgr_ssn` ASC)) ENGINE = InnoDB;
		));
	printf("DEPARTMENT 테이블 생성완료\n");
	

	
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
	// DB 그대로 남기려면 이 아래 문장을 주석표시할 것.
	mysql_query(m_pConnection, "DROP DATABASE IF EXISTS COMPANYX");
	
	// 연결 종료
	mysql_close(m_pConnection);
}

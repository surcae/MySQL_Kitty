// mysql201320897.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "CMainApp.h"

int main()
{
	CMainApp* pMainApp = new CMainApp();
	if (pMainApp->Simulator()) {
		printf("<Program Success>\n");
		system("pause");
		return 0; //exit
	}

	printf("!Querying Failed!\n");
	getchar();
    return 0;
}


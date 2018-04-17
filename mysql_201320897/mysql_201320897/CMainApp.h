#pragma once
#include "CSQuery.h"

class CMainApp
{
private:
	CSQuery* MainSQL;
public:
	bool connect();
public:
	CMainApp();
	~CMainApp();
};


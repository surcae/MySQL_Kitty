#pragma once
#include "CSQuery.h"

class CMainApp
{
private:
	CSQuery* MainSQL;
public:
	bool Simulator();
public:
	CMainApp();
	~CMainApp();
};


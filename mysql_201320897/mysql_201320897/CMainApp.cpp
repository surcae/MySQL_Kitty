#include "stdafx.h"
#include "CMainApp.h"

bool CMainApp::connect()
{
	if (this->MainSQL->connect())
		return true;

	return true;
}

CMainApp::CMainApp()
{
	MainSQL = new CSQuery();
}


CMainApp::~CMainApp()
{
}

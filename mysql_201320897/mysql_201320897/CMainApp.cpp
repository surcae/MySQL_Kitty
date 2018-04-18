#include "stdafx.h"
#include "CMainApp.h"

bool CMainApp::Simulator()
{
	// Check Connection
	if (!MainSQL->connect()) 
		return false;

	// Do Progress
	if (!MainSQL->Progress())
		return false;

	// Print
	MainSQL->Print();
	return true;
}

CMainApp::CMainApp()
{
	MainSQL = new CSQuery();
}


CMainApp::~CMainApp()
{
}

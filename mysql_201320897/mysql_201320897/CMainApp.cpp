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
	if (!MainSQL->Print())
		return false;
	return true;
}

CMainApp::CMainApp()
{
	MainSQL = new CSQuery();
}


CMainApp::~CMainApp()
{
}

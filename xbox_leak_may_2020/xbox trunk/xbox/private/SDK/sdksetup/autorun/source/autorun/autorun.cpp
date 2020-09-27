//-----------------------------------------------------------------------------
// File: autorun.cpp
//
// Desc: Defines the class behaviors for the application
//
// Hist: 10.27.00 Emily Wang Created For XBOX
//       
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "autorun.h"
#include "autorunDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//-----------------------------------------------------------------------------
// CAutorunApp 
//-----------------------------------------------------------------------------
HANDLE g_hmutexPrevInst = NULL; // For checking prevous instance.

BEGIN_MESSAGE_MAP(CAutorunApp, CWinApp)
	//{{AFX_MSG_MAP(CAutorunApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
// CAutorunApp construction
//-----------------------------------------------------------------------------
CAutorunApp::CAutorunApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	ZeroMemory(&m_sInfo, sizeof(m_sInfo));
}

CAutorunApp::~CAutorunApp()
{
	delete m_sInfo.pszTitle;
	for (INT i = 0; i < m_sInfo.cnLinks; i++)
	{
		delete m_sInfo.psLinks[i].pszName;
		delete m_sInfo.psLinks[i].pszCommand;
	}
	delete m_sInfo.psLinks;
}


//-----------------------------------------------------------------------------
// The one and only CAutorunApp object
//-----------------------------------------------------------------------------
CAutorunApp theApp;



//-----------------------------------------------------------------------------
// Name: InitIstance()
// Desc: Entry point to the program. CAutorunApp initialization.
//-----------------------------------------------------------------------------
BOOL CAutorunApp::InitInstance()
{
	INT    nResponse, cnTries;
	INT		nPathLength, nStringLength;
	TCHAR szBuffer[512];
	TCHAR szSection[128];
	CAutorunDlg dlg;

TryAgain:

    g_hmutexPrevInst = CreateMutex(NULL, TRUE, TEXT("Mutex_XBoxAutoRun"));
    if (!g_hmutexPrevInst)
    {
        return FALSE;
    }
    else if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // Is there a previous instance existing?
        cnTries = 0;
        while(cnTries--)
        {
            // Wait for a while
            Sleep(3000);
            ReleaseMutex(g_hmutexPrevInst);
            goto TryAgain;
        }
        
		return FALSE;
    }

	

//-----------------------------------------------------------------------------
// Name: GetModuleFileName()
// Desc: Get install path
//-----------------------------------------------------------------------------	

	INT		nFileLength;
	TCHAR	szFilePath[MAX_PATH];
	TCHAR	FileBuffer[MAX_PATH];
	LPTSTR	lpFilePart;
	lpFilePart = 0;


	nPathLength = GetModuleFileName(NULL, szFilePath, MAX_PATH);
	nFileLength = GetFullPathName(szFilePath,MAX_PATH, FileBuffer, &lpFilePart);
	
	// Remove the file name.
	*(lpFilePart) = 0;
	
	// Path of CD ROM, image, cursor and icon.
	_tcscpy(m_sInfo.szPath, FileBuffer);
_tcscpy(m_sInfo.szImagePath, FileBuffer);
	_tcscpy(m_sInfo.szCursorPath, FileBuffer);
	_tcscpy(m_sInfo.szIconPath, FileBuffer);
	

	// Path of ini file
	_tcscpy(szFilePath, FileBuffer);
	_tcscat(szFilePath, _T("autorun.ini"));


//-----------------------------------------------------------------------------
// Name: GetUserDefaultLangID()
// Desc: Get user default Language
//-----------------------------------------------------------------------------	
	int nLangID = GetUserDefaultLangID();
		
	if (nLangID == 0)
	{
		_tcscpy(szSection, "Default");
	}
	else
	{
		_itot(nLangID, szSection, 10);
	
		nStringLength = GetPrivateProfileString(szSection, _T("TITLE"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
		if (nStringLength == 0) 
		{
			_tcscpy(szSection, "Default");			
		}
	}



//-----------------------------------------------------------------------------
// Name: GetPrivateProfileString()
// Desc: Get info of dialog.
//-----------------------------------------------------------------------------	
	nStringLength = GetPrivateProfileString(szSection, _T("TITLE"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process title
	m_sInfo.pszTitle = new TCHAR[_tcslen(szBuffer) + 1 ];
	_tcscpy(m_sInfo.pszTitle, szBuffer);
	
	nStringLength = GetPrivateProfileString(szSection, _T("IMAGE"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process image path
	_tcscat(m_sInfo.szImagePath, szBuffer);

	nStringLength = GetPrivateProfileString(szSection, _T("ICON"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process icon path
	_tcscat(m_sInfo.szIconPath, szBuffer);

	nStringLength = GetPrivateProfileString(szSection, _T("CURSOR"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process cursor path
	_tcscat(m_sInfo.szCursorPath, szBuffer);

	nStringLength = GetPrivateProfileString(szSection, _T("NAME"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process text name
	_tcscpy(m_sInfo.szFontName, szBuffer);
	
	nStringLength = GetPrivateProfileString(szSection, _T("SIZE"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process text size
	m_sInfo.nFontSize = atoi(szBuffer);

	nStringLength = GetPrivateProfileString(szSection, _T("COLOR"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process text color 
	m_sInfo.crFont = (COLORREF)_tcstoul(szBuffer, NULL, 16);

	nStringLength = GetPrivateProfileString(szSection, _T("HIGHLIGHTCOLOR"), _T(""), szBuffer, sizeof(szBuffer), szFilePath);
	// process text highlight color
	m_sInfo.crHighLight = (COLORREF)_tcstoul(szBuffer, NULL, 16);

	// Count the links
	for ( int i = 0; ; i++ )
	{
		TCHAR* pszDelimit;
		TCHAR* pszLine;
		TCHAR szKey[128];
		
		_stprintf(szKey, _T("LINK%d"), i);
		nStringLength = GetPrivateProfileString(szSection, szKey, _T(""), szBuffer, sizeof(szBuffer), szFilePath);
		if(nStringLength == 0) break;

		pszLine = szBuffer;

		for(int cDelims = 0; ; cDelims++)
		{
			pszDelimit = _tcsstr(pszLine, TEXT(","));
			if(pszDelimit == NULL) break;
			pszLine = pszDelimit + 1;
		}
		ASSERT(cDelims == 3);
		if(cDelims != 3)
		{
			break;
		}
	}
	m_sInfo.cnLinks = i;

	// Allocat link buffer
	m_sInfo.psLinks = new SAutorunLink[m_sInfo.cnLinks];
	ZeroMemory(m_sInfo.psLinks, sizeof(SAutorunLink) * m_sInfo.cnLinks);
	
	// Get the links
	for ( i = 0; i < m_sInfo.cnLinks; i++ )
	{
		TCHAR* pszDelimit;
		TCHAR* pszLine;
		TCHAR szKey[128];
		
		_stprintf(szKey, _T("LINK%d"), i);
		nStringLength = GetPrivateProfileString(szSection, szKey, _T(""), szBuffer, sizeof(szBuffer), szFilePath);

		// Get link names
		pszLine = szBuffer;
		pszDelimit = _tcsstr(pszLine, TEXT(","));
		*pszDelimit = 0;
		m_sInfo.psLinks[i].pszName = new TCHAR[_tcslen(pszLine) + 1];
		_tcscpy(m_sInfo.psLinks[i].pszName, pszLine);
		pszLine = pszDelimit + 1;

		// Get link positions
		pszDelimit = _tcsstr(pszLine, TEXT(","));
		*pszDelimit = 0;
		m_sInfo.psLinks[i].rcRect.right = m_sInfo.psLinks[i].rcRect.left = atoi(pszLine);
		pszLine = pszDelimit + 1;

		pszDelimit = _tcsstr(pszLine, TEXT(","));
		*pszDelimit = 0;
		m_sInfo.psLinks[i].rcRect.bottom = m_sInfo.psLinks[i].rcRect.top = atoi(pszLine);
		pszLine = pszDelimit + 1;

		// Get link commands
		pszDelimit = _tcsstr(pszLine, TEXT(","));
        if (pszDelimit)
        {
            *pszDelimit = 0;
        }
		m_sInfo.psLinks[i].pszCommand = new TCHAR[_tcslen(pszLine) + 1];
		_tcscpy(m_sInfo.psLinks[i].pszCommand, pszLine);
	}

	if (i != m_sInfo.cnLinks)
	{
		ASSERT(FALSE);
		goto Error;
	}

    m_pMainWnd = &dlg;
	nResponse = dlg.DoModal();
	if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
Error:
	    
    ReleaseMutex(g_hmutexPrevInst);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE; 
};

///////////////////////////////////////////////////////////////////////////////
//      SNIFF.CPP
//
//      Created by :                    Date :
//              RickKr                                  8/30/93
//
//      Description :
//              implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "syscases.h"
#include "bindcase.h"
#include "editcase.h"
#include <process.h>
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

static char _szDllRegSvr[] = "DllRegisterServer";
static char _szDllUnregSvr[] = "DllUnregisterServer";

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CBindSubSuite, CIDESubSuite, "ResEditor controls", "ChrisKoz")

BEGIN_TESTLIST(CBindSubSuite)
	TEST(CContTest, RUN)
	TEST(CAddControlTest, RUN) //	DEPENDENCY(CContTest)
	TEST(CBindBuildTest, RUN) //	    DEPENDENCY(CContTest)
	TEST(CPubsTablesTest, RUN) //	DEPENDENCY(CBindBuildTest)
	TEST(CEditResTest, RUN) //	DEPENDENCY(CBindBuildTest)
	TEST(CRandomTest, RUN) //	DEPENDENCY(CBindBuildTest)
//	if ((GetSystem() & SYSTEM_DBCS) != 0) { // Only if on a DBC system
//		TEST(CSourceTestCases, DONTRUN)
//	}
END_TESTLIST()

void CBindSubSuite::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
// obsolete call (tries to register the inexistent OLE controls)
//	if(RegisterAll()!=ERROR_SUCCESS)
//		throw CTestException("Can't register the components. Unable to continue", CTestException::causeOperationFail);
	CString strDataFile=m_SuiteParams->GetTextValue("DATASOURCES", "DATASRC.DEF");
	GetDataInfo(strDataFile);
		
	m_projOpen=0; //hack - proj->IsOpen() does not work

}

BOOL CBindSubSuite::RegisterAll()
{
	HRESULT (FAR /*STDAPICALLTYPE*/__stdcall * lpDllEntryPoint)(void);

	BOOL bVisualC = FALSE;
	BOOL bUnregister = FALSE;
	LPSTR pszRegEntryPoint = _szDllRegSvr;
	LPSTR pszUnregEntryPoint = _szDllUnregSvr;
	CString szOCXfiles[]={
		"MSRDC32.OCX",
		"DBLIST32.OCX",
		"MSMASK32.OCX",
		"DBGRID32.OCX"
	};

	CString pLib;
	const int nLibs=4;
	int i;

	for(i=0;i<nLibs;i++)
	{
		pLib=m_strCWD+szOCXfiles[i];
	// Load the library.	
	HINSTANCE hLib = LoadLibrary(pLib);

	if (hLib < (HINSTANCE)HINSTANCE_ERROR)
	{
		TCHAR szError[12];
		wsprintf(szError, _T("0x%08lx"), GetLastError());
		AfxMessageBox("Library load failed - "+pLib);
		return ERROR_ERROR;
	}

	// Find the entry point.		
	(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, pszRegEntryPoint);

	if (lpDllEntryPoint == NULL)
	{
		AfxMessageBox("Library Register entry not found - "+pLib);
		FreeLibrary(hLib);
		return ERROR_ERROR;
	}

	// Call the entry point.	
	if (FAILED((*lpDllEntryPoint)()))
	{
		AfxMessageBox("Library Register entry failed - "+pLib);
		FreeLibrary(hLib);
		return ERROR_ERROR;		
	}
//	Info(IDS_CALLSUCCEEDED, pszDllEntryPoint, pszDllName);
	FreeLibrary(hLib);
	} //for
	if(_spawnlp( _P_WAIT, "regedit.exe", "regedit.exe", "/s",
		  m_strCWD + "SQL.REG", NULL )==-1)
	{
		AfxMessageBox("REG file registration failed");
		return ERROR_ERROR;		
	}
	return ERROR_SUCCESS;
}



void CBindSubSuite::CleanUp(void)
{
	int i;
	if (m_prj.IsOpen()) {
		m_prj.Close();
	}
	for(i=0;i<2;i++)
	m_DataInfo[i].ClearAll();
	// remove the project directory
	KillAllFiles(m_strCWD + PROJDIR, TRUE);	
	m_projOpen=0;
	// call the base class
	CIDESubSuite::CleanUp();
}



void CBindSubSuite::GetDataInfo(CString &strDataFile)
{
	CStdioFile fileData(m_strCWD + strDataFile,
							CFile::modeRead | CFile::typeText);
  	const int len=100;
	char buf[len];char *ptoken;
	int i=0;
	CString str("");

//	for(i=0;i<2;i++) --right now, only one server
	if(fileData.ReadString(buf, len))
	{
		m_DataInfo[i].m_strConnect=CString(buf);
		ptoken=strtok(buf,";"); //driver name
		ptoken=strtok(NULL,"="); //"SERVER"
			ASSERT(strcmp(ptoken,"SERVER")==0);
		m_DataInfo[i].m_strServer=strtok(NULL,";");
		ptoken=strtok(NULL,"="); //"DATABASE"
			ASSERT(strcmp(ptoken,"DATABASE")==0);
		m_DataInfo[i].m_strDataB=strtok(NULL,";");
		ptoken=strtok(NULL,"="); //"UID"
			ASSERT(strcmp(ptoken,"UID")==0);
		m_DataInfo[i].m_strUser=strtok(NULL,";");
		ptoken=strtok(NULL,"="); //"PWD"
			ASSERT(strcmp(ptoken,"PWD")==0);
		m_DataInfo[i].m_strPassWord=strtok(NULL,"; ");

		TrimString(&m_DataInfo[i].m_strConnect);
		TrimString(&m_DataInfo[i].m_strServer);
		TrimString(&m_DataInfo[i].m_strDataB);
		TrimString(&m_DataInfo[i].m_strUser);
		TrimString(&m_DataInfo[i].m_strPassWord);
//TODO what to do with registry sources?		m_DataInfo[i].m_strSource="Test"+m_DataInfo[i].m_strServer;
//		m_DataInfo[i].GetDataInfo(fileData); -this is developped below

    // Now get rest of information, if any.
    CString strDelimiter("***");
    BOOL bDone = FALSE;

    while (!bDone && fileData.ReadString(buf, len))
    {
        str = buf;
        TrimString(&str);

        if (!str.IsEmpty() && str.GetAt(0) != ';')
        {
            if (str == strDelimiter)
                bDone = TRUE;
			else 
			{
				CObjectInfo* pNew = new CObjectInfo();
				if(str.Find("TABLE") == 0)
				{	pNew->m_Type=CObjectInfo::TABLE;}
				if(str.Find("VIEW") == 0)
				{	pNew->m_Type=CObjectInfo::VIEW;}
				if(str.Find("QUERY") == 0)
				{	pNew->m_Type=CObjectInfo::PROC;}
				strtok(buf, " "); // TABLE or VIEW or PROC or any valid object
				str=strtok(NULL, " ");
				pNew->m_strName	= str;
				TrimString(&pNew->m_strName);

				str=strtok(NULL, " ");
				pNew->m_strColumn	= str;
				TrimString(&pNew->m_strColumn);

				m_DataInfo[i].m_listMembers.AddTail(pNew);
			}
        }
    } //while
	}//if
} //CBindSubSuite::GetDataInfo

CObjectInfo::~CObjectInfo()
{
	ClearAll();
}

CDataInfo::~CDataInfo()
{
	ClearAll();
}

void CDataInfo::ClearAll()
{ 
    m_strDataB.Empty(); 
    
	POSITION pos = m_listMembers.GetHeadPosition();
	while(pos != NULL)
	{
		delete m_listMembers.GetAt(pos);
		m_listMembers.GetNext(pos);
	}

	m_listMembers.RemoveAll();
}

void CObjectInfo::ClearAll()
{ 
    m_strName.Empty(); 
	m_strColumn.Empty();
}

BOOL SetFocusToResSym(LPCSTR SymName)
{
	//////////////////////TODO do it better below
	if(!UIWB.IsCommandEnabled(IDM_EDIT_SYMBOLS))
	{
		LOG->RecordFailure("ResEdit not enabled");
		return FALSE;
	}
	//MST.DoKeys("%(Vy)",FALSE,5000); //View.Symbols (wait for idle time
	UIWB.DoCommand(IDM_EDIT_SYMBOLS, DC_MNEMONIC);
	WaitForInputIdle(g_hTargetProc, 5000);
	//	if(MST.WListItemExists(cntlrID))
	MST.WListItemClk(GetLabel(0x6814),SymName); 
	MST.WButtonClick(GetLabel(0x681d)); // View use of the dialog

	WaitForInputIdle(g_hTargetProc, 5000);
	return TRUE;
}


// EOF - sniff.cpp

//////////////////////////////////////////////////////////////////////////////
//	DATACASE.CPP
//
//	Created by :			Date :
//		ChrisKoz					2/01/96
//
//	Description :
//		Implementation of the CSrcDataCases class
//

#include "stdafx.h"
#include "syscase.h"
#include "datacase.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;



///////////////////////////////////////////////////////////////////////////////
//	CTablesTest class
IMPLEMENT_TEST(CTablesTest, CTest, "Table/View/Dgrm objects", -1, CDataSubSuite)

void CTablesTest::Run(void)
{
	CDataSubSuite *pSuite=GetSubSuite();
	pSuite->OpenDataProject();
	if(pSuite->ForcePassword(&pSuite->m_DataInfo)!=ERROR_SUCCESS)
	{
			GetLog()->RecordFailure("Coudn't find the DataSource - %s",
				GetSubSuite()->m_DataInfo.m_strDSN);
			throw CTestException("Cannot reopen the Datasource connection; unable to continue.", 
				CTestException::causeOperationFail);
	}
	// now we are at the DataSource Node let's examine the tables
	MST.DoKeys("{Right}"); //expand the tables of active datasource
	if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
		GetLog()->RecordFailure("Coudn't deploy the tables");
	
	TestTableObjects();
	GetSubSuite()->m_prj.Close(TRUE); GetSubSuite()->m_projOpen=0;
} //CTablesTest::Run


void CTablesTest::TestTableObjects()
{
	CDataSubSuite *pSuite=GetSubSuite();
	CDataInfo *info=&pSuite->m_DataInfo;


    GetLog()->RecordInfo("Checking object of database: %s", info->GetName());

	UIWorkspaceWindow wnd;

	for(POSITION pos = info->m_listMembers.GetHeadPosition();
		pos!=NULL;info->m_listMembers.GetNext(pos))
	{
		CObjectInfo* pMember=info->m_listMembers.GetAt(pos);// has members.
		if(pMember->m_Type!=CObjectInfo::TABLE &&
				  pMember->m_Type!=CObjectInfo::VIEW &&
				  pMember->m_Type!=CObjectInfo::DIAGRAM)
			continue; //test only tables/diagrams
		char name[256];
		switch(pMember->m_Type)
		{
			case CObjectInfo::TABLE:
				pSuite->ExpandTableNode("");
				break;
			case CObjectInfo::VIEW:
				pSuite->ExpandViewNode("");
				break;
			case CObjectInfo::DIAGRAM:
				pSuite->ExpandDiagramsNode("");
				break;
			default:
				continue; // object type not supported
		} //switch
		strncpy(name,pMember->GetName(),255);
		MST.DoKeys(name);
		if(pSuite->CheckObjectProp(pMember)!=ERROR_ERROR)
		{
			char ob_typename[128];
			int ret;
			GetLog()->RecordSuccess("Found the %s name: '%s'",pMember->GetTypeName(),name);
			if(pMember->m_Type!=CObjectInfo::DIAGRAM)
			{	//tables & views open as queries
				strncpy(ob_typename,GetLocString(IDSS_QUERY),128);
				ret=OpenGridView(name,ob_typename,pMember->m_strParams); //open table or view
			}
			else if(pMember->m_Type==CObjectInfo::DIAGRAM)
			{
				strncpy(ob_typename,pMember->GetTypeName(),128);
				ret=OpenDiagView(name,ob_typename,pMember->m_strParams); //open diagram
			}
			LOG->RecordCompare(ret==ERROR_SUCCESS,
				"Openning the %s name: '%s'",ob_typename,name);
			wnd.Activate(); //returns to the project workspace
		}
//		pSuite->CollapseObjects((pMember->m_Type!=CObjectInfo::TRIGGER)?0:1); //not here
		pSuite->CollapseObjects(0);
	}	//for
	pSuite->CollapseObjects(1);	//final collapse of all objects
} //TestTableObjects


/////////////////////////////////////////////////////////////////////
// Opens the source window for the object type  TABLE or VIEW
// and checks the grup by query operations
// RETURNS: ERROR_SUCCESS if OK
////////////////////////////////////////////////////////////////////
int CTablesTest::OpenGridView(LPCSTR pszName,CString strType,CString &Params)
{
	COSource srcFile;
	int ret = ERROR_SUCCESS;
	CString Name=pszName;
	Name.MakeUpper();

	MST.DoKeys("{Enter}"); //for open, provided the lack of better handle
	for(int i=0;i<5;i++)
	{ //must do it five times because idling is done in intervals
		WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
		Sleep(1000);							// Wait for message box to arrive.
	}
	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())			// If we can attach, then an error must've occurred.
	{
		CString str;
		MST.WGetText(HWND (wndMsgBox), str);
		LOG->RecordInfo("'%s' message box detected", str);
		if(MST.WStaticExists("Unable to open"))
			LOG->RecordInfo("Unable to open the object");
		wndMsgBox.ButtonClick();
		ret = ERROR_ERROR;
	}
	else									// A source window must've become active.
	{
		CString title=CString(pszName) +" : "+strType;	

		srcFile.AttachActiveEditor();
		HWND hwnd=UIWB.GetActiveEditor(); //don't know how to take the handler from COSource

		CString str;
		MST.WGetText(hwnd, str); //should be MDI window
		LOG->RecordCompare((str.Find(title) !=-1),
			"Find the source window for object: %s",pszName);
	}
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for opening the query
	Sleep(1000); //safety (idle is usu hit when query is running
	WaitForInputIdle(g_hTargetProc, 5000); //safety	
	UIWB.DoCommand(IDM_QUERY_GROUPBYMENU, DC_MNEMONIC); //&Query.&Group By
	WaitForInputIdle(g_hTargetProc, 5000); //safety
	Sleep(500);
	UIWB.DoCommand(IDM_QUERY_VERIFYMENU, DC_MNEMONIC); // &Query.&Verify SQL Syntax
	Sleep(500);

	if(wndMsgBox.AttachActive())		
	{	// If we can attach, then a login error must've occurred.
		BOOL bResult=MST.WStaticExists(GetLocString(IDSS_SQLSYNTAX_OK));
		LOG->RecordCompare(bResult,"Verify GROUP BY query option");	
		wndMsgBox.ButtonClick(); //Get rid of the message
	}
	WaitForInputIdle(g_hTargetProc, 5000);
	UIWB.DoCommand(IDM_TOOLS_RUNMENU, DC_MENU); //&Tools.&Run
	WaitForInputIdle(g_hTargetProc, 5000);
//check message box & the result
	Sleep(5000); //let WinNT recover the memory
	srcFile.Close();
	Sleep(2000); //let WinNT recover the memory
	return ret;
} //CTablesTest::OpenGridView

/////////////////////////////////////////////////////////////////////
// Opens the source window for the object type  DIAGRAM
// and checks 
// Params - list of the tables to check
// RETURNS: ERROR_SUCCESS if OK
////////////////////////////////////////////////////////////////////
int LocalFindChildWindow(HWND WHandle,CString &ChildTitle);
int CTablesTest::OpenDiagView(LPCSTR pszName,CString strType,CString &Params)
{
	COSource srcFile;
	int ret = ERROR_SUCCESS;
	CString Name=pszName;
	Name.MakeUpper();

	MST.DoKeys("{Enter}"); //for open, provided the lack of better handle
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(2000);							// Wait for message box to arrive.

	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())			// If we can attach, then an error must've occurred.
	{
		CString str;
		MST.WGetText(HWND (wndMsgBox), str);
		LOG->RecordInfo("'%s' message box detected", str);
		if(MST.WStaticExists("Unable to open"))
			LOG->RecordInfo("Unable to open the object");
		wndMsgBox.ButtonClick();
		ret=ERROR_ERROR;
	}
	else									// A source window must've become active.
	{
		CString title=CString(pszName) +" : "+strType;	

		srcFile.AttachActiveEditor();
		HWND hwnd=UIWB.GetActiveEditor(); //don't know how to take the handler from COSource

		CString str;
		MST.WGetText(hwnd, str); //should be MDI window
		LOG->RecordCompare((str.Find(title) !=-1),
			"Find the source window for object: %s",pszName);
		int imod=0;
#if 0 //windows of type GridWndClass inside the diagram do not have titles anymore
//enable this tables inside diagram check when find better way to do that
		char szparam[512];
		strncpy(szparam,Params,512);
		str=strtok(szparam," ");
		while(str.IsEmpty()==0)
		{
			LOG->RecordInfo("...Searching for window '%s' in the diagram...",str);
			if(FindChildWindow(hwnd, str, "GridWnd")==NULL)
			{
				LOG->RecordFailure("Didn't find window '%s' in the diagram",str);
				break;
			}
/*
			BOOL temptest=(MST.WFndWndC(str.GetBufferSetLength(255),"GridWndClass",FW_ACTIVE)!=0);
			str.ReleaseBuffer(-1);
			LOG->RecordCompare(temptest==TRUE,"Find '%s' child",str);
*/
			str=strtok(NULL," ");
		}
#endif
		Sleep(10000); //let WinNT recover the memory
		srcFile.Close();
		Sleep(5000); //let WinNT recover the memory
	}
	return ret;
} //CTablesTest::OpenDiagView


int LocalFindChildWindow(HWND WHandle,CString &ChildTitle)
{
	char szFound[80];
	HWND childhwnd;
	for(int i=0;i<5;i++)
	{	int nbsiblings=0;
		childhwnd=::GetWindow(WHandle,GW_CHILD);		
		childhwnd=::GetWindow(childhwnd,GW_CHILD);		
		childhwnd=::GetWindow(childhwnd,GW_CHILD);		
		childhwnd=::GetWindow(childhwnd,GW_CHILD);		
		while(childhwnd!=NULL)
		{
			::GetWindowText(childhwnd, szFound, 80);
			TRACE("--%d:child no:%d--:Found the Diagram child: %s\n",i,++nbsiblings,szFound);
			if(szFound[0]!=0 && ChildTitle.Find(szFound)==0)	
			{
				return ERROR_SUCCESS;
			}
			childhwnd=::GetWindow(childhwnd,GW_HWNDNEXT);
		}
		Sleep(1000);
	}
	return ERROR_ERROR;
}  //FindChildWindow

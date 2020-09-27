///////////////////////////////////////////////////////////////////////////////
//	SQLSDI.CPP
//
//	Created by :			Updated :
//		CHRISKOZ			5/15/97
//
//	Description :
//		Implementation of the CSQLSDICases class
//			Test cases for the SQL debugger
//

#include "stdafx.h"
#include "sniff.h"
#include "sdi.h"

#include <objbase.h>
#include <initguid.h>
#include "evnthdlr.h" //MSDEV object model

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


IMPLEMENT_TEST(CAddDSNTest,CTest, NULL, -1, CSniffDriver)
IMPLEMENT_TEST(CBuildTest, CTest, NULL, -1, CSniffDriver)
IMPLEMENT_TEST(CInprocTest,CTest, NULL, -1, CSniffDriver)
IMPLEMENT_TEST(CAppTest,   CTest, NULL, -1, CSniffDriver)

////////////////////////////////////////////////
// sets the project as an active one
//	  projectNo - 1-based number of the project to be activated
//
//
void SetActiveProject(int projectNo)
{
	ASSERT(projectNo>0);
	UIWB.DoCommand(IDM_PROJ_ACTIVEPROJMENU, DC_MNEMONIC);
	Sleep(500);
	for(int i=0;i<projectNo;i++)
	{
		MST.DoKeys("{DOWN}");
		Sleep(500);  
	}
	MST.DoKeys("{ENTER}");	//select
} // SetActiveProject

///////////////////////////////////////////////////////////////////////////////////////
// CAddDSNTest
///////////////////////////////////////////////////////////////////////////////////////
void CAddDSNTest::Run(void)
{
	
	// !!! Replace with string resources for localization
	CSniffDriver* pSubSuite = GetSubSuite();
	pSubSuite->OpenProject();

	BOOL bResult=pSubSuite->m_prj.NewProject(DATABASE_PROJECT,
		pSubSuite->m_AppName + "1",GetCWD()+pSubSuite->m_AppName,
		GetUserTargetPlatforms(),TRUE);
	for(int ind=0;ind<pSubSuite->m_Servers;ind++)
	{
		bResult=pSubSuite->m_prj.AddSQLDataSource(
			pSubSuite->m_svrtable[ind].m_DSN,pSubSuite->m_svrtable[ind].m_User,
			pSubSuite->m_svrtable[ind].m_Psw,pSubSuite->m_svrtable[ind].m_DataBase);
		if(!GetLog()->RecordCompare(bResult == ERROR_SUCCESS, 
					"Opening the SQL Server '%s' database %s.",
					pSubSuite->m_svrtable[ind].m_Server,
					pSubSuite->m_svrtable[ind].m_DSN))
			throw CTestException("Cannot establish the connection; unable to continue.\n", 
					CTestException::causeOperationFail);
		if(pSubSuite->SelectDatabase(
			pSubSuite->m_svrtable[ind].m_Server,
			pSubSuite->m_svrtable[ind].m_DataBase)
			==ERROR_SUCCESS)
		{
			LPCSTR szScript=pSubSuite->m_svrtable[ind].m_SpScripts;
			for(szScript=strtok((LPSTR)szScript,"\n\t ");szScript!=NULL;szScript=strtok(NULL,"\n\t "))
			{
				UIWB.OpenFile(m_strCWD + szScript);
				UIWB.DoCommand(IDM_TOOLS_RUNMENU, DC_MNEMONIC); //&Tools.&Run
				UIMessageBox wndMsgBox;
				if (wndMsgBox.AttachActive())// If we can attach, an error must've occurred.
				{
					char title[128];
					::GetWindowText(wndMsgBox, title, 128);
					if(MST.WFndWndWait("Server Error(s)"/*GetLocString()*/, FW_PART, 0))
					{
						LOG->Comment("'Server Error(s)' message box detected while executing '%s' script", szScript);
					}
					else
					{
						LOG->Comment("Unknown (%s) message box detected while executing '%s' script",title, szScript);
					}
					throw CTestException("Cannot execute setup SQL script. Unable to continue\n",
						CTestException::causeOperationFail);
					//wndMsgBox.ButtonClick(); //does not work for server error(s)
					MST.DoKeys("{ENTER}");
				}
			} //for m_SpScripts

		} //SelectDatabase
	} //for m_Servers
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);

	//setting the breakpoints on the stored procedures
	//make the first project the active one
	SetActiveProject(1); //active the native C++ project

	WaitForInputIdle(g_hTargetProc, 5000); // wait while project switched
	BPInfo *pbpTable=pSubSuite->BPTable;
	for(int i=0;i<pSubSuite->m_BPs;i++)
	{
		struct CSniffDriver::serverinfo *srvinfo=pSubSuite->m_svrtable+pbpTable[i].ServerInd;
		if(SetBP(&pbpTable[i],srvinfo->m_Server,srvinfo->m_DataBase)!=ERROR_SUCCESS)
			LOG->RecordInfo("Failed to set the BP in %s, on: %s",
			pbpTable[i].Proc,pbpTable[i].Stmt);
		if(pbpTable[i].HitCount==0)
		{
			LOG->RecordInfo("Disabling the breakpoint: %s", pbpTable[i].Stmt);
			UIBreakpoints UIBP;
			if(UIBP.Activate()==FALSE)
			{
				LOG->RecordFailure("Couldn't activate Breakpoints dialog");
			}
			else
			{
				if(UIBP.Disable(i)==FALSE)
					LOG->RecordFailure("Couldn't disable the BP");
				UIBP.Close();
			}
		}
	}
	pSubSuite->m_prj.Close(TRUE);
	pSubSuite->m_projOpen=0;
}

//Sets the breakpoint on the SP
// Stmt - Statement to set the BR on (usually the first word)
// Server - server name to take it from
// DataBase - database name to take it from
// ProcName - name of the procedure
// Table - name of the table if this is a trigger (NULL if the procedure)
int CAddDSNTest::SetBP(BPInfo *pBPInfo,CString &Server,CString &DataBase)
{
	CString ProcName=pBPInfo->Proc;
	CString Stmt=pBPInfo->Stmt;
    GetLog()->RecordInfo("Setting breakpoint on: %s in procedure %s",Stmt, ProcName);
	char strname[256];
	strncpy(strname,ProcName,255);
	if(ProcName.Find(";")>0)
	{	// trigger name - must go through tables
		GetSubSuite()->ExpandTable(Server,DataBase,strtok(strname,";"));
		strcpy(strname,strtok(NULL,";"));
		MST.DoKeys(strname);
	}
	else
	{
		GetSubSuite()->ExpandProcedureNode(Server,DataBase);
		MST.DoKeys(strname);
	}
	int ret=CheckNodeProperties(strname);
	if(ret!=ERROR_ERROR)
	{
		GetLog()->RecordSuccess("Found the procedure: %s",ProcName);
		ret=SetBPOnStmt(/*ProcName*/strname,Stmt);
		LOG->RecordCompare(ret == ERROR_SUCCESS,
			"Setting BP on procedure: %s",ProcName);
		UIWorkspaceWindow wnd; //same as DoKeys("%VK") returns to the workspace

	}
	GetSubSuite()->CollapseObjects();
	return ret;
}


int CAddDSNTest::CheckNodeProperties(LPCSTR Name)
{
	MST.DoKeys("%({Enter})"); //bring up the properties
	BOOL bFound = FALSE;
	int ret=ERROR_SUCCESS;
	int nSec = 10;
	while (!bFound && ret == ERROR_SUCCESS)
	{
		// Attempt to open the property page for the Data Source
		
		
		CString str;
		MST.WGetText(NULL, str); //should be "<Object type> Properties"
		if(str.Find(GetLocString(IDSS_PROP_TITLE))>0)
		{
			if(nSec>2)nSec=2; //properties window is up, but XTRA safety required!
		}
		//TODO add check for the object type
		if (MST.WStaticExists(Name))
			//TODO add when applicable for each object  && MST.WStaticExists(pMember->GetType())) 
		{
			bFound = TRUE;
		}
		else
		{
			if (--nSec > 0) // If we still have some waiting time...
			{
				GetLog()->RecordInfo("Waiting up to %d more second(s) for properties of %s...", 
					nSec,Name);
				Sleep(1000);
			}
			else                // We've timed out.
			{       
				GetLog()->RecordFailure(
					"Timeout waiting for properties of: %s",Name);
				ret = ERROR_ERROR;
			}
		}
	} //while
	MST.DoKeys("{Escape}"); // close the property page
	return ret;
}
/////////////////////////////////////////////////////////////////////
//	Sets the bpreakpoint in the SP or trigger highlithed in the workspace
//	on the specified statement
// Opens the source window for the object type != TABLE or VIEW
// checks the source window could be opened, finds the Stmt and put BP
// Params:
//	Name - name of the SP or trigger (without table name)
//	Stmt - first few characters uniquely identifying Stmt within the SP/trigger scope
//			(without the leading blanks)
// RETURNS: ERROR_SUCCESS if OK
////////////////////////////////////////////////////////////////////
int CAddDSNTest::SetBPOnStmt(CString Name,CString Stmt)
{
	int ret = ERROR_SUCCESS;
	Name.MakeUpper();
	Stmt.MakeUpper();

//	MST.DoKeys("+({F10})");		// Bring up the popup context menu.
	Sleep(1000); //xtra safety
//	CString strOpenMenuItem = GetLocString(IDSS_FO_TITLE);

//	if (!MST.WMenuExists(strOpenMenuItem)) //This one GPFs due to wrong calling convention
//	int count;
//	if((count=MST.WMenuCount())==0) 
//		ret = ERROR_NO_DEFINITION;	//this does not work, MenuCount always returns 0

	MST.DoKeys("{Enter}");
//	MST.DoKeys("O"); //for open, provided the lack of better handle
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(1000);							// Wait for message box to arrive.

//	UIMessageBox wndMsgBox;
//	if (wndMsgBox.AttachActive())			// If we can attach, then an error must've occurred.
//	{
//		ret = ERROR_NO_DEFINITION;
//		wndMsgBox.ButtonClick();
//	}
//	else									// A source window must've become active.
	{
		COSource SrcFile;
//		CString title=DataInfo->m_strServer+"!"+DataInfo->m_strName;	
// TODO: change to better title	when available
		CString title=Name;
		title.MakeUpper();

//		SrcFile.AttachActiveEditor();
		if(MST.WFndWnd(title, FW_PART | FW_FOCUS)==NULL )
		{
			LOG->RecordFailure("Didn't find the title source window: %s",title);
			ret=ERROR_ERROR;
		}
		ret= SrcFile.Find(Stmt);
		if(ret==TRUE)
		{
			MST.DoKeys("{F9}");
			LOG->RecordSuccess("BP on stmnmt: %s in proc: %s",Stmt,Name);
			return ERROR_SUCCESS;
		}
		return ERROR_ERROR;
	}
	
	return ret;
}



///////////////////////////////////////////////////////////////////////////////
//	Utility Functions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// CBuildTest
///////////////////////////////////////////////////////////////////////////////////////

BOOL CBuildTest::RunAsDependent(void)
{
	return TRUE; //run always if needed
}

void CBuildTest::Run(void)
{
	GetSubSuite()->OpenProject();
	VerifyBP();
	SetDebugTarget();
	BuildProject();
}



BOOL CBuildTest::VerifyBP(void)
{
	UIBreakpoints UIBP;
	GetLog()->RecordInfo("Verifying SQL Breakpoints...");
	if(UIBP.Activate()==FALSE)
	{
		LOG->RecordFailure("Couldn't activate Breakpoints dialog");
		return FALSE;
	}
	int count=UIBP.GetCount()-1; // funny bug - GetCount() returns count+1
	if(count!=GetSubSuite()->m_BPs)
		LOG->RecordFailure("Only %d breakpoints recovered. Expected %d",count,GetSubSuite()->m_BPs);
	//TODO don't check the location here
	//make sure  you validate the pwd on edit code
	//but check if you can hit edit code from here
	//would be nice to check each BP being enabled, but no good COBP function exist
	//consider OLE Aut for this purpose
	UIBP.Close();
	return TRUE;
}



void CBuildTest::SetDebugTarget(void)
{
	CString DebugTrg=GetLocString(IDSS_WIN32_DEBUG);
	BOOL bPass = GetSubSuite()->m_prj.SetTarget(DebugTrg);
	LOG->RecordCompare(bPass==ERROR_SUCCESS,"Setting target to %s", DebugTrg);
}


BOOL CBuildTest::BuildProject(void)
{
	BOOL bResult;

	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	GetLog()->RecordInfo("Building project...");

	// build the project
	GetLog()->RecordCompare(pPrj->Build( 60 ) == ERROR_SUCCESS, "Building project.");


	GetLog()->RecordInfo("Verifying the build...");

	// verify the build
	bResult = GetLog()->RecordCompare(pPrj->VerifyBuild(TRUE) == ERROR_SUCCESS, "Verifying build.");
	if (!bResult) {
		throw CTestException("CSysBuildTest::BuildProject: cannot build the Application project; unable to continue.", CTestException::causeOperationFail);
	}
	return bResult;
}


BOOL StoredProcParam(LPCSTR Params=NULL);
///////////////////////////////////////////////////////////////////////////////////////
// CInprocTest
///////////////////////////////////////////////////////////////////////////////////////
void CInprocTest::Run(void)
{
	CSniffDriver *pSubSuite=GetSubSuite();
	pSubSuite->OpenProject();
	BPInfo *pbpTable=pSubSuite->BPTable;
//REVIEW: CHRISKOZ, I have to force the password at the begining,
//	because the login dialogs come unpredictably without any relation to BPs	
	int ret=ERROR_SUCCESS;
	for(int indsvr=0;indsvr<pSubSuite->m_Servers;indsvr++)
	if(pSubSuite->m_svrtable[indsvr].m_Psw.GetLength()!=0 &&
		pSubSuite->m_projOpen<0)
	{	//	ForcePasword();
			LPCSTR ServerName=pSubSuite->m_svrtable[indsvr].m_Server;
			LPCSTR DataBase=pSubSuite->m_svrtable[indsvr].m_DataBase;
			UIWorkspaceWindow wnd;
			Sleep(1000); //Xtra safety
			wnd.ActivateDataPane();
			
			LOG->RecordInfo("Forcing the Login dialog to validate password");
			Sleep(1000); //Xtra safety
			MST.DoKeys("{Left}{Left}{Left}{Right}"); //expand the datasources
			Sleep(1000); //Xtra safety
			//TODO this sequence can be changed
			MST.DoKeys(DataBase,TRUE);
			MST.DoKeys(" (",TRUE);
			MST.DoKeys(ServerName,TRUE);
				
			MST.DoKeys("(%{Enter})"); //bring up the properties

			int bResult=pSubSuite->m_prj.ValidatePassword(
			pSubSuite->m_svrtable[indsvr].m_User,
			pSubSuite->m_svrtable[indsvr].m_Psw,
			pSubSuite->m_svrtable[indsvr].m_DataBase);
			if(bResult!=ERROR_SUCCESS)
			{
				ret=ERROR_ERROR;
				break;
			}
	} //force pasword
	if(ret==ERROR_SUCCESS)
		pSubSuite->m_projOpen=1;
//force password

	SetActiveProject(2); //active SQL project
	for(int ind=0;ind<pSubSuite->m_BPs;ind++)
	{
		int svrno=pbpTable[ind].ServerInd;
		UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
		UIBreakpoints UIBP;
		GetLog()->RecordInfo("Openning SPs with SQL Breakpoints...");
		if(UIBP.Activate()==FALSE)
		{
			LOG->RecordFailure("Couldn't activate Breakpoints dialog");
//			return FALSE;
			return;
		}
		int count=UIBP.GetCount()-1; // funny bug - GetCount() returns count+1
		MST.WListItemClk(GetLabel(0x7d0),ind+1); //select the item from BP list
		MST.WButtonClick(GetLabel(0x7d1)); // edit code
		// start 3D debugging for the active window
		BOOL bPass = TRUE;
		CODebug debug;
		// hack: the SQL Server Login dialog must be validated
		if(pSubSuite->m_svrtable[svrno].m_Psw.GetLength()!=0 &&
			pSubSuite->m_projOpen<0)
		{
			if(pSubSuite->m_prj.ValidatePassword(
				pSubSuite->m_svrtable[svrno].m_User,
				pSubSuite->m_svrtable[svrno].m_Psw,
				pSubSuite->m_svrtable[svrno].m_DataBase)==ERROR_SUCCESS)
			pSubSuite->m_projOpen=1;
			else MST.WButtonClick(GetLabel(IDCANCEL));
		}
		WaitForInputIdle(g_hTargetProc, 5000);
		Sleep(2000); //safety: MSDev returns out of Idle but the debug is inactive for some obscure reason
		CString str;
		MST.WGetText(NULL, str); //should be the name of the SP window
		// TODO - write the procedure to check the window title
		// CheckCurrentSP(
		//	CString str /*window title or NULL if active*/
		//	CString Proc /* pbpTable[ind].Proc */ )
		GetLog()->RecordInfo("Opened the SP window: %s",str);
		
		if(pbpTable[ind].Proc.Find(';')>0)
			continue; // do not try to debug the trigger
		GetLog()->RecordInfo("Debugging SP: %s",pbpTable[ind].Proc);

//		if((bPass=debug.Go())==FALSE)
		for(int cnt=0;cnt<3;cnt++)
		{	//assume 3 times here. You'd better read it from params
			if(cnt==0)
			{
				MST.DoKeys("{F5}"); //debug
				bPass=StoredProcParam(); //waits for break
			}
			else if(cnt==1)
			{	// do StepInto two (2) times
				bPass=debug.StepInto(2,NULL,NULL,NULL,WAIT_FOR_BREAK);
			}
			else // cnt==2
			{
				bPass=UIWB.Go(WAIT_FOR_BREAK);
			}
			if(bPass==FALSE)
			{
//				while(1) //remove this line later
//					Sleep(100000);
				LOG->RecordFailure("Didn't break in 3D debugging");
				throw CTestException("SQL debugging broken; unable to continue.\nCheck if AutMgr32 is running.\nCheck this machine's & server's error log.\n", 
					CTestException::causeOperationFail);
				break;
			}
		}
		if(bPass==FALSE)
			break;
		if((bPass=debug.StopDebugging())==FALSE)
		{
//			while(1) //remove this line later
//				Sleep(100000);
			LOG->RecordFailure("Didn't stopped 3D debugging");
			throw CTestException("3D debugging broken; unable to continue.\nCheck if AutMgr32 is running.\nCheck this machine's & server's error log.\n", 
				CTestException::causeOperationFail);
			break;
		}
		GetLog()->RecordSuccess("Debugging SP: %s",pbpTable[ind].Proc);
	} //for
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
}


////////////////////////////////////////////////////////////////////
// Waits for the MS Developper St [break] title
// Meanwhile, dismiss the SP params dialog
//
// RETURN - TRUE if success, otherwise FALSE 
////////////////////////////////////////////////////////////////////
BOOL StoredProcParam(LPCSTR Params /*=NULL*/)
{
	int nSec=60;
	BOOL bFound=FALSE;
	while (nSec>0)
	{
		CString str;
		CString szTitleBreak  = "[";
		szTitleBreak+=GetLocString(IDSS_DBG_BREAK);
		szTitleBreak+="]";
		MST.WGetText(NULL, str); //should be ServerName - APPNAME"
		nSec--;
		if(str.Find(GetLocString(IDSS_WORKBENCH_TITLEBAR))>=0 &&
			str.Find(szTitleBreak)>=0)
		{
			LOG->RecordSuccess("Found the break in the window title");
			Sleep(1000); //break in window title is up, but XTRA safety required!
			bFound = TRUE;
			break;
		}
		else if(str.Find(GetLocString(IDSS_RUN_STOREDPROC)) >=0)
		{	// the SQL param dlg popped up
			//TODO maybe do not run something which requires the param
			MST.WButtonClick(GetLabel(IDOK)); // accept defaults
		}
		else
		{	if (nSec > 0) // If we still have some waiting time...
			{
			LOG->RecordInfo("Current wnd title: %s",str);
			LOG->RecordInfo("Expected wnd title: %s %s",
				GetLocString(IDSS_WORKBENCH_TITLEBAR),szTitleBreak);
			LOG->RecordInfo("Waiting up to %d more second(s) for an expected window",nSec);
			Sleep(1000);
			}
			else                // We've timed out.
				LOG->RecordFailure("Timeout waiting for the Devstudio window");
		}
	} //while */
	return bFound;
}

///////////////////////////////////////////////////////////////////////////////////////
// CAppTest
///////////////////////////////////////////////////////////////////////////////////////
BOOL  WaitForAppWnd(CString &StrTitle);

void CAppTest::Run(void)
{
	::OleInitialize(NULL);
	CSniffDriver* pSubSuite = GetSubSuite();
	BOOL bPass =pSubSuite->OpenProject();
	int ret=ERROR_SUCCESS;
	for(int indsvr=0;indsvr<pSubSuite->m_Servers;indsvr++)
	if(pSubSuite->m_svrtable[indsvr].m_Psw.GetLength()!=0 &&
		pSubSuite->m_projOpen<0)
	{	//	ForcePasword();
			LPCSTR ServerName=pSubSuite->m_svrtable[indsvr].m_Server;
			LPCSTR DataBase=pSubSuite->m_svrtable[indsvr].m_DataBase;
			UIWorkspaceWindow wnd;
			Sleep(1000); //Xtra safety
			wnd.ActivateDataPane();
			
			LOG->RecordInfo("Forcing the Login dialog to validate password");
			Sleep(1000); //Xtra safety
			MST.DoKeys("{Left}{Left}{Left}{Right}"); //expand the datasources
			Sleep(1000); //Xtra safety
			//TODO this sequence can be changed
			MST.DoKeys(DataBase,TRUE);
			MST.DoKeys(" (",TRUE);
			MST.DoKeys(ServerName,TRUE);
				
			MST.DoKeys("(%{Enter})"); //bring up the properties

			BOOL bResult=pSubSuite->m_prj.ValidatePassword(
			pSubSuite->m_svrtable[indsvr].m_User,
			pSubSuite->m_svrtable[indsvr].m_Psw,
			pSubSuite->m_svrtable[indsvr].m_DataBase);
			if(bResult!=ERROR_SUCCESS)
			{
				ret=ERROR_ERROR;
				break;
			}
	} //force pasword
	if(ret==ERROR_SUCCESS)
		pSubSuite->m_projOpen=1;
//force password

	SetActiveProject(1); // active the native C++ project
	{ //obsolete - same as CBuildTest::SetDebugTarget()
		CString DebugTrg=GetLocString(IDSS_WIN32_DEBUG);
		bPass = GetSubSuite()->m_prj.SetTarget(DebugTrg);
	}
	//review CHRISKOZ: default file to grab is the FIRST server name from tha server table
	CString strArguments=pSubSuite->m_SuiteParams->GetTextValue("APPCMD", GetSubSuite()->m_svrtable[0].m_Server + " /r");
	bPass=GetSubSuite()->m_prj.SetProgramArguments(strArguments);
	LOG->RecordCompare(bPass==ERROR_SUCCESS,"Setting argument");
 	
	int debugcount=pSubSuite->m_SuiteParams->GetIntValue("DEBUGCOUNT", 1);
	bPass=TRUE;
	while(debugcount>0 && bPass==TRUE)
	{
		//	use UIWB.Go rather thsn CODebug.go here, because you do want the focus to be on the APP
		// bPass=UIWB.Go(NOWAIT); //does not work, hangs sometimes
		bPass=TRUE;MST.DoKeys("{F5}");Sleep(5000);//instead the prev line
		LOG->RecordCompare(bPass==TRUE,"Lanching ODBC application");
		if(bPass==FALSE)
			break;
		//wait until ODBC app gets the focus if you launched with /r option

		/*	BOOL  bFound=WaitForAppWnd(APPNAME +" - "+GetSubSuite()->m_Server);
		if(bFound==TRUE)
		{
			LOG->RecordInfo("Now hitting the <APPLICATION> menu...");
			MST.DoKeys("%CR");	//reconnect all (temporary test)
			Sleep(10000);		//safety
			MST.DoKeys("%TC");	// create threads
			Sleep(1000);
			MST.DoKeys("%TE");	// execute threads
			Sleep(1000);
		}
		bFound=Wait(WAIT_FOR_BREAK); //just wait, no F5 key
		//bFound = m_coDebug.Go(NULL, NULL, NULL, WAIT_FOR_BREAK); */
		bPass=HitBreakpoints();	//we'll see how it runs
		debugcount--;
		UIWB.Activate(); //focus problem when debugee running
		UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
	}
	LOG->RecordInfo("Uninitalizing OLE...");
	::OleUninitialize();
}


BOOL  WaitForAppWnd(CString &StrTitle)
{
	BOOL bFound=FALSE;
	int nSec = 30;
	CString strExpect=StrTitle;
	strExpect.MakeUpper();
	while (nSec>0)
	{
		CString str;
		MST.WGetText(NULL, str); //should be ServerName - APPNAME"
		str.MakeUpper();
		if(str.Find(strExpect)>=0)
		{
			Sleep(1000); //APP window is up, but XTRA safety required!
			bFound = TRUE;
			break;
		}
		else if (--nSec > 0) // If we still have some waiting time...
		{
			LOG->RecordInfo("Current wnd title: %s",str);
			LOG->RecordInfo("Expected wnd title: %s",strExpect);
			LOG->RecordInfo("Waiting up to %d more second(s) for App window...",nSec);
			Sleep(1000);
		}
		else                // We've timed out.
		{
			LOG->RecordFailure("Timeout waiting for the App window");
		}
	} //while */
	return bFound;
}


BOOL WaitState(IDebugger *pDbg,int WAIT);

////////////////////////////////////////////////////////////////////
// Retreives pointers to the app,debugger,doc objects in DevStudio
// Hits run in debugger object and calculates the locations stopped
//
// RETURN - TRUE if success, otherwise FALSE 
////////////////////////////////////////////////////////////////////
BOOL CAppTest::HitBreakpoints()
{
	BOOL bPass=TRUE;
	int hitcount[100]; //I hope 100 is MAX big enough
	int nb_hits=0; //number times breakpoints must be hit
	BPInfo *pbpTable=GetSubSuite()->BPTable;
	for(int i=0;i<GetSubSuite()->m_BPs;i++)
	{
		hitcount[i]=pbpTable[i].HitCount;
		nb_hits+=hitcount[i];
	}
//	BOOL debugide=GetSubSuite()->GetIDE()->GetSettings()->GetBooleanValue(settingDebugBuild);

	IApplication *pApp=UIWB.m_pEventHandler->m_pApp;
	IDebugger *pDbg=UIWB.m_pEventHandler->m_pDbg;
//short lived interfaces we must get from IApplication
	ITextDocument	*pDoc = NULL;
	ITextSelection	*pSel = NULL;
	ITextWindow		*pTextWin = NULL;
	IUnknown	*pUnk = NULL;
	IDispatch	*pDisp = NULL;
	HRESULT hr;
	try 
	{
		while(--nb_hits>=0)
		{
			WaitStepInstructions("Looping in Debugger Object...");
			bPass=WaitState(pDbg,WAIT_FOR_BREAK);  // wait for break in the IDE title
			if(bPass==FALSE)
			{
				pDbg->Stop();
				throw CTestException("Cannot break in the debugger; Unable to continue.\n", 
					CTestException::causeOperationFail);
			}		
			if(FAILED(hr=pApp->get_ActiveDocument(&pDisp))
			|| FAILED(hr=pDisp->QueryInterface(IID_ITextDocument, (void**)&pDoc)))
		        throw CTestException("COM error in OLEAUT.\n", 
						CTestException::causeOperationFail);
			pDisp->Release();pDisp=NULL;
				#ifdef _DEBUG
				LOG->RecordInfo("\tGot the text document");
				#endif
			if(FAILED(hr=pDoc->get_Selection(&pDisp))
			|| FAILED(hr=pDisp->QueryInterface(IID_ITextSelection, (void**)&pSel)))
		        throw CTestException("COM error in OLEAUT.\n", 
						CTestException::causeOperationFail);
			pDisp->Release();pDisp=NULL;
				#ifdef _DEBUG
				LOG->RecordInfo("\tGot the text selection");
				#endif
			BSTR bstrline;			
			BSTR bstrcaption;
			long lineno;
			pDoc->get_Name(&bstrline); //doc name always empty for SP
		    if(FAILED(hr=pDoc->get_ActiveWindow(&pDisp))
			|| FAILED(hr=pDisp->QueryInterface(IID_ITextWindow, (void**)&pTextWin)))
		        throw CTestException("COM error in OLEAUT.\n", 
						CTestException::causeOperationFail);
			pDisp->Release();pDisp=NULL;

			pTextWin->get_Caption(&bstrcaption);

			LOG->RecordInfo("Current wnd : %S", bstrcaption);
			pSel->SelectLine();
			pSel->get_Text(&bstrline);
				#ifdef _DEBUG
				LOG->RecordInfo("\tGot the current line text");
				#endif
			CString strFound;
			strFound.Format("%S",bstrline);
			strFound.TrimRight(); //get rid of tabs & EOL
			
			pSel->get_CurrentLine(&lineno);
				#ifdef _DEBUG
				LOG->RecordInfo("\tGot the current line number");
				#endif
			LOG->RecordInfo("Current line: '%s' #%d", strFound, lineno);


			LOG->RecordInfo("%d BPs left to be hit...",nb_hits);
			if(bPass==FALSE)
			{
				pDbg->Stop();
				break; //we didn't succeed - escape
			}
			strFound.TrimLeft();
			CString strCaption;
			strCaption.Format("%S", bstrcaption); 
			strCaption = strCaption.SpanExcluding(":");
			strCaption.TrimRight();
			for(int ind=0;ind<GetSubSuite()->m_BPs;ind++)
			{
				if(strFound.Find(pbpTable[ind].Stmt)==0) //must be match @1 character of the line
//				if(pbpTable[ind].Proc==strCaption)	//does not work for triggers
				{
					LOG->RecordInfo("Hit BP on stmnmt: %s in proc: %s",
					pbpTable[ind].Stmt,pbpTable[ind].Proc);
					hitcount[ind]--; //change local count table only
					break;
				}
			}  
			if(nb_hits>0)
			{
				LOG->RecordInfo("Hitting GO...");
				hr=pDbg->Go();
				//hr=pApp->ExecuteCommand(L"DebugGo"); //TODO: we need a BSTR rather than short* here
			}
			else
			{
				LOG->RecordInfo("Hitting STOP...");
				hr=pDbg->Stop();
				//hr=pApp->ExecuteCommand(L"DebugStopDebugging"); //TODO: we need a BSTR rather than short* here
				WaitState(pDbg,WAIT_FOR_TERMINATION);  // wait for termination
			}
			if(FAILED(hr))
		        throw CTestException("COM error in OLEAUT.\n", 
					CTestException::causeOperationFail);

			pDoc->Release();pDoc=NULL;
			pSel->Release();pSel=NULL;
			pTextWin->Release();pTextWin=NULL;
		} //while 
		if(bPass==TRUE)
		for(int i=0;i<GetSubSuite()->m_BPs;i++)
			if(hitcount[i]!=0)
			{
				LOG->RecordInfo("%d BP not hit %d times",i,hitcount[i]);
				bPass=FALSE;
			}
		LOG->RecordCompare(bPass!=FALSE,"Pass count in each BP hit");
    } 
	catch(...) 
	{ //catch all exceptions & do not rethrow as the interfaces must be released
        if(FAILED(hr))
			LOG->RecordFailure("COM error Code = %08lx" /*\n\tCode meaning = %s"*/, hr);
	}
	if(pDoc != NULL)pDoc->Release();
	if(pSel != NULL)pSel->Release();
	if(pTextWin != NULL)pTextWin->Release();
	if(pUnk != NULL)pUnk->Release();
	if(pDisp != NULL)pDisp->Release();
	return bPass;
} //CAppTest::HitBreakpoints()


//void dump_com_error(_com_error &e)
//{
//    LOG->RecordFailure("Hit a COM error!\n\tCode = %08lx\n\tCode meaning = %s",
//		e.Error(), e.ErrorMessage());
//}







////////////////////////////////////////////////////////////////////
// waits for devstudio active window with break in tile
// pDbg - pointer to the debugger object in DevStudio
// WAIT - WAIT_FOR_BREAK or WAIT_FOR_TERMINATION or WAIT_FOR_RUN
////////////////////////////////////////////////////////////////////
BOOL WaitState(IDebugger *pDbg,int WAIT)
{
	BOOL bSuccess=FALSE;
	int nSec=60;
	DsExecutionState expState;
	CString str_State;
	if(WAIT==WAIT_FOR_BREAK)
	{
		expState=dsBreak;
		str_State="break";
	}
	if(WAIT==WAIT_FOR_RUN)
	{
		expState=dsRunning;
		str_State="run";
	}
	if(WAIT==WAIT_FOR_TERMINATION)
	{
		expState=dsNoDebugee;
		str_State="finish";
	}
	while (nSec>0)
	{
		nSec-=2;
		DsExecutionState currentState;
		pDbg->get_State(&currentState);
		if(currentState==expState)
		{
			LOG->RecordSuccess("The debugger %ss", str_State);
			Sleep(1000); //break in window title is up, but XTRA safety required!
			bSuccess = TRUE;
			break;
		}
		else 
		{	
			if (nSec > 0) // If we still have some waiting time...
			{
				LOG->RecordInfo("Waiting up to %d more second(s) for an expected state",nSec);
				Sleep(2000);
			}
			else
			{
                // We've timed out.
				if(currentState==dsBreak)
					str_State="break";
				if(currentState==dsRunning)
					str_State="run";
				if(currentState==dsNoDebugee)
					str_State="finish";
				LOG->RecordInfo("Current Devstudio state: %s",str_State);
				LOG->RecordFailure("Timeout waiting for the correct state");				
			}
		}
	} //while */
	return bSuccess;
}




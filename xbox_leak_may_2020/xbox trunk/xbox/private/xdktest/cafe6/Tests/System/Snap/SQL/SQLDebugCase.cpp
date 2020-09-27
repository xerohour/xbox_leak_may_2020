///////////////////////////////////////////////////////////////////////////////
//	SQLDebugCase.CPP
//
//	Created by :			Date :
//		CHRISKOZ					5/21/97
//
//	Description :
//		Implementation of the CSQLDebugCase class

#include "stdafx.h"
#include "SQLDebugCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CSQLDebugCase, CSystemTestSet, NULL, -1, CSQLDebugSuite)

void CSQLDebugCase::PreRun(void)
{
	if(GetSystem() & SYSTEM_DBCS)//REVIEW(chriskoz): note doubled character '\' - compiler treats it as ESC sequence
		m_SQLProjectName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü SQL Project";
	else
		m_SQLProjectName ="SQL Project";

	// call the base class
	CTest::PreRun();
}

void CSQLDebugCase::Run(void)
{
	if(GetLang()!=CAFE_LANG_ENGLISH)
		return;
//	XSAFETY;
	CString m_Server;
	CString m_DataBase;
	m_Server=GetSubSuite()->m_SuiteParams->GetTextValue("SERVER", "DLAB1");
	m_DataBase=GetSubSuite()->m_SuiteParams->GetTextValue("DATABASE", "pubs");
	m_User=GetSubSuite()->m_SuiteParams->GetTextValue("USER", "sa");
	m_Pwd=GetSubSuite()->m_SuiteParams->GetTextValue("PWD");

	prj.Open(GetCWD() + m_strWorkspaceLoc + m_strWorkspaceName + ".DSW");

	CreateDBProjectInCurrentWksp(m_Server,m_DataBase,m_User,m_Pwd);
	InsertStoredProcs(m_Server, m_DataBase,m_User,m_Pwd);
	DebugSP(m_Server, m_DataBase,m_User,m_Pwd);
}


void CSQLDebugCase::CreateFileDSN(LPCSTR DSN,LPCSTR ServerName, LPCSTR DBName,LPCSTR User, LPCSTR Pwd)
{
	CStdioFile fileDSN(DSN, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		
	fileDSN.WriteString("[ODBC]\nDRIVER=SQL Server\nUID=");
	fileDSN.WriteString("sa");
		
	fileDSN.WriteString("\nDATABASE=");
	fileDSN.WriteString(DBName);
		
	fileDSN.WriteString("\nWSID=");
	DWORD length = MAX_COMPUTERNAME_LENGTH+1;
	char lpszNetName[MAX_COMPUTERNAME_LENGTH+1];
	if (!GetComputerName(lpszNetName, &length))
		strcpy(lpszNetName, "[ErrorComputerName]");
	m_Computername=lpszNetName;
	fileDSN.WriteString(lpszNetName);
		
	fileDSN.WriteString("\nAPP=");
	CString str;
	str.Format("%s - Owner: %s", (LPCSTR)GetSubSuite()->GetName(), (LPCSTR)GetSubSuite()->GetOwner());
	fileDSN.WriteString(str);
	
	fileDSN.WriteString("\nSERVER=");
	fileDSN.WriteString(ServerName);
	fileDSN.WriteString("\n");
} //CreateFileDSN()


void CSQLDebugCase::CreateDBProjectInCurrentWksp(LPCSTR ServerName,LPCSTR DBName,LPCSTR User, LPCSTR Pwd)
{
//	sprintf(m_DSN,
	KillAllFiles(GetCWD() + m_SQLProjectName);
	m_DSN=GetCWD() + m_SQLProjectName + "\\" + ServerName + ".DSN"; //name of file datasource
	::CreateDirectory(GetCWD() + m_SQLProjectName, NULL);

	CreateFileDSN(m_DSN, ServerName, DBName,User,Pwd);

	BOOL bResult=m_prj.NewProject(DATABASE_PROJECT,
		m_SQLProjectName,GetCWD()+m_SQLProjectName,
		GetUserTargetPlatforms(),TRUE);
	if(!LOG->RecordCompare(bResult == ERROR_SUCCESS,"Creating the DB projecct"))
			throw CTestException("Cannot create DB project; unable to continue.\n", 
					CTestException::causeOperationFail);   
	bResult=m_prj.AddSQLDataSource(m_DSN,CString(User),CString(Pwd),DBName);
	if(!LOG->RecordCompare(bResult == ERROR_SUCCESS, 
			"Opening the SQL Server '%s' database %s.",
			ServerName,
			DBName))
			throw CTestException("Cannot establish the connection; unable to continue.\n", 
					CTestException::causeOperationFail);
} //CreateDBProjectInCurrentWksp()


void CSQLDebugCase::InsertStoredProcs(LPCSTR ServerName,LPCSTR DBName,LPCSTR User, LPCSTR Pwd)
{
	CString m_Source=GetCWD();
	CString m_Dest=GetCWD() + m_SQLProjectName + "\\";
	CopySQLFile(m_Source, m_Dest, User, "DELETE.SQL");
	CopySQLFile(m_Source, m_Dest, User, "inner_upd.SQL");
	CopySQLFile(m_Source, m_Dest, User, "Cursor_emp.SQL");
	
	if(ExpandDatabase(ServerName,DBName)!=ERROR_SUCCESS)
		LOG->RecordFailure("Didn't expand database");
	UIWB.OpenFile( m_Dest + "DELETE.SQL");
	UIWB.DoCommand(IDM_TOOLS_RUNMENU, DC_MNEMONIC); //&Tools.&Run
	Sleep(3000); //no other way to determine if it's finished
	
	ExpandProcedureNode(ServerName,DBName);
	NewSP(m_Dest + "inner_upd.SQL");

	ExpandProcedureNode(ServerName,DBName);
	NewSP(m_Dest + "Cursor_emp.SQL");
	
	UIWB.DoCommand(IDM_FILE_SAVE_ALL, DC_MNEMONIC);
	WaitForInputIdle(g_hTargetProc, 5000);
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
	WaitForInputIdle(g_hTargetProc, 5000);

	XSAFETY;
} //InsertStoredProcs()


void CSQLDebugCase::CopySQLFile(CString &m_Source,CString &m_Dest, LPCSTR User, LPCSTR FName)
{
	CStdioFile fileDest(m_Dest + FName, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
	CStdioFile fileSource(m_Source + FName, 	CFile::modeRead | CFile::typeText);

	CString strUser;
	
	if(strcmp(User,"sa")==0)
		strUser="dbo";
	else
		strUser=CString(User);

    CString str;
    const int len=255;
    char buf[len],dest[len];
	char *pservername,*prestline;

	BOOL done=FALSE;
	while(fileSource.ReadString(buf, len))
	{
		while((pservername=strstr(buf,"$(MACHINE)"))!=NULL)
		{
			prestline=pservername+strlen("$(MACHINE)");
			pservername[0]=0; //cut the "$(MACHINE)"
			strcat(strcat(strcpy(dest,buf),m_Computername),prestline);
			strcpy(buf,dest);
		}
		while((pservername=strstr(buf,"$(USER)"))!=NULL)
		{
			prestline=pservername+strlen("$(USER)");
			pservername[0]=0; //cut the "$(USER)"
			strcat(strcat(strcpy(dest,buf),strUser),prestline);
			strcpy(buf,dest);
		}
		fileDest.WriteString(buf);	
//		fileDest.WriteString("\n");  //looks like the CRLF is included in "buf" string
	}	
} //CopySQLFile()


BOOL StepInUntilFrameAdded( CString addframe );


void CSQLDebugCase::DebugSP(LPCSTR Server,LPCSTR strDBName,LPCSTR User, LPCSTR Pwd)
{
	CString strOutterproc(CString("Cursor_emp")+m_Computername);
	CString strInnerproc(CString("inner_upd")+m_Computername);
	CString strTrigger("employee_insupd");
	CString strUserPrefix;
	
	if(strcmp(User,"sa")==0)
		strUserPrefix="dbo.";
	else
		strUserPrefix=CString(User) + '.';
	ExpandProcedureNode(Server,strDBName);
	MST.DoKeys(strOutterproc,TRUE,1000);
	Sleep(1000); //extra safety
	MST.DoKeys("+{F10}");
	Sleep(1000);
	MST.DoKeys("b"); //debug

	BOOL bPass = TRUE;
	CODebug dbg;
	COStack stk;
	COBreakpoints bp1;

	bPass=StoredProcParam("2"); //waits for parameters dialog and suply it with "2"

	// we expect sql debugging to fail on pro and learning edition.
	DWORD dwProductType = GetProductType();

	if(!bPass && ((dwProductType == VC_PRO) || (dwProductType == VC_LRN)))
		return;

	if(!LOG->RecordCompare(bPass == TRUE, 
			"Starting debugging stored procedure'%s'",
			strOutterproc))
			throw CTestException("Cannot establish the connection; unable to continue.\n", 
					CTestException::causeOperationFail);
	
	COSource src;
	src.AttachActiveEditor();

	EXPECT_TRUE( src.Find("execute inner_upd") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "execute inner_upd") );

	CString empid;
	CString fname;
	CString lname;
	COExpEval ee;

//check the @fname & @lname & store them
	ee.GetExpressionValue("@empid",empid);
	LOG->RecordInfo("Got the value for @empid:'%s'", empid);
	ee.GetExpressionValue("@fname",fname);
	LOG->RecordInfo("Got the value for @fname:'%s'", fname);
	ee.GetExpressionValue("@lname",lname);
	LOG->RecordInfo("Got the value for @lname:'%s'", lname);
	EXPECT_TRUE( !empid.IsEmpty() && !fname.IsEmpty() && !lname.IsEmpty() );
	
	EXPECT_TRUE( dbg.StepInto(1, NULL, "if(@id=NULL)") );
	EXPECT_TRUE(StepInUntilFrameAdded("dbo." + strTrigger) ); //this trigger is always owned by dbo

	EXPECT_TRUE( stk.NavigateStack( strUserPrefix +  strOutterproc + "(int 2)")>0 ); //suplied param - "2"
	EXPECT_TRUE( src.Find("exec inner_upd") );

	bp1.SetBreakpoint(); //put a BP at the second invocation of inner_upd

	EXPECT_TRUE( stk.NavigateStack( strUserPrefix +  strInnerproc)>0 );
	EXPECT_TRUE( stk.NavigateStack( "dbo." +  strTrigger)>0 ); //this trigger is always owned by dbo
	src.AttachActiveEditor();

	EXPECT_TRUE( src.Find( "IF (@job_id = 1) and (@emp_lvl <> 10)" ) );

	bp1.SetBreakpoint(); //put a BP in the trigger
	EXPECT_TRUE( dbg.Go( NULL, "IF (@job_id = 1) and (@emp_lvl <> 10)") );

 //change the @job_id value in watches & check if expression changed
		char job_id[30];
		CString strjobid;
		EXPECT_TRUE(ee.GetExpressionValue("@job_id", strjobid) );
		strncpy(job_id, strjobid, 30);
		UIWatch watch;
		watch.Activate();
		watch.SetName("@job_id", 1);
		watch.SetValue(_itoa(atoi(job_id)+1,job_id,10), 1);
		EXPECT_TRUE( ee.ExpressionValueIs("@job_id",job_id) );

		EXPECT_TRUE(MST.WFndWndWait(strTrigger, FW_FOCUS | FW_PART, 2) ); //UIWatch steals the focus
		src.AttachActiveEditor(); 

	EXPECT_TRUE( dbg.Go( NULL, "exec inner_upd") ); //must've hit the second invocation
//check if @Fname & @lname are the same as stored but switched
		EXPECT_TRUE( ee.ExpressionValueIs("@empid",empid) );
#pragma message (__LOC__ "change COMMENT to FAIL below when VC98#3725 is fixed")
		EXPECT_TRUE_COMMENT( ee.ExpressionValueIs("@fname",lname), "fname test:");
		EXPECT_TRUE_COMMENT( ee.ExpressionValueIs("@lname",fname), "lname test:");
//change the string value in the EE
		EXPECT_TRUE( ee.SetExpressionValue("@pchMsg","'Message changed in debugger'") );

//change the SQL param & expect it to change in the Stack
		watch.Activate();
		watch.SetName("@NumbeR", 2); //SQL server case insensitive
		watch.SetValue("3", 2); //change the param to "3"
#pragma message (__LOC__ "change \"int 2\" to \"int 3\" below when VC98#3722 is fixed")
		EXPECT_TRUE(stk.Index(strUserPrefix +  strOutterproc + "(int 2)")==0); //param changed to 1
		EXPECT_TRUE(MST.WFndWndWait(strTrigger, FW_FOCUS | FW_PART, 2) ); //UIWatch steals the focus
		src.AttachActiveEditor(); 
		

	EXPECT_TRUE( dbg.StepOver(1, NULL, "IF (@job_id = 1) and (@emp_lvl <> 10)") ); //stepover the BP which shld be hit

#pragma message (__LOC__ "change COMMENT to FAIL below when VC98#3614 is fixed")
	EXPECT_TRUE_COMMENT( dbg.Go( NULL, NULL, NULL, ASSUME_NORMAL_TERMINATION), "Terminated:");
#pragma message ("Add to execute sth in another database when Azeem checks in VC98#1984 fix in SDI50.dll")
} //DebugSP()

BOOL StepInUntilFrameAdded( CString addframe )
{
	BOOL bResult = TRUE;
	COStack stk;
	CODebug dbg;
	FrameList framelist0;

	// Establish base line
	framelist0.CreateFromText( stk.GetAllFunctions() );
	Frame topframe( framelist0[0] );

	while( bResult ) {
		dbg.StepInto();
		FrameList current;
		current.CreateFromText( stk.GetAllFunctions() );

		if( current.GetSize() == framelist0.GetSize() ) {
			// Only check function name or image name for top frame.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					LOG->RecordInfo( "StepInUntilFrameAdded - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else if( topframe.GetImageName( str0 ) ){
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					LOG->RecordInfo( "StepInUntilFrameAdded - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else {
				// Bad format
				LOG->RecordInfo( "StepInUntilFrameAdded - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame];
				if( !bResult ) {
					LOG->RecordInfo( "StepInUntilFrameAdded - Mismatch in frame %d, '%s' != '%s'",
						(LPCTSTR)current[0], (LPCTSTR)framelist0[0] );
				}
			}
		}
		else if( current.GetSize() == framelist0.GetSize() + 1 ) {
			// Test new top frame
			bResult = Frame( current[0] ).Test( addframe );

			// Only check function name or image name for 2nd frame since the line number
			// may change.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				bResult = Frame( current[1] ).Test( str0 );
				if( !bResult ) {
					LOG->RecordInfo( "StepInUntilFrameAdded - Mismatch 2nd frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[1] );
				}
			}
			else if( topframe.GetImageName( str0 ) ) {
				bResult = Frame( current[1] ).Test( str0 );
				if( !bResult ) {
					LOG->RecordInfo( "StepInUntilFrameAdded - Mismatch 2nd frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[1] );
				}
			}
			else {
				// Bad format
				LOG->RecordInfo( "StepInUntilFrameAdded - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame+1];
				if( !bResult ) {
					LOG->RecordInfo( "StepInUntilFrameAdded - Mismatch in frame %d, '%s' != '%s'",
						(LPCTSTR)current[iFrame+1], (LPCTSTR)framelist0[iFrame] );
				}
			}
			break;	// Get out of loop

		}
		else {
			// Unexpected number of frames
			LOG->RecordInfo( "StepInUntilFrameAdded - Expected %d frames, got %d frames",
				framelist0.GetSize() + 1, current.GetSize() );
			bResult = FALSE;
		}
	}

	return bResult;
}



////////////////////////////////////////////////////////////////////////////
// utility functions
//waits up to 60 seconds to STored Procedure params dialog to come up
BOOL ExecSPParam(LPCSTR Params /*=NULL*/)
{
	int nSec=2;
	CString str;
	int ret=ERROR_SUCCESS;
	BOOL bExecuting=FALSE;
	while (nSec>0)
	{
		MST.WGetText(NULL, str);
		if(Params!=NULL &&
			str.Find(GetLocString(IDSS_RUN_STOREDPROC)) >=0)
		{
			LPCSTR pparam=strtok((char*)Params," \t\n");
			while(pparam!=NULL)
			{
				MST.DoKeys(pparam, TRUE, 1000); //suply next param
				MST.DoKeys("{Down}", FALSE, 1000); //go to the next row in a grid
				pparam=strtok(NULL," \t\n");
			}
			MST.WButtonClick(GetLabel(IDOK)); // accept params
			bExecuting=TRUE;
			Sleep(2000);
			break;
		}
		if (nSec > 0) // If we still have some waiting time...
		{
			LOG->RecordInfo("Current wnd title: %s",str);
			LOG->RecordInfo("Waiting up to %d more second(s) for SP to execute",nSec);
			nSec--;
			Sleep(1000);
		}
	} //while */
	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())// If we can attach, an error must've occurred.
	{
		ret = ERROR_ERROR;
		wndMsgBox.ButtonClick();
	}
//TODO: maybe wait for the message about the execution finished
	return ret;
} //ExecSPParam


BOOL CSQLDebugCase::StoredProcParam(LPCSTR Params /*=NULL*/)
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
			BOOL bResult=ExecSPParam(Params); //enter the params
			
			if(bResult==ERROR_ERROR)
				break; //SP params didn't pass so return FALSE
		}
		else
		
		{	
			if (nSec > 0) // If we still have some waiting time...
			{
				LOG->RecordInfo("Current wnd title: %s",str);
				LOG->RecordInfo("Expected wnd title: %s %s", GetLocString(IDSS_WORKBENCH_TITLEBAR),szTitleBreak);
				LOG->RecordInfo("Waiting up to %d more second(s) for an expected window",nSec);
				Sleep(1000);
			}

			else
			
			{
				// we expect sql debugging to fail against pro and learning edition.
				DWORD dwProductType = GetProductType();

				if((dwProductType == VC_PRO) || (dwProductType == VC_LRN))
					LOG->RecordSuccess("SQL debugging failed on non-Enterprise product.");
				else
					// We've timed out.
					LOG->RecordFailure("Timeout waiting for the Devstudio window");
			}
		}
	} //while */
	return bFound;
}



void NewSP(LPCSTR filename)
{	
	MST.DoKeyshWnd(UIWB.HWnd(), "+{F10}");
	Sleep(1000); //xtra safety
	MST.DoKeyshWnd(UIWB.HWnd(), "{Home}");
	MST.WMenu("Ne&w Stored Procedure");

	COSource srcFile;

	srcFile.AttachActiveEditor();
	UIWB.DoCommand(ID_EDIT_SELECT_ALL, DC_MNEMONIC); //select all
	CStdioFile fileSource(filename,	CFile::modeRead | CFile::typeText);
    const int len=255;
    char buf[len];
	while(fileSource.ReadString(buf, len))
	{
		srcFile.TypeTextAtCursor(buf,TRUE);
		srcFile.TypeTextAtCursor("{ENTER}{HOME}",FALSE);
	}
} //NewSP



void CollapseObjects()
{
	MST.DoKeys("{Left}"); // goto the table
	Sleep(500); 			// Xtra Wait
	MST.DoKeys("{NUMPAD-}");// collapse table tree
	Sleep(500); 			// Xtra Wait
	MST.DoKeys("{Left}"); // goto the parent node
	Sleep(500); 			// Xtra Wait
	MST.DoKeys("{NUMPAD-}"); // collapse the expanded object
	Sleep(500); 			// Xtra Wait
	MST.DoKeys("{Left}"); // goto the parent node
	Sleep(500); 			// Xtra Wait
	MST.DoKeys("{NUMPAD-}");// collapse objects' tree
}


void ExpandTable(LPCSTR Server,LPCSTR DBName,LPCSTR strTableName)
{
	ExpandDatabase(Server,DBName);
	ExpandTableNode(Server,DBName);
	MST.DoKeys(strTableName);
	Sleep(2000); 			// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");
	Sleep(500); 			// Xtra Wait
}



BOOL SelectDatabase(LPCSTR Server,LPCSTR strDBName)
{
	UIWorkspaceWindow wnd;
 	int ret=ERROR_SUCCESS;
	Sleep(1000); //Xtra safety
	wnd.ActivateDataPane();

	MST.DoKeys("^{HOME}");				// Go to top of tree
	MST.DoKeys("{NUMPAD-}");
	Sleep(500);
	MST.DoKeys("{NUMPAD+}");	// deploy the first level
	Sleep(500);
	//TODO this sequence can be changed
	MST.DoKeys(strDBName,TRUE);
	MST.DoKeys(" (",TRUE);
	MST.DoKeys(Server,TRUE);
	
	MST.DoKeys("%({Enter})"); //bring up the properties
	BOOL bReady = FALSE;
	int nSec = 5;
	while (!bReady && ret == ERROR_SUCCESS)
	{
			// Attempt to open the property page for the Data Source
			

			CString str;
			MST.WGetText(NULL, str);
	//		if(str.Find(GetLocString(IDSS_PROP_TITLE))>0)
	//			nSec=2; //properties window is up, but XTRA safety required!
			if (MST.WStaticExists(Server) && MST.WStaticExists(strDBName))
			{
				LOG->RecordSuccess("Found the DataSource properties");
				bReady = TRUE;
			}
			else
			{
				if (--nSec > 0)	// If we still have some waiting time...
				{
//					LOG->RecordInfo("Waiting up to %d more second(s) for datasource properties...", nSec);
					Sleep(1000);
				}
				else				// We've timed out.
				{
					LOG->RecordInfo("Didn't find the Datasource properties");
					ret = ERROR_ERROR;
				}
			}
	} //while
	MST.DoKeys("{Escape}");
	Sleep(2001); //timing problem hit when switched to mstest4.0
	return ret;		
}


int ExpandDatabase(LPCSTR Server,LPCSTR strDBName)
{
	if (SelectDatabase(Server,strDBName)!=ERROR_SUCCESS) 
	{
		return ERROR_ERROR;
	}
	MST.DoKeys("{NUMPAD-}");
	Sleep(500);
	MST.DoKeys("{NUMPAD+}");
	Sleep(500);
	return ERROR_SUCCESS;
}


void ExpandProcedureNode(LPCSTR Server,LPCSTR strDBName)
{
	ExpandDatabase(Server,strDBName);
	MST.DoKeys(GetLocString(IDSS_STOREDPROC)); //"Stored Procedures"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the procedure node
	Sleep(500); 			// Xtra Wait
}


void ExpandTableNode(LPCSTR Server,LPCSTR strDBName)
{
	ExpandDatabase(Server,strDBName);
	MST.DoKeys(GetLocString(IDSS_TABLE)); // "Table"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the Table node
	Sleep(500); 			// Xtra Wait
}

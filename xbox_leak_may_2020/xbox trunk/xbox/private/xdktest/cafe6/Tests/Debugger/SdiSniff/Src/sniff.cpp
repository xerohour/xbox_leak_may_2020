///////////////////////////////////////////////////////////////////////////////
//      SNIFF.CPP
//
//      Created by :                    Updated:
//			Chriskoz                    9/15/96
//
//      Description :
//              implementation of the CSniffTest class
//

#include <process.h>
#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "sdi.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "SQLDebug Test", "ChrisKoz")
// IMPLEMENT_DESTROYSUBSUITE(CSniffDriver)


BEGIN_TESTLIST(CSniffDriver)
	BOOL brun;
	char buff[1024];
	m_AppName=m_SuiteParams->GetTextValue("APPNAME", "ODBCMT");  // or LOOPNT
	m_AppCmd=m_SuiteParams->GetTextValue("APPCMD", "<default>");// shld be <servername> /r
	CString strBPTableFile=m_SuiteParams->GetTextValue("BPTABLE", "sdibp.def");
	CString debugcount=m_SuiteParams->GetTextValue("DEBUGCOUNT", "1");

	brun=(m_SuiteParams->GetBooleanValue("DONTREBUILD", FALSE)==FALSE);
TEST(CAddDSNTest, (brun?RUN:DONTRUN))
	strcpy(buff,"Set SQLBPs - ");
	strcat(buff,strBPTableFile);
	pTest->SetName(buff);
TEST(CBuildTest, (brun?RUN:DONTRUN))
	strcpy(buff,"Ver.BPs,build ");
	strcat(buff,m_AppName);
	pTest->SetName(buff);
TEST(CInprocTest, RUN)
	strcpy(buff,"3D SQL Debug - ");
	strcat(buff,"1 time(s)");
	pTest->SetName(buff);		
	brun=!(m_SuiteParams->GetBooleanValue("LEGO", FALSE)!=FALSE
	||	CMDLINE->GetBooleanValue("LEGO", FALSE)!=FALSE);
TEST(CAppTest, (brun?RUN:DONTRUN))	// DEPENDENCY(CBuildTest)
	strcpy(buff,m_AppName);
	strcat(strcat(buff," "),m_AppCmd);
	strcat(buff," - ");
	strcat(strcat(buff,debugcount)," time(s)");
	pTest->SetName(buff);		
END_TESTLIST()


void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
	// gather all command line info 
	m_AppName=m_SuiteParams->GetTextValue("APPNAME", "ODBCMT");  // or LOOPNT
	::CreateDirectory(m_strCWD + m_AppName, NULL); //create just in case it does not exist
	m_AppCmd=m_SuiteParams->GetTextValue("APPCMD", "GALSQL1 /r");			// or <servername> /r
	CString strBPTableFile=m_SuiteParams->GetTextValue("BPTABLE", "sdibp.def");

	// Read DataBase info into m_Server, m_DataBase, m_User, m_Psw, m_DSN
	GetDataInfo(strBPTableFile);
	// register >REG file or create the .DSN file
	if(RegisterAll()!=ERROR_SUCCESS)
		throw CTestException("Can't register the components. Unable to continue", CTestException::causeOperationFail);
	m_projOpen=0; //hack - proj->IsOpen() does not work
}

///////////////////////////////////////////////////////////////////
// Helper function to provide the data from the outer filename
// retreives the information from each line of the file in a form
// <TableName;><Procname> <Statement name>;<HitCount>
//where		TableName - name of table for trigger (empty for SP)
//			ProcName  - name of SP/trigger
//			Statement name - string (witthout ';') unique ident of the line
//			HitCount  - number of times BP is supposed to be hit.
//						BP wiil be  disabled if HitCount==0
void CSniffDriver::GetDataInfo(CString &strBPTableFile)
{
    CStdioFile fileVerify(m_strCWD + strBPTableFile,
		CFile::modeRead | CFile::typeText);

    CString str;
    const int len=100;
    char buf[len];
	int ind=0,indserver=0;
	CString strDelimiter("***");
	CString strEnd("$$$");

	BOOL done=FALSE;
	while(!done && fileVerify.ReadString(buf, len) && indserver<MAX_SRVTABLE)
	{
		if (strstr(buf,strEnd)==buf) //end of strings
			break;
		m_svrtable[indserver].m_Server=strtok(buf," ");
		m_svrtable[indserver].m_DataBase=strtok(NULL," ");
		m_svrtable[indserver].m_User=strtok(NULL," ");
		m_svrtable[indserver].m_Psw=strtok(NULL," ");
		TrimString(&m_svrtable[indserver].m_Server);
		TrimString(&m_svrtable[indserver].m_DataBase);
		TrimString(&m_svrtable[indserver].m_User);
		TrimString(&m_svrtable[indserver].m_Psw);
		if(m_svrtable[indserver].m_Server.Find("DSN=")==0)
		{
			m_svrtable[indserver].m_Server=m_svrtable[indserver].m_Server.Right(m_svrtable[indserver].m_Server.GetLength()-4);
			m_svrtable[indserver].m_DSN=m_strCWD + m_AppName + "\\" + m_svrtable[indserver].m_Server + ".DSN";
		}
		else
			m_svrtable[indserver].m_DSN="Test"+m_svrtable[indserver].m_Server;

		// Now get the information about the datasource BPS
		while (fileVerify.ReadString(buf, len) && ind<MAX_BPTABLE)
		{ //read info for one server
			str = buf;
			TrimString(&str);

			if (!str.IsEmpty() && str.GetAt(0) != ';')
			{
				if (str.Find(strDelimiter)==0)
				{
					char *pScripts = buf+strlen(strDelimiter);
					m_svrtable[indserver].m_SpScripts=pScripts;
					break;
				}
				else if(strstr(buf,strEnd)==buf)
				{
					done=TRUE;
					break;
				}
				else 
				{
				BPTable[ind].Proc=strtok(buf, " \t"); // valid ProcName
				BPTable[ind].Stmt=strtok(NULL, ";");// Stmt string
				BPTable[ind].HitCount=atoi(strtok(NULL, ";")); //hit count
				TrimString(&BPTable[ind].Proc);
				TrimString(&BPTable[ind].Stmt);
				BPTable[ind].ServerInd=indserver;
				ind++;
				}
			}
		} //while
		indserver++;
	}
	m_BPs=ind;
	m_Servers=indserver;
} //GetDataInfo


BOOL CSniffDriver::RegisterAll()
{
	for(int ind=0;ind<m_Servers;ind++)
	if(m_svrtable[ind].m_DSN.Right(4)==".DSN")
	{
		CStdioFile fileDSN(m_svrtable[ind].m_DSN, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		
		fileDSN.WriteString("[ODBC]\nDRIVER=SQL Server\nUID=");
		fileDSN.WriteString(m_svrtable[ind].m_User);
		
		fileDSN.WriteString("\nDATABASE=");
		fileDSN.WriteString(m_svrtable[ind].m_DataBase);
		
		fileDSN.WriteString("\nWSID=");
		DWORD len = MAX_COMPUTERNAME_LENGTH+1;
		char lpszNetName[MAX_COMPUTERNAME_LENGTH+1];
		if (!GetComputerName(lpszNetName, &len))
			strcpy(lpszNetName, "[Error calling GetComputerName()]");
		fileDSN.WriteString(lpszNetName);
		
		fileDSN.WriteString("\nAPP=");
		CString str;
		str.Format("%s - Owner: %s", (LPCSTR)GetName(), (LPCSTR)GetOwner());
		fileDSN.WriteString(str);
		
		fileDSN.WriteString("\nSERVER=");
		fileDSN.WriteString(m_svrtable[ind].m_Server);
		fileDSN.WriteString("\n");
	}
	
	if(_spawnlp( _P_WAIT, "regedit.exe", "regedit.exe", "/s",
		  m_strCWD + "SDI.REG", NULL )==-1)
	{
		AfxMessageBox("REG file registration failed");
		return ERROR_ERROR;		
	}
	
	return ERROR_SUCCESS;

}


void CSniffDriver::CleanUp(void)
{
	if (m_prj.IsOpen()) {
		m_prj.Close();
	}
	// gather all command line info 
	m_AppName=m_SuiteParams->GetTextValue("APPNAME", "ODBCMT");
	::CreateDirectory(m_strCWD + m_AppName, NULL); //create just in case it does not exist

	// remove the project directory
	if(m_SuiteParams->GetBooleanValue("DONTCLEAN", FALSE)==FALSE)
	{
		KillAllFiles(m_strCWD + m_AppName + "\\", FALSE);
		CopyTree(m_strCWD + m_AppName + ".SRC\\",m_strCWD + m_AppName + "\\");
	}
	m_projOpen=0;
	// call the base class
	CIDESubSuite::CleanUp();
}

BOOL CSniffDriver::OpenProject(void)
{
	BOOL bResult;
	
	// set the project's filename
	CString strProjFile = m_strCWD + m_AppName + "\\" + m_AppName + ".DSW";

	// open the project
	GetLog()->RecordInfo("Opening the project file '%s'...", (LPCSTR)strProjFile);

	// if the project's already open, no need to open it again
	if (m_projOpen!=0) 
	{
		GetLog()->RecordSuccess("The project '%s' is already open.", (LPCSTR)(m_prj.GetFullPath()));
		bResult=TRUE;
	}
	else {
		// open the project
		bResult = GetLog()->RecordCompare((m_prj.Open(strProjFile, 0) == ERROR_SUCCESS), "Opening the project file '%s'.", (LPCSTR)strProjFile);
		if (!bResult) {
			throw CTestException("CBuildTest::OpenProject: cannot open project; unable to continue.", CTestException::causeOperationFail);
		}
		m_projOpen=-1; // DSN not pwd validated
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////
// utility functions
void CSniffDriver::CollapseObjects()
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

void CSniffDriver::ExpandTable(CString &Server,CString &DBName,LPCSTR strTableName)
{
	ExpandDatabase(Server,DBName);
	ExpandTableNode(Server,DBName);
	MST.DoKeys(strTableName);
	Sleep(2000); 			// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");
	Sleep(500); 			// Xtra Wait
}



BOOL CSniffDriver::SelectDatabase(CString &Server,CString &strDBName)
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

int CSniffDriver::ExpandDatabase(CString &Server,CString &strDBName)
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


void CSniffDriver::ExpandProcedureNode(CString &Server,CString &strDBName)
{
	ExpandDatabase(Server,strDBName);
	MST.DoKeys(GetLocString(IDSS_STOREDPROC)); //"Stored Procedures"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the procedure node
	Sleep(500); 			// Xtra Wait
}


void CSniffDriver::ExpandTableNode(CString &Server,CString &strDBName)
{
	ExpandDatabase(Server,strDBName);
	MST.DoKeys(GetLocString(IDSS_TABLE)); // "Table"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the Table node
	Sleep(500); 			// Xtra Wait
}

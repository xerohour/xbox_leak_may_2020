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
#include <process.h>

#include "syscase.h"
#include "spcase.h"
#include "datacase.h"
#include "davcase.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

int ImeInit = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CDataSubSuite, CIDESubSuite, NULL, "ChrisKoz")

BEGIN_TESTLIST(CDataSubSuite)
	CString strDSNFile=m_SuiteParams->GetTextValue("DSN", "sql.def");
	m_strName=CString("Data Pane, DSN=") + strDSNFile;
	BOOL bDavinci=m_SuiteParams->GetBooleanValue("DESIGNERS", FALSE);
TEST(CConnectTest, RUN)
TEST(CDavinciTest, bDavinci?RUN:DONTRUN)	DEPENDENCY(CConnectTest)
	CString strTable=m_SuiteParams->GetTextValue("DESIGNTABLE", "sales");
	strTable = "Designers on table - " + strTable;
	pTest->SetName(strTable);		
TEST(CTablesTest, bDavinci?DONTRUN:RUN)	DEPENDENCY(CConnectTest)
TEST(CSPTest, bDavinci?DONTRUN:RUN)		DEPENDENCY(CConnectTest)
//	if ((GetSystem() & SYSTEM_DBCS) != 0) { // Only if on a DBC system
//		TEST(CSourceTestCases, DONTRUN)
//	}
END_TESTLIST()

void CDataSubSuite::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);

	CString strDSNFile=m_SuiteParams->GetTextValue("DSN", "sql.def");

	m_DataInfo.GetDataInfo(m_strCWD,strDSNFile);
	if(RegisterAll()!=ERROR_SUCCESS)
		throw CTestException("Can't register the components. Unable to continue", CTestException::causeOperationFail);

	m_projOpen=0; //hack - proj->IsOpen() does not work
}

//inserts the script to generate the table/cleanup the views/diagrams
//creates the new views/diagrams with name (%MACHINE)sqlTableName
//RETURN - returns how many rows have been generated (how many insert statements)
//the current database context is assumed
int CDataSubSuite::CleanAndInsertTable(LPCSTR sqlTableName)
{
	CString m_Dest=m_strCWD + PROJDIR;
	int ret=CopySQLFile(m_strCWD, m_Dest, CString(sqlTableName) + ".SQL");
	
	UIWB.OpenFile( m_Dest + sqlTableName + ".SQL");
	UIWB.DoCommand(IDM_TOOLS_RUNMENU, DC_MENU); //&Tools.&Run
	Sleep(3000); //no other way to determine if it's finished

	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())// If we can attach, an error must've occurred.
	{
		char title[128];
		::GetWindowText(wndMsgBox, title, 128);
//		if(MST.WFndWndWait("Server Error(s)"/*GetLocString()*/, FW_PART, 0))
//			LOG->Comment("'Server Error(s)' message box detected");
//		else
		{
			LOG->RecordFailure("Unknown (%s) message box detected while running the cleanup script",title);

		}
		//wndMsgBox.ButtonClick(); //does not work for server error(s)
		MST.DoKeys("{ENTER}");
	}
	return ret;
}

int CDataSubSuite::CopySQLFile(CString &m_Source,CString &m_Dest,LPCSTR FName)
{
	CStdioFile fileDest(m_Dest + FName, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
	CStdioFile fileSource(m_Source + FName, 	CFile::modeRead | CFile::typeText);


    CString str;
    const int len=255;
    char buf[len],dest[len];
	char *pservername,*prestline;

	int nrows=0;
	while(fileSource.ReadString(buf, len))
	{
		while((pservername=strstr(buf,"$(MACHINE)"))!=NULL)
		{
			prestline=pservername+strlen("$(MACHINE)");
			pservername[0]=0; //cut the "$(MACHINE)"
			strcat(strcat(strcpy(dest,buf),m_Computername),prestline); //store new line in dest
			strcpy(buf,dest); //line back to buf
		}
		fileDest.WriteString(buf);
		if(strstr(buf,"insert ") == buf)
			nrows++;
//		fileDest.WriteString("\n");  //looks like the CRLF is included in "buf" string
	}	
	return nrows;
} //CopySQLFile()



BOOL CDataSubSuite::RegisterAll()
{
	DWORD len = MAX_COMPUTERNAME_LENGTH+1;
	char lpszNetName[MAX_COMPUTERNAME_LENGTH+1];
	if (!GetComputerName(lpszNetName, &len) || lpszNetName[0]==0)
	{  //computer name must be valid & non empty
		strcpy(lpszNetName, "[Error calling GetComputerName()]");
		LOG->RecordFailure(lpszNetName);
		return ERROR_ERROR;		
	}
	m_Computername=lpszNetName;

	if(m_DataInfo.m_strDSN.Right(4)==".DSN")
	{
    CStdioFile fileDSN(m_DataInfo.m_strDSN, 
		CFile::modeCreate | CFile::modeWrite | CFile::typeText);

	fileDSN.WriteString("[ODBC]\nDRIVER=SQL Server\nUID=");
	fileDSN.WriteString(m_DataInfo.m_strUser);

	fileDSN.WriteString("\nDATABASE=");
	fileDSN.WriteString(m_DataInfo.m_strDataBase);

	fileDSN.WriteString("\nWSID=");
	fileDSN.WriteString(lpszNetName);

	fileDSN.WriteString("\nAPP=");
	CString str;
	str.Format("%s - Owner: %s", (LPCSTR)GetName(), (LPCSTR)GetOwner());
	fileDSN.WriteString(str);

	fileDSN.WriteString("\nSERVER=");
	fileDSN.WriteString(m_DataInfo.m_strServer);
	fileDSN.WriteString("\n");
	}

	if(_spawnlp( _P_WAIT, "regedit.exe", "regedit.exe", "/s",
		  m_strCWD + "SQL.REG", NULL )==-1)
	{
		LOG->RecordFailure("REG file registration failed");
		return ERROR_ERROR;		
	}
	return ERROR_SUCCESS;
}

void CDataSubSuite::CleanUp(void)
{
	if (m_prj.IsOpen()) {
		m_prj.Close();
	}
	m_DataInfo.ClearAll();
	// remove the project directory
	KillAllFiles(m_strCWD + PROJDIR, TRUE);	
	if(m_DataInfo.m_strDSN.Right(4)==".DSN")
		::DeleteFile(m_DataInfo.m_strDSN);
	m_projOpen=0;
	// call the base class
	CIDESubSuite::CleanUp();
}

///////////////////////////////////////////////////////////////////
// Helper functions to manipulate the data workspace
//
void CDataSubSuite::OpenDataProject()
{
	if(!m_projOpen)
	{
		CString strPrjName=m_strCWD + PROJDIR + PROJNAME + ".DSW";
		if(m_prj.Open(strPrjName)!=ERROR_SUCCESS)
			GetLog()->RecordFailure("Coudn't open project - %s",strPrjName);
		else
		{
			m_projOpen=-1;
			Sleep(2000); //safety
		}
	}
} //OpenDataProject

//
BOOL CDataSubSuite::ForcePassword(CDataInfo *pDataInfo)
{
	LPCSTR ServerName=pDataInfo->m_strServer;
	LPCSTR DataBase=pDataInfo->m_strDataBase;
	int ret=ERROR_SUCCESS;
	UIWorkspaceWindow wnd;
	Sleep(1000); //Xtra safety
	wnd.ActivateDataPane();
	CollapseObjects(-1); //collapse the whole tree
	LOG->RecordInfo("Forcing the Login dialog to validate pasword");
	Sleep(1000); //Xtra safety
	MST.DoKeys("{Left}{Left}{Left}{Right}"); //expand the datasources
	Sleep(1000); //Xtra safety
	//TODO this sequence can be changed
	MST.DoKeys(DataBase,TRUE);
	MST.DoKeys(" (",TRUE);
	MST.DoKeys(ServerName,TRUE);
		
	MST.DoKeys("%({Enter})"); //bring up the properties

	if(pDataInfo->m_strPassWord.GetLength()!=0 &&
		m_projOpen<0)
	{
		BOOL bResult=m_prj.ValidatePassword(
		pDataInfo->m_strUser,
		pDataInfo->m_strPassWord,
		pDataInfo->m_strDataBase);
		if(bResult==ERROR_SUCCESS)
			m_projOpen=1;
		else
			return ERROR_ERROR;
	}

	BOOL bReady = FALSE;
	int nSec = 5;
	while (!bReady && ret == ERROR_SUCCESS)
	{
			// Attempt to open the property page for the Data Source
			

			CString str;
			MST.WGetText(NULL, str);
	//		if(str.Find("Properties")>0)
	//			nSec=2; //properties window is up, but XTRA safety required!
			if (MST.WStaticExists(ServerName) && MST.WStaticExists(DataBase))
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
	MST.DoKeys("{Escape}"); //get rid of property window
	wnd.ActivateDataPane(); //workspace loses focus when password validated
	return ret;		
} //ForcePassword

int CDataSubSuite::CheckObjectProp(CObjectInfo* pMember)
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
//TODO add check for the object type (remember that COLUMN type is not introduced into GetTypeMember() yet)
		char name[256];
		strncpy(name, pMember->GetName(), 256);
		if(pMember->m_Type==CObjectInfo::TRIGGER)
		{
			strtok(name,";");
			char *pname=strtok(NULL,";");
			if(pname)
				strcpy(name,pname);
		}
		if (MST.WStaticExists(name))
//TODO add when applicable for each object	&& MST.WStaticExists(pMember->GetTypeName())) 
		{
			bFound = TRUE;
		}
		else
		{
			if (--nSec > 0)	// If we still have some waiting time...
			{
			GetLog()->RecordInfo("Waiting up to %d more second(s) for properties of %s...", 
				nSec,pMember->GetName());
			Sleep(1000);
			}
			else				// We've timed out.
			{		
				GetLog()->RecordFailure(
					"Timeout waiting for properties of %s name: '%s'",
					pMember->GetTypeName(),pMember->GetName());
				ret = ERROR_ERROR;
			}
		}
	} //while
	MST.DoKeys("{Escape}"); // close the property page
	return ret;
} //CheckObjectProp


void CDataSubSuite::ExpandProcedureNode(CString strDBName)
{
//	ExpandDatabase(strDBName);
	MST.DoKeys(GetLocString(IDSS_STOREDPROC)); //"Stored Procedures"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the Table node
}


void CDataSubSuite::ExpandTableNode(CString strDBName)
{
//	ExpandDatabase(strDBName);
	MST.DoKeys(GetLocString(IDSS_TABLE)); // "Table"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the Table node
}

void CDataSubSuite::ExpandDiagramsNode(CString strDBName)
{
//	ExpandDatabase(strDBName);
	MST.DoKeys(GetLocString(IDSS_DIAGRAM)); // "Database Diagrams"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the Table node
}


void CDataSubSuite::ExpandViewNode(CString strDBName)
{
//	ExpandDatabase(strDBName);
	MST.DoKeys(GetLocString(IDSS_VIEW)); // "View"
	Sleep(2000);						// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");			// Expand the Table node
}

void CDataSubSuite::CollapseObjects(int isTrigger)
{
	if(isTrigger>0)
	{
		MST.DoKeys("{Left}"); // goto the table
		MST.DoKeys("{NUMPAD-}");// collapse table tree
	}
	if(isTrigger>=0)
	{
		MST.DoKeys("{NUMPAD-}"); // collapse the expanded object
		MST.DoKeys("{Left}"); // goto the parent node
		MST.DoKeys("{NUMPAD-}");// collapse objects' tree
	}
	else 
	{ //collapse the whole tree
		MST.DoKeys("{Home}");
		MST.DoKeys("{NUMPAD-}");
	}
}

void CDataSubSuite::ExpandTable(CString strTableName)
{
//	ExpandDatabase(strDBName);
	ExpandTableNode("");
	MST.DoKeys(strTableName);
	Sleep(2000); 			// Wait so next key isn't included in search
	MST.DoKeys("{NUMPAD+}");
}

BOOL CDataSubSuite::SelectDatabase(CString strDBName)
{
	if (1 /*!ActivateDataPane()*/) {
		m_pLog->RecordFailure("Couldn't activate the DataView");
		return FALSE;
	}

	MST.DoKeys("^{HOME}");				// Go to top of tree
	MST.DoKeys(strDBName);				// Select database
	Sleep(2000);						// Wait so next key isn't included in search
	
	return TRUE;
}

BOOL CDataSubSuite::RefreshDatabase(CString strDBName)
{
	if (1/*!ActivateDataPane()*/) {
		m_pLog->RecordFailure("Couldn't activate the DataView");
		return FALSE;
	}

	SelectDatabase(strDBName);
	MST.DoKeys("+{F10}");				// Shift+F10 brings up context menu
	Sleep(1000);

  	MST.DoKeys("{HOME}{ENTER}"); //usually, the refresh is the first command

	return TRUE;
}


//void CTablesTest::ExpandDatabase(CString strDBName)
//{
//	SelectDatabase(strDBName);
//	MST.DoKeys("{NUMPAD+}");
//}


///////////////////////////////////////////////////////////////////
// Helper functions to provide the data from the outer filename
//
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
    m_strDataBase.Empty();
	m_strDSN.Empty();
	m_strServer.Empty();
	m_strUser.Empty();
	m_strPassWord.Empty();
    
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
}

void CDataInfo::GetDataInfo(CString &DirName,CString &FileName)
{
    CStdioFile fileVerify(DirName+FileName, CFile::modeRead | CFile::typeText);
  	const int len=256;
	char buf[len];
	CString str("");

	// DataBase info comes first.
	if(fileVerify.ReadString(buf, len)==0)
	{
		LOG->RecordFailure("No SQL.BAS file found");
		return; //error
	}
	m_strServer=strtok(buf," ");
	m_strDataBase=strtok(NULL," ");
	m_strUser=strtok(NULL," ");
	m_strPassWord=strtok(NULL," ");
	TrimString(&m_strServer);
	TrimString(&m_strDataBase);
	TrimString(&m_strUser);
	TrimString(&m_strPassWord);
	if(m_strServer.Find("DSN=")==0)
	{
		m_strServer=m_strServer.Right(m_strServer.GetLength()-4);
		m_strDSN=DirName + m_strServer + ".DSN";
	}
	else
		m_strDSN="Test"+m_strServer;

    // Now get rest of information, if any.
    CString strDelimiter("***");
    BOOL bDone = FALSE;

    while (!bDone && fileVerify.ReadString(buf, len))
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
				if(str.Find("DIAGRAM") == 0)
				{	pNew->m_Type=CObjectInfo::DIAGRAM;}
				if(str.Find("PROC") == 0)
				{	pNew->m_Type=CObjectInfo::PROC;}
				if(str.Find("TRIGGER") == 0)
				{	pNew->m_Type=CObjectInfo::TRIGGER;}
				if(str.Find("ADHOC") == 0)
				{	pNew->m_Type=CObjectInfo::ADHOC;}
				if(str.Find("NEWPROC") == 0)
				{	pNew->m_Type=CObjectInfo::NEWPROC;}
				strtok(buf, " "); // TABLE or VIEW or PROC or any valid object
				str=strtok(NULL, ",\n");
				pNew->m_strName	= str;
				TrimString(&pNew->m_strName);
				str=strtok(NULL, ",\n");
				pNew->m_strParams=str;
				TrimString(&pNew->m_strParams);
				str=strtok(NULL, ",\n");
				pNew->m_strOwner=str;
				TrimString(&pNew->m_strOwner);
				m_listMembers.AddTail(pNew);
			}
        }
    }
} //GetDataInfo


// EOF - sniff.cpp
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
#include <io.h>
#include "syscases.h"
#include "bindcase.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;




///////////////////////////////////////////////////////////////////////////////
//	CSourceTestCases class
	// shared by NewProject(), InsertProject(), and NewAppWiz().
int OpenSQLDataSource(CDataInfo *pDataInfo);


IMPLEMENT_TEST(CBindBuildTest, CTest, "Build & test dlg", -1, CBindSubSuite)

BOOL CBindBuildTest::RunAsDependent(void)
{
	return FALSE; //bogus code below
	// name of the EXE file
	CString strEXEFile = m_strCWD + PROJDIR + PROJNAME + ".EXE";

	// if the EXE file exists, there's no need to run this test as a dependency
	if (_access(strEXEFile, 0) != -1) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

void CBindBuildTest::Run(void)
{
	OpenProject();
	BuildProject();
	RunTestMode();
}

BOOL CBindBuildTest::OpenProject(void)
{
	BOOL bResult;

	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	// set the project's filename
	CString strProjFile = m_strCWD + PROJDIR + PROJNAME + ".DSW";

	// open the project
	GetLog()->RecordInfo("Opening the project file '%s'...", (LPCSTR)strProjFile);

	// if the project's already open, no need to open it again
	if(GetSubSuite()->m_projOpen)
	{
		GetLog()->RecordSuccess("The project '%s' is already open.", (LPCSTR)(pPrj->GetFullPath()));
		return TRUE;
	}
	else {
		// open the project
		bResult = GetLog()->RecordCompare((pPrj->Open(strProjFile, 0) == ERROR_SUCCESS), "Opening the project file '%s'.", (LPCSTR)strProjFile);
		if (!bResult) {
			throw CTestException("CSysBuildTest::OpenProject: cannot open project; unable to continue.", CTestException::causeOperationFail);
		}
		return bResult;
	}
}

BOOL CBindBuildTest::BuildProject(void)
{
	BOOL bResult;

	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	LOG->RecordInfo("Building project...");

	// build the project
	LOG->RecordCompare(pPrj->Build( 60 ) == ERROR_SUCCESS, "Building project.");


	LOG->RecordInfo("Verifying the build...");

	// verify the build
	bResult = LOG->RecordCompare(pPrj->VerifyBuild() == ERROR_SUCCESS, "Verifying the build.");
	if (!bResult) {
		throw CTestException("CBindBuildTest::BuildProject: cannot build a container project; unable to continue.", CTestException::causeOperationFail);
	}
	return bResult;
}

void CBindBuildTest::RunTestMode(void)
{
//TODO: put this test mode function into Suite
	if(!SetFocusToResSym(m_DialogId /*"IDD_DIALOG1"*/))
		return; //FALSE 
	
	UIWB.DoCommand(IDMY_TEST, DC_MNEMONIC);
	WaitForInputIdle(g_hTargetProc, 50000); //wait for an eventual timeout lor login
	UIDialog uTestDlg;
	uTestDlg.WaitAttachActive(3000);
	char acBuf[256];
	uTestDlg.GetText(acBuf, 255); 
//TODO what to check for while running the test mode?
	MST.DoKeys("{ESC}");
	uTestDlg.WaitUntilGone(3000);
}

///////////////////////////////////////////////////////////////////////////////
//	CPubsTablesTest class
IMPLEMENT_TEST(CPubsTablesTest, CTest, "Bind DBase & test", -1, CBindSubSuite)
void CPubsTablesTest::Run(void)
{
	CDataInfo *pDataInfo=GetSubSuite()->m_DataInfo+0;
	CString strTable,strColumn;

//	GetSubSuite()->PropertySet(controldata[8].ctrlID, //MY_FlexGrid //bug FlexGrid stops RDC from working
//		"DataSource",controldata[2].ctrlID, METHOD_LIST); //MY_RDC
	GetSubSuite()->PropertySet(controldata[7].ctrlID, //MY_DBGrid
		"DataSource",controldata[2].ctrlID, METHOD_LIST); //MY_RDC
	GetSubSuite()->PropertySet(controldata[5].ctrlID, //MY_MASKEDIT
		"DataSource",controldata[2].ctrlID, METHOD_LIST); //MY_RDC
	GetSubSuite()->PropertySet(controldata[6].ctrlID, //MY_RICHEDIT
		"DataSource",controldata[2].ctrlID, METHOD_LIST); //MY_RDC

	POSITION pos = pDataInfo->m_listMembers.GetHeadPosition();
	while(pos != NULL)
	{
		strTable=pDataInfo->m_listMembers.GetAt(pos)->GetName();
		strColumn=pDataInfo->m_listMembers.GetAt(pos)->GetColumn();
		LOG->RecordInfo("Setting the table of control: %s as: %s",
			controldata[2].ctrlID/*MY_RDC*/, strTable);
		SetSource(controldata[2].ctrlID, //MY_RDC
			pDataInfo->m_strConnect,
			pDataInfo->m_strUser,
			pDataInfo->m_strPassWord,
			strTable);
//SetColumns:
		GetSubSuite()->PropertySet(controldata[5].ctrlID, //MY_MASKEDIT
			"DataField",strColumn, METHOD_LIST);
		GetSubSuite()->PropertySet(controldata[6].ctrlID, //MY_RICHEDIT
			"DataField",strColumn, METHOD_LIST);
		//TODO: put this test mode function into Suite
		if(!SetFocusToResSym(m_DialogId /*"IDD_DIALOG1"*/))
			return; //FALSE 
	
		UIWB.DoCommand(IDMY_TEST, DC_MNEMONIC);
		WaitForInputIdle(g_hTargetProc, 50000); //wait for an eventual timeout lor login
		UIDialog uTestDlg;
		uTestDlg.WaitAttachActive(3000);
		char acBuf[256];
		uTestDlg.GetText(acBuf, 255); 
		Sleep(2000);//TODO what to check for while running the test mode?
		MST.DoKeys("{ESC}");
		uTestDlg.WaitUntilGone(3000);
		//////////////////////////////////////////////////////
		
		pDataInfo->m_listMembers.GetNext(pos);
	} //while
}//CPubsTablesTest::Run


/////////////////////////////////////////////////////////////////////////
// Sets the SQL property as "select * from TableName" on RDC control
//
// RdcID:		ID of the RDC
// pDataInfo:	Datasource info
// TableName:	table in question
/////////////////////////////////////////////////////////////////////////
int CPubsTablesTest::SetSource(LPCSTR RdcID,LPCSTR szConnect,LPCSTR szUser,LPCSTR szPWD,
							  LPCSTR TableName)
{

	int bResult;
	MST.DoKeys("%(Vy)",FALSE,5000); //View.Symbols (wait for idle time
	//	if(MST.WListItemExists(cntlrID))
	MST.WListItemClk(GetLabel(0x6814),RdcID); 
	MST.WButtonClick(GetLabel(0x681d)); // View use of the RDC
	
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	if((bResult=prop.IsValid())==FALSE)
	{
		LOG->RecordFailure("Cannot access the property of control: '%s'",RdcID);
	}
	else
	{
		if(prop.GetID()!=RdcID)
		{
			bResult=ERROR_ERROR;
			LOG->RecordFailure("This is not the control: %s",RdcID);
		}
	}
	if(bResult==ERROR_ERROR)
	{
		prop.Close(); //get rid of the properties
		return ERROR_ERROR;
	}
	if(prop.SetAllPage()==FALSE)
		return ERROR_ERROR;
//	prop.putProperty("DataSourceName",pDataInfo->m_strSource,METHOD_LIST);
	prop.putProperty("Connect",szConnect);
	prop.putProperty("UserName",szUser);
	prop.putProperty("Password",szPWD);
	prop.putProperty("SQL",CString("Select * from ")+CString(TableName));
	prop.Close();
	return ERROR_SUCCESS;
} // CAddControlTest::SetSource



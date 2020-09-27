///////////////////////////////////////////////////////////////////////////////
//	ATLWizCase.CPP
//
//	Created by :			Date :
//		ChrisKoz					1/28/98
//
//	Description :
//		Implementation of the CSysATLWizCase class

#include "stdafx.h"
#include "ATLWizCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CSysATLWizCase, CTest, "System ATLWiz", -1, CSysATLWizSubsuite)




void CSysATLWizCase::LogResult(int line, int result,LPCSTR szoperation, int code /* 0 */, LPCSTR szComment /* "" */)
{
	CString szOpCom = (CString)szoperation + "  " + szComment;
	if(result == PASSED)
		m_pLog->RecordSuccess("%s", szOpCom);
	else
	{
		CString szextra;
		char chbuf[24];
		szextra = szextra + "Error Code = " + itoa(code, chbuf, 10);
		m_pLog->RecordFailure("LINE %d %s", line, szextra + szOpCom);
	}
}
#define EXPECT_TRUE(f)    ((f)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f));
#define EXPECT_SUCCESS(f) (((ReturnCode=(f))==0)?LogResult(__LINE__, PASSED,#f):LogResult(__LINE__, FAILED,#f,ReturnCode));
#define EXPECT_TRUE_COMMENT(f,comment) ((f)?LogResult(__LINE__, PASSED,#f,0,#comment):LogResult(__LINE__, FAILED,#f,0,#comment));



void CSysATLWizCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

//TODO: later move it ot coprjct.cpp
int CSysATLWizCase::InsertDialogATLObject(LPCSTR szObjectName)
{
	// TODO(michma): the ATL Object Wizard needs its own UI class with full functionality. 
	UIWB.DoCommand(IDM_NEW_ATL_OBJECT, DC_MNEMONIC);

 	// wait up to 3 seconds for the wizard dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_ATL_OBJ_WIZ_TITLE), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! COProject::InsertDialogATLObject - ATL Object Wizard dlg not found");
		return ERROR_ERROR;
	}

//select category
	MST.WListItemClk("@1", "Miscellaneous");
	WaitForInputIdle(g_hTargetProc, 5000); //the ListView to the right must update
//select the object type
	if(MST.WViewItemExists("@1", "Dialog")==FALSE)
	{
		LOG->RecordInfo("ERROR! COProject::InsertDialogATLObject - Dialog item not found in the list");
		return ERROR_ERROR;
	}
	MST.WViewItemClk("@1", "Dialog");
	// simply hit ENTER for next button for now.
	MST.DoKeys("{ENTER}");
	
	
	// wait up to 3 seconds for the wizard properties dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_ATL_OBJ_WIZ_PROP_TITLE), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! COProject::InsertDialogATLObject - ATL Object Wizard Properties dlg not found");
		return ERROR_ERROR;
	}
	
	// simply enter the shortname of the object for now.
	MST.DoKeys(szObjectName);
	// finish the wizard.
	MST.DoKeys("{ENTER}");

	// wait around a few seconds in case we need to confirm using an existing class id.
	if(MST.WFndWndWaitC(GetLocString(IDSS_USE_EXISTING_CLASS_ID), "Static", FW_PART, 5))
		MST.DoKeys("{ENTER}");

 	// wait up to 10 seconds for the wizard properties dlg to go away. 
	if(!MST.WFndWndWait(GetLocString(IDSS_ATL_OBJ_WIZ_PROP_TITLE), FW_NOEXIST | FW_PART, 10))
	{
		LOG->RecordInfo("ERROR! COProject::InsertDialogATLObject - ATL Object Wizard Properties dlg didn't go away");
		return ERROR_ERROR;
	}

	return ERROR_SUCCESS;
}



#define IDSS_MEMBER_FUNCTION	"Add Member Function"
#define IDSS_ATL_MESSAGES_TITLE "New Windows Message and Event Handlers for class "
int CSysATLWizCase::InsertATLMessageHandler(LPCSTR szMessageName, LPCSTR szClasName, LPCSTR szProjectName /*=NULL*/)
{
	UIWorkspaceWindow wnd;

	int ret=ERROR_SUCCESS;
	wnd.ActivateClassPane();
	if(wnd.SelectClass(szClasName, szProjectName)!=SUCCESS)
		return FALSE;
	
	MST.DoKeyshWnd(UIWB.HWnd(), "+{F10}");
	Sleep(1000); //xtra safety
	MST.DoKeyshWnd(UIWB.HWnd(), "{Home}");
	
	MST.WMenu("&" + GetLocString(IDSS_WIZBAR_HANDLER));
	WaitForInputIdle(g_hTargetProc, 5000); //the dialog comes up slowly
	EXPECT_TRUE(MST.WFndWndWait(IDSS_ATL_MESSAGES_TITLE, FW_FOCUS | FW_PART, 5) );

	MST.WListItemClk("@3", szClasName);
	MST.WListItemClk("@1",szMessageName);
	MST.WButtonClick(GetLabel(0x639F)); //"Add and Exit"

	EXPECT_TRUE(MST.WFndWndWait(IDSS_MEMBER_FUNCTION, FW_NOEXIST, 10));
	EXPECT_TRUE(MST.WFndWndWait(IDSS_ATL_MESSAGES_TITLE, FW_NOEXIST | FW_PART, 10) );

	WaitForInputIdle(g_hTargetProc, 10000); //the dialog goes away slowly
	return ERROR_SUCCESS;
}


//GetLocString(IDSS_ATL_MESSAGES_TITLE)
int CSysATLWizCase::InsertATLEventHandler(LPCSTR szMessageName, LPCSTR szClasName, LPCSTR szObjectName, LPCSTR szProjectName /*=NULL*/)
{
	UIWorkspaceWindow wnd;

	int ret=ERROR_SUCCESS;
	wnd.ActivateClassPane();
	if(wnd.SelectClass(szClasName, szProjectName)!=SUCCESS)
		return FALSE;
	
	MST.DoKeyshWnd(UIWB.HWnd(), "+{F10}");
	Sleep(1000); //xtra safety
	MST.DoKeyshWnd(UIWB.HWnd(), "{Home}");
	
	MST.WMenu("&" + GetLocString(IDSS_WIZBAR_HANDLER));
	WaitForInputIdle(g_hTargetProc, 5000); //the dialog comes up slowly
	EXPECT_TRUE(MST.WFndWndWait(IDSS_ATL_MESSAGES_TITLE, FW_FOCUS | FW_PART, 5) );
	
	MST.WListItemClk("@3", szObjectName);
	MST.WListItemClk("@1",szMessageName);
	MST.WButtonClick(GetLabel(0x639F)); //"Add and Exit"
	
	EXPECT_TRUE(MST.WFndWndWait(IDSS_MEMBER_FUNCTION, FW_FOCUS, 5) );
	EXPECT_TRUE(MST.WStaticExists(szObjectName) );
	MST.WButtonClick(GetLabel(IDOK));
 	// wait up to 10 seconds for the Messages dlg to go away. 
	EXPECT_TRUE(MST.WFndWndWait(IDSS_MEMBER_FUNCTION, FW_NOEXIST, 10));
	EXPECT_TRUE(MST.WFndWndWait(IDSS_ATL_MESSAGES_TITLE, FW_NOEXIST | FW_PART, 10) );

	WaitForInputIdle(g_hTargetProc, 10000); //the dialog goes away slowly
	return ERROR_SUCCESS;
}


void CSysATLWizCase::Run(void)
{	 
	COProject prj;
	COSource src;

	// TODO(ChrisKoz): internationalize the project name.
	CString strProjName = "ATLWizard Project";
	CString strObjectName = "DlgObject";
	
	KillAllFiles(GetCWD() + strProjName, TRUE);//It does not need to be in CSubSuite::Cleanup()
//create the ATL project
	// set the name and location of the atl com server project. we also want the project to be added to
	// the current workspace.
	CATLCOMWizOptions *pATLCOMWizOpt = new(CATLCOMWizOptions);
	pATLCOMWizOpt->m_acstServerType = CATLCOMWizOptions::ATL_COM_SERVER_EXE;
	pATLCOMWizOpt->m_bSupportMFC = TRUE;//just for the heck of it
	pATLCOMWizOpt->m_strLocation = GetCWD();
	pATLCOMWizOpt->m_strName = strProjName;	
	pATLCOMWizOpt->m_pwWorkspace = ADD_TO_CURRENT_WORKSPACE;
//	pATLCOMWizOpt->m_phHierarchy = SUB_PROJ;
//	pATLCOMWizOpt->m_strParent = m_strAppProjName; 
	
	// create the activex control project.
	LOG->RecordCompare(prj.New(pATLCOMWizOpt) == ERROR_SUCCESS, 
		"Create ATL COM Server project named: %s", pATLCOMWizOpt->m_strName);


// select the ATL project so we can add a new object to it.
	prj.SetTarget((CString)strProjName + " - " + 
					GetLocString(IDSS_PLATFORM_WIN32) + " " +
					GetLocString(IDSS_BUILD_DEBUG));

//add the Dialog ATL object
	LOG->RecordCompare(InsertDialogATLObject(strObjectName) == ERROR_SUCCESS, 
		"Insert Dlg object '%s' into '%s' project.", strObjectName, strProjName);

//add the ActiveX control to the dialog here, as we don't know the name of a dialog, so it's hard to navigate the reesource pane to see it
//select the name 'szCtrlName' in porperties		
	CString strCtrlName = "DBGrid  Control";
	CString strCtrlID = "IDC_DBgrid";
	
	LOG->RecordInfo("Inserting '%s' ActiveX control into the dialog", strCtrlName);
	MST.DoKeys("+{F10}");				// Shift+F10 brings up context menu
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(500);
	MST.DoKeys("X"); //insert ActiveX control

	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(1000);
	CString listlabel=GetLabel(0x6ad6)/* OLE_CONTROL_LIST*/;
	int nelem=MST.WListCount(listlabel);
	if(MST.WListItemExists(listlabel,strCtrlName))
		MST.WListItemClk (listlabel,strCtrlName);
	else
	{
		LOG->RecordFailure("'%s' item does not exist in list '%s'. Clicked the first one", strCtrlName, listlabel);
		MST.WListItemClk (listlabel,1);
	}
	MST.WButtonClick(GetLabel(IDOK)); //OLE control chosen
		
	if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
	{
		LOG->RecordFailure("ActiveX control hanged...");
		return /* ERROR_ERROR*/;
	}
	Sleep(2000); //bug DBgrid GPFaults if I do not insert the sleep here

//bring the property of inserted control & set it to desired text
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	BOOL bResult;
	if((bResult=prop.IsValid())==FALSE)
	{
		LOG->RecordFailure("Cannot access the property of control: '%s'",strCtrlName);
	}
	prop.SetID(strCtrlID);
	prop.Close();	
	
	
	LOG->RecordCompare(InsertATLEventHandler("Click", "C" + strObjectName, strCtrlID)==ERROR_SUCCESS,
		"Insert event handler");

	
	LOG->RecordCompare(InsertATLMessageHandler("WM_PAINT", "C" + strObjectName)==ERROR_SUCCESS,
		"Insert message handler");

	// build the project.
	LOG->RecordCompare(prj.Build() == ERROR_SUCCESS, "Building '%s' project.", strProjName);
	
	// verify no errors or warnings were generated.
	int iErr, iWarn;
	LOG->RecordCompare(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS,
		"Verifying build of '%s' project.", strProjName);


}

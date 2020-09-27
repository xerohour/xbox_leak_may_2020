///////////////////////////////////////////////////////////////////////////////
//      SYSCASES.CPP
//
//      Created by :                    Date :
//              ChrisKoz                                  1/25/96
//
//      Description :
//              Implementation of the creation & classview in Galileo project
//

#include "stdafx.h"
#include <io.h>
#include "syscases.h"
#include "sniff.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;



///////////////////////////////////////////////////////////////////////////////
// CContTest: create the MDI MFC container & save it
IMPLEMENT_TEST(CContTest, CTest, "Create Container App", -1, CBindSubSuite)

BOOL CContTest::RunAsDependent(void)
{
	return FALSE; //bogus code below
	// name of the project file
	CString strProjFile = m_strCWD + PROJDIR + PROJNAME + ".DSW";

	// if the project file exists, there's no need to run this test as a dependency
	if (_access(strProjFile, 0) != -1) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

void CContTest::Run(void)
{
	NewProject();
	SaveProject();
}

BOOL CContTest::NewProject(void)
{
	BOOL bResult;
	
	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	GetLog()->RecordInfo("Creating an AppWizard project named '%s'...", PROJNAME);

	// create a new AppWizard project
	APPWIZ_OPTIONS options;
	options.m_bUseMFCDLL = CMDLINE->GetBooleanValue("MFCDLL", FALSE); 

	// The following command line switch is mapped as follows:
	//	0 - APPWIZ_OPTIONS::AW_OLE_NONE (Default)
	//  1 - APPWIZ_OPTIONS::AW_OLE_CONTAINER
	//  2 - APPWIZ_OPTIONS::AW_OLE_MINISERVER (not supported)
	//  3 - APPWIZ_OPTIONS::AW_OLE_FULLSERVER
	//  4 - APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER
	options.m_OLECompound = (APPWIZ_OPTIONS::OLESupport)CMDLINE->GetIntValue("OLE", 1);
	ASSERT(options.m_OLECompound < 5);

	switch(options.m_OLECompound)
	{
	case APPWIZ_OPTIONS::AW_OLE_NONE:
		GetLog()->RecordInfo("No OLE options selected.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_CONTAINER:
		GetLog()->RecordInfo("Creating an OLE container application.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_MINISERVER:
		GetLog()->RecordFailure("Creating an OLE mini-server application is not supported by the System sniff.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_FULLSERVER:
		GetLog()->RecordInfo("Creating an OLE full-server application.");
		break;
	case APPWIZ_OPTIONS::AW_OLE_CONTAINERSERVER:
		GetLog()->RecordInfo("Creating an OLE container/server application.");
		break;
	default:
		GetLog()->RecordInfo("Invalid OLE option specified: %d  Defaulting to no OLE support", options.m_OLECompound);
		options.m_OLECompound = APPWIZ_OPTIONS::AW_OLE_NONE;
		break;
	}

	bResult = GetLog()->RecordCompare
		(pPrj->NewAppWiz(PROJNAME, GetCWD(), GetUserTargetPlatforms(), &options) 
		== ERROR_SUCCESS, "Creating an AppWizard project named '%s'.", PROJNAME);
	if (!bResult) {
		throw CTestException("CContTest::NewProject: cannot create a new AppWizard project; unable to continue.", CTestException::causeOperationFail);
	}


	// change output file location to same as project file (avoid debug, macdbg, pmcdbg, etc).
//	bResult = LOG->RecordCompare(pPrj->SetOutputFile((CString)PROJNAME + ".Exe") == ERROR_SUCCESS, "Resetting output file location.", PROJNAME);
//	if (!bResult) {
//		throw CTestException("CContTest::NewProject: cannot reset output file location; unable to continue.", CTestException::causeOperationFail);
//	}

	return bResult;
}


BOOL CContTest::SaveProject(void)
{
	// get the project from the subsuite
	COProject* pPrj = &(GetSubSuite()->m_prj);

	CString strPrjName = pPrj->GetFullPath();

	GetLog()->RecordInfo("Saving the AppWizard project. Expecting the file '%s' to be written...", strPrjName);

	// save the project
	pPrj->Close(TRUE); GetSubSuite()->m_projOpen=0;
	return 0; // the bogus code below screws up the pPrj state
//	return GetLog()->RecordCompare(
//		pPrj->Save(TRUE, strPrjName) == ERROR_SUCCESS,
//		"Saving the AppWizardProject.");
}

///////////////////////////////////////////////////////////////////////////////
// CAddControlTest: creates the new dialog resource, adds controls to the dialogs
IMPLEMENT_TEST(CAddControlTest, CTest, "Add DBind Controls", -1, CBindSubSuite)


void CAddControlTest::Run(void)
{
	// if(!GetSubSuite()->m_prj.IsOpen())  this does not work
	if(!GetSubSuite()->m_projOpen)
	{
		CString strPrjName=m_strCWD + PROJDIR + PROJNAME + ".DSW";
		if(GetSubSuite()->m_prj.Open(strPrjName)!=ERROR_SUCCESS)
			GetLog()->RecordFailure("Coudn't open project - %s",strPrjName);
		else
			GetSubSuite()->m_projOpen=1;
	}
	int bResult=CreateResource("Dialog");
	if(!LOG->RecordCompare(bResult == ERROR_SUCCESS, 
					"Adding the dialog to the project"))
		throw CTestException("Cannot create the dialog; unable to continue.", 
				CTestException::causeOperationFail);

	for(int i=0;i<sizeof(controldata)/sizeof(*controldata);i++)
	{
		LOG->RecordInfo("Adding the control: %s",controldata[i].ctrlID);
		AddOleControl(controldata[i].ctrlName, controldata[i].ctrlID, &controldata[i].pos);
		LOG->RecordInfo("Bringing All PP of control: %s", controldata[i].ctrlID);
		UIControlProp prop = UIWB.ShowPropPage(TRUE);
		EXPECT_EXEC(prop.IsValid(),"Failed to show property page");
		if(prop.SetAllPage()==FALSE)
			LOG->RecordFailure("Cannot bring the all property page");
		prop.Close();
	}

	GetSubSuite()->PropertySet(controldata[2].ctrlID, //MY_RDC
		"Caption","Myrdc2.0");
	GetSubSuite()->PropertySet(controldata[6].ctrlID, //MY_RICHEDIT
		"Text","MyRich5.0");
	GetSubSuite()->PropertySet(controldata[7].ctrlID, //MY_DBGrid
		"Caption","Hey, Apex is cool!");
	
	UIWB.DoCommand(IDM_FILE_SAVE_ALL, DC_MNEMONIC);
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
	//saving the project
	COProject* pPrj = &(GetSubSuite()->m_prj);

	CString strPrjName = pPrj->GetFullPath();

	GetLog()->RecordInfo("Saving the project...");

	// save the project
	pPrj->Close(TRUE); GetSubSuite()->m_projOpen=0;
	return;

}//CAddControlTest::Run


////////////////////////////////////////////////////////////////////////////
// Creates the new resource of type szResName
// At exit, the resource editor ewindow should be opened
////////////////////////////////////////////////////////////////////////////
int CAddControlTest::CreateResource( LPCSTR szResName )
{
	UIWB.DoCommand(IDM_RESOURCE_NEW, DC_MNEMONIC);
	if( MST.WFndWndWait(GetLocString(IDSS_NEW_RES_TITLE), FW_PART, 2) == 0 )	// make sure the dialog comes up
		return ERROR_ERROR;		
	MST.DoKeys(szResName);				// Type Name of resource .... 
	if (!MST.WButtonEnabled(GetLabel(IDOK)))  // Timeout supposedly defaults to 5 sec.
	{
		LOG->RecordFailure("Timeout waiting for OK button on New Resource Dialog to become enabled.");
		return ERROR_ERROR;
	}

	MST.WButtonClick(GetLabel(IDOK));

	if( MST.WFndWndWait(szResName, FW_PART, 2)  == 0 ) // Wait for the editor to come up and verify the Caption
		return ERROR_ERROR;
	m_DialogId=UIWB.GetProperty(P_ID); //store the ID of the dialog
	MST.DoKeys("+{DOWN 100}"); //resize the dialog
	MST.DoKeys("{TAB}");
	while(UIWB.GetProperty(P_ID)!=m_DialogId)
	{
		MST.DoKeys("{Del}"); //delete all controls
	}
	return ERROR_SUCCESS;
}


//sets the property of the control
// tries to set the property using METHOD_LIST.
// METHOD_EDIT is used if the list isempty
int CBindSubSuite::PropertySet(LPCSTR cntlrID,LPCSTR PropName,LPCSTR Value, int METHOD /*=METHOD_EDIT*/)
{
	LOG->RecordInfo("Setting (%s) control's (%s) property to: (%s)",cntlrID,PropName,Value);
	MST.DoKeys("%(Vy)",FALSE,5000); //View.Symbols (wait for idle time
//	if(MST.WListItemExists(cntlrID))
	MST.WListItemClk(GetLabel(0x6814),cntlrID); 
	MST.WButtonClick(GetLabel(0x681d));
	Sleep(3000); //bug it does not hit the use of DBGrid
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	EXPECT(UIWB.GetProperty(P_ID)==cntlrID);
	EXPECT(prop.IsValid());
	if(prop.putProperty(PropName,Value,METHOD)<0 && METHOD==METHOD_LIST &&
		prop.putProperty(PropName,Value,METHOD_EDIT)==0) //try METHOD_EDIT if METHOD_LIST failed
	{
		LOG->RecordFailure("Set the property (%s) of the control (%s) to '%s'",PropName,cntlrID,Value);
	}
	prop.Close(); //get rid of the properties
	return ERROR_SUCCESS;
}//CAddControlTest::PropertySet()


////////////////////////////////////////////////////////////////////
// Adds the OLE control to the current dialog 
// sets its ID to ID_Name
// Assumes that the dialog is currently opened
// Returns: ERROR_SUCCESS or ERROR_ERROR
////////////////////////////////////////////////////////////////////
int CAddControlTest::AddOleControl(LPCSTR Name,LPCSTR ID_Name,
								   RECT *position /*=NULL*/)
{
	int cnt=100; //never more that 100 controls
	while(cnt-->0)
	{
		if(UIWB.GetProperty(P_ID)==m_DialogId)
			break; //found the dialog ID
		MST.DoKeys("{Tab}");
	}
	if(cnt<=0)
	{
		LOG->RecordFailure("Cannot reach the dialog '%s' context",m_DialogId);
		return ERROR_ERROR;
	}

	MST.DoKeys("+{F10}");				// Shift+F10 brings up context menu
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(500);
	MST.DoKeys("X"); //insert ActiveX control

	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	CString listlabel=GetLabel(0x6ad6)/* OLE_CONTROL_LIST*/;

	Sleep(1000);
	int nelem=MST.WListCount(listlabel);
	LOG->Comment("Found (%d) elements in ActiveX list",nelem);

	if(MST.WListItemExists(listlabel,Name)==0)
	{	
		LOG->RecordFailure("Coudn't find OLE control: %s",Name);
		MST.WButtonClick(GetLabel(IDCANCEL));
		return ERROR_ERROR;
	}
	MST.WListItemClk(listlabel,Name);
	MST.WButtonClick(GetLabel(IDOK)); //OLE control chosen
	if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
	{
		LOG->RecordFailure("ActiveX control hanged...");
		return ERROR_ERROR;
	}
	Sleep(2000); //bug DBgrid GPFaults if I do not insert the sleep here
// Popup the property page for the currently active item.
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	if(prop.IsValid()==FALSE)
	{
		LOG->RecordFailure("Coudn't find the control '%s' properties",Name);
		MST.DoKeys("{Esc}"); //get rid of the properties
		return ERROR_ERROR;
	}
	prop.SetID(ID_Name);
	prop.Close();
	if(position==NULL)
		return ERROR_SUCCESS;

	
		//Move the control
	long sizeHor=position->right-position->left;
	long sizeVer=position->bottom-position->top;
	CString strPos;
	strPos = UIWB.GetProperty(P_Position);
	char posbuffer[50];
	RECT current;

	char *pospointer=strtok(strncpy(posbuffer,strPos,50),",");
	current.left=atoi(pospointer);
	if(pospointer!=NULL)pospointer=strtok(NULL,",");
	current.top=atoi(pospointer);
	if(pospointer!=NULL)pospointer=strtok(NULL,",");
	current.right=atoi(pospointer);
	if(pospointer!=NULL)pospointer=strtok(NULL,",");
	current.bottom=atoi(pospointer);
	long curSizeHor=current.right-current.left;
	long curSizeVer=current.bottom-current.top;

	long moveright,movedown,sizeright,sizedown;
	moveright=position->left - current.left;
	movedown=position->top - current.top;
	sizeright=sizeHor - curSizeHor;
	sizedown=sizeVer - curSizeVer;

	while(moveright!=0 || movedown!=0 || sizeright!=0 || sizedown!=0)
	{
		char dokeys[40];
		if(moveright!=0)
		{
			sprintf(dokeys,(moveright>0?"{RIGHT %d}":"{LEFT %d}"),
				abs(moveright)/2+1);
			MST.DoKeys(dokeys); 
		}
		if(movedown!=0)
		{
			sprintf(dokeys,(movedown>0?"{DOWN %d}":"{UP %d}"),
				abs(movedown)/2+1);
			MST.DoKeys(dokeys); 
		}
		if(sizeright!=0)
		{
			sprintf(dokeys,(sizeright>0?"+{RIGHT %d}":"+{LEFT %d}"),
				abs(sizeright)/2+1);
			MST.DoKeys(dokeys); 
		}
		if(sizedown!=0)
		{
			sprintf(dokeys,(sizedown>0?"+{DOWN %d}":"+{UP %d}"),
				abs(sizedown)/2+1);
			MST.DoKeys(dokeys); 
		}
		
		CString strnextPos=UIWB.GetProperty(P_Position);
		if(strnextPos==strPos)
		{
			TRACE("Dokeys(...) didnt move the control");
			break;
		}
		strPos=strnextPos;
		pospointer=strtok(strncpy(posbuffer,strnextPos,50),",");
		current.left=atoi(pospointer);
		if(pospointer!=NULL)pospointer=strtok(NULL,",");
		current.top=atoi(pospointer);
		if(pospointer!=NULL)pospointer=strtok(NULL,",");
		current.right=atoi(pospointer);
		if(pospointer!=NULL)pospointer=strtok(NULL,",");
		current.bottom=atoi(pospointer);
		curSizeHor=current.right-current.left;
		curSizeVer=current.bottom-current.top;
		
		if(moveright>0 && current.left>=position->left ||
		   moveright<0 && current.left<=position->left)
			moveright=0;
		else
			if(moveright!=0)moveright=position->left - current.left;

		if(movedown>0 && current.top>=position->top ||
		   movedown<0 && current.top<=position->top)
			movedown=0;
		else
			if(movedown!=0)movedown=position->top - current.top;

		if(sizeright>0 && curSizeHor >= sizeHor ||
		   sizeright<0 && curSizeHor <= sizeHor)
			sizeright=0;
		else
			if(sizeright!=0)sizeright=sizeHor - curSizeHor;

		if(sizedown>0 && curSizeVer >= sizeVer ||
		   sizedown<0 && curSizeVer <= sizeVer)
			sizedown=0;
		else
			if(sizedown!=0)sizedown=sizeVer - curSizeVer;
	} //while
	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////
// Brings the properties for given control
//	TRUE=General property page
//	FALSE=All property page
// Name
//   if(NULL) - just the property is brought without any check
//   if(!NULL)- the PP is checked if the name is inside
//int CAddControlTest::BringProperties(BOOL General,LPCSTR Name /*=NULL*/)
/*{	
	MST.DoKeys("%({Enter})"); //bring up the properties
	BOOL bReady = FALSE;
	int nSec = 5;
	int ret=ERROR_SUCCESS;
	int otherPage=0;
		int idexist=-10,editexist=-10,comboexist=-10;
	
	while (!bReady && ret == ERROR_SUCCESS)
	{
		// Attempt to open the property page for the Data Source
		
		CString str;
		MST.WGetText(NULL, str);
		if(Name==NULL)
		{ //check the correct property page
			if(General &&  
			(comboexist=MST.WComboExists(GetLabel(0xde))))
			{
				LOG->RecordInfo("Found the Control General properties");
				bReady = TRUE;
			}
			if(General)
			{
				TRACE("ID Combo returnded: %d\n",comboexist);
			}
			if(!General &&
				(editexist=(MST.WComboExists(GetLabel(0x65ea)) || MST.WListExists(GetLabel(0x65e8)))))
			{
				LOG->RecordInfo("Found the Control All properties");
				bReady = TRUE;
			}
			if(!General)
			{
				TRACE("Static in ALL Property returnded: %d\n",idexist);
				TRACE("List in ALL returnded: %d\n",editexist);
				TRACE("Combo in ALL returnded: %d\n",comboexist);
			}
		}
		else
		{	//check the window title only
			if(str.Find("Properties")>=0)
			{
				bReady=TRUE;
				if(str.Find(Name)<0)
					ret = ERROR_ERROR;
			}
		}
		if(!bReady)
		{
			if (--nSec != 0)	// If we still have some waiting time...
			{
 				Sleep(1000);
			}
			else if(otherPage)		// We've timed out.
			{
				LOG->RecordInfo("Didn't find the Control properties");
				ret = ERROR_ERROR;
			}
			else
			{
				MST.DoKeys(!General?"^{PgUp}":"^{PgDn}");
				otherPage=TRUE;
				nSec=5;
			}
		}
	} //while
	return ret;		
} // BringProperties
*/
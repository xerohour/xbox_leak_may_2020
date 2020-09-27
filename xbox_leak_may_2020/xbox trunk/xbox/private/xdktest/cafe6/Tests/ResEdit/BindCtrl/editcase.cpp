//////////////////////////////////////////////////////////////////////////////
//	EDITCASE.CPP
//
//	Created by :			Date :
//		ChrisKoz					2/01/96
//
//	Description :
//		Implementation of the CSrcDataCases class
//

#include "stdafx.h"
#include "editcase.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CEditResTest, CTest, "Drag&Drop Controls", -1, CBindSubSuite)

void CEditResTest::Run(void)
{
	if(!GetSubSuite()->m_projOpen)
	{
		CString strPrjName=m_strCWD + PROJDIR + PROJNAME + ".DSW";
		if(GetSubSuite()->m_prj.Open(strPrjName)!=ERROR_SUCCESS)
			GetLog()->RecordFailure("Coudn't open project - %s",strPrjName);
		else
			GetSubSuite()->m_projOpen=1;
	}
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);
	//open the source dialog
	if(!SetFocusToResSym(m_DialogId /*"IDD_DIALOG1"*/))
		return; //FALSE 
	UIDlgEdit m_uDlgEdSrc = UIWB.GetActiveEditor();
	if (!m_uDlgEdSrc.IsValid()) {
		m_pLog->RecordFailure("Unable to create Dialog Resource");
		return; // FALSE;
	}
	m_uDlgEdSrc.OnUpdate(); //validate the m_hwndDialog

	//open the destination dialog
	SetFocusToResSym(/*m_AboutId*/"IDD_ABOUTBOX");
	CString m_AboutId=UIWB.GetProperty(P_ID); //store the ID of the dialog
	MST.DoKeys("+{DOWN 200}"); //resize the dialog
	MST.DoKeys("{TAB}");

	while(UIWB.GetProperty(P_ID)!=m_AboutId)
	{
		if(UIWB.GetProperty(P_ID)!="IDOK")
			MST.DoKeys("{Del}"); //delete all but IDOK controls
		else
			MST.DoKeys("{TAB}"); //skip IDOK control
	}

	UIDlgEdit m_uDlgEdDest = UIWB.GetActiveEditor();
	if (!m_uDlgEdDest.IsValid()) {
		m_pLog->RecordFailure("Unable to create Dialog Resource");
		return; // FALSE;
	}
	m_uDlgEdDest.OnUpdate(); //validate the m_hwndDialog
	MST.DoKeys("%WT"); //Windw.Tile Vertical
//done the resising of about dialog & other preparations
	if(!SetFocusToResSym(controldata[7].ctrlID)) //DBGrid
		return; //FALSE 
	CString strPos;
	strPos = UIWB.GetProperty(P_Position);
	char posbuffer[50];
	RECT current;memset(&current,0, sizeof(current));
	char *pospointer=strtok(strncpy(posbuffer,strPos,50),",");
	current.left=atoi(pospointer);
	if(pospointer!=NULL)pospointer=strtok(NULL,",");
	current.top=atoi(pospointer);
	if(pospointer!=NULL)pospointer=strtok(NULL,",");
	current.right=atoi(pospointer);
	if(pospointer!=NULL)pospointer=strtok(NULL,",");
	current.bottom=atoi(pospointer);
	long centerX=(current.right+current.left)/2;
	long centerY=(current.bottom+current.top)/2;

//	DragMouse(VK_LBUTTON, 
  //  		m_uDlgEdSrc.GetDialog(), centerX, centerY, 
    //		m_uDlgEdDest.GetDialog(), centerX, centerY); //does not work
{
	int nButton=VK_LBUTTON;
	HWND hwnd1=m_uDlgEdSrc.GetDialog();
	int cX1=centerX; int cY1=centerY;
	HWND hwnd2=m_uDlgEdDest.GetDialog(); 
	int cX2=centerX; int cY2=centerY;
	WaitStepInstructions("Mouse from window '0x%X'(%d,%d) to '0x%X'(%d,%d)", 
		hwnd1, cX1, cY1, hwnd2, cX2, cY2);
	// Mouse down at point1    

	CPoint ptFrom(cX1, cY1);
	ASSERT(hwnd1 != NULL);
		ClientToScreen(hwnd1, &ptFrom);

	CPoint ptTo(cX2, cY2);
	ASSERT(hwnd2 != NULL);
		ClientToScreen(hwnd2, &ptTo);

	WaitStepInstructions("Drag mouse from %d,%d to %d,%d", ptFrom.x, ptFrom.y, ptTo.x, ptTo.y);

	MST.QueMouseDn(nButton, ptFrom);
	QueFlush(TRUE);
	Sleep(1000); //safety
	WaitStepInstructions("Clicked mouse down at %d,%d", ptFrom.x, ptFrom.y);

	MST.QueMouseMove(ptTo);
	QueFlush(TRUE);
	Sleep(1000); //safety
	WaitStepInstructions("Dragged mouse to %d,%d", ptTo.x, ptTo.y);

	
	MST.QueMouseUp(nButton,ptTo);
	QueFlush(TRUE);
	WaitStepInstructions("Released the mouse at %d,%d", ptTo.x, ptTo.y);
}

	
	
	//TODO check its properties: it shld not be set to the datasource anymore
	UIWB.DoCommand(ID_EDIT_UNDO, DC_ACCEL);
	if(!SetFocusToResSym(controldata[7].ctrlID)) //DBGrid
		return; //FALSE 
	//TODO check its properties: it shld not be set to the datasource anymore
	UIWB.DoCommand(IDM_FILE_SAVE_ALL, DC_MNEMONIC);
	//TODO test mode
}

IMPLEMENT_TEST(CRandomTest, CTest, "Random Controls", -1, CBindSubSuite)

void CRandomTest::Run(void)
{
	if(!SetFocusToResSym(m_DialogId /*"IDD_DIALOG1"*/))
		return; //FALSE 
	MST.DoKeys("+{F10}");				// Shift+F10 brings up context menu
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(500);
	MST.DoKeys("X"); //insert ActiveX control

	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(1000);
	CString listlabel=GetLabel(0x6ad6)/* OLE_CONTROL_LIST*/;
	int nelem=MST.WListCount(listlabel);
	LOG->RecordInfo("Starting the loop through all %d controls",nelem);
	MST.DoKeys("{ESC}"); //insert ActiveX control

	for(int ielem=1;ielem<=nelem;ielem++)
	{
	if(!SetFocusToResSym(m_DialogId /*"IDD_DIALOG1"*/))
		return; //FALSE 
	MST.DoKeys("+{F10}");				// Shift+F10 brings up context menu
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	Sleep(500);
	MST.DoKeys("X"); //insert ActiveX control

	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
////////////////////////////////////
		CString strControl;
		MST.WListItemClk (listlabel,ielem);
		MST.WListText (listlabel,strControl);
		LOG->RecordInfo("Clicked the control: '%s'",strControl);
		MST.WButtonClick(GetLabel(IDOK)); //OLE control chosen
		
		if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
		{
			LOG->RecordFailure("ActiveX control hanged...");
			return /* ERROR_ERROR*/;
		}
		Sleep(2000); //bug DBgrid GPFaults if I do not insert the sleep here
	// Popup the property page for the currently active item.
		UIControlProp prop = UIWB.ShowPropPage(TRUE);
		
		prop.SetExpectedTitle(strControl);
		if(prop.IsValid()==FALSE)
		{

			LOG->RecordFailure("Coudn't find the control '%s' properties",strControl);
			MST.DoKeys("{Esc}"); //get rid of the properties
		}
		else
		{
			prop.SetAllPage();
			LOG->RecordSuccess("Inserted ActiveX control: (%s)",prop.GetID());
			prop.Close();
		}
	} //for
}

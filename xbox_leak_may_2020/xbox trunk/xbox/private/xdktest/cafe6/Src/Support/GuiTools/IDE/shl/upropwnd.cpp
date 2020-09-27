///////////////////////////////////////////////////////////////////////////////
//  UPROPWND.CPP
//
//  Created by :            Date :
//      DavidGa                 10/27/93
//
//  Description :
//      Implementation of the UIProperties class
//

#include "stdafx.h"

#include "upropwnd.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "wbutil.h"
#include "guitarg.h"
#include "uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: BOOL UIProperties::IsValid(void) const
// Description: Determine if the property page is valid by checking the window handle and the title of the property page.
// Return: TRUE if the property page is valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIControlProp::IsValid(void) const
{
	CString strTitle=GetText();
	CString strExpected=ExpectedTitle();
	if(UIWindow::IsValid()==FALSE)
		return FALSE;
	if(strTitle.Find(GetLocString(IDSS_PROP_TITLE)) < 0)
		return FALSE;
	if(!strExpected.IsEmpty() && strTitle.Find(strExpected) < 0)
		return FALSE;
	return TRUE;
}



// BEGIN_HELP_COMMENT
// Function: BOOL UIControlProp::GetID(void)
// Description: Gets the string from the property General page.
// Return: empty string if not success.
// END_HELP_COMMENT
CString UIControlProp::GetID(void)
{
static CString idName;
	idName.Empty();
	HWND hGenPage=ShowPage(/*"General"*/ 0xDE /*combobox*/,0);
	if(hGenPage==NULL)
		return idName;
	MST.WComboText(GetLabel(0xde), idName);
	::WaitForInputIdle(g_hTargetProc, 3000);
	return idName;
} //GetID


// BEGIN_HELP_COMMENT
// Function: BOOL UIControlProp::SetID(void)
// Description: Gets the string from the property General page.
// Return: FALSE if not success.
// END_HELP_COMMENT
BOOL UIControlProp::SetID(CString newID)
{
	HWND hGenPage=ShowPage(/*"General"*/ 0xDE /*combobox*/,0);
	if(hGenPage==NULL)
		return FALSE;
	MST.WComboSetText(GetLabel(0xDE), newID);
	MST.DoKeys("{TAB}");	// validate the field which was just set
	::WaitForInputIdle(g_hTargetProc, 3000);
	return TRUE;
} //SetID

#include "..\..\testutil.h"
// BEGIN_HELP_COMMENT
// Function: BOOL UIControlProp::SetAllPage(void)
// Description: Changes the current page to "All" page.
// Return: FALSE if not success.
// END_HELP_COMMENT
BOOL UIControlProp::SetAllPage(void)
{
//	if(SetPage("All")==FALSE)
//		return FALSE;
	
	HWND hGenPage=ShowPage(/*"All"*/ 0x52d5 /*listbox*/,0);
	if(hGenPage==NULL)
		return FALSE;
	::WaitForInputIdle(g_hTargetProc, 3000);

	RECT rcPage;
	GetClientRect(HWnd(), &rcPage);
	ClickMouse( VK_LBUTTON ,HWnd(), rcPage.left+150, rcPage.top + 150 );	  // Click into the control

	return TRUE;
} //SetAllPage

#include "guiv1.h" //for LOG macro

// BEGIN_HELP_COMMENT
// Function: BOOL UIControlProp::putProperty(LPCSTR PropName,LPCSTR szValue)
// Description: changes the property in "All" page. 
// Params: PropName - name of the property
//			szValue - value of the propery
//			method  - METHOD_EDIT: edit the field (by default), 
//						or select from the list
// Return: 0 if not success.
//         1 if success.
//        -1 if not propery not found in the list (METHOD_LIST only)
// END_HELP_COMMENT
int UIControlProp::putProperty(LPCSTR PropName,LPCSTR szValue,BOOL method /*=METHOD_EDIT*/)
{
	if(SetAllPage()==FALSE)
		return 0;
	int count=MST.WListCount(NULL);
	int nitem=MST.WListItemExists(NULL, PropName);
	if(nitem>0)
	{ //only this way of doing things, no way to validate what we are doing
		MST.WListItemClk(NULL,PropName);
		MST.DoKeys("{TAB}", FALSE, 1000); //move to the value part of the field
		if(method==METHOD_LIST)
		{
//			MST.WListItemClk(NULL,PropName,-10);
//			MST.WListText(NULL,strSlItem);
//			ASSERT(strSlItem==PropName);
//			if((nitem=MST.WListItemExists(NULL, szValue))>0)
//			{
//				MST.WListItemClk(NULL,szValue);
//			}
//			else
//				return FALSE;
			
//			COClipboard clip;clip.SetText("");
			SetClipText("");
			CString strSlItem,nextItem;
			int loopcount=0;
			char firstchar[4];
			firstchar[0]=szValue[0];
			firstchar[1]=0;
			do
			{
nextitem:
				loopcount++;
				if(szValue[0]==0) //empty string - clean the field
				{
					MST.DoKeys("+{END}{DEL}",FALSE,10000); //replace with the keystrokes
					MST.DoKeys("{UP}{DOWN}"); //wierd way to escape focus off value field
					goto handle_error;				
				}
				MST.DoKeys(firstchar); //typing the first char cycles through the list
				::WaitForInputIdle(g_hTargetProc, 10000);
				MST.DoKeys("+{END}");
				UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);
//				nextItem=clip.GetText();
				GetClipText(nextItem);	// grab the clipboard
				if(nextItem==szValue)
				{ //found the item
					MST.DoKeys("{UP}{DOWN}"); //wierd way to escape focus off value field
					goto handle_error;				
				}
				if(strSlItem.IsEmpty())
				{ 
					if(nextItem.IsEmpty())
					{ //empty list
						MST.DoKeys("{UP}{DOWN}"); //wierd way to escape focus off value field
						LOG->RecordInfo("Empty list for th property (%s)",PropName);
						return -1;
					}
					strSlItem=nextItem;
					goto nextitem; //in case of first item don't compare it
				}
			}while(nextItem!=strSlItem && loopcount<100); //returned to the begin of loop?
			MST.DoKeys("{UP}{DOWN}");
			LOG->RecordFailure("Item (%s) of property (%s) not found",szValue,PropName);
			return 0; //not found
		}
		else
		{
			MST.DoKeys("+{END}");//select the old value
			if(szValue[0]==0) //empty string - clean the field
				MST.DoKeys("{DEL}",FALSE,10000); //replace with the keystrokes
			else
				MST.DoKeys(szValue,TRUE,10000); //replace with the keystrokes
			MST.DoKeys("{UP}{DOWN}"); //wierd way to escape focus off value field
		}
	}
	else 
		return 0;
handle_error:
	::WaitForInputIdle(g_hTargetProc, 10000);
	UIDialog uErrorDlg;
	if(uErrorDlg.WaitAttachActive(1000))
	{
		char acBuf[256];
		uErrorDlg.GetText(acBuf, 255); 
		LOG->RecordFailure("Unexpaected Dialog box: (%s) detected. Clicked default",acBuf);
		uErrorDlg.Close();
		return 0;
	}
	CString strprop;
	strprop=getProperty(PropName);
	BOOL equal=(strprop==szValue);
	LOG->RecordCompare(equal==TRUE, "Property set: (%s) == propery displayed: (%s)",szValue, strprop);
	return equal;
} //putProperty
//	char dokey[20];
//		MST.DoKeys("{HOME}");
//		sprintf(dokey,"{DOWN %d}",nitem-1);
//		MST.DoKeys(dokey); //move down to the correct field
		
//		MST.WListText(NULL,strSlItem);
//		ASSERT(strSlItem==PropName);
//		MST.DoKeys("{TAB}"); //move to the value part of the field
//		MST.DoKeys("+{END}");//select the old value
//		MST.DoKeys(szValue); //replace with the keystrokes


// BEGIN_HELP_COMMENT
// Function: CString& UIControlProp::getProperty(LPCSTR PropName)
// Description: gets the property in "All" page. 
// Focus problem: the focus must be on the ListBox control.
//Usually, it is when the PP is broght & All page found using keyboard only
// Return: Empty string if not success.
// END_HELP_COMMENT
CString UIControlProp::getProperty(LPCSTR PropName)
{
static CString idName;
	idName.Empty();

	if(SetAllPage()==FALSE)
		return idName;
	int count=MST.WListCount(NULL);
	int nitem=MST.WListItemExists(NULL, PropName);
//	COClipboard clip;clip.SetText("");
	SetClipText("");
	if(nitem>0)
	{ //only this way of doinf things, no way to validate what we are doing
//		MST.DoKeys("{HOME}");
//		sprintf(dokey,"{DOWN %d}",nitem-1);
//		MST.DoKeys(dokey); //move down to the correct field
		MST.WListItemClk(NULL,PropName);
		MST.DoKeys("{TAB}", FALSE, 1000); //move to the value part of the field
		MST.DoKeys("+{END}");//select the old value
		UIWB.DoCommand(ID_EDIT_COPY, DC_ACCEL);		// REVIEW: is there a better way?
		MST.DoKeys("{UP}{DOWN}"); //wierd way to escape focus off value field
//		return clip.GetText();	
		GetClipText(idName); // grab the clipboard
	}	
	return idName;
} //getProperty


// BEGIN_HELP_COMMENT
// Function: HWND UIControlProp::Close(void)
// Description: Closes an open property page frame. Gives the focus to the open PP frame, validates it & closes it
// Return: Handle of the frame found, NULL if not success.
// END_HELP_COMMENT
HWND UIControlProp::Close(void)
{	
	//set focus on properties frame
	HWND hprop=MST.WFndWndWait(GetLocString(IDSS_PROP_TITLE),FW_PART|FW_FOCUS,0);
	if(hprop!=NULL)
	{
		MST.DoKeys("{ENTER}");	// ENTER should validate the window
		while(MST.WFndWndWait(GetLocString(IDSS_PROP_TITLE),FW_PART|FW_FOCUS,0))
			MST.DoKeys("{ESC}"); //shld close the window (ENTER does not if it's pinned)
	}
	return hprop;
} //Close()

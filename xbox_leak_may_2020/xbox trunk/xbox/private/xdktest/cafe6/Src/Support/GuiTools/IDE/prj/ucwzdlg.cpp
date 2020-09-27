///////////////////////////////////////////////////////////////////////////////
//  UCWZDLG.CPP
//
//  Created by :            Date :
//      Ivanl              3/07/94
//                                
//  Description :
//      Implementation of the UIClassWizard class
//

#include "stdafx.h"
#include "odbcdlgs.h"
#include "ucwzdlg.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "guiv1.h"
#include "guitarg.h"
#include "..\shl\wbutil.h"
#include "..\shl\uwbframe.h"
#include "..\sym\cmdids.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void UIClassWizard::OnUpdate(void)
{
	UIDialog::OnUpdate();
    m_AutoDlg = NULL ;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIClassWizard::Display() 
// Description: Display the ClassWizard dialog.
// Return: The HWND of the ClassWizard dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIClassWizard::Display() 
{
    WaitForInputIdle(g_hTargetProc, 60000);
	AttachActive();           // Check if is already up
	if (!IsValid())
	{
     WaitForInputIdle(g_hTargetProc, 60000);
	 //UIWB.DoCommand(IDMY_CLASSWIZARD, DC_MENU);
	 MST.DoKeys("^w");
	 AttachActive();
	}
	return WGetActWnd(0); 
}

// BEGIN_HELP_COMMENT
// Function: HWND UIClassWizard::DeleteFunction(int ClassIndex, int FuncIndex, int ObjectIndex)
// Description: Delete a message map member function from the class.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ClassIndex An integer that contains the 1-based index into the list of classes in ClassWizard. This specifies the class that contains the function to delete.
// Param: FuncIndex An integer that contains the 1-based index into the list of messages. This specifies the member function to delete.
// Param: ObjectIndex An integer that contains the 1-based index into the list of objects for the selected class. This specifies the object that contains the function to delete.
// END_HELP_COMMENT
int UIClassWizard::DeleteFunction(int ClassIndex, int FuncIndex, int ObjectIndex )
{
	Display() ; //Make sure ClassWiz has is the active window.
	ShowPage(TAB_MESSAGEMAP, 8);
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSBOX)) < ClassIndex )
	return ERROR_ERROR ;
	MST.WComboItemDblClk(GetLabel(CLSWIZ_IDC_CLASSBOX), ClassIndex) ;
	if (!WaitForOriginalWnd(HWnd(),2)) // If a dlg comes up press Enter
		    MST.DoKeys("{ENTER}") ;
    if (MST.WListCount(GetLabel(CLSWIZ_IDC_THINGLIST)) < ObjectIndex)
		return ERROR_ERROR ;
	MST.WListItemClk(GetLabel(CLSWIZ_IDC_THINGLIST), ObjectIndex) ;
	if (MST.WListCount(GetLabel(CLSWIZ_IDC_MESSAGELIST)) < FuncIndex)
		return ERROR_ERROR ;
	MST.WListItemClk(GetLabel(CLSWIZ_IDC_MESSAGELIST), FuncIndex) ;
// Delete the selected message handler.
	int MappedCount = MST.WListCount(GetLabel(CLSWIZ_IDC_MAPPEDLIST)) ;
	MST.WButtonClick(GetLabel(CLSWIZ_ID_REMOVE)) ;
	if (!WaitForOriginalWnd(HWnd(),2)) // Press Yes on the alert about deleting the implementation 
		MST.DoKeys("{ENTER}") ;
// Verify that the message handlers are less by one after the delete.
	if (MST.WListCount(GetLabel(CLSWIZ_IDC_MAPPEDLIST)) == MappedCount)
	{
		if (MappedCount)  
		{
			CString cs = "											" ;
			MST.WListText(GetLabel(CLSWIZ_IDC_MAPPEDLIST), cs) ;
			LOG->RecordInfo(cs) ;
		}
		return ID_NODELETE ;
	}       
	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIClassWizard::Close() 
// Description: Close the ClassWizard dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
HWND UIClassWizard::Close() 
{
   MST.WButtonClick(GetLabel(IDOK)) ;
   if (m_AutoDlg) // Delete this object if used one.
		delete (m_AutoDlg) ;

   return ERROR_SUCCESS ; 
}

// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::ClassCount()
// Description: Gets the number of classes in the active project.
// Return: An integer that indicates the class count.
// END_HELP_COMMENT
int UIClassWizard::GetClassCount()
{
	Display() ; //Make sure ClassWiz has is the active window.
	ShowPage(TAB_MESSAGEMAP, 8);
	return MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSBOX)) ;
}

// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::AddFunction(int ClassIndex, int FuncIndex, int ObjectIndex)
// Description: Add a message map member function to the class.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ClassIndex An integer that contains the 1-based index into the list of classes in ClassWizard. This specifies the class that contains the function to add.
// Param: FuncIndex An integer that contains the 1-based index into the list of messages. This specifies the member function to add.
// Param: ObjectIndex An integer that contains the 1-based index into the list of objects for the selected class. This specifies the object that contains the function to add.
// END_HELP_COMMENT
int UIClassWizard::AddFunction(int ClassIndex, int FuncIndex, int ObjectIndex)
{
	Display() ; //Make sure ClassWiz has is the active window.
	ShowPage(TAB_MESSAGEMAP, 8);
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSBOX)) < ClassIndex )
	return ERROR_ERROR ;
	MST.WComboItemDblClk(GetLabel(CLSWIZ_IDC_CLASSBOX), ClassIndex) ;
//      if (WaitForWndWithCtrl(8,1000))
	if (!WaitForOriginalWnd(HWnd(),2)) // If a dlg comes up press Enter
		    MST.DoKeys("{ENTER}") ;
    if (MST.WListCount(GetLabel(CLSWIZ_IDC_THINGLIST)) < ObjectIndex)
		return ERROR_ERROR ;
	MST.WListItemClk(GetLabel(CLSWIZ_IDC_THINGLIST), ObjectIndex) ;
	if (MST.WListCount(GetLabel(CLSWIZ_IDC_MESSAGELIST)) < FuncIndex)
		return ERROR_ERROR ;
	MST.WListItemDblClk(GetLabel(CLSWIZ_IDC_MESSAGELIST), FuncIndex) ;
	if (WaitForWndWithCtrl(CLSWIZ_IDC_MEMBERNAME,500))
		{
		 CString cs = "                                            " ;
		 MST.WEditText("",cs) ;
		 MST.DoKeys("{ENTER}") ;
//               if (WaitForWndWithCtrl(3,500)) // If its a duplicate cancel
		if (!WaitForOriginalWnd(HWnd(),2)) // If its a dupliate
			{
			 MST.DoKeys("{ESC 2}") ;
		     LOG->RecordInfo(cs+":  Was a duplicate function") ;
			}
		}
//      if (WaitForWndWithCtrl(3,500)) // If its a duplicate cancel
	if (!WaitForOriginalWnd(HWnd(),2)) // If a dlg comes up press Enter
	{
		 MST.DoKeys("{ESC}") ;
	     LOG->RecordInfo("Duplicate function") ;
	}

	m_ActiveClass = ClassIndex ;
	return ERROR_SUCCESS;
}
	  
	  
// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::AddFunction(int ClassIndex, int FuncIndex, LPCSTR szObject)
// Description: Add a message map member function to the class.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ClassIndex An integer that contains the 1-based index into the list of classes in ClassWizard. This specifies the class that contains the function to add.
// Param: FuncIndex An integer that contains the 1-based index into the list of messages. This specifies the member function to add.
// Param: szObject A string that contains the name of object ID to use for the function.
// END_HELP_COMMENT
int UIClassWizard::AddFunction(int ClassIndex, int FuncIndex, LPCSTR szObject)
{
	Display() ; //Make sure ClassWiz has is the active window.
	ShowPage(TAB_MESSAGEMAP, 8);
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSBOX)) < ClassIndex )
	return ERROR_ERROR ;
	MST.WComboItemClk(GetLabel(CLSWIZ_IDC_CLASSBOX), ClassIndex) ;
    if (!MST.WListItemExists(GetLabel(CLSWIZ_IDC_THINGLIST), szObject))
		return ERROR_ERROR ;
	MST.WListItemClk(GetLabel(CLSWIZ_IDC_THINGLIST), szObject) ;
	if (MST.WListCount(GetLabel(CLSWIZ_IDC_MESSAGELIST)) < FuncIndex)
		return ERROR_ERROR ;
	MST.WListItemDblClk(GetLabel(CLSWIZ_IDC_MESSAGELIST), FuncIndex) ;
	if (WaitForWndWithCtrl(CLSWIZ_IDC_MEMBERNAME,500))
		{
		 CString cs = "                                            " ;
		 MST.WEditText("",cs) ;
		 MST.DoKeys("{ENTER}") ;
		if (!WaitForOriginalWnd(HWnd(),2)) // If its a dupliate
			{
			 MST.DoKeys("{ESC 2}") ;
		     LOG->RecordInfo(cs+":  Was a duplicate function") ;
			}
		}
	if (!WaitForOriginalWnd(HWnd(),2)) // If a dlg comes up press Enter
	{
		 MST.DoKeys("{ESC}") ;
	     LOG->RecordInfo("Duplicate function") ;
	}

	m_ActiveClass = ClassIndex ;
	return ERROR_SUCCESS;
}


// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::AddFunction2(int ClassIndex, int FuncIndex, int ObjectIndex)
// Description: Add a message map member function to the class.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ClassIndex An integer that contains the 1-based index into the list of classes in ClassWizard. This specifies the class that contains the function to add.
// Param: FuncIndex An integer that contains the 1-based index into the list of messages. This specifies the member function to add.
// Param: ObjectIndex An integer that contains the 1-based index into the list of objects for the selected class. This specifies the object that contains the function to add.
// END_HELP_COMMENT
int UIClassWizard::AddFunction2(int ClassIndex, int FuncIndex, int ObjectIndex)
{
	Display() ; //Make sure ClassWiz has is the active window.
	ShowPage(TAB_MESSAGEMAP, 8);
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSBOX)) < ClassIndex )
	return ERROR_ERROR ;
	MST.WComboItemClk(GetLabel(CLSWIZ_IDC_CLASSBOX), ClassIndex) ;
//      if (WaitForWndWithCtrl(8,1000))
/*	if (!WaitForOriginalWnd(HWnd(),2)) // If a dlg comes up press Enter
		    MST.DoKeys("{ENTER}") ;
*/    if (MST.WListCount(GetLabel(CLSWIZ_IDC_THINGLIST)) < ObjectIndex)
		return ERROR_ERROR ;
	MST.WListItemClk(GetLabel(CLSWIZ_IDC_THINGLIST), ObjectIndex) ;
	if (MST.WListCount(GetLabel(CLSWIZ_IDC_MESSAGELIST)) < FuncIndex)
		return ERROR_ERROR ;
	MST.WListItemDblClk(GetLabel(CLSWIZ_IDC_MESSAGELIST), FuncIndex) ;
	if (WaitForWndWithCtrl(CLSWIZ_IDC_MEMBERNAME,500))
		{
		 CString cs = "                                            " ;
		 MST.WEditText("",cs) ;
		 MST.DoKeys("{ENTER}") ;
//               if (WaitForWndWithCtrl(3,500)) // If its a duplicate cancel
		if (!WaitForOriginalWnd(HWnd(),2)) // If its a dupliate
			{
			 MST.DoKeys("{ESC 2}") ;
		     LOG->RecordInfo(cs+":  Was a duplicate function") ;
			}
		}
//      if (WaitForWndWithCtrl(3,500)) // If its a duplicate cancel
	if (!WaitForOriginalWnd(HWnd(),2)) // If a dlg comes up press Enter
	{
		 MST.DoKeys("{ESC}") ;
	     LOG->RecordInfo("Duplicate function") ;
	}

	m_ActiveClass = ClassIndex ;
	return ERROR_SUCCESS;
}
	  
	// Name = ""  Derive from Class type 
	// DotH = "", DotCPP ="" Use default
// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::AddClass(int TypeIndex,CString Name,  CString DotH, CString DotCPP) 
// Description: Add a new class the project via ClassWizard.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: TypeIndex An integer that contains the 1-based index into the list of base classes to derive this class from.
// Param: Name A CString that contains the name of the new class.
// Param: DotH A CString that contains the name of the header file for the new class.
// Param: DotCPP A CString that contains the name of the source file for the new class.
// END_HELP_COMMENT
int UIClassWizard::AddClass(int TypeIndex,CString Name,  CString DotH, CString DotCPP) 
{
 int ret ;
 
  //Display() ; //Make sure ClassWiz is the active window.
  //if(MST.WFndWndWait(GetLocString(IDSS_WORKBENCH_TITLEBAR), FW_FULL, 1)){
  // 	 MST.WButtonClick(GetLocString(IDSS_OK));
  //	 MST.WButtonClick(GetLocString(IDSS_CANCEL));
  //}
 if (m_AddClassDlg.Show()) 
 {
	if(Name.IsEmpty())
	{
		MST.WComboItemText(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE), TypeIndex, Name) ;
		Name.MakeUpper() ;
	 }
	 ret = m_AddClassDlg.SetType(TypeIndex); 
	 if (ret == ERROR_ERROR)
		{
		MST.DoKeys("{ESC}") ;
		return ERROR_ERROR ;
	  }                                              
	 m_AddClassDlg.SetName(Name) ;
	 m_AddClassDlg.SetHeader(DotH) ;
	 m_AddClassDlg.SetCPP(DotCPP) ;
	 m_AddClassDlg.AttachDlgID() ;
	// #ifdef ODBC_OLE
	 if (m_AddClassDlg.CheckOle() == ERROR_SUCCESS)
	 {
		m_AddClassDlg.CheckOle() ;
	 }
	 //#endif 

	 if (m_AddClassDlg.Create() != ERROR_SUCCESS) // If we got an error message 
		{
		MST.DoKeys("{ESC}") ;
		return -99;
		}
	 return ERROR_SUCCESS ;
 }
 else
 {
	 LOG->RecordInfo("Could not open Add Class dialog.");
	 return ERROR_ERROR ;
 }

}


	// Name = ""  Derive from Class type 
	// DotH = "", DotCPP ="" Use default
// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::AddClass(CString BaseClassName,CString Name,  CString DotH, CString DotCPP) 
// Description: Add a new class the project via ClassWizard.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param:  BaseClassName: A base class name
// Param: Name A CString that contains the name of the new class.
// Param: DotH A CString that contains the name of the header file for the new class.
// Param: DotCPP A CString that contains the name of the source file for the new class.
// END_HELP_COMMENT
int UIClassWizard::AddClass2(CString BaseClassName,CString Name,  CString DotH, CString DotCPP) 
{
 int ret ;
 
 Display() ; //Make sure ClassWiz is the active window.

// define TypeIndex;
 int TypeIndex=0;
 CString str;	

 if (m_AddClassDlg.Show()) 
 {
    for (int i=1; i<=MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE)); i++)
	{
	  MST.WComboItemText(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE), i, str) ;
	  if( str.Find(BaseClassName) != -1 )
	  {
		  TypeIndex = i;
		  break;
	  }
	}

	if(Name.IsEmpty())
	{
	  MST.WComboItemText(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE), TypeIndex, Name) ;
		Name.MakeUpper() ;
	 }
	 ret = m_AddClassDlg.SetType(TypeIndex); 
	 if (ret == ERROR_ERROR)
		{
		MST.DoKeys("{ESC}") ;
		return ERROR_ERROR ;
	  }                                              
	 m_AddClassDlg.SetName(Name) ;
	 m_AddClassDlg.SetHeader(DotH) ;  
	 m_AddClassDlg.SetCPP(DotCPP) ;	  
	 m_AddClassDlg.AttachDlgID() ;
	// #ifdef ODBC_OLE
	 if (m_AddClassDlg.CheckOle() == ERROR_SUCCESS)
	 {
		m_AddClassDlg.CheckOle() ;
	 }
	 //#endif 

	 if (m_AddClassDlg.Create() != ERROR_SUCCESS) // If we got an error message 
		{
		MST.DoKeys("{ESC}") ;
		return -99;
		}
	 return ERROR_SUCCESS ;
 }
 else
 {
	 LOG->RecordInfo("Could not open Add Class dialog.");
	 return ERROR_ERROR ;
 }

}


// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::ImportClass(CString Name, CString DotH, CString DotCPP) 
// Description: Import an existing class into the project via ClassWizard.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Name A CString containing the name of the new class.
// Param: DotH A CString containing the name of the header file for the class.
// Param: DotCPP A CString containing the name of the source file for the class.
// END_HELP_COMMENT
int UIClassWizard::ImportClass(CString Name,  CString DotH, CString DotCPP) 
{
	UIImportOrCreateDlg ImpCrDlg ;

	ImpCrDlg.Show() ;
	ImpCrDlg.AddImport() ;
	//if(!WaitForWndWithCtrl(CLSWIZ_IDC_CPPFILENAME,1500))
//              return FALSE ;
	return TRUE ;

}


// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::EditCode()
// Description: Select the Edit Code button the Message Map tab in ClassWizard.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int UIClassWizard::EditCode()
{
    WaitForInputIdle(g_hTargetProc, 60000);
	ShowPage(TAB_MESSAGEMAP, 8);
	MST.WButtonClick(GetLabel(CLSWIZ_ID_GOTOCODE));
    if (WaitForWndWithCtrl(6,500) || WaitForWndWithCtrl(8,500)) // If prompted to reload the file (bug)
		MST.DoKeys("{ENTER}") ;
	return ERROR_SUCCESS ;
}                                                                                                  

// BEGIN_HELP_COMMENT
// Function: CString UIClassWizard::GetClassFile()
// Description: Get the name of the source file for the currently selected class in ClassWizard.
// Return: A CString containing the name of the source file for the currently selected class.
// END_HELP_COMMENT
CString UIClassWizard::GetClassFile()
{
	HWND hWnd = ControlOnPropPage(CLSWIZ_IDC_FILES) ;
	CString FileName ;
	CString tmp= GetLabel(CLSWIZ_IDC_FILES) ;
	int i = tmp.Find(",") ;                                                                    
	FileName = tmp.Mid(i+2) ;
	return FileName ;
}

// BEGIN_HELP_COMMENT
// Function: int UIClassWizard::SetDataSourceName()
// Description: Set the datasource name to be used when creating Database classes.
// Return: void. 
// END_HELP_COMMENT
void UIClassWizard::SetDataSourceName(CString Name)
{
	m_AddClassDlg.SetDataSourceName(Name) ;         
}

// BEGIN_HELP_COMMENT
// Function: CString UIClassWizard::GetActiveClass()
// Description: Get the name of the currently selected class in ClassWizard.
// Return: A CString that contains the name of the currently selected class.
// END_HELP_COMMENT
CString UIClassWizard::GetActiveClass()
{
	CString Class  = "																		";
	MST.WComboItemText(GetLabel(CLSWIZ_IDC_CLASSBOX),m_ActiveClass, Class) ;
	return Class ;
}

UIAddMethod * UIClassWizard::AddOleMethod() 
{
	if (m_AutoDlg) // Delete this object if we already have had one.
		delete (m_AutoDlg) ;
	UIAddMethod *  MthdDlg = new UIAddMethod ;

	ShowPage(TAB_OLE_AUTOMATION, 8);
	MST.WButtonClick(GetLabel(CLSWIZ_ID_ADDMETHOD)) ;
    WaitForInputIdle(g_hTargetProc, 60000);
	m_AutoDlg = (UIAutomationDlg * ) MthdDlg ;
	return MthdDlg ;
}

UIMemVarDlg * UIClassWizard::AddMemberVar(int index) 
{
	if (m_AddMemDlg) // Delete this object if we already have had one.
		delete (m_AddMemDlg) ;
		m_AddMemDlg = new UIMemVarDlg ;

	ShowPage(TAB_MEMBER_VARIABLES, 8);

	MST.WListItemClk(GetLabel(CLSWIZ_IDC_THINGLIST), index) ;
	MST.WButtonClick(GetLabel(CLSWIZ_ID_ADDVAR)) ;
    WaitForInputIdle(g_hTargetProc, 60000);
	return m_AddMemDlg ;
}

UIMemVarDlg * UIClassWizard::AddMemberVar(CString CtrId) 
{
	if (m_AddMemDlg) // Delete this object if we already have had one.
		delete (m_AddMemDlg) ;
		m_AddMemDlg = new UIMemVarDlg ;

	ShowPage(TAB_MEMBER_VARIABLES, 8);

	MST.WListItemClk(GetLabel(CLSWIZ_IDC_THINGLIST), CtrId) ;
	MST.WButtonClick(GetLabel(CLSWIZ_ID_ADDMETHOD)) ;
    WaitForInputIdle(g_hTargetProc, 60000);
	return m_AddMemDlg ;
}

int UIClassWizard::GetControlCount() 
{
	ShowPage(TAB_MEMBER_VARIABLES, 8);
	return MST.WListCount(GetLabel(CLSWIZ_IDC_THINGLIST)) ;
}

UIAddProperty * UIClassWizard::AddOleProperty() 
{
	if (m_AutoDlg) // Delete this object if we already have had one.
		delete (m_AutoDlg) ;

	UIAddProperty *  PropDlg = new UIAddProperty ;

	ShowPage(TAB_OLE_AUTOMATION, 8);
	MST.WButtonClick(GetLabel(CLSWIZ_ID_ADD_PROP)) ;
    WaitForInputIdle(g_hTargetProc, 60000);
	m_AutoDlg = (UIAutomationDlg * ) PropDlg ;
	return PropDlg ;
}

UIAddEvent * UIClassWizard::AddOleEvent() 
{
	if (m_AutoDlg) // Delete this object if we already have had one.
		delete (m_AutoDlg) ;

	UIAddEvent *  evntDlg = new UIAddEvent ;
	ShowPage(TAB_OLE_EVENTS, 8);
	MST.WButtonClick(GetLabel(CLSWIZ_ID_ADD_CUSTOM)) ;
    WaitForInputIdle(g_hTargetProc, 60000);
	m_AutoDlg = (UIAutomationDlg *) evntDlg ;
	return evntDlg ;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::Show()
// Description: Display the Create New Class dialog from ClassWizard.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
BOOL UIAddClassDlg::Show()
{
	HWND DevStudHnd = MST.WFndWndWait(GetLocString(IDSS_CLW_TITLE),FW_PART,60); //GetLocString(IDSS_WORKBENCH_TITLEBAR)
    if(!DevStudHnd) return FALSE; 
	//Sleep(2000);
	CString strGotoMenuItem = GetLocString(IDS_CLSWZ_NEWCLASS);
	HWND hWnd = ControlOnPropPage(CLSWIZ_ID_ADDCLASS) ;
	if ((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) 
	{
		//*****WMenuEnabled commented out for work around for bug #12931
		//make sure focus is on msdev ide
		//MST.QueSetFocus(DevStudHnd);
		MST.WButtonClick(GetLabel(CLSWIZ_ID_ADDCLASS, hWnd,TRUE)) ;
		//if (!MST.WMenuEnabled(strGotoMenuItem))
		//{
		//	LOG->RecordInfo("Menu item %s doesn't exist or is disabled",strGotoMenuItem);
		//	return FALSE;
		//}
		//else
		//{
			MST.DoKeys("{DOWN}") ;
			MST.DoKeys("{ENTER}") ;
		//}
	}
	Sleep(2000);

	if(!WaitForWndWithCtrl(CLSWIZ_IDC_CLASSTEMPLATE,3000))
	{
		return FALSE ;
	}
	else
	{
		return TRUE ;
	}
	
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::SetType(int Type) 
// Description: Set the the base class of the new class in the Create New Class dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Type An integer that contains the 1-based index into the list of base classes.
// END_HELP_COMMENT
int UIAddClassDlg::SetType(int Type) 
{
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE)) < Type )
		return ERROR_ERROR ;

	MST.WComboSetFocus(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE) ) ;
	MST.DoKeys("{F4}"); ;
	MST.WComboItemClk(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE), Type) ;
	if(MST.WComboIndex(GetLabel(CLSWIZ_IDC_CLASSTEMPLATE)) != Type)
		return ERROR_ERROR ;
	return ERROR_SUCCESS ;
} 

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::SetName(CString Name)
// Description: Set the name of the class in the Create New Class dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Name A CString containing the name of the new class.
// END_HELP_COMMENT
int UIAddClassDlg::SetName(CString Name)
{
 // If the name has space, use the first half.
 Name = Name.SpanExcluding(" ") ;
 MST.WEditSetText(GetLabel(CLSWIZ_IDC_CLASSNAME),Name) ;

 return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::SetHeader(CString DotH)
// Description: Set the name of the header file for the new class in the Create New Class dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: DotH A CString that contains the name of the new header file.
// END_HELP_COMMENT
int UIAddClassDlg::SetHeader(CString DotH)
{  // ToDo: SetCPP doesn't  work properly : it should use the Change Files dialog
if (!DotH.IsEmpty())
	MST.WEditSetText(GetLabel(CLSWIZ_IDC_HFILENAME),DotH) ;
return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::SetCPP(CString DotCPP)
// Description: Set the name of the source file for the new class in the Create New Class dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: DotCPP A CString that contains the name of the new header file.
// END_HELP_COMMENT
int UIAddClassDlg::SetCPP(CString DotCPP)
{  // ToDo: SetCPP doesn't  work properly : it should use the Change Files dialog
 if (!DotCPP.IsEmpty())
	MST.WEditSetText(GetLabel(CLSWIZ_IDC_CPPFILENAME),DotCPP) ;
 return TRUE ;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::SetDataSourceName()
// Description: Set the datasource name to be used when creating Database classes.
// Return: void. 
// END_HELP_COMMENT
void UIAddClassDlg::SetDataSourceName(CString Name)
{
	m_DBdlgs.SetDataBasePath(Name) ;                
}




// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::Create()
// Description: Create the new class in the current project by clicking the Create button in the Create New Class dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int UIAddClassDlg::Create()
{
	MST.WButtonClick(GetLabel(IDOK)) ;
	if(MST.WFndWndWait(GetLocString(IDSS_WORKBENCH_TITLEBAR), FW_FULL, 2)){
		MST.WButtonClick(GetLocString(IDSS_OK));
		MST.WButtonClick(GetLocString(IDSS_CANCEL));
	}

	// If more information is required, or if duplicate classname then we return with error 
	if(WaitForWndWithCtrl(8,500) || WaitForWndWithCtrl(3,500))
	{
		MST.DoKeys("{ENTER}") ;
		return -99 ;
	} 
	if(WaitForWndWithCtrl(CLSWIZ_IDC_ODBC_SOURCES,500)) // if a Database classs.
	{
	//#ifndef ODBC_OLE
	//MST.DoKeys("{ESC 2}") ;
	//       return -98 ; 
	//#endif ODBC_OLE

	//      m_DBdlgs.WaitForSources() ;
	m_DBdlgs.Sources.SetDAOName(m_DBdlgs.GetDataBasePath()) ;
	m_DBdlgs.Sources.Close() ;

	//      m_DBdlgs.WaitForLogin() ;
	/*      m_DBdlgs.login.ID("sa") ;
		m_DBdlgs.login.Pw("") ;
		m_DBdlgs.login.Close() ;
	
	*/  
	m_DBdlgs.WaitForTables() ;
	m_DBdlgs.tables.Name(1) ;
	MST.DoKeys("{ENTER}") ;
  
	/* MST.DoKeys("{ESC 2}") ;
	 return -98 ; */
	} 
	if(WaitForWndWithCtrl(CLSWIZ_IDC_CLASSNAME,500)) // if a Database classs.
	{
	// Take the default recordset.
		int recSetCnt = MST.WComboCount(GetLabel(CLSWIZ_IDC_CLASSNAME)) ;
		if (recSetCnt<= 1) // If there is no recordset, created a new one.
		{   
			// Support for the new functionality of displaying the Classwizard dialog twice.
			static int cnt = 0 ;
			CString num ;
			num.Format("%i",cnt) ;
			CString name = "CRecSet" ;
			name+=num ;
			cnt++ ;
			MST.WButtonClick(GetLabel(CLSWIZ_ID_ADD));
			
			// Wait for the second instance of ClassWizard
			if(!WaitForWndWithCtrl(CLSWIZ_IDC_CLASSTEMPLATE,1500))
				return ERROR_ERROR ;
			SetName(name) ;
			MST.DoKeys("{ENTER}") ;
			if(!WaitForWndWithCtrl(CLSWIZ_IDC_ODBC_SOURCES,500)) // if a Database classs.
				return ERROR_ERROR ;
			m_DBdlgs.Sources.SetDAOName(m_DBdlgs.GetDataBasePath()) ;
			m_DBdlgs.Sources.Close() ;
			m_DBdlgs.WaitForTables() ;
			m_DBdlgs.tables.Name(1) ;
		}

		MST.DoKeys("{ENTER}") ;
	} 
	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::CheckOle()
// Description: Select the OLE automation for the new class in the Create New Class dialog.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int UIAddClassDlg::CheckOle(OLE_TYPE OleType/*= ID_OLE */)
{
		MST.WOptionClick(GetLabel(OleType));
		return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::CheckOleCreateable(CString ExtName) 
// Description: Select OLE automation createable by name for the new class in the Create New Class dialog. This function is NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ExtName A CString that contains the name.
// END_HELP_COMMENT
int UIAddClassDlg::CheckOleCreateable(CString ExtName) 
{
//      if(MST.WCheckEnabled(GetLabel(CLSWIZ_IDC_CREATEABLE)))
//      {
//              MST.WCheckCheck(GetLabel(CLSWIZ_IDC_CREATEABLE));
//              MST.WEditSetText(GetLabel(CLSWIZ_IDC_EXTERNAL_NAME),ExtName) ;
//              return ERROR_SUCCESS ;
//      }
//      return ERROR_ERROR ;    
	ASSERT(FALSE); return ERROR_ERROR;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::CheckAddToGallery(BOOL bCheck /*= TRUE */)
// Description: Checks or unchecks the Add To Object Gallery check box.
// Return: ERROR_SUCCESS or ERROR_ERROR if it failed to do the operation.
// Param: If bCheck is TRUE it makes sure the check box is check. Otherwise it uncheck it if necessary.
// END_HELP_COMMENT
int UIAddClassDlg::CheckAddToGallery(BOOL bCheck /*= TRUE */)
{
	if(MST.WCheckEnabled(GetLabel(CLSWIZ_IDC_ADD_TO_OG)))
	{
		if (bCheck)
			MST.WCheckCheck(GetLabel(CLSWIZ_IDC_ADD_TO_OG));
		else
			MST.WCheckUnCheck(GetLabel(CLSWIZ_IDC_ADD_TO_OG));

		return ERROR_SUCCESS ;
	}
	return ERROR_ERROR ;
}


// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::AttachDlgID(int index /*= 1 */)
// Description: Select the dialog (by ID) that's to be associated with the new class in the Create New Class dialog. (See AttachDlgID(CString) to associate dialog by name.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: index An integer that contains the 1-based index into the list of dialog templates used to specify the dialog associated with the new class.
// END_HELP_COMMENT
int UIAddClassDlg::AttachDlgID(int index /*= 1 */)
{
	if (MST.WComboEnabled(GetLabel(CLSWIZ_IDC_RESOURCE)))
 
	{
		MST.WComboSetFocus(GetLabel(CLSWIZ_IDC_RESOURCE) );
		MST.DoKeys("{F4}");
		MST.WComboItemClk(GetLabel(CLSWIZ_IDC_RESOURCE), index) ;
		return ERROR_SUCCESS ;
	}

	return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: int UIAddClassDlg::AttachDlgID(CString Name) 
// Description: Select the dialog (by name) that's to be associated with the new class in the Create New Class dialog. (See AttachDlgID(int) to associate dialog by index.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Name A CString that contains the name of the dialog template used to specify the dialog associated with the new class.
// END_HELP_COMMENT
int UIAddClassDlg::AttachDlgID(CString Name) 
{
if (MST.WComboEnabled(GetLabel(CLSWIZ_IDC_RESOURCE)))
 
	{
		MST.WComboSetFocus(GetLabel(CLSWIZ_IDC_RESOURCE) );
		MST.DoKeys("{F4}");
		MST.WComboItemClk(GetLabel(CLSWIZ_IDC_RESOURCE),Name) ;
		return ERROR_SUCCESS ;
	}

	return ERROR_ERROR ;

}

int UIImportOrCreateDlg::Show()
{
	HWND hWnd = ControlOnPropPage(CLSWIZ_ID_ADDCLASS) ;
	MST.WButtonClick(GetLabel(CLSWIZ_ID_ADDCLASS, hWnd,TRUE)) ;
	if(!WaitForWndWithCtrl(CLSWIZ_IDC_CREATECLASS,1500))
		return FALSE ;
	return TRUE ; 
}             

void UIImportOrCreateDlg::AddNew()
{
	MST.WOptionClick(GetLabel(CLSWIZ_IDC_CREATECLASS)) ;
	MST.WButtonClick(GetLabel(IDOK)) ;
	return ;
}             

void UIImportOrCreateDlg::AddImport() 
{
	MST.WOptionClick(GetLabel(CLSWIZ_IDC_IMPORT)) ; 
	MST.WButtonClick(GetLabel(IDOK)) ;
	return ;
}             

HWND UIImportOrCreateDlg::Cancel()
{
	return UIDialog::Cancel() ;
}             
           
int UIAutomationDlg::SetExtName(CString Name)
{
	// Remove the '_' from the Name.ddk_dkdkd_dkdk_dk

	CString temp = Name.SpanExcluding("_") ;
	int cnt ;
	while((cnt = Name.Find("_")) != -1)
	{       
		Name = Name.Right(Name.GetLength()-(cnt+1)) ;
		temp += Name.SpanExcluding("_") ;
	}
	Name = temp ;
	LPCSTR labelName = GetLabel(CLSWIZ_IDC_OLE_NAME); //delete this line
	MST.WComboSetText(GetLabel(CLSWIZ_IDC_OLE_NAME),Name) ;
	return ERROR_SUCCESS ;
}
int UIAutomationDlg::SetExtName(int Index)
{
	CString Name(' ',32);
	MST.WComboItemText(GetLabel(CLSWIZ_IDC_OLE_NAME),Index,Name) ;
	MST.WComboSetText(GetLabel(CLSWIZ_IDC_OLE_NAME),Name) ;
	return ERROR_SUCCESS ;
}

int UIAutomationDlg::SetImplementation(IMPLEMENTATION Type /* = STOCK*/) 
{
	
	switch (Type)
	{
		case STOCK:
			MST.WOptionClick(GetLabel(CLSWIZ_IDC_PREIMPLEMENT));
			break ;
		case CUSTOM:
			MST.WOptionClick(GetLabel(CLSWIZ_IDC_CUSTOM));
			break ;
		case MEMBER_VAR:
			MST.WOptionClick(GetLabel(CLSWIZ_IDC_MEMBERVAR));
			break ;
			
		case GET_SET_MEHOD:
			MST.WOptionClick(GetLabel(CLSWIZ_IDC_GETSET_METHODS));
			break ;
		default:
			MST.WOptionClick(GetLabel(CLSWIZ_IDC_CUSTOM));
	}
	return ERROR_SUCCESS ;
}

CString UIAutomationDlg::SetReturnType(int Index) 
{
	CString Name(' ',30) ;
	MST.WComboSetFocus(GetLabel(CLSWIZ_IDC_RETURN_TYPE));
	MST.DoKeys("{F4}");
	MST.WComboItemDblClk(GetLabel(CLSWIZ_IDC_RETURN_TYPE),Index) ;
	MST.WComboItemText(GetLabel(CLSWIZ_IDC_RETURN_TYPE),Index,Name) ;
	return Name ;
}


int UIAutomationDlg::GetStockCount() 
{       
	return MST.WComboCount(GetLabel(CLSWIZ_IDC_OLE_NAME)) ; 
}

int UIAutomationDlg::GetRetTypeCount() 
{       
	return MST.WComboCount((GetLabel(CLSWIZ_IDC_RETURN_TYPE))) ; 
}

int UIAutomationDlg::ParamTypeCount() 
{
	MST.WComboCount((GetLabel(CLSWIZ_IDC_RETURN_TYPE))) ; 
	MST.DoKeys("{DOWN}") ;
	MST.DoKeys("TEST");//"TEST is just a place holder so we can access the return types
	MST.DoKeys("{TAB}") ;
	return MST.WComboCount((GetLabel(CLSWIZ_IDC_RETURN_TYPE)));
}

int UIAutomationDlg::AddParameter(int Type, CString Name /*= "NULL"*/) 
{
	//Currently the Add Method dialog of the Class Wizard
	//hass problems.
	//Apparently the Parameter list has no string Label associated with it
	//so WListItemClk and WComboItemClk and WComboText fall fail because
	//(GetLabel(CLSWIZ_IDC_PARAM_LIST) returns NULL instead of a pointer to
	//&Parameter list: which is expected by looking at the Class Wizard's RC file
	//To make matters worse, if the External Name field is selected and F4 is used to
	//select a Member function name, then Alt-P doesn't switch
	//focus to the Parameter List
	//THEREFORE, you have focus in the Parameter list prior to calling AddParameter.
	//AND all parameters will be given type short.  Icky, huh?
	//A-RCahn 9-19-1997
	CString ParamName(' ',30) ;
//	MST.WListItemClk("&Parameter list:",Type) ;
//	MST.WListItemClk(GetLabel(CLSWIZ_IDC_PARAM_LIST),Type) ;
//	MST.DoKeys("TEST");
//	MST.DoKeys("{TAB}") ;
//	MST.DoKeys("%(p)");
	//These really aren't doing anything, but they should.
	MST.DoKeys("%(p)");
	MST.DoKeys("{DOWN}") ;
	MST.WComboItemClk("&Parameter list:",Type);
	if (Name == "NULL")
	{
		CString count ;
		count.Format("%d",Type);

		MST.WComboText(GetLabel(CLSWIZ_IDC_PARAM_LIST),ParamName) ;

		// Remove the "*" pointer specifier and replace it with Ptr.
		if(ParamName.Find("*") != -1)
		{       
			ParamName= ParamName.SpanExcluding("*") ;
			ParamName += "Ptr" ;
		}
		ParamName = ParamName +"Param"+count ;
	}
	else 
		ParamName = Name ;
	MST.DoKeys("{ENTER}") ;
	MST.DoKeys("A") ;
	MST.WEditSetText(NULL,ParamName) ;
	MST.DoKeys("{ENTER}") ;
	return ERROR_SUCCESS ;
}

int UIAutomationDlg::Create() 
{
	
	MST.WButtonClick("OK") ;
	return ERROR_SUCCESS ;
}

int UIMemVarDlg::CategoryCount() 
{
	
	return MST.WComboCount(GetLabel(CLSWIZ_IDC_PROPBOX)) ;
}

int UIMemVarDlg::SetVarName(CString Name) 
{
	Name = Name.SpanExcluding(" ") ;
	MST.WEditSetText(GetLabel(CLSWIZ_IDC_MEMBERNAME_EDIT),Name) ;
	return ERROR_SUCCESS ;
}

int UIMemVarDlg::SetCategory(int index) 
{
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_PROPBOX)) < index )
		return ERROR_ERROR ;

	CString Name;
	MST.WComboItemText(GetLabel(CLSWIZ_IDC_PROPBOX), index, Name) ;
	MST.WComboSetText(GetLabel(CLSWIZ_IDC_PROPBOX), Name) ;
	MST.WComboItemClk(GetLabel(CLSWIZ_IDC_PROPBOX), index) ;
	return ERROR_SUCCESS ;
}

CString UIMemVarDlg::GetCategory(int index) 
{
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_PROPBOX)) < index )
		return ERROR_ERROR ;

	CString Name;
	MST.WComboItemText(GetLabel(CLSWIZ_IDC_PROPBOX), index, Name) ;
	return Name ;
}


int UIMemVarDlg::SetType(int index)  
{
	
	if (MST.WComboCount(GetLabel(CLSWIZ_IDC_TYPEBOX)) < index )
		return ERROR_ERROR ;

	CString Name;
	MST.WComboItemText(GetLabel(CLSWIZ_IDC_TYPEBOX), index, Name) ;
	MST.WComboSetText(GetLabel(CLSWIZ_IDC_TYPEBOX), Name) ;
	MST.WComboItemClk(GetLabel(CLSWIZ_IDC_TYPEBOX), index) ;
	return ERROR_SUCCESS ;
}

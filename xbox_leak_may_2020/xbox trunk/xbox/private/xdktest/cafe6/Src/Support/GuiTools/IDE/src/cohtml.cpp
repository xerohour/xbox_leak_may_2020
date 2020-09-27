///////////////////////////////////////////////////////////////////////////////
//	COHTML.CPP
//
//	Created by :			Date :
//		MichMa					5/28/97
//
//	Description :
//		Implementation of the COHTML component object class
//

#include "stdafx.h"
#include "mstwrap.h"
#include "cohtml.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE

static char BASED_CODE THIS_FILE[] = __FILE__;


COHTML::COHTML()
{
}

// BEGIN_HELP_COMMENT
// Function: int COHTML::Create(LPCSTR szSaveAs /*=NULL*/, LPCSTR szProject /* NULL */)
// Description: Create a new html window and file in the IDE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szSaveAs A pointer to a string that contains the name to save the new file with. NULL indicates don't save (no name specified). (The default value is NULL.)
// Param: szProject A pointer to a string that contains the name of the project to add the file to. NULL indicates don't add it to project. (The default value is NULL.)
// END_HELP_COMMENT
int COHTML::Create(LPCSTR szSaveAs /*=NULL*/, LPCSTR szProject /* NULL */)
{
	return COFile::Create(FILE_HTML_PAGE, szSaveAs, szProject);
}


// BEGIN_HELP_COMMENT
// Function: int COHTML::InsertActiveXControl(LPCSTR szCtrl)
// Description: Insert a registered ActiveX Control into the current html page at the current cursor location.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szCtrl A pointer to a string that contains the name of the control to insert.
// END_HELP_COMMENT
int COHTML::InsertActiveXControl(LPCSTR szCtrl)

{
	// activate the "Insert ActiveX Control" dlg.
	if(!m_uiInsCtrlDlg.Activate())
	{
		LOG->RecordInfo("ERROR! COHTML::InsertActiveXControl - could not activate %s dlg.", m_uiInsCtrlDlg.ExpectedTitle());
	 	return ERROR_ERROR;
	}

	// select the control from the list.
	if(!m_uiInsCtrlDlg.SetControl(szCtrl))
	{
		LOG->RecordInfo("ERROR! COHTML::InsertActiveXControl - could set control to %s.", szCtrl);
	 	return ERROR_ERROR;
	}

	// OK the dlg.
	if(m_uiInsCtrlDlg.OK() != NULL)
	{
		LOG->RecordInfo("ERROR! COHTML::InsertActiveXControl - could not close %s dlg.", m_uiInsCtrlDlg.ExpectedTitle());
	 	return ERROR_ERROR;
	}

	// wait up to 10 seconds for the control to come up in the ide.	we do this by scanning for a static
	// text field that contains the name of the control. this field will be in a properties window for the
	// control that is displayed after the insert is complete.
	// TODO(michma): some sort of control editor ui class should take over from here.
	if(!MST.WFndWndWaitC(szCtrl, "Static", FW_PART, 10))
	{
		LOG->RecordInfo("ERROR! COHTML::InsertActiveXControl - could not detect control '%s' within ide.", szCtrl);
	 	return ERROR_ERROR;
	}

	return ERROR_SUCCESS;
}

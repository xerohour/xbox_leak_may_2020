///////////////////////////////////////////////////////////////////////////////
//  UPRJTDLG.CPP
//
//  Created by :            Date :
//      EnriqueP              2/7/94
//				  
//  Description :
//      Implementation of the UIProjOptionsTabDlg class	 
//

#include "stdafx.h"
#include "uprjtdlg.h"
#include "mstwrap.h"
#include "..\sym\cmdids.h"
#include "..\sym\optncplr.h"
#include "..\sym\optnlink.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "Strings.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: HWND UIProjOptionsTabDlg::Display() 
// Description: Bring up the Project Settings tabbed dialog.
// Return: The HWND of the dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIProjOptionsTabDlg::Display() 
{	
	AttachActive();		  // Check if is already up
	if (!IsValid())
	{
		UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	 	AttachActive();
	}
	return WGetActWnd(0); 
}


// BEGIN_HELP_COMMENT
// Function: int UIProjOptionsTabDlg::SelectFile (CString szFile,CString szGrp, CString szTarget ) 
// Description: Select a file in the Project Settings dialog to work with.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFile A CString that contains the name of the file to select.
// Param: szGrp A CString that contains the name of the group that contains the file to select.
// Param: szTarget A CString that contains the name of the target that contains the file to select.
// END_HELP_COMMENT
int UIProjOptionsTabDlg::SelectFile (CString szFile,CString szGrp, CString szTarget ) 
{
	if(!szTarget.IsEmpty())
		UIWB.SelectProjItem(PROJITEM_TARGET,szTarget) ;
	if(!szGrp.IsEmpty())
		UIWB.SelectProjItem(PROJITEM_GROUP,szGrp) ;
	UIWB.SelectProjItem(PROJITEM_FILE,szFile) ;
	return ERROR_SUCCESS ; 
}


// BEGIN_HELP_COMMENT
// Function: int UIProjOptionsTabDlg::SelectGroup(CString szGrp, CString szTarget) 
// Description: Select a group in the Project Settings dialog to work with.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szGrp A CString that contains the name of the group to select.
// Param: szTarget A CString that contains the name of the target that contains the group to select.
// END_HELP_COMMENT
int UIProjOptionsTabDlg::SelectGroup(CString szGrp, CString szTarget) 
{
	if(!szTarget.IsEmpty())
		UIWB.SelectProjItem(PROJITEM_TARGET,szTarget) ;
	UIWB.SelectProjItem(PROJITEM_GROUP,szGrp) ;
	return ERROR_SUCCESS ; 
}


// BEGIN_HELP_COMMENT
// Function: int UIProjOptionsTabDlg::SelectTarget(CString szTarget) 
// Description: Select a target in the Project Settings dialog to work with.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szTarget A CString that contains the name of the target to select.
// END_HELP_COMMENT
int UIProjOptionsTabDlg::SelectTarget(CString szTarget) 
{  	
	UIWB.SelectProjItem(PROJITEM_TARGET,szTarget) ;
	return ERROR_SUCCESS ; 
}


// BEGIN_HELP_COMMENT
// Function: HWND UIProjOptionsTabDlg::ShowCompilerTab(PROJSET_TAB tabCompiler /* = TAB_CPLUSPLUS */)
// Description: Display the compiler options tab in the Project Settings dialog.
// Return: The HWND of the compiler options tab, if successful; NULL otherwise.
// Param: tabCompiler A TAB specifier for the compiler to show.  Can be one of TAB_CPLUSPLUS or TAB_FORTRAN.  TAB_COMPILER can also be used and is equivalent to TAB_CPLUSPLUS for backwards compatibility.
// END_HELP_COMMENT
HWND UIProjOptionsTabDlg::ShowCompilerTab(PROJSET_TAB tabCompiler /* = TAB_CPLUSPLUS */)
{
	ASSERT(tabCompiler == TAB_CPLUSPLUS || 
		   tabCompiler == TAB_COMPILER || 
		   tabCompiler == TAB_FORTRAN);
	ShowPage(GetLocString(tabCompiler));

	return GetCurrentPage();
}


int GetCategory(UIProjOptionsTabDlg::PROJSET_TAB iTab, UINT iCtrlId)
{
	switch (iTab)
	{
		case UIProjOptionsTabDlg::TAB_DEBUG :
			switch (iCtrlId)
			{
				case 	DBG_IDC_CATEGORY: 
				case 	DBG_IDC_DEFAULT: 
				case 	DBG_IDC_EXE_FOR_SESSION:
				case 	DBG_IDC_WORKDIR:
				case 	DBG_IDC_ARGS:
				case 	DBG_IDC_REMOTE_TARGET:
							return DBG_GENERAL;

				case	DBG_IDC_TRY_LOCATE_DLLS:
							return DBG_ADDITIONAL_DLLS;
			
			}
			break;

		case UIProjOptionsTabDlg::TAB_COMPILER :
			switch (iCtrlId)
			{
				case 	CL_IDC_CATEGORY: 
				case	CL_IDC_DEFAULT: 
				case	CL_IDC_OPTIONS:
				case	CL_IDC_WARN_LEVEL:
				case	CL_IDC_DEBUG_INFO:
				case	CL_IDC_OPTIMIZATIONS:
				case	CL_IDC_WARN_AS_ERRORS:
				case	CL_IDC_GEN_BROWSE_INFO:
				case	CL_IDC_PREPROC_DEF : 
								return CL_GENERAL;

				case	CL_IDC_AUTOPCH:
				case	CL_IDC_AUTONAME:
				case	CL_IDC_USEPCH:
				case	CL_IDC_USENAME:
				case	CL_IDC_CREATEPCH:
				case	CL_IDC_CREATENAME:
								return CL_PRECOMPILE_H;

				case    CL_IDC_ADD_INCLUDE:
								if (GetLang() == CAFE_LANG_JAPANESE)
									return 7;
								return CL_PREPROCESSOR;		
			}
			break;

		case UIProjOptionsTabDlg::TAB_LINKER :
			switch (iCtrlId)
			{
				case 	LNK_IDC_CATEGORY: 
				case	LNK_IDC_DEFAULT: 
				case	LNK_IDC_OPTIONS:
				case	LNK_IDC_TARGET_NAME:
				case	LNK_IDC_OBJ_LIB_MOD:
				case	LNK_IDC_GEN_DEBUG_INFO:
				case	LNK_IDC_IGNORE_DEF_LIBS:
				case	LNK_IDC_LINK_INCREMENT:
				case	LNK_IDC_GEN_MAPFILE : 
				case	LNK_IDC_ENABLE_PROFILE :
							return LNK_GENERAL;

				case	LNK_IDC_ADDL_LIB_PATH :
							return LNK_INPUT;
			}
			break;

		case UIProjOptionsTabDlg::TAB_IMAGEBLD :
			switch (iCtrlId)
			{
				case 	IMAGEBLD_IDC_CATEGORY: 
				case	IMAGEBLD_IDC_DEFAULT: 
				case	IMAGEBLD_IDC_XBE_NAME:
				case	IMAGEBLD_IDC_AUTOCOPY_OFF:
							return IMAGEBLD_GENERAL;

				case	IMAGEBLD_IDC_TITLE_ID:
				case	IMAGEBLD_IDC_TITLE_NAME:
				case	IMAGEBLD_IDC_PUBLISHER_NAME:
							return IMAGEBLD_TITLE;
			}
			break;

		default:
			;
	}

	LOG->RecordFailure("Invalid control ID passed to GetCategory(%d, %d)", iTab, iCtrlId);
	return -1;
}


UIProjOptionsTabDlg::PROJSET_TAB StringIDToTab(PROJ_PROP_CATEGORIES ProjPropCat)
{
	switch (ProjPropCat)
	{
		//case CT_GENERAL			: return UIProjOptionsTabDlg::TAB_GENERAL;
		case CT_DEBUG			: return UIProjOptionsTabDlg::TAB_DEBUG;
		case CT_MAKE_TYPE_LIB	: return UIProjOptionsTabDlg::TAB_MAKE_TYPE_LIB;
		case CT_COMPILER		: return UIProjOptionsTabDlg::TAB_COMPILER;
		case CT_RESOURCE		: return UIProjOptionsTabDlg::TAB_RESOURCE;
		case CT_BROWSER			: return UIProjOptionsTabDlg::TAB_BROWSER;
		case CT_LINKER			: return UIProjOptionsTabDlg::TAB_LINKER;
		case CT_IMAGEBLD		: return UIProjOptionsTabDlg::TAB_IMAGEBLD;
		default:
			return UIProjOptionsTabDlg::TAB_GENERAL;
	}
}
	
// emmang@xbox xbox specific
// BEGIN_HELP_COMMENT
// Function: int UIProjOptionsTabDlg::SetProjOption(PROJ_PROP_CATEGORIES ProjPropCat, UINT iCtrlId, int iNewValue)
// Description: Set a value-based option for the currently selected target, group, or file in the Project Settings dialog. (See SetLinkerOption(UINT, LPCSTR) to set text-based linker options.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCtrlId An integer that contains the ID of the control for the option to set.
// Param: iNewValue An integer that contains the new value of the option.
// END_HELP_COMMENT
int UIProjOptionsTabDlg::SetProjOption(PROJ_PROP_CATEGORIES ProjPropCat, UINT iCtrlId, int iNewValue)
{
	ShowProjSettingsTab(StringIDToTab(ProjPropCat));
	SetOption(TAB_CURRENT, CL_IDC_CATEGORY, GetCategory(StringIDToTab(ProjPropCat), iCtrlId));
	return SetOption(TAB_CURRENT, iCtrlId, iNewValue);
}

// emmang@xbox xbox specific
// BEGIN_HELP_COMMENT
// Function: int UIProjOptionsTabDlg::SetProjOption(PROJ_PROP_CATEGORIES ProjPropCat, UINT iCtrlId, LPCSTR szNewValue)
// Description: Set a text-based option for the currently selected target, group, or file in the Project Settings dialog. (See SetLinkerOption(UINT, int) to set value-based linker options.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCtrlId An integer that contains the ID of the control for the option to set.
// Param: szNewValue A pointer to a string that contains the new value of the option.
// END_HELP_COMMENT
int UIProjOptionsTabDlg::SetProjOption(PROJ_PROP_CATEGORIES ProjPropCat, UINT iCtrlId, LPCSTR szNewValue)
{
	ShowProjSettingsTab(StringIDToTab(ProjPropCat));
	SetOption(TAB_CURRENT, CL_IDC_CATEGORY, GetCategory(StringIDToTab(ProjPropCat), iCtrlId));
	return SetOption(TAB_CURRENT, iCtrlId, szNewValue);
}
			

// emmang@xbox xbox specific
// BEGIN_HELP_COMMENT
// Function: int UIProjOptionsTabDlg::GetProjOption( UINT iCtrlId )
// Description: Get a value-based option for the currently selected target, group, or file in the Project Settings dialog. (See GetLinkerOptionStr(UINT) to get text-based linker options.)
// Return: An integer that contains the value of the linker option.
// Param: iCtrlId An integer that contains the ID of the control for the option to get.
// END_HELP_COMMENT
int UIProjOptionsTabDlg::GetProjOption(PROJ_PROP_CATEGORIES ProjPropCat, UINT iCtrlId)
{
	ShowProjSettingsTab(StringIDToTab(ProjPropCat));
	SetOption(TAB_CURRENT, CL_IDC_CATEGORY, GetCategory(StringIDToTab(ProjPropCat), iCtrlId));
	return GetOption(TAB_CURRENT, iCtrlId);
}

// emmang@xbox xbox specific
// BEGIN_HELP_COMMENT
// Function: LPCSTR UIProjOptionsTabDlg::GetProjOptionStr( UINT iCtrlId )
// Description: Get a text-based option for the currently selected target, group, or file in the Project Settings dialog. (See GetLinkerOptionStr(UINT) to get value-based linker options.)
// Return: A pointer to a string that contains the value of the linker option.
// Param: iCtrlId An integer that contains the ID of the control for the option to get.
// END_HELP_COMMENT
LPCSTR UIProjOptionsTabDlg::GetProjOptionStr(PROJ_PROP_CATEGORIES ProjPropCat, UINT iCtrlId)
{
	ShowProjSettingsTab(StringIDToTab(ProjPropCat));
	SetOption(TAB_CURRENT, CL_IDC_CATEGORY, GetCategory(StringIDToTab(ProjPropCat), iCtrlId));
	return GetOptionStr(TAB_CURRENT, iCtrlId);
}


// BEGIN_HELP_COMMENT
// Function: HWND UIProjOptionsTabDlg::ShowProjSettingsTab(void)
// Description: Display the  options tab in the Project Settings dialog that is indicate by iTab. see enum  PROJSET_TAB for a list of defines
// Return: The HWND of the options page tab, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIProjOptionsTabDlg::ShowProjSettingsTab(UINT iTab)
{
	// see enum  PROJSET_TAB for a list of defines
	ShowPage(GetLocString(iTab));

	return GetCurrentPage();
}

int UIProjOptionsTabDlg::ExpandConfig(void)
	{
	GiveFocusToConfigList();
	MST.DoKeys("{RIGHT}");
	return ERROR_SUCCESS;
	}


int UIProjOptionsTabDlg::CollapseConfig(void)
	{
	GiveFocusToConfigList();
	MST.DoKeys("{LEFT}");
	return ERROR_SUCCESS;
	}


int UIProjOptionsTabDlg::GiveFocusToConfigList(void)
	{
	MST.WListSetFocus("@1");
	// REVIEW (michma): need to add verification?
	return ERROR_SUCCESS;
	}

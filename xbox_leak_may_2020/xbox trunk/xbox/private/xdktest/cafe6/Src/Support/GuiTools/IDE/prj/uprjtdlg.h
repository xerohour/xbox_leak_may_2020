///////////////////////////////////////////////////////////////////////////////
//  UPRJTDLG.H
//
//  Created by :            Date :
//      EnriqueP              2/7/94
//
//  Description :
//      Declaration of the UIProjOptionsTabDlg class
//

#ifndef __UPRJTDLG_H__
#define __UPRJTDLG_H__

#include "..\sym\vproj.h"
#include "..\sym\optnrc.h"
#include "..\shl\uitabdlg.h"
#include "..\shl\wbutil.h"
// emmang@xbox.com
#include "..\sym\xboxsyms.h"

#include "prjxprt.h"

#ifndef __UITABDLG_H__
	#error include 'uitabdlg.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIProjOptionsTabDlg class

typedef enum
{
	CT_GENERAL			= VPROJ_IDC_PROJ_USEMFC,
	CT_EXT_DBGOPTS		= VPROJ_IDC_EXTOPTS_TARGET,
	CT_DEBUG			= VPROJ_IDC_PROG_WORKDIR,
	CT_MAKE_TYPE_LIB	= 19204, // REVIEW: get ID from OLE page
	CT_COMPILER			= 18723, // REVIEW: can't find control with this ID
	CT_RESOURCE			= OPTNRC_IDC_RESIGNPATH,
	CT_BROWSER			= VPROJ_IDC_BSCMAKE_DEFERRED, 
	CT_LINKER			= 19158, // REVIEW: can't find control with this ID
	CT_IMAGEBLD			= 15624
} PROJ_PROP_CATEGORIES ; 


//////////////////  GENERAL PAGE   //////////////////////////////////////////////
// Control ID's for General Page  ( original id's defined in vproj.h)
#define GEN_IDC_MFC				VPROJ_IDC_PROJ_USEMFC	 // Combo box
#define GEN_IDC_INTER_DIR		VPROJ_IDC_OUTDIRS_INT	 // Edit box
#define GEN_IDC_TARGET_DIR		VPROJ_IDC_OUTDIRS_TRG	 // Edit box

// Indexes for GEN_IDC_MFC combo box  in Directories Page
#define GEN_MFC_NOT_USED			1
#define GEN_MFC_STATIC				2
#define GEN_MFC_DLL					3

//////////////////  DEBUG PAGE   //////////////////////////////////////////////////////
// Control ID's for Debug Page  ( original id's defined in vproj.h)
#define DBG_IDC_CATEGORY		16112 /*VPROJ_IDC_MINI_PAGE*/	 	// Combo box
#define DBG_IDC_DEFAULT			16074 /*VPROJ_IDC_SET_DEFAULT*/	// Button
#define DBG_IDC_OPTIONS			16017 /*VPROJ_IDC_OPTSTR*/	 	// Multi line Edit box
#define DBG_IDC_EXE_FOR_SESSION	VPROJ_IDC_CALLING_PROG
#define DBG_IDC_WORKDIR			VPROJ_IDC_PROG_WORKDIR
#define DBG_IDC_ARGS			VPROJ_IDC_PROG_ARGS
#define DBG_IDC_REMOTE_TARGET	VPROJ_IDC_REMOTE_TARGET
#define DBG_IDC_TRY_LOCATE_DLLS VPROJ_IDC_LOAD_DLL_PROMPT

// **** Pre defined index values for Category combo
#define DBG_GENERAL				1		
#define DBG_ADDITIONAL_DLLS		2

//////////////////  MAKE TYPE LIB PAGE   //////////////////////////////////////////////
// Control ID's for Make Type Lib Page  ( original id's defined in vproj.h)



//////////////////  C/C++ COMPILER PAGE   //////////////////////////////////////////////
// Control ID's for C/C++ Compiler Page  ( original id's defined in vproj.h)
#define CL_IDC_CATEGORY			16112 /*VPROJ_IDC_MINI_PAGE*/	 	// Combo box
#define CL_IDC_DEFAULT			16074 /*VPROJ_IDC_SET_DEFAULT*/	// Button
#define CL_IDC_OPTIONS			16017 /*VPROJ_IDC_OPTSTR*/	 	// Multi line Edit box

// **** Pre defined index values for Category combo
#define CL_GENERAL				1		
#define CL_INTELX86				2
#define CL_CUSTOMIZE			3
#define CL_CUSTOMIZE_CXX		4
#define CL_LIST_FILES			5
#define CL_OPTIMIZATION			6
#define CL_PRECOMPILE_H			7
#define CL_PREPROCESSOR			8


// ********   General Mini Page control Id's

#define CL_IDC_WARN_LEVEL		18723	 	// Combo box
#define CL_IDC_DEBUG_INFO		18784		// Combo box
#define CL_IDC_OPTIMIZATIONS	18832	 	// Combo box
#define CL_IDC_WARN_AS_ERRORS	18724	 	// Check box
#define CL_IDC_GEN_BROWSE_INFO	18816		// Check box
#define CL_IDC_PREPROC_DEF		18896	 	// Edit Box		

// **** Pre defined values

#define CL_WARN_LEVEL_NONE		1		// Warning level combo indexes
#define CL_WARN_LEVEL_1			2
#define CL_WARN_LEVEL_2			3
#define CL_WARN_LEVEL_3			4
#define CL_WARN_LEVEL_4			5

// ********   Precompiled Headers controls ID's

#define CL_IDC_AUTOPCH			OPTNCPLR_IDC_AUTOPCH	 		// Check box
#define CL_IDC_AUTONAME			OPTNCPLR_IDC_AUTOPCHNAME	 	// Edit box
#define CL_IDC_CREATEPCH		OPTNCPLR_IDC_CREATEPCH	 		// Check box
#define CL_IDC_CREATENAME		OPTNCPLR_IDC_CREATENAME	 		// Edit box
#define CL_IDC_USEPCH			OPTNCPLR_IDC_USEPCH		 		// Check box
#define CL_IDC_USENAME			OPTNCPLR_IDC_USENAME	 		// Edit box
 
// ********   Preprocesser control ID's (emmang@xbox.com)

#define CL_IDC_ADD_INCLUDE		OPTNCPLR_IDC_INCLUDE


//////////////////  RESOURCE COMPILER PAGE   //////////////////////////////////////////////
// Control ID's for Resource Compiler Page  ( original id's defined in vproj.h)

//////////////////  BROWSE INFO PAGE   //////////////////////////////////////////////
// Control ID's for Browse Info Page  ( original id's defined in vproj.h)

//////////////////  LINKER PAGE   //////////////////////////////////////////////
// Control ID's for Linker Page  ( original id's defined in optnlink.h)

#define LNK_IDC_CATEGORY		16112 /*VPROJ_IDC_MINI_PAGE*/	 	// Combo box
#define LNK_IDC_DEFAULT			16074 /*VPROJ_IDC_SET_DEFAULT*/	// Button
#define LNK_IDC_OPTIONS			16017 /*VPROJ_IDC_OPTSTR*/	 	// Multi line Edit box

// **** Pre defined index values for Category combo
#define LNK_GENERAL				1		
#define LNK_CUSTOMIZE			2
#define LNK_DEBUG				3
#define LNK_INPUT				4
#define LNK_IMAGE_ATTRIBS		5


// ********   General Mini Page control Id's

#define LNK_IDC_TARGET_NAME			OPTNLINK_IDC_OUT	 		// Edit Box
#define LNK_IDC_OBJ_LIB_MOD			OPTNLINK_IDC_LIBS			// Edit Box
#define LNK_IDC_GEN_DEBUG_INFO		OPTNLINK_IDC_LNK_DBGFULL	// Check box
#define LNK_IDC_IGNORE_DEF_LIBS		OPTNLINK_IDC_EXLIB		 	// Check box
#define LNK_IDC_LINK_INCREMENT		OPTNLINK_IDC_INCLINK		// Check box
#define LNK_IDC_GEN_MAPFILE			OPTNLINK_IDC_MAPGEN		 	// Check box		
#define LNK_IDC_ENABLE_PROFILE		OPTNLINK_IDC_PROFILE	 	// Check box	

// added by emmang@xbox.com
#define LNK_IDC_MODULES				19088
#define LNK_IDC_ADDL_LIB_PATH		19092

// Project items on the Project Settings dialog listbox .
enum { PROJITEM_TARGET = 0, PROJITEM_GROUP, PROJITEM_FILE } ;

///////////////////////////////////////////////////////////////////////////////
 
// BEGIN_CLASS_HELP
// ClassName: UIProjOptionsTabDlg
// BaseClass: UITabbedDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIProjOptionsTabDlg : public UITabbedDialog 
	{
    UIWND_COPY_CTOR (UIProjOptionsTabDlg, UITabbedDialog) ; 

// General Utilities
public:
	inline CString ExpectedTitle(void) const
	{ return GetLocString(IDSS_PROJ_OPTIONS_TITLE); }	   // Project Options
	
	HWND Display(void) ;

// Configuration pane utilities.
public:
	int ExpandConfig(void);
	int CollapseConfig(void);
	int GiveFocusToConfigList(void);
			
// Compiler Page Utilities
public:

	//	Project Settings tabs.  This enumeration sets up a mapping of 
	//  string IDs to symbols used when choosing a particular dialog
	//  tab to display for Project Settings.  Note that some symbols
	//  are identically defined in order to keep existing tests
	//  that relied on them from breaking.  The strings are derived
	//  from the window captions of the tabs.
	enum  PROJSET_TAB
	{
				TAB_GENERAL =        IDSS_PROJSET_GENERAL, 
				TAB_EXT_DBGOPTS =    IDSS_PROJSET_DEBUG, 
				TAB_DEBUG =          IDSS_PROJSET_DEBUG,
				TAB_CUSTBUILD =      IDSS_PROJSET_CUSTBUILD,
				TAB_COMPILER =       IDSS_PROJSET_CPLUSPLUS,
				TAB_CPLUSPLUS =      IDSS_PROJSET_CPLUSPLUS, 
				TAB_FORTRAN =        IDSS_PROJSET_FORTRAN,
				TAB_LINKER =         IDSS_PROJSET_LINKER,
				TAB_RESOURCE =       IDSS_PROJSET_RESOURCES, 
				TAB_MAKE_TYPE_LIB =  IDSS_PROJSET_TYPE_LIB, 
				TAB_BROWSER =        IDSS_PROJSET_BROWSER,
				TAB_IMAGEBLD =       IDSS_PROJSET_IMAGEBLD,
	};

	int SetProjOption(PROJ_PROP_CATEGORIES, UINT iCtrlId, int iNewValue) ;
	int SetProjOption(PROJ_PROP_CATEGORIES, UINT iCtrlId, LPCSTR szNewValue) ;
	int GetProjOption(PROJ_PROP_CATEGORIES, UINT iCtrlId);
	LPCSTR GetProjOptionStr(PROJ_PROP_CATEGORIES, UINT iCtrlId);

	HWND ShowCompilerTab(PROJSET_TAB = TAB_CPLUSPLUS);

	int SelectFile (CString szFileName,CString szGrpName = "", CString szTarget = "" );
	int SelectGroup(CString szGrpName, CString szTarget = ""  ) ;
	int SelectTarget(CString szTarget);


public:
	// Utility Project Tab methods
	HWND ShowProjSettingsTab(UINT iTab);	
   } ;

#endif //__UPRJTDLG_H__

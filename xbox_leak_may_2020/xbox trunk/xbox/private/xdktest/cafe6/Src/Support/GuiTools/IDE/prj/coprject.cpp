///////////////////////////////////////////////////////////////////////////////
//  COPRJECT.CPP
//
//  Created by :            Date :
//      IvanL               1/17/94
//
//  Description :
//      Implementation of the COProject class
//

#include "stdafx.h"
#include <process.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "coprject.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\sym\cmdids.h"
#include "..\sym\vproj.h"
#include "..\sym\props.h"
#include "..\sym\projprop.h"
#include "..\sym\vproj.h"
#include "..\sym\vres.h"
#include "..\sym\optnlink.h"
#include "..\sym\xboxsyms.h"
#include "guiv1.h"
#include "..\..\umsgbox.h"
#include "..\shl\ucommdlg.h"
#include "..\shl\uioutput.h"
#include "..\shl\coconnec.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"
#include "..\wrk\uiwrkspc.h"
#include "uappwiz.h"
#include "uitarget.h"
#include "uprjfile.h"
#include "uidepdlg.h"
#include "..\shl\uiwbmsg.h"
#include "..\shl\uopenwrk.h"
#include "..\..\testutil.h"
#include "..\shl\newfldlg.h"

#include "guitarg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// macro that determines whether the COProject object is valid
#define VERIFY_VALID_PROJECT(message)			\
	ASSERT(IsOpen());							\
	if (!IsOpen()) {							\
		LOG->RecordInfo("%s: the project is not opened. %s", THIS_FUNCTION, (LPCSTR)(message));  \
		return ERROR_ERROR;						\
	}

#define MAXFILEPATH _MAX_DRIVE+_MAX_DIR+_MAX_FNAME+_MAX_EXT
CString MyGetLabel(UINT id) ;

CString GROUP_WAS_OMITTED ="&*%#~**********@+==" ;

// The following defines are here temporarily until PROJPROP.H is updated. REVIEW(IVANL) remove ASAP.
#define P_QA_ProjItemType                       PROJ_FAKE + 95
#define QA_TypeIs_Target                        0
#define QA_TypeIs_Group                         1
#define QA_TypeIs_File                          2
#define QA_TypeIs_DepGroup                      3
#define QA_TypeIs_DepFile                       4

APPWIZ_OPTIONS::APPWIZ_OPTIONS()
{
	 m_bUseMFCDLL = TRUE;
		m_OLECompound = AW_OLE_NONE;

		// Initialize all currently unused members to known value.
		m_Type = m_DBase = m_OLEAuto = m_ToolBar = m_StatusBar = m_Printing 
		= m_Help = m_Language = m_Comments = m_VCMakefile = m_AboutBox = 0;
}

CMemberInfo::~CMemberInfo()
{
	ClearAll();
}

CClassInfo::~CClassInfo()
{
	ClearAll();
}

void CClassInfo::ClearAll()
{ 
    m_strName.Empty(); 
    m_strBase.Empty(); 

	POSITION pos = m_listMembers.GetHeadPosition();
	while(pos != NULL)
	{
		delete m_listMembers.GetAt(pos);
		m_listMembers.GetNext(pos);
	}

	m_listMembers.RemoveAll();
}

void CMemberInfo::ClearAll()
{ 
    m_strName.Empty(); 
	m_strDataType.Empty();
}

 
COProject::COProject(void)
: m_pProjWnd(NULL)
{
}

COProject::~COProject()
{
	// delete the project window
	DeleteProjectWindow();
}


// BEGIN_HELP_COMMENT
// Function: int COProject::SelectDependenciesGroup(CString Group /* = ""*/)
// Description: Select the dependencies folder for a particular group in the project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Group A CString that contains the name of the group to select the dependencies folder for. An empty string means use the current group.
// END_HELP_COMMENT
int COProject::SelectDependenciesGroup(CString Group /* = ""*/)
{
	const char* const THIS_FUNCTION = "COProject::SelectDependeciesGroup";

	VERIFY_VALID_PROJECT("Unable to select dependencies group.")

	CString szCur = "", Prev = "" ;
	// make project window active.
	ActivateProjWnd();
	// if a dependencies group is currently selected.
	if (Group.IsEmpty())
		Group = UIWB.GetItemProperty(P_GroupName) ;       // Get active groupname
	if (ERROR_ERROR == SelectGroup((const char *) Group)) // If group couldn't be found
		return ERROR_ERROR ;
	MST.DoKeys("{ENTER}") ;
	for (int i = 1; i ;)
		{
			MST.DoKeys("{DOWN}") ;
			szCur = UIWB.GetItemProperty(P_QA_ProjItemType) ;
			if ((atoi(szCur)) == QA_TypeIs_DepGroup)
				return ERROR_SUCCESS ;
			szCur = UIWB.GetItemProperty(P_ProjItemName) ;
			if (Prev == szCur) // We are at the end
				return ERROR_ERROR ;
			Prev = szCur ;
		}
	return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::SelectDependentFile(CString File,CString Group /* = ""*/)
// Description: Select a file in the dependencies folder for a particular group in the project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: File A CString that contains the name of the file to select.
// Param: Group A CString that contains the name of the group that contains the dependencies folder. An empty string means use the current group.
// END_HELP_COMMENT
int COProject::SelectDependentFile(CString File,CString Group /* = ""*/)
{
	const char* const THIS_FUNCTION = "COProject::SelectDependentFile";

	VERIFY_VALID_PROJECT("Unable to select dependent file.")

	if (ERROR_ERROR == SelectDependenciesGroup(Group))
		return ERROR_ERROR ;
	CString Prev = "", szCur = "" ;
	MST.DoKeys("{ENTER}") ; //EXPAND DEP GROUP
	for (int i = 1; i ; )
		{
			CString Prev, szCur ;
			MST.DoKeys("{DOWN}") ;
			szCur = UIWB.GetItemProperty(P_ProjItemName) ;
			if (szCur == File)
				return ERROR_SUCCESS ;
			if (Prev == szCur) // We are at the end
				return ERROR_ERROR ;
			Prev = szCur ;
		}
	return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::NewGroup(CString Name)
// Description: Add a new group to the current project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Name A CString that contains the name of the new group.
// END_HELP_COMMENT
int COProject::NewGroup(CString Name)
{
	const char* const THIS_FUNCTION = "COProject::NewGroup";

	VERIFY_VALID_PROJECT("Unable to create a new group.")

	UIWB.DoCommand(IDM_PROJITEM_ADD_GROUP , DC_MNEMONIC);   // make project editor MDI active
	if (Name.IsEmpty())
	{
		MST.DoKeys("{ESC}") ;
		return ERROR_ERROR ;
	}
	if (!UIWB.WaitOnWndWithCtrl(VPROJ_IDC_GROUPNAME,3000)) // If the dialog didn't show up
		return ERROR_ERROR ;
	MST.WEditSetText(GetLabel(VPROJ_IDC_GROUPNAME),(const char *) Name);// uffer(Name.GetLength())) ;
	MST.WButtonClick(GetLabel(IDOK)) ;
	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: BOOL COProject::ActivateProjWnd()
// Description: Activate the project window.
// Return: UIMainFrame::DoCommand()
// END_HELP_COMMENT
BOOL COProject::ActivateProjWnd()
{
    return UIWB.DoCommand(IDM_VIEW_WORKSPACE, DC_MNEMONIC);
}

// BEGIN_HELP_COMMENT
// Function: int COProject::Close(int iSave)
// Description: Close the project and project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iSave A Boolean value that indicates whether to save the project before closing (TRUE) or not.
// END_HELP_COMMENT
int COProject::Close(int iSave)
{
	const char* const THIS_FUNCTION = "COProject::Close";

	VERIFY_VALID_PROJECT("Unable to close the project.")

	// Make sure that the project window has focus.
	ActivateProjWnd();
	if(iSave==TRUE && Save()!=ERROR_SUCCESS)
		return ERROR_ERROR;
	return UIWB.CloseProject()? ERROR_SUCCESS: ERROR_ERROR;


/* 	
	//if (UIWB.GetItemProperty(P_ProjIsExternalMakeFile) == "1")
	//      return ERROR_SUCCESS ;
	if (iSave)
		Save() ;
	UIWB.DoCommand(ID_FILE_CLOSE, DC_MNEMONIC);

	UIWBMessageBox mb;
	while( mb.WaitAttachActive(3000) != NULL )
	{
		if( mb.ButtonExists(MSG_NO) )
			// this will handle "do you want to save" msg box
			mb.ButtonClick(MSG_NO);
		else
			// this will handle "access denied" msg box for read-only projects
			MST.DoKeys("~");
	}

	return ERROR_SUCCESS;
*/
}

int COProject::Attach(void)
{
	// bring up the project window
	NewProjectWindow();

	// determine if the window is valid
	return ERROR_SUCCESS;
}


// BEGIN_HELP_COMMENT
// Function: int COProject::GetErrorCount()
// Description: Get the number of errors reported by the last build of the project.
// Return: An integer containing the number of errors.
// END_HELP_COMMENT
int COProject::GetErrorCount()
{
	const char* const THIS_FUNCTION = "COProject::GetErrorCount";

	VERIFY_VALID_PROJECT("Unable to get the error count.")

	return UIWB.GetErrorCount() ;
}
// BEGIN_HELP_COMMENT
// Function: int COProject::GetWarningCount()
// Description: Get the number of warning reported by the last build of the project.
// Return: An integer containing the number of warnings.
// END_HELP_COMMENT
int COProject::GetWarningCount()
{
	const char* const THIS_FUNCTION = "COProject::GetWarningCount";

	VERIFY_VALID_PROJECT("Unable to get the warning count.")

	return UIWB.GetWarningCount() ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::VerifyBuild(BOOL LogErrors /* TRUE */, int * iErr /* NULL */, int * iWarn /* NULL */,  BOOL WrnsAsErrs /*= FALSE*/)
// Description: Determine whether the last build of the project was successful or not. An unsuccessful build contains errors (or warnings treated as errors).
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: LogErrors A Boolean value that indicates whether to record the results of the last build in the log (TRUE) or not.
// Param: iErr A pointer to an integer that will contain the number of errors reported by the last build.
// Param: iWarn A pointer to an integer that will contain the number of warnings reported by the last build.
// Param: WrnsAsErrs A Boolean value that indicates whether to treat warnings as errors (TRUE) or not. (Default value is FALSE.)
// END_HELP_COMMENT
int COProject::VerifyBuild(BOOL LogErrors /* TRUE */, int * iErr /* NULL */, int * iWarn /* NULL */,  BOOL WrnsAsErrs /*= FALSE*/)
{
	const char* const THIS_FUNCTION = "COProject::VerifyBuild";

	VERIFY_VALID_PROJECT("Unable to verify the build.")

    int Error = UIWB.GetErrorCount() ;
	int Warn  = UIWB.GetWarningCount() ;

	if (iErr)
		*iErr = Error ;
	if (iWarn)
	{       CString msg = " There were Warnings: " ;
		char szwn[] = "               " ;
		_itoa(*iWarn,szwn,10) ;
		msg + szwn ;
		LOG->RecordInfo((const char *) msg) ;
		*iWarn =Warn ;
	}
	if (Error || (Warn && WrnsAsErrs))
	{
		if (LogErrors)
		{
			char szTmpPath[MAX_PATH];
			if (GetTempPath(MAX_PATH, szTmpPath) == 0)
			{
				LOG->RecordInfo("COProject::VerifyBuild() - Couldn't get path to temp directory.  Build file not created");
				AfxThrowFileException(CFileException::generic);
				return ERROR_ERROR;
			}

			CString strBuildLog=CString(szTmpPath)+CTime::GetCurrentTime().Format("%y%H%M%S.%j");
			// CString strBuildLog;
			// GetTempFileName(szTmpPath, "prj", 0, strBuildLog.GetBuffer(MAX_PATH));
			// strBuildLog.ReleaseBuffer();
			// Save Output Window
			UIOutput OutWin;
			OutWin.Activate();
			UIFileSaveAsDlg SaveOutWin = UIWB.FileSaveAsDlg();
		    EXPECT( SaveOutWin.IsValid() );
			SaveOutWin.SetName(strBuildLog);
			SaveOutWin.OK(TRUE);
			LOG->RecordTextFile(strBuildLog, "Build log");
			ActivateProjWnd();
		}
		return ERROR_ERROR ;
	}
	else
		return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::WaitUntilBuildDone(int Min /* 3 */,DWORD dwSleep=1000)
// Description: Wait until the current build is finished or the amount of time specified has elasped. 
// Return: An integer that indicates success. ERROR_SUCCESS if the build completes within the time specified; ERROR_ERROR otherwise.
// Param: Min An integer that contains the number of minutes to wait for the build to complete.
// Param: dwSleep An DWORD that contains the number of milliseconds to wait between checks to see if build is done
// END_HELP_COMMENT
int COProject::WaitUntilBuildDone(int Min /* 3 */, DWORD dwSleep /* 1000 */)
{	DWORD dwMillSec = Min * 60000 ;   
	for ( DWORD dwDone = 0; dwDone <= dwMillSec; dwDone += dwSleep)
	{	
		Sleep(dwSleep);		// wait 
		if (UIWB.CanWeBuild())    // If the IDE is in a CanBuild state, we are done building
		{
		   		MST.DoKeys("{ESC}") ;
				Sleep(2000) ;
		   		return ERROR_SUCCESS ;
		}	
	}
	MST.DoKeys("{ESC}") ;
	return ERROR_ERROR;	

	// EventHandler
//	if(pEventHandler->WaitForBuildFinish(Min) == true)  
//		return ERROR_SUCCESS;
//	else
//		return ERROR_ERROR;
}


// BEGIN_HELP_COMMENT
// Function: int COProject::New(PROJOPTIONS &ProjOpts)
// Description: Create a new project and project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: ProjOpts A PROJOPTIONS structure that defines the project. szName is a pointer to a string that contains the name of the project. szDir is a pointer to a string that contains the directory for the project. iType is a value that indicates the type of project: UIAW_PT_APPWIZ, UIAW_PT_APPWIZ_DLL, UIAW_PT_APP, UIAW_PT_DLL, UIAW_PT_CONSOLE, UIAW_PT_STATIC_LIBRARY, UIAW_PT_DUMMY_EXEC, UIAW_PT_DUMMY_DLL. szNewSubDir is a pointer to a string that contains the new subdirectory for the project. ProjFileOpt is a value that indicates which files to add: ADD_ALL_SOURCE, ADD_ALL_HEADERS, ADD_ALL_RES_SCRIPTS, ADD_ALL_DEF_FILES, ADD_ALL_ODL_FILES, ADD_ALL_LIBS, ADD_ALL_OBJ_FILES, ADD_ALL_FILES, ADD_FILE, ADD_FILES_MYSELF. szProjFilesPath is a pointer to a string containing the path that contains the project files to be added. szRemotePath is a pointer to a string that contains the remote executable path for this project. szRemoteFile is a pointer to a string that contains the remote executable filename for this project.
// END_HELP_COMMENT
int COProject::New(PROJOPTIONS &ProjOpts)
{   
	const char* const THIS_FUNCTION = "COProject::New";

    HWND hWnd;

	if (!ProjOpts.szName)
	{
		LOG->RecordInfo("%s: no project name given. Unable to create a new project.", THIS_FUNCTION);
		MST.DoKeys("{ESC 2}") ; // Just close the dialog.
		return ERROR_ERROR ;
	}
	else
	{
		
		hWnd = CreateNewProject(ProjOpts.szName,
								ProjOpts.iType,
								ProjOpts.szDir,
								ProjOpts.szNewSubDir,
								ProjOpts.ProjFileOpt,
								ProjOpts.szProjFilesPath,
								ProjOpts.szRemotePath,
								ProjOpts.szRemoteFile ) ;
	}

	SetPathMembers() ;
	if(!(IsWindow(hWnd)))
		{return ERROR_ERROR ;}
	Sleep(3) ;
	MST.DoKeys("{ESC}") ;

	//if (MST.WFndWnd("Add to Source",FW_DEFAULT | FW_PART | FW_ACTIVE))
	Sleep(3);
	MST.DoKeys("{ESC}");
		
	SetPathMembers() ;
	// Add files if user asks for that.
/*
	if( ProjFileOpt & CNP_ADD_ALL_SOURCE )
		upf.AddAllFiles(UIPF_FOT_SOURCE, szProjFilesPath);

	if (ProjFileOpt & CNP_ADD_FILE)
		upf.AddFile(szProjFilesPath);
*/
	// the project should now be valid
	ASSERT(IsOpen());

	m_hWnd = hWnd ;

	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::New(LPCSTR szProjName, int ProjType, LPCSTR szDir /* NULL */, LPCSTR szNewSubDir /* NULL */, int ProjFileOpt /* 0 */, LPCSTR szProjFilesPath /* NULL */, LPCSTR szRemotePath /* NULL */, LPCSTR szRemoteFile /* NULL */)
// Description: Create a new project and project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szProjName A pointer to a string that contains the name of the project.
// Param: ProjType is a value that indicates the type of project: UIAW_PT_APPWIZ, UIAW_PT_APPWIZ_DLL, UIAW_PT_APP, UIAW_PT_DLL, UIAW_PT_CONSOLE, UIAW_PT_STATIC_LIBRARY, UIAW_PT_DUMMY_EXEC, UIAW_PT_DUMMY_DLL.
// Param: szDir A pointer to a string that contains the directory for the project. (Default value is NULL.)
// Param: szNewSubDir A pointer to a string that contains the new subdirectory for the project.  This subdirectory will be based off szDir, so it cannot contain root, UNC, or drive specifiers. (Default value is NULL.)
// Param: ProjFileOpt A value that indicates which files to add: ADD_ALL_SOURCE, ADD_ALL_HEADERS, ADD_ALL_RES_SCRIPTS, ADD_ALL_DEF_FILES, ADD_ALL_ODL_FILES, ADD_ALL_LIBS, ADD_ALL_OBJ_FILES, ADD_ALL_FILES, ADD_FILE, ADD_FILES_MYSELF. (Default value is 0.)
// Param: szProjFilesPath A pointer to a string containing the path that contains the project files to be added. (Default value is NULL.)
// Param: szRemotePath A pointer to a string that contains the remote executable path for this project. (Default value is NULL.)
// Param: szRemoteFile A pointer to a string that contains the remote executable filename for this project. (Default value is NULL.)
// END_HELP_COMMENT
int COProject::New(LPCSTR szProjName, 
				   PROJTYPE ProjType,
				   	LPCSTR szDir /* NULL */,
					LPCSTR szNewSubDir /* NULL */,
					int ProjFileOpt /* 0 */,
					LPCSTR szProjFilesPath /* NULL */,
					LPCSTR szRemotePath /* NULL */,
					LPCSTR szRemoteFile /* NULL */)
{
	const char* const THIS_FUNCTION = "COProject::New";

	HWND hwnd = CreateNewProject(szProjName, ProjType, szDir, szNewSubDir,ProjFileOpt,
								szProjFilesPath, szRemotePath, szRemoteFile);


	// the project should be valid
	ASSERT(IsOpen());

	if((ProjType == UIAW_PT_APPWIZ_DLL )|| (ProjType ==UIAW_PT_APPWIZ))
	{
		if (hwnd)
		{
			m_hWnd = hwnd ;
			SetPathMembers() ;
			return ERROR_SUCCESS ;
		}
		else
			return ERROR_ERROR ;
	}
	
	Sleep(2000) ;
	SetPathMembers() ;
	CString strTitle;
	WGetText(WGetActWnd(0), strTitle.GetBuffer(MAX_PATH));
	strTitle.ReleaseBuffer();
	strTitle.MakeUpper();
	if (strTitle.Find(GetName()) == -1) {
		LOG->RecordInfo("%s: Verifying project was created. Expected project name '%s' in title bar '%s'.", THIS_FUNCTION, GetName(), strTitle);
		return ERROR_ERROR ;
	}
	else
	{
		m_hWnd = hwnd ;
		return ERROR_SUCCESS ;
	}
}


// BEGIN_HELP_COMMENT
// Function: int COProject::BuildNew(LPCSTR szProjName, PROJTYPE ProjType, LPCSTR szDir /* NULL */, LPCSTR szNewSubDir /* NULL */, int ProjFileOpt /* 0 */, LPCSTR szProjFilesPath /* NULL */, LPCSTR szRemotePath /* NULL */, LPCSTR szRemoteFile /* NULL */)
// Description: Create a new project and build it.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szProjName A pointer to a string that contains the name of the project.
// Param: ProjType is a value that indicates the type of project: UIAW_PT_APPWIZ, UIAW_PT_APPWIZ_DLL, UIAW_PT_APP, UIAW_PT_DLL, UIAW_PT_CONSOLE, UIAW_PT_STATIC_LIBRARY, UIAW_PT_DUMMY_EXEC, UIAW_PT_DUMMY_DLL.
// Param: szDir A pointer to a string that contains the directory for the project. (Default value is NULL.)
// Param: szNewSubDir A pointer to a string that contains the new subdirectory for the project. (Default value is NULL.)
// Param: ProjFileOpt A value that indicates which files to add: ADD_ALL_SOURCE, ADD_ALL_HEADERS, ADD_ALL_RES_SCRIPTS, ADD_ALL_DEF_FILES, ADD_ALL_ODL_FILES, ADD_ALL_LIBS, ADD_ALL_OBJ_FILES, ADD_ALL_FILES, ADD_FILE, ADD_FILES_MYSELF. (Default value is 0.)
// Param: szProjFilesPath A pointer to a string containing the path that contains the project files to be added. (Default value is NULL.)
// Param: szRemotePath A pointer to a string that contains the remote executable path for this project. (Default value is NULL.)
// Param: szRemoteFile A pointer to a string that contains the remote executable filename for this project. (Default value is NULL.)
// END_HELP_COMMENT
int COProject::BuildNew(LPCSTR szProjName, PROJTYPE ProjType,
						 LPCSTR szDir /* NULL */,
						 LPCSTR szNewSubDir /* NULL */,
						 int ProjFileOpt /* 0 */,
						 LPCSTR szProjFilesPath /* NULL */,
						 LPCSTR szRemotePath /* NULL */,
						 LPCSTR szRemoteFile /* NULL */)
{
/*      EXPECT(IsWindow(CreateNewProject(szProjName, ProjType, szDir, szNewSubDir,
										  ProjFileOpt, szProjFilesPath, szRemotePath,
	REVIEW: why does this fail?                       szRemoteFile)));*/

	New(szProjName, ProjType, szDir, szNewSubDir, ProjFileOpt, szProjFilesPath, szRemotePath, szRemoteFile);

	// the project should be valid
	ASSERT(IsOpen());

	if(GetUserTargetPlatforms() == PLATFORM_MAC_68K)
		{
		Build();
		return UpdateRemoteFile();
		}

	return Build();
}


// BEGIN_HELP_COMMENT
// Function: int COProject::NewProject(PROJTYPE ptProjType, LPCSTR szProjName, LPCSTR projdir,int intPlatforms, BOOL AddToWorkspace /*=FALSE*/)
// Description: Create a new project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szProjType A PROJTYPE value that indicates the type of project to create.
// Param: szProjName A pointer to a string that contains the name of the project.
// Param: intPlatforms A combination of PLATFORM_TYPE values that indicates the platforms the project will target.
// Param: ProjDir A pointer to a string that contains the location for the project.
// Param: AddToWorkspace TRUE - if want to add it to the existing workspace, FALSE if create new workspace.
// END_HELP_COMMENT
int COProject::NewProject(PROJTYPE ptProjType, LPCSTR szProjName, LPCSTR projdir, 
						  int intPlatforms, BOOL AddToWorkspace /*=FALSE*/)
{
	const char* const THIS_FUNCTION = "COProject::NewProject";

	// bring up the "new project" dlg.
	UIProjectWizard uprjwiz = AppWizard();
 	
	// make sure the dlg came up.
	if(!uprjwiz.HWnd())
		{		
		LOG->RecordInfo("ERROR in %s: can't activate new project dlg", THIS_FUNCTION);
		return ERROR_ERROR;
		}

	// set the project info and complete creation.
	Sleep(1000);	// BJoyce: we are too fast
	uprjwiz.SetProjType(ptProjType);
	uprjwiz.SetName(szProjName);
	uprjwiz.SetAddToWorkspace(AddToWorkspace);

/*
	(Now, no projects need to update the platforms - BJoyce)
	if(ptProjType!=DATABASE_PROJECT)	//no platforms for Database project
		SetNewProjectPlatforms(intPlatforms);
*/

	uprjwiz.SetLocation(projdir);
	uprjwiz.Create();
	// dismiss connect to datasource dialog
	if(ptProjType==DATABASE_PROJECT)
	{	//get rid of the Data Source window
		HWND hwnd;
		if (hwnd = MST.WFndWndWait(GetLocString(IDS_SELECT_DSN), FW_PART, 1))
		{	// click the mouse to make this window active
			LOG->RecordInfo("Found Window: '%s'", GetLocString(IDS_SELECT_DSN));
			MST.WSetActWnd(hwnd) ;

			RECT rcPage;
			GetClientRect(hwnd, &rcPage);
			ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+150, rcPage.top - 15 );	  // Click on first Tab	(left most)
		}
		if(MST.WButtonExists(GetLabel(IDCANCEL)))
		MST.WButtonClick(GetLabel(IDCANCEL)); //get rid of the window
	}

	// Quick Hack, just "Finish" and "OK", update this for better testing
	// of Custom AppWizards
	//ReplaceExistingProjectIfNecessary();
	uprjwiz.Finish();
	MST.WButtonClick(GetLabel(IDOK));

	if(!(ptProjType==DATABASE_PROJECT && AddToWorkspace!=FALSE))
	{ // this will never be included in ide title for data project added to the workspace
		if(WaitForProjectNameToBeIncludedInIDETitle(szProjName) != ERROR_SUCCESS)

		{		
		LOG->RecordInfo("ERROR in %s: ide title not updated with project name %s",
						THIS_FUNCTION, szProjName);

		return ERROR_ERROR;
		}

	}
	NewProjectWindow();
//	SetPathMembers();
	return ERROR_SUCCESS;
}


// BEGIN_HELP_COMMENT
// Function: int COProject::InsertProject(PROJTYPE ptProjType, LPCSTR szProjName, int intPlatforms, PROJ_HIERARCHY phProjHier = TOP_LEVEL_PROJ, LPCSTR szTopLevelProj = NULL)
// Description: Inserts a new project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szProjType A PROJTYPE value that indicates the type of project to create.
// Param: szProjName A pointer to a string that contains the name of the project.
// Param: intPlatforms A combination of PLATFORM_TYPE values that indicates the platforms the project will target.
// Param: phProjHier A PROJECT_HIERARCHY value indicating a top-levl or subproject. The default is TOP_LEVEL_PROJ.
// Param: szTopLevelProj A pointer to a string that contains the name of the top-level project under which the new subproject is to be inserted.
// END_HELP_COMMENT
int COProject::InsertProject(PROJTYPE ptProjType, LPCSTR szProjName, int intPlatforms,
							 PROJ_HIERARCHY phProjHier /* TOP_LEVEL_PROJ */, 
							 LPCSTR szTopLevelProj /* NULL */)

	{
	const char* const THIS_FUNCTION = "COProject::InsertProject";

#ifdef OLD_INSERT_DIALOG
	// bring up the "insert project" dlg.
	UIProjectWizard uprjwiz;
	UINewFileDlg nwflDlg ;
	MST.DoKeys(GetLocString(IDSS_INSERT_PROJ_ACCEL));

	
	if(!uprjwiz.WaitAttachActive(3000))
		{		
		LOG->RecordInfo("ERROR in %s: can't activate insert project dlg", THIS_FUNCTION);
		return ERROR_ERROR;
		}
#endif	// OLD_INSERT_DIALOG
	UIProjectWizard uprjwiz = AppWizard(); 
	// set the project info and complete creation (can't set location when inserting proj).
	Sleep(1000);	// BJoyce: we are too fast
	uprjwiz.SetProjType(ptProjType);
	uprjwiz.SetName(szProjName);
	uprjwiz.SetHierarchy(phProjHier);	// top-level or subproject?

	if(phProjHier == SUB_PROJ)
		uprjwiz.SetTopLevelProj(szTopLevelProj);

	SetNewProjectPlatforms(intPlatforms);
	uprjwiz.Create();

	// Quick Hack, just "Finish" and "OK", update this for better testing
	// of Custom AppWizards
	// ReplaceExistingProjectIfNecessary();
	uprjwiz.Finish();
	MST.WButtonClick(GetLabel(IDOK));

	return ERROR_SUCCESS;

}


// shared by NewProject(), InsertProject(), and NewAppWiz().
void COProject::SetNewProjectPlatforms(int intPlatforms)
	
	{  

	UINewFileDlg nwflDlg ;
	// turn off any undesired platforms (all are on by default).
	if(!((intPlatforms & PLATFORM_WIN32_X86) || (intPlatforms & PLATFORM_WIN32S_X86) || (intPlatforms & PLATFORM_WIN32_MIPS) || (intPlatforms & PLATFORM_WIN32_ALPHA)))
		nwflDlg.SetProjPlatform(GetLocString(IDSS_PLATFORM_WIN32));

	if(!(intPlatforms & PLATFORM_MAC_68K))
		nwflDlg.SetProjPlatform(GetLocString(IDSS_PLATFORM_MAC68K));

	if(!(intPlatforms & PLATFORM_MAC_PPC))
		nwflDlg.SetProjPlatform(GetLocString(IDSS_PLATFORM_MACPPC));
	
	}


void COProject::ReplaceExistingProjectIfNecessary(void)
	{
	// wait up to 3 seconds for file replace msg box to come up.
	if(MST.WFndWndWaitC(GetLocString(IDSS_REPLACE_EXISTING_FILE), "Static", FW_PART, 3))
		MST.WButtonClick(GetLocString(IDSS_YES));
	if (MST.WFndWndWait(GetLocString(IDSS_WORKBENCH_TITLEBAR), FW_FULL|FW_ALL|FW_FOCUS, 2))
		MST.WButtonClick(GetLocString(IDSS_YES));
	}


// shared by NewProject() and InsertProject().
int COProject::WaitForProjectNameToBeIncludedInIDETitle(LPCSTR szProjName)
	
	{
	const char* const THIS_FUNCTION = "COProject::WaitForProjectNameToBeIncludedInIDETitle";

	// when project creation is finished and the ide is ready to continue,
	// the title will be appended with the project name.
	// this has changed - project name now comes first in title
	CString cstrExpectedTitle = (CString)szProjName + " - " + (GetLocString(IDSS_WORKBENCH_TITLEBAR));
	CString cstrActualTitle;
	
	// wait up to 10 seconds for the ide title to be updated.
	for(int i = 0; i < 10; i++)
		
		{
		Sleep(1000);
		// get the ide title.
		MST.WGetText(UIWB.HWnd(), cstrActualTitle);
		
		if(cstrActualTitle == cstrExpectedTitle)
			break;
		}

	// if title wasn't found in 10 seconds, log an error.
	if(i == 10)
		
		{		
		LOG->RecordInfo("ERROR in %s: ide title not updated with project name %s",
						THIS_FUNCTION, szProjName);

		return ERROR_ERROR;
		}
	
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::NewAppWiz(LPCSTR projname, LPCSTR projdir, int platform, const APPWIZ_OPTIONS* pOptions, LPCSTR apptype)
// Description: Create a new AppWizard project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: projname A pointer to a string that contains the name of the AppWizard project.
// Param: projdir A pointer to a string that contains the path for the AppWizard project.
// Param: platform A value that indicates the platform to create the AppWizard project for: PLATFORM_WIN32_X86, PLATFORM_WIN32S_X86, PLATFORM_MAC_68K, PLATFORM_MAC_PPC, PLATFORM_WIN32_MIPS, PLATFORM_WIN32_ALPHA, PLATFORM_WIN32_PPC, PLATFORM_CURRENT, PLATFORM_USER, PLATFORM_NON_USER.
// Param: pOptions A structure containing appwizard options.
// Param: apptype A pointer to a string that contains application type.
// END_HELP_COMMENT
int COProject::NewAppWiz(LPCSTR projname, LPCSTR projdir, int platforms, const APPWIZ_OPTIONS *pOptions /*=NULL*/, LPCSTR apptype /* = NULL */)
{
	const char* const THIS_FUNCTION = "COProject::NewAppWiz";

	UIProjectWizard uprjwiz = AppWizard();
	UIAppWizard uaw;

	// set the project directory and name
	if(projdir)
		uprjwiz.SetDir(projdir);
	uprjwiz.SetName(projname);

	if (apptype != NULL)
	{
		const char* const JAVA_APPWIZ = "Java Applet Wizard";

		if (strcmp(apptype,JAVA_APPWIZ) == 0)
		{
			// Special proj type specified - so ignore special steps
			uprjwiz.SetProjType(apptype);
			uaw.Create();           // goes to step 1
			// Do a kluge hack for now 
			// Bug 6292
			uaw.NextPage();         // step 2
			uaw.NextPage();         // step 3
		}
		else
		{
			// Special proj type specified - so ignore special steps
			uprjwiz.SetProjType(apptype);
			uaw.Create();           // goes to step 1
		}
	}
	else
	{
		uprjwiz.SetProjType(UIAW_PT_APPWIZ);

		SetNewProjectPlatforms(platforms);
		uaw.Create();           // goes to step 1

                if( uaw.GetPage() != UIAW_APPTYPE )
                {
                        LOG->RecordInfo("%s: clicking Create button didn't move to step 1. Unable to create a new AppWizard project.", THIS_FUNCTION);
                        MST.DoKeys("{ESC 5}");
                        return ERROR_ERROR;
                }
                uaw.NextPage();         // step 2
                uaw.NextPage();         // move to step 3

                // set options for step 3
                if (pOptions) {
                        int aOLEOptions[] = { UIAW_OLE_NONE, UIAW_OLE_CONTAINER, UIAW_OLE_MINI_SERVER, UIAW_OLE_FULL_SERVER, UIAW_OLE_CONTAINER_SERVER };
                        // the index into the array (pOptions->mOLECompound) cannot be less than 0 or greater than the number of items in the array
                        ASSERT(pOptions->m_OLECompound >= 0 && pOptions->m_OLECompound < sizeof(aOLEOptions)/sizeof(int));
                        // set the OLE option
                        uaw.SetOLEOptions(aOLEOptions[pOptions->m_OLECompound]);
                }
                if (pOptions && (pOptions->m_OLEAuto == 1)) {
					uaw.SetOLEOptions2(APPWZ_IDCD_AUTOMATION);
				}

                uaw.NextPage();         // step 4

                // set options for step 4
                if (pOptions  && (pOptions->m_Help == 1)) {
//					uaw.SetProjOptions(APPWZ_IDCD_POHELP);
					uaw.SetFeatures(APPWZ_IDCD_POHELP);
				}

                uaw.NextPage();         // step 5

                // Set options for step 5;
                if (pOptions)
                {
                        if (pOptions->m_bUseMFCDLL)
                                uaw.SetProjOptions(UIAW_PROJ_MFCDLL);
                        else
                                uaw.SetProjOptions(UIAW_PROJ_MFCSTATIC);
                }
	}

	if( uaw.Finish() != UIAW_CONFIRM )
	{
		LOG->RecordInfo("%s: clicking Finish button didn't move to Confirmation screen. Unable to create a new AppWizard project.", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	if( uaw.ConfirmCreate() == FALSE )
	{
		LOG->RecordInfo("%s: problem generating project. Unable to create a new AppWizard project.", THIS_FUNCTION);
		return ERROR_ERROR;
	}
    ActivateProjWnd(); // Give focus to workspace window.

	// bring up the project window
	NewProjectWindow();

	// set up the member variables that contain information about this project
	SetPathMembers();

	return ERROR_SUCCESS;
}



// BEGIN_HELP_COMMENT
// Function: int COProject::Open(LPCSTR szProjName, int iMethod, BOOL LogWarning /* = TRUE */)
// Description: Open a project and the project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szProjName A pointer to a string containing the filename of the project to open. This is usually a full path to the project file.
// Param: iMethod Unused.
// Param: LogWarning A Boolean value that indicates whether to log any warnings that occur when opening a project (TRUE) or not. (Default value is TRUE.)
// END_HELP_COMMENT
int COProject::Open(LPCSTR szProjName, int iMethod, BOOL LogWarning /* = TRUE */)
{
	const char* const THIS_FUNCTION = "COProject::Open";

	BOOL bDone;
	int i;
	DWORD dwControl;
	DWORD dwProcessId;
	
	// if the given project name is NULL, use the member that specifies the name
	if (szProjName == NULL)
	{
		if (m_ProjName.IsEmpty()) {
			LOG->RecordInfo("%s: project name not given. Unable to open project.", THIS_FUNCTION);
			return ERROR_ERROR;
		}
		else
			szProjName = (const char *) m_FullPath ;
	}

	// open the project file via the Open Workspace dialog
	UIOpenWorkspaceDlg uiOpenWrkDlg;

	// open the Open Workspace dialog
	uiOpenWrkDlg.Display();

	// set the filename
	uiOpenWrkDlg.SetName(szProjName);

	// click OK
	if (!uiOpenWrkDlg.OK(FALSE)) {
		LOG->RecordInfo("%s: cannot dismiss the Open Workspace dialog. Unable to open the project.", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	SetPathMembers();

	// get the process ID of UIWB
	GetWindowThreadProcessId(UIWB.HWnd(), &dwProcessId);

	// after opening a project, any number of dialogs/message boxes can come
	// up before the project is actually opened

	// we will loop looking for any of the message boxes that can potentially
	// come up until the project workspace window comes up, we timeout, or
	// we get too many message boxes/dialogs before seeing the project
	// workspace window
	
	// set up array of controls to wait for
	// 0: Save As dialog; 0x441 is the Save Files As Type static text
	// 1: message box with Yes button
	// 2: message box with OK button
	int aControls[] = { VRES_stc2, VPROJ_IDC_PLATFORMS, MSG_YES, MSG_OK };

	bDone = FALSE;
	// we shouldn't loop more than five times
	for (i = 0; i < 5 && !bDone; i++) {
		// wait for the IDE to be idle
		WaitForInputIdle(g_hTargetProc, 60000);
		// wait for one of the controls in the array to be displayed
		dwControl = WaitForMultipleControls(dwProcessId, sizeof(aControls)/sizeof(int), aControls, 2000);

		switch (dwControl) {
			case WAIT_TIMEOUT: {
				// we timed out; this means we didn't find any of the windows we expected
				bDone = TRUE;
				break;
			}
			case WAIT_CONTROL_0: {
				// Save As dialog
				UIFileSaveAsDlg uisaveas;

				if (uisaveas.AttachActive()) {
					LOG->RecordInfo("%s: Save As dialog detected when opening the project. Clicking default button (Save).", THIS_FUNCTION);

					// Change the value if necessary and save it.
					uisaveas.SetName(uisaveas.GetName() + "new");

					// dismiss it with the default button
					MST.DoKeys("{ENTER}");
					Sleep(2000);					
				}
				else
					LOG->RecordInfo("%s: Couldn't attach to Save As dialog", THIS_FUNCTION);

				break;
			}

			case WAIT_CONTROL_1: {
				// Platform selection box from opening an external project
				LOG->RecordInfo("%s: Platforms dialog detected", THIS_FUNCTION);

				/* REVIEW: VerifyClass() && VerifyUnique() are both failing 
				   in the call to dlg.AttachActive(), so to work-around we are
				   just hitting ENTER for now.

				UIDialog dlg;

				// attach to the active message box
				if (dlg.AttachActive()) {
					LOG->RecordInfo("%s: Clicked OK", THIS_FUNCTION);
					// dismiss it with the default button
					dlg.OK();
					Sleep(2000);					
				}
				else
					LOG->RecordInfo("%s: Couldn't attach to Platform message box", THIS_FUNCTION);
				*/
				
				LOG->RecordInfo("%s: hit ENTER", THIS_FUNCTION);
				MST.DoKeys("{ENTER}");
				Sleep(2000);
				break;
			}

			case WAIT_CONTROL_2: {
				// message box with a Yes button
				UIWBMessageBox uimsg;
				LOG->RecordInfo("%s: Yes/No message box detected", THIS_FUNCTION);
				
				// attach to the active message box
				uimsg.Attach(MST.WGetActWnd(0)) ;
				// REVIEW(Ivanl) AttachActive was failing, we are bypassing it for now.
				//if (uimsg.AttachActive()) 
					{
					LOG->RecordInfo("%s: Yes/No message box contains '%s'", THIS_FUNCTION, (LPCSTR)uimsg.GetMessageText());

					// if the message box text contains the string 'replace,'
					// then we need to handle this as the case when the
					// wrapper makefile is created but one already exists on
					// the disk
					if ((uimsg.GetMessageText()).Find(GetLocString(IDSS_REPLACE)) != -1) {
						LOG->RecordInfo("%s: Clicked Yes", THIS_FUNCTION);
						MST.WButtonClick(GetLocString(IDSS_YES));
						Sleep(2000);
					}
					// If the message box text contains a message pertaining to the fact
					// that a file is marked read-only, so it has to be saved to a different 
					// filename, then say no.
					else if ((uimsg.GetMessageText()).Find(GetLocString(IDSS_READ_ONLY)) != -1) {
						LOG->RecordInfo("%s: Clicked No", THIS_FUNCTION);
						MST.WButtonClick(GetLocString(IDSS_NO));
						Sleep(2000);					
					}
					else {
						LOG->RecordInfo("%s: Clicked Default", THIS_FUNCTION);
						// dismiss the dialog with the default button
						MST.DoKeys("{ENTER}") ;
						Sleep(2000);					
					}
				}
				break;
			}
			case WAIT_CONTROL_3: {
				// TODO(michma - 5/22/98): for some reason on nt5 we have a timing problem where
				// UIWBMessageBox::AttachActive (below) fails if we don't wait for a second. it's almost
				// as if nt is notifying us about WAIT_CONTROL_3 before the message box is up, or maybe
				// control id's on nt have changed and we are finding a different WAIT_CONTROL_3.
				Sleep(1000);
				// message box with an OK button
				LOG->RecordInfo("%s: OK message box detected", THIS_FUNCTION);

				UIWBMessageBox uimsg;

				// attach to the active message box
				if (uimsg.AttachActive()) {
					LOG->RecordInfo("%s: OK message box contains '%s'", THIS_FUNCTION, (LPCSTR)uimsg.GetMessageText());
					LOG->RecordInfo("%s: Clicked default", THIS_FUNCTION);
					// dismiss it with the default button
					MST.DoKeys("{ENTER}");
					Sleep(2000);					
				}
				else
					LOG->RecordInfo("%s: Couldn't attach to message box", THIS_FUNCTION);
				break;
			}
			default: {
				LOG->RecordInfo("%s: WaitForMultipleControls() returned an unknown value. Unable to open the project.", THIS_FUNCTION);
				return ERROR_ERROR;
				break;
			}
		}
	}

	// bring up the project window
	NewProjectWindow();

	// is it valid?

	SetPathMembers();

	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::Save(int wait == FALSE, LPCSTR file = NULL)
// Description: Save the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: wait A Boolean value that indicates whether to wait for the file to be completely written before returning (TRUE) or not. (Default value is FALSE.)
// Param: file A pointer to a string that contains the name of the project file. This parameter is only required if wait is set to TRUE. (Default value is NULL.)
// END_HELP_COMMENT
int COProject::Save(int wait /* FALSE */, LPCSTR file /* NULL */)
{
	const char* const THIS_FUNCTION = "COProject::Save";

	VERIFY_VALID_PROJECT("Unable to save the project.")

	CTime timeStart;
	CTimeSpan timeSpan;

	CFileStatus statusFile;

	ActivateProjWnd();
	
	// get the current time
	timeStart = CTime::GetCurrentTime();

	// save the project
	UIWB.DoCommand(IDM_SAVE_WORKSPACE, DC_MNEMONIC);
	WaitForInputIdle(g_hTargetProc, 2000); //safety

	// are we supposed to wait for the file to be saved?
	if (wait) {
		// the filename must be supplied
		ASSERT(file && file[0] != '\0');
		if (!file || file[0] == '\0') {
			LOG->RecordInfo("COProject::Save() filename not specified.");
			return ERROR_ERROR;
		}

		// Wait 5 minutes for the save to finish.
        if (WaitForInputIdle(g_hTargetProc, 5 * 60000) == WAIT_TIMEOUT)	{
				LOG->RecordInfo("COProject::Save() timed out because saving took longer than 5 minutes");
				return ERROR_ERROR;
		}
	}

	return ERROR_SUCCESS;

}

// BEGIN_HELP_COMMENT
// Function: int COProject::AddFiles(LPCSTR szFileName, LPCSTR szGrpName)
// Description: Add the files given by the filespec in szFileName to the specified group in the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFileName A pointer to a string that contains the filespec describing the files to add. Wildcards (* and ?) are permitted.
// Param: szGrpName A pointer to a string that contains the name of the group to add the files to.
// END_HELP_COMMENT
int COProject::AddFiles(LPCSTR szFileName, LPCSTR szGrpName)
{
	const char* const THIS_FUNCTION = "COProject::AddFiles";

	VERIFY_VALID_PROJECT("Unable to add files to the project.")

	CString Files = szFileName ; //make me a CString object from szFileName for ease of manipulation
	int iNextName = 0 ;

	HWND hOrig = WGetActWnd(0) ;
	UIWB.DoCommand(IDM_PROJITEM_ADD_FILES, DC_MNEMONIC);	// DevStudio96:11551, don't use MNENOMIC
	
//      control IDs, obtained from Spy++
#define ID_FSAD_FILENAME                0x480

	if (!UIWB.WaitOnWndWithCtrl(ID_FSAD_FILENAME,3000))
	   return ERROR_ERROR ;
	
	LOG->RecordInfo("Add File(s) %s", szFileName);

	MST.WEditSetText(GetLabel(ID_FSAD_FILENAME), szFileName) ;
	Sleep(250);	// Win2000 needs this sleep (BJoyce)
	
	// If the name has a wildcard,  filter the listbox and multi-select
	if((Files.Find('*')>=0) || (Files.Find('?')>=0) )
	{
		LOG->RecordInfo("Multiple File Select");
		MST.DoKeys("{ENTER}") ; //To fill up the Files listbox
		Sleep(500);
		//set focus to Files listbox
		MST.DoKeys("+{TAB}");
		Sleep(250);
		MST.DoKeys(" "); //turn on selection
		Sleep(250);
		MST.DoKeys("+{END}"); //select all files
		Sleep(500);
		// what files are we going to add according to the selection
		CString strTemp;
		MST.WGetText(::GetDlgItem(WGetActWnd(0), ID_FSAD_FILENAME), strTemp);
		LOG->RecordInfo("Which files are we adding - %s", strTemp);

		MST.WButtonClick(GetLabel(IDOK)) ; // Add and Close dialog.
	}
	else  //
	{
		LOG->RecordInfo("Only one file");
		HWND hWnd = WGetActWnd(0) ;       // Dialog handle
		
		MST.WButtonClick(GetLabel(IDOK)) ;  // Add and Close dialog.
		// Did an error occur?
		if ((WGetActWnd(0) != hWnd) && (WGetActWnd(0) != hOrig))  // We got an error message
		{ // Press enter and cancel operation.
			if (WGetActWnd(0) != hWnd)
				MST.DoKeys("{ENTER}") ;
			MST.DoKeys("{ESC}") ;
			return ERROR_ERROR ;
		}
	}

	HWND hWait = (HWND) 1 ;
	int i = 0 ;

	while( i )        // Scan for a minute.
	{
		if(UIWB.WaitOnWndWithCtrl(8,3000)) //Wait for an error message to show up, if it shows up exit
		{       
            MST.DoKeys("{ESC}") ;
			return ERROR_ERROR ;
		}
		else
			if (WGetActWnd(0) == hOrig) // if we are back to the editor
			{
				return ERROR_SUCCESS ;
			}
		i-- ;
	}
	return ERROR_SUCCESS;
}


// BEGIN_HELP_COMMENT
// Function: int COProject::RemoveFiles(LPCSTR szFileNames, LPCSTR szGrpName )
// Description: Remove the files named in szFileNames from the specified group in the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFileNames A pointer to a string that contains the list of files (separated by semicolons (;)) to remove from the group.
// Param: szGrpName A pointer to a string that contains the name of the group.
// END_HELP_COMMENT
int COProject::RemoveFiles(LPCSTR szFileNames, LPCSTR szGrpName )
{
	const char* const THIS_FUNCTION = "COProject::RemoveFiles";

	VERIFY_VALID_PROJECT("Unable to remove files from the project.")

    CString szFile, szSpaces ;
	CString Files = szFileNames ; //make me a CString object from szFileName for ease of manipulation
	int iNextName = 0 ;

// REVIEW(briancr): you don't remove files via the Files dialog. This function needs to be changed.
	ASSERT(0);
	return ERROR_ERROR;
#if 0
	// Bring up the Files dialog
	UIWB.DoCommand(IDM_PROJITEM_ADD_FILES, DC_MNEMONIC);
	if (!UIWB.WaitOnWndWithCtrl(VPROJ_IDC_EDITPROJ_GROUP,3000))
	   return ERROR_ERROR ;

	// Select the desired group to remove file from
	MST.WListItemClk(GetLabel(VPROJ_IDC_EDITPROJ_GROUP),(LPSTR)szGrpName) ;

	for (; !Files.IsEmpty(); )
	{
	// Get a name from the Files list
		if((iNextName = Files.Find(';')) != -1) // If there are more than one filename
		{
			szFile = Files.Left(iNextName) ;
		}
		else
		{
			szFile = Files ;
			Files = "" ; //Prepare to end the loop.
			iNextName = -1 ;
		}

		// Remove any leading and trailing spaces from the name.
		szSpaces =szFile.SpanExcluding("' '") ;
		if(szSpaces.IsEmpty())  // If there were leading spaces
		{
			szSpaces =szFile.SpanIncluding("' '") ;
			szFile = (szFile.Right(szFile.GetLength() - szSpaces.GetLength())).SpanExcluding("' '") ;
		}
		else // If there were no leading spaces assign szFile to the extracted string without spaces.
		{
			szFile = szSpaces ;
		}

		// Remove the file from the listbox of project filenames.
		if (!MST.WListItemExists(GetLabel(VPROJ_IDC_EDITPROJ_GROUP),szFile.GetBuffer(szFile.GetLength())))
			{
				MST.DoKeys("{ESC}") ;
				return ERROR_ERROR ;
			}
		else
			{
				MST.WListItemDblClk(GetLabel( VPROJ_IDC_EDITPROJ_GROUP),szFile.GetBuffer(szFile.GetLength())) ;
				Files = Files.GetBuffer(Files.GetLength()) + iNextName + 1 ;
				Files.ReleaseBuffer() ;
			}
	}
	MST.WButtonClick(GetLabel(VPROJ_IDCLOSE)) ;

	return ERROR_SUCCESS ;

#endif
}

// BEGIN_HELP_COMMENT
// Function: int COProject::RemoveAll(LPCSTR szGrpName)
// Description: Remove all files from the specified group in the project. This function is NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szGrpName A pointer to a string that contains the name of the group.
// END_HELP_COMMENT
int COProject::RemoveAll(LPCSTR szGrpName)
{
	const char* const THIS_FUNCTION = "COProject::RemoveAll";

	VERIFY_VALID_PROJECT("Unable to remove all files from the project.")

 return ERROR_ERROR ; //NYI

}

// Directory / File delete utilities (REVIEW)IVANL Does Delnode exist on MIPS?? HOPE SO!!
CString CommandLine = "CMD /C \"echo y | rd /s \"", arg ;
// BEGIN_HELP_COMMENT
// Function: int COProject::DeleteMakeFile()
// Description: Delete the makefile associated with the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COProject::DeleteMakeFile()
{
	HWND hOrig = WGetActWnd(0) ;
	KillFile (m_FullPath);
	return WaitForOriginalWnd(hOrig, 20) ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::DeleteFromProjectDir(LPCSTR strDelTarget)
// Description: Delete all files in the project directory.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: strDelTarget A pointer to a string that contains a relative path to append to the project directory, or a full path if there is no project open. This parameter can be NULL.
// END_HELP_COMMENT
int COProject::DeleteFromProjectDir(LPCSTR strDelTarget)
{
	HWND hOrig = WGetActWnd(0) ;
	CString wild = "*.*" ;
	if (m_ProjDir.IsEmpty())
		if (strDelTarget == NULL)
		{
			LOG->RecordInfo("You have to provide a Delete Target if no project is open. ::DeleteFromProjectDir()") ;
			return ERROR_ERROR ;
		}
		else
		m_ProjDir = strDelTarget ;
	else
		m_ProjDir +=strDelTarget ;
	return KillAllFiles(m_ProjDir) ;
	
}

// BEGIN_HELP_COMMENT
// Function: int COProject::RemoveIntermediateDir()
// Description: Delete the directory that contains the intermediate files generated by building the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COProject::RemoveIntermediateDir()
{
	HWND hOrig = WGetActWnd(0) ;
	return KillAllFiles(GetIntermediateDir());
	return WaitForOriginalWnd(hOrig, 20) ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::RemoveTargetDir()
// Description: Delete the directory that contains the target files generated by building the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COProject::RemoveTargetDir()
{
	HWND hOrig = WGetActWnd(0) ;
	// need to go grab the m_OutputFile since it might have been changed by someone
	// during testing. It can happen.
	GetOutputFile();

	char drive[_MAX_DRIVE];   char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   char ext[_MAX_EXT];

	_splitpath( m_OutputFile, drive, dir, fname, ext );
	CString strTemp = drive;
	strTemp += dir;

	return KillAllFiles(strTemp);
	
}

// Configurations / Targets
// BEGIN_HELP_COMMENT
// Function: int COProject::AddTarget(LPCSTR Name, int Type , BOOL Debug/* =FALSE*/)
// Description: Add a new target to the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Name A pointer to a string that contains the name of the new target.
// Param: Type A value that indicates the type of target to add: TARGET_WIN32_X86_DEBUG, TARGET_WIN32_X86_RELEASE, TARGET_MAC_68K_DEBUG, TARGET_MAC_68K_RELEASE, TARGET_MAC_PPC_DEBUG, TARGET_MAC_PPC_RELEASE, TARGET_USER, TARGET_CURRENT.
// Param: Debug A Boolean value that indicates whether to make this target debug (TRUE) or not. (Default value is FALSE.)
// END_HELP_COMMENT
int COProject::AddTarget(LPCSTR Name, int Type , BOOL Debug/* =FALSE*/)
{
	const char* const THIS_FUNCTION = "COProject::AddTarget";

	VERIFY_VALID_PROJECT("Unable to add a target to the project.")

	UIProjectTarget tr ;
	int ret ;
	tr.Show() ;
	ret = tr.New(Name,Type,Debug) ;
	tr.Close() ;
	if (ret)
		return ERROR_SUCCESS ;
	else
		return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::AddTarget(LPCSTR Name, LPCSTR CopyFrom)
// Description: Add a new target to the project by deriving from an existing target.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Name A pointer to a string that contains the name of the new target.
// Param: CopyFrom A pointer to a string that contains the name of the target to derive this target from.
// END_HELP_COMMENT
int COProject::AddTarget(LPCSTR Name, LPCSTR CopyFrom)
{
	const char* const THIS_FUNCTION = "COProject::AddTarget";

	VERIFY_VALID_PROJECT("Unable to add a target to the project.")

	UIProjectTarget tr ;
	int ret ;
	tr.Show() ;
	ret = tr.New(Name,CopyFrom) ;
	tr.Close() ;
	if (ret)
		return ERROR_SUCCESS ;
	else
		return ERROR_ERROR ;

}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetTarget(LPCSTR Target)
// Description: Select one of the targets for this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Target A pointer to a string that contains the name of the target to select.
// END_HELP_COMMENT
int COProject::SetTarget(LPCSTR target)
{
	const char* const THIS_FUNCTION = "COProject::SetTarget";

	VERIFY_VALID_PROJECT("Unable to select a target.")

	UIProjectTarget tr;
	int ret;
	tr.Show();
	ret = tr.SetTarget(target);
	tr.Close();
	return ret ? ERROR_SUCCESS : ERROR_ERROR;
	}


int COProject::SetTarget(TARGET_TYPE target)

// todo: this function allows the user to set the current project target using
// portable symbols instead of strings, which will ease maintainence. however,
// this function still needs to be implemented.  see definition of TARGET_TYPE
// in coprject.h.  note that a target of TARGET_USER should set the current
// target to that specified by the user.  the user-specified platform can be
// retrieved with GetUserTargetPlatforms().  currently there is no standard way
// for a test to find out if it is targetting a retail target or a debug target.

	{
	return ERROR_SUCCESS;
	}


TARGET_TYPE COProject::GetCurrentTarget(void)

// todo: this function allows the user to get the current project target using
// portable symbols instead of strings, which will ease maintainence. however,
// this function still needs to be implemented.  see definition of TARGET_TYPE
// in coprject.h.

	{
	return TARGET_WIN32_X86_DEBUG;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::DeleteTarget(LPCSTR Target)
// Description: Delete one of the targets for this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: Target A pointer to a string that contains the name of the target to delete.
// END_HELP_COMMENT
int COProject::DeleteTarget(LPCSTR Target)
{
	const char* const THIS_FUNCTION = "COProject::DeleteTarget";

	VERIFY_VALID_PROJECT("Unable to delete a target from the project.")

	UIProjectTarget tr ;
	int ret ;
	tr.Show() ;
	ret = tr.Delete(Target) ;
	tr.Close() ;
	if (ret)
		return ERROR_SUCCESS ;
	else
		return ERROR_ERROR ;
}
// BEGIN_HELP_COMMENT
// Function: int COProject::RenameTarget(LPCSTR OldName, LPCSTR NewName)
// Description: Change the name of one of the targets in this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: OldName A pointer to a string that contains the name of the target to change.
// Param: NewName A pointer to a string that contains the new name for the target.
// END_HELP_COMMENT
int COProject::RenameTarget(LPCSTR OldName, LPCSTR NewName)
{
	const char* const THIS_FUNCTION = "COProject::RenameTarget";

	VERIFY_VALID_PROJECT("Unable to rename a target.")

	UIProjectTarget tr ;
	int ret ;
	tr.Show() ;
	ret = tr.Rename(OldName,NewName) ;
	tr.Close() ;
	if (ret)
		return ERROR_SUCCESS ;
	else
		return ERROR_ERROR ;

}
// BEGIN_HELP_COMMENT
// Function: LPCSTR COProject::GetTarget(int Target /* = 0 Active target */)
// Description: Get the name of the either the active target or the specified target.
// Return: A pointer to a string that contains the name of the target specified.
// Param: Target An integer that contains a 1-based index into the list of targets. 0 will retrieve the name of the current target. (Default value is 0.)
// END_HELP_COMMENT
LPCSTR COProject::GetTarget(int Target /* = 0 Active target */)
{
	const char* const THIS_FUNCTION = "COProject::GetTarget";

	ASSERT(IsOpen());
	if (!IsOpen()) {
		LOG->RecordInfo("%s: the project is invalid (not opened). Unable to get a target.", THIS_FUNCTION);
		return NULL;						\
	}

	UIProjectTarget tr ;
	const char * ret ;
	tr.Show() ;
	ret = tr.GetTarget(Target) ;
	tr.Close() ;
	return ret ;
}

// BEGIN_HELP_COMMENT
// Function: COProject::Execute(int iRebuild)
// Description: Execute the current project. This function does not execute the project under the debugger.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iRebuild An Boolean value that indicates whether to rebuild the project before executing, if asked (TRUE) or not.
// END_HELP_COMMENT
COProject::Execute(int iRebuild)
{
	const char* const THIS_FUNCTION = "COProject::Execute";

	VERIFY_VALID_PROJECT("Unable to execute the project.")

	HWND hWnd = WGetActWnd(0) ;
	UIWB.DoCommand(IDM_PROJECT_EXECUTE, DC_ACCEL);

	// Did we get a rebuild messagebox?
	Sleep(2) ; // Wait to see if we get a messagebox to rebuild.

	if(WGetActWnd(0) != hWnd)
	{
		if (iRebuild)
		{
			MST.DoKeys("{ENTER}"); // Take default
			WaitUntilBuildDone(3) ; //Wait on build for 5 minutes. (REVIEW) return here if exceeds max time.
			UIWB.DoCommand(IDM_PROJECT_EXECUTE, DC_ACCEL);
		}
		else
		{       MST.DoKeys("{ESC}");
		}
	}
  return ERROR_SUCCESS ;
}

	// Target
// BEGIN_HELP_COMMENT
// Function: int COProject::Build(int iWait, DWORD interval /*1000*/)
// Description: Build the current target in this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iWait An integer that contains the amount of time in minutes to wait for the build to complete.
// Param: interval An DWORD that contains the amount of time in milliseconds to wait between checks for build completion
// END_HELP_COMMENT
int COProject::Build(int iWait, DWORD interval /*1000*/)
{       // iWait how many MINUTES to wait.
	const char* const THIS_FUNCTION = "COProject::Build";

	VERIFY_VALID_PROJECT("Unable to build the project.")

	
	UIWB.DoCommand(IDM_PROJITEM_BUILD, DC_MNEMONIC);
	return WaitUntilBuildDone(iWait, interval) ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::RebuildAll(int iWait /* 3 */,DWORD interval /*1000*/)
// Description: Perform a complete rebuild of the current target in this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iWait An integer that contains the amount of time in minutes to wait for the build to complete.
// Param: interval An DWORD that contains the amount of time in milliseconds to wait between checks for build completion
// END_HELP_COMMENT
int COProject::RebuildAll(int iWait /* 3 */, DWORD interval /*1000*/)
{  // iWait how many MINUTES to wait.
	const char* const THIS_FUNCTION = "COProject::RebuildAll";

	VERIFY_VALID_PROJECT("Unable to rebuild the project.")

	UIWB.DoCommand(IDM_PROJITEM_REBUILD_ALL, DC_MNEMONIC) ;
	return WaitUntilBuildDone(iWait, interval) ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::StopBuild()
// Description: Stop the current build.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COProject::StopBuild()
{
	const char* const THIS_FUNCTION = "COProject::StopBuild";

	VERIFY_VALID_PROJECT("Unable to stop building the project.")

	UIWB.DoCommand(IDM_PROJECT_STOP_BUILD, DC_MNEMONIC) ;
	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: CString COProject::GetProjectItemProp(int P_PropID)
// Description: Get the name of the item indicated by P_PropID.
// Return: A CString that contains the name of the item.
// Param: P_PropID An integer that contains the ID of the item.
// END_HELP_COMMENT
CString COProject::GetProjectItemProp(int P_PropID)
{
	const char* const THIS_FUNCTION = "COProject::GetProjectItemProp";

	VERIFY_VALID_PROJECT("Unable to get the property.")

	return UIWB.GetItemProperty(P_PropID) ;
}
/*
int COProject::SelectFile(LPCSTR szFile, LPCSTR szGrp)
{
	UIWB.SelectProjItem(ITM_GROUP,szGrp) ;
	UIWB.SelectProjItem(ITM_FILE,szFile) ;
	return ERROR_ERROR ;
}

int COProject::SelectGroup(LPCSTR szGrpName)
{
	UIWB.WaitOnWndWithCtrl(IDOK,3000 ) ;
	UIWB.SelectProjItem(ITM_GROUP,szGrpName) ;
	return ERROR_ERROR ; // Didn't find the group.
}
 */


// BEGIN_HELP_COMMENT
// Function: int COProject::SelectFile(LPCSTR szFile, LPCSTR szGrp)
// Description: Select a file in the project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFile A pointer to a string that contains the name of the file to select.
// Param: szGrp A pointer to a string that contains the name of the group that contains the file. The value can be an empty string, if the group is not known.
// END_HELP_COMMENT
int COProject::SelectFile(LPCSTR szFile, LPCSTR szGrp)
{
	const char* const THIS_FUNCTION = "COProject::SelectFile";

	VERIFY_VALID_PROJECT("Unable to select a file in the project.")

//      UIWB.SelectProjItem(ITM_GROUP,szGrp) ;
//      UIWB.SelectProjItem(ITM_FILE,szFile) ;

	CString stGrp = szGrp ;
	CString stFile = szFile ;
	stFile.MakeUpper() ;

	// make project window active.
	ActivateProjWnd();

	// If a group was not provided.
	if(stGrp == "")
		stGrp = UIWB.GetItemProperty(P_GroupName) ;
	if(!stGrp.IsEmpty())
	{       // Select the provided group
		if (SelectGroup(stGrp)== ERROR_ERROR)
		return ERROR_ERROR ;
	}
	else
		return ERROR_ERROR ;
	// Now that we have selected the group, find the file.
	CString szPrev = "" ;
	CString szCur = "" ;
	MST.DoKeys("{ENTER}") ;

	for (int i = 1; i ; )
		{
			MST.DoKeys("{DOWN}") ;
			szCur = UIWB.GetItemProperty(P_ProjItemName) ;
			szCur.MakeUpper() ;
			if(szCur == stFile) // We found the desired file
			{
				return ERROR_SUCCESS ;
			}
			else
			{
				CString szGroup ;
				szGroup = UIWB.GetItemProperty(P_GroupName) ;
				szGroup.MakeUpper();
				stGrp.MakeUpper() ;

				if(szGroup != stGrp)  // Did we run into another group?
					return ERROR_ERROR ;

				szPrev.MakeUpper() ; szCur.MakeUpper() ;
				if(szPrev == szCur) // We reached the end of the list
				{
					i = 0 ; // We didn't find it.
				}
				szPrev = szCur ;
			}
		}
	return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::SelectGroup(LPCSTR szGrpName)
// Description: Select a group in the project window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szGrpName A pointer to a string that contains the name of the group to select in the project window.
// END_HELP_COMMENT
int COProject::SelectGroup(LPCSTR szGrpName)
{
	const char* const THIS_FUNCTION = "COProject::SelectGroup";

	VERIFY_VALID_PROJECT("Unable to select a group in the project.")

	// Activate project window
	ActivateProjWnd();
	//Select the Project folder
	MST.DoKeys("{BS 4}") ; // Only need 3
	// Expand all
	MST.DoKeys("{*}") ;
	// Show groups only
	MST.DoKeys("{ENTER 2}") ;
	// Loop through the groups to find the desired group.
	CString szPrev = "", szCur ;
	CString stGrpName(szGrpName) ;
	stGrpName.MakeUpper() ;
	for (int i = 1; i ; )
		{
			MST.DoKeys("{DOWN}") ;
			szCur = UIWB.GetItemProperty(P_GroupName) ;
			szCur.MakeUpper() ;
			if(szCur == stGrpName) // We found the desired group
				return ERROR_SUCCESS ;
			if(szPrev == szCur) // We reached the end of the list
				return ERROR_ERROR ; // We didn't find it.
			szPrev = szCur ;
		}
	return ERROR_ERROR ; // Didn't find the group.
}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId, UINT iNewValue )
// Description: Set a value-based property of the project. (See SetProjProperty(PROJ_PROP_CATEGORIES, UINT, LPCSTR) to set text-based properties.) This function will set project properties such as general, debugger, compiler, linker, resource, and browser.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: iCtrlId An integer that contains the ID of the control that modifies the property.
// Param: iNewValue An integer that contains the new value of the property.
// END_HELP_COMMENT
int COProject::SetProjProperty(PROJ_PROP_CATEGORIES iCategory, UINT iCtrlId, UINT iNewValue)
{
	const char* const THIS_FUNCTION = "COProject::SetProjProperty";

	VERIFY_VALID_PROJECT("Unable to set a project property.")

	int iResult;
	// Display the Project Options dialog.

	UIProjOptionsTabDlg uipo;        // REVIEW(EnriqueP) Attach and use OK()
	uipo.AttachActive();
	if (!uipo.IsValid())
	{
		uipo.Display();
		uipo.AttachActive();
	}
	
	iResult = uipo.SetProjOption(iCategory, iCtrlId, iNewValue);

	MST.WButtonClick(GetLabel(IDOK)) ;
	return iResult;
}
	

// BEGIN_HELP_COMMENT
// Function: int COProject::SetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId, LPCSTR szNewValue )
// Description: Set a text-based property of the project. (See SetProjProperty(PROJ_PROP_CATEGORIES, UINT, UINT) to set value-based properties.) This function will set project properties such as general, debugger, compiler, linker, resource, and browser.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: iCtrlId An integer that contains the ID of the control that modifies the property.
// Param: szNewValue An pointer to a string that contains the new value of the property.
// END_HELP_COMMENT
int COProject::SetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId, LPCSTR szNewValue)
{
	const char* const THIS_FUNCTION = "COProject::SetPropProperty";

	VERIFY_VALID_PROJECT("Unable to set a project property.")

	int iResult;

	UIProjOptionsTabDlg uipo;        // REVIEW(EnriqueP) Attach and use OK()
	uipo.AttachActive();
	if (!uipo.IsValid())
	{
		uipo.Display();
		uipo.AttachActive();
	}

	iResult = uipo.SetProjOption(iCategory, iCtrlId, szNewValue);

	MST.WButtonClick(GetLabel(IDOK)) ;
	return iResult;
}


//Project
// BEGIN_HELP_COMMENT
// Function: int COProject::GetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId,CString szValue)
// Description: Get the value of a property of this project. (See GetProjProperty(PROJ_PROP_CATEGORIES, UINT) to get text-based properties.) This function will get project properties such as general, debugger, compiler, linker, resource, and browser. This function in NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// Param: szNewValue A CString that adds no value that I can see.
// END_HELP_COMMENT
int COProject::GetProjProperty(PROJ_PROP_CATEGORIES iCategory, UINT iCtrlId, CString szValue)
{
	const char* const THIS_FUNCTION = "COProject::GetProjProperty";

	VERIFY_VALID_PROJECT("Unable to get a project property.")

	return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: CString COProject::GetProjProperty(PROJ_PROP_CATEGORIES iCategory,UINT iCtrlId)
// Description: Get the text value of a property of this project. (See GetProjProperty(PROJ_PROP_CATEGORIES, UINT, CString) to get value-based properties.) This function will get project properties such as general, debugger, compiler, linker, resource, and browser. This function in NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// END_HELP_COMMENT
CString COProject::GetProjProperty(PROJ_PROP_CATEGORIES iCategory, UINT iCtrlId)
{
	const char* const THIS_FUNCTION = "COProject::GetProjProperty";

	VERIFY_VALID_PROJECT("Unable to get a project property.")

	CString strResult = "";

	UIProjOptionsTabDlg uipo;        // REVIEW(EnriqueP) Attach and use OK()
	uipo.AttachActive();
	if (!uipo.IsValid())
	{
		uipo.Display();
		uipo.AttachActive();
	}

	strResult = uipo.GetProjOptionStr(iCategory, iCtrlId);

	MST.WButtonClick(GetLabel(IDOK)) ;

	return strResult;
}


//Group
// BEGIN_HELP_COMMENT
// Function: int COProject::GetGroupProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName, UINT iCtrlId)
// Description: Get the value of a property for the given group. (See GetGroupProperty(PROJ_PROP_CATEGORIES, LPCSTR, UINT, CString) to get text-based properties.) This function in NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group to get properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// END_HELP_COMMENT
int  COProject::GetGroupProperty(PROJ_PROP_CATEGORIES iCategory, LPCSTR szGrpName, UINT iCtrlId)
{
	const char* const THIS_FUNCTION = "COProject::GetGroupProperty";

	VERIFY_VALID_PROJECT("Unable to get a group property.")

	return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: CString COProject::GetGroupProperty(PROJ_PROP_CATEGORIES  iCategory,LPCSTR szGrpName ,UINT iCtrlId,CString szValue)
// Description: Get the text value of a property for the given group. (See GetGroupProperty(PROJ_PROP_CATEGORIES, LPCSTR, UINT) to get value-based properties.) This function in NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group to get properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// Param: szValue A CString that contains the value of the property for the group.
// END_HELP_COMMENT
CString COProject::GetGroupProperty(PROJ_PROP_CATEGORIES  iCategory,LPCSTR szGrpName ,UINT iCtrlId,CString szValue)
{
	const char* const THIS_FUNCTION = "COProject::GetGroupProperty";

	VERIFY_VALID_PROJECT("Unable to get a group property.")

	return "" ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetGroupProperty(PROJ_PROP_CATEGORIES  iCategory,LPCSTR szGrpName ,UINT iCtrlId, UINT iNewValue )
// Description: Set the value of a property for the given group. (See SetGroupProperty(PROJ_PROP_CATEGORIES, LPCSTR, UINT, LPCSTR) to set text-based properties.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group to get properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// Param: iNewValue An integer that contains the new value for the property.
// END_HELP_COMMENT
int COProject::SetGroupProperty(PROJ_PROP_CATEGORIES  iCategory,LPCSTR szGrpName ,UINT iCtrlId, UINT iNewValue )
{
	const char* const THIS_FUNCTION = "COProject::SetGroupProperty";

	VERIFY_VALID_PROJECT("Unable to set a group property.")

	UIProjOptionsTabDlg uipo;        // REVIEW(EnriqueP) Attach and use OK()
	uipo.AttachActive();
	if (!uipo.IsValid())
	{
		uipo.Display();
		uipo.AttachActive();
	}
	uipo.SelectGroup(szGrpName) ; // REVIEW(Ivanl) Need to pass in a target name.
	return SetOption(iCategory,iCtrlId, iNewValue) ;

}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetGroupProperty(PROJ_PROP_CATEGORIES  iCategory,LPCSTR szGrpName ,UINT iCtrlId, LPCSTR szNewValue )
// Description: Set the text value of a property for the given group. (See SetGroupProperty(PROJ_PROP_CATEGORIES, LPCSTR, UINT, UINT) to set value-based properties.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group to get properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// Param: szNewValue A pointer to a string that contains the text value for the property.
// END_HELP_COMMENT
int COProject::SetGroupProperty(PROJ_PROP_CATEGORIES  iCategory,LPCSTR szGrpName ,UINT iCtrlId, LPCSTR szNewValue )
{       // Display the Project Options dialog.
	const char* const THIS_FUNCTION = "COProject::SetGroupProperty";

	VERIFY_VALID_PROJECT("Unable to set a group property.")

	UIProjOptionsTabDlg uipo;        // REVIEW(EnriqueP) Attach and use OK()
	uipo.AttachActive();
	if (!uipo.IsValid())
	{
		uipo.Display();
		uipo.AttachActive();
	}
	uipo.SelectGroup(szGrpName) ; // REVIEW(Ivanl) Need to pass in a target name.
	return SetOption(iCategory,iCtrlId, szNewValue) ;
}


//File .
// BEGIN_HELP_COMMENT
// Function: int COProject::GetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,LPCSTR szFileName,UINT iCtrlId)
// Description: Get the value of a property for the given file. (See GetFileProperty(PROJ_PROP_CATEGORIES, LPCSTR, LPCSTR, UINT, CString) to get text-based properties.) This function in NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group that contains the file.
// Param: szFileName A pointer to a string that contains the file to get properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// END_HELP_COMMENT
int COProject::GetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,LPCSTR szFileName,UINT iCtrlId)
{
	const char* const THIS_FUNCTION = "COProject::GetFileProperty";

	VERIFY_VALID_PROJECT("Unable to get a file property.")

	return ERROR_ERROR ;
}

// BEGIN_HELP_COMMENT
// Function: CString COProject::GetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,LPCSTR szFileName,UINT iCtrlId,CString szValue)
// Description: Get the text value of a property for the given file. (See GetFileProperty(PROJ_PROP_CATEGORIES, LPCSTR, LPCSTR, UINT) to get value-based properties.) This function in NYI.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group that contains the file.
// Param: szFileName A pointer to a string that contains the file to get properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// Param: szValue A CString that contains the text value of the property for the given file.
// END_HELP_COMMENT
CString COProject::GetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName ,LPCSTR szFileName,UINT iCtrlId,CString szValue)
{
	const char* const THIS_FUNCTION = "COProject::GetFileProperty";

	VERIFY_VALID_PROJECT("Unable to get a file property.")

	return "" ;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName, LPCSTR szFileName ,UINT iCtrlId, UINT iNewValue )
// Description: Set the value of a property for the given file. (See SetFileProperty(PROJ_PROP_CATEGORIES, LPCSTR, LPCSTR, UINT, LPCSTR) to set text-based properties.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group that contains the file.
// Param: szFileName A pointer to a string that contains the file to set properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// Param: iNewValue An integer that contains the new value for the property.
// END_HELP_COMMENT
int COProject::SetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName, LPCSTR szFileName ,UINT iCtrlId, UINT iNewValue )
{       // Display the Project Options dialog.
	const char* const THIS_FUNCTION = "COProject::SetFileProperty";

	VERIFY_VALID_PROJECT("Unable to set a file property.")

	UIProjOptionsTabDlg uipo;        // REVIEW(EnriqueP) Attach and use OK()
	uipo.AttachActive();
	if (!uipo.IsValid())
	{
		uipo.Display();
		uipo.AttachActive();
	}
	uipo.SelectGroup(szFileName,szGrpName) ; // REVIEW(Ivanl) Need to pass in a target name.
	return SetOption(iCategory,iCtrlId, iNewValue) ;

}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName, LPCSTR szFileName ,UINT iCtrlId, LPCSTR szNewValue )
// Description: Set the value of a property for the given file. (See SetFileProperty(PROJ_PROP_CATEGORIES, LPCSTR, LPCSTR, UINT, UINT) to set value-based properties.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory A value that indicates the category of the property: CT_GENERAL, CT_EXT_DBGOPTS, CT_MAKE_TYPE_LIB, CT_COMPILER, CT_RESOURCE, CT_BROWSER, CT_LINKER.
// Param: szGrpName A pointer to a string that contains the name of the group that contains the file.
// Param: szFileName A pointer to a string that contains the file to set properties for.
// Param: iCtrlId An integer that contains the ID of the control that identifies the property.
// Param: szNewValue A pointer to a string that contains the text value for the property.
// END_HELP_COMMENT
int COProject::SetFileProperty(PROJ_PROP_CATEGORIES iCategory,LPCSTR szGrpName, LPCSTR szFileName ,UINT iCtrlId, LPCSTR szNewValue )
{       // Display the Project Options dialog.
	const char* const THIS_FUNCTION = "COProject::SetFileProperty";

	VERIFY_VALID_PROJECT("Unable to set a file property.")

	UIProjOptionsTabDlg uipo;        // REVIEW(EnriqueP) Attach and use OK()
	uipo.AttachActive();
	if (!uipo.IsValid())
	{
		uipo.Display();
		uipo.AttachActive();
	}
	uipo.SelectGroup(szFileName,szGrpName) ; // REVIEW(Ivanl) Need to pass in a target name.
	return SetOption(iCategory,iCtrlId, szNewValue) ;

}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetOption(INT iCategory,UINT iCtrlId, LPCSTR szNewValue)
// Description: Set the text value of the project option specified by iCtrlId in the given category (iCategory). (See SetOption(UINT, UINT, UINT) to set value-based options.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory An integer that contains the project category that contains the option to set. This is the ID of a control on the property page in the project options dialog.
// Param: iCtrlId An integer that contains the ID of the control that identifies the option to set.
// Param: szNewValue A pointer to a string that contains the value to set the option to.
// END_HELP_COMMENT
int COProject::SetOption(INT iCategory,UINT iCtrlId, LPCSTR szNewValue)
 {
 	const char* const THIS_FUNCTION = "COProject::SetOption";

	VERIFY_VALID_PROJECT("Unable to set a project option.")

/*     if (UIWB.GetItemProperty(P_ProjIsExternalMakeFile) == "1")
	{
	UITabbedDialog PropTab ;
	PropTab.ShowPage(iCategory,6) ;
	if (!(hActive =ControlOnPropPage(iCtrlId)) ) // If the control is not on the page, try advanced.
		{       // Somewhat redundant since external projs don't have advanced, but who know what, things change!
			CString szItem = MyGetLabel(VPROJ_IDC_ADVANCED_OPT);
			if(!WButtonExists(szItem.GetBuffer(szItem.GetLength())))
			{
				return -1 ; // The control doesnt exist. (REVIEW) change return to constant error code.
			}
			WButtonClick(szItem.GetBuffer(szItem.GetLength())) ;
			Sleep(2000) ; // Wait for the dialog to show
			UITabbedDialog tabAdv ;
			if (!tabAdv.ShowPage(iCtrlId,8))
				return  ERROR_ERROR;
			hActive = ControlOnPropPage(iCtrlId) ;
		}
	}
	else
	{ */

//      MST.DoKeys("{TAB}") ;
	UITabbedDialog PropTab ;
	if (iCategory == CT_IMAGEBLD)
		PropTab.ShowPage("Xbox Image");
	else
		PropTab.ShowPage(iCategory,6) ;
	// If the control is not on the page, go through the categories listbox
	if (!(hActive =ControlOnPropPage(iCtrlId)))
		{       CString itemSt ="@" ;
			char * numSt = "                                 " ;
			int iCts ;
					//If no Categories combobox, error
			if ( (iCts =MST.WComboCount(MyGetLabel(VPROJ_IDC_COMBO1))) == -1)
			{
				MST.DoKeys("{ESC 2}") ;
				return ERROR_ERROR ;
			}
			// Loop through the categories combobox looking for the control.
			for (int i = 1; i < iCts+1; i++)
			{
				MST.WComboItemClk(MyGetLabel(VPROJ_IDC_COMBO1),itemSt+ _itoa(i,numSt, 10)) ;
				if ((hActive =ControlOnPropPage(iCtrlId)))
					i = iCts+1 ;
			}
		}
		if (!hActive) // The control wasn't found
		{
			MST.DoKeys("{ESC 2}") ;
			return ERROR_ERROR ;
		}

		/*      CString szItem = MyGetLabel(VPROJ_IDC_ADVANCED_OPT) ;
			if(!WButtonExists(szItem.GetBuffer(szItem.GetLength())))
			{
				return -1 ; // The control doesnt exist. (REVIEW) change return to constant error code.
			}
			WButtonClick(szItem.GetBuffer(szItem.GetLength())) ;
			Sleep(2000) ; // Wait for the dialog to show
			UITabbedDialog myTesttab ;
			if (!myTesttab.ShowPage(iCtrlId,8))
				return -1 ;
			hActive = ControlOnPropPage(iCtrlId) ;
		  */
//      }
	// We now have the page with the desired control.
	HWND hCtl = GetDlgItem(hActive,iCtrlId) ;
	CString szLabel, szNewText ;
	int iCtrlClass ;
	szNewText = szNewValue ;
	// We need the label, to call MSTest APIs.
	szLabel =MyGetLabel(iCtrlId) ;

	iCtrlClass = GetDlgControlClass(hCtl) ;
	// Determine the control class type
	switch (GetDlgControlClass(hCtl))
	{
		case(GL_COMBOBOX):
			MST.WComboItemDblClk(szLabel.GetBuffer(szLabel.GetLength()),(char *)szNewValue) ;
		break ;
		case (GL_LISTBOX):
			MST.WListItemDblClk(szLabel.GetBuffer(szLabel.GetLength()),(char *) szNewValue) ;
		break ;
		case (GL_EDIT):
			WSetText(hCtl,szNewText.GetBuffer(szNewText.GetLength())) ;
		break ;
		default:
		{
			MST.DoKeys("{ESC 2}")  ;
			return ERROR_ERROR ;
		}
	}

	// Close the dialog
	MST.DoKeys ("{ENTER}") ;
	Sleep(2000) ; // As we close the dialog
	if (IsWindowVisible(hActive))
	{
		MST.WButtonClick(GetLabel(IDOK)) ;
		Sleep(2000) ;
		if (IsWindowVisible(hActive)) // if OK didn't close the dialog
		{
			MST.DoKeys("{ESC 2}") ;
			return ERROR_ERROR ;
		}
		LOG->RecordInfo("{ENTER} did not close the Options Modal dialog");
	}
	return ERROR_SUCCESS ;
}

// BEGIN_HELP_COMMENT
// Function: INT COProject::SetOption(INT iCategory, UINT iCtrlId, UINT iNewValue)
// Description: Set the value of the project option specified by iCtrlId in the given category (iCategory). (See SetOption(UINT, UINT, LPCSTR) to set text-based options.)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: iCategory An integer that contains the project category that contains the option to set. This is the ID of a control on the property page in the project options dialog.
// Param: iCtrlId An integer that contains the ID of the control that identifies the option to set.
// Param: iNewValue An integer that contains the value to set the option to.
// END_HELP_COMMENT
INT COProject::SetOption(INT iCategory, UINT iCtrlId, UINT iNewValue)
{
	const char* const THIS_FUNCTION = "COProject::SetOption";

	VERIFY_VALID_PROJECT("Unable to set a project option.")

/*  (UIWB.GetItemProperty(P_ProjIsExternalMakeFile) == "1")
	{
	UITabbedDialog PropTab ;
	PropTab.ShowPage(iCategory,6) ;
	if (!(hActive =ControlOnPropPage(iCtrlId)) ) // If the control is not on the page, try advanced.
		{  // Somewhat redundant since external projs don't have advanced, but who know what, things change!
			CString szItem = MyGetLabel(VPROJ_IDC_ADVANCED_OPT);
			if(!WButtonExists(szItem.GetBuffer(szItem.GetLength())))
			{
				return -1 ; // The control doesnt exist. (REVIEW) change return to constant error code.
			}
			WButtonClick(szItem.GetBuffer(szItem.GetLength())) ;
			Sleep(2000) ; // Wait for the dialog to show
			UITabbedDialog tabAdv ;
			if (!tabAdv.ShowPage(iCtrlId,8))
				return  ERROR_ERROR;
			hActive = ControlOnPropPage(iCtrlId) ;
		}
	}
	else
	{         */
	UITabbedDialog PropTab ;
	PropTab.ShowPage(iCategory,6) ;
	// If the control is not on the page, go through the categories listbox
	if (!(hActive =ControlOnPropPage(iCtrlId)))
		{       CString itemSt ="@" ;
			char * numSt = "                                 " ;
			int iCts ;
					//If no Categories combobox, error
			if ( (iCts =MST.WComboCount(MyGetLabel(VPROJ_IDC_COMBO1))) == -1)
			{
				MST.DoKeys("{ESC 2}") ;
				return ERROR_ERROR ;
			}
			// Loop through the categories combobox looking for the control.
			for (int i = 1; i < iCts+1; i++)
			{
				MST.WComboItemClk(MyGetLabel(VPROJ_IDC_COMBO1),itemSt+ _itoa(i,numSt, 10)) ;
				if ((hActive =ControlOnPropPage(iCtrlId)))
					i = iCts+1 ;
			}
		}
		if (!hActive) // The control wasn't found
		{
			MST.DoKeys("{ESC 2}") ;
			return ERROR_ERROR ;
		}
	//}
	
	// We now have the page with the desired control.
	HWND hCtl = GetDlgItem(hActive,iCtrlId) ;
	CString szLabel ;
	CString index = "@" ;
	char * sz = "      " ;
		
	// We need the label, to call MSTest APIs.
	 szLabel = MyGetLabel(iCtrlId) ;
	 // We need a string out of iNewValue to use MSTest combo/list item APIs to click.
	 _itoa(iNewValue, sz, 10) ;
	 index = index + sz ;

	switch (GetDlgControlClass(hCtl))
	{
		case(GL_CHECKBOX):
		{
			if(iNewValue == OPTION_ON)
				MST.WCheckCheck(szLabel.GetBuffer(szLabel.GetLength())) ;
			else
				MST.WCheckUnCheck(szLabel.GetBuffer(szLabel.GetLength())) ;
		}
		break ;
		case(GL_RADIOBUTTON):
			if(iNewValue == OPTION_ON)
				MST.WOptionClick(szLabel.GetBuffer(szLabel.GetLength())) ;
		break ;
		case (GL_BUTTON):
			if(iNewValue == OPTION_ON)
				MST.WButtonClick(szLabel.GetBuffer(szLabel.GetLength())) ;
		break ;
		case(GL_COMBOBOX):
			MST.WComboItemDblClk(GetLabel(iCtrlId),index) ;
		break ;
		case (GL_LISTBOX):
			MST.WListItemDblClk(GetLabel(iCtrlId),index) ;
		break ;
		case (GL_EDIT):
			MST.WEditClick(szLabel.GetBuffer(szLabel.GetLength())) ;
		break ;
		default:
		{
			MST.DoKeys("{ESC 2}") ;
			return ERROR_ERROR ;
		}
	}

	// Close the dialog
	MST.DoKeys ("{ENTER}") ;
	Sleep(2000) ; // As we close the dialog
	if (IsWindowVisible(hActive))
	{
		MST.WButtonClick(GetLabel(IDOK)) ;
		Sleep(2000) ;
		if (IsWindowVisible(hActive)) // if OK didn't close the dialog
		{
			MST.DoKeys("{ESC 2}") ;
			return ERROR_ERROR ;
		}
		LOG->RecordInfo("{ENTER} did not close the Options Modal dialog");
	}
	return ERROR_SUCCESS ;
}


CString MyGetLabel(UINT id)
{
	HWND hwndDialog = ControlOnPropPage(id);
	HWND hwndControl = GetDlgItem(hwndDialog, id);
	EXPECT( IsWindow(hwndControl) );

	int iClass = GetLabelNo(hwndControl);

	switch( iClass )
	{
		case GL_STATIC:
		case GL_BUTTON:
			return GetText(hwndControl);
		case GL_EDIT:
		case GL_LISTBOX:
		case GL_COMBOBOX:
		{
			HWND hwndLabel = GetWindow(hwndControl, GW_HWNDPREV);
			EXPECT( hwndLabel != NULL );
			EXPECT( GetLabelNo(hwndLabel) == GL_STATIC );
			return GetText(hwndLabel);
		}
		case GL_DIALOG:
		case GL_NONE:
		default:
			EXPECT(FALSE);
	}
	return CString("");
}

// BEGIN_HELP_COMMENT
// Function: int COProject::MakeProjectExternal(LPCSTR sName/* =NULL*/)
// Description: Make the project an external project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: LPCSTR A pointer to a string that contains the new name of the project, if desired. NULL means don't rename the project. (Default value is NULL.)
// END_HELP_COMMENT
int COProject::MakeProjectExternal(LPCSTR sName/* =NULL*/)
{
	const char* const THIS_FUNCTION = "COProject::MakeProjectExternal";

	CFile fUserFile ;
	CFileException e;
	char zero = '0' ;
	if (!sName)
	{       if (m_FullPath.IsEmpty())
			return ERROR_ERROR ;
		sName =(const char *)  m_FullPath ;
	}

   if( !(fUserFile.Open( sName, CFile::modeReadWrite, &e )))
   {
	return ERROR_ERROR ;
   }

  fUserFile.Seek(60, CFile::begin) ;

  fUserFile.Write(&zero, sizeof(char)) ;
  fUserFile.Close() ;
  return ERROR_SUCCESS ;
}

void COProject::SetPathMembers()
{
	const char* const THIS_FUNCTION = "COProject::SetPathMembers";

	CString szBuf, fname, Drive, Ext, Path ;
	m_FullPath = UIWB.GetProjPath() ;
	_splitpath((const char *) m_FullPath,Drive.GetBuffer(_MAX_DRIVE), Path.GetBuffer(_MAX_DIR),
			 m_ProjName.GetBuffer(_MAX_FNAME), Ext.GetBuffer(_MAX_EXT) );

	Drive.ReleaseBuffer() ;
	Path.ReleaseBuffer() ;
	m_ProjName.ReleaseBuffer() ;
	Ext.ReleaseBuffer() ;
	m_ProjName.MakeUpper();
	m_ProjDir = Drive+Path;
}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetPlatform(PLATFORM_TYPE platform)
// Description: Set the current platform for the project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: platform A value that specifies the platform: PLATFORM_WIN32_X86, PLATFORM_WIN32S_X86, PLATFORM_WIN32_MIPS, PLATFORM_MAC_68K, PLATFORM_MAC_PPC.
// END_HELP_COMMENT
int COProject::SetPlatform(int platform)
{
	const char* const THIS_FUNCTION = "COProject::SetTarget";

	VERIFY_VALID_PROJECT("Unable to set project platform.")

	int index;

	switch(platform)
		{
		case PLATFORM_WIN32_X86:
		case PLATFORM_WIN32S_X86:
		case PLATFORM_WIN32_MIPS:
		case PLATFORM_WIN32_ALPHA:
			index = 1;
			break;
		case PLATFORM_MAC_68K:
			index = 2;
			break;
		case PLATFORM_MAC_PPC:
			index = 3;
			break;
		default:
			return ERROR_ERROR;
		}
		
	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.WaitAttachActive(3000);
	uitd.SetOption(VPROJ_IDC_EXTOPTS_PLATFORM, VPROJ_IDC_EXTOPTS_PLATFORM, index);
	MST.WButtonClick(GetLabel(IDOK));
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetRemotePath(LPCSTR remote_file, TARGET_TYPE target /*= TARGET_CURRENT*/, SRT_OPT srt_opt /*= PREPEND_REMOTE_PATH*/, PLATFORM_TYPE platformad /*= PLATFORM_USER*/)
// Description: Set the remote path for this project. This is used for remote transfer and remote debugging.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: remote_file A pointer to a string that contains the remote file name.
// Param: target A value that indicates the target to set the remote path for: TARGET_WIN32_X86_DEBUG, TARGET_WIN32_X86_RELEASE, TARGET_MAC_68K_DEBUG, TARGET_MAC_68K_RELEASE, TARGET_MAC_PPC_DEBUG, TARGET_MAC_PPC_RELEASE, TARGET_USER, TARGET_CURRENT. (Default value is TARGET_CURRENT.)
// Param: srt_opt A value that indicates whether the full remote path is supplied or to prepend the given path to the existing remote path: PREPEND_REMOTE_PATH, FULL_PATH_SUPPLIED. (Default value is PREPEND_REMOTE_PATH.)
// Param: platform A value that specifies the platform: PLATFORM_WIN32_X86, PLATFORM_WIN32S_X86, PLATFORM_WIN32_MIPS, PLATFORM_MAC_68K, PLATFORM_MAC_PPC. (Default value is PLATFORM_USER.)
// END_HELP_COMMENT
int COProject::SetRemotePath(LPCSTR remote_file,
							 TARGET_TYPE target /* TARGET_CURRENT */,
							 SRT_OPT srt_opt /* PREPEND_REMOTE_PATH */,
							 PLATFORM_TYPE platform /* PLATFORM_USER */)

{
	const char* const THIS_FUNCTION = "COProject::SetRemotePath";

	VERIFY_VALID_PROJECT("Unable to set the remote path.")

	CString remote_target;
	ASSERT(target == TARGET_CURRENT);       // todo: add support for changing target
										
	if(srt_opt == PREPEND_REMOTE_PATH)
		{
		CString remote_path = gpConnec->GetUserRemoteLocalPath(platform);
		remote_target = remote_path + remote_file;
		}

	else
		remote_target = remote_file;
								
	/* pre-xbox stuff
	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.WaitAttachActive(3000);
	
	if(uitd.SetOption(VPROJ_IDC_REMOTE_TARGET, VPROJ_IDC_REMOTE_TARGET,
					  remote_target) != ERROR_SUCCESS)

		{
		LOG->RecordInfo("ERROR in COProject::SetRemotePath(): "
			  "failed to set remote path option");

		return ERROR_ERROR;
		}

	MST.WButtonClick(GetLabel(IDOK));
	return ERROR_SUCCESS;
	*/

	// xbox version	follows
	/*
	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.AttachActive();

	// show debug page and select dll category	
	uitd.ShowPage(GetLocString(IDSS_TB_DEBUG));
	MST.WComboItemClk("@1", "@1");

	MST.DoKeys("{TAB}",FALSE, 250); //switch focus to the edit box
	Sleep(250); //safety
	MST.DoKeys("{TAB}",FALSE, 250);
	Sleep(250);
	MST.DoKeys("{TAB}",FALSE, 250);
	Sleep(250);
	MST.DoKeys("{TAB}",FALSE, 250);
	Sleep(250);
	MST.DoKeys(remote_target,TRUE);
	Sleep(250);
	MST.DoKeys("{TAB}",FALSE, 250); //switch focus away
	MST.WButtonClick(GetLabel(IDOK));
	*/
	SetProjProperty(CT_DEBUG, DBG_IDC_REMOTE_TARGET, remote_file);
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetAdditionalDLLs(LPCSTR szDLLs)
// Description: Set additional DLLs to be loaded when debugging this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: LPCSTR A pointer to a string that contains the name of the additional DLL to add.
// END_HELP_COMMENT
int COProject::SetAdditionalDLLs(LPCSTR szDLLs)
{
	const char* const THIS_FUNCTION = "COProject::SetAdditionalDLLs";

	VERIFY_VALID_PROJECT("Unable to set additional DLLs.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.AttachActive();

	// TODO (michma): this function cannot work until it is
	// able to select the targets for which it wants to load dlls.
	// when more than one target platform is selected,
	// the dll list is disabled.

	// show debug page and select dll category	
	uitd.ShowPage(GetLocString(IDSS_TB_DEBUG));
	MST.WComboItemClk("@1", "@2");

	MST.DoKeys("{TAB}",FALSE, 1000); //switch focus to the custom conrol
	Sleep(1000); //safety
	MST.DoKeys(szDLLs,TRUE);
	Sleep(1000); //safety
	MST.DoKeys("{TAB}",FALSE, 1000); //switch focus away
	MST.WButtonClick(GetLabel(IDOK));
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::RemoveAdditionalDLLs(int iDll)
// Description: Remove a dll from the Additional Dlls list.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: int iDll - An integer specifying which dll in the list to remove.
// END_HELP_COMMENT
int COProject::RemoveAdditionalDLLs(int iDll)
{
	const char* const THIS_FUNCTION = "COProject::RemoveAdditionalDLLs";

	VERIFY_VALID_PROJECT("Unable to remove additional DLLs.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.AttachActive();

	// TODO (michma): this function cannot work until it is
	// able to select the targets for which it wants to load dlls.
	// when more than one target platform is selected,
	// the dll list is disabled.

	// show debug page and select dll category	
	uitd.ShowPage(GetLocString(IDSS_TB_DEBUG));
	MST.WComboItemClk("@1", "@2");

	MST.DoKeys("{TAB}",FALSE, 1000); //switch focus to the custom conrol
	Sleep(1000); //safety
	
	// cursor down to the dll we are removing.
	for(int i = 1; i < iDll; i++)
		MST.DoKeys("{DOWN}");

	// remove the dll.
	MST.DoKeys("{DEL}");
	Sleep(1000); //safety
	MST.DoKeys("{TAB}",FALSE, 1000); //switch focus away
	MST.WButtonClick(GetLabel(IDOK));
	return ERROR_SUCCESS;
	}


// emmang@xbox
// BEGIN_HELP_COMMENT
// Function: int COProject::SetLocateOtherDLLs(LPCSTR szDLLs)
// Description: Sets whether additional DLLs will be located when debugging this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: LPCSTR A pointer to a string that contains the name of the additional DLL to add.
// END_HELP_COMMENT
int COProject::SetLocateOtherDLLs(TOGGLE_TYPE choice)
{
	/*
	const char* const THIS_FUNCTION = "COProject::SetLocateOtherDLLs";

	VERIFY_VALID_PROJECT("Unable to set locate other DLLs.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.AttachActive();

	// TODO (michma): this function cannot work until it is
	// able to select the targets for which it wants to load dlls.
	// when more than one target platform is selected,
	// the dll list is disabled.

	// show debug page and select dll category	
	uitd.ShowPage(GetLocString(IDSS_TB_DEBUG));
	MST.WComboItemClk("@1", "@2");

	if (choice == TOGGLE_ON)
		MST.WCheckCheck("Try to locate other DLLs");
	else
		MST.WCheckUnCheck("Try to locate other DLLs");
	MST.WButtonClick(GetLabel(IDOK));
	return ERROR_SUCCESS;
	*/
	return SetProjProperty(CT_DEBUG, DBG_IDC_TRY_LOCATE_DLLS, (choice == TOGGLE_ON ? 1 : 0));
}


// BEGIN_HELP_COMMENT
// Function: CString COProject::GetRemoteTarget(void)
// Description: Get the project's remote path.
// Return: A CString that contains the project's remote path.
// END_HELP_COMMENT
CString COProject::GetRemoteTarget(void)
{
	const char* const THIS_FUNCTION = "COProject::GetRemoteTarget";

	VERIFY_VALID_PROJECT("Unable to get remote target.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.AttachActive();
	CString remote = uitd.GetOptionStr(VPROJ_IDC_REMOTE_TARGET,VPROJ_IDC_REMOTE_TARGET);
	MST.WButtonClick(GetLabel(IDOK));
	return remote;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetBuildCommand(LPCSTR command)
// Description: Set the build command if this project is an external project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: LPCSTR A pointer to a string that contains the build command.
// Param: index The 1 based index of the configuration to choose.  0 (default) selects all configurations.
// END_HELP_COMMENT
int COProject::SetBuildCommand(LPCSTR command, int index /* = 0 */)
{
	const char* const THIS_FUNCTION = "COProject::SetBuildCommand";

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.AttachActive();
	if (index != 0)		// Make sure the configuration at the given index is selected.
	{
		char buf[5];
		wsprintf(buf, "@%d", index);
		MST.WListItemClk(buf, GetLocString(IDSS_LISTBOX_BUILD_SETTING_CFG));
	}
	uitd.SetOption(VPROJ_IDC_EXTOPTS_CMDLINE, VPROJ_IDC_EXTOPTS_CMDLINE, command);
	MST.WButtonClick(GetLabel(IDOK));
	return ERROR_SUCCESS;
	}

int ExecuteShellCMD(char *cmd, ...) {                   // Args should be char*, last one=NULL!
	va_list marker;
	int     retval;
	CString szCommand;
	char*   szTemp;
	CString szSYSTEMlog;

	va_start(marker, cmd);
	for (szTemp = cmd; szTemp != NULL; szTemp = va_arg(marker, char*))
		szCommand = (szCommand + szTemp) + " ";
	va_end( marker );

	GetTempFileName(".", "prj", 0, szSYSTEMlog.GetBuffer(MAX_PATH));
	szSYSTEMlog.ReleaseBuffer();
	switch (retval = system(szCommand + ">" + szSYSTEMlog + " 2>&1 ")) {
		case  0: if (errno!=ENOENT) return ERROR_SUCCESS;
		case -1:
			LOG->RecordFailure((LPCSTR) "system(\"%s\") failed.", (LPCSTR) szCommand);
			switch (errno) {
				case E2BIG:   LOG->RecordFailure((LPCSTR) "Argument list is too big"); break;
				case ENOENT:  LOG->RecordFailure((LPCSTR) "Command interpreter not found"); break;
				case ENOEXEC: LOG->RecordFailure((LPCSTR) "CMD interpreter has invalid format"); break;
				case ENOMEM:  LOG->RecordFailure((LPCSTR) "Not enough memory to run command"); break;
				default:      LOG->RecordFailure((LPCSTR) "errno=%d is not documented", errno); break;
			}
			break;
		default:LOG->RecordFailure((LPCSTR) "Returned %d=system(\"%s\")", retval, (LPCSTR) szCommand);
	}
	LOG->RecordTextFile(szSYSTEMlog, "System log");
	return ERROR_ERROR;
}


// BEGIN_HELP_COMMENT
// Function: int COProject::UpdateRemoteFile(LPCSTR local_file /*= NULL*/, TARGET_TYPE target /*= CURRENT_TARGET*/, PLATFORM_TYPE platform /*= PLATFORM_USER*/)
// Description: Update the remote file from the host machine to the remote machine.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: local_file A pointer to a string that contains the local filename of the file to transfer. This parameter is only used when PLATFORM_WIN32S_X86 is specified by the platform parameter. (Default value is NULL.)
// Param: target A value that indicates the target to set the remote path for: TARGET_WIN32_X86_DEBUG, TARGET_WIN32_X86_RELEASE, TARGET_MAC_68K_DEBUG, TARGET_MAC_68K_RELEASE, TARGET_MAC_PPC_DEBUG, TARGET_MAC_PPC_RELEASE, TARGET_USER, TARGET_CURRENT. (Default value is TARGET_CURRENT.)
// Param: platform A value that specifies the platform: PLATFORM_WIN32_X86, PLATFORM_WIN32S_X86, PLATFORM_WIN32_MIPS, PLATFORM_MAC_68K, PLATFORM_MAC_PPC. (Default value is PLATFORM_USER.)
// END_HELP_COMMENT
int COProject::UpdateRemoteFile(LPCSTR local_file /* NULL */,
								TARGET_TYPE target /* CURRENT_TARGET */,
								PLATFORM_TYPE platform /* PLATFORM_USER */)

{
	const char* const THIS_FUNCTION = "COProject::UpdateRemoteFile";

	VERIFY_VALID_PROJECT("Unable to update the remote file.")

	ASSERT(target == TARGET_CURRENT);       //todo: add support for changing target

	// GetUserTargetPlatforms() must only return 1 platform!
		
	PLATFORM_TYPE true_platform = (platform != PLATFORM_USER) ? platform :
								  (PLATFORM_TYPE)GetUserTargetPlatforms();
	
	switch(true_platform)
	
		{
		case PLATFORM_WIN32_X86:        // since the ide doesn't handle remote
		case PLATFORM_WIN32_MIPS:
		case PLATFORM_WIN32_ALPHA:
		case PLATFORM_WIN32S_X86:       // copies to win32/s machines, we must
									// be told what the local file name is.
			{
			ASSERT(local_file != NULL);
			CString remote_file = local_file;

			remote_file = gpConnec->GetUserRemoteSharePath(platform) +
						  remote_file.Right(remote_file.GetLength() -
											remote_file.ReverseFind('\\') - 1);

			// some debugees are part of the slm project so they
			// get copied to the win32/s machine as read-only.
			// however that causes subsequent copies to fail
			// which will invalidate the test if the debugee
			// really needs to be updated.
			
			SetFileAttributes(remote_file, FILE_ATTRIBUTE_NORMAL);
			CopyFile(local_file, remote_file, FALSE);
			break;
			}
																	

		case PLATFORM_MAC_68K:
		case PLATFORM_MAC_PPC:
			{
			UIWB.DoCommand(IDM_PROJECT_TOOL_MECR, DC_MNEMONIC);
			break;
			}

		default:
			{
			ASSERT(0);
			}
		}

	WaitUntilBuildDone(30);       // need 30 minutes to copy appwiz app to mac via serial.
	return VerifyBuild();        // ditto
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetLocalTarget(LPCSTR localfile, LPCSTR config, DEFAULT_DBG_EXE_TYPE ddetDefaultDbgExe /* NO_DEFAULT_DBG_EXE */)
// Description: Set the name of the executable for debugging with this project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: localfile A pointer to a string that contains the name of the executable for debugging. If this parameter is NULL, the ddetDefaultDbgExe parameter determines which local target is selected.
// Param: config A pointer to a string that contains the name of the configuration for which to set the local target. If this parameter is NULL (default), the default configuration is set. 
// Param: ddetDefaultDbgExe (optional) A DEFAULT_DBG_EXE_TYPE enum that specifies which default debug exe to use (DEFAULT_DEBUG_EXE_CONTAINER, DEFAULT_DBG_EXE_BROWSER). the default is NO_DEFAULT_DBG_EXE.
// END_HELP_COMMENT
int COProject::SetLocalTarget(LPCSTR localfile, LPCSTR config, DEFAULT_DBG_EXE_TYPE ddetDefaultDbgExe /* NO_DEFAULT_DBG_EXE */)
{
	
	// TODO (michma): this function should be using UIProjOptionsTabDlg for all the project settings manipulation.
	const char* const THIS_FUNCTION = "COProject::SetLocalTarget";

	VERIFY_VALID_PROJECT("Unable to set local target name.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UIProjOptionsTabDlg uiProjOptDlg;
	uiProjOptDlg.Display();

	if(config)
	{
		// REVIEW(michma): why won't SelectTarget work? ShowPage (below) fails if we use it.
		//uiProjOptDlg.SelectTarget(config);
		uiProjOptDlg.GiveFocusToConfigList();
		MST.DoKeys(config);
		// this will give focus back to the settings side of the project settings dlg.
		MST.DoKeys("{TAB 2}");
	}
	
	if(!uiProjOptDlg.ShowPage(GetLocString(IDSS_TB_DEBUG)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetLocalTarget: "
						"could not find page \"%s\" in dialog", 
						(LPCSTR)GetLocString(IDSS_TB_DEBUG));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	if(!MST.WComboItemExists("@1", GetLocString(IDSS_CATEGORY_GENERAL)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetLocalTarget: "
						"could not find category \"%s\" in page %s", 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
						(LPCSTR)GetLocString(IDSS_TB_DEBUG));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	else
		MST.WComboItemClk("@1", 1);		// set category to General		

	if(!MST.WEditExists(GetLocString(IDSS_EXE_FOR_DBG_SESSION)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetLocalTarget: "
						"could not find edit field \"%s\" in category \"%s\" of page \"%s\"", 
						(LPCSTR)GetLocString(IDSS_EXE_FOR_DBG_SESSION), 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
						(LPCSTR)GetLocString(IDSS_TB_DEBUG));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	// if a literal file is specified, enter it.
	if(localfile)
		MST.WEditSetText("@1", localfile);
	// else select the default debug executable specified (either the test container or the internet browser).
	else
	{
		// the button (an arrow button) for this pop-up menu is the first "Button" on the page.
		MST.WButtonClick("@1");
		// select the default debug exe specified (the DEFAULT_DBG_EXE_TYPE is mapped accordingly)
		for(int i = 0; i < ddetDefaultDbgExe; i++)
			MST.DoKeys("{DOWN}");
		// commit the selection from the pop-up menu.
		MST.DoKeys("{ENTER}");
	}

	
	MST.WButtonClick(GetLocString(IDSS_OK));
	return uiProjOptDlg.WaitUntilGone(5000) ? ERROR_SUCCESS : ERROR_ERROR;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetOutputFile(LPCSTR localfile)
// Description: Set the location for the output executable.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: LPCSTR A pointer to a string that contains the location of the output executable.
// END_HELP_COMMENT
int COProject::SetOutputFile(LPCSTR localfile)
{
	const char* const THIS_FUNCTION = "COProject::SetOutputFile";

	VERIFY_VALID_PROJECT("Unable to set output file name.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.WaitAttachActive(1000);

	if(!uitd.ShowPage(GetLocString(IDSS_PROJSET_LINKER)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetOutputFile: "
						"could not find page \"%s\" in dialog", 
						(LPCSTR)GetLocString(IDSS_PROJSET_LINKER));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	if(!MST.WComboItemExists("@1", GetLocString(IDSS_CATEGORY_GENERAL)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetOutputFile: "
						"could not find category \"%s\" in page %s", 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
					    (LPCSTR)GetLocString(IDSS_PROJSET_LINKER));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	else
		MST.WComboItemClk("@1", 1);		// set category to General		

	if(!MST.WEditExists(GetLocString(IDSS_OUTPUT_FILE)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetOutputFile: "
						"could not find edit field \"%s\" in category \"%s\" of page \"%s\"", 
						(LPCSTR)GetLocString(IDSS_OUTPUT_FILE), 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
						(LPCSTR)GetLocString(IDSS_PROJSET_LINKER));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	MST.WEditSetText("@1", localfile);
	MST.WButtonClick(GetLocString(IDSS_OK));
	return uitd.WaitUntilGone(5000) ? ERROR_SUCCESS : ERROR_ERROR;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::ToggleBSCGeneration(TOGGLE_TYPE t)
// Description: Toggle automatic BSC Generation for the project (off by default).
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: TOGGLE_TYPE t specifies whether to turn automatic BSC off (TOGGLE_OFF) or on (TOGGLE_ON)
// END_HELP_COMMENT
int COProject::ToggleBSCGeneration(TOGGLE_TYPE t)

	{
	const char* const THIS_FUNCTION = "COProject::ToggleBSCGeneration";
	VERIFY_VALID_PROJECT("Unable to toggle bsc generation.")

	// bring up project settings dlg.
	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.WaitAttachActive(1000);

	// select compiler page.
	if(!uitd.ShowPage(GetLocString(IDSS_PROJSET_CPLUSPLUS)))
		
		{
		LOG->RecordInfo("ERROR in COProject::ToggleBSCGeneration: "
						"could not find page \"%s\" in dialog", 
						(LPCSTR)GetLocString(IDSS_PROJSET_CPLUSPLUS));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	// verify general category exists.
	if(!MST.WComboItemExists("@1", GetLocString(IDSS_CATEGORY_GENERAL)))
		
		{
		LOG->RecordInfo("ERROR in COProject::ToggleBSCGeneration: "
						"could not find category \"%s\" in page %s", 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
					    (LPCSTR)GetLocString(IDSS_PROJSET_CPLUSPLUS));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	else
		MST.WComboItemClk("@1", 1);		// set category to General		

	// verify "generate browse info" check-box exists.
	if(!MST.WCheckExists(GetLocString(IDSS_GENERATE_BROWSE_INFO)))
		
		{
		LOG->RecordInfo("ERROR in COProject::ToggleBSCGeneration: "
						"could not find check box \"%s\" in page \"%s\"", 
						(LPCSTR)GetLocString(IDSS_GENERATE_BROWSE_INFO),
						(LPCSTR)GetLocString(IDSS_PROJSET_CPLUSPLUS));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	// toggle check-box state.
	if(t == TOGGLE_ON)
		MST.WCheckCheck(GetLocString(IDSS_GENERATE_BROWSE_INFO));
	else
		MST.WCheckUnCheck(GetLocString(IDSS_GENERATE_BROWSE_INFO));

	// selet browser page.
	if(!uitd.ShowPage(GetLocString(IDSS_PROJSET_BROWSER)))
		
		{
		LOG->RecordInfo("ERROR in COProject::ToggleBSCGeneration: "
						"could not find page \"%s\" in dialog", 
						(LPCSTR)GetLocString(IDSS_PROJSET_BROWSER));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	// verify "Build browse info file" check-box exists.
	if(!MST.WCheckExists(GetLocString(IDSS_BUILD_BROWSE_INFO_FILE)))
		
		{
		LOG->RecordInfo("ERROR in COProject::ToggleBSCGeneration: "
						"could not find check box \"%s\" in page \"%s\"", 
						(LPCSTR)GetLocString(IDSS_BUILD_BROWSE_INFO_FILE),
						(LPCSTR)GetLocString(IDSS_PROJSET_BROWSER));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	// toggle check-box state.
	if(t == TOGGLE_ON)
		MST.WCheckCheck(GetLocString(IDSS_BUILD_BROWSE_INFO_FILE));
	else
		MST.WCheckUnCheck(GetLocString(IDSS_BUILD_BROWSE_INFO_FILE));
	
	// close project settings.
	MST.WButtonClick(GetLocString(IDSS_OK));
	uitd.WaitUntilGone(5000) ? ERROR_SUCCESS : ERROR_ERROR;

	// check to see if we get a msg about the project being read-only.
	if(MST.WFndWndWaitC(GetLocString(IDSS_PROJECT_IS_READ_ONLY), "Static", FW_PART, 3))
		{
		// if it's there, ok it and wait for it to disappear.
		MST.DoKeys("{ENTER}");	
		while(MST.WFndWndWaitC(GetLocString(IDSS_PROJECT_IS_READ_ONLY), "Static", FW_PART,1));
		}
		
	return ERROR_SUCCESS;	
	}

	// BEGIN_HELP_COMMENT
// Function: int COProject::SetDebugInfo(DEBUG_INFO_TYPE DebugInfo)
// Description: Select the debug info type (current target assumed).
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: DEBUG_INFO_TYPE DebugInfo specifies the debug info type to select (NO_DEBUG_INFO, LINE_NUMBERS_ONLY, C7_COMPATIBLE, PROGRAM_DATABASE, EC_PROGRAM_DATABASE)
// END_HELP_COMMENT
int COProject::SetDebugInfo(DEBUG_INFO_TYPE DebugInfo)

{
	const char* const THIS_FUNCTION = "COProject::SetDebugInfo";
	VERIFY_VALID_PROJECT("Unable to set debug info.")

	// bring up project settings dlg.
	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.WaitAttachActive(1000);

	// select compiler page.
	if(!uitd.ShowPage(GetLocString(IDSS_PROJSET_CPLUSPLUS)))
		
	{
		LOG->RecordInfo("ERROR in COProject::SetDebugInfo: "
						"could not find page \"%s\" in dialog", 
						(LPCSTR)GetLocString(IDSS_PROJSET_CPLUSPLUS));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
	}

	// verify general category exists.
	if(!MST.WComboItemExists("@1", GetLocString(IDSS_CATEGORY_GENERAL)))
		
	{
		LOG->RecordInfo("ERROR in COProject::SetDebugInfo: "
						"could not find category \"%s\" in page %s", 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
					    (LPCSTR)GetLocString(IDSS_PROJSET_CPLUSPLUS));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
	}

	else
		MST.WComboItemClk("@1", 1);		// set category to General		

	// select the specified debug info option.
	MST.WComboItemClk("@4", DebugInfo);
	// close project settings.
	MST.WButtonClick(GetLocString(IDSS_OK));
	uitd.WaitUntilGone(5000) ? ERROR_SUCCESS : ERROR_ERROR;

	// check to see if we get a msg about the project being read-only.
	if(MST.WFndWndWaitC(GetLocString(IDSS_PROJECT_IS_READ_ONLY), "Static", FW_PART, 3))
	{
		// if it's there, ok it and wait for it to disappear.
		MST.DoKeys("{ENTER}");	
		while(MST.WFndWndWaitC(GetLocString(IDSS_PROJECT_IS_READ_ONLY), "Static", FW_PART,1));
	}
		
	return ERROR_SUCCESS;	
}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetCustomBuildOptions(LPCSTR szDescription, CStringArray &cstrArrayBuildCmds, CStringArray &cstrArrayOutputFiles, ConfigStrArray &csArrayConfigs)
// Description: Set the custom build options (including description, build commands, and output files) for one or more projects or files.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szDescription A pointer to a string that contains the custom build description.
// Param: cstrArrayBuildCmds A reference to a CStringArray that contains the list of custom build commands.
// Param: cstrArrayOutputFiles A reference to a CStringArray that contains the list of custom output files.
// Param: csArrayConfigs A reference to a ConfigStrArray  that contains a list of configurations in the project settings dlg. the specified custom options will be applied to those configurations. 
// END_HELP_COMMENT
int COProject::SetCustomBuildOptions(LPCSTR szDescription, 
									 CStringArray &cstrArrayBuildCmds,
									 CStringArray &cstrArrayOutputFiles,
									 ConfigStrArray &csArrayConfigs)

	{
	const char* const THIS_FUNCTION = "COProject::SetCustomBuildOptions";
	VERIFY_VALID_PROJECT("Unable to set custom build options.")
	// dummy file list.
	CStringArray cstrArrayFiles;

	return SetCustomBuildOptions(szDescription, cstrArrayBuildCmds, cstrArrayOutputFiles, 
								 csArrayConfigs, cstrArrayFiles);
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetCustomBuildOptions(LPCSTR szDescription, CStringArray &cstrArrayBuildCmds, CStringArray &cstrArrayOutputFiles, ConfigStrArray &csArrayConfigs, CStringArray &cstrArrayFiles, int intConfigInclusion = INCLUDE_CONFIG_LEVEL)
// Description: Set the custom build options (including description, build commands, and output files) for one or more projects or files.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szDescription A pointer to a string that contains the custom build description.
// Param: cstrArrayBuildCmds A reference to a CStringArray that contains the list of custom build commands.
// Param: cstrArrayOutputFiles A reference to a CStringArray that contains the list of custom output files.
// Param: csArrayConfigs A reference to a ConfigStrArray  that contains a list of configurations in the project settings dlg. the specified custom options will be applied to those configurations. 
// Param: cstrArrayFiles A reference to a CStringArray that contains a list of files inside configurtaions in the project settings dlg. the specified custom options will be applied to those files of the specified configurations.  
// Param: intConfigInclusion An int value indicating whether or not to apply the custom build settings at the configuration level as well as the file level (INCLUDE_CONFIG_LEVEL or EXCLUDE_CONFIG_LEVEL). the default is INCLUDE_CONFIG_LEVEL.
// END_HELP_COMMENT
int COProject::SetCustomBuildOptions(LPCSTR szDescription, 
									 CStringArray &cstrArrayBuildCmds,
									 CStringArray &cstrArrayOutputFiles,
									 ConfigStrArray &csArrayConfigs, 
									 CStringArray &cstrArrayFiles,
									 int intConfigInclusion /* INCLUDE_CONFIG_LEVEL */)

	{
	const char* const THIS_FUNCTION = "COProject::SetCustomBuildOptions";
	VERIFY_VALID_PROJECT("Unable to set custom build options.")

 	// bring up the project settings dlg.
	if(m_uPrjOptDlg.Display() == NULL)
		
		{
		LOG->RecordInfo("ERROR in %s: could not open \"%s\" dlg.", 
						THIS_FUNCTION, GetLocString(IDSS_PROJ_OPTIONS_TITLE));

		// just in case the wrong dlg came up.
		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	// get total number of configs listed in project settings.
	int intTotalConfigs = MST.WListCount("@1");
	// we'll delete files from the temp list as we process them.
	CStringArray cstrArrayFilesTmp;
	// gets the number of files displayed for a configuration in the project settings dlg.
	int intTotalFiles;
	// gets the "Input File:" name displayed in the custom build settings page.
	CString cstrInputFile;
	// gets the index of the backslash character in the "Input File:" name.
	int intIndexBackslash;
	// various "for" loop counters.
	int i, f, j;

	// loop through each of the specified configurations.
	for(int c = 0; c < csArrayConfigs.GetSize(); c++)

		{
		// select the configuration.
		m_uPrjOptDlg.SelectTarget(csArrayConfigs[c].GetString());

		// if specified, apply custom build settings to configuration level.
		if(intConfigInclusion == INCLUDE_CONFIG_LEVEL)

			{
			// select custom build settings page.
			if(!m_uPrjOptDlg.ShowPage(GetLocString(IDSS_PROJSET_CUSTBUILD)))
				
				{
				LOG->RecordInfo("ERROR in %s: could not find page \"%s\" for configuration %s.", 
								THIS_FUNCTION, GetLocString(IDSS_PROJSET_CUSTBUILD), 
								csArrayConfigs[c].GetString());

				m_uPrjOptDlg.Cancel();
				return ERROR_ERROR;		
				}

			// set the description, build command, and output file options for the configuration.
			if(SetCustomBuildOptionsCore(szDescription, cstrArrayBuildCmds, cstrArrayOutputFiles) == ERROR_ERROR)
				
				{
				LOG->RecordInfo("ERROR in %s: could not set options for configuration \"%s\".", 
								THIS_FUNCTION, csArrayConfigs[c].GetString());

				m_uPrjOptDlg.Cancel();
				return ERROR_ERROR;		
				}
			}

		// apply custom settings to any specified files in the configuration.
		if(cstrArrayFiles.GetSize() > 0)

			{
			// expand the configuration to reveal the files.
			m_uPrjOptDlg.ExpandConfig();						
			// the config list box item count should go up by the number of files in the config. 
			intTotalFiles = MST.WListCount("@1") - intTotalConfigs;
				
			// verify we have some files to process. 
			if(intTotalFiles == 0)
				{
				LOG->RecordInfo("ERROR in %s: could not expand configuration \"%s\".",
								 THIS_FUNCTION, csArrayConfigs[c].GetString());

				m_uPrjOptDlg.Cancel();
				return ERROR_ERROR;		
				}

			// load up the temp array used to keep track of which files have been processed.
			for(i = 0; i < cstrArrayFiles.GetSize(); i++)
				cstrArrayFilesTmp.Add(cstrArrayFiles[i]);

			// loop through each of the displayed files.
			for (i = 0; i < intTotalFiles; i++)
				
				{
				// select the next file in the configuration.
				m_uPrjOptDlg.GiveFocusToConfigList();
				MST.DoKeys("{DOWN}");

				// verify we can select the custom build settings page for this file. if not, skip to next.
				if(m_uPrjOptDlg.ShowPage(GetLocString(IDSS_PROJSET_CUSTBUILD)))
					
					{
					// get the "Input File:" string for the file.
					MST.WStaticText("@2", cstrInputFile);
					// find the backslash in the "Input File:" string.				
					intIndexBackslash = cstrInputFile.Find("\\");
					
					// verify that we found a backslash.
					if(intIndexBackslash < 0)

						{ 
						LOG->RecordInfo("ERROR in %s: no backslash found in \"Input File:\" string \"%s\".",
										 THIS_FUNCTION, cstrInputFile);

						m_uPrjOptDlg.Cancel();
						return ERROR_ERROR;		
						}

					// extract just the filename from the "Input File:" string (to the right of the backslash).
					cstrInputFile = cstrInputFile.Mid(cstrInputFile.Find("\\") + 1); 

					// loop through each of the unprocessed, specified files.
					for(f = 0; f < cstrArrayFilesTmp.GetSize(); f++)
						
						{
						// check if the "Input File:" file matches a specified file.
						if(cstrArrayFilesTmp[f] == cstrInputFile)
							
							{
							// set the description, build command, and output file options for the file.
							if(SetCustomBuildOptionsCore(szDescription, cstrArrayBuildCmds, cstrArrayOutputFiles) == ERROR_ERROR)
								
								{
								LOG->RecordInfo("ERROR in %s: could not set options for file \"%s\" in "
												"configuration %s", THIS_FUNCTION, cstrArrayFilesTmp[f],
												csArrayConfigs[c].GetString());

								m_uPrjOptDlg.Cancel();
								return ERROR_ERROR;		
								}

							// the specified file has been processed so remove it from the list.
							cstrArrayFilesTmp.RemoveAt(f);
							break;
							}
						}
					}

				// if all the specified files have been processed, 
				// then skip the rest of the files listed in the dlg. 
				if(cstrArrayFilesTmp.GetSize() == 0)
					break;
				}
			
			// check if we didn't find all the specified files in those listed in the dlg.
			if(cstrArrayFilesTmp.GetSize() > 0)
				
				{
				LOG->RecordInfo("ERROR in %s: could not find all specified files in configuration \"%s\"."								"configuration %s", THIS_FUNCTION, cstrArrayFilesTmp[f],
								THIS_FUNCTION, csArrayConfigs[c].GetString());

				m_uPrjOptDlg.Cancel();
				return ERROR_ERROR;		
				}

			// give focus back to configuration list.
			m_uPrjOptDlg.GiveFocusToConfigList();

			// navigate back up to the configuration level.	"i" was incremented for each key DOWN we did
			// except for the one that selected the last specified file to be processed.
			for(j = 0; j < (i + 1); j++)
				MST.DoKeys("{UP}");

			// collapse the configuration.
			m_uPrjOptDlg.CollapseConfig();						

			// the config list box item count should return to the number of configs displayed. 
			if(MST.WListCount("@1") != intTotalConfigs)
				
				{
				LOG->RecordInfo("ERROR in %s: could not collapse configuration \"%s\".",
								 THIS_FUNCTION, csArrayConfigs[c].GetString());

				m_uPrjOptDlg.Cancel();
				return ERROR_ERROR;		
				}
			}
		}

	//m_uPrjOptDlg.OK();
	// VERY CHEESY WORK ARROUND
	// Clickin OK on JPN will fail the test
	// But there is nothing wrong with the app.
	MST.DoKeys("{TAB}");
	MST.DoKeys("{TAB}");
	MST.DoKeys("{TAB}");
	MST.DoKeys("{ENTER}");

	return ERROR_SUCCESS;
	}


int COProject::SetCustomBuildOptionsCore(LPCSTR szDescription, 
										 CStringArray &cstrArrayBuildCmds,
										 CStringArray &cstrArrayOutputFiles)

	{
	const char* const THIS_FUNCTION = "COProject::SetCustomBuildOptionsCore";

	// set the description.
	MST.WEditSetText("@1", szDescription);
	// set the build commands.
	CString Alt = "%" ;
	Alt += ExtractHotKey(GetLocString(IDS_BUILDCOMMANDS));
  	SetCustomBuildLists(Alt.GetBuffer(Alt.GetLength()), cstrArrayBuildCmds);
	// set the output files.
	Alt = "%";
	Alt += ExtractHotKey(GetLocString(IDS_OUTPUTFILE));

	SetCustomBuildLists(Alt.GetBuffer(Alt.GetLength()), cstrArrayOutputFiles);

	return ERROR_SUCCESS;
	}


void COProject::SetCustomBuildLists(LPCSTR szListAccel, CStringArray &cstrArrayList)
	
	{
	// give focus to the edit box
	MST.DoKeys(szListAccel);

	// delete current contents
	MST.DoKeys("{DELETE}^c");

	// enter each of the new items.
	CString csBarfItIn("");
	for(int i = 0; i < cstrArrayList.GetSize(); i++)
		{
		csBarfItIn += cstrArrayList[i] + CString("\r\n");
		}
	MST.WEditSetText("", csBarfItIn);
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::SetOutputTarget(LPCSTR outfile)
// Description: Set the name of the output target when this project is an external project.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: outfile A pointer to a string that contains the name of the output filename.
// END_HELP_COMMENT
int COProject::SetOutputTarget(LPCSTR outfile)
{
	const char* const THIS_FUNCTION = "COProject::SetOutputTarget";

	VERIFY_VALID_PROJECT("Unable to set output target name.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.SetOption(VPROJ_IDC_EXTOPTS_TARGET, VPROJ_IDC_EXTOPTS_TARGET, outfile);
	MST.WButtonClick(GetLabel(IDOK));
	return ERROR_SUCCESS;
	}

// BEGIN_HELP_COMMENT
// Function: HWND COProject::AppWizard(int bSaveChanges /*= FALSE*/)
// Description: Bring up the AppWizard (New Project) dialog.
// Return: The HWND of the AppWizard (New Project) dialog.
// Param: bSaveChanges A Boolean value that specifies whether to save the dirty files before closing them. (Default value is FALSE.)
// END_HELP_COMMENT
// this function is taken directly from uwbframe.cpp because it does not belong there
HWND COProject::AppWizard(int bSaveChanges /*=FALSE*/)
{
	const char* const THIS_FUNCTION = "COProject::AppWizard";

//	DWORD dwProcessId;

	ActivateProjWnd();

	UINewFileDlg nwFlDlg ;
	
	nwFlDlg.Display() ;
	nwFlDlg.NewProjectTab() ;
	WaitForInputIdle(g_hTargetProc, 60000);

//	nwFlDlg.SelectFromList(GetLocString(IDSS_NEW_PROJECT));
	
	/* UIWB.DoCommand(ID_FILE_NEW, DC_MNEMONIC);   // cannot use CreateNewFile, because that checks for new MDI child

	UIDialog nd(0x64, GL_LISTBOX);      // AFX_IDC_LISTBOX?
//  (GetLocString(IDSS_NEW_TITLE));
	if( !nd.WaitAttachActive(1000) )
	{
		LOG->RecordInfo("%s: could not open the File/New dialog. Unable to start AppWizard.", THIS_FUNCTION);
		MST.DoKeys("{ESC 5}") ;
		return NULL;
	}

	MST.WListItemClk("", GetLocString(IDSS_NEW_PROJECT));
    */
	//nd.OK();
/*	MST.WButtonClick(GetLabel(IDOK)) ;

	// get the process ID of UIWB
	GetWindowThreadProcessId(UIWB.HWnd(), &dwProcessId);
	// wait until we don't get any Save Changes? message boxes
	for (int i = 0; i < 20; i++) {
		// 8 is the Yes button
		if (WaitForSingleControl(dwProcessId, 8, 500) == WAIT_CONTROL_0) {
			if (bSaveChanges) {
				// click Yes
				MST.WButtonClick(GetLabel(8));
			}
			else {
				// click No
				MST.WButtonClick(GetLabel(9));
			}
		}
		else {
			break;
		}
	}

	// too many Save Changes? message boxes
	if (i >= 20) {
		LOG->RecordInfo("%s: detected more than 20 Save Changes? message boxes when creating new project. Unable to start AppWizard.", THIS_FUNCTION);
		return NULL;
	}

	// wait for the new project dialog to be displayed
	if (WaitForSingleControl(dwProcessId, VPROJ_IDC_WORKSPACE_NAME, 10000) != WAIT_CONTROL_0) {
		LOG->RecordInfo("%s: cannot find the New Project dialog (WaitForSingleControl timed out). Unable to start AppWizard.", THIS_FUNCTION);
		return NULL;
	}

	// attach to the new project dialog
	UIAppWizard uaw;
	if (!uaw.WaitAttachActive(2000)) {
		// Cancel out of the dialog and exit.
		LOG->RecordInfo("%s: could not open the New Project dialog (UIAppWizard::WaitAttachActive failed). Unable to start AppWizard.", THIS_FUNCTION);
		MST.DoKeys("{ESC 3}");
		return NULL;
	}
	  */
	Sleep(500) ;
	UIAppWizard uaw = MST.WGetActWnd(0)	;

	return uaw;
}


// BEGIN_HELP_COMMENT
// Function: HWND COProject::CreateNewProject(LPCSTR szProjName, PROJTYPE ProjType, LPCSTR szDir, LPCSTR szNewSubDir, int ProjFileOpt, LPCSTR szProjFilesPath, LPCSTR szRemotePath, LPCSTR szRemoteFile)
// Description: This function creates a new project.
// Return: The HWND of the workspace window.
// Param: szProjName - Name of project to create.
// Param: ProjType - Type of project.  UIAW_PT_APPWIZ, etc.
// Param: szDir - Directory in which to create the project.
// Param: szNewSubDir - Further subdirectory specifier for project.  Based off of szDir.
// Param: ProjFileOpt - Not currently used.
// Param: szProjFilesPath - Not currently used.
// Param: szRemotePath - Not currently used.
// Param: szRemoteFile - Not currently used.
// END_HELP_COMMENT
HWND COProject::CreateNewProject(LPCSTR szProjName, 
								PROJTYPE ProjType,
								LPCSTR szDir /* NULL */,
								LPCSTR szNewSubDir /* NULL */,
				/* Not used */	int ProjFileOpt /* 0 */,
				/* Not used */	LPCSTR szProjFilesPath /* NULL */,
				/* Not used */	LPCSTR szRemotePath /* NULL */,
				/* Not used */	LPCSTR szRemoteFile /* NULL */)

{
	const char* const THIS_FUNCTION = "COProject::CreateNewProject";

	// REVIEW(briancr):why is all the AppWizard work done at this level?
	// Shouldn't the AppWz class traverse the AppWizard pages as the
	// caller sets attributes of the wizard?

	EXPECT(UIWB.IsValid() & UIWB.IsActive());
	UIWB.DeleteProject(szProjName, szDir);          

	CString DirPath  = szDir ;
	UIAppWizard uaw = AppWizard();

	uaw.SetProjType(ProjType) ;

	if( szDir != NULL && *szDir != '\0')
		uaw.SetDir(DirPath);

	if( szNewSubDir != NULL && *szNewSubDir != '\0')
	{                       
		if (szNewSubDir[0] == '\\' || (strlen(szNewSubDir) > 1 && szNewSubDir[1] == ':'))
			LOG->RecordInfo("%s: Illegal subdirectory specified for new project: %s", THIS_FUNCTION, szNewSubDir);
		else
			uaw.SetSubDir(szNewSubDir);
	}

	uaw.SetName(szProjName) ;
	
	uaw.Create();

	// create a new AppWizard EXE project
	if (ProjType == UIAW_PT_APPWIZ)   // Just handles the default AppWizard project.
	{  // This will create a default AppWizard application.
		UIWindow uiMsgbox  ;
		if (!UIWB.WaitOnWndWithCtrl(APPWZ_IDC_RADIO1,2000))
		{
			LOG->RecordInfo("%s: could not find the first AppWizard EXE page. Unable to create new project.", THIS_FUNCTION);
			 // Cancel out of the dialog and exit.
			 MST.DoKeys("{ESC 5}") ;
			 return NULL ;
		}
		MST.WButtonClick(GetLabel(APPWZ_IDC_END)) ;
		if (!UIWB.WaitOnWndWithCtrl(APPWZ_IDC_OUTPUT,2000))
		{
			LOG->RecordInfo("%s: could not find the summary AppWizard EXE page. Unable to create new project.", THIS_FUNCTION);
			 MST.DoKeys("{ESC 5}") ;
			 return NULL ;
		}

		MST.WButtonClick(GetLabel(IDOK))  ;
		uiMsgbox.AttachActive() ;
		Sleep(1500) ; // Shouldn't take that long. REVIEW(Ivan) shaky ..
	}
	// create a new AppWizard DLL project
	else if(ProjType ==UIAW_PT_APPWIZ_DLL)   // Just handles the default AppWizard project.
	{  // This will create a default AppWizard application.
		UIWindow uiMsgbox  ;
		if (!UIWB.WaitOnWndWithCtrl(APPWZ_IDC_RADIO3,2000))
		{
			LOG->RecordInfo("%s: could not find the first AppWizard DLL page. Unable to create new project.", THIS_FUNCTION);
			 // Cancel out of the dialog and exit.
			 MST.DoKeys("{ESC 5}") ;
			 return NULL ;
		}
		MST.WButtonClick(GetLabel(IDOK)) ;
		if (!UIWB.WaitOnWndWithCtrl(APPWZ_IDC_OUTPUT,2000))
		{
			LOG->RecordInfo("%s: could not find the summary AppWizard DLL page. Unable to create new project.", THIS_FUNCTION);
			 MST.DoKeys("{ESC 5}") ;
			 return NULL ;
		}

		MST.WButtonClick(GetLabel(IDOK))  ;
		uiMsgbox.AttachActive() ;
		Sleep(1500) ; // Shouldn't take that long. REVIEW(Ivan) shaky ..
		return MST.WGetActWnd(0) ;
	}
	// create a new project
	else {
		WaitForInputIdle(g_hTargetProc, 60000);
	}

	// bring up the project window
	NewProjectWindow();

	// return the HWND of the project window
	return *(GetProjectWindow());
}

UIWorkspaceWindow* COProject::NewProjectWindow(void)
{
	// delete any existing project window
	DeleteProjectWindow();

	ActivateProjWnd();		// Make sure the window is visible.

	m_pProjWnd = new UIWorkspaceWindow;
	m_hWnd = *m_pProjWnd;

	return m_pProjWnd;
}

BOOL COProject::DeleteProjectWindow(void)
{
	delete m_pProjWnd;
	m_pProjWnd = NULL;
	m_hWnd = NULL;

	return TRUE;
}

UIWorkspaceWindow* COProject::GetProjectWindow(void)
{
	return m_pProjWnd;
}

BOOL COProject::IsOpen(void)
{
	return (m_pProjWnd != NULL);
}

// BEGIN_HELP_COMMENT
// Function: HWND COProject::VerifyClasses(LPCSTR pszBaseLineFile)
// Description: This function creates a
// Param: pszBaseLineFile - Name of file containing 
// END_HELP_COMMENT
void COProject::VerifyClasses(LPCSTR pszBaseLineFile)
{
    CClassInfo infoClass;
	CGlobalInfo infoGlobal;
    BOOL bFoundOne = FALSE;

    try
    {
        CStdioFile fileVerify(pszBaseLineFile, CFile::modeRead | CFile::typeText);

	    const int len=100;
		char buf[len];
	    CString str("");

		// Type of object comes first.
		while (fileVerify.ReadString(buf, len))
		{
			// Get a line from the file.
			str = strtok(buf, "\n\r");
			TrimString(&str);

			if (!str.IsEmpty() && str.GetAt(0) != ';')	// If we got something and it wasn't a comment...
			{
				// Get the rest of this thing's info...
				if (strstr(buf, "class "))
				{
					infoClass.ClearAll();
					infoClass.m_strName = strchr(buf, ' ') + 1;
					TrimString(&infoClass.m_strName);
					GetClassInfo(fileVerify, &infoClass);
					VerifySingleClass(infoClass);
				}
				else if (strstr(buf, "global"))
				{
					
					infoGlobal.ClearAll();
					infoGlobal.m_strName = strchr(buf, ' ') + 1;
					TrimString(&infoGlobal.m_strName);

					if (strstr(buf, "globalfunc"))
						infoGlobal.m_Type = CGlobalInfo::FUNCTION;
					else
						infoGlobal.m_Type = CGlobalInfo::DATA;

					GetGlobalInfo(fileVerify, &infoGlobal);
					VerifySingleGlobal(infoGlobal);
				}
			}
		}
    } 
    catch(CFileException *e)
    {
        if(e->m_cause != CFileException::endOfFile)
        {
            CFileStatus status;
            if (CFile::GetStatus(pszBaseLineFile, status))
				LOG->RecordFailure("Couldn't read the file: %s", status.m_szFullName);
			else
				LOG->RecordFailure("Couldn't find the file: %s", pszBaseLineFile);
        }
    }
}

void COProject::GetClassInfo(CStdioFile& file, CClassInfo *pInfo)
{
    CString str("");
    const int len=100;
    char buf[len];

    // Now get rest of information, if any.
    CString strDelimiter("***");
    BOOL bDone = FALSE;

    while (!bDone && file.ReadString(buf, len))
    {
        str = buf;
        TrimString(&str);

        if (!str.IsEmpty() && str.GetAt(0) != ';')
        {
            if (str == strDelimiter)
                bDone = TRUE;
            else if (str.Find("base") > -1)
            {
                pInfo->m_strBase = strtok(buf, ";");
                TrimString(&pInfo->m_strBase);
            }
			else if (str.Find("mf") == 0 || str.Find("md") == 0)
			{
				CMemberInfo* pNew = new CMemberInfo();
				strtok(buf, ";");
				pNew->m_strDataType = strtok(NULL, ";");
				pNew->m_strName	= strtok(NULL, ";");
				TrimString(&pNew->m_strDataType);
				TrimString(&pNew->m_strName);

				CString strAccess = strtok(NULL, ";");	
				TrimString(&strAccess);
				if (!strcmp("private", strAccess))
					pNew->m_Access = CMemberInfo::PRIVATE;
				else if (!strcmp("public", strAccess))
					pNew->m_Access = CMemberInfo::PUBLIC;
				else if (!strcmp("protected", strAccess))
					pNew->m_Access = CMemberInfo::PROTECTED;

				pNew->m_Type = str.GetAt(1) == 'd'? CMemberInfo::DATA: CMemberInfo::FUNCTION;
				pInfo->m_listMembers.AddTail(pNew);
			}
        }
    }
}

void COProject::GetGlobalInfo(CStdioFile& file, CGlobalInfo *pInfo)
{
    CString str("");
    const int len=100;
    char buf[len];

    // Now get rest of information, if any.
    CString strDelimiter("***");
    BOOL bDone = FALSE;

    while (!bDone && file.ReadString(buf, len))
    {
        str = buf;
        TrimString(&str);

        if (!str.IsEmpty() && str.GetAt(0) != ';')
        {
            if (str == strDelimiter)
                bDone = TRUE;
        }
    }
}


void COProject::VerifySingleClass(const CClassInfo& info)
{
	const char* const THIS_FUNCTION = "COProject::VerifySingleClass";

	BOOL bFound = FALSE;	// Has a class member been found?  Used to determine if a shortcut can be taken.
	int status;				// Return status of Select... functions.

    LOG->RecordInfo("Checking class: %s", info.GetName());

	UIWorkspaceWindow wnd;

	BOOL bFindMoreMembers = TRUE;	// Should additional members be searched for?

	// Verify existence of any member functions/data.
	POSITION pos = info.m_listMembers.GetHeadPosition();
	while(bFindMoreMembers)
	{
		wnd.ActivateClassPane();

		CMemberInfo* pMember;		// Only used if the current class has members.
		 
		if (pos == NULL)
			status = wnd.GotoClassDefinition(info.GetName(), NULL, GetName());		// Class has no members specified, so just check for class.
		else
		{
			pMember = info.m_listMembers.GetAt(pos);		// Class has members.

			if (bFound == FALSE)
				 status = wnd.GotoClassMemberDefinition(pMember->GetName(), 
											  pMember->GetType() == CMemberInfo::FUNCTION? UIWorkspaceWindow::Function : UIWorkspaceWindow::Data,
											  NULL,
											  info.GetName(),
											  GetName());
			else	// Shortcut...
			{
				MST.DoKeys("{left 2}");	// Get back to class name and force collapse, otherwise, first members might not be found.				
				status = wnd.GotoClassMemberDefinition(pMember->GetName(),
											 pMember->GetType() == CMemberInfo::FUNCTION? UIWorkspaceWindow::Function : UIWorkspaceWindow::Data,
											 NULL);
			}
		}

		switch(status)
		{
			case ERROR_NO_MEMBER:
				if (pMember->GetType() == CMemberInfo::FUNCTION)
					LOG->RecordFailure("Couldn't find member function %s::%s() in Class View", info.GetName(), pMember->GetName());
				else
					LOG->RecordFailure("Couldn't find data member %s::%s in Class View", info.GetName(), pMember->GetName());
				bFound = FALSE;
			break;

			case ERROR_NO_CLASS:
				LOG->RecordFailure("Couldn't find class %s in the ClassView.", info.GetName());
				bFindMoreMembers = FALSE;
			break;

			case ERROR_NO_PROJECT:
				LOG->RecordFailure("Couldn't find project %s in the ClassView.", GetName());
				bFindMoreMembers = FALSE;
			break;

			case ERROR_GOTO:
				if (pos == NULL)	// We were navigating to a class.
					LOG->RecordFailure("Navigation to wrong place for definition for class %s", info.GetName());
				else				// We were navigating to a class member.
					LOG->RecordFailure("Navigation to wrong place for definition for %s::%s", info.GetName(), pMember->GetName());
				bFound = TRUE;
			break;

			case ERROR_NO_DECLARATION:
				LOG->RecordFailure("ClassView reports that %s::%s has no declaration.", info.GetName(), pMember->GetName());
				bFound = TRUE;
			break;

			case ERROR_NO_DEFINITION:
				if (pos == NULL)	// We were navigating to a class.
					LOG->RecordFailure("ClassView reports that class %s has no definition.", info.GetName());
				else				// We were navigating to a class member.
					LOG->RecordFailure("ClassView reports that %s::%s has no definition.", info.GetName(), pMember->GetName());
				bFound = TRUE;
			break;

			case SUCCESS:
				if (pos != NULL)	// If we were successful navigating to a member definition, try to get to its declaration.
				{
					wnd.ActivateClassPane();

					if (pMember->GetType() == CMemberInfo::FUNCTION && 
						wnd.GotoDeclarationFromHere(pMember->GetName()) != SUCCESS)
						LOG->RecordFailure("Couldn't navigate to member declaration %s::%s", info.GetName(), pMember->GetName());
					bFound = TRUE;
				}

			break;

			default:
				LOG->RecordFailure("%s:  An error occurred in UIWorkspaceWindow::GotoClassMemberDefinition()", THIS_FUNCTION);
				bFindMoreMembers = FALSE;
			break;

		}

		if (pos == NULL)
			bFindMoreMembers = FALSE;

		if (bFindMoreMembers == TRUE)
			info.m_listMembers.GetNext(pos);

	}	// end while.
}

void COProject::VerifySingleGlobal(const CGlobalInfo& info)
{
	const char* const THIS_FUNCTION = "COProject::VerifySingleGlobal";

    LOG->RecordInfo("Checking global symbol: %s", info.GetName());

	UIWorkspaceWindow wnd;
	wnd.ActivateClassPane();

	switch(wnd.GotoGlobalDefinition(info.GetName(),
						  info.GetType() == CGlobalInfo::FUNCTION? UIWorkspaceWindow::Function: UIWorkspaceWindow::Data,
						  NULL,
						  GetName()))
	{
		case ERROR_NO_CLASS:
			LOG->RecordFailure("Couldn't find class %s in the ClassView.", info.GetName());
		break;

		case ERROR_NO_PROJECT:
			LOG->RecordFailure("Couldn't find project %s in ClassView.", GetName());
		break;

		case ERROR_GOTO:
			LOG->RecordFailure("Couldn't navigate to global definition for %s.", GetName());
		break;

		case SUCCESS:
		break;

		default:
			LOG->RecordFailure("%s:  An error occurred in UIWorkspaceWindow::GotoGlobalDefinition()", THIS_FUNCTION);
		break;
	}
}


ConfigStr::ConfigStr(LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild)
	{
	SetString(szProj, ptPlatform, btBuild);
	}


CString ConfigStr::SetProject(LPCSTR szProj)
	{
	m_cstrProj = szProj;
	return BuildString();
	}


CString ConfigStr::SetPlatform(PLATFORM_TYPE ptPlatform)
	{
	m_ptPlatform = ptPlatform;
	return BuildString();
	}


CString ConfigStr::SetBuild(BUILD_TYPE btBuild)
	{
	m_btBuild = btBuild;
	return BuildString();
	}


CString ConfigStr::SetString(LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild)
	{
	m_cstrProj = szProj;
	m_ptPlatform = ptPlatform;
	m_btBuild = btBuild;
	return BuildString();
	}


CString ConfigStr::BuildString(void)
	
	{
	CString cstrPlatform;
	CString cstrBuild;
	
	switch(m_ptPlatform)
		{
		case PLATFORM_WIN32_X86:
		case PLATFORM_WIN32S_X86:
		case PLATFORM_WIN32_MIPS:
		case PLATFORM_WIN32_ALPHA:
		case PLATFORM_WIN32_PPC:
			cstrPlatform = "Win32";
			break;
		case PLATFORM_MAC_68K:
			cstrPlatform = "Macintosh";
			break;
		case PLATFORM_MAC_PPC:
			cstrPlatform = "Power Macintosh";
			break;
		}

	switch(m_btBuild)
		{
		case BUILD_DEBUG:
			cstrBuild = "Debug";			
			break;
		case BUILD_RELEASE:
			cstrBuild = "Release";			
			break;
		}

	m_cstrConfig = m_cstrConfig = m_cstrProj + " - " + cstrPlatform + " " + cstrBuild;
	return m_cstrConfig;
	}


CString ConfigStrArray::Add(LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild)
	{
	m_configs.Add(new ConfigStr(szProj, ptPlatform, btBuild));
  	return ((ConfigStr *)(m_configs[m_configs.GetSize() - 1]))->GetString();
	}


CString ConfigStrArray::Add(ConfigStr &config_str)
	{
	m_configs.Add(new ConfigStr(config_str.GetProject(), config_str.GetPlatform(), config_str.GetBuild()));
  	return ((ConfigStr *)(m_configs[m_configs.GetSize() - 1]))->GetString();
	}


CString ConfigStrArray::InsertAt(int intIndex, LPCSTR szProj, PLATFORM_TYPE ptPlatform, BUILD_TYPE btBuild)
	{
	m_configs.InsertAt(intIndex, new ConfigStr(szProj, ptPlatform, btBuild));
  	return ((ConfigStr *)(m_configs[intIndex]))->GetString();
	}


CString ConfigStrArray::InsertAt(int intIndex, ConfigStr &config_str)
	
	{
	m_configs.InsertAt(intIndex, new ConfigStr(config_str.GetProject(), 
											   config_str.GetPlatform(),
											   config_str.GetBuild()));

  	return ((ConfigStr *)(m_configs[intIndex]))->GetString();
	}


ConfigStr ConfigStrArray::operator[](int intIndex)
	{
	return *((ConfigStr *)(m_configs[intIndex]));
	}

// BEGIN_HELP_COMMENT
// Function: int COProject::SetProgramArguments(LPCSTR progargs, LPCSTR config /* NULL */)
// Description: Set program arguments for the exe (debuggee).
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: LPCSTR A pointer to a string that contains the program arguments.
// Param: config A pointer to a string that contains the name of the configuration for which to set the program arguments. If this parameter is NULL (default), the default configuration is set. 
// END_HELP_COMMENT
int COProject::SetProgramArguments(LPCSTR progargs, LPCSTR config /* NULL */)
{
	const char* const THIS_FUNCTION = "COProject::SetProgramArguments";

	VERIFY_VALID_PROJECT("Unable to set program arguments.")

	UIWB.DoCommand(IDM_PROJECT_SETTINGS, DC_MNEMONIC);
	UIProjOptionsTabDlg uiProjOptDlg;
	uiProjOptDlg.Display();

	if(config)
	{
		// REVIEW(michma): why won't SelectTarget work? ShowPage (below) fails if we use it.
		//uiProjOptDlg.SelectTarget(config);
		uiProjOptDlg.GiveFocusToConfigList();
		MST.DoKeys(config);
		// this will give focus back to the settings side of the project settings dlg.
		MST.DoKeys("{TAB 2}");
	}
	
	if(!uiProjOptDlg.ShowPage(GetLocString(IDSS_TB_DEBUG)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetProgramArguments: "
						"could not find page \"%s\" in dialog", 
						(LPCSTR)GetLocString(IDSS_TB_DEBUG));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	if(!MST.WComboItemExists("@1", GetLocString(IDSS_CATEGORY_GENERAL)))
		
		{
		LOG->RecordInfo("WARNING in COProject::SetProgramArguments: "
						"could not find category \"%s\" in page %s", 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
						(LPCSTR)GetLocString(IDSS_TB_DEBUG));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	else
		MST.WComboItemClk("@1", 1);		// set category to General		

	if(!MST.WEditExists(GetLocString(IDSS_PROGRAM_ARGUMENTS)))
		{
		LOG->RecordInfo("WARNING in COProject::SetProgramArguments: "
						"could not find edit field \"%s\" in category \"%s\" of page \"%s\"", 
						(LPCSTR)GetLocString(IDSS_PROGRAM_ARGUMENTS), 
						(LPCSTR)GetLocString(IDSS_CATEGORY_GENERAL),
						(LPCSTR)GetLocString(IDSS_TB_DEBUG));

		MST.DoKeys("{ESC}");
		return ERROR_ERROR;		
		}

	MST.WEditSetText("@3", progargs);
	MST.WButtonClick(GetLocString(IDSS_OK));
	return uiProjOptDlg.WaitUntilGone(5000) ? ERROR_SUCCESS : ERROR_ERROR;
	}


// BEGIN_HELP_COMMENT
// Function: int COProject::ExportMakefile()
// Description: To create an export makefile that is compatiable with NMAKE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COProject::ExportMakefile()
{  
	const char* const THIS_FUNCTION = "COProject::ExportMakefile";

	VERIFY_VALID_PROJECT("Unable to export makefile.")

	UIWB.DoCommand(IDM_PROJITEM_EXPORT_MAKEFILE, DC_MNEMONIC) ;
	return TRUE;
}



// BEGIN_HELP_COMMENT
// Function: int COProject::AddSQLDataSource()
// Description: To insert the Datasource into the database project
// Param: DSN A name of the DSN to be found in the ODBC dialog
// Param: User A user name for connection dialog
// Param: Psw Apassword for the connection dialog
// Param: DataBase A database name
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COProject::AddSQLDataSource(CString &DSN,CString &User,CString &Psw,
								LPCSTR DataBase /*=NULL*/)
{
	BOOL fileDSN=(DSN.Right(4)==".DSN");

    LOG->RecordInfo("Adding datasource %s to the project", DSN);
	UIWB.DoCommand(IDM_PROJ_ADD_DATASOURCE, DC_MNEMONIC);
	if(WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
	{
		LOG->RecordFailure("Coudn't bring up the connection dialog");
		return ERROR_ERROR;
	}
	Sleep(1000); //WaitForInputIdle does not work here, big sefety required
	
	HWND hwnd;
	if (hwnd = MST.WFndWndWait(GetLocString(IDS_SELECT_DSN), FW_PART, 5))
	{
		LOG->Comment("Found Window: '%s'", GetLocString(IDS_SELECT_DSN));
		MST.WSetActWnd(hwnd);

		RECT rcPage;
		GetClientRect(hwnd, &rcPage);
		ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+150, rcPage.top - 15 );	  // Click on first Tab	(left most)
		if(fileDSN==FALSE) //ODBC3.0 Registry data source
		{ 	// we end up here with new ODBC connect
			ClickMouse( VK_LBUTTON ,hwnd, rcPage.left+150, rcPage.top + 15 );	  // Click on first Tab	(left most)
			Sleep(1000);
			if(!MST.WViewItemExists("@1", DSN))
			{
				LOG->RecordFailure("Registry Data Source (%s) does not exists", DSN);
				MST.WButtonClick(GetLabel(IDCANCEL)); //close SQL Data Sources dialog
				return ERROR_ERROR;
			}
			MST.WViewItemClk("@1", DSN);
		}
		else // if(fileDSN==TRUE) //ODBC3.0 file data source
		{
			MST.WEditSetText(GetLocString(IDS_DSN_NAME)/*GetLabel(0x75F /* IDC_DSN_NAME) */, DSN);
		}
	}
	else
	{
		LOG->RecordFailure("Coudn't find the Data Source List");
		return ERROR_ERROR;
	}
	MST.WButtonClick(GetLabel(IDOK)); //close SQL Data Sources dialog

	if(ValidatePassword(User,Psw,DataBase)!=ERROR_SUCCESS)
		return ERROR_ERROR; //login failed 
	return ERROR_SUCCESS;
} // COProject::AddSQLDataSource


// BEGIN_HELP_COMMENT
// Function: int COProject::ValidatePassword()
// Description: To accept params database/user/password in SQL server login dialog
// Param: User A user name for connection dialog
// Param: Psw Apassword for the connection dialog
// Param: DataBase A database name (maybe NULL if default to be used)
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// END_HELP_COMMENT
int COProject::ValidatePassword(CString &User,CString &Psw,
								LPCSTR DataBase /*=NULL*/)
{
	LPCSTR Login=User;
	LPCSTR PassWord=Psw;

	WaitForInputIdle(g_hTargetProc, 5000);
	Sleep(500); // big sefety required	
	if(MST.WFndWndWait(GetLocString(IDSS_SQL_LOGIN), FW_PART, 5)==NULL)
	{
		LOG->RecordInfo("Didn't find SQL server login dialog");
		return ERROR_ERROR;
	}	

	MST.WCheckUnCheck("@1");

	MST.WEditSetText(GetLocString(IDS_LOGIN_ID)/*GetLabel(IDC_LOGIN_ID: 0x7534)*/, Login);
	if(PassWord!=NULL)
		MST.WEditSetText(GetLocString(IDS_PASSWORD)/*GetLabel(IDC_PASSWORD_ID: 0x7535)*/, PassWord);
	MST.WButtonClick(GetLocString(IDS_OPTIONS1)/*GetLabel(IDC_OPTIONS: 0x7541)*/);
	
	Sleep(1000); // big sefety required
	if(DataBase!=NULL)
	{
		int tries_tconnect=1;
		if(GetSystem() & SYSTEM_WIN)
			tries_tconnect=2; //one extra retry on Win95
		do
		{
		tries_tconnect--;
		MST.DoKeyshWnd(MST.WGetActWnd(0),"{F4}"); //this should try to login SQL server
		if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
			LOG->RecordFailure("Coudn't connect to the server");
		Sleep(1000); // WaitForInputIdle doesn't work big sefety required
		UIMessageBox wndMsgBox;
		if (wndMsgBox.AttachActive())		
		{	// If we can attach, then a login error must've occurred.
			if(MST.WStaticExists(GetLocString(IDS_CONN_FAILED)))
			{
				wndMsgBox.ButtonClick();
				if(tries_tconnect)
					LOG->RecordInfo("FAILED: SQL server login");
				else
				{
					MST.WButtonClick(GetLabel(IDCANCEL));
					LOG->RecordFailure("SQL server login failed.");
					return ERROR_ERROR; //login failed
				}
			}
		}
			Sleep(500); // safety
		}while(tries_tconnect>0);

		if(!MST.WComboItemExists("&Database:" /*GetLabel(0x753A) IDC_DATABASE */, DataBase))
		{
			LOG->RecordFailure("Coudn't find the Database %s",DataBase);
			MST.WButtonClick(GetLabel(IDCANCEL));
			return ERROR_ERROR; //no database found
		}
		MST.WComboItemClk("&Database:" /*GetLabel(0x753A)  IDC_DATABASE */, DataBase);
	} //DataBase!=NULL
	
	MST.WButtonClick(GetLabel(IDOK)); //SQL server chosen
	if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
		LOG->RecordFailure("Coudn't connect to the server");
	Sleep(1000); // WaitForInputIdle doesn't work big sefety required
	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())		
	{	// If we can attach, then a login error must've occurred.
		if(MST.WStaticExists(GetLocString(IDS_CONN_FAILED)))
		{
			wndMsgBox.ButtonClick();
			LOG->RecordFailure("SQL server login failed.");
			MST.WButtonClick(GetLabel(IDCANCEL));
			return ERROR_ERROR; //login failed
		}
	}
	return ERROR_SUCCESS;
} //COProject::ValidatePassword


// BEGIN_HELP_COMMENT
// Function: BOOL COProject::AddComponent(COComponent *pComponent, LPCSTR szLocation /* NULL */)
// Description: Adds a component to the project.
// Param: COComponent *pComponent - A pointer to an object representing the component to add. The actual type will be a component-specific derivative of COComponent. For a list of these, see COCOMPON.H.
// Param: szLocation (optional) - The directory the component is in. It may be relative (to the Gallery directory) or absolute. If not supplied, the default components directory is used (ex. Developer Studio Components). 
// Return: BOOL - Returns TRUE if the component was successfully added, FALSE if not.
// END_HELP_COMMENT
BOOL COProject::AddComponent(COComponent *pComponent, LPCSTR szLocation /* NULL */)

{
	
	// TODO: allow batch mode to support components in different directories.
	// currently only supports adding all batched components from default component gallery directory.
	BOOL bBatchMode = m_uigal.IsActive();
	
	// if components are being added in "batch mode", the user will have already enabled the gallery,
	// and set it to the directory where all the components are.
	if(!bBatchMode)
	{
		// open the gallery dlg.
		if(!m_uigal.Display())
		{
			LOG->RecordInfo("ERROR! COProject::AddComponent - could not display gallery dlg");
	 		return FALSE;
		}
		// set the directory to look in.
		if(!m_uigal.LookIn(szLocation ? szLocation : GetLocString(IDSS_COMPONENTS_DIR)))
		{
			LOG->RecordInfo("ERROR! COProject::AddComponent - could not change gallery 'look in'");
			m_uigal.Close();
			return FALSE;
		}
	}
	
	// select the component
	if(!m_uigal.SelectComponent((LPCSTR)(pComponent->m_strFileName)))
	{
		LOG->RecordInfo("ERROR! COProject::AddComponent - could not select component");
		m_uigal.Close();
		return FALSE;
	}

	// verify the component's description field
	if(!pComponent->m_strDescription.IsEmpty() &&  
		m_uigal.Description() != pComponent->m_strDescription)
	{
		LOG->RecordInfo("ERROR! COProject::AddComponent - unexpected component description");
		m_uigal.Close();
		return FALSE;
	}

	// insert the component
	if(!m_uigal.Insert((LPCSTR)(pComponent->m_strGalleryName)))
	{
		LOG->RecordInfo("ERROR! COProject::AddComponent - could not insert component");
		m_uigal.Close();
		return FALSE;
	}

	// do the component's wizard, if any.
	if(!(pComponent->WizardFunc()))
	{
		LOG->RecordInfo("ERROR! COProject::AddComponent - could not complete component wizard");
		m_uigal.Close();
		return FALSE;
	}

	// if components are being added in "batch mode", the user won't want the gallery closed.
	if(!bBatchMode)
	{
		// close the gallery dlg.
		if(!m_uigal.Close())
		{
			LOG->RecordInfo("ERROR! COProject::AddComponent - could not close gallery dlg");
			return FALSE;
		}
	}

	return TRUE;	
}


// BEGIN_HELP_COMMENT
// Function: BOOL COProject::EnableComponents(LPCSTR szLocation /* NULL */)
// Description: Opens all initial gallery/components dlgs/windows necessary to add components. This is for tests that add a lot of components consecutively (i.e. batch mode), and want to save time, for example, by not closing and reopening the gallery dlg everytime.
// Param: szLocation (optional) - The directory the components are in. It may be relative (to the Gallery directory) or absolute. If not supplied, the default components directory is used (ex. Developer Studio Components). 
// Return: BOOL - Returns TRUE if the components were sucessfully enabled (i.e. the gallery dlg was opened), FALSE if not.
// END_HELP_COMMENT
BOOL COProject::EnableComponents(LPCSTR szLocation /* NULL */)

{
	// display the component gallery.
	m_uigal.Display();

	// verify the dlg came up.
	if(!m_uigal.IsActive())
	{
		LOG->RecordInfo("ERROR! COProject::EnableComponents - could not open gallery dlg");
		return FALSE;
	}

	// set the directory to look in.
	if(!m_uigal.LookIn(szLocation ? szLocation : GetLocString(IDSS_COMPONENTS_DIR)))
	{
		LOG->RecordInfo("ERROR! COProject::EnableComponents - could not change gallery 'look in'");
		m_uigal.Close();
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COProject::DisableComponents(void)
// Description: Closes all initial gallery/components dlgs/windows necessary to add components. This is for tests that add a lot of components consecutively (i.e. batch mode), and want to save time, for example, by not closing and reopening the gallery dlg everytime.
// Return: BOOL - Returns TRUE if the components were sucessfully disabled (i.e. the gallery dlg was closed), FALSE if not.
// END_HELP_COMMENT
BOOL COProject::DisableComponents(void)
{
	// close the gallery dlg.
	m_uigal.Close();

	// verify it actually did close.
	if(m_uigal.IsActive())
	{
		LOG->RecordInfo("ERROR! COProject::DisableComponents - could not close gallery dlg");
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CProjWizOptions::CProjWizOptions(void)
// Description: Constructs a CProjWizOptions object. The default
// Return: none.
// END_HELP_COMMENT
CProjWizOptions::CProjWizOptions(void)
{
	m_strName = "";
	m_strLocation = "";
	m_pwWorkspace = CREATE_NEW_WORKSPACE;
	m_phHierarchy = TOP_LEVEL_PROJ;
	m_strParent = ""; 
	m_iPlatforms = GetUserTargetPlatforms();
}


// BEGIN_HELP_COMMENT
// Function: CControlWizOptions::CControlWizOptions(void)
// Description: Constructs a CControlWizOptions object.
// Return: none.
// END_HELP_COMMENT
CControlWizOptions::CControlWizOptions(void)
{
	// set the project type for UIProjectWizard in COProject::New()
	m_ptType = ACTIVEX_CONTROL;
	// set the activex control wizard defaults.
	m_iNumControls = 1;
	m_bRuntimeLicense = FALSE;
	m_bGenerateComments = TRUE;
	m_bGenerateHelpFiles = FALSE;
	m_bActivatesWhenVisible = TRUE;
	m_bInvisibleAtRuntime = FALSE;
	m_bAvailableInInsertObjectDlg = FALSE;
	m_bAboutBox = TRUE;
	m_bSimpleFrameControl = FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CControlWizOptions::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CControlWizOptions object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CControlWizOptions::WizardFunc(void)

{
	UICtrlWizard uictrlwiz;

 	// wait up to 3 seconds for page 1 of the wizard dlg to come up. 
	if(!MST.WFndWndWait(uictrlwiz.ExpectedTitle(1), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CControlWizOptions::WizardFunc - page 1 of wizard not found");
		return FALSE;
	}

	// attach to the wizard dlg.
	uictrlwiz.AttachActive();
	// go to page 2.
	uictrlwiz.NextPage();

 	// wait up to 3 seconds for page 2 of the wizard dlg to come up. 
	if(!MST.WFndWndWait(uictrlwiz.ExpectedTitle(2), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CControlWizOptions::WizardFunc - page 2 of wizard not found");
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CATLCOMWizOptions::CATLCOMWizOptions(void)
// Description: Constructs a CATLCOMWizOptions object.
// Return: none.
// END_HELP_COMMENT
CATLCOMWizOptions::CATLCOMWizOptions(void)
{
	// set the project type for UIProjectWizard in COProject::New()
	m_ptType = ATL_COM;
	// set the atl com wizard defaults.
	m_acstServerType = ATL_COM_SERVER_DLL;
	m_bAllowMergingOfProxyStubCode = FALSE;
	m_bSupportMFC = FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CATLCOMWizOptions::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CATLCOMWizOptions object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CATLCOMWizOptions::WizardFunc(void)

{
	// TODO(michma): this wizard needs its own class like the app wizard and control wizard.

 	// wait up to 3 seconds for page 1 of the wizard dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_PROJTYPE_ATL_COM_WIZ), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CATLCOMWizOptions::WizardFunc - page 1 of wizard not found");
		return FALSE;
	}

	// set the options for the wizard.
	char szOption[4];
	sprintf(szOption, "@%d", m_acstServerType); 
	MST.WOptionSelect(szOption);

	if(m_bAllowMergingOfProxyStubCode)
		MST.WCheckCheck("@1");
	else
		MST.WCheckUnCheck("@1");

	if(m_bSupportMFC)
		MST.WCheckCheck("@2");
	else
		MST.WCheckUnCheck("@2");

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CWin32AppWizOptions::CWin32AppWizOptions(void)
// Description: Constructs a CWin32AppWizOptions object.
// Return: none.
// END_HELP_COMMENT
CWin32AppWizOptions::CWin32AppWizOptions(void)
{
	// set the project type for UIProjectWizard in COProject::New()
	m_ptType = EXE;
	// set the win32 app wizard defaults.
	m_AppType = EMPTY_PROJECT;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CWin32AppWizOptions::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CWin32AppWizOptions object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CWin32AppWizOptions::WizardFunc(void)

{
	// TODO(michma): this wizard needs its own class like the app wizard and control wizard.

 	// wait up to 3 seconds for page 1 of the wizard dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_PROJTYPE_APPLICATION), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CWin32AppWizOptions::WizardFunc - page 1 of wizard not found");
		return FALSE;
	}

	// set the options for the wizard.
	char szOption[4];
	sprintf(szOption, "@%d", m_AppType); 
	MST.WOptionSelect(szOption);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CISAPIExtensionWizOptions::CISAPIExtensionWizOptions(void)
// Description: Constructs a CISAPIExtensionWizOptions object.
// Return: none.
// END_HELP_COMMENT
CISAPIExtensionWizOptions::CISAPIExtensionWizOptions(void)
{
	// set the project type for UIProjectWizard in COProject::New()
	m_ptType = ISAPI_EXTENSION;
	// set the isapi extension wizard defaults.
	m_bGenerateFilter = FALSE;
	m_bGenerateExtension = TRUE;
	m_MFCLinkage = LINK_AS_SHARED_DLL;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CISAPIExtensionWizOptions::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CISAPIExtensionWizOptions object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CISAPIExtensionWizOptions::WizardFunc(void)

{
	// TODO(michma): this wizard needs its own class like the app wizard and control wizard.

 	// wait up to 3 seconds for page 1 of the wizard dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_PROJTYPE_ISAPI_EXTENSION), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CISAPIExtensionWizOptions::WizardFunc - page 1 of wizard not found");
		return FALSE;
	}

	// set the options for the wizard.
	// TODO(michma - 11/21/97) need to support non-default class name and description fields.
	if(m_bGenerateFilter)
		MST.WCheckCheck("@1");
	else
		MST.WCheckUnCheck("@1");

	if(m_bGenerateExtension)
		MST.WCheckCheck("@2");
	else
		MST.WCheckUnCheck("@2");

	char szOption[4];
	sprintf(szOption, "@%d", m_MFCLinkage); 
	MST.WOptionSelect(szOption);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CWin32DllWizOptions::CWin32DllWizOptions(void)
// Description: Constructs a CWin32DllWizOptions object.
// Return: none.
// END_HELP_COMMENT
CWin32DllWizOptions::CWin32DllWizOptions(void)
{
	// set the project type for UIProjectWizard in COProject::New()
	m_ptType = DLL;
	// set the win32 dll wizard defaults.
	m_DllType = EMPTY_PROJECT;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CWin32DllWizOptions::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CWin32DllWizOptions object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CWin32DllWizOptions::WizardFunc(void)

{
	// TODO(michma): this wizard needs its own class like the app wizard and control wizard.

 	// wait up to 3 seconds for page 1 of the wizard dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_PROJTYPE_DLL), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! CWin32DllWizOptions::WizardFunc - page 1 of wizard not found");
		return FALSE;
	}

	// set the options for the wizard.
	char szOption[4];
	sprintf(szOption, "@%d", m_DllType); 
	MST.WOptionSelect(szOption);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: CXboxGameOptions::CXboxGameOptions(void)
// Description: Constructs a CXboxGameOptions object.
// Return: none.
// END_HELP_COMMENT
CXboxGameOptions::CXboxGameOptions(void)
{
	// set the project type for UIProjectWizard in COProject::New()
	m_ptType = XBOX_PROJECT;
}


// BEGIN_HELP_COMMENT
// Function: CXboxStaticLibOptions::CXboxStaticLibOptions(void)
// Description: Constructs a CXboxStaticLibOptions object.
// Return: none.
// END_HELP_COMMENT
CXboxStaticLibOptions::CXboxStaticLibOptions(void)
{
	// set the project type for UIProjectWizard in COProject::New()
	m_ptType = XBOX_LIB_PROJECT;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CXboxGameOptions::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CXboxGameOptions object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CXboxGameOptions::WizardFunc(void)
{
	// no options to set
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CXboxStaticLibOptions::WizardFunc(void)
// Description: Manipulates the component's wizard, using values stored by the CXboxStaticLibOptions object.
// Return: TRUE if the wizard was successful, FALSE if not.
// END_HELP_COMMENT
BOOL CXboxStaticLibOptions::WizardFunc(void)
{
	// no options to set
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: int COProject::New(CProjWizOptions *pProjWizOpt)
// Description: Creates a new project, including handling of the project-specific wizard. 
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: pProjWizOpt A pointer to a CProjWizOptions derived class that defines the project (ex. the type, the name, the location, project-specific options, etc.). See CProjWizOptions and derived types for more info (in coprject.h)
// END_HELP_COMMENT
int COProject::New(CProjWizOptions *pProjWizOpt)

{
	const char* const THIS_FUNCTION = "COProject::New";

	// bring up the new project dlg.
	UIProjectWizard uprjwiz = AppWizard();
	
	// verify it came up.
	if(!uprjwiz.HWnd())
	{
		LOG->RecordInfo("%s: could not bring up the new project dlg", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	// set the project type.
	uprjwiz.SetProjType(pProjWizOpt->m_ptType);

	// set the project location, if specified.
	if(pProjWizOpt->m_strLocation != "")
		uprjwiz.SetDir(pProjWizOpt->m_strLocation);

	// set the project name.
	uprjwiz.SetName(pProjWizOpt->m_strName);

	// set workspace/dependency option.
	if(pProjWizOpt->m_pwWorkspace == ADD_TO_CURRENT_WORKSPACE)
	{
		// add to current workspace.
		uprjwiz.SetAddToWorkspace(TRUE);
		// if there is a dependency, set it.
		if((pProjWizOpt->m_phHierarchy == SUB_PROJ) && (pProjWizOpt->m_strParent != ""))
			uprjwiz.SetTopLevelProj(pProjWizOpt->m_strParent);
	}
	else
		// create a new workspace.
		uprjwiz.SetAddToWorkspace(FALSE);

	// set the platforms.
	SetNewProjectPlatforms(pProjWizOpt->m_iPlatforms);

	// confirm the creation.
	if(uprjwiz.Create() == NULL)
	{
		LOG->RecordInfo("%s: could not create new project.", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	// do the project-specific wizard.
	if(!pProjWizOpt->WizardFunc())
	{
		LOG->RecordInfo("%s: failed to do wizard.", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	// update UIProjectWizard on what the current window handle is
	// (it will be the handle of the window to whichever page the project-specific wizard left off at).
	uprjwiz.AttachActive();
	// finish the wizard.
	// REVIEW (michma): in NewAppWiz() we check the page returned by Finish(), but only pages for
	// appwizard seem to be supported, not control wizard or atl wizard, etc. so this gets us by for now.
	uprjwiz.Finish();

	// confirm the new project information.
	if(!uprjwiz.ConfirmCreate())
	{
		LOG->RecordInfo("%s: problem generating project.", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	// give focus to workspace window.
    ActivateProjWnd();	
	// bring up the project window
	NewProjectWindow();
	// set up the member variables that contain information about this project.
	SetPathMembers();
	// this will ensure that the ide is done making menu changes, etc., for the new project.
	Sleep(1000);

	return ERROR_SUCCESS;
}


// BEGIN_HELP_COMMENT
// Function: int COProject::InsertNewATLObject(LPCSTR szObjectName)
// Description: Inserts a new ATL object into the current project. 
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szObjectName A pointer to a string specifying the name of the new object.
// END_HELP_COMMENT
int COProject::InsertNewATLObject(LPCSTR szObjectName)

{
	// TODO(michma): the ATL Object Wizard needs its own UI class with full functionality. 
	UIWB.DoCommand(IDM_NEW_ATL_OBJECT, DC_MNEMONIC);

 	// wait up to 3 seconds for the wizard dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_ATL_OBJ_WIZ_TITLE), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! COProject::InsertNewATLObject - ATL Object Wizard dlg not found");
		return ERROR_ERROR;
	}

	// simply select the default "Simple Object" for now.
	MST.DoKeys("{ENTER}");

 	// wait up to 3 seconds for the wizard properties dlg to come up. 
	if(!MST.WFndWndWait(GetLocString(IDSS_ATL_OBJ_WIZ_PROP_TITLE), FW_PART, 3))
	{
		LOG->RecordInfo("ERROR! COProject::InsertNewATLObject - ATL Object Wizard Properties dlg not found");
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
		LOG->RecordInfo("ERROR! COProject::InsertNewATLObject - ATL Object Wizard Properties dlg didn't go away");
		return ERROR_ERROR;
	}

	return ERROR_SUCCESS;
}


// BEGIN_HELP_COMMENT
// Function: LPCSTR COProject::GetOutputFile()
// Description: Get the output file from the Project Settings dialog
// Return: A char string that of what is contained in the Link Tab on the setting dialog. If the path is relative, it will build the full path for the file
// Param: <none>
// END_HELP_COMMENT
LPCSTR COProject::GetOutputFile()
{
	m_OutputFile = GetProjProperty(CT_LINKER, LNK_IDC_TARGET_NAME);

	// now lets see if the path is relative
	char drive[_MAX_DRIVE];   char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   char ext[_MAX_EXT];

	_splitpath(m_OutputFile, drive, dir, fname, ext);
	
	// Assumption: if drive is NULL, then this is a reference path
	if (strlen(drive) == 0)
	{
		// we need to append the drectory of the project to the path
		m_OutputFile = m_ProjDir + dir;
	}
	m_OutputFile.Replace('/','\\');
	m_OutputFile += fname;
	m_OutputFile += ext;
	
	return m_OutputFile.GetBuffer(0);
}

// emmang@xbox xbox-specific
// BEGIN_HELP_COMMENT
// Function: LPCSTR COProject::GetOutputXbeFile()
// Description: Get the output file from the Project Settings dialog
// Return: A char string that of what is contained in the Link Tab on the setting dialog. If the path is relative, it will build the full path for the file
// Param: <none>
// END_HELP_COMMENT
LPCSTR COProject::GetOutputXbeFile()
{
	m_OutputFile = GetProjProperty(CT_IMAGEBLD, IMAGEBLD_IDC_XBE_NAME);

	// now lets see if the path is relative
	char drive[_MAX_DRIVE];   char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   char ext[_MAX_EXT];

	_splitpath(m_OutputFile, drive, dir, fname, ext);
	
	// Assumption: if drive is NULL, then this is a reference path
	if (strlen(drive) == 0)
	{
		// we need to append the drectory of the project to the path
		m_OutputFile = m_ProjDir + dir;
	}
	m_OutputFile.Replace('/','\\');
	m_OutputFile += fname;
	m_OutputFile += ext;
	
	return m_OutputFile.GetBuffer(0);
}

// BEGIN_HELP_COMMENT
// Function: LPCSTR COProject::GetIntermediateDir()
// Description: Get the IntermediateDir from the Project Settings dialog on the General Tab
// Return: A char string that of what is contained in the Itermediate Files edit field. This a relative path to the project
// Param: <none>
// END_HELP_COMMENT
LPCSTR COProject::GetIntermediateDir()
{
	UIProjOptionsTabDlg uipo;
	uipo.AttachActive();

	//m_InterDir = uipo.GetGeneralOptionStr(GEN_IDC_INTER_DIR);
	m_InterDir = uipo.GetOptionStr(CT_GENERAL, GEN_IDC_INTER_DIR);
	
	return m_InterDir.GetBuffer(0);
}


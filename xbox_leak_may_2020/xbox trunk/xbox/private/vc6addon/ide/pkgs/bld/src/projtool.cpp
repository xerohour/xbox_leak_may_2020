//
//	PROJTOOL.CPP
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
					
#pragma hdrstop
#include "resource.h"
#include "progdlg.h"
#include "dirmgr.h"
#include "dlgbase.h"
#include "projcomp.h"
#include "targitem.h"
#include "toolsdlg.h"
#include "bldrfile.h"
#include "toolcplr.h"
#include "mrdepend.h"
#include "toolmtl.h"
#include "bldslob.h"

IMPLEMENT_DYNCREATE(CBuildTool, CBldSysCmp)
/*
R.I.P. for v4.0 with VBA?
IMPLEMENT_DYNAMIC(CCustomTool, CBuildTool)
*/
IMPLEMENT_DYNAMIC(CCustomBuildTool, CBuildTool)
IMPLEMENT_DYNAMIC(CUnknownTool, CBuildTool)
IMPLEMENT_DYNAMIC(COLEBuildTool, CBuildTool)
IMPLEMENT_DYNAMIC(CSpecialBuildTool, CBuildTool)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL ReplaceMsDevMacros( CString &str );
extern CProjComponentMgr g_prjcompmgr;
extern BOOL g_bUpdateDep;

///////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////
BOOL FileNameMatchesExtension(const CPath * pPath, const  TCHAR * pExtensions)
{
	if (pPath == (const CPath *)NULL)	return FALSE;

	const TCHAR * pExtNext = pExtensions;
	const TCHAR * pFileExt = pPath->GetExtension();
	if (*pFileExt) pFileExt++;
		// GetExtension() returns pointer to '.'

	int n = _tcslen (pFileExt);

	if (n)
	{
		while (*pExtNext != _T('\0'))

		{
			pExtNext = pExtensions;
			while (*pExtNext != _T(';') && *pExtNext != _T(',') && *pExtNext != _T('\0'))
				pExtNext = _tcsinc ((TCHAR *) pExtNext );

			// skip over the optional '*.'
			if (pExtensions[0] == _T('*') && pExtensions[1] == _T('.'))
				pExtensions += 2;
			else if (pExtensions[0] == _T('.'))
				pExtensions += 1;

			// compare extensions in case-insensitive fashion
			if ((n == (pExtNext - pExtensions)) &&			// check for ext. length equivalence
				_tcsnicmp (pFileExt, pExtensions, n) == 0)	// check for ext. string equivalence
				return TRUE;

			pExtensions = pExtNext + 1;	// skip over the sep.
		}
	}

	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
BOOL PtrListsAreEqual
(
	const CPtrList &l1,
	const CPtrList &l2,
	BOOL (*pfn) (DWORD, FileRegHandle) /*= NULL*/,
	DWORD dwFuncCookie /*= NULL */
)
{					
	int ncount = l1.GetCount ();
	POSITION pos;
	void *pv1, *pv2;

    if (ncount == 0 ) return TRUE;
	if ( (pfn ==0)
		  &&
		 (ncount != l2.GetCount ())
		 ) return FALSE; // No chance a filter can make them the same size!

	CMapPtrToPtr pm (ncount);

	for (pos = l1.GetHeadPosition (); pos != NULL; )
	{
		pv1 = l1.GetNext (pos);

		// Since I happen to know we'll only use this for file lists, we'll
		// take an extra chance to make sure there are no duplicates in the
		// list:
		ASSERT (!pm.Lookup (pv1, pv2));
		pm.SetAt ( pv1, (void *) 0);
	}

	// Now try to remove the items from list 2, one by one:
	for (pos = l2.GetHeadPosition (); pos != NULL; )
	{
		pv1 = l2.GetNext (pos);
		if (pfn && ! (*pfn) (dwFuncCookie, (FileRegHandle) pv1)) continue;
		if (pm.Lookup (pv1, pv2)) pm.RemoveKey ( pv1 );
		else return FALSE;
	}

	// Should be nothing left:
	return ( pm.IsEmpty () ) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Filter function for writing the CFileItem Targets to exclude certain outputs
// like .pdb and .idb so that debug and release configs are folded properly
BOOL FilterFileItemTargets ( DWORD dw, FileRegHandle frh)
{
	CBuildTool * pTool = (CBuildTool *)dw;
	ASSERT(pTool->IsKindOf(RUNTIME_CLASS(CBuildTool)));
	ASSERT(!pTool->IsKindOf(RUNTIME_CLASS(CSchmoozeTool)));

	CPath * pPath = (CPath *)g_FileRegistry.GetRegEntry(frh)->GetFilePath();
	ASSERT(pPath);
	ASSERT(pPath->IsInit());
	
#ifdef _DEBUG
	// doesn't ever try to filter custom build tools
	if (pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
		ASSERT(pTool->IsProductFile(pPath));
#endif

	// filter based on pTool type
	return (pTool->IsProductFile(pPath));
}

///////////////////////////////////////////////////////////////////////////////
// Get output file name for a projitem based on options pairs like /Fm and
// map file name, accounting for output directories.
//
// Basic plan is to check the generate flag (if present) and then get the name
// from the item.  If the returned name ends in a back slash (or forward slash)
// then its acutally an output directory, relative the the _project_ directory,
// since this is what the compiler & linker do.  If the output directory starts
// in a backslash or drive letter combo then its an absolute name.

// To get the filename (if needed), we take the item's file name and change
// extension to the supplied extension.
BOOL GetItemOutputFile
(
	CProjItem * pItem,
	UINT nFileNameID,
	FileRegHandle & frh,
	const TCHAR * pcDefExtension,
	const CDir * pBaseDir,
	UINT nEnablePropId /*= -1*/
)
{
	ASSERT (pBaseDir);

	BOOL b;
	CString strFileName;
	CPath tp;
	frh = NULL;
	const TCHAR *pc;
	int len;
	BOOL bChangeEx = FALSE;

	// First check the enabling prop
	if (nEnablePropId != -1)
		if ((pItem->GetIntProp(nEnablePropId, b) != valid) || !b)
			return FALSE;

	// Output filename? 
	if ((pItem->GetStrProp(nFileNameID, strFileName) == valid) &&
		!strFileName.IsEmpty())
	{
		// Try to substitute for $(INTDIR)
		if (!pItem->SubstOutDirMacro(strFileName, pItem->GetActiveConfig()))
		{
			// Try $(OUTDIR) if $(INTDIR) doesn't work
			pItem->GetProject()->SubstOutDirMacro(strFileName, pItem->GetProject()->GetActiveConfig());
		}

		// Okay, the item has an apperently non empty-name entry.  See if its
		// a directory by checking last character.  If it is,
		len = strFileName.GetLength();
		pc = (const TCHAR *) strFileName;
		pc = _tcsdec(pc, (TCHAR *)(pc+len));
	
		if ( *pc == _T('/') || *pc == _T('\\') )
		{
			// just append and change extension later
			strFileName += pItem->GetFilePath()->GetFileName();
			bChangeEx = TRUE;
		}
		if (tp.CreateFromDirAndFilename(*pBaseDir, strFileName))
		{
			if (bChangeEx)
				tp.ChangeExtension (pcDefExtension);
#ifndef REFCOUNT_WORK
			if ((frh = pItem->GetTarget()->GetRegistry()->RegisterFile(&tp)) != NULL)
#else
			if ((frh = CFileRegFile::GetFileHandle(tp)) != NULL)
#endif
				return TRUE;
		}

		return FALSE;
	}

	// No filename or is empty filename
	ASSERT(pItem->GetFilePath());
	if (tp.CreateFromDirAndFilename(*pBaseDir, pItem->GetFilePath()->GetFileName ()))
	{
		tp.ChangeExtension (pcDefExtension);
#ifndef REFCOUNT_WORK
		if ((frh = pItem->GetTarget()->GetRegistry()->RegisterFile(&tp)) != NULL)
#else
		if ((frh = CFileRegFile::GetFileHandle(tp)) != NULL)
#endif
			return TRUE;
	}

	return FALSE;
}

// returns TRUE if this command is a batch file in a subproject dir (in which
// case we fixed strCmd)
BOOL FAdjustPathForBatchFile(CString &strCmd)
{
	LPCTSTR pch = _tcsstr(strCmd, ".bat");

	// quick check
	if (pch == NULL)
		return FALSE;

	CDir dirProj;
	CDir dirWksp;
	dirProj = g_pActiveProject->GetProjDir();
	dirWksp = g_pActiveProject->GetWorkspaceDir();
	if (_tcsicmp(dirProj, dirWksp) == 0)
		// same dir - no problem
		return FALSE;

	// It looks like we've got a batch file command in a subdirectory
	// Make sure the subdir is the subproj dir.

	CString strProjName;
	CPath path;
	int  cQuotes = 0 ; // How many quotes do we have at the begining of the command.

	if (!path.Create((LPCTSTR)dirProj))
	{	
		ASSERT(FALSE);
		return FALSE;
	}

	if (!path.GetRelativeName(dirWksp, strProjName, TRUE, FALSE))
		return FALSE;

	// lose the ".\" prefix
	strProjName.TrimLeft();
	if (_tcsncmp(strProjName, ".\\", 2) == 0)
	{
		strProjName = strProjName.Mid(2);
	}

	pch = strCmd;

	while ( *pch == '\"' )
	{
		pch++;
		cQuotes++;
	}

	while (*pch != NULL && (*pch == '.' || *pch == '\\'))
		pch++;

	int cchProjName = strProjName.GetLength();
	if (_tcsnicmp(pch, strProjName, cchProjName) == 0 && pch[cchProjName] == '\\')
	{
		// Finally, we've decided that the subdir in the command matches
		// the project directory.  Remove the subdir name from the command
		// so we can spawn the batch file in the subproj dir.
		CString str;

		while (cQuotes--) 
			str += "\"";

		str += (pch + cchProjName + 1);
		strCmd = str;
		return TRUE;
	}

	return FALSE;
}

enum SpecialBuildToolType
{
	InvalidSpecialBuildTool = -1,
	PreLinkDescId = 0,
	PreLinkCmdsId,
	PostBuildDescId,
	PostBuildCmdsId,
	MaxSpecialBuildToolId
};
const TCHAR* s_ppszSpecialBuildToolMacroName[] = 
{	_T("PreLink_Desc"),
	_T("PreLink_Cmds"),
	_T("PostBuild_Desc"),
	_T("PostBuild_Cmds"),
};

const int s_pSpecialBuildToolProp[] =
{
	P_PreLink_Description,
	P_PreLink_Command,
	P_PostBuild_Description,
	P_PostBuild_Command,
};

static BOOL IsSpecialBuildTool( const CString& strName, int& idtype )
{
	idtype = 0;

	while (idtype < MaxSpecialBuildToolId)
	{
		if ( 0 == strName.CompareNoCase(s_ppszSpecialBuildToolMacroName[idtype]) )
		{
			return TRUE;
		}

		idtype++;
	}
	idtype = InvalidSpecialBuildTool;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// -------------------------------- CBuildTool --------------------------------
//
///////////////////////////////////////////////////////////////////////////////

#define theClass CBuildTool
BEGIN_SLOBPROP_MAP(CBuildTool, CBldSysCmp)
	STR_PROP(ToolExeName)
	STR_PROP(ToolPrefix)
	STR_PROP(ToolInput)
	STR_PROP(ToolOutput)
	STR_PROP(ToolMenuText)
	BOOL_PROP(ToolPerProject)
	BOOL_PROP(ToolInternal)
END_SLOBPROP_MAP()
#undef theClass

#define theClass CBuildTool
BEGIN_REGISTRY_MAP(CBuildTool, CBldSysCmp)	
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLEXE, NO_SUB_KEY, "Executable Path", NO_SUB_KEY, REG_STRING, m_strToolExeName)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLPREFIX, NO_SUB_KEY, "Macro_Prefix", NO_SUB_KEY, REG_STRING, m_strToolPrefix)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLINPUT, NO_SUB_KEY, "Input_Spec", NO_SUB_KEY, REG_STRING, m_strToolInput)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLOUTPUT, NO_SUB_KEY, "Output_Spec", NO_SUB_KEY, REG_STRING, m_strToolOutput)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLMENU, NO_SUB_KEY, "Menu Text", NO_SUB_KEY, REG_STRING, m_strToolMenuText)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLINTERNAL, NO_SUB_KEY, "Internal", NO_SUB_KEY,REG_BOOL, m_bToolInternal)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLPERFIFABLE, NO_SUB_KEY, "Perform_If_Able", NO_SUB_KEY,REG_BOOL, m_fPerformIfAble)
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLDEFERRED, NO_SUB_KEY, "Deferred", NO_SUB_KEY, REG_BOOL, m_fDeferredTool)
#ifdef _DEBUG
	REGISTRY_DATA_ENTRY(IDS_REG_TOOLDBG, NO_SUB_KEY, "Debugging", NO_SUB_KEY, REG_DWORD, m_dwDebugging)
#endif // _DEBUG
END_REGISTRY_MAP()
#undef theClass

int CBuildTool::m_nMyMarker = 0;

CBuildTool::CBuildTool(DWORD dwDbg)
{
	m_strToolExeName = _TEXT("");
	m_strDescription = _TEXT("");
	m_strToolInput = _TEXT("");
	m_strToolPrefix = _TEXT("");
	m_bToolPerProject = FALSE;
	m_bToolInternal = TRUE;
	m_fPerformIfAble = m_fDeferredTool = FALSE;
	m_popthdlr = (COptionHandler *)NULL;
	m_fTargetTool = FALSE;
	m_MenuID = m_nIDName = 0;
	m_wBuildFileMark = 0;

	m_nIDName = m_nIDUIName = 0;
	m_nOrder = 0;

	// FUTURE: clean-this up (from old CSourceTool)
	bForceNoInferenceRule = FALSE;

#ifdef _DEBUG
	m_dwDebugging = dwDbg;
#else // _DEBUG
	m_dwDebugging = 0;	// off
#endif // !_DEBUG
}

CBuildTool::~CBuildTool()
{
}

void CBuildTool::FInit()
{
	// do we have a name?
	if (m_nIDName)
		VERIFY(m_strCompName.LoadString(m_nIDName));
		
	// do we have a UI name?
	if (m_nIDUIName)
		VERIFY(m_strUICompName.LoadString(m_nIDUIName));
#if _DEBUG
	else
		m_strUICompName = m_strCompName;
#endif

	// FUTURE: figure this out properly based on input and
	// output file sets, for now use CSchmoozeTool until
	// we delete this class
	m_fTargetTool = IsKindOf(RUNTIME_CLASS(CSchmoozeTool));
}

BOOL ReplaceEnvVars(CString &string)
{
	// replace $(ENV_VARIABLES)
	int i,j;
	char value[256];
	CString newString;

	i = string.Find( "$(" );
	while( i != -1 ){
		// copy the characters up to the $( into the new string.
		newString += string.Left( i );

		// skip the $( 
		CString temp;
		temp = string.Right( string.GetLength() - (i + 2) );

		// find the variable for lookup
		j = temp.Find( ")" );
		if( j != -1 ) {
			CString token;
			token = temp.Left( j );
			if( GetEnvironmentVariable( token, value, 255 ) ){
				newString += value;
			} else {
				PBUILD_TRACE("warning: unresolved Environment Variable  \"$(%s)\"\n", (const char *)token );	
				CBuildIt::m_mapMissingEnvVars.SetAt(token, NULL);
			}
			// skip past the env variable and search for the next.
			string = string.Right( string.GetLength() - (i+j+3) );
			i = string.Find( "$(" );
		} else {
			// open without close !!! kill the end of the command line
			// this will likely cause a build failure
			PBUILD_TRACE("Warning : Open $( without close ) \"%s\"\n", (const char *)string );
			string = "";
			return FALSE;
		}
	}
	newString += string;
	string = newString;
	return TRUE;
}

void CBuildTool::ReplaceEnvVars(CString &string){
	::ReplaceEnvVars(string);
}

// default tool options for the tool for the target type component, 'bsc_id', or
// for any target type with attributes
BOOL CBuildTool::GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption)
{
	return FALSE;	// didn't retrieve default options
}

const int CBuildTool::nMaxCommandLineLength = 127;  // play it safe for Chicago

// Set the property P_SCanDeps for each of the files in each of the actions
// (errrors should be pushed into the error context EC)
BOOL CBuildTool::GenerateDependencies(CActionSlob * pAction, FileRegHandle frh, CErrorContext & EC)
{
	return TRUE;	// ok
}

// Scan a file item and fill out its depenency list.
// Note this is not virtual, the tool specific behaviour
// is implemented in the 'GenerateDependencies()' method.
BOOL CBuildTool::ScanDependencies(CProjItem * pItem, BOOL bWriteOutput /* = TRUE */)
{
	ConfigurationRecord * pcr = pItem->GetActiveConfig();
    if (pcr == (ConfigurationRecord *)NULL)
		return FALSE;

	CActionSlobList * pActions = pcr->GetActionList();
	if (pActions->IsEmpty())
		return FALSE;

	// FUTURE: handle more than one tool on this source file
	ASSERT(pActions->GetCount() == 1);

	CActionSlob * pAction = (CActionSlob *)pActions->GetHead();

	POSITION pos, posEntryList;

	CPtrList lstDeps;

	// list of FileRegHandles for files to be examined. Entries in this
	// list have there own references:
	CPtrList lToBeScanned; 	
	CIncludeEntry *pIncludeEntry;
	FileRegHandle hndFileReg, hndIncludeEntryItem = NULL;
	BOOL bDirty = FALSE;
	
	// remember previously scanned deps.
	// and also any old ones (so we can compare diffs)
	CMapPtrToPtr depsPrevious;

	// construct the map of current deps
	FileRegHandle frh;
	CFileRegSet * pfrs;
	
	pfrs = pAction->GetScannedDep();
	pfrs->InitFrhEnum();
	while ((frh = pfrs->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
		depsPrevious.SetAt(frh, (void*)DEP_Scanned);
#else
	{
		depsPrevious.SetAt(frh, (void*)DEP_Scanned);
		frh->ReleaseFRHRef();
	}
#endif

	pfrs = pAction->GetMissingDep();
	pfrs->InitFrhEnum();
	while ((frh = pfrs->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
		depsPrevious.SetAt(frh, (void*)DEP_Missing);
#else
	{
		depsPrevious.SetAt(frh, (void*)DEP_Missing);
		frh->ReleaseFRHRef();
	}
#endif

	// This marker is incremented once every call to this function. 
	// It is used to prevent processing of the an include file that is seen
	// multiple times when scanning a particular compiland(.cpp). 
	m_nMyMarker++;

	// get the include path for this item
	CString strIncludePath;
	GetIncludePaths(pItem, strIncludePath);

	// should temporarily change to the project directory so we
	// could scan appropriately (since relative SEARCH path should be from the project level)
	CCurDir currentDir(pItem->GetProject()->GetWorkspaceDir());

	// file's registry handle  
	FileRegHandle hndItemFile;
	VERIFY((hndItemFile = pItem->GetFileRegHandle()) != 0);

	// directory of this file to be scanned
	CDir dirFile; dirFile.CreateFromPath(*g_FileRegistry.GetRegEntry(hndItemFile)->GetFilePath());
	
	// add our file name to the scan list to start the process:
	BOOL fDontScanItem = FALSE;
	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
#if 0
		//
		// review(tomse) : I don't know why P_ItemExcludedFromBuild wasn't checked
		// for cpp/cxx/c files but this causes template files with bogus include's to
		// be scanned.
		//

		// for non-C files
		TCHAR * pch = _tcschr((LPCTSTR)(*g_FileRegistry.GetRegEntry(hndItemFile)->GetFilePath()), '.');
		if (pch && (_tcsicmp(pch, ".cpp")!=0 && _tcsicmp(pch, ".cxx")!=0 && _tcsicmp(pch, ".c")!=0 ))
#endif
			VERIFY(((CFileItem*)pItem)->GetIntProp(P_ItemExcludedFromBuild, fDontScanItem) == valid);
	}
	else
		fDontScanItem = FALSE;

	if (!fDontScanItem)
		lToBeScanned.AddTail((void *) hndItemFile);

	// inform the tool that we are about to scan the action's file
	OnActionEvent(ACTEVT_PreGenDep, pAction);

	while (!lToBeScanned.IsEmpty())	
	{
		hndFileReg = (FileRegHandle) lToBeScanned.GetHead();
		
		CIncludeEntryList * pEntryList = g_ScannerCache.LookupFile(hndFileReg);
		if (!pEntryList)
		{
			// Allow cancel from update dependency dialog.
			if (g_bUpdateDep && GetAsyncKeyState(VK_ESCAPE))
				break;	// user aborts
								   
   			lToBeScanned.RemoveHead();	// scanned this file

			// FUTURE: remove hack init.
			pAction->SetDepState(DEP_Empty);

			if (hndFileReg == hndItemFile)
			{
				// if we are scanning the source file, 
				// need to clear any of the action data Fortran uses
				OnActionEvent(ACTEVT_ClearData, pAction);
			}

			if (!GenerateDependencies(pAction, hndFileReg, g_DummyEC /* FUTURE: put proper one here */))
			{
				if (hndFileReg == hndItemFile)
					break;	// failure
				else
					continue;
			}

  			pEntryList = pAction->GetIncludeDep();

			// FUTURE: remove hack init.
			// do we need an 'dummy' list?
			if (pEntryList == DEP_Empty)
				pEntryList = new CIncludeEntryList(5);

  			g_ScannerCache.Add(hndFileReg, pEntryList);

			if (pEntryList == DEP_None)
			{
#ifndef REFCOUNT_WORK
				g_FileRegistry.ReleaseRegRef(hndFileReg);
#endif
				continue;
			}

			// Go through an mark all the include entries with the path of the
			// file we scanned, so we can use it to check later
			CDir dir;
	 		const CPath * pPath = g_FileRegistry.GetRegEntry(hndFileReg)->GetFilePath();
			if (dir.CreateFromPath(*pPath))
			{
				for (pos = pEntryList->GetHeadPosition (); pos != NULL ; )
				{
					CIncludeEntry * pEntry = (CIncludeEntry *) pEntryList->GetNext(pos);
					pEntry->m_strReferenceIn = *pPath;
					if (pEntry->m_EntryType & IncTypeCheckOriginalDir)
					{
						pEntry->m_OriginalDir = dir;
					}
		  		}
		  	}
			else
			{
				for (pos = pEntryList->GetHeadPosition (); pos != NULL ; )
				{
					CIncludeEntry * pEntry = (CIncludeEntry *) pEntryList->GetNext(pos);
					pEntry->m_strReferenceIn = *pPath;
		  		}
		  	}

			// Certainly haven't seen this file, so add it to dep. list,
			// unless its the initial path:
			if (hndFileReg != hndItemFile)
			{
				// check to see if this dependency was here before
				UINT depType;
				// If we know we have to change the dependencies anyway (bDirty = TRUE)  
				// avoid doing these costly lookups
				if (!bDirty && depsPrevious.Lookup((void *)hndFileReg, (void *&)depType))
				{
					// may be dirty if the dep. is no longer same type
					// ie. missing or scanned
					if (hndFileReg->ExistsOnDisk())
					{
						if (depType == DEP_Missing)
							bDirty = TRUE;
					}
					else if (hndIncludeEntryItem != NULL)
					{
						CFileRegEntry * preg = g_FileRegistry.GetRegEntry(hndIncludeEntryItem);
						CPath * pPath = (CPath *)preg->GetFilePath();
						if (pPath && !pPath->ExistsOnDisk())
						{
							if (depType == DEP_Scanned)	
								bDirty = TRUE;
						}
					}

					// move it to the current dep list
					VERIFY(depsPrevious.RemoveKey((void *)hndFileReg));

				}
				else
				{
					bDirty = TRUE;
				}

				lstDeps.AddTail((void *)hndFileReg);
#ifdef REFCOUNT_WORK
				hndFileReg->AddFRHRef();
#endif
			}						
			
			//  Mark list as seen by this file:
			pEntryList->m_LastTouch = m_nMyMarker;
		}
		else if (pEntryList == DEP_None)
		{
		    // this is a file that should not be listed
			lToBeScanned.RemoveHead();
#ifndef REFCOUNT_WORK
			g_FileRegistry.ReleaseRegRef(hndFileReg);
#endif
			continue;	
		}
		else
		{
			lToBeScanned.RemoveHead();
			if (pEntryList->m_LastTouch == m_nMyMarker)
			{
				continue;				// Already went through this file.
			}
			else						// Add to list;
			{
				pEntryList->m_LastTouch = m_nMyMarker;
#ifndef REFCOUNT_WORK
				g_FileRegistry.AddRegRef(hndFileReg); // ref in different file
#endif
				if (hndFileReg != hndItemFile)
				{
					// check to see if this dependency was here before
					UINT depType;
					// If we know we have to change the dependencies anyway (bDirty = TRUE)  
					// avoid doing these costly lookups
					if (!bDirty && depsPrevious.Lookup((void *)hndFileReg, (void *&)depType))
					{
						// may be dirty if the dep. is no longer same type
						// ie. missing or scanned
						if (hndFileReg->ExistsOnDisk())
						{
							if (depType == DEP_Missing)
								bDirty = TRUE;
						}
						else if (hndIncludeEntryItem != NULL)
						{
							CFileRegEntry * preg = g_FileRegistry.GetRegEntry(hndIncludeEntryItem);
							CPath * pPath = (CPath *)preg->GetFilePath();
							if (pPath && !pPath->ExistsOnDisk())
							{
								if (depType == DEP_Scanned)	
									bDirty = TRUE;
							}
						}

						// move it to the current dep list
						VERIFY(depsPrevious.RemoveKey((void *)hndFileReg));

					}
					else
					{
						bDirty = TRUE;
					}

					// lstDeps takes the existing ref.
					lstDeps.AddTail(hndFileReg);
#ifdef REFCOUNT_WORK
					hndFileReg->AddFRHRef();
#endif
				}
			}
		}

		//	Loop through all the files references by this file, and get the
		//	absolute path name from the reference.  Add this name to our
		//	dependencies list and to the to be scanned list if we haven't
		//	touched it before.
		posEntryList=pEntryList->GetHeadPosition();
		while (posEntryList != (POSITION)NULL)		
		{
			pIncludeEntry = (CIncludeEntry *)pEntryList->GetAt(posEntryList);

			if (!pIncludeEntry->FindFile
									((const TCHAR *)strIncludePath,
									 dirFile,
									 (const TCHAR *)(pItem->GetProject()->GetWorkspaceDir()),
									 hndIncludeEntryItem, 
									 pItem))
			{
				if (bWriteOutput)
				{
					// can't find dependency file so give an error message			
					CString str, strFormat;
					VERIFY( strFormat.LoadString(IDS_INCLUDE_NOT_FOUND));

					sprintf( (LPTSTR) str.GetBuffer((MAX_PATH+1) * 2 + 20 + strFormat.GetLength()), (LPCTSTR) strFormat,
						(LPCTSTR) pIncludeEntry->m_strReferenceIn,
						pIncludeEntry->m_nLineNumber,
						(LPCTSTR) pIncludeEntry->m_FileReference);
					str.ReleaseBuffer();

					if (theApp.m_bInvokedCommandLine)
					{
						theApp.WriteLog(str, TRUE);
					}
					else
					{
						g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow( str, TRUE, TRUE );
						g_VPROJIdeInterface.GetOutputWindow()->ShowOutputWindow();
					}
				}

				if (hndIncludeEntryItem == (FileRegHandle)NULL)
				{
					pIncludeEntry = (CIncludeEntry *)pEntryList->GetNext(posEntryList);
					continue;
				}
			}

			// Try to keep relative paths relative
			if (pIncludeEntry->m_FileReference[0] == _T('.'))
				g_FileRegistry.GetRegEntry(hndIncludeEntryItem)->SetRelative(TRUE);

			// If this is not a scannble entry, then just check to see if its
			// already a dependendent.  If its not, then add to dependencies
			// list:
			if (!pIncludeEntry->m_bShouldBeScanned)
			{
				for (pos = lstDeps.GetHeadPosition (); pos != NULL; )
				{
					hndFileReg = (FileRegHandle) lstDeps.GetAt(pos);
					if (hndFileReg == hndIncludeEntryItem)
						break;

					lstDeps.GetNext (pos);
				}

				// check to see if this dependency was here before
				UINT depType;
				if (!bDirty && depsPrevious.Lookup((void *)hndIncludeEntryItem, (void *&)depType))
				{
					// may be dirty if the dep. is no longer same type
					// ie. missing or scanned
					if (hndIncludeEntryItem->ExistsOnDisk())
					{
						if (depType == DEP_Missing)
							bDirty = TRUE;
					}
					else
					{
						CFileRegEntry * preg = g_FileRegistry.GetRegEntry(hndIncludeEntryItem);
						CPath * pPath = (CPath *)preg->GetFilePath();
						if (pPath && !pPath->ExistsOnDisk())
						{
							if (depType == DEP_Scanned)	
								bDirty = TRUE;
						}
					}

					// move it to the current dep list
					VERIFY(depsPrevious.RemoveKey((void *)hndIncludeEntryItem));
					lstDeps.AddTail((void *)hndIncludeEntryItem);
#ifdef REFCOUNT_WORK
					hndIncludeEntryItem->AddFRHRef();
#endif
				}
 				else if (pos == (POSITION)NULL && hndFileReg != hndIncludeEntryItem)
				{
					// move it to the current dep list
					lstDeps.AddTail((void *)hndIncludeEntryItem);
#ifdef REFCOUNT_WORK
					hndIncludeEntryItem->AddFRHRef();
#endif
					bDirty = TRUE;
				}
			}
			else
			{
				lToBeScanned.AddTail((void *) hndIncludeEntryItem);
			}

			pEntryList->GetNext(posEntryList);

#ifdef REFCOUNT_WORK
			hndIncludeEntryItem->ReleaseFRHRef();
#endif
		} 	// End of loop through pEntryList.

		if (posEntryList !=NULL) break; 	// Bailed from previous loop.
	
	}	// End of loop through lToBeScanned.

	// inform the tool that we are about to scan the action's file
	OnActionEvent(ACTEVT_PostGenDep, pAction, (DWORD)&lstDeps);

	//
	//	We know if we aborted because there are still files to be scanned:
	//
	if (!lToBeScanned.IsEmpty())
#ifndef REFCOUNT_WORK
		return FALSE;
#else
	{
		POSITION pos = lstDeps.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			FileRegHandle frh = (FileRegHandle)lstDeps.GetNext(pos);
			frh->ReleaseFRHRef();
		}
		return FALSE;
	}
#endif

	// if we left entries in the depsPrevious map, free them
	pos = depsPrevious.GetStartPosition();

	if (pos != NULL)
	{
		bDirty = TRUE;
	}

	// if we dirtied the dependency list, dirty the project
	if (bDirty)
	{
		// clear the action's dependencies list
		pAction->ClearScannedDep();
		pAction->ClearMissingDep();

		// add new ones
		POSITION pos = lstDeps.GetHeadPosition();
		while (pos != (POSITION)NULL)
#ifndef REFCOUNT_WORK
			pAction->AddScannedDep((FileRegHandle)lstDeps.GetNext(pos));
#else
		{
			FileRegHandle frh = (FileRegHandle)lstDeps.GetNext(pos);
			pAction->AddScannedDep(frh);
			frh->ReleaseFRHRef();
		}
#endif

		// Dirty the project object
//		pItem->GetProject()->DirtyProject();
	}
#ifdef REFCOUNT_WORK
	else
	{
		POSITION pos = lstDeps.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			FileRegHandle frh = (FileRegHandle)lstDeps.GetNext(pos);
			frh->ReleaseFRHRef();
		}
	}
#endif
	pAction->UpdateDepInfoTimeStamp();

	return TRUE;
}

//Automatic update dependencies. The list of dep comes from the background parser
BOOL CBuildTool::UpdateDependencies(CProjItem * pItem, CPtrList *pdepList)
{
	// Should not reach this code when using new model.
	ASSERT(!g_bNewUpdateDepModel);

#if 0	// Unused code.

	ConfigurationRecord * pcr = pItem->GetActiveConfig();
    if (pcr == (ConfigurationRecord *)NULL)
		return FALSE;

	CActionSlobList * pActions = pcr->GetActionList();
	if (pActions->IsEmpty())
		return FALSE;

	ASSERT(pActions->GetCount() == 1);

	CActionSlob * pAction = (CActionSlob *)pActions->GetHead();

	// Get the current deps
	CFileRegSet * pOriginal = pAction->GetScannedDep();
#ifndef REFCOUNT_WORK
	const CPtrList * pOriginalDeps = pOriginal->GetContent();
#endif

	// Check to see if the current deps are the same as the new scanned deps
	CPtrList lstAdd;
	CPtrList lstSub;
	CMapPtrToPtr mapNew;
	BOOL bDirty = FALSE;

	// get the number of differences so tha we can short circuit
	// this thing if there are no changes or we are just adding.
#ifndef REFCOUNT_WORK
	int nDiff =  pdepList->GetCount() - pOriginalDeps->GetCount();
#else
	int nDiff =  pdepList->GetCount() - pOriginal->GetCount();
#endif
	int nLast = 0;

	// Record and clear missing deps
	CFileRegSet *pfrs = pAction->GetMissingDep(FALSE);
	CMapPtrToPtr *pMissing = NULL;
	if( pfrs ){
#ifndef REFCOUNT_WORK
		const CPtrList *plst = pfrs->GetContent();
		nLast = plst->GetCount();
		if( nLast ){
			pMissing = new CMapPtrToPtr( nLast );
			pMissing->InitHashTable(37);
			POSITION pos;
			pos = plst->GetHeadPosition();
			while( pos != NULL ){
				pMissing->SetAt(plst->GetNext(pos),NULL);
			}
#else
		nLast = pfrs->GetCount();
		if( nLast ){
			pMissing = new CMapPtrToPtr( nLast );
			pMissing->InitHashTable(37);
			pfrs->InitFrhEnum();
			FileRegHandle frh = pfrs->NextFrh();
			while( frh != NULL ){
				pMissing->SetAt(frh,NULL);
				frh->ReleaseFRHRef();
				frh = pfrs->NextFrh();
			}
#endif
		}

		// now we have a copy do clear deps.
		pAction->ClearMissingDep();
	}

	// Check for new deps to add
	POSITION pos = pdepList->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		FileRegHandle frh = (FileRegHandle)pdepList->GetNext(pos);
		if (!pOriginal->RegHandleExists(frh))
		{
			lstAdd.AddTail(frh);
		}
		mapNew.SetAt(frh,NULL);
	}

	// have we got all the changes ?
	if( nDiff != lstAdd.GetCount() ) {
		// check for old deps to remove
		void *pDummy;
#ifndef REFCOUNT_WORK
		pos = pOriginalDeps->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			// Does this dep exist in the original deps
			FileRegHandle frh = (FileRegHandle)pOriginalDeps->GetNext(pos);
			if (!mapNew.Lookup(frh, pDummy ))
			{
				lstSub.AddTail(frh);
			}
		}
#else
		pOriginal->InitFrhEnum();
		FileRegHandle frh = pOriginal->NextFrh();
		while (frh != NULL)
		{
			// Does this dep exist in the original deps
			if (!mapNew.Lookup(frh, pDummy ))
			{
				lstSub.AddTail(frh);
			}
			frh->ReleaseFRHRef();
			frh = pOriginal->NextFrh();
		}
#endif
	}

	// replace with the new ones we got from the parser
	pos = lstSub.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		FileRegHandle frh = (FileRegHandle)lstSub.GetNext(pos);
		if (frh != NULL)
			pAction->SubScannedDep(frh);
	}

	pos = lstAdd.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		FileRegHandle frh = (FileRegHandle)lstAdd.GetNext(pos);
		if (frh != NULL)
			pAction->AddScannedDep(frh);
	}

	// if anything changed dirty the project then Dirty the project object
	if( !lstSub.IsEmpty() ){
		bDirty = TRUE;
	}
	else {
		pfrs = pAction->GetMissingDep(FALSE);
		if( !pfrs ) 
		{
			if (lstAdd.GetCount( ) != 0 || nLast != 0)
				bDirty = TRUE;
		}
		else
		{
#ifndef REFCOUNT_WORK
			int nMiss = pfrs->GetContent()->GetCount();
	
			// potentially dirty.
			if( nMiss == lstAdd.GetCount() && nMiss == nLast ){
				POSITION pos;
				void 	*ptr;
				const CPtrList *plst = pfrs->GetContent();
				pos = plst->GetHeadPosition();
				while( pos != NULL ){
					if( !pMissing->Lookup(plst->GetNext(pos),ptr) ){
						bDirty = TRUE;
						break;
					}
				}
#else
			int nMiss = pfrs->GetCount();
	
			// potentially dirty.
			if( nMiss == lstAdd.GetCount() && nMiss == nLast ){
				void 	*ptr;
				pfrs->InitFrhEnum();
				FileRegHandle frh = pfrs->NextFrh();
				while( frh != NULL ){
					if( !pMissing->Lookup(frh,ptr) ){
						bDirty = TRUE;
						frh->ReleaseFRHRef();
						break;
					}
					frh->ReleaseFRHRef();
					frh = pfrs->NextFrh();
				}
#endif
			} else {
				// we added more than we are missing so something changed
				bDirty = TRUE;
			}
		}
	}

	if( bDirty == TRUE ){
		pItem->GetProject()->DirtyProject();
	}
	if( pMissing ) delete pMissing;

#endif // Unused code.
	return TRUE;
}

// Generate the corresponding outputs for each of the input file sets in each
// of the actions
// (errors should be pushed into the error context EC)
// The 'type' is an AOGO_.
BOOL CBuildTool::GenerateOutput(UINT type, CActionSlobList & actions, CErrorContext & EC)
{
	return FALSE;	// ok
}

// Return the action state given an action that specifies the input and output
// file sets
// (errors should be pushed into the error context EC)
ActionState CBuildTool::FigureActionSate(CActionSlob action, CErrorContext & EC)
{
	return _Unknown;
}

// UINT type can be TOB_Build, or TOB_ReBuild
// UINT stage can be TOB_Pre, TOB_Stage, TOB_Post
UINT CBuildTool::PerformBuildActions(UINT type, UINT stage, CActionSlobList & lstActions,
									 DWORD attrib, CErrorContext & EC)
{
	UINT act = ACT_Complete;

	CProject * pProject = g_pActiveProject;	// default

	// make sure we're using the right project
	if (!lstActions.IsEmpty())
		pProject = ((CActionSlob *)(lstActions.GetHead()))->Item()->GetProject();

	// perform the pre-build?
	if (stage == TOB_Pre)
	{
		CDir dir;	// directory of output file

		POSITION pos = lstActions.GetHeadPosition(); 
		while (pos != (POSITION)NULL && (act == ACT_Complete) /* failure */)
		{
			CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(pos);
			CFileRegSet * psetOutput = pAction->GetOutput(); // want full list of output files

#ifdef KEEP_PCH
			FileRegHandle frhCreatePch = NULL;
			if (type == TOB_Clean)
			{
				if ( IsKindOf(RUNTIME_CLASS(CCCompilerTool)) )
				{
					if ( !((CCCompilerTool*)this)->GetItemCreatePCHFile(pAction->Item(), frhCreatePch))
					{
						frhCreatePch = NULL;
					}
				}

			}
#endif

			psetOutput->InitFrhEnum();
			FileRegHandle frh;
			while ((frh = psetOutput->NextFrh()) != (FileRegHandle)NULL)
			{
				// need to create directory for output?
				if (dir.CreateFromPath(*pAction->m_pregistry->GetRegEntry(frh)->GetFilePath()) &&
					!dir.ExistsOnDisk()	// directory doesn't exist on disk?
				   )
				{
					// try to create
					if (!dir.CreateOnDisk())
					{
						// error, 'failed to create directory'
						CString str;
						EC.AddString(::MsgText(str, IDS_COULD_NOT_CREATE_DIR, (const TCHAR *) dir));

						if (!(attrib & AOB_IgnoreErrors))
						{
							act = ACT_Error; break;
						}
					}
				}

				// delete output files for rebuild?
				if (type == TOB_Clean)
				{
					// Special handling is required for schmooze tool users, since we may need to
					// explicitly delete certain targets (such as .ILK files) in order to
					// guarantee a clean build, however we may not want to delete other targets
					// (such as the browser file) which are useful during the build.  Thus we keep
					// separate filters for targets which are "Products" (i.e. time-stamping) and
					// targets to "Delete on Rebuild" for a clean buld.
					if (IsKindOf(RUNTIME_CLASS(CSchmoozeTool)))
					{
						// for now, this can only be the main project
						ASSERT(pAction->Item()->IsKindOf(RUNTIME_CLASS(CTargetItem)));

						// FUTURE: fix this stuff when CSchmoozeTool merges with CSourceTool
						// linker deletes all outputs, bscmake none, mfile none
						if (!((CSchmoozeTool *)this)->IsDelOnRebuildFile(g_FileRegistry.GetRegEntry(frh)->GetFilePath()))
#ifndef REFCOUNT_WORK
							continue;
#else
						{
							frh->ReleaseFRHRef();
							continue;
						}
#endif
					}

#ifdef KEEP_PCH
					// Don't delete pch used for persistent pch.
					if ( frh == frhCreatePch )
#ifndef REFCOUNT_WORK
						continue;
#else
					{
						frh->ReleaseFRHRef();
						continue;
					}
#endif
#endif


					// if we can't delete this output and we're not forcing a build then error
					if (!g_buildengine.DeleteFile(frh, EC) && !(attrib & AOB_IgnoreErrors))		  
					{
						act = ACT_Error; break;
					}
				}
#ifdef REFCOUNT_WORK
				frh->ReleaseFRHRef();
#endif
			}
#ifdef KEEP_PCH
#ifdef REFCOUNT_WORK
			if (NULL!=frhCreatePch)
				frhCreatePch->ReleaseFRHRef();
#endif
#endif
		}
	}
	// actually perform the build?
	else if (stage == TOB_Stage)
	{
		CPtrList plCmds;

		if (!GetCommandLines(lstActions, plCmds, attrib, EC))
		{
			g_buildengine.LogBuildError(BldError_CantGetCmdLines, this, EC);
			if (!(attrib & AOB_IgnoreErrors))
				act = ACT_Error;	// failure
		}

		// ok to continue with what we have?
		if (act == ACT_Complete)
		{
			// check for need to preprend command with comspec if custom build tool
			BOOL fCheckForComSpec = IsKindOf(RUNTIME_CLASS(CCustomBuildTool));
			UINT cmd = CMD_Complete;

			cmd = g_buildengine.ExecuteCmdLines(plCmds, pProject->GetWorkspaceDir(), fCheckForComSpec, attrib & AOB_IgnoreErrors, EC);
			
			switch (cmd)
			{
				case CMD_Complete:	act = ACT_Complete;	break;
				case CMD_Canceled:	act = ACT_Canceled;	break;
				case CMD_Error:		act = ACT_Error;	break;

				default:			ASSERT(FALSE);		break;
			}
		}
	}
	// perform the post-build?
	else if (stage == TOB_Post)
	{
		// all of our outputs of the action are dirty
		POSITION pos = lstActions.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(pos);
			CFileRegSet * pregset = pAction->GetOutput();
			pregset->InitFrhEnum();
			FileRegHandle frh;
			while ((frh = pregset->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
				pAction->m_pregistry->GetRegEntry(frh)->MakeDirty();
#else
			{
				pAction->m_pregistry->GetRegEntry(frh)->MakeDirty();
				frh->ReleaseFRHRef();
			}
#endif
		}

#if 0
		// Our dependencies may have been updated.
		if (g_bNewUpdateDepModel && (TOB_Build==type || TOB_ReBuild==type))
		{
			CActionSlob* pFirstAction = (CActionSlob*)lstActions.GetHead();
			if (NULL!=pFirstAction)
			{
				// Get IMreDependencies* so that it is cached.
				IMreDependencies* pMreDepend = NULL;
				BOOL fRefreshDepCtr = FALSE;
				CPath pathIdb = pFirstAction->m_pItem->GetProject()->GetMrePath(pFirstAction->m_pcrOur);
				if (!pathIdb.IsEmpty())
				{
					// Minimal rebuild dependency interface.
					pMreDepend = 
						IMreDependencies::GetMreDependenciesIFace((LPCTSTR)pathIdb);
				}

				// Update dependencies.
				// Set up a new cache for this target			
				g_ScannerCache.BeginCache();

				pos = lstActions.GetHeadPosition();
				CProjItem* pItem = NULL;
				while (pos != (POSITION)NULL)
				{
					CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(pos);
					if (NULL!=pAction)
						pItem = pAction->m_pItem;

					if (NULL!=pItem && pItem->IsKindOf( RUNTIME_CLASS(CFileItem) ))
						fRefreshDepCtr = pItem->ScanDependencies() || fRefreshDepCtr;
				}

				g_ScannerCache.EndCache();
				if (NULL!=pMreDepend)
					pMreDepend->Release();

				// Use target of last pItem to refresh dependency folder.
				if (NULL!=pItem && fRefreshDepCtr)
					pItem->GetTarget()->RefreshDependencyContainer();

			}
		}
#endif

	}

	return act;
}

// Get command lines for this tool
// o performs default for both non-target and target tools
BOOL CBuildTool::GetCommandLines
( 
	CActionSlobList & lstActions,
	CPtrList & plCommandLines,
	DWORD attrib,
	CErrorContext & EC
)
{
	if (m_fTargetTool)
	{
		// generate standard command-line for target tool
		// FUTURE:
		ASSERT(FALSE);
	}
	else
	{
		// generate standard command-line for non-target tool
		
		CString strFilePath, strCmdLine;

		POSITION posAction = lstActions.GetHeadPosition();
		while (posAction != (POSITION)NULL)
		{
			CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(posAction);

			CProjItem * pItem = pAction->Item();
			ActionState as = pAction->m_state;

 			strFilePath = (const TCHAR *)*pItem->GetFilePath();

			CString strOptions;
			if (!pAction->GetCommandOptions(strOptions))
				return FALSE;

			// our command-line of form,
			// '<exe> <options> "<input-file>"'
			strCmdLine = m_strToolExeName;
			strCmdLine += _TEXT(" ");
			strCmdLine += strOptions;
			strCmdLine += _TEXT(" \"");
			strCmdLine += strFilePath;
			strCmdLine += _TEXT("\"");

			CCmdLine *pCmdLine = new CCmdLine;
			pCmdLine->slCommandLines.AddHead(strCmdLine);

			// our description
			pCmdLine->slDescriptions.AddHead(m_strDescription);
			pCmdLine->nProcessor = 1;
			plCommandLines.AddTail(pCmdLine);
		}
	}

	return TRUE;
}

BOOL CBuildTool::WriteBuildRule(CActionSlob * pAction)
{
	ASSERT(PresentInActiveConfig(pAction->m_pItem));

	//
	// review(tomse/karlsi):
	// Should check if the tool does inference rule.  Check for CSpecialBuildTool or
	// CCustomBuildTool for now so that they are written properly when
	// g_bExportMakefile is TRUE.
	//
	if (g_bExportMakefile && !IsKindOf(RUNTIME_CLASS(CSpecialBuildTool))
		&& !IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
	{
		if (pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		{
			if (bUsedInAllConfigs)
			{
				if (!bPerConfigFlags)
					WriteInferenceRuleMacro(pAction);
			 	WriteInferenceRules(pAction);
			}

			return TRUE;
		}
	}

	// 	Write it if we haven't already:
	if (!bShouldWriteBuildRulePerConfig && !DoWriteBuildRule(pAction))
		return FALSE;	// failure to write build rule

	return TRUE;	// success
}

BOOL CBuildTool::DoWriteBuildRule(CActionSlob * pAction)
{
	BOOL bUseProject = FALSE;

	if (!g_bExportMakefile)
		return FALSE; // nothing to write unless this is a makefile

	CString strTargets, strDeps, strDesc, strBuildLine;
	TRY
	{
		CObList lstItem; lstItem.AddHead(pAction->m_pItem);
		if (pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
		{
			if (FileNameMatchesExtension(pAction->m_pItem->GetFilePath(), _T("odl")))
			{
				// use OUTDIR for certain "source" tools
				bUseProject = TRUE;

				lstItem.AddTail(pAction->m_pItem->GetProject());
			}

			ASSERT(!IsKindOf(RUNTIME_CLASS(CCustomBuildTool)));
			// Do the targets : dependents line:
			MakeQuotedString(
						pAction->GetOutput(),
						strTargets,
						m_pBaseDir,
						TRUE,
						FilterFileItemTargets,
						(DWORD) this,
						_TEXT("\t"),FALSE, // write all valid
						(const CObList *)&lstItem
						, FALSE // not sorted
 					);
			
			strDesc = CProjItem::GetFileNameMacro();
			
#if 0 // NOT USED ANYMORE
			if (((CFileItem *)pAction->m_pItem)->GetDependenciesMacro(DEP_Scanned, strDeps, *m_pNameMunger))
			{
				strDesc += _TEXT(" $(");
				strDesc += strDeps;
				strDesc += _TEXT(")");
			}
#endif

			// add intermediate/target directories pseudo-dependencies
			strDeps = _TEXT("$(OUTDIR)");	// default for project
			if (bUseProject || pAction->m_pItem->GetOutDirString(strDeps, NULL, FALSE))
				strDesc += _TEXT(" \"") + strDeps + _TEXT("\"");	// quote for LFN

			// add our source dependencies
			CFileRegSet * pregsetSource = pAction->GetSourceDep();
#ifndef REFCOUNT_WORK
			if (!pregsetSource->GetContent()->IsEmpty())
#else
			if (!pregsetSource->IsEmpty())
#endif
			{
				strDesc += _T(' ');	// initial sep.

				MakeQuotedString(
							pregsetSource,
							strDesc,
							m_pBaseDir,
							TRUE,
							NULL, NULL,
							_TEXT(" "),FALSE, // all items
							(const CObList *)&lstItem
							, FALSE // not sorted
							);
			}

			// Write the action line.  First check to see if we can get away with
			// the project's inference rule:

			if (bParentFlags || bPerConfigFlags || bForceNoInferenceRule || bWeHaveFlags ||
				(!((CFileItem *)pAction->m_pItem)->m_bIsInProjDir)
			   )
			{
				if (bParentFlags || bPerConfigFlags || (bWeHaveFlags && !bProjPerConfigFlags))
				{
					g_prjoptengine.SetOptionHandler (GetOptionHandler());
					g_prjoptengine.SetPropertyBag (pAction->m_pItem);
					g_prjoptengine.GenerateString (strBuildLine, (OBShowMacro | OBShowDefault | OBShowFake | OBInherit));

					// The cCompiler supporst response files
					if( IsKindOf(RUNTIME_CLASS(CCCompilerTool)) ){
						m_pMakWriter->WriteMacro(_T("CPP_SWITCHES"), strBuildLine);
						strBuildLine = _T("@<<\r\n  $(CPP_SWITCHES) ");
						strBuildLine += CProjItem::GetFileNameMacro();
						strBuildLine += _T("\r\n<< ");
					}
					
					// The cCompiler supporst response files
					else if( IsKindOf(RUNTIME_CLASS(CMkTypLibTool)) ){
						m_pMakWriter->WriteMacro(_T("MTL_SWITCHES"), strBuildLine);
						strBuildLine = _T("@<<\r\n  $(MTL_SWITCHES) ");
						strBuildLine += CProjItem::GetFileNameMacro();
						strBuildLine += _T("\r\n<< ");
					}
					else
					{
						strBuildLine += CProjItem::GetFileNameMacro();
					}

				}
				else
				{
					ASSERT(strBuildLine.IsEmpty());
					strBuildLine = (strBuildLine + _T("$(")) + m_strToolPrefix + _T("_PROJ) ");
					strBuildLine += CProjItem::GetFileNameMacro();
				}

				strBuildLine = _TEXT("$(") + (m_strToolPrefix + (_TEXT(") ") + strBuildLine));
			}

			m_pMakWriter->WriteDescAndAction(strTargets, strDesc, strBuildLine);
			m_pMakWriter->EndLine ();

			return TRUE;	// success
		}

		ASSERT(pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)));
	}
	CATCH (CException, e)
	{
		return FALSE;	// failure
	}	
	END_CATCH

	return TRUE; // success
}

// Get command line options for this tool
BOOL CBuildTool::GetCommandOptions
(
	CActionSlob * pAction,
	CString & strOptions
)
{
	CProjItem * pTheItem = pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pAction->m_pItem->GetProject() : pAction->m_pItem;

	g_prjoptengine.SetOptionHandler(pAction->m_pTool->GetOptionHandler());
	g_prjoptengine.SetPropertyBag(pTheItem);

	// try to generate the command options from the tool's option handler
	if (!g_prjoptengine.GenerateString(strOptions))
		return FALSE;

	return TRUE;	// success
}

BOOL CBuildTool::WritePerConfigData(CProjItem * pItem, CActionSlob * pAction)
{
	//	Write our macro and our flags.  In some cases need to write our the
//  build rule as well.
	BOOL b;

	//
	// review(tomse/karlsi):
	// Should check if the tool does inference rule.  Check for CSpecialBuildTool or
	// CCustomBuildTool for now so that they are written properly when
	// g_bExportMakefile is TRUE.
	//
	if (g_bExportMakefile && !IsKindOf(RUNTIME_CLASS(CSpecialBuildTool))
		&& !IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
	{
		// For project, see if we need to write the inference rule or possibly
		// the default ine
		if (((pItem->IsKindOf(RUNTIME_CLASS(CProject))) || (pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))) && (pAction != NULL))
		{
			if ((!bUsedInAllConfigs) && (PresentInActiveConfig (pItem)))
			{
				WriteInferenceRuleMacro(pAction);			
				if (m_OutputDirOptions == PerConfigOutputDirs)
				 	WriteOutputDirMacro(pAction);
				WriteInferenceRules(pAction);
			}
			else
			{
				if (bPerConfigFlags)
					WriteInferenceRuleMacro(pAction);

				// Write output dirs macro that the inference rule will use later.
				if (m_OutputDirOptions == PerConfigOutputDirs)
				 	WriteOutputDirMacro(pAction);
			}

			// Usually, don't have to do anything.
			return TRUE;
		}
	}

	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
		CTargetItem * pTarget = pItem->GetTarget();
		ConfigurationRecord * pcrCurrentBase = (ConfigurationRecord*)pItem->GetActiveConfig()->m_pBaseRecord;
		
		ConfigurationRecord * pcrTarget;
		const CPtrArray* pConfigArray = pTarget->GetConfigArray();
		int nSize = pConfigArray->GetSize();
	 	for (int nConfig = 0; nConfig < nSize; nConfig++)
		{
 			pcrTarget = (ConfigurationRecord*)pConfigArray->GetAt(nConfig);
			if (pcrTarget->m_pBaseRecord == pcrCurrentBase && !pcrTarget->IsValid())
 				return TRUE;
 		}
	}

	if ((pItem->GetIntProp(P_ItemExcludedFromBuild, b) == valid && b != FALSE) ||
		!PresentInActiveConfig(pItem) 
	   )
		return TRUE;

	// There are other tools, then we need a per config build rule:
	if ( (bShouldWriteBuildRulePerConfig &&
		   (!pItem->IsKindOf(RUNTIME_CLASS(CFileItem)) ||
		     pItem->GetActiveConfig()->GetBuildTool() == this) 
	     ) ||
		 // if the tool is being used and the item is a CFileItem
		 // then we should write out the Build Rule as well.
		 (bWeAreUsed &&
		 	pItem->IsKindOf (RUNTIME_CLASS (CFileItem))
		 )
	   )
	{
		if (pAction != NULL)
			return DoWriteBuildRule(pAction);	// failure to write build rule
	}
	return TRUE;	// success
}

BOOL CBuildTool::WriteHeaderSection(CActionSlob * pAction)
{
	TRY
	{
		// is this a target oriented tool?
		if (m_fTargetTool)
		{
			ASSERT(FALSE);	// FUTURE:
		}
		else
		{
			// write out 'TOOL_PREFIX = TOOL_EXE'
			if (!m_strToolExeName.IsEmpty())
			{
				if (g_bWriteProject)
					m_pPrjWriter->WriteMacro(m_strToolPrefix, m_strToolExeName);
				if (g_bExportMakefile)
					m_pMakWriter->WriteMacro(m_strToolPrefix, m_strToolExeName);
			}
		}
	}
	CATCH(CException, e)
	{
			return FALSE;	// failure
	}	
	END_CATCH

	return TRUE;
}

#if 0
BOOL CBuildTool::WriteMultiInputs(CActionSlob * pAction)
{
	ASSERT(g_bExportMakefile);
	ASSERT(m_fTargetTool);	// only valid for target-tools (ie. legacy CSchmoozeTool)
	ASSERT(IsKindOf(RUNTIME_CLASS(CSchmoozeTool)));

	TRY
	{
		CString strInputs, strSpecInputs;
		CString str, strOutDirMacro, strOutDirValue, strCachedOutDirValue;
		
		// increase size of buffers
		// (note, it is not necessarily empty to begin with)
		int len = strInputs.GetLength();
		strInputs.GetBuffer(2048);
		strInputs.ReleaseBuffer(len);

		CDir dirOut;

		// actions that contrbiute to our input
		CActionSlobList lstActions;

		CFileRegSet * pregset = pAction->GetInput();
		pregset->InitFrhEnum();
		FileRegHandle frh;
		while ((frh = pregset->NextFrh()) != (FileRegHandle)NULL)
		{
			BOOL fSpecInput = FALSE;
			CFileRegEntry * preg = pAction->m_pregistry->GetRegEntry(frh);
			CPath * pPath = (CPath *)preg->GetFilePath();
			ASSERT(pPath != (const CPath *)NULL);

			// a special input?
			if (m_fTargetTool)
				fSpecInput = ((CSchmoozeTool *)this)->IsSpecialConsumable(pPath);

			CString * pstr = fSpecInput ? &strSpecInputs : &strInputs;

			*pstr += _TEXT ( " \\\r\n\t");

			CDir * pdirOut = (CDir *)NULL;

			// get output dir.
			// what is our item context?
			if (pAction->m_pdepgraph->RetrieveOutputActions(lstActions, g_DummyEC, frh) == CMD_Complete &&
				lstActions.GetCount() >= 1)
			{
				// our context
				CProjItem * pItem = ((CActionSlob *)lstActions.GetHead())->Item();
				if (pItem->GetOutDirString(strOutDirMacro, &strOutDirValue))
					// FUTURE (karlsi): slightly broken on LHS when (pItem && !bOneOnly)
					if (strOutDirValue == strCachedOutDirValue ||	// got this in the cache?
						dirOut.CreateFromString(strCachedOutDirValue = strOutDirValue))
					{
						pdirOut = &dirOut;	// got an output directory from the context
					}
			}

			int nPathStart = 0;
			if (pdirOut &&
				(pPath->GetRelativeName(*pdirOut, str, FALSE, TRUE)) &&
				(str[nPathStart++] == _T('.') && (str[nPathStart] == _T('\\'))))
			{
				//
				// relative to output directory
				//
				// replace '.\' at head of relative path with './' to fix NMAKE bug
				str.SetAt(1,_T('/'));

				// quote
				*pstr += _T('"');
				*pstr += strOutDirMacro;
				*pstr += ((const TCHAR *)str + 1);	// skip '.'
				*pstr += _T('"');
			}
			else if (m_pBaseDir)
			{
				//
				// relative to base-directory
				//
				// always get a relative path
				BOOL fOldAlwaysRelative = pPath->GetAlwaysRelative();
				pPath->SetAlwaysRelative();
				pPath->GetRelativeName(*m_pBaseDir, str);
				pPath->SetAlwaysRelative(fOldAlwaysRelative);

				// quote
				*pstr += _T('"');
				*pstr += str;
				*pstr += _T('"');
			}
			else
			{
				//
				// raw
				//
				*pstr += (const TCHAR *) *pPath;
			}

 		}

		// write-out our special inputs macro?
		const CSchmoozeTool::SchmoozeData macs = ((CSchmoozeTool *)this)->GetMacs();
		if (*(macs.pszSpecialConsume) && !strSpecInputs.IsEmpty())
		{
			// FUTURE: do we nee this?
			CString strOptions;
			if (*(macs.pszSpecialFlags))
			{
				if (g_bExportMakefile)
					m_pMakWriter->WriteMacro(macs.pszSpecialFlags, strOptions);
			}

			if (g_bExportMakefile)
 				m_pMakWriter->WriteMacro(macs.pszSpecialConsume, strSpecInputs);
		}	

		// write out inputs macro
		if (g_bExportMakefile)
			m_pMakWriter->WriteMacro(macs.pszConsumeMacro, strInputs);
	}
	CATCH(CException, e)
	{
		return FALSE;	// failure
	}	
	END_CATCH

	return TRUE;
}

#else // new version

BOOL CBuildTool::WriteMultiInputs(CActionSlob * pAction, BOOL bSort /* = FALSE */)
{
	ASSERT(g_bExportMakefile);
	ASSERT(m_fTargetTool);	// only valid for target-tools (ie. legacy CSchmoozeTool)
	ASSERT(IsKindOf(RUNTIME_CLASS(CSchmoozeTool)));

	TRY
	{
		CString strInputs, strSpecInputs;
		CFRFPtrArray regInputs, regSpecInputs;
		CObList olItems, olSpecItems;
		int nInputs = 0, nSpecInputs = 0;

		static CDir dirOut;

		// actions that contrbiute to our input
		CActionSlobList lstActions;
		CFileRegEntry * preg;
		CPath * pPath;
		CProjItem * pItem;

		CFileRegSet * pregset = pAction->GetInput();
		pregset->InitFrhEnum();
#ifndef REFCOUNT_WORK
		regInputs.SetSize(pregset->GetContent()->GetCount());
#else
		regInputs.SetSize(pregset->GetCount());
#endif
		
		FileRegHandle frh;
		while ((frh = pregset->NextFrh()) != (FileRegHandle)NULL)
		{
			preg = pAction->m_pregistry->GetRegEntry(frh);
			pPath = (CPath *)preg->GetFilePath();
			ASSERT(pPath != (const CPath *)NULL);

			// what is our item context?
			if ((pAction->m_pdepgraph->RetrieveOutputActions(lstActions, g_DummyEC, frh) == CMD_Complete) && (lstActions.GetCount() >= 1))
			{
				// our context
				pItem = ((CActionSlob *)lstActions.GetHead())->Item();
			}
			else
			{
				pItem = NULL; // default is no context
			}

			// a special input?
			if ((m_fTargetTool) && (((CSchmoozeTool *)this)->IsSpecialConsumable(pPath)))
			{
				regSpecInputs.SetAtGrow(nSpecInputs++, preg);
				olSpecItems.AddTail(pItem);
			}
			else
			{
				regInputs.SetAt(nInputs++, preg);
				olItems.AddTail(pItem);
			}
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif

 		}

		// pItem = pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pAction->m_pItem->GetProject() : pAction->m_pItem;
		pItem = pAction->m_pItem; // REVIEW

		// write-out our special inputs macro?
		const CSchmoozeTool::SchmoozeData macs = ((CSchmoozeTool *)this)->GetMacs();
		if (nSpecInputs != 0)
		{
			if (*(macs.pszSpecialConsume))
			{
				// FUTURE: do we need this?
				CString strOptions;
				if (*(macs.pszSpecialFlags))
				{
					// if (g_bExportMakefile)
						m_pMakWriter->WriteMacro(macs.pszSpecialFlags, strOptions);
				}

				// add extra item to determine config
				olSpecItems.AddHead(pItem);

				ASSERT(strSpecInputs.IsEmpty());
				strSpecInputs += _T(" \\\r\n\t");
				ASSERT(regSpecInputs.GetSize()==nSpecInputs);
				MakeQuotedString(
							&regSpecInputs,
							strSpecInputs,
							m_pBaseDir,
							TRUE,
							NULL, NULL,
							_T(" \\\r\n\t"),FALSE, // all items
							(const CObList *)&olSpecItems
							,bSort	// sort output
							);

				// if (g_bExportMakefile)
		 			m_pMakWriter->WriteMacro(macs.pszSpecialConsume, strSpecInputs);
			}	

			// adjust main array size if special consumables
			regInputs.SetSize(nInputs);
		}
		else
		{
			ASSERT(regInputs.GetSize()==nInputs);
		}

		// add extra item to determine config
		olItems.AddHead(pItem);

		ASSERT(strInputs.IsEmpty());
		strInputs += _T(" \\\r\n\t");
		MakeQuotedString(
					&regInputs,
					strInputs,
					m_pBaseDir,
					TRUE,
					NULL, NULL,
					_T(" \\\r\n\t"),FALSE, // all items
					(const CObList *)&olItems
					,bSort	// sort output
					);

		// write out inputs macro
		// if (g_bExportMakefile)
			m_pMakWriter->WriteMacro(macs.pszConsumeMacro, strInputs);
	}
	CATCH(CException, e)
	{
		return FALSE;	// failure
	}	
	END_CATCH

	return TRUE;
}
#endif

// Does this tool operate on this file?
BOOL CBuildTool::AttachToFile(FileRegHandle frh, CProjItem * pItem)
{
	// is this a target oriented tool? if so then only attach to a target ...
	if (m_fTargetTool)
		return pItem && (pItem->GetTarget() == pItem);

	// this is a source oriented tool, so only attach to a file if it's in our input set
	return frh && FileNameMatchesExtension(g_FileRegistry.GetRegEntry(frh)->GetFilePath(), m_strToolInput); 
}

BOOL CBuildTool::ProcessAddSubtractString
(
	CProjItem * pItem,
	const TCHAR * pstr,
	BOOL fSubtract,
	BOOL fBaseBag /* = FALSE */
)
{
	PFILE_TRACE ("%s %s options \"%s\" for %s at %lx in config %s (@%lx)\n",
		(const TCHAR *) GetRuntimeClass()->m_lpszClassName,
		(fSubtract) ? "subtracting" : "adding",
		(const TCHAR *) pstr,
		(const TCHAR *) pItem->GetRuntimeClass()->m_lpszClassName,
		pItem,
		(const TCHAR *) pItem->GetActiveConfig()->GetConfigurationName(),
		(UINT) pItem->GetActiveConfig()
		);

	g_prjoptengine.SetOptionHandler(GetOptionHandler());
	g_prjoptengine.SetPropertyBag ( pItem );

	OptBehaviour optbeh = OBSetDefault | OBNeedEmptyString;
	if (fSubtract)	optbeh |= OBAnti;
	if (fBaseBag) optbeh |= OBBasePropBag;
	return g_prjoptengine.ParseString ( CString (pstr), optbeh );
}

int CBuildTool::GetToolUsedCount(CProject * pProject)
{
	const CPtrArray & ppcr = *pProject->GetConfigArray();
	int i, nNumConfigs = pProject->GetPropBagCount();

	int nUsedCount = 0;
	CProjTempConfigChange projTempConfigChange(pProject);

	for (i = 0; i < nNumConfigs; i++)
	{
		ASSERT_VALID ((ConfigurationRecord *)ppcr[i]);

		projTempConfigChange.ChangeConfig((ConfigurationRecord *)ppcr[i]);

		if (PresentInActiveConfig(pProject))
			nUsedCount++;

	}

	return nUsedCount;
}

BOOL CBuildTool::PresentInActiveConfig(CProjItem * pItem)
{
	// FUTURE: remove (v4?)
	// switch in the project if we need to
	// we store the actions there...
	CProjItem * pTheItem = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;

	CActionSlobList * pActions = pTheItem->GetActiveConfig()->GetActionList();
	POSITION pos = pActions->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos);
		if (pAction->m_pTool == this)
			return TRUE;
	}

	return FALSE;
}

extern int g_nProjTypesToWrite;	// defined in project.cpp

int	CBuildTool::BeginWritingSession
(
	CProjItem * pItem,
	CActionSlob * /* pAction */,	// not used
	CMakFileWriter * ppw,
	CMakFileWriter * pmw,
	const CDir * pBaseDir,
	CNameMunger * pnm
)
{
	CProjItem * pTheItem = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;

	int wa = NoAreas;
	bPerConfigTargs = bParentFlags = bOtherTools = bShouldWriteBuildRulePerConfig =
		bUsedInAllConfigs = bWeHaveFlags = bWeAreUsed = bPerConfigFlags = bPerConfigDeps = FALSE;
	bPerConfigInt = bPerConfigExt = FALSE;
	CPtrList pl;
	CObList lstItem;
	CString strTargetsBase, strTargets;
	BOOL bFirstConfig, bAlwaysExcluded, bEverExcluded, b;
	ConfigurationRecord * pcr;
	const CPtrArray & ppcr = *pTheItem->GetConfigArray();
	int i, nNumConfigs = pTheItem->GetPropBagCount();

	// FUTURE: remove this
	m_pItemBogus = pTheItem;

	m_pPrjWriter = ppw;
	m_pMakWriter = pmw;
	m_pBaseDir = pBaseDir;
	m_pNameMunger = pnm;

	COptionHandler * pOptionHandler = GetOptionHandler();
	g_prjoptengine.SetOptionHandler(pOptionHandler);
	g_prjoptengine.SetPropertyBag (pTheItem);

	if (pTheItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{

		// Enum the item's configs. If it's a file item, see if we are the tool
		// for any of them and if we are, see if we have any flags in any of those configs.
		// For containers, just look for our flags.  Also examine the targets lists
		// to see if theyre all the same (up to automatically created pch files)

		/* bAlwaysExcluded = */ bFirstConfig = TRUE;
		bAlwaysExcluded = TRUE;
		bEverExcluded = FALSE;
		BOOL bCheckedFlags = FALSE;
		for (i = 0; i < nNumConfigs; i++)
		{
			pcr = (ConfigurationRecord *)ppcr[i];
			ASSERT_VALID (pcr);
			pTheItem->ForceConfigActive(pcr);
			if (pTheItem->GetIntProp(P_ItemExcludedFromBuild, b) == valid)
			{
			    if (b)
			    	bEverExcluded = TRUE;
			    else
			    	bAlwaysExcluded = FALSE;
			}
			else
				b = FALSE;

			if (!b && (this == pcr->GetBuildTool()))
			{
				bWeAreUsed = TRUE;

				if (bFirstConfig)
				{
					bWeHaveFlags = pOptionHandler->HasAnyOfOurProperties(pTheItem, TRUE);
					bPerConfigExt = bPerConfigFlags = ((bProjPerConfigFlags && bWeHaveFlags) ||
					/* bEverExcluded || */
					(pOptionHandler->HasPerConfigProperties(pTheItem, FALSE)));
	
					bPerConfigInt = (bEverExcluded ||
					(pOptionHandler->HasPerConfigProperties(pTheItem, FALSE)));

					bCheckedFlags = TRUE;

					if (bEverExcluded)
						bPerConfigTargs = TRUE;
					else
					{
						lstItem.AddHead(pTheItem);
						// Create a base list to compare the others against:
						MakeQuotedString(pcr->GetOutputFilesList(),
							strTargetsBase,
							m_pBaseDir,
							TRUE,
							FilterFileItemTargets,
							(DWORD) this
							,_TEXT(","),FALSE
							,(const CObList *)&lstItem
							,TRUE // sorted
 						);
					}
					bFirstConfig = FALSE;
				}
				else
				{
					ASSERT(bCheckedFlags);
					if (!bPerConfigTargs)
					{
						if (bEverExcluded)
							bPerConfigTargs = TRUE;
						else
						{
							strTargets.Empty();	 // make sure this is empty each time
							MakeQuotedString(pcr->GetOutputFilesList(),
								strTargets,
								m_pBaseDir,
								TRUE,
								FilterFileItemTargets,
								(DWORD) this
								,_TEXT(","),FALSE
								,(const CObList *)&lstItem
							    ,TRUE // sorted
 							);
							if (strTargets.CompareNoCase(strTargetsBase))
								bPerConfigTargs = TRUE;
						}
					}
				}
			}
			else bOtherTools = TRUE;	// other tool or no tool

			// reset the config.
			pTheItem->ForceConfigActive();
		}

		if (bWeAreUsed)
		{
			wa |= ToolUsed;
			// Being excluded from a config is the same as having no tool,
			// hence there would be other tools:
			if (bEverExcluded)
				bOtherTools = /* bPerConfigFlags = */ bPerConfigTargs = TRUE;

			if (bOtherTools)
			{
				wa |= PerConfig;
			}
			else if (bPerConfigFlags || bPerConfigExt || bPerConfigTargs || bParentFlags)
			{
				ASSERT(!bParentFlags); // should no longer be used
				wa |= PerConfigExt;
			}
#if 0		// REVIEW: never use BuildRule logic for fileitems -- broken
			else
				wa |= BuildRule;
#endif

			if (bPerConfigInt)
				wa |= PerConfigInt;
		}
	}
	else if (pTheItem->IsKindOf(RUNTIME_CLASS(CProject)))
	{
		// Project
		bProjPerConfigFlags = FALSE;	// make sure init
		if ((i = GetToolUsedCount(pTheItem->GetProject())) > 0)
		{		
			UINT bTemp;
			bWeAreUsed = TRUE;
			wa = Header | ToolUsed;

			// If we are used in all the configurations, then we can put our
			// inference rule in the build rule section:
			if ((g_nProjTypesToWrite == 1) &&  (i == pTheItem->GetPropBagCount()))
			{
				wa |= BuildRule;
				bUsedInAllConfigs = TRUE;
				// note: this function's ret val is pseudo-boolean (tri-state)
				bTemp = pOptionHandler->HasPerConfigProperties(pTheItem, TRUE);
			}
			else
			{
				bUsedInAllConfigs = FALSE;
				bTemp = 1;
			}
			m_OutputDirOptions = HasPerConfigOutputDirs (pTheItem);

			bProjPerConfigFlags = (bTemp==1);	// ignore Base flags for this
			bPerConfigFlags = (bTemp!=0);
			if (bPerConfigFlags)
				wa |= PerConfig;
		}
	}

	// Internally remember where we plan to write the build rules:
	bShouldWriteBuildRulePerConfig =
						bPerConfigTargs ||
				        ((!bUsedInAllConfigs || bPerConfigFlags) &&
						 pTheItem->IsKindOf(RUNTIME_CLASS(CProject))
						) ||
						((bOtherTools || bPerConfigFlags || bPerConfigExt || bParentFlags) &&
						 pTheItem->IsKindOf(RUNTIME_CLASS(CFileItem))
						);

	return wa;
}

void CBuildTool::EndWritingSession()
{
}

void CBuildTool::WriteAddSubtractLine(CProjItem * pItem)
{	
	ASSERT(g_bWriteProject);

	// FUTURE: remove (v4?)
	// switch in the project if we need to, we store the 
	// properties there
	CProjItem * pTheItem = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;

	COptionHandler * popthdlr = GetOptionHandler();
	g_prjoptengine.SetOptionHandler(popthdlr);
	g_prjoptengine.SetPropertyBag(pTheItem);

	if (popthdlr->HasAnyOfOurProperties(pTheItem))
	{
		CString str;

		// first write BASE properties, if any
		if (g_prjoptengine.GenerateString(str, OBBasePropBag | OBShowDefault | OBNeedEmptyString) && !str.IsEmpty())
		{
			ASSERT(!m_strToolPrefix.IsEmpty());
			if (!m_strToolPrefix.IsEmpty())
			{
				str = CProjItem::pcADD + (CProjItem::pcBASE + (m_strToolPrefix + (_T(' ') + str)));
				m_pPrjWriter->WriteComment(str);
			}
			str.Empty();
		}
		if (g_prjoptengine.GenerateString(str, OBBasePropBag | OBShowDefault | OBAnti | OBNeedEmptyString) && !str.IsEmpty())
		{
			ASSERT(!m_strToolPrefix.IsEmpty());
			if (!m_strToolPrefix.IsEmpty())
			{
				str = CProjItem::pcSUBTRACT + (CProjItem::pcBASE + (m_strToolPrefix + (_T(' ') + str)));
				m_pPrjWriter->WriteComment(str);
			}
			str.Empty();
		}

		// next write current properties
		if (g_prjoptengine.GenerateString(str, OBShowDefault | OBNeedEmptyString) && !str.IsEmpty())
		{
			ASSERT(!m_strToolPrefix.IsEmpty());
			if (!m_strToolPrefix.IsEmpty())
			{
				str = CProjItem::pcADD + (m_strToolPrefix + (_T(' ') + str));
				m_pPrjWriter->WriteComment(str);
			}
			str.Empty();
		}
		if (g_prjoptengine.GenerateString(str, OBShowDefault | OBAnti | OBNeedEmptyString) && !str.IsEmpty())
		{
			ASSERT(!m_strToolPrefix.IsEmpty());
			if (!m_strToolPrefix.IsEmpty())
			{
				str = CProjItem::pcSUBTRACT + (m_strToolPrefix + (_T(' ') + str));
				m_pPrjWriter->WriteComment(str);
			}
			str.Empty();
		}
	}
}

/*
R.I.P. for v4.0 with VBA?
///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CCustomTool ------------------------------
//
///////////////////////////////////////////////////////////////////////////////

CCustomTool::CCustomTool
(
	const TCHAR * pchName
)
{
	// our component name
	m_strCompName = pchName;

	// we're not internal!
	m_bToolInternal = FALSE;

	// FUTURE: we assume we not a target tool
	m_fTargetTool = FALSE;

	// create ourselves a simple option handler
	SetOptionHandler(new COptHdlrUnknown(this));

	// FUTURE: clean-this up (from old CSourceTool)
	bForceNoInferenceRule = TRUE;

	Refresh();
}

CCustomTool::~CCustomTool()
{
	// FUTURE: recycle these
	// delete our option handleR
	delete GetOptionHandler();
}

void CCustomTool::Refresh() 
{
	// generate our description
	VERIFY(m_strDescription.LoadString(IDS_DESC_PERFORMING));
	m_strDescription += _T('\'');
	m_strDescription += *GetName();
	m_strDescription += _T('\'');
}

BOOL CCustomTool::GenerateOutput(UINT type, CActionSlobList & actions, CErrorContext & EC)
{
	POSITION posAction = actions.GetHeadPosition();
	while (posAction != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)actions.GetNext(posAction);
		CProjItem * pItem = pAction->Item();

		CFileRegistry * pRegistry = pItem->GetRegistry();

		CProject *pProject = pItem->GetProject();
		ASSERT (pProject);

		if (m_fTargetTool)
		{
			ASSERT(!m_fTargetTool);	// not supported yet!
		}
		else
		{
			// process each of our output specs.
			COptionList lstOutSpec; lstOutSpec.SetString((const TCHAR *)m_strToolOutput);
			POSITION pos = lstOutSpec.GetHeadPosition();

			while (pos != (POSITION)NULL)
			{
				const TCHAR * pchOutSpec = lstOutSpec.GetNext(pos);
				CPath pathOutput;		// path of our output based on this spec.

				CFileRegSet * pfrsInput = pAction->GetInput();
				pfrsInput->InitFrhEnum();
				FileRegHandle frh;
				while ((frh = pfrsInput->NextFrh()) != (FileRegHandle)NULL)
				{
					const CPath * pPath = pRegistry->GetRegEntry(frh)->GetFilePath();
					ASSERT(pPath != (const CPath *)NULL);

					// form the output
					if (FormOutputI(pathOutput, pPath, pchOutSpec, pItem))
					{
						if (!pAction->AddOutput((const CPath *)&pathOutput))
					 	{
							// FUTURE: error goes here! 
							ASSERT(FALSE);	
						}
					}
#ifdef REFCOUNT_WORK
					frh->ReleaseFRHRef();
#endif
				}
			} 
		}
	}

	return TRUE; // success
}

BOOL CCustomTool::FormOutputI
(
	CPath & pathOutput,
	const CPath * pPathInput,
	const TCHAR * pchOutSpec,
	CProjItem * pItem
)
{
	TCHAR pchOutput[_MAX_PATH], * pchSpec = (TCHAR *)pchOutSpec;
	*pchOutput = _T('\0');	// our generated output based on this spec.
	
	// copy everything verbatim unless we come across a macro
	while (*pchSpec != _T('\0'))
	{
		// macro?
		TCHAR * pch = pchSpec;
		while (!(*pch == _T('$') && *(pch + 1) == _T('(')) && *pch != _T('\0'))
			pch = _tcsinc(pch);

		// do block append?
		if (pch != pchSpec)
		{
			// yes
			_tcsncat(pchOutput, pchSpec, pch - pchSpec);

			// advance
			pchSpec = pch;
		}
		else
		{
			// no, macro?
			if (*pch == _T('$') && *(pch + 1) == _T('('))
			{
				UINT idMacro; int cchMacro;

				// macro-name exact match?
				// skip '$('
				pch += 2;

				if (CCustomTool::MapMacroNameToId(pch, cchMacro, idMacro) &&
					*(pch + cchMacro) == _T(')'))
				{
					// advance and skip macro
					pchSpec = pch + cchMacro;

					// skip ')'
					pchSpec ++;

					switch (idMacro)
					{
						// intermediate directory
						// output directory
						case IDMACRO_INTDIR:
						case IDMACRO_OUTDIR:
						{
							// directory prop?
							UINT idOutDirProp = idMacro == IDMACRO_INTDIR ?
												 P_OutDirs_Intermediate : P_OutDirs_Target;

							CString strDir;
							GPT gpt = pItem->GetStrProp(idOutDirProp, strDir);
							while (gpt != valid)
							{
								pItem = (CProjItem *)pItem->GetContainer();
								ASSERT(pItem != (CSlob *)NULL);
								gpt = pItem->GetStrProp(idOutDirProp, strDir);
							}

							if (!strDir.IsEmpty())
							{
								// append a '\'?
								TCHAR chLast = strDir[strDir.GetLength() - 1];
								if (chLast != _T('\\') && chLast != _T('/'))
									strDir += _T('/');

								// append directory value
								_tcscat(pchOutput, (const TCHAR *)strDir);
							}
							break;
						}

						// input basename
						case IDMACRO_INPBASE:
						{
							CString strBase;
							pPathInput->GetBaseNameString(strBase);

							if (!strBase.IsEmpty())
							{
								// append file base
								_tcscat(pchOutput, (const TCHAR *)strBase);
							}
							break;
						}

						// target basename
						case IDMACRO_TRGBASE:
						{
							break;
						}

						default:
							break;
					}
				}
				else
				{
					// FUTURE: not a recognised macro...
					ASSERT(FALSE);
					return FALSE;
				}
			}
		}
	}

	// create our path given this output string
	return pathOutput.Create(pchOutput);
}
///////////////////////////////////////////////////////////////////////////////
//
// CCustomTool::MapMacroIdToName()
// CCustomTool::MapMacroNameToId()
// moved into toolsdlg.h as global functions
//
*/

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CCustomBuildTool ----------------------------
//
///////////////////////////////////////////////////////////////////////////////

const TCHAR g_pcBeginCustomBuild[] = _TEXT("Begin Custom Build");
const TCHAR g_pcEndCustomBuild[] = _TEXT("End Custom Build");
const TCHAR g_pcCustomDescSep[] = _TEXT(" - ");

CCustomBuildTool::CCustomBuildTool()
{
	// our component name (and UI name)
	m_nIDName = m_nIDUIName = IDS_CUSTOMBUILD_TOOL;

	// we're not internal!
	m_bToolInternal = FALSE;

	// FUTURE: we assume we not a target tool
	m_fTargetTool = FALSE;

	// FUTURE: clean-this up (from old CSourceTool)
	bForceNoInferenceRule = TRUE;

	// create ourselves a simple option handler
	SetOptionHandler(new COptHdlrUnknown(this));

	m_nOrder = 6;
}

CCustomBuildTool::~CCustomBuildTool()
{
	// FUTURE: recycle these
	// delete our option handleR
	delete GetOptionHandler();
}

BOOL CCustomBuildTool::GetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & strTab, BOOL fDefault /* = TRUE */)
{
	// get the tool string prop, don't use inheritance!
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	OptBehaviour optbeh;
	if (pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		optbeh = optbehOld | OBInherit;
	else
		optbeh = optbehOld & ~OBInherit;

	// use defaults?
	if (!fDefault) optbeh &= ~OBShowDefault;

	(void)pItem->SetOptBehaviour(optbeh);

	// get the prop.
	strTab.Empty();  // make sure this starts off empty
	CString strNewLine;
	BOOL fRet = pItem->GetStrProp(idProp, strNewLine);
	int i=0;
	int last = strNewLine.GetLength();
	while( i < last ){
		if( strNewLine[i] == _T('\r') ) {
			if( (i+1)<last && strNewLine[i+1] == _T('\n') ){
				strTab += _T('\t');
				i++;
			}
			else {
				strTab += strNewLine[i];
			}
		} else {
			strTab += strNewLine[i];
		}
		i++;
	}

	(void)pItem->SetOptBehaviour(optbehOld);

	return fRet;
}

BOOL CCustomBuildTool::SetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & str)
{
	int i=0;
	CString strNew;
	while( i < str.GetLength() ){
		if( str[i] == _T('\t') ){
			strNew += _T("\r\n");
		}
		else {
			strNew += str[i];
		}
		i++;
	}
	return pItem->SetStrProp(idProp, strNew);
}

BOOL CCustomBuildTool::AttachToFile(FileRegHandle frh, CProjItem * pItem)
{
	// attach only if the command and output file string props. are non-empty
	CString str;
	
		// non-empty command prop?
	if (!GetCustomBuildStrProp(pItem, P_CustomCommand, str) || str.IsEmpty() ||

		// non-empty output prop?
		!GetCustomBuildStrProp(pItem, P_CustomOutputSpec, str) || str.IsEmpty()
	   )
		return FALSE;	// no

	return TRUE;	// yes
}

BOOL CCustomBuildTool::GenerateOutput(UINT type, CActionSlobList & actions, CErrorContext & EC)
{
	POSITION posAction = actions.GetHeadPosition();
	while (posAction != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)actions.GetNext(posAction);
		CProjItem * pItem = pAction->Item();

		CFileRegistry * pRegistry = pItem->GetRegistry();

		// get the project directory
		CProject *pProject = pItem->GetProject();
		ASSERT (pProject);

		CDir * pDir = &pProject->GetWorkspaceDir();

		// generate our output in the context of the project directory
		CCurDir dirCur(*pDir);

		if (m_fTargetTool)
		{
			ASSERT(!m_fTargetTool);	// not supported yet!
		}
		else
		{
			// process each of our output specs.
			CString strOutput;
			if (!GetCustomBuildStrProp(pItem, P_CustomOutputSpec, strOutput))
				continue;	// can't get the output specs.

			COptionList lstOutSpec(_T('\t'), FALSE,TRUE);
			lstOutSpec.SetString((const TCHAR *)strOutput);
			POSITION pos = lstOutSpec.GetHeadPosition();

			while (pos != (POSITION)NULL)
			{
				const TCHAR * pchOutSpec = lstOutSpec.GetNext(pos);
				CPath pathOutput;		// path of our output based on this spec.

				CFileRegSet * pfrsInput = pAction->GetInput();
				pfrsInput->InitFrhEnum();
				FileRegHandle frh;
				while ((frh = pfrsInput->NextFrh()) != (FileRegHandle)NULL)
				{
					const CPath * pPath = pRegistry->GetRegEntry(frh)->GetFilePath();
					ASSERT(pPath != (const CPath *)NULL);

					// form the output
					CString strOutput;
					VERIFY(ExpandMacros(&strOutput, pchOutSpec, pAction));
					
					// create our path given this output string
					if (pathOutput.Create(strOutput))
					{
						BOOL fTargetLevel = (pAction->Item()->GetProject() == pAction->Item());
						// only filter if not target-level custom build tool
						if (!pAction->AddOutput((const CPath *)&pathOutput, TRUE, !fTargetLevel))
					 	{
							// FUTURE: error goes here! 
							ASSERT(FALSE);	
						}
					}
#ifdef REFCOUNT_WORK
					frh->ReleaseFRHRef();
#endif
				}
			}
			//
			// Add source deps.
			//
			pAction->ClearSourceDep();
			CString strDeps;

			if (valid == pItem->GetStrProp(P_UserDefinedDeps, strDeps) )
			{
				CDir BaseDir = pItem->GetProject()->GetWorkspaceDir();

				//
				// SuckDependency is normally done while reading a makefile and
				// depends on the dependency caches being enabled.
				// Artificially Init and Free dependency caches.
				extern CMapStringToPtr	*g_pMakReadDepCache;
				BOOL bInitAndFreeCaches = NULL==g_pMakReadDepCache;

				if (bInitAndFreeCaches)
					InitDependencyCaches();

				while (!strDeps.IsEmpty())
				{
					// trim leading tabs
					while (_T('\t')==strDeps[0])
						strDeps = strDeps.Mid(1);

					CString str;

					int iEnd = strDeps.Find(_T('\t'));

					// iEnd can't be 0 since leading tabs were trimmed.
					ASSERT(0!=iEnd);

					if (-1==iEnd)
					{
						str = strDeps;
						strDeps.Empty();
					}
					else
					{
						str = strDeps.Left(iEnd);
						strDeps = strDeps.Mid(iEnd+1);
					}

					// form the output
					CString strExpanded;
					VERIFY(ExpandMacros(&strExpanded, str, pAction));

					BOOL fPerConfig = TRUE;
					FileRegHandle frh = pItem->SuckDependency(const_cast<TCHAR*>(LPCTSTR(strExpanded)), pAction->m_pTool, BaseDir, fPerConfig);

					if (NULL!=frh)
					{
						pAction->AddSourceDep(frh);
						frh->ReleaseFRHRef();
					}
				}
				if (bInitAndFreeCaches)
					FreeDependencyCaches( );
			}
		}
	}

	return TRUE; // success
}

BOOL CCustomBuildTool::GetCommandLines
( 
	CActionSlobList & lstActions,
	CPtrList & plCommandLines,
	DWORD attrib,
	CErrorContext & EC
)
{
	// generate command-lines for custom-build tool
	CString strFilePath, strCmdLine, strDescription;

	POSITION posAction = lstActions.GetHeadPosition();
	while (posAction != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(posAction);

		CProjItem * pItem = pAction->Item();

		// get the description 
		CString strDesc;
		VERIFY(GetCustomBuildStrProp(pItem, P_CustomDescription, strDesc));
		VERIFY(ExpandMacros(&strDescription, strDesc, pAction));
	
		// process each of our command spec.
		CString strCommand;
		if (!GetCustomBuildStrProp(pItem, P_CustomCommand, strCommand))
			continue;	// can't get the command spec.

		COptionList lstCmdSpec(_T('\t'), FALSE,TRUE);
		lstCmdSpec.SetString((const TCHAR *)strCommand);
		POSITION pos = lstCmdSpec.GetHeadPosition();
		if( pos == NULL )
			continue;

		CString strCmdLine;
		strCmdLine = _TEXT("@echo off\n");
		while (pos != (POSITION)NULL)
		{
			const TCHAR * pchOutSpec = lstCmdSpec.GetNext(pos);

			// construct our command-line
			CString strLine;
			VERIFY(ExpandMacros(&strLine, pchOutSpec, pAction));

			strCmdLine +=  strLine + _TEXT("\n");
		}
		// append our command-line
		if (!g_buildengine.FormBatchFile(strCmdLine, EC))
			return FALSE;	// error

		CCmdLine *pCmdLine = new CCmdLine;
		pCmdLine->slCommandLines.AddHead(strCmdLine);

		// our description
		pCmdLine->slDescriptions.AddHead(strDescription);
		pCmdLine->nProcessor = 1;
		plCommandLines.AddTail(pCmdLine);
	}

	return TRUE;
}

BOOL CCustomBuildTool::DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude)
{
	CObject * pObject = NULL;	// builder file element

	BOOL fRet = FALSE;	// only success if we find custom build end

    // Are we reading for an unknown platform
    BOOL fUnknown = !pItem->GetProject()->GetProjType()->IsSupported();

	// get our custom description from the prelude text
	// skip text
	pchCustomPrelude += (_tcslen(g_pcBeginCustomBuild) * sizeof(TCHAR));

	// do we have a description?
	if (*pchCustomPrelude != _T('\0'))
	{
		// skip over " - "
		int cSep = _tcslen(g_pcCustomDescSep);
		if (_tcsnicmp(pchCustomPrelude, g_pcCustomDescSep, cSep) == 0)
		{
			CString strPrelude(pchCustomPrelude + cSep * sizeof(TCHAR));
			VERIFY(SetCustomBuildStrProp(pItem, P_CustomDescription, strPrelude));
		}
	}

    CString strUnknownMacros;   // Raw macro strings for unknown platform

	TRY
	{
	   	while (pObject = g_buildfile.m_pmr->GetNextElement())
 	   	{
			// process this builder file element

			// premature EOF?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
				break;

			// look for the custom build end
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
			{
				CMakComment * pMakComment = (CMakComment *)pObject;

				// get the comment and skip leading whitespace
				TCHAR * pch = pMakComment->m_strText.GetBuffer(1);
				SkipWhite(pch);

				if (_tcsnicmp(g_pcEndCustomBuild, pch, _tcslen(g_pcEndCustomBuild)) == 0)
				{
					fRet = TRUE;
					break;
				}
			}
			// is this the description block?
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				CMakDescBlk * pMakDescBlk = (CMakDescBlk *)pObject;
				
				// read in the output
				CString str;

				GetCustomBuildStrProp(pItem, P_CustomOutputSpec, str);

				// strip quotes?
				if( str.GetLength() > 0 )
					str += _T('\t');

				if (pMakDescBlk->m_strTargets[0] == _T('"'))
			    	str += pMakDescBlk->m_strTargets.Mid(1, pMakDescBlk->m_strTargets.GetLength() - 2);
				else
					str += pMakDescBlk->m_strTargets;
				
				// replace bad macros here
				if( ReplaceMsDevMacros( str ) ){
					// Dirty the project object
					pItem->GetProject()->DirtyProject();
				}

				VERIFY(SetCustomBuildStrProp(pItem, P_CustomOutputSpec, str));

				// convert the string list into a '\t' sep'd one
				COptionList optlst(_T('\t'), FALSE,TRUE);
				POSITION pos = pMakDescBlk->m_listCommands.GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					CString str = pMakDescBlk->m_listCommands.GetNext(pos);

					// replace bad macros here
					if( ReplaceMsDevMacros( str ) ){
						// Dirty the project object
						pItem->GetProject()->DirtyProject();
					}

					TCHAR * pch = (TCHAR *)(const TCHAR *)str;
					TCHAR * pchStart = pch;
					int cch = str.GetLength();

					// skip leading whitespace
					SkipWhite(pch);
					cch -= (pch - pchStart);

					if (_tcsnicmp(pch, _TEXT("$(BuildCmds)"), cch) == 0)
						break;

					// skip leading '@'?
					if (*pch == _T('@'))
					{
						pch++;
						cch -= sizeof(TCHAR);
					}

					optlst.Append(pch, cch);
				}

				// set this property
				optlst.GetString(str);

				// anything?
				if (!str.IsEmpty())
					VERIFY(SetCustomBuildStrProp(pItem, P_CustomCommand, str));
			}
			// is this the macros ... we're going to read in 'BuildCmds=' if there is one
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				CMakMacro * pMacro = (CMakMacro *)pObject;

				if (_tcsicmp(pMacro->m_strName, _TEXT("BuildCmds")) == 0)
				{
					// read in the output
					CString str;

					// convert the string list into a '\t' sep'd one
					COptionList optlst(_T('\t'), FALSE,TRUE);

					// possible token start and end
					TCHAR * pBase = pMacro->m_strValue.GetBuffer(1);
					TCHAR * pMakEnd = pBase + pMacro->m_strValue.GetLength();	
					TCHAR * pTokenEnd = max(pBase, (TCHAR *)pMakEnd - 1);
					
					while (pTokenEnd < pMakEnd)
					{
						// skip whitespace
		  				SkipWhite(pBase);

						TCHAR * pTokenStart = pBase;

						// olympus 2020 (briancr)
						// Instead of calling GetMakToken, which will delimit
						// each token at any white space and cause the build
						// commands to be broken at each space, we'll find the
						// end of the token right here. The end of the token
						// is a tab or end of string.
						BOOL bInQuotes = FALSE;
						pTokenEnd = pTokenStart;

						while (*pTokenEnd) {
							// toggle quote flag
							if (*pTokenEnd ==  _T('"')) {
								bInQuotes = ~bInQuotes;
							}
							else if (*pTokenEnd == _T('\t')) {
								if (!bInQuotes) {
									break;
								}
							}
							pTokenEnd = _tcsinc ((char *) pTokenEnd);
						}  
						#ifdef _DEBUG
						if (bInQuotes && !*pTokenEnd) TRACE ("Unmatched quotes while getting token in DoReadBuildRule\n");
						#endif 

						// end olympus 2020 fix

						// skip leading '@'?
						if (*pTokenStart == _T('@'))
						{
							pTokenStart++;
						}

						optlst.Append(pTokenStart, pTokenEnd - pTokenStart);

						// next token
						pBase = _tcsinc(pTokenEnd);
					}

					// set this property
					optlst.GetString(str);

					// anything?
					if (!str.IsEmpty())
						VERIFY(SetCustomBuildStrProp(pItem, P_CustomCommand, str));
				}
				
				if (fUnknown)
				{
                    CString strMacro = pMacro->m_strName + _TEXT("=") + pMacro->m_strValue + _T("\r\n");

                    if (strUnknownMacros.IsEmpty())
                        strUnknownMacros = strMacro;
                    else
                        strUnknownMacros += strMacro;
				}
			}	

	   		delete pObject; pObject = (CObject *)NULL;
		}

        if (fUnknown)
            SetCustomBuildStrProp(pItem, P_CustomMacros, strUnknownMacros);
						 
		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	CATCH (CException, e)
	{
		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	END_CATCH

	return fRet;	// ok
}

// fix for speed bugs #4039 and 4307
// MsDevDir is an environment variable.
// environment variables are case insensitive.
// nmake is case sensitive. therefore our makefiles
// did not work from nmake in 4.0
// This routine touppers the bogus macro
BOOL ReplaceMsDevMacros( CString &str ){
	int nBegin = 0;
	int nEnd;
	int rValue = FALSE;
	CString strMsdev("MSDevDir");
	CString strRemoteTarget("RemoteTargetPath");
	int nLength = str.GetLength();

	// loop until end of string
	for(;;){
		CString strMacro;

		// find the beging of a macro
		while ( nBegin < (nLength-1) ) {
		    if( str[nBegin] == _T('$') && str[nBegin+1] == _T('(') ){
				nBegin+=2;
				break;
			}
			nBegin++;
		}
	
		if( nBegin >= nLength-1 ) return rValue;
	
   		nEnd = nBegin;
		// find the end of a macro
		while ( str[nEnd] != _T(')')  ) {
			strMacro += str[nEnd];
			nEnd++;
			if( nEnd>= nLength )
				return rValue;
		}

		// compare and replace if equal
		if( strMacro == strMsdev ){
			CString strUpper("MSDEVDIR");
			rValue = TRUE;
			int i;
			for( i=0; nBegin < nEnd; i++ ){
				str.SetAt(nBegin,strUpper[i]);
				nBegin++;
			}
		}
		if( strMacro == strRemoteTarget ) {
			CString strUpper("REMOTETARGETPATH");
			rValue = TRUE;
			int i;
			for( i=0; nBegin < nEnd; i++ ){
				str.SetAt(nBegin,strUpper[i]);
				nBegin++;
			}
		}
	}
}

int	CCustomBuildTool::BeginWritingSession
(
	CProjItem * /* pItem */,
	CActionSlob * /* pAction */,
	CMakFileWriter * ppw,
	CMakFileWriter * pmw,
	const CDir * pBaseDir,
	CNameMunger * pnm
)
{
	// always write build rule per-config.
	bShouldWriteBuildRulePerConfig = TRUE;

	m_pPrjWriter = ppw;
	m_pMakWriter = pmw;
	m_pBaseDir = pBaseDir;
	m_pNameMunger = pnm;

	// REVIEW
	return ToolUsed | PerConfig | PerConfigInt | PerConfigExt;	// per-config tool used...
}

BOOL CCustomBuildTool::DoWriteBuildRule(CActionSlob * pAction)
{
	CProjItem * pItem = pAction->Item();
	CString str, strCommand, strOutput, strDeps;
	BOOL fWroteRule = FALSE;

	// get our commands + outputs
	// FUTURE: handle multiple outputs
	if (GetCustomBuildStrProp(pItem, P_CustomCommand, strCommand) &&
		GetCustomBuildStrProp(pItem, P_CustomOutputSpec, strOutput))
	{
		if (g_bWriteProject)
		{
			// custom build prelude
			// this has the 'custom build' token and the name of the step
			CString strBeginText;
			strBeginText = g_pcBeginCustomBuild;

			// do we have user-specified name for this step?
			if (GetCustomBuildStrProp(pItem, P_CustomDescription, str, FALSE))
			{
				strBeginText += g_pcCustomDescSep;
				strBeginText += str;
			}

 			g_buildfile.m_ppw->WriteComment(strBeginText);
		}

		if (pItem->GetProject()->GetProjType()->IsSupported())
		{
			// all are unused initially
			SetMacroIdUsage(FALSE);

			if (ExpandMacros((CString *)NULL, strCommand, pAction) &&

				((valid != pItem->GetStrProp(P_UserDefinedDeps, strDeps) ) ||
				ExpandMacros((CString *)NULL, strDeps, pAction)) &&
				ExpandMacros((CString *)NULL, strOutput, pAction)
			   )
			{
				BYTE fMacros[IDMACRO_LAST-IDMACRO_FIRST];
				memset(fMacros, 0, sizeof(BYTE)*(IDMACRO_LAST-IDMACRO_FIRST));
				// write out any macros the custom build rule might need
				for (int idMacro = IDMACRO_FIRST; idMacro <= IDMACRO_LAST; idMacro++)
				{
					// Don't write IDEDIR (it's in the environment).
					// Only write macros that are used.
					// Always write INPUTFILE because it's always used by $(SOURCE)
					if (idMacro != IDMACRO_IDEDIR &&
						(idMacro == IDMACRO_INPTFILE || GetMacroIdUsage(idMacro))) {
						// set the flag for the macro
						fMacros[idMacro-IDMACRO_FIRST] = TRUE;
					}
				}
				// write out the macros
				// REVIEW: combine into one call!
				if (g_bWriteProject)
					WriteMacros(*(g_buildfile.m_ppw), fMacros, pAction);
				if (g_bExportMakefile)
					WriteMacros(*(g_buildfile.m_pmw), fMacros, pAction);
			}
			
			// need to write alias for the 'SOURCE' macro?
			if ((pItem->IsKindOf(RUNTIME_CLASS(CProject))) ||
				(pItem->IsKindOf(RUNTIME_CLASS(CTargetItem))))
			{
				// get the 'Input' macro name
				const TCHAR * pchMacroName;
				VERIFY(MapMacroIdToName(IDMACRO_INPTFILE, pchMacroName));
				CString strMacroValue;
				VERIFY(GetMacroValue(IDMACRO_INPTFILE, strMacroValue, pAction));
				BOOL bQuote = (strMacroValue[0] != _T('\"'));

				CString strMacroName;
				if (bQuote)
					strMacroName += _T('\"');
				strMacroName += _TEXT("$(");
				strMacroName += pchMacroName;
				strMacroName += _TEXT(")");
				if (bQuote)
					strMacroName += _T('\"');

				if (g_bWriteProject)
					g_buildfile.m_ppw->WriteMacro(_TEXT("SOURCE"), strMacroName);
				if (g_bExportMakefile)
					g_buildfile.m_pmw->WriteMacro(_TEXT("SOURCE"), strMacroName);
			}
		}
		else
		{
			// Write out macros saved when we read this custom build tool (for
			// an unknown platform)
			CString strMacros;
			GetCustomBuildStrProp(pItem, P_CustomMacros, strMacros);
		
		if (g_bWriteProject)
        		g_buildfile.m_ppw->WriteString(strMacros);
		if (g_bExportMakefile)
        		g_buildfile.m_pmw->WriteString(strMacros);
		}

		if (g_bExportMakefile)
		{
			// write out any dependencies, if necessary
			// only used for custom build rules
			if (!pItem->WriteMakeFileDependencies(*m_pMakWriter, *m_pNameMunger, FALSE))
				AfxThrowFileException(CFileException::generic);
		}

		// write the nmake dependency line
		CString strTargets, strDesc;
		CString strTargetsMak; // work-around nmake bug

		// write targets relative to project directory
		CPath path;
		(void)path.SetAlwaysRelative();

		if (g_bExportMakefile)
		{
			CObList lstItem; lstItem.AddHead(pItem);
			MakeQuotedString(
					pAction->GetOutput(),
					strTargetsMak,
					m_pBaseDir,
					TRUE,
					NULL,
					(DWORD) this,
					_TEXT("\t"),FALSE, // write all valid
					(const CObList *)&lstItem
					, FALSE // not sorted
 				);
		}


		if (g_bWriteProject)
		{
			// REVIEW: use literal format for .dsp files to avoid churn
			COptionList optlst(_T('\t'), FALSE);
			optlst.SetString(strOutput);

			POSITION pos = optlst.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				CString str0 = optlst.GetNext(pos);

				CString str = str0;

				BOOL bAlreadyQuoted = (str[0] == _T('"'));

				// quote if not already
				if (!bAlreadyQuoted)
					strTargets += _T('\"');

				// always get append a relative path
				strTargets += str;

				// quote if not already
				if (!bAlreadyQuoted)
					strTargets += _T('\"');

				if (!strTargets.IsEmpty())
					strTargets += _T('\t');
			}
		}

		strDesc = _TEXT("$(SOURCE)");
		strDesc += _TEXT(" \"$(INTDIR)\" \"$(OUTDIR)\"");
		
		if (g_bWriteProject)
			g_buildfile.m_ppw->WriteDescAndAction(strTargets, strDesc, strCommand);
		if (g_bExportMakefile)
		{
			strCommand = "<<tempfile.bat\t@echo off\t" + strCommand + "\n<<";
			CString strMacro;
			if (pItem->GetDependenciesMacro(DEP_UserDefined, strMacro, *m_pNameMunger))
			{
				strDesc += _T( " $(" );
				strDesc += strMacro;
				strDesc += _T(")");
			}
			g_buildfile.m_pmw->WriteDescAndAction(strTargetsMak, strDesc, strCommand);
		}

		fWroteRule = TRUE;
	}

	if (fWroteRule)
	{
		// custom build end
		if (g_bWriteProject)
	 		g_buildfile.m_ppw->WriteComment(g_pcEndCustomBuild);
	}

	return fWroteRule;	// success
}

BOOL CCustomBuildTool::WriteMacros(CMakFileWriter& mw, BYTE aMacros[], CActionSlob* pAction)
{
	// the array, aMacros[], contains a flag for each macro.
	// if the flag is set, write out the macro.
	// assume the array contains an entry for each macro.
	int nMacros = IDMACRO_LAST - IDMACRO_FIRST;
	for (int i = 0; i < nMacros; i++) {
		if (aMacros[i]) {
			int idMacro = IDMACRO_FIRST + i;
			// get the macro name
			const TCHAR* pchMacroName;
			VERIFY(MapMacroIdToName(idMacro, pchMacroName));

			// get the macro value
			CString strMacroValue;
			if (idMacro != IDMACRO_WKSPDIR)
				VERIFY(GetMacroValue(idMacro, strMacroValue, pAction));
			else
				strMacroValue = _TEXT(".");         // write out workspace dir relative to current dir

			// write this to the build file
			mw.WriteMacro(pchMacroName, strMacroValue);
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CUnknownTool ------------------------------
//
///////////////////////////////////////////////////////////////////////////////

UINT g_nIDToolUnknown = PROJ_PER_CONFIG_UNKNOWN;

CUnknownTool::CUnknownTool
(
	const TCHAR * lpszPlatform,
	const TCHAR * lpszPrefix,
	const TCHAR * lpszExeName /* = NULL */
)
{
	m_nIDUnkStr = g_nIDToolUnknown++;
	ASSERT(g_nIDToolUnknown <= (PROJ_PER_CONFIG_LAST));

	if (lpszExeName)
	{
		m_strToolExeName = lpszExeName;
		m_strCompName += _T(" (") + m_strToolExeName + _T(')');
	}
	else
	{
		m_strCompName = lpszPlatform;
		m_strCompName += _T(' ') + m_strToolPrefix + _T(" Tool");
	}

	m_strToolPrefix = lpszPrefix;

	SetOptionHandler(new COptHdlrUnknown(this));
	g_prjcompmgr.RegisterBuildTool(this);
}

CUnknownTool::~CUnknownTool()
{
	g_prjcompmgr.DeregisterBuildTool(this);
	delete GetOptionHandler();
}

///////////////////////////////////////////////////////////////////////////////
BOOL CUnknownTool::AttachToFile(FileRegHandle frh, CProjItem * pItem)
{
	// attach done in CProjItem::SuckMacro
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// function called to read a tool builder macro line
// ie. one of the form '<tool prefix>_FOOBAR = '
// for the unknown tool we'll preserve these...
BOOL CUnknownTool::ReadToolMacro(CMakMacro * pMacro, CProjItem * pItem)
{
	// don't inherit while getting prop.
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	(void)pItem->SetOptBehaviour(optbehOld & ~OBInherit);

	// collect all of our macro's raw-data
	if (pItem->GetStrProp(GetUnknownPropId(), m_strBuffer) != valid)
		m_strBuffer = _TEXT("");

	// append raw-data
	m_strBuffer += pMacro->m_strRawData;

	// set
	pItem->SetStrProp(GetUnknownPropId(), m_strBuffer);
	
	(void)pItem->SetOptBehaviour(optbehOld);

	return TRUE;	// ok
}

///////////////////////////////////////////////////////////////////////////////
int CUnknownTool::BeginWritingSession
(
	CProjItem * pItem,
	CActionSlob * /* pAction */,	// not used
	CMakFileWriter * ppw,
	CMakFileWriter * pmw,
	const CDir * pBaseDir,
	CNameMunger * pnm
)
{
	CString str;
	int rmr = Header;
	ConfigurationRecord * pcr;

    CProjItem * pTheItem = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;

	int i, nNumConfigs = pTheItem->GetPropBagCount();

	m_pPrjWriter = ppw;
	m_pMakWriter = pmw;
	m_pBaseDir = pBaseDir;
	m_pNameMunger = pnm;

	g_prjoptengine.SetOptionHandler(GetOptionHandler());
	g_prjoptengine.SetPropertyBag (pTheItem);

	BOOL bOldInherit = pTheItem->EnablePropInherit(FALSE);

	const CPtrArray * pCfgArray = pTheItem->GetConfigArray();
	for (i = 0; i < nNumConfigs; i++)
	{
		pcr = (ConfigurationRecord *)pCfgArray->GetAt(i);
		ASSERT_VALID(pcr);
		pTheItem->ForceConfigActive(pcr);

		// unknown tool should always be per-config if anything to write
		if (GetOptionHandler()->HasAnyOfOurProperties(pTheItem) ||
			(pTheItem->GetStrProp(GetUnknownPropId(), str) == valid && !str.IsEmpty()))
		{
			rmr |= PerConfig | PerConfigInt | PerConfigExt;

			// reset the config.
			pTheItem->ForceConfigActive();
			break;
		}

		// reset the config.
		pTheItem->ForceConfigActive();
	}

	pTheItem->EnablePropInherit(bOldInherit);

	return rmr;
}
///////////////////////////////////////////////////////////////////////////
void CUnknownTool::WriteInferenceRuleMacro(CActionSlob * pAction)
{
	CString strBuildLine;

	CProjItem * pTheItem = pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pAction->m_pItem->GetProject() : pAction->m_pItem;

	TRY
	{
		// UNDONE: if (bUsedInAllConfigs)
		{
			g_prjoptengine.SetOptionHandler (GetOptionHandler());
			g_prjoptengine.SetPropertyBag (pTheItem);
			g_prjoptengine.GenerateString (strBuildLine, (OBShowMacro | OBShowDefault | OBShowFake | OBInherit));

#if 0
			if (g_bWriteProject)	// REVIEW: needed?
				m_pPrjWriter->WriteMacro (m_strToolPrefix + _T("_PROJ"), strBuildLine);
#endif
			if (g_bExportMakefile)
				m_pMakWriter->WriteMacro (m_strToolPrefix + _T("_PROJ"), strBuildLine);
		}

	}
	CATCH (CException, e)
	{
		strBuildLine.Empty ();
		THROW_LAST ();
	}	
	END_CATCH
}

///////////////////////////////////////////////////////////////////////////
// Makefile reading and writing for a generic tool
///////////////////////////////////////////////////////////////////////////

void CUnknownTool::WriteInferenceRules(CActionSlob * pAction)
{
	ASSERT(pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)));

#if 0
	if (g_bWriteProject) //REVIEW: needed?
	{
		m_pPrjWriter->InsureSpace();
		m_pPrjWriter->WriteString(_T("UNDONE: per config inference rules:"));
		m_pPrjWriter->EndLine();
	}
#endif

	if (g_bExportMakefile)
	{
		m_pMakWriter->InsureSpace();
		m_pMakWriter->WriteString(_T("UNDONE: per config inference rules:"));
		m_pMakWriter->EndLine();
	}
}

void CUnknownTool::WriteOutputDirMacro(CActionSlob * pAction)
{
// UNDONE
#if 0
	if (g_bExportMakefile)
	{
		CString str;
		if (pAction->m_pItem->GetStrProp(P_OutputDir_Obj, str) != valid)
			str = "";	// failed to get prop, no output dir!

		GroomOutputString(str);
		m_pMakWriter->WriteMacro(_TEXT("CPP_OBJS"), str);

		if (pAction->m_pItem->GetStrProp (P_OutputDir_Sbr, str) != valid)
			str = "";	// failed to get prop, no output dir!

		GroomOutputString(str);
		m_pMakWriter->WriteMacro(_TEXT("CPP_SBRS"), str);
	}
#endif
}

BOOL CUnknownTool::WritePerConfigData(CProjItem * pItem, CActionSlob  * /* pAction */)
{
	ASSERT(g_bExportMakefile);

	BOOL retval = TRUE; // default
	if (g_bExportMakefile)
	{
	//	Write our macro and our flags, build rule(s), etc.

	// UNDONE: write all stored macros, inference/build rules for this
	// tool and m_pItem.
	CString str;

	CProjItem * pTheItem = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;

	BOOL bOldInherit = pTheItem->EnablePropInherit(FALSE);
	if ((retval=(pTheItem->GetStrProp(m_nIDUnkStr, str)==valid)) && (!str.IsEmpty()))
	{
		// FUTURE: broken for Unicode?
		m_pMakWriter->WriteStringEx(LPCTSTR(str));	// write raw data as read
	}

	pTheItem->EnablePropInherit(bOldInherit);
	}
	return retval;
}

//*******************************
// OLE Proxy BUILD TOOL
//*******************************

COLEBuildTool::COLEBuildTool( LPUNKNOWN pUnk, DWORD dwDebug ) : CBuildTool(dwDebug){
	// QuereyInterfaces for
	m_pBuildTool =  (IBuildTool*)pUnk;
	m_pDepScanner = NULL;
	m_pDepManager = NULL;
	// pUnk->QueryInterfacce( IID_IBuildTool, &m_pBuildTool );
	// pUnk->QueryInterfacce( IID_IDepScanner, &m_pDepScanner );
	// pUnk->QueryInterfacce( IID_IDepManager, &m_pDepManager );
};

COLEBuildTool::~COLEBuildTool(){
	// if( m_pBuildTool ) m_pBuildTool->Release();
	// if( m_pDepScanner ) m_pDepScanner->Release();
	// if( m_pDepManager ) m_pDepManager->Release();
}


//*******************************
// IBUILDTOOL
//*******************************
// this tool claims this file if Filter returns true.
BOOL COLEBuildTool::AttachToFile(FileRegHandle frh, CProjItem * pItem){
	if( m_pBuildTool && frh ) {
		LPBUILDFILE pFile = (LPBUILDFILE) frh;
		// LPBUILDFILE pFile = (LPBUILDFILE) frh->GetInterface()
		if(	m_pBuildTool->Filter( pFile ) == S_OK){
			return TRUE;
		}
	}
	return FALSE;
}

// UINT type can be TOB_Build, or TOB_ReBuild
// UINT stage can be TOB_Pre, TOB_Stage, TOB_Post
UINT COLEBuildTool::PerformBuildActions(UINT type, UINT stage, CActionSlobList & lstActions,
									 DWORD attrib, CErrorContext & EC){
	int rval = ACT_Error;
	if( m_pBuildTool ) {

		// let the build tool do any processing it need to.
		LPENUMBSACTIONS pActionList = lstActions.GetInterface();
		// add a second reference  for the Build tool to release.
		pActionList->AddRef();
		HRESULT hr = m_pBuildTool->PerformBuild( type, stage, pActionList, &rval );
		// release our reference and set the action list to invalid
		lstActions.ReleaseInterface();

		// call the base class implementation
		if( hr == S_FALSE ){
			rval = CBuildTool::PerformBuildActions(type, stage, lstActions, attrib, EC);
		}
	}
	return rval;
}

// Get command lines for this tool
BOOL COLEBuildTool::GetCommandLines
	( 
		CActionSlobList & lstActions,
		CStringList & slCommandLines, CStringList & slDescriptions,
		DWORD attrib,
		CErrorContext & EC
	) {

	if( m_pBuildTool ){

		LPENUMBSACTIONS pActionList = lstActions.GetInterface();
		// add a second reference for the Build tool to release.
		pActionList->AddRef();
		// TODO: replace CStringLists with OleStrings.
		m_pBuildTool->GenerateCommandLines( pActionList, slCommandLines, slDescriptions );
		// release our reference and set the action list to invalid
		lstActions.ReleaseInterface();

	}
	return TRUE;
}

BOOL COLEBuildTool::GenerateOutput(UINT type, CActionSlobList & lstActions, CErrorContext & EC){
	if( m_pBuildTool ){

		LPENUMBSACTIONS pActionList = lstActions.GetInterface();
		// add a second reference for the Build tool to release.
		pActionList->AddRef();
		m_pBuildTool->GenerateOutputs( pActionList );
		// release our reference and set the action list to invalid
		lstActions.ReleaseInterface();
	}
	return TRUE;
}

BOOL COLEBuildTool::GetDependencies( ConfigurationRecord* pcr, CStringArray& strArray, BOOL* bUpToDate ) {
	BOOL b = FALSE;
	if( m_pBuildTool && !m_pDepScanner ){
		// NYI.
		/*
		LPENUMBSACTIONS pActionList = lstActions.GetInterface();
		pActionList->AddRef();
		b = m_pBuildTool->GetDependencies( pActionList );
		pActionList->ReleaseInterface();
		*/

	}
	return b;
}


//*******************************
// IDEPSCANNER
//*******************************

// Generate dependencies for 'frhFile' and use CAction::AddScannedDep() or CAction::AddSourceDep()
// to modify the action 'pAction'.
// (errrors should be pushed into the error context EC)
BOOL COLEBuildTool::GenerateDependencies(CActionSlob * pAction, FileRegHandle frh, CErrorContext & EC){
	BOOL b = FALSE;
	if( m_pDepScanner ) {

		LPBSACTION pIAction = pAction->GetInterface();
		pIAction->AddRef();
		b = m_pDepScanner->GenerateDependencies( pIAction );
		pAction->ReleaseInterface();

	}
	return b;
}


UINT DoSpecialCommandLines
( 
	CProjItem* pItem,
	BOOL bPreLink,
	DWORD attrib,
	CErrorContext & EC
)
{
	// generate command-lines for custom-build tool
	CString strFilePath, strCmdLine, strDescription;
	CString strCommand;
	CProject* pProject = pItem->GetProject();
	CPtrList plCmds;

	int idCmds;
	int idDesc;

	if (bPreLink)
	{
		idCmds = P_PreLink_Command;
		idDesc = P_PreLink_Description;
	}
	else
	{
		idCmds = P_PostBuild_Command;
		idDesc = P_PostBuild_Description;
	}

	// Allow inheritance.
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	pItem->SetOptBehaviour(optbehOld | OBInherit);

	if ( !( pItem->GetStrProp(idCmds, strCommand) && !strCommand.IsEmpty() ))
	{
		// Restore original behavior.
		pItem->SetOptBehaviour(optbehOld);
		return CMD_Complete;
	}

	strDescription.Empty();
	pItem->GetStrProp(idDesc, strDescription);

	// Restore original behavior.
	pItem->SetOptBehaviour(optbehOld);


	COptionList lstCmdSpec(_T('\t'), FALSE,TRUE);
	lstCmdSpec.SetString((const TCHAR *)strCommand);
	POSITION pos = lstCmdSpec.GetHeadPosition();

	CString strAllCmdLines;
	strAllCmdLines = _TEXT("@echo off\n");
	while (pos != (POSITION)NULL)
	{
		const TCHAR * pchOutSpec = lstCmdSpec.GetNext(pos);
		CString strCmdLine;

		// construct our command-line
		CActionSlob action(NULL, NULL, FALSE, NULL);
		CActionSlob* pAction = &action;
		pAction->m_pItem = pItem;
		pAction->DirtyInput(FALSE);

		// These initialize some internal data.
		pAction->GetMissingDep();
		pAction->GetScannedDep();
		pAction->GetSourceDep();
		VERIFY(ExpandMacros(&strCmdLine, pchOutSpec, pAction));

		if (!theApp.m_bWin95)
		{
			// on NT, may need to quote the command 
			// for .bat file which has args require quotes
			TCHAR * pch = _tcsstr(strCmdLine, ".bat");
			if (!pch)
			{
				pch = _tcsstr(strCmdLine, ".cmd");
			}
			// assuming batch file
			if (pch)
			{
				TCHAR * pchQuote = _tcschr(pch, '"');
				if (pchQuote)
				{
					// there are quotes in the args, add extra quote
					strCmdLine = _TEXT("\"") + strCmdLine + _TEXT("\"");
				}
			}
		}

		strAllCmdLines += strCmdLine + "\n";
	}
	// append our command-line
	if (!g_buildengine.FormBatchFile(strAllCmdLines, EC))
		return FALSE;	// error

	CCmdLine *pCmdLine = new CCmdLine;
	pCmdLine->nProcessor = 1;
	// append our command-line
	pCmdLine->slCommandLines.AddTail(strAllCmdLines);

	// add our one and only description.
	pCmdLine->slDescriptions.AddTail(strDescription);
	plCmds.AddTail(pCmdLine);
	int cmd;
		
	cmd = g_buildengine.ExecuteCmdLines(plCmds, pProject->GetProjDir(), TRUE, attrib & AOB_IgnoreErrors, EC);

	return cmd;
}

const TCHAR g_pcBeginSpecialBuildTool[] = _TEXT("Begin Special Build Tool");
const TCHAR g_pcEndSpecialBuildTool[] = _TEXT("End Special Build Tool");

class COptHdlrSpecialTool : public COptionHandler
{
	DECLARE_DYNAMIC(COptHdlrSpecialTool);

public:
	COptHdlrSpecialTool(CBuildTool * pBuildTool);

	// create our default option map
	void CreateOptDefMap() {/* do nothing*/}

	// retrieve our 'special' option props, ie. the unknown strins and options prop IDs
	void GetSpecialLogicalOptProps(UINT & nIDUnkOpt, UINT & nIDUnkStr, OptBehaviour optbeh = OBNone)
	{
		nIDUnkOpt = (UINT) -1;
		nIDUnkStr = (UINT)0;
	}

	// retrieive our min. and max. option prop IDs
	void GetMinMaxLogicalOptProps(UINT & nIDMinProp, UINT & nIDMaxProp)
	{
		nIDMinProp = P_PreLink_Description;
		nIDMaxProp = P_PostBuild_Command;
	}

	// case sensitive (though this is ignored as we have ignored options'n' strings)
	__inline const BOOL IsCaseSensitive() {return TRUE;}

	// retrieve our option string table
	// (just has two entries, unknown option and unknown string)
	COptStr * GetOptionStringTable(){ return poptstrOptHdlrUknown; }

	// we don't have an option lookup
	__inline COptionLookup * GetOptionLookup()	{return (COptionLookup *)NULL;}

	// we don't have any option UI!
	__inline CRuntimeClass * GetGeneralOptionPage() {return (CRuntimeClass *)NULL;}

private:
	static COptStr poptstrOptHdlrUknown[];
};

COptStr COptHdlrSpecialTool::poptstrOptHdlrUknown[] = {
	IDOPT_UNKNOWN_STRING,	"",		NO_OPTARGS, single,
	(UINT)-1,				NULL,	NO_OPTARGS, single
};

IMPLEMENT_DYNAMIC(COptHdlrSpecialTool, COptionHandler);

COptHdlrSpecialTool::COptHdlrSpecialTool(CBuildTool * pBuildTool) : COptionHandler(pBuildTool)
{
	// our base
	SetOptPropBase(P_PreLink_Description);

	Initialise();
}	

CSpecialBuildTool::CSpecialBuildTool()
{
#if 0
	// our component name (and UI name)
	m_nIDName = m_nIDUIName = IDS_CUSTOMBUILD_TOOL;
#endif

	// we're not internal!
	m_bToolInternal = FALSE;

	// FUTURE: we assume we not a target tool
	m_fTargetTool = FALSE;

	// FUTURE: clean-this up (from old CSourceTool)
	bForceNoInferenceRule = TRUE;

	// create ourselves a simple option handler
	SetOptionHandler(new COptHdlrSpecialTool(this));
}

CSpecialBuildTool::~CSpecialBuildTool()
{
	// FUTURE: recycle these
	// delete our option handleR
	delete GetOptionHandler();
}

BOOL CSpecialBuildTool::AttachToFile(FileRegHandle frh, CProjItem * pItem)
{
	// attach only if the command string props. are non-empty and pItem is a project
	// or target.
	if ( !(pItem->IsKindOf(RUNTIME_CLASS(CProject)) || pItem->IsKindOf(RUNTIME_CLASS(CTargetItem))))
		return FALSE;

	CString strPreCmds;
	CString strPostCmds;

	// Allow inheritance.
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	pItem->SetOptBehaviour(optbehOld | OBInherit);

	BOOL bAttached = ( pItem->GetStrProp(s_pSpecialBuildToolProp[PreLinkCmdsId], strPreCmds) && !strPreCmds.IsEmpty() )
		|| ( pItem->GetStrProp(s_pSpecialBuildToolProp[PostBuildCmdsId], strPostCmds) && !strPostCmds.IsEmpty() );

	// Restore original behavior.
	pItem->SetOptBehaviour(optbehOld);

	return bAttached;
}

BOOL CSpecialBuildTool::DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude, BOOL bAllConfigs)
{
	CObject * pObject = NULL;	// builder file element

	BOOL fRet = FALSE;	// only success if we find custom build end

    // Are we reading for an unknown platform
    BOOL fUnknown = !pItem->GetProject()->GetProjType()->IsSupported();

    CString strUnknownMacros;   // Raw macro strings for unknown platform

	TRY
	{
	   	while (pObject = g_buildfile.m_pmr->GetNextElement())
 	   	{
			// process this builder file element

			// premature EOF?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
				break;

			// look for the custom build end
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
			{
				CMakComment * pMakComment = (CMakComment *)pObject;

				// get the comment and skip leading whitespace
				TCHAR * pch = pMakComment->m_strText.GetBuffer(1);
				SkipWhite(pch);

				if (_tcsnicmp(g_pcEndSpecialBuildTool, pch, _tcslen(g_pcEndSpecialBuildTool)) == 0)
				{
					fRet = TRUE;
					break;
				}
			}
			// is this the description block?
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				//
				// Not sure what to do.
				ASSERT(0);
			}
			// is this the macros ... we're going to read in 'BuildCmds=' if there is one
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				CMakMacro * pMacro = (CMakMacro *)pObject;
				int idtype;

				if (IsSpecialBuildTool(pMacro->m_strName, idtype))
				{
					// Process special project build tool.
					CString str = pMacro->m_strValue;
					if (!pMacro->m_strValue.IsEmpty())
					{
						if ( bAllConfigs )
						{
							pItem->CreateAllConfigRecords();
							int	nConfigs = pItem->GetPropBagCount();
							int k;

							for (k=0; k < nConfigs; k++)
							{
								// global across *all configs* so
								// make sure we have all of our configs matching the project

								// Force config. active, set prop, and then restore.
								pItem->ForceConfigActive((ConfigurationRecord *)(*pItem->GetConfigArray())[k]);
								pItem->SetStrProp(s_pSpecialBuildToolProp[idtype], pMacro->m_strValue);
								pItem->ForceConfigActive();
							}
						}
						else
						{
							pItem->SetStrProp(s_pSpecialBuildToolProp[idtype], pMacro->m_strValue);
						}
					}
				}
				
				if (fUnknown)
				{
                    CString strMacro = pMacro->m_strName + _TEXT("=") + pMacro->m_strValue + _T("\r\n");

                    if (strUnknownMacros.IsEmpty())
                        strUnknownMacros = strMacro;
                    else
                        strUnknownMacros += strMacro;
				}
			}	

	   		delete pObject; pObject = (CObject *)NULL;
		}

		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	CATCH (CException, e)
	{
		if (pObject != (CObject *)NULL)
			delete pObject;
	}
	END_CATCH

	return fRet;	// ok
}

int	CSpecialBuildTool::BeginWritingSession
(
	CProjItem * pItem,
	CActionSlob * pAction,
	CMakFileWriter * ppw,
	CMakFileWriter * pmw,
	const CDir * pBaseDir,
	CNameMunger * pnm
)
{
	// always write build rule per-config.
	bShouldWriteBuildRulePerConfig = TRUE;

	m_pPrjWriter = ppw;
	m_pMakWriter = pmw;
	m_pBaseDir = pBaseDir;
	m_pNameMunger = pnm;

	// REVIEW
	return ToolUsed | PerConfig | PerConfigInt | PerConfigExt;	// per-config tool used...
}

BOOL CSpecialBuildTool::DoWriteBuildRule(CActionSlob * pAction)
{
	CProjItem * pItem = pAction->Item();
	CString str, strCommand, strOutput;
	BOOL fWroteRule = FALSE;

	// Pre-link
	CString strPreCmds;
	CString strPostCmds;
	CString strDesc;

	strPreCmds.Empty();
	strPostCmds.Empty();

	// Allow inheritance.
	OptBehaviour optbehOld = pItem->GetOptBehaviour();
	pItem->SetOptBehaviour(optbehOld | OBInherit);

	pItem->GetStrProp(s_pSpecialBuildToolProp[PreLinkCmdsId], strPreCmds);
	pItem->GetStrProp(s_pSpecialBuildToolProp[PostBuildCmdsId], strPostCmds);

	if ( !strPreCmds.IsEmpty() || !strPostCmds.IsEmpty() )
	{
		if (g_bWriteProject)
		{
			// Special build tool prelude
 			g_buildfile.m_ppw->WriteComment(g_pcBeginSpecialBuildTool);
		}

		if (pItem->GetProject()->GetProjType()->IsSupported())
		{
			// all are unused initially
			SetMacroIdUsage(FALSE);

			if (ExpandMacros((CString *)NULL, strPreCmds, pAction) &&
				ExpandMacros((CString *)NULL, strPostCmds, pAction)
			   )
			{
				BYTE fMacros[IDMACRO_LAST-IDMACRO_FIRST];
				memset(fMacros, 0, sizeof(BYTE)*(IDMACRO_LAST-IDMACRO_FIRST));
				// write out any macros the custom build rule might need
				for (int idMacro = IDMACRO_FIRST; idMacro <= IDMACRO_LAST; idMacro++)
				{
					// Don't write IDEDIR (it's in the environment).
					// Only write macros that are used.
					// Always write INPUTFILE because it's always used by $(SOURCE)
					if (idMacro != IDMACRO_IDEDIR && GetMacroIdUsage(idMacro)) {
						// set the flag for the macro
						fMacros[idMacro-IDMACRO_FIRST] = TRUE;
					}
				}
				// write out the macros
				// REVIEW: combine into one call!
				if (g_bWriteProject)
					WriteMacros(*(g_buildfile.m_ppw), fMacros, pAction);
				if (g_bExportMakefile)
					WriteMacros(*(g_buildfile.m_pmw), fMacros, pAction);
			}
			
			// need to write alias for the 'SOURCE' macro?
			if (pItem->IsKindOf(RUNTIME_CLASS(CProject)))
			{
				// get the 'Input' macro name
				const TCHAR * pchMacroName;
				VERIFY(MapMacroIdToName(IDMACRO_INPTFILE, pchMacroName));

				CString strMacroValue;
				VERIFY(GetMacroValue(IDMACRO_INPTFILE, strMacroValue, pAction));
				BOOL bQuote = (strMacroValue[0] != _T('\"'));

				CString strMacroName;
				if (bQuote)
					strMacroName += _T('\"');
				strMacroName += _TEXT("$(");
				strMacroName += pchMacroName;
				strMacroName += _TEXT(")");
				if (bQuote)
					strMacroName += _T('\"');

				if (g_bWriteProject)
					g_buildfile.m_ppw->WriteMacro(_TEXT("SOURCE"), strMacroName);
				if (g_bExportMakefile)
					g_buildfile.m_pmw->WriteMacro(_TEXT("SOURCE"), strMacroName);
			}
		}
		else
		{
#if 0
			// Write out macros saved when we read this custom build tool (for
			// an unknown platform)
			CString strMacros;
			GetCustomBuildStrProp(pItem, P_CustomMacros, strMacros);
		
			if (g_bWriteProject)
        			g_buildfile.m_ppw->WriteString(strMacros);
			if (g_bExportMakefile)
        			g_buildfile.m_pmw->WriteString(strMacros);
#endif
		}
	}

	//
	// Only write pre-link command for g_bWriteProject, NOT g_bExportMakefile
	//
	if ( g_bWriteProject && !strPreCmds.IsEmpty() )
	{
		strDesc.Empty();
		pItem->GetStrProp(s_pSpecialBuildToolProp[PreLinkDescId], strDesc);

		if ( !strDesc.IsEmpty() )
		{
			if (g_bWriteProject)
				g_buildfile.m_ppw->WriteMacro(s_ppszSpecialBuildToolMacroName[PreLinkDescId], strDesc);
#if 0
			// REVIEW(tomse): Not sure if this will ever be used.
			if (g_bExportMakefile)
				g_buildfile.m_pmw->WriteMacro(s_ppszSpecialBuildToolMacroName[PreLinkDescId], strDesc);
#endif
		}
		if (g_bWriteProject)
			g_buildfile.m_ppw->WriteMacro(s_ppszSpecialBuildToolMacroName[PreLinkCmdsId], strPreCmds);
#if 0
			// REVIEW(tomse): Not sure if this will ever be used.
		if (g_bExportMakefile)
			g_buildfile.m_pmw->WriteMacro(s_ppszSpecialBuildToolMacroName[PreLinkCmdsId], strPreCmds);
#endif

		fWroteRule = TRUE;
	}

	// Post-build
	if ( !strPostCmds.IsEmpty() )
	{
		strDesc.Empty();
		pItem->GetStrProp(s_pSpecialBuildToolProp[PostBuildDescId], strDesc);

		if ( !strDesc.IsEmpty() )
		{
			if (g_bWriteProject)
				g_buildfile.m_ppw->WriteMacro(s_ppszSpecialBuildToolMacroName[PostBuildDescId], strDesc);
			if (g_bExportMakefile)
				g_buildfile.m_pmw->WriteMacro(s_ppszSpecialBuildToolMacroName[PostBuildDescId], strDesc);
		}
		if (g_bWriteProject)
			g_buildfile.m_ppw->WriteMacro(s_ppszSpecialBuildToolMacroName[PostBuildCmdsId], strPostCmds);
		if (g_bExportMakefile)
#if 0
			// REVIEW(tomse): Not sure if this will ever be used.
			g_buildfile.m_pmw->WriteMacro(s_ppszSpecialBuildToolMacroName[PostBuildCmdsId], strPostCmds);
#else
		{
			CProject* pProject = pItem->GetProject();

#if 0		// Not needed.
			CString strIntDir;
			VERIFY( GetMacroValue(IDMACRO_INTDIR,strIntDir,pAction) );
			g_buildfile.m_pmw->WriteMacro(_T("INTDIR"), strIntDir);
#endif

			// Postbuild dependency file
			g_buildfile.m_pmw->WriteMacro(_T("DS_POSTBUILD_DEP"), _T("$(INTDIR)\\postbld.dep"));

			g_buildfile.m_pmw->WriteDesc(_T("ALL"), _T("$(DS_POSTBUILD_DEP)"));
			g_buildfile.m_pmw->EndLine();

			// Post build rule.
			pProject->WriteMasterDepsLine(*g_buildfile.m_pmw, pProject->GetWorkspaceDir(), TRUE );

			CString strCmd;
			COptionList lstCmdSpec(_T('\t'), FALSE,TRUE);
			lstCmdSpec.SetString((const TCHAR *)strPostCmds);
			POSITION pos = lstCmdSpec.GetHeadPosition();

			while (pos != (POSITION)NULL)
			{
				const TCHAR * pchOutSpec = lstCmdSpec.GetNext(pos);

				// construct our command-line
				CString strCmdLine;
				CActionSlob action(NULL, NULL, FALSE, NULL);
				CActionSlob* pAction = &action;
				pAction->m_pItem = pItem;
				pAction->DirtyInput(FALSE);

				// These initialize some internal data.
				pAction->GetMissingDep();
				pAction->GetScannedDep();
				pAction->GetSourceDep();

				VERIFY(ExpandMacros(&strCmdLine, pchOutSpec, pAction));

				strCmd += strCmdLine;
				strCmd += _T("\r\n\t");
			}

			// The post buu
			strCmd += _T("echo Helper for Post-build step > \"$(DS_POSTBUILD_DEP)\"");

			g_buildfile.m_pmw->Indent();
			g_buildfile.m_pmw->WriteString(strCmd);
			g_buildfile.m_pmw->EndLine();
			g_buildfile.m_pmw->EndLine();
		}
#endif

		fWroteRule = TRUE;
	}

	if (fWroteRule)
	{
		// custom build end
		if (g_bWriteProject)
	 		g_buildfile.m_ppw->WriteComment(g_pcEndSpecialBuildTool);
	}

	// Restore original behavior.
	pItem->SetOptBehaviour(optbehOld);

	return fWroteRule;	// success
}

BOOL CSpecialBuildTool::WriteMacros(CMakFileWriter& mw, BYTE aMacros[], CActionSlob* pAction)
{
	// the array, aMacros[], contains a flag for each macro.
	// if the flag is set, write out the macro.
	// assume the array contains an entry for each macro.
	int nMacros = IDMACRO_LAST - IDMACRO_FIRST;
	for (int i = 0; i < nMacros; i++) {
		if (aMacros[i]) {
			int idMacro = IDMACRO_FIRST + i;
			// get the macro name
			const TCHAR* pchMacroName;
			VERIFY(MapMacroIdToName(idMacro, pchMacroName));

			// get the macro value
			CString strMacroValue;
			if (idMacro != IDMACRO_WKSPDIR)
				VERIFY(GetMacroValue(idMacro, strMacroValue, pAction));
			else
				strMacroValue = _TEXT(".");         // write out workspace dir relative to current dir

			// write this to the build file
			mw.WriteMacro(pchMacroName, strMacroValue);
		}
	}
	return TRUE;
}

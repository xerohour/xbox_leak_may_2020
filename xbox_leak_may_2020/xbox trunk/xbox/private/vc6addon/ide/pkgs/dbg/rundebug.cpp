#include "stdafx.h"
#pragma hdrstop
#define _XBDM_
#define XbeGetLocalModulePathA HrXbeGetLocalModulePath
#include <xboxdbg.h>

#define MAX_ATOM_LEN	1024

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// DLLGridInfoList contains a list of pointers to GRIDINFOs
//	(one for each build target)
// Each GRIDINFO contains an HTARGET and a list of pointers to DLLRECs
//	(one for each grid entry)
CPtrList DLLGridInfoList;


/****************************************************************************

	FUNCTION:	DLLRecListFromTarget()

	PURPOSE:	Find the list of DLLRec's associated with a given target.

	INPUT:		The given target.

	OUTPUT:		A pointer to the CPtrList containing the targets DLLRec's.

****************************************************************************/
static CPtrList *DLLRecListFromTarget(HTARGET hTarget)
{
	POSITION posGridList;
	GRIDINFO *pGridInfo;

	posGridList = DLLGridInfoList.GetHeadPosition();

	// For every build target...
	while (posGridList != NULL)
	{
		pGridInfo = (GRIDINFO *)DLLGridInfoList.GetNext(posGridList);

		// ...look for a matching hTarget
		if (pGridInfo->hTarget == hTarget)
		{
			return &pGridInfo->DLLRecList;
		}
	}

	// For some reason, we didn't know about this target, so create it
	// (probably a corrupt or missing vcp file)
	pGridInfo = new GRIDINFO;
	pGridInfo->hTarget = hTarget;
	DLLGridInfoList.AddTail(pGridInfo);
	return &pGridInfo->DLLRecList;
}


/****************************************************************************

	FUNCTION:	BuildLocalDLLsStringList

	PURPOSE:	Build a CString of the local DLLs from DLLGridInfoList.

	INPUT:		fPreloadOnly - if TRUE we only put the DLLs marked as preload
				into the string.

	OUTPUT:		String containing the list of local DLLs, separated by spaces

****************************************************************************/
void BuildLocalDLLsStringList(CStringList& slLocalDLLs, BOOL fPreloadOnly)
{
	DLLREC *pDLLRec;
	CPtrList *pDLLRecList;
	POSITION posDLLRecList;
	HBLDTARGET hTarget;

	ASSERT(gpIBldSys);

	gpIBldSys->GetActiveTarget(ACTIVE_BUILDER, &hTarget);
	pDLLRecList = DLLRecListFromTarget((HTARGET)hTarget);
	posDLLRecList = pDLLRecList->GetHeadPosition();

	slLocalDLLs.RemoveAll();
	while (posDLLRecList != NULL)
    {
		pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);

		// If we are debugging, there may be deleted DLLs still around
		if (pDLLRec->fDelete)
			continue;

		// Filter non-preload DLLs if so requested
		if ( !pDLLRec->strLocalName.IsEmpty() && ( !fPreloadOnly || pDLLRec->fPreload ) ) {
			slLocalDLLs.AddTail(pDLLRec->strLocalName);
		}
	}
}


BOOL FCreateDLLMappingsKey(CString strMapKeyName, UINT nCurrentPlatform)
{
	typedef struct {
		PCHAR szRemote;
		PCHAR szLocal;
	} DLLMAP;

	static const DLLMAP rgMacPPCMaps[] =
	{
		// Mapped MS DLLs
		{"MicrosoftMFC40Lib",					"mfc40p.dll"},
		{"MicrosoftMFC40DLib",					"mfc40pd.dll"},

		{"MicrosoftMFC42Lib",					"mfc42p.dll"},
		{"MicrosoftMFC42DLib",					"mfc42pd.dll"},

		{"MicrosoftMFCD40Lib",					"mfcd40p.dll"},
		{"MicrosoftMFCD40DLib",					"mfcd40pd.dll"},

		{"MicrosoftMFCD42Lib",					"mfcd42p.dll"},
		{"MicrosoftMFCD42DLib",					"mfcd42pd.dll"},

		{"MicrosoftMFCO40Lib",					"mfco40p.dll"},
		{"MicrosoftMFCO40DLib",					"mfco40pd.dll"},

		{"MicrosoftMFCO42Lib",					"mfco42p.dll"},
		{"MicrosoftMFCO42DLib",					"mfco42pd.dll"},

		{"MicrosoftCRT4.0Library",				"msvcrt40.dll"},
		{"DebugCRT4.0Library",					"msvcr40d.dll"},

		// Unmapped MS DLLs
		{"MicrosoftControlsLib",				""},
		{"DebugMicrosoftControlsLib",			""},

		{"MicrosoftOLEPortabilityLib",			""},
		{"DebugMicrosoftOLEPortabilityLib",		""},

		{"MicrosoftOLEUIPortabilityLib",		""},
		{"DebugMicrosoftOLEUIPortabilityLib",	""},

		{"Microsoft_OLE2",						""},
		{"MicrosoftOLE2AutomationLib",			""},

		{"MicrosoftPortabilityLib",				""},
		{"DebugMicrosoftPortabilityLib",		""},

		{"MicrosoftRichEditLib",				""},
		{"DebugMicrosoftRichEditLib",			""},

		// ODBC fragments
		{"vsi:ODBC$ConfigMgr",					""},
		{"vsi:ODBC$CursorLibrary",				""},
		{"vsi:ODBC$DriverMgr",					""},
		{"vsi:ODBC$NetLibADSP",					""},
		{"vsi:ODBC$NetLibTCPIP",				""},
		{"vsi:ODBC$SQLServer",					""},

		// New system DLLs not filtered by DM
		// Keep these alphabetical so individual items remain easy to find
		// as more and more are added.
		{"AOCELib",								""},
		{"ColorPickerLib",						""},
		{"DragLib",								""},
		{"MathLib",								""},
		{"ObjectSupportLib",					""},
		{"StdCLib",								""},
		{"ThreadsLib",							""},
		{"ThreadsLib.1248",						""},
		{"Translation",							""},
	};

#define nMacPPCMaps (sizeof(rgMacPPCMaps) / sizeof(DLLMAP))
	
	const DLLMAP *pDLLMap;
	int nMaps;
		
	switch (nCurrentPlatform)
	{
		case (macppc):
			pDLLMap = rgMacPPCMaps;
			nMaps = nMacPPCMaps;
			break;

		default:
			ASSERT (FALSE);
			return FALSE;
			break;
	}

	TCHAR szFullKeyName[_MAX_PATH];
	TCHAR sz[20];

	for (int i=0; i < nMaps; i++)
	{
		sprintf(sz, "Mapping%d", i+1);
		_ftcscpy(szFullKeyName, strMapKeyName);
		_ftcscat(szFullKeyName, sz);
		if ( !theApp.WriteProfileString(szFullKeyName, _T("Remote Name"), rgMacPPCMaps[i].szRemote) ) {
			return FALSE;
		}
		if ( !theApp.WriteProfileString(szFullKeyName, _T("Local Name"), rgMacPPCMaps[i].szLocal) ) {
			return FALSE;
		}

	}

	return TRUE;
}

/****************************************************************************

	FUNCTION:	FGetLocalDLLFromRemoteDLL
	
	PURPOSE:	Search the appropriate DLLRecList to see if we have an entry
				for the given remote DLL name. If not found in DLLRecList,
				check the registry mappings.

	INPUT:		strRemote is the remote DLL we are trying to find a match for.

	OUTPUT:		If an entry is found for strRemote, strLocal will contain
				the associated local name, including the FULL PATH.
				NOTE: this string may be empty even though a remote name was
				found, which means the user doesn't want to be prompted
				anymore for this DLL's local file.

	RETURNS:	TRUE if the remote name was found.  NOTE: you shouldn't
				use strLocal.IsEmpty to see if an entry exists.  See above
				explanation.

****************************************************************************/
BOOL FGetLocalDLLFromRemoteDLL(CString& strLocal, CString strRemote)
{
	CString strT;

	strLocal.Empty();

	// During JIT session, we may not have project info
	if ( gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK )
	{
		DLLREC *pDLLRec;
		CPtrList *pDLLRecList;
		POSITION posDLLRecList;
		HBLDTARGET hTarget;

		ASSERT(gpIBldSys);

		gpIBldSys->GetActiveTarget(ACTIVE_BUILDER, &hTarget);
		pDLLRecList = DLLRecListFromTarget((HTARGET)hTarget);
		posDLLRecList = pDLLRecList->GetHeadPosition();

		while (posDLLRecList != NULL)
        {
			pDLLRec = (DLLREC*) pDLLRecList->GetNext(posDLLRecList);
			
			if (strRemote.CompareNoCase(pDLLRec->strRemoteName) == 0)
			{

				strT = pDLLRec->strLocalName;

				// If we started debugging, this mapping must always exist
				// (even if the grid entry is deleted, our internal mapping
				// should will linger until the end of the debug session).

				if (strT.IsEmpty()) {
					strLocal.Empty();
				} else {
					VERIFY (FFindDLL (strT, strLocal, TRUE));
				}
				
				return TRUE;
			}
		}
	}

	// If we're debugging xbox, see if we can pluck the mapping out of the
	// xbe -- but we won't return the EXE
	if(pDebugCurr->GetPlatform() == xbox && lpprcCurr) {
		char szPath[MAX_PATH + 1];
		char szPathExe[MAX_PATH + 1];

		HRESULT hr = HrXbeGetLocalModulePath(lpprcCurr->szName, strRemote,
			szPath, sizeof szPath);
		if(SUCCEEDED(hr)) {
			hr = HrXbeGetLocalModulePath(lpprcCurr->szName, NULL, szPathExe,
				sizeof szPathExe);
			if(FAILED(hr) || _tcsicmp(szPath, szPathExe)) {
				strLocal = szPath;
				return TRUE;
			}
		}
	}

	//
	// Not found in DLL info.  Check the registry.
	//

	// Currently, only macppc stores remote mappings in the registry
	
	if (pDebugCurr->GetPlatform() != macppc) {
		return FALSE;
	}

	// First get a ptr to the current CPlatform
	UINT nCurrentPlatform = pDebugCurr->GetPlatform();
	uniq_platform upPlatform = unknown_platform;
	HKEY hkey;
	CString strRegRemoteName;
	CString strBaseKeyName;
	TCHAR szFullKeyName[_MAX_PATH];
	TCHAR sz[20];

	gpIBldPlatforms->InitPlatformEnum();

	while (SUCCEEDED(gpIBldPlatforms->NextPlatform(&upPlatform)))
	{
		if (upPlatform == (uniq_platform)nCurrentPlatform)
		{
			// We have found the current platform
			ASSERT(gpIBldPlatforms->IsPlatformSupported(upPlatform) == S_OK);
			break;
		}
	}

	// We better have found the current platform...
	ASSERT(upPlatform == (uniq_platform)nCurrentPlatform);

	// see if "Remote DLL Mappings" exists.  If not, create it.
	const TCHAR *szKey;

	gpIBldPlatforms->GetPlatformCompRegKeyName(upPlatform, TRUE, &szKey);
	strBaseKeyName = szKey;
	strBaseKeyName += _T("Remote DLL Mappings");	// Do not localize;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, strBaseKeyName, 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
		gpIBldPlatforms->GetPlatformCompRegKeyName(upPlatform, FALSE, &szKey);
		strBaseKeyName = szKey;
		strBaseKeyName += _T("Remote DLL Mappings\\");

		if (!FCreateDLLMappingsKey(strBaseKeyName, nCurrentPlatform))
        {
			RegCloseKey(hkey);
			return FALSE;
		}
	}

	RegCloseKey(hkey);

	gpIBldPlatforms->GetPlatformCompRegKeyName(upPlatform, FALSE, &szKey);
	strBaseKeyName = szKey;
	strBaseKeyName += _T("Remote DLL Mappings\\");

	for (int i=1; ; i++)
	{
		// FUTURE - put "Mapping%d" into .rc file
		sprintf(sz, "Mapping%d", i);
		_ftcscpy(szFullKeyName, strBaseKeyName);
		_ftcscat(szFullKeyName, sz);
		strRegRemoteName = theApp.GetProfileString(szFullKeyName, _T("Remote Name"));
		if (strRegRemoteName.IsEmpty()) {
			return FALSE;
		}
		if (strRemote.CompareNoCase(strRegRemoteName) == 0) {
			strT = theApp.GetProfileString(szFullKeyName, _T("Local Name"));
			if (strT.IsEmpty()) {
				strLocal.Empty();
				return TRUE;
			}
			if (FFindDLL(strT, strLocal, TRUE)) {
				return TRUE;
			} else {
				return FALSE;
			}
		}
	}

	return FALSE;
}


/****************************************************************************

	FUNCTION:	FGetRemoteDLLFromLocalDLL

	PURPOSE:	Search the appropriate DLLRecList to see if we have an entry
				for the given local DLL name.

	INPUT:		strLocal       - the local DLL we are trying to find a match
								 for.

	OUTPUT:		If an entry is found for strLocal, strRemote will contain
				the associated remote name.

	RETURNS:	TRUE if the remote name was found.  NOTE: you shouldn't
				use strLocal.IsEmpty to see if an entry exists.  See above
				explanation.

****************************************************************************/
BOOL
FGetRemoteDLLFromLocalDLL(
	CString 	strLocal,
	CString& 	strRemote
	)
{
	CString strLocalNoPath;
	DLLREC *pDLLRec;
	CPtrList *pDLLRecList;
	POSITION posDLLRecList;

	strRemote.Empty();

    // Are we trying to find the local name of the exe?
    // If we couldn't create a project, the exe mapping will be stored with
    // the DLL mappings
	
    if ((strLocal.CompareNoCase(ExecutablePath) == 0) &&
         gpIBldSys &&
        (gpIBldSys->IsActiveBuilderValid() == S_OK))
    {
		gpIBldSys->GetRemoteTargetFileName(ACTIVE_BUILDER, strRemote);

		// ppc and 68k only send module name, should be sending the full path
		if ((pDebugCurr->GetPlatform() == macppc) || (pDebugCurr->GetPlatform() == mac68k))
			strRemote = LtszRemoteFileFromPath(strRemote);

		return TRUE;
	}

#ifdef _DEBUG
	// The incoming local name must always include a full path (possibly UNC)
	_splitpath(strLocal, szDrive, szDir, szFName, szExt);
	ASSERT((*szDrive != '\0' && *szDir != '\0') ||
			(strLocal[0] == '\\' && strLocal[1] == '\\'));
#endif

	// NOTE:
	// o  strLocal will always specify a full path.
	// o  The local name in the grid may or may not have a full path
	// o  The local name in the registry may or may not have a full path

	// Get the filename from the full path
	_splitpath(strLocal, szDrive, szDir, szFName, szExt);
	strLocalNoPath = szFName;
	strLocalNoPath += szExt;


	// First look for this local name in the dll grid.  Compare using both
	// the full path and also just the filename of the given file since the
	// grid may contain either form.
	
	ASSERT(gpIBldSys);

	HBLDTARGET hTarget;

	gpIBldSys->GetActiveTarget(ACTIVE_BUILDER, &hTarget);
	pDLLRecList = DLLRecListFromTarget((HTARGET)hTarget);
	posDLLRecList = pDLLRecList->GetHeadPosition();

	while (posDLLRecList != NULL)
    {
		pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);

		if (strLocal.CompareNoCase(pDLLRec->strLocalName) == 0 ||
			strLocalNoPath.CompareNoCase(pDLLRec->strLocalName) == 0)
		{
			strRemote = pDLLRec->strRemoteName;
			return TRUE;
		}
	}

	// Now look for this local name in the registry mappings.  Compare using both
	// the full path and also just the filename of the given file since the
	// registry may contain either form.

	// Currently, only macppc stores remote mappings in the registry
	if (pDebugCurr->GetPlatform() != macppc) {
		return FALSE;
	}


	UINT nCurrentPlatform = pDebugCurr->GetPlatform();
	CString strRegRemoteName;
	CString strRegLocalName;
	CString strBaseKeyName;
	uniq_platform upPlatform = unknown_platform;
	TCHAR szFullKeyName[_MAX_PATH];
	TCHAR sz[20];

	// First get a ptr to the current CPlatform
	gpIBldPlatforms->InitPlatformEnum();
	while (SUCCEEDED(gpIBldPlatforms->NextPlatform(&upPlatform)))
	{
		if (upPlatform == (uniq_platform)nCurrentPlatform)
		{
			// We have found the current platform
			ASSERT(gpIBldPlatforms->IsPlatformSupported(upPlatform) == S_OK);
			break;
		}
	}

	// We better have found the current platform...
	ASSERT(upPlatform == (uniq_platform)nCurrentPlatform);

	const TCHAR *szKey;

	gpIBldPlatforms->GetPlatformCompRegKeyName(upPlatform, FALSE, &szKey);
	strBaseKeyName = szKey;
	strBaseKeyName += _T("Remote DLL Mappings\\");

	for (int i=1; ; i++)
	{
		// FUTURE - put "Mapping%d" into .rc file
		sprintf(sz, "Mapping%d", i);
		_ftcscpy(szFullKeyName, strBaseKeyName);
		_ftcscat(szFullKeyName, sz);

		// Make sure we haven't reached the end of the mappings
		strRegRemoteName = theApp.GetProfileString(szFullKeyName, _T("Remote Name"));
		if (strRegRemoteName.IsEmpty()) {
			return FALSE;
		}

		// Now see if the local names match
		strRegLocalName = theApp.GetProfileString(szFullKeyName, _T("Local Name"));

		if (strLocal.CompareNoCase(strRegLocalName) == 0 ||
			strLocalNoPath.CompareNoCase(strRegLocalName) == 0)
		{
			strRemote = strRegRemoteName;
			return TRUE;
		}
	}

	return FALSE;
}


/****************************************************************************

	FUNCTION:	ClearDLLInfo

	PURPOSE:	Delete all records from DLLGridInfoList, or from just
				one target if a target is specified.

****************************************************************************/
void ClearDLLInfo(HTARGET hTarget)
{
	// Clear info for a specific target, or for all targets?
	if (hTarget)
	{
		CPtrList *pDLLRecList = DLLRecListFromTarget(hTarget);

		// ...free mem for every DLL record
		while ( !pDLLRecList->IsEmpty() ) {
			DLLREC *pDLLRec = (DLLREC *)pDLLRecList->RemoveTail();
			delete pDLLRec;
		}
	}
	else
	{
		GRIDINFO *pGridInfo;
		CPtrList *pDLLRecList;

		// For every build target...
		while (!DLLGridInfoList.IsEmpty())
		{
			pGridInfo = (GRIDINFO *)DLLGridInfoList.RemoveTail();
			pDLLRecList = &pGridInfo->DLLRecList;

			// ...free mem for every DLL record
			while ( !pDLLRecList->IsEmpty() ) {
				DLLREC *pDLLRec = (DLLREC *)pDLLRecList->RemoveTail();
				delete pDLLRec;
			}

			delete pGridInfo;
		}

	}
}


/****************************************************************************

	FUNCTION:	AddRecToDLLInfo

	PURPOSE:	Add a record to the DLLInfo list.

	INPUT:		strLocal - the local name of the new entry
				strRemote - the name of the drummer for Rush.  Just kidding.
				fPreload - do we preload symbols for this DLL
				hTarget - specifies the build target grid to which we are
					adding the DLL
							
	RETURNS:	absolutely nothing

****************************************************************************/
void AddRecToDLLInfo(CString strLocal, CString strRemote, BOOL fPreload, HTARGET hTarget)
{
	DLLREC *pDLLRec = new DLLREC;
	CPtrList *pDLLRecList = DLLRecListFromTarget(hTarget);

	pDLLRec->fPreload = fPreload;
	pDLLRec->fDelete = FALSE;
	pDLLRec->strLocalName = strLocal;
	pDLLRec->strRemoteName = strRemote;
	pDLLRecList->AddTail(pDLLRec);
}


void
UpdateDLLInfoRec(
	CString	strLocal,
	CString	strRemote,
	BOOL	fPreLoad,
	HTARGET	hTarget,
	BOOL	fSearchOnLocal
	)
/*++

Routine Description:

	This routine is much like AddRecToDLLInfo except that it attempts to
	UPDATE the information before blindly adding it.  This is important in
	several circumstances.

Arguments:

	fSearchOnLocal - TRUE means that we should search using strLocal as the
					 key.  FALSE means search using strRemote as the key.

	fPreLoad - The fPreLoad argument is only used if this is NOT and update;
			   if this is an update, the fPreLoad field is unchanged.

	All other args are as in AddRecToDLLInfo.

Comments:

	If we're searching on the local string, allow it to match on either a
	full path match or only a image-name, extension match.  If we're
	searching on remote, match only on complete match.  This is how we
	search elsewhere.

--*/
{
	CPtrList*	list = DLLRecListFromTarget (hTarget);
	POSITION	cur;
	BOOL		fFound = FALSE;
	CString		strShortLocal;
	TCHAR		FileName [_MAX_FNAME + 1];
	TCHAR		Ext [_MAX_EXT + 1];
	DLLREC*		entry = NULL;

	_tsplitpath (strLocal, NULL, NULL, FileName, Ext);

	strShortLocal = FileName;
	strShortLocal += Ext;

	cur = list->GetHeadPosition ();
	
	while (!fFound && cur)
    {
		entry = (DLLREC*) list->GetNext (cur);

		if (fSearchOnLocal)
		{
			if (strLocal.CompareNoCase (entry->strLocalName) == 0 ||
				strShortLocal.CompareNoCase (entry->strLocalName) == 0)
			{
				fFound = TRUE;
			}
		}
		else
		{
			if (strRemote.CompareNoCase (entry->strRemoteName) == 0)
			{
				fFound = TRUE;
			}
		}
	}


	if (fFound)
	{
		ASSERT (entry);

		entry->strLocalName = strLocal;
		entry->strRemoteName = strRemote;
	}
	else
	{
		AddRecToDLLInfo (strLocal, strRemote, fPreLoad, hTarget);
	}
}

		

	

	
	

/****************************************************************************

	FUNCTION:	PurgeDLLInfo

	PURPOSE:	Remove any records marked for deletion during debugging.

****************************************************************************/
void PurgeDLLInfo(void)
{
	POSITION posGridList;
	POSITION posDLLRecList;
	GRIDINFO *pGridInfo;
	CPtrList *pDLLRecList;
	DLLREC *pDLLRec;

	posGridList = DLLGridInfoList.GetHeadPosition();

	// For every build target...
	while (posGridList != NULL)
	{
		pGridInfo = (GRIDINFO *)DLLGridInfoList.GetNext(posGridList);
		pDLLRecList = &pGridInfo->DLLRecList;

		posDLLRecList = pDLLRecList->GetHeadPosition();

		// remove any DLL records marked for deletion
		while (posDLLRecList != NULL) {
			POSITION posCur = posDLLRecList;
			pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);
			if (pDLLRec->fDelete) {
				pDLLRecList->RemoveAt(posCur);
				delete pDLLRec;
			}
		}
	}
}


/****************************************************************************

	FUNCTION:	GetDLLCount()

	PURPOSE:	The bld package calls GetDLLCount to determine how much
				memory to alloc for the rgDLLRec that will be initialized in
				InitDLLGrid().

	INPUT:		hTarget: specifies a certain build target's grid

	RETURNS:	count of DLLs

****************************************************************************/
IDE_EXPORT int GetDLLCount(HTARGET hTarget)
{
	int i=0;
	DLLREC *pDLLRec;
	CPtrList *pDLLRecList = DLLRecListFromTarget(hTarget);
	POSITION posDLLRecList = pDLLRecList->GetHeadPosition();

	while (posDLLRecList != NULL) {
		pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);
		if (!pDLLRec->fDelete) {
			i++;
		}
	}
	// If debuggee isn't running, no records should be marked for deletion
	// (they should have all been purged)
	ASSERT(DebuggeeAlive() || i == pDLLRecList->GetCount());
	return i;
}


/****************************************************************************

	FUNCTION:	InitDLLList()

	PURPOSE:	The bld package calls InitDLLList to initialize the
				"Additional DLLs" DLL list (after calling GetDLLCount() to
				determine how much mem to allocate at rgDLLRec).

	INPUT:		rgDLLRec: the memory allocated after calling GetDLLCount().

	RETURNS:	nothing

****************************************************************************/
void InitDLLList(HTARGET hTarget, CPtrList &listDLLs)
{
	DLLREC *pDLLRec;
	CPtrList *pDLLRecList = DLLRecListFromTarget(hTarget);
	POSITION posDLLRecList = pDLLRecList->GetHeadPosition();

	while (posDLLRecList != NULL)
	{
		pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);
		if (!pDLLRec->fDelete)
			listDLLs.AddTail(pDLLRec);
		else
			ASSERT (DebuggeeAlive());
	}
}


/****************************************************************************

	FUNCTION:	SaveDLLList()

	PURPOSE:	The bld package calls SaveDLLList to save away the info
				entered in the "Additional DLLs" grid.

	INPUT:		A list of DLLRECs.

	RETURNS:	nothing

****************************************************************************/
void SaveDLLList(HTARGET hTarget, CPtrList &listDLLs)
{
	int cDLLs;
	DLLREC *pDLLRec;
	CPtrList *pDLLRecList = DLLRecListFromTarget(hTarget);

	cDLLs = listDLLs.GetCount();

	// First update DLLInfo
	ASSERT(gpIBldSys);

	HBLDTARGET hActiveTarget;
	gpIBldSys->GetActiveTarget(ACTIVE_BUILDER, &hActiveTarget);

	if (!DebuggeeAlive() || hTarget != (HTARGET)hActiveTarget)
	{
		// Easy case - just create a new DLL list	
		ClearDLLInfo(hTarget);
	}
	else
	{
		// Not as easy.  We need to keep the old mappings around for when we
		// call UnRegisterEmi (we'll need the remote name)

		// Check to see if the user has removed any DLLs
		POSITION posDLLRecList = pDLLRecList->GetHeadPosition();
		POSITION posCur;

		// For every local DLL in the old list...
		while (posDLLRecList != NULL)
		{
			posCur = posDLLRecList;
			pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);
			// If it doesn't have a local name, we don't care about it
			if (pDLLRec->strLocalName.IsEmpty())
			{
				pDLLRecList->RemoveAt(posCur);
				delete pDLLRec;
				continue;
			}
			// Mark all DLLRECs with a local name for deletion.  If we find a
			// new record that has the same local name, unmark it for deletion.
			pDLLRec->fDelete = TRUE;

			// ...is there a match in the new list?
			POSITION pos = listDLLs.GetHeadPosition();
			while (pos != NULL)
			{
				DLLREC *pSaveDLLRec = (DLLREC *)listDLLs.GetNext(pos);

				if (pSaveDLLRec->strLocalName == pDLLRec->strLocalName)
				{
					pDLLRec->fDelete = FALSE;		// use the old mapping
					pSaveDLLRec->fDelete = TRUE;	// don't use the new mapping
					break;
				}
			}
		}
	}

	// Add new (non-duplicate) mappings
	POSITION pos = listDLLs.GetHeadPosition();
	while (pos != NULL)
	{
		DLLREC *pSaveDLLRec = (DLLREC *)listDLLs.GetNext(pos);
		
		if (!pSaveDLLRec->fDelete)
		{
			pDLLRec = new DLLREC;
			pDLLRec->fPreload = pSaveDLLRec->fPreload;

			pDLLRec->strLocalName = pSaveDLLRec->strLocalName;
			pDLLRec->strRemoteName = pSaveDLLRec->strRemoteName;
			pDLLRec->fDelete = FALSE;
			pDLLRecList->AddTail(pDLLRec);
		}
	}

	// If debugging, load symbols for any new DLLs in the active target
	if (DebuggeeAlive() && hTarget == (HTARGET)hActiveTarget)
	{
		// Put up hourglass (the constructor does it)
		CWaitCursor	wc;

		CStringList sl;
		// include ALL DLLs (not just preload)
		BuildLocalDLLsStringList(sl, FALSE);
		// NOTE: the fQuiet flag MUST be TRUE here!  Otherwise, a dialog may
		// appear which, when dismissed, will set focus back to the project
		// settings dialog, which will cause a depency to be added to the CProxySlob
		// that will never be removed (because this func is being called during
		// OnOK processing which has already removed the dependency).  Then we
		// will crash in InformDependents.
		LoadAdditionalDLLs(&sl, FALSE, TRUE);		
	}
}

/****************************************************************************

	FUNCTION:	DiffDlls()

	PURPOSE:	If the user brings up the Options.Debug dialog while the
				debuggee is already running, we want to load symbols for
				any DLLs he has added to the list.  Note, we do NOT unload
				symbols for any DLLs that he has removed from the list; but
				we do warn him about this.

	INPUT:		szNewDLLs = new list of DLLs
				pichBegin = pointer where, if a DLL load fails, the starting
					offset of the DLL name that caused the failure will be
					stored.
				pichEnd = pointer, goes with pichBegin.  Will point just
					after last character of DLL name.

	RETURNS:	pidMsg = when not -1 this is a message concerning recent diff.
				TRUE for success.

****************************************************************************/

BOOL DiffDlls(HTARGET hTarget, CPtrList &listDLLs, int *piRow, UINT *pidMsg)
{
	CString strNewLocalDLLs;
	int i = 0;

	*pidMsg = (UINT)-1;	// default is *no* message
	*piRow = 0;

	// Validate all local DLLs listed
	POSITION pos = listDLLs.GetHeadPosition();
	while (pos != NULL)
	{
		DLLREC *pDiffDLLRec = (DLLREC *)listDLLs.GetNext(pos);
		if (!pDiffDLLRec->strLocalName.IsEmpty())
		{
			CString strLocalPath;
			// Can't call CheckEXEForDebug here because debuggee might not
			// be running
			if (!FFindDLL(pDiffDLLRec->strLocalName, strLocalPath))
			{
				*piRow = i;
				return FALSE;
			}
		}

		i++;
	}

	// If we aren't debugging, we don't have to worry about warning
	// that symbols will not be unloaded from memory.		
	if (!DebuggeeAlive())
		return TRUE;

	// Check to see if the user has removed any DLLs
	DLLREC *pDLLRec;
	CPtrList *pDLLRecList = DLLRecListFromTarget(hTarget);
	POSITION posDLLRecList = pDLLRecList->GetHeadPosition();
	BOOL fFound;

	// For every local DLL in the old list...
	while ( posDLLRecList != NULL )
	{
		pDLLRec = (DLLREC *)pDLLRecList->GetNext(posDLLRecList);
		if (pDLLRec->strLocalName.IsEmpty())
		{
			continue;
		}
		fFound = FALSE;

		// ...is there a match in the new list?
		POSITION pos = listDLLs.GetHeadPosition();
		while (pos != NULL)
		{
			DLLREC *pDiffDLLRec = (DLLREC *)listDLLs.GetNext(pos);
			if (pDiffDLLRec->strLocalName == pDLLRec->strLocalName)
			{
				fFound = TRUE;
				break;
			}
		}
		if (!fFound)
		{
			*pidMsg = IDS_DllsNotUnloaded;
			break;
		}
	}
	return TRUE;
}


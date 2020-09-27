
// Common Compiler Tool
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "toolcplr.h"	// our local header file
#include "mrdepend.h"

#include <winver.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern int g_nBuildProcessors;

IMPLEMENT_DYNAMIC(CCCompilerTool, CBuildTool)
IMPLEMENT_DYNAMIC(CCCompilerNTTool, CCCompilerTool)

CCCompilerTool::CCCompilerTool() : CBuildTool()
{
	m_strToolExeName = _TEXT("cl.exe");
	m_strToolInput = _TEXT("*.c;*.cpp;*.cxx");
	m_strToolPrefix = _TEXT("CPP");
	m_MrSupportLevel = None;
	m_nOrder = 3;
}

///////////////////////////////////////////////////////////////////////////////
// Perform build action(s)
UINT CCCompilerTool::PerformBuildActions(UINT type, UINT stage, CActionSlobList & lstActions,
									  DWORD aob, CErrorContext & EC)
{
	UINT act = ACT_Complete;

	// what stage is this?
	switch (stage)
	{
		// pre?
		case TOB_Pre:
		{
			// need to rebuild?

			if (type == TOB_Clean)
			{
				// delete our .pch on re-build
				// Only deletes if AutoPch. In that case, we need to walk all the files to be compiled,
				// possibly deleting a different header for each file. In the standard case where you have
				// one file that creates the pch and others use it, we don;t need to delete; compiling the
				// file that creates it will rebuild the pch file. bobz 9/5/95

				CActionSlob * pAction;
				POSITION pos = lstActions.GetHeadPosition();
				while (pos != (POSITION)NULL && (act == ACT_Complete) /* failure */)
				{
					pAction = (CActionSlob *)lstActions.GetNext(pos);

					FileRegHandle frh = NULL;
					if (GetItemPCHFileName(pAction->Item(), frh))
					{
						g_buildengine.DeleteFile(frh, EC);
						frh->ReleaseFRHRef();
					}
				}
			}
			else if( type == TOB_Build )
			{
				// delete the .obj file if we think we should have a .pdb and we don't
				CActionSlob * pAction;
				POSITION pos = lstActions.GetHeadPosition();
				while (pos != (POSITION)NULL && (act == ACT_Complete) /* failure */)
				{
					pAction = (CActionSlob *)lstActions.GetNext(pos);
					CFileRegSet * pfrs = pAction->GetOutput();
					FileRegHandle frh;

					pfrs->InitFrhEnum();
					while ((frh = pfrs->NextFrh()) != (FileRegHandle)NULL)
					{
						const CPath * pPath = pAction->m_pregistry->GetRegEntry(frh)->GetFilePath();
						if (_tcsicmp(pPath->GetExtension(), _TEXT(".pdb")) == 0)
						{
							if (!pPath->ExistsOnDisk())
							{
								CProject *pProject = pAction->Item()->GetProject();
								ASSERT(pProject != (CProject *)NULL);

#ifdef REFCOUNT_WORK
								// GetItemOutputFile will write over frh.
								frh->ReleaseFRHRef();
#endif
								if (GetItemOutputFile(pAction->Item(), MapLogical(P_OutputDir_Obj), frh, _TEXT(".obj"), &pProject->GetWorkspaceDir()))
									if (!g_buildengine.DeleteFile(frh, EC) && !(aob & AOB_IgnoreErrors))
										act = ACT_Error;	// failure
							}
#ifdef REFCOUNT_WORK
							if (NULL!=frh)
								frh->ReleaseFRHRef();
#endif
							break;	// done!
						}
#ifdef REFCOUNT_WORK
						if (NULL!=frh)
							frh->ReleaseFRHRef();
#endif
					}
				}
			}
			break;
		}

		// post?
		case TOB_Post:
			break;

		// action?
		case TOB_Stage:
			break;

		default:
			break;
	}

	if (act != ACT_Complete)
		return act;

	return CBuildTool::PerformBuildActions(type, stage, lstActions, aob, EC);
}
///////////////////////////////////////////////////////////////////////////////
// Respond to browser info. query/set.
void CCCompilerTool::OnActionEvent(UINT idEvent, CActionSlob * pAction, DWORD dwExtra)
{
	// call base-class
	CBuildTool::OnActionEvent(idEvent, pAction, dwExtra);

	// what is the event?
	switch (idEvent)
	{
		// setting browser info.
		// (for v3/4 options stored on project)
		case ACTEVT_SetBrowse:
		{
			(void) pAction->Item()->GetProject()->SetIntProp(MapLogical(P_GenBrowserInfo), (int)pAction->m_dwEvent);
			break;
		}

		// querying browser info.
		// (for v3/4 options stored on project)
		case ACTEVT_QueryBrowse:
		{
			if (pAction->Item()->GetProject()->GetIntProp(MapLogical(P_GenBrowserInfo), (int &)pAction->m_dwEvent) != valid)	
				pAction->m_dwEvent = (DWORD)FALSE;
			break;
		}

		default:
			break;
	}
}
///////////////////////////////////////////////////////////////////////////////
//	Scans C/C++ files for includes. Note the routine ignores "include" statements
//	on a line beginning with // but not inside /*  -- */ comment blocks.
BOOL CCCompilerTool::GenerateDependencies(CActionSlob * pAction, FileRegHandle frhFile, CErrorContext & EC)
{
	CString strFile;
	BOOL bSkipping = FALSE;
	int iEntryType;
	TCHAR cDelim;

	HANDLE hMem;
	DWORD dwFileSize;

	const TCHAR * pcFileMap;
	if (!g_buildengine.OpenMemFile(frhFile, hMem, pcFileMap, dwFileSize, EC))
	{
		const CPath * pPath = g_FileRegistry.GetRegEntry(frhFile)->GetFilePath();
		g_buildengine.LogBuildError(BldError_ScanError, (void *)pPath);
		return FALSE;	// couldn't open
	}

	const TCHAR * pMax;
	int nLine;

	if (pcFileMap == (const TCHAR *)NULL)
		goto CloseMemFile;	// no need to open

	pMax = pcFileMap + dwFileSize;
	nLine = 0;

	//	Check for afx hack.  If the file begins with "//{{NO_DEP", don't scan:
	if ((dwFileSize > 10) && _tcsnicmp (pcFileMap, _TEXT("//{{NO_DEP"), 10) == 0)
	{
		pAction->SetDepState(DEP_None);
		goto CloseMemFile;
	}

	do
	{
		nLine++; // start of a new line

		// Skip leading blanks
		while (pcFileMap < pMax && _istspace( *pcFileMap))
		{
			if (*pcFileMap == _T('\n'))
				nLine++;
			pcFileMap++;
		}

		if (pcFileMap >= pMax)
			break;

		 // Check this first so we ignore comments.
		if (*pcFileMap == _T('/'))
		{
			if (bSkipping)
			{
			if (pMax - pcFileMap >= 17
				&&
				_tcsncmp (pcFileMap, _TEXT("//}}START_DEPSCAN"), 17) == 0
				)
						bSkipping = FALSE;
			}
			else
			{
			if (pMax - pcFileMap >= 16
				&&
				_tcsncmp (pcFileMap, _TEXT("//{{STOP_DEPSCAN"), 16) == 0
				)
						bSkipping = TRUE;
			}
		}
		else if (*pcFileMap == _T('#') && (!bSkipping))
		{
			// Go past #:

			pcFileMap++;

			// skip white space after #:

			while (pcFileMap < pMax && _istspace( *pcFileMap)) pcFileMap++;

			// Look for "include':

			if ((pMax - pcFileMap > 7) && (!strncmp(pcFileMap, "include", 7)))
			{
				pcFileMap += 7;

				// skip white space after include:

				while (pcFileMap < pMax && _istspace( *pcFileMap)) pcFileMap++;

				if ( pcFileMap >= pMax )
					break;

				// Look for the delimeter either " or <

				if ( *pcFileMap == _T('"') )
				{
					iEntryType = IncTypeCheckParentDir |  IncTypeCheckIncludePath
								 | IncTypeCheckOriginalDir;
					cDelim = _T('"');
				}
				else if ( *pcFileMap == _T('<') )
				{
					iEntryType = IncTypeCheckIncludePath;
					cDelim = _T('>');
				}
				else
				{
					iEntryType = IncTypeCheckAbsolute; // REVIEW
					cDelim = _T('\0');
				}

				if (cDelim && (++pcFileMap < pMax))
				{
					const TCHAR * pFileNameBegin = pcFileMap;

					// Look for	other delimeter, or a return if the user screwed up:

					while (pcFileMap < pMax && *pcFileMap != cDelim && *pcFileMap != _T('\n'))
								pcFileMap = _tcsinc ( (TCHAR *) pcFileMap);

					if ( pcFileMap >= pMax )
						break;

					if (*pcFileMap == _T('\n'))
					{
						pcFileMap++;
						continue;
					}

					// File name now starts at pFileNameBegin and ends at pcFileMap-1.
					int nNameLength =  pcFileMap - pFileNameBegin;
					TCHAR * pbuf = strFile.GetBuffer(nNameLength + 1);

					strncpy(pbuf, pFileNameBegin, nNameLength);
					pbuf[nNameLength] = _T('\0');
					strFile.ReleaseBuffer();

					VERIFY (pAction->AddScannedDep(iEntryType, strFile, nLine));
				}
			}
		}

		// skip to the end of the current line
		while (pcFileMap < pMax)
		{
			if (*pcFileMap == _T('\n'))
			{
				pcFileMap++;
				break;
			}
			pcFileMap = _tcsinc ((TCHAR *) pcFileMap);
		}

	} while (pcFileMap < pMax);

	CloseMemFile:

	g_buildengine.CloseMemFile(hMem, EC);

	return TRUE;	 		
}		
///////////////////////////////////////////////////////////////////////////////
// Return the list of files generated by the compiler tool
BOOL CCCompilerTool::GenerateOutput(UINT type, CActionSlobList & lstActions, CErrorContext & EC)
{
	POSITION posAction = lstActions.GetHeadPosition();
	while (posAction != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(posAction);
		CProjItem * pItem = pAction->Item();

		CProject *pProject = pItem->GetProject();
		ASSERT (pProject);
		CDir * pDir = &pProject->GetWorkspaceDir();

		// object files
		if (!pAction->AddOutput(MapLogical(P_OutputDir_Obj), _TEXT(".obj"), pDir)){
			ASSERT(FALSE);
			return( FALSE );
		}

		// source browser files
		if (!pAction->AddOutput(MapLogical(P_OutputDir_Sbr), _TEXT(".sbr"), pDir, (TCHAR *)NULL, MapLogical(P_GenBrowserInfo))){
			ASSERT(FALSE);
			return( FALSE );
		}
				
		// pdb files (both .pdb and .idb)
		// P_DebugInfo == 3 is /Zi
		//
		int iDebugInfo;
		if ((pItem->GetIntProp (MapLogical(P_DebugInfo), iDebugInfo) == valid) && iDebugInfo >= DebugInfoPdb)
		{
			if (!pAction->AddOutput(MapLogical(P_OutputDir_Pdb), _TEXT(".pdb"), pDir, CActionSlob::szDefVCFile, (UINT)-1 /* no enable id */, FALSE /* 2ndary output */)){
				ASSERT(FALSE);
				return( FALSE );
			}
		}
		
		// Add idb for /Gm.
		int iMr,iFd;
		if ((pItem->GetIntProp( MapLogical(P_Enable_MR), iMr) == valid) && iMr ){
			if (!pAction->AddOutput(MapLogical(P_OutputDir_Pdb), _TEXT(".idb"), pDir, CActionSlob::szDefVCFile,  MapLogical(P_Enable_MR), FALSE /* 2ndary output */)){
				ASSERT(FALSE);
				return FALSE;
			}
		}
		else if ((pItem->GetIntProp( MapLogical(P_Enable_FD), iFd) == valid) && iFd )
		{
			// Add idb for /FD.
			if (!pAction->AddOutput(MapLogical(P_OutputDir_Pdb), _TEXT(".idb"), pDir, CActionSlob::szDefVCFile,  MapLogical(P_Enable_FD), FALSE /* 2ndary output */)){
				ASSERT(FALSE);
				return FALSE;
			}
		}

		// check to see if this file produces or uses a PCH file....
		// get the location of of the /Fp setting
		CPath pathPch;
		BOOL bval;

		// clear the .pch file dep. from the source deps...
		CFileRegSet * pfrs = pAction->GetSourceDep();
		FileRegHandle frh;

		pfrs->InitFrhEnum();
		while ((frh = pfrs->NextFrh()) != (FileRegHandle)NULL)
		{
			const CPath * pPath = pAction->m_pregistry->GetRegEntry(frh)->GetFilePath();
			if (_tcsicmp(pPath->GetExtension(), _TEXT(".pch")) == 0)
			{
				// remove the .pch as a source dep.
				if (!pAction->RemoveSourceDep(frh))
					ASSERT(FALSE);
#ifdef REFCOUNT_WORK
				frh->ReleaseFRHRef();
#endif
				break;	// done!
			}
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif
		}

		CString strPch;
		if ((pItem->GetStrProp(MapLogical(P_PchDefaultName), strPch) != valid) || strPch.IsEmpty())
			continue;	// no pch name....
			
		// add a possible .pch file dep.
		if ((pItem->GetIntProp(MapLogical(P_PchCreate), bval) == valid) && bval)
		{
			if (pathPch.CreateFromDirAndFilename(*pDir, strPch))
				if (!pAction->AddOutput(&pathPch))
					ASSERT(FALSE);
		}
		else if ((pItem->GetIntProp(MapLogical(P_PchUse), bval) == valid) && bval)
		{
			// add the .pch as a source dep.
			if (pathPch.CreateFromDirAndFilename(*pDir, strPch))
			{
				CString strPch = (const TCHAR *)pathPch;
				if (!pAction->AddSourceDep(strPch))
					ASSERT(FALSE);
			}
		}
	}

	return TRUE; // success
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
BOOL CCCompilerTool::AffectsOutput(UINT idPropL)
{
	// change our outputs?
	if (
		idPropL == P_GenBrowserInfo ||	// enables *.sbr
		idPropL == P_PchCreate ||		// enable *.pch
		idPropL == P_PchUse ||			// ""
		idPropL == P_AutoPch ||			// ""
		idPropL == P_OutputDir_Obj ||	// *.obj
		idPropL == P_OutputDir_Sbr ||	// *.sbr
		idPropL == P_OutputDir_Pch ||	// *.pch
		idPropL == P_OutputDir_Pdb ||	// *.pdb / *.icc
		idPropL == P_DebugInfo ||		// enables *.pdb
		idPropL == P_Enable_Incr ||		// enables *.icc
		idPropL == P_BldrFileName		// *.pch
	   )
	{
	   return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Return the semi-colon seperated list of paths to search for Includes in 
void CCCompilerTool::GetIncludePaths(CProjItem * pItem, CString & str)
{
	// make sure it starts empty
	str.Empty();

	CString strFileIncs;

	// -I should be in front of INCLUDE path
	// we should search the /I (per-file) includes first!
	if (pItem->GetStrProp(MapLogical(P_CplrIncludes), strFileIncs))
		str = (strFileIncs + _TEXT (";"));

	ReplaceEnvVars(str);

	GetDirMgr()->GetDirListString(strFileIncs, DIRLIST_INC);
	
	str += strFileIncs;

}
//////////////////////////////////////////////////////////////////////////////////
void CCCompilerTool::GetIncludePathFromOptions(CProjItem *pItem, CString& strResult)
{
	strResult.Empty();
	pItem->GetStrProp (MapLogical (P_CplrIncludes), strResult);
};

///////////////////////////////////////////////////////////////////////////////
// Return the file reg. handle the name of the PCH file associated with this item
int CCCompilerTool::GetItemPCHFileName(CProjItem * pItem, FileRegHandle &frh)
{
	const CPath *pPath = pItem->GetFilePath();
	if (!pPath) return NoPch;		
 
	int pct = NoPch;
 
	BOOL b;

	// Check to see if this file produces or uses a PCH file:
	if ((pItem->GetIntProp (MapLogical(P_PchCreate), b)==valid) && b)
	{
		pct = PchCreate;
	}
	else if ((pItem->GetIntProp (MapLogical(P_PchUse), b)==valid) && b)
	{
		pct = PchUse;
        b = FALSE;
	}
	else if (( pItem->GetIntProp (MapLogical(P_AutoPch), b) == valid) && b)
	{
		pct = PchAuto;
	}

	// Now register the file:
	if (b)
	{
		CString str;

		// First get the value of the /Fp flag.
		if ((pItem->GetStrProp(MapLogical(P_PchDefaultName),str) != valid) || str.IsEmpty())
			return NoPch;

		CFileRegistry * pRegistry = pItem->GetRegistry();

		CPath path;
		if (path.CreateFromDirAndFilename(pItem->GetProject()->GetProjDir(), str))
		{
#ifndef REFCOUNT_WORK
			if ((frh = pRegistry->LookupFile(&path)) == (FileRegHandle)NULL)
				frh = pRegistry->RegisterFile(&path);	// incr. ref
			else
				pRegistry->AddRegRef(frh);	// incr. ref
#else
			frh = CFileRegFile::GetFileHandle(path);
#endif

			return	pct;
		}
	}

	frh = (FileRegHandle)NULL;
	return NoPch;
}
///////////////////////////////////////////////////////////////////////////////
// Return the file reg. handle the name of the PCH file associated with this item
// FUTURE:
// the following function is very similar to GetItemPCHFileName() except this returns
// the frh only when P_AutoPch is set. The reason for having a separate function
// is for speed, since this function is needed when we do a rebuild all and have to
// delete ALL the pch files (including the one specified by P_UsePch (/Yu))
BOOL CCCompilerTool::GetItemAutoPCHFile(CProjItem * pItem, FileRegHandle & frh)
{
	const CPath *pPath = pItem->GetFilePath();
	if (!pPath) return FALSE;

	BOOL bAutoPch = FALSE;
	CString str;
		
	if ((pItem->GetIntProp (MapLogical(P_AutoPch), bAutoPch) != valid) || !bAutoPch ||
		(pItem->GetStrProp (MapLogical(P_PchDefaultName), str) != valid) || str.IsEmpty())
		return FALSE;

	CPath path;
	// to avoid leaks, do not add frh to the registry. Create a new one, and delete it in the caller after use
	if (path.CreateFromDirAndFilename(pItem->GetProject()->GetProjDir(), str))
	{
#ifndef REFCOUNT_WORK
		frh = (FileRegHandle)(new CFileRegFile(&path, FALSE /* bMatchCase */, FALSE /* !bNoFilter */));
#else
		frh = CFileRegFile::GetFileHandle(path);
#endif
	}
	else
		frh = (FileRegHandle)NULL;

	return frh != (FileRegHandle)NULL;
}


#ifdef KEEP_PCH
// Copied from CCCompilerTool::GetItemAutoPCHFile.
BOOL CCCompilerTool::GetItemCreatePCHFile(CProjItem * pItem, FileRegHandle & frh)
{
	frh = NULL;
//	const CPath *pPath = pItem->GetFilePath();
//	if (!pPath) return FALSE;
	// This item should always have a path.
	ASSERT( NULL != pItem->GetFilePath() );

	BOOL bCreatePch = FALSE;
	CString str;
		
	if ((pItem->GetIntProp (MapLogical(P_PchCreate), bCreatePch) != valid) || !bCreatePch ||
		(pItem->GetStrProp (MapLogical(P_PchDefaultName), str) != valid) || str.IsEmpty())
		return FALSE;

	CPath path;
	if (path.CreateFromDirAndFilename(pItem->GetProject()->GetProjDir(), str))
	{
		CFileRegistry * pRegistry = pItem->GetRegistry();
		ASSERT( NULL != pRegistry );
#ifndef REFCOUNT_WORK
		if ((frh = pRegistry->LookupFile(&path)) == (FileRegHandle)NULL)
			frh = pRegistry->RegisterFile(&path);	// incr. ref
		else
			pRegistry->AddRegRef(frh);	// incr. ref
#else
		frh = CFileRegFile::GetFileHandle(path);
#endif
	}

	return frh != (FileRegHandle)NULL;
}
#endif // KEEP_PCH


///////////////////////////////////////////////////////////////////////////////
struct cmdrec
{
	CString str;
	CString strFront;
	BOOL 	bFront;
	PchTypes ePchType;
	CString strPerFileProlog;
	int 	count;
};

// Generate command lines for a C/C++ Compiler, doing all the good ordering things.
// Basic plan is to keep all the option strings we've seen in cmstpFlagMap so
// we can quickly tell if we've seen a flag set.  cmstpFlagMap has a pointer
// to a cmdrec which keeps a string with the input files and tells if there
// are any "truly" out of date files:
BOOL CCCompilerTool::GetCommandLines
(
	CActionSlobList & lstActions,
	CPtrList & plCommandLines,
	DWORD attrib,
	CErrorContext & EC
)
{
	CString strDescription;
	VERIFY(strDescription.LoadString(IDS_DESC_COMPILING));

  	CMapStringToPtr cmstpFlagMap; cmstpFlagMap.InitHashTable(151);
 	cmdrec *pcmdrec;

	CStringList strFlagList;
	CString strFilePath;
	TCHAR *pc;

    // revers the order again to get them back in alphabetical order
	POSITION posAction = lstActions.GetTailPosition();
	while (posAction != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)lstActions.GetPrev(posAction);

		CProjItem * pItem = pAction->Item();
		ActionState as = pAction->m_state;

		strFilePath = (const TCHAR *)*pItem->GetFilePath();

		CString str;
		if (!pAction->GetCommandOptions(str))
			return FALSE;


		ReplaceEnvVars(str);


#if 1
#pragma message( "Turn of /FD hack" )
		if (Full!=MrSupportLevel())
		{
			int iFD = str.Find("/FD");
			if (-1 != iFD)
				str = str.Left(iFD) + str.Mid(iFD+3);
		}
#endif
		
		if (cmstpFlagMap.Lookup(str, (void * &)pcmdrec))
		{
			// N.B. don't re-order if /YX
			if ((as == _SrcOutOfDate) && (pcmdrec->ePchType != PchAuto))
			{	
				pcmdrec->count++;
				pcmdrec->bFront = TRUE;

				if (pcmdrec->strFront.IsEmpty())
					pcmdrec->strFront = strFilePath;	// no prolog for 1st one yet
				else
				{
					if (pcmdrec->strPerFileProlog.IsEmpty ())
					{			
						pcmdrec->strFront = (pcmdrec->strFront + _TEXT ("\"\n\"")) +
						      			strFilePath;
					}
					else
					{
						pcmdrec->strFront = (pcmdrec->strFront +
										_TEXT ("\"\n")) +
										(pcmdrec->strPerFileProlog +
										_TEXT(" \"")) +
					   		   			strFilePath;
					}
				}
			}
			else
			{
				pcmdrec->count++;
				if (pcmdrec->str.IsEmpty())
					pcmdrec->str = strFilePath;	// no prolog for 1st one yet
				else
				{
					if (pcmdrec->strPerFileProlog.IsEmpty ())
					{			
						pcmdrec->str = (pcmdrec->str + _TEXT ("\"\n\"")) +
						      			strFilePath;
					}
					else
					{
						pcmdrec->str = (pcmdrec->str +
										_TEXT ("\"\n")) +
										(pcmdrec->strPerFileProlog +
										_TEXT(" \"")) +
						      			strFilePath;
					}
				}
			}
		}	  
		else	// No existing record, must make one:
		{
			BOOL b;

			pcmdrec = new  cmdrec;
			pcmdrec->count = 1;
			strFlagList.AddTail(str);
			cmstpFlagMap.SetAt (str, pcmdrec);

			// See if this creates a PCH file:
			// Note: /Yc overrides /Yu overrides /YX (order is important!)
			if ((pItem->GetIntProp (MapLogical(P_PchCreate), b)==valid) && b)
				pcmdrec->ePchType = PchCreate;
			else if ((pItem->GetIntProp(MapLogical(P_PchUse), b)==valid) && b)
				pcmdrec->ePchType = PchUse;
			else if ((pItem->GetIntProp(MapLogical(P_AutoPch), b)==valid) && b)
				pcmdrec->ePchType = PchAuto;
			else
				pcmdrec->ePchType = NoPch;

			// N.B. don't change order if /YX
			if ((as == _SrcOutOfDate) && (pcmdrec->ePchType!=PchAuto))
			{
				pcmdrec->bFront = TRUE;
				pcmdrec->strFront = strFilePath;
			}
			else
			{
				pcmdrec->bFront = FALSE;
				pcmdrec->str = strFilePath;
			}

			// g_prjoptengine must be setup correctly before using it below or
			// we crash.  speed:4030.
			g_prjoptengine.SetOptionHandler (GetOptionHandler());
			g_prjoptengine.SetPropertyBag (pItem);

			// See if it uses /Tc
			if ((pItem->GetIntProp (MapLogical(P_TreatFileAsC), b)==valid) && b)
			{
				str.Empty ();
				g_prjoptengine.GenerateSingleOption (IDOPT_TREATFILEAS_C, str );
				pcmdrec->strPerFileProlog =	str;
			}
			// See if it uses /Tp
			if ((pItem->GetIntProp (MapLogical(P_TreatFileAsCPP), b)==valid) && b)
			{
				if (!pcmdrec->strPerFileProlog.IsEmpty())
						pcmdrec->strPerFileProlog += _T (' ');
				str.Empty ();
				g_prjoptengine.GenerateSingleOption (IDOPT_TREATFILEAS_CPP, str );
				pcmdrec->strPerFileProlog += str;
			}

		}
	}

	// CStringList slCreateLines, slCreateDescs;
	// CStringList slFrontLines, slFrontDescs;
	
	CCmdLine *pCreateLines = new CCmdLine;
	CCmdLine *pFrontLines = new CCmdLine;
	pCreateLines->nProcessor = 1;
	pFrontLines->nProcessor = 1;

	POSITION pos = strFlagList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CString str;
		str = strFlagList.GetNext(pos);

		cmstpFlagMap.Lookup(str, (void *&) pcmdrec);
		cmstpFlagMap.RemoveKey(str); // FUTURE: is this really necessary?
			
		// append strFront + prolog + str
		if (!pcmdrec->strFront.IsEmpty())
		{
			if (pcmdrec->str.IsEmpty())
			{
				pcmdrec->str = pcmdrec->strFront;
				pcmdrec->strFront.Empty();
			}
			else
			{
				if (pcmdrec->strPerFileProlog.IsEmpty ())
				{			
					pcmdrec->str = (pcmdrec->strFront + _TEXT ("\"\n\"")) +
					      			pcmdrec->str;
				}
				else
				{
					pcmdrec->str = (pcmdrec->strFront +
									_TEXT ("\"\n")) +
									(pcmdrec->strPerFileProlog +
									_TEXT(" \"")) +
					      			pcmdrec->str;
				}
			}
			pcmdrec->strFront.Empty();
		}

		// If there's a prolog string, nuke it in the main command
		// line.  Note thate we can be hosed if there's more than
		// one argument in strPerFileProlog:
		if (!pcmdrec->strPerFileProlog.IsEmpty ())
		{
			str.GetBuffer (1);
			pc = _tcsstr ( (const TCHAR *)str, (const TCHAR *)pcmdrec->strPerFileProlog);
			if (pc != NULL)
			{
				_tcsnset ( pc, (int) ' ', pcmdrec->strPerFileProlog.GetLength ());
			}
			str.ReleaseBuffer ();				
			pcmdrec->strPerFileProlog += _T (' ');
		}

		// only try this if you are certain to get away with it
		if ( pcmdrec->strPerFileProlog.IsEmpty () &&
			(g_nBuildProcessors>1) &&
			(pcmdrec->count > (g_nBuildProcessors*2)) &&
		    (pcmdrec->ePchType != PchCreate) &&
			!pcmdrec->bFront
		   ) {
			// split the task in parts here ...
		    int nFiles =  pcmdrec->count / g_nBuildProcessors;
			CString strFiles;
			int i;
			for( i=0; i<g_nBuildProcessors; i++ ){
				// TODO !!! loop through source files and split here.
				strFiles = pcmdrec->str;
	
				str += _TEXT ("\n\"") + strFiles + _TEXT ("\"\n");
				CPath pathtTemp;
				if (!g_buildengine.CreateTempFile(str, pathtTemp, EC))
					return FALSE;
		
				// quote response file path if necessary
				CString strRspPath = pathtTemp;
				if (pathtTemp.ContainsSpecialCharacters())
					strRspPath = _T('\"') + strRspPath + _T('\"');

				// use response file
				str = m_strToolExeName + _TEXT ( " @" ) + strRspPath;
				PBUILD_TRACE ("Creating command line \"%s\" \n",(const TCHAR *)	str );

				CCmdLine *pCmdLine = new CCmdLine;
				pCmdLine->slCommandLines.AddHead(str);
				pCmdLine->slDescriptions.AddHead(strDescription);
				pCmdLine->nProcessor = i;
				plCommandLines.AddTail(pCmdLine);
			}
		} else {
			str = str + _TEXT ('\n') + pcmdrec->strPerFileProlog +
					 _TEXT ('\"') + pcmdrec->str + _TEXT ("\"\n");
			CPath pathtTemp;
			if (!g_buildengine.CreateTempFile(str, pathtTemp, EC))
				return FALSE;
	
			// quote response file path if necessary
			CString strRspPath = pathtTemp;
			if (pathtTemp.ContainsSpecialCharacters())
				strRspPath = _T('\"') + strRspPath + _T('\"');

			// use response file
			str = m_strToolExeName + _TEXT ( " @" ) + strRspPath;
	
			PBUILD_TRACE ("Creating command line \"%s\" \n",(const TCHAR *)	str );
			if (pcmdrec->ePchType == PchCreate)
			{
				// copy to temp list (we'll prepend these to the main list later)
				if (pcmdrec->bFront)
				{
					pCreateLines->slCommandLines.AddHead(str);
					pCreateLines->slDescriptions.AddHead(strDescription);
				}
				else
				{
					pCreateLines->slCommandLines.AddTail(str);
					pCreateLines->slDescriptions.AddTail(strDescription);
				}
			}
			else
			{
				if ( pcmdrec->bFront)
				{
					pFrontLines->slCommandLines.AddHead(str);
					pFrontLines->slDescriptions.AddHead(strDescription);
				}
				else
				{
					CCmdLine *pCmdLine = new CCmdLine;
					pCmdLine->slCommandLines.AddTail(str);
					pCmdLine->slDescriptions.AddTail(strDescription);
					pCmdLine->nProcessor = 1;
					plCommandLines.AddTail(pCmdLine);
				}
			}
		}
		delete (pcmdrec);
	}

	// copy front entries, if any, to the very front of the list
	if (!pFrontLines->slCommandLines.IsEmpty())
	{
		plCommandLines.AddHead(pFrontLines);
	} else {
		delete pFrontLines;
	}

	// now copy /Yc entries, if any, to the very front of the list
	if (!pCreateLines->slCommandLines.IsEmpty())
	{
		plCommandLines.AddHead(pCreateLines);
	} else {
		delete pCreateLines;
	}

	strFlagList.RemoveAll();

	ASSERT(cmstpFlagMap.IsEmpty());

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////
// Makefile reading and writing for the compiler tool
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const TCHAR *pDefCCompRule = _TEXT ("$(CPP) @<<\r\n   $(CPP_PROJ) $< \r\n<<" );

void CCCompilerTool::WriteInferenceRuleMacro (CActionSlob * pAction)
{
	ASSERT(g_bExportMakefile);

	g_prjoptengine.SetOptionHandler(GetOptionHandler());
	g_prjoptengine.SetPropertyBag(pAction->m_pItem);

	CString strBuildLine;
	g_prjoptengine.GenerateString(strBuildLine, (OBShowMacro | OBShowDefault | OBShowFake | OBInherit));

	TRY
	{
		m_pMakWriter->WriteMacro("CPP_PROJ", strBuildLine);
	}
	CATCH (CException, e)
	{
		strBuildLine.Empty();
		THROW_LAST();
	}	
	END_CATCH
}

void CCCompilerTool::WriteInferenceRules (CActionSlob * pAction)
{
	ASSERT(g_bExportMakefile);

	// Basically there are two cases: Output directories and no output
	// directories.

	if (m_OutputDirOptions != NoOutputDirs)
	{
		if (m_OutputDirOptions == OneOutputDir)
										WriteOutputDirMacro (pAction);
			
		m_pMakWriter->InsureSpace ();


		m_pMakWriter->WriteString (_TEXT ( ".c{$(INTDIR)}.obj::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cpp{$(INTDIR)}.obj::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cxx{$(INTDIR)}.obj::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".c{$(INTDIR)}.sbr::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cpp{$(INTDIR)}.sbr::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cxx{$(INTDIR)}.sbr::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();
	}
	else
	{
		m_pMakWriter->InsureSpace ();

		m_pMakWriter->WriteString (_TEXT ( ".c.obj::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cpp.obj::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cxx.obj::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".c.sbr::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cpp.sbr::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();

		m_pMakWriter->WriteString (_TEXT ( ".cxx.sbr::"));
		m_pMakWriter->EndLine (); m_pMakWriter->Indent ();
		m_pMakWriter->WriteString (pDefCCompRule);
		m_pMakWriter->EndLine (); m_pMakWriter->EndLine ();
	}
}

void GroomOutputString ( CString &str )
{
	const TCHAR *pc= str;
	if (str.IsEmpty()) return;

	// Check to see if we have some sort of absolute path and if we don't,
	// prpend ".\"
	if (*pc != '\\' && *pc != '/' && *pc != '.'
		&& !(
	#ifdef _MBCS
			(!IsDBCSLeadByte((BYTE)*pc))
			&&
	#endif
			(*(pc + 1) == _T(':'))
			)
		)
	{
		str = _TEXT (".\\") + str;
	}

	if (::ScanPathForSpecialCharacters ( str ))
					str = (_T('"') + str) + _T('"');
}

void CCCompilerTool::WriteOutputDirMacro (CActionSlob * pAction)
{
#if 0
	ASSERT(g_bExportMakefile);
	CString str;
	if ((pAction->m_pItem->GetStrProp (MapLogical(P_OutputDir_Obj), str) != valid) || (str.IsEmpty()))
		str = ".";	// failed to get prop, so use current dir

	GroomOutputString (str);
	m_pMakWriter->WriteMacro ( _TEXT ("CPP_OBJS"), str );

	if ((pAction->m_pItem->GetStrProp (MapLogical(P_OutputDir_Sbr), str) != valid) || (str.IsEmpty()))
		str = ".";	// failed to get prop, so use current dir

	GroomOutputString (str);
	m_pMakWriter->WriteMacro ( _TEXT ("CPP_SBRS"), str );
#endif
}

CCCompilerTool::OutputDirOptions CCCompilerTool::HasPerConfigOutputDirs (CProjItem * pItem)
{
	CString strObjs, strSbrs, str;
	CString strBaseObjs, strBaseSbrs;
	const CPtrArray & ppcr = *pItem->GetConfigArray();
	int i, nNumConfigs = pItem->GetPropBagCount();
	BOOL bFirstIter = TRUE;
	OutputDirOptions odo = NoOutputDirs;

	int idOldBag = pItem->UsePropertyBag(CurrBag);

	UINT idPropOBJ = MapLogical(P_OutputDir_Obj);
	UINT idPropSBR = MapLogical(P_OutputDir_Sbr);

	for (i = 0; i < nNumConfigs; i++)
	{
		ASSERT_VALID((ConfigurationRecord *)ppcr[i]);
		pItem->ForceConfigActive((ConfigurationRecord *)ppcr[i]);

		if (bFirstIter)
		{
			bFirstIter = FALSE;
			if (pItem->GetStrProp (idPropOBJ, strObjs) != valid)
				strObjs = "";	// failed to get prop, no output dir.
			if (pItem->GetStrProp (idPropSBR, strSbrs) != valid)
				strSbrs = "";	// failed to get prop, no output dir.
			if (!strObjs.IsEmpty () || !strSbrs.IsEmpty ())	odo = OneOutputDir;
			
			// repeat for Base dirs
			(void) pItem->UsePropertyBag(BaseBag);

			if (pItem->GetStrProp (idPropOBJ, strObjs) != valid)
				strBaseObjs = "";	// failed to get base prop, no output dir.
			if (pItem->GetStrProp (idPropSBR, strSbrs) != valid)
				strBaseSbrs = "";	// failed to get base prop, no output dir.
			if (!strBaseObjs.IsEmpty () || !strBaseSbrs.IsEmpty ())	odo = OneOutputDir;

			(void) pItem->UsePropertyBag(CurrBag);
		}
		else
		{
			// FUTURE: this logic looks bogus!
			if (pItem->GetStrProp (idPropOBJ, str) != valid)
				str = "";	// failed to get prop, no output dir.

			if (str.Collate (strObjs))  { odo = PerConfigOutputDirs; break; }
			if (odo != OneOutputDir && !str.IsEmpty())
			{
				ASSERT(0); // This should be dead code
				odo = OneOutputDir;
			}

			if (pItem->GetStrProp (idPropSBR, str) != valid)
				str = "";	// failed to get prop, no output dir.

			if (str.Collate (strSbrs)) { odo = PerConfigOutputDirs; break; }
			if (odo != OneOutputDir && !str.IsEmpty())
			{
				ASSERT(0); // This should be dead code
				odo = OneOutputDir;
			}
					
			// repeat for Base dirs
			(void) pItem->UsePropertyBag(BaseBag);

			if (pItem->GetStrProp (idPropOBJ, str) != valid)
				str = "";	// failed to get prop, no output dir.

			if (str.Collate (strBaseObjs))  { odo = PerConfigOutputDirs; break; }
			if (odo != OneOutputDir && !str.IsEmpty())
			{
				ASSERT(0); // This should be dead code
				odo = OneOutputDir;
			}

			if (pItem->GetStrProp (idPropSBR, str) != valid)
				str = "";	// failed to get prop, no output dir.

			if (str.Collate (strBaseSbrs)) { odo = PerConfigOutputDirs; break; }
			if (odo != OneOutputDir && !str.IsEmpty())
			{
				ASSERT(0); // This should be dead code
				odo = OneOutputDir;
			}

			(void) pItem->UsePropertyBag(CurrBag);
					
		}		

		// reset the config.
		pItem->ForceConfigActive();
	}	

	(void) pItem->UsePropertyBag(idOldBag);

	// reset the config.
	pItem->ForceConfigActive();

	return odo;
}

///////////////////////////////////////////////////////////////////////////////
// Ensure that this project compiler options are ok to use/not use MFC
BOOL CCCompilerTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	CString strDefine;

	COptionHandler * popthdlr = GetOptionHandler(); ASSERT(popthdlr != (COptionHandler *)NULL);

	// add /D"_MBCS" if we are now using MFC
	// add /GX if we are now using MFC
	if (iUseMFC != NoUseMFC)
	{

		// Seems awfully rude to force _MBCS on users. I am leaving this here so it this turns
		// out to be a problem we can re-enable [sanjays]
		// strDefine.LoadString(IDS_MBCS_DEFINE);

		pProjItem->SetIntProp(MapLogical(P_Enable_EH), TRUE);

	  	// popthdlr->MungeListStrProp(MapLogical(P_MacroNames), strDefine, TRUE, _T(','));
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CCCompilerTool::IsProductFile ( const CPath *pPath )
{
	// FUTURE: remove the 2nd test when first test is verified
	return ((FileNameMatchesExtension(pPath, _T("obj;pch"))) ||
		(!FileNameMatchesExtension(pPath, _T("pdb;idb"))));
}

BOOL CCCompilerTool::HasDependencies( ConfigurationRecord* pcr)
{
	if (!HasMrSupport())
		return FALSE;

	// Get CFileItem*.
	BOOL bVal = FALSE;
	CFileItem* pFileItem = (CFileItem*)pcr->m_pOwner;
	ASSERT(pFileItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
	int nPropId = -1;

	if (Partial==m_MrSupportLevel)
		nPropId = MapLogical(P_Enable_MR);
	else if (Full==m_MrSupportLevel)
		nPropId = MapLogical(P_Enable_FD);

	if (-1 != nPropId)
		if (pFileItem->GetIntProp(nPropId, bVal) != valid)
			bVal = FALSE;

	return bVal;
}

BOOL CCCompilerTool::GetDependencies( ConfigurationRecord* pcr, 
	CStringArray& strArray, BOOL* bUpToDate)
{
	BOOL bHasDepend = FALSE;

#if defined(_DEBUG)
		CString strMessage;
#if 0	// defined(_DEBUG)
		strMessage = _T("Getting dependencies for ");
		strMessage += ((CFileItem*)pcr->m_pOwner)->GetFileRegHandle()->GetFilePath()->GetFullPath();
		g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strMessage, FALSE, TRUE);
#endif
#endif

	if (!HasDependencies(pcr))
		return bHasDepend;

	// Get CFileItem*.
	CFileItem* pFileItem = (CFileItem*)pcr->m_pOwner;
	ASSERT(pFileItem->IsKindOf(RUNTIME_CLASS(CFileItem)));

	// Get CFileRegFile*
	FileRegHandle frh = pFileItem->GetFileRegHandle();
	ASSERT(NULL!=frh);
	ASSERT(frh->IsNodeType(CFileRegNotifyRx::nodetypeRegFile));
	CFileRegFile* frf = (CFileRegFile*)g_FileRegistry.GetRegEntry(frh);

	// Get filename of source file.
	const CPath* pPath = frf->GetFilePath();

	CPath pathIdb = pFileItem->GetProject()->GetMrePath(pcr);

	// Minimal rebuild dependency interface.
	IMreDependencies* pMreDepend = 
		IMreDependencies::GetMreDependenciesIFace((LPCTSTR)pathIdb);
	if (NULL!=pMreDepend)
	{
		CStringList strList;

		//
		// Does this file use the pch?
		//
		BOOL bUsePch = FALSE;
		if ((pFileItem->GetIntProp(MapLogical(P_PchUse), bUsePch) == valid) && bUsePch)
			bUsePch = TRUE;
		else
			bUsePch = FALSE;

		if (pMreDepend->GetDependencies( pPath->GetFullPath(), strList, bUsePch ))
		{
			bHasDepend = TRUE;

			//
			// Filter out system headers and write into array.
			//
			strArray.RemoveAll();
			POSITION pos = strList.GetHeadPosition();
			while (NULL!=pos)
			{
				CString str = strList.GetNext(pos);
				ASSERT(!str.IsEmpty());

				//
				// NOTE: IsSysInclude() will always return FALSE when
				// passed the full pathname of a file that exists.
				// Therefore, only the file name is passed.  This will cause
				// slightly different behavior for files included relative
				// to the project directory, and which are part of the
				// sysincl.dat list.
				//
				// review(tomse): The call to IsSysInclude takes about 50-70% of the 
				// total time in this loop(based on hand-timings on debug builds).
				// IsSysInclude is called for each dependency of each file scanned
				// when using the minimal rebuild information.  This is more
				// than code that uses g_ScannerCache so that no file is scanned more
				// than once.  (This problem is seen when /Yu is not used and 
				// #include's are nested.)
				// 
				int iBackSlash = str.ReverseFind(_T('\\'));
				ASSERT(-1!=iBackSlash);
				ASSERT(str.GetLength() > iBackSlash+1);

				// The MR should never return an invalid path but just in case,
				// make sure that we don't try to add an invalid path.
				if ( -1 != iBackSlash && str.GetLength() > iBackSlash+1 )
				{
					CString strFilename = str.Mid(iBackSlash+1);
					if (!g_SysInclReg.IsSysInclude(strFilename, pcr->m_pOwner->GetProject()))
					{
						strArray.Add(str);
					}
				}
			}
		}
#if 0	// defined(_DEBUG)
		else
		{
			strMessage = _T("Could not get dependencies for ");
			strMessage += pPath->GetFullPath();
			g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strMessage, FALSE, TRUE);
		}
#endif
		VERIFY( pMreDepend->Release() );
	}
#if 0	// defined(_DEBUG)
	else
	{
		strMessage = _T("Could not open ");
		strMessage += (LPCTSTR)pathIdb;
		g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strMessage, FALSE, TRUE);
	}
#endif

	// Are dependencies up-to-date?
	if (NULL!=bUpToDate)
	{
		*bUpToDate = FALSE;
	}
	return bHasDepend;
}

void CCCompilerTool::CalcMrSupport()
{
	m_MrSupportLevel = None;	// Should be set in ctor.
#if 0
	// Review: (tomse) We may want to allow multiple calls to CalcMrSupport
	// if there are correctable problems that previously resulted 
	// in "None" for the support level.
	ASSERT(Unknown==m_MrSupportLevel);

	// Assume support is None if any failure occurs.
	m_MrSupportLevel = None;
	TCHAR szCl[_MAX_PATH];
	LPTSTR szFileName; 
	DWORD dw;

	// Look for cl.exe.
	// review: (tomse) How do we know we have the right environment? Make
	// sure that m68k and pmc compilers are located correctly.
	CEnvironmentVariableList bldenviron;

	// get the environment from the directories manager
	CDirMgr * pDirMgr = GetDirMgr();

	CString str;

	pDirMgr->GetDirListString(str, DIRLIST_PATH);
	bldenviron.AddVariable ("path", str);
	bldenviron.SetVariables();				// set it

#if 0	// defined(_DEBUG)
		CString strMessage;
		strMessage = _T("Minimal rebuild dependency support ");
		g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strMessage, FALSE, TRUE);
#endif

	if (0==SearchPath( NULL, m_strToolExeName, NULL, _MAX_PATH, szCl, &szFileName ))
	{
		bldenviron.ResetVariables();
		return;
	}
	bldenviron.ResetVariables();

#if 0	// defined(_DEBUG)
		strMessage = szCl;
		strMessage += _T(" supports ");
		g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strMessage, FALSE, TRUE);
#endif

	// Get version information from cl.exe.
	DWORD dwSize = GetFileVersionInfoSize( szCl, &dw );
	if (0!=dwSize)
	{
		LPVOID lpBuffer = new BYTE[dwSize];

		if (NULL==lpBuffer)
			return;

		if (GetFileVersionInfo( szCl, dw, dwSize, lpBuffer))
		{
			UINT uLen;
			VS_FIXEDFILEINFO* pVerInfo;
			if (0!=VerQueryValue( lpBuffer, _T("\\"), (LPVOID*)&pVerInfo, &uLen ))
			{
				const unsigned dwFD_VersionMS = 0x000c0000;   // WinslowF -- 5.0 had -->0x000b0000;
				const unsigned dwFD_VersionLS = 0x00000000;
				const unsigned dwFD_VersionLearn = 0x000b0063;
				// Must be >= 12 for full support.
				if ( dwFD_VersionMS < pVerInfo->dwFileVersionMS )
					m_MrSupportLevel = Full;
				else if ( dwFD_VersionMS == pVerInfo->dwFileVersionMS && 
					dwFD_VersionLS <= pVerInfo->dwFileVersionLS )
					m_MrSupportLevel = Full;
				// test for v 11.99
				else if ( dwFD_VersionLearn <= pVerInfo->dwFileVersionMS )
					m_MrSupportLevel = Partial;
			}
		}
		delete [] lpBuffer;
	}
#if 0	// defined(_DEBUG)
		if (Full==m_MrSupportLevel)
			strMessage = "/FD";
		else if (Partial==m_MrSupportLevel)
			strMessage = "/Gm";
		else
			strMessage = "None";

		g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strMessage, FALSE, TRUE);
#endif
#endif	// #if 0 // removed
}

///////////////////////////////////////////////////////////////////////////////
// We need to be /MD for UseMFCInDll because the MFC DLL and us need to share the
// same C-runtime,
// set the default for using MFC not in a DLL
// FUTURE: setting the default might not be what the user-originally had .. but I don't
// want to warn .. they change MFC type we change their options and try to be good about it :-)
int CCCompilerNTTool::ThreadForUsingMFC(CProjItem * pProjItem, int iUseMFC)
{
	BOOL bUseDebugLibs = FALSE;
	pProjItem->GetIntProp(P_UseDebugLibs, bUseDebugLibs);

	int nThread = 4;
	if (bUseDebugLibs)
		{
		nThread = 1;      //MLd
		}

	if (iUseMFC == UseMFCInDll)
	{
		if (!bUseDebugLibs)
		{
			nThread = 6;	// MD
		}
		else
		{
			nThread = 3;    //MDd
		}
	}
	else
	{
		// /MT for a Dll project or MFC staic lib. project, else /ML
		CProjType * pProjType;
		if (g_BldSysIFace.GetProjComponentMgr()->GetProjTypefromProjItem(pProjItem, pProjType))
		{
			ASSERT(pProjType != (CProjType *)NULL);
			if (iUseMFC == UseMFCInLibrary || pProjType->GetAttributes() & ImageDLL)
			{
				if (!bUseDebugLibs)
				{
					nThread = 5;	// MT
				}
				else
				{
					nThread = 2;    //MTd
				}
			}
			else
			{
				if (!bUseDebugLibs)
				{				
					nThread = 4;	// ML
				}
				else
				{
					nThread = 1;   //MLd
				}				
			}
		}
	}

	// what is the thread type?
	return nThread;
}

BOOL CCCompilerNTTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	COptionHandler * popthdlr = GetOptionHandler();
	CString strDefine;


	CProjType * pProjType;
	BOOL fTargIsDll = g_BldSysIFace.GetProjComponentMgr()->GetProjTypefromProjItem(pProjItem, pProjType) &&
				   pProjType->GetAttributes() & ImageDLL;

	BOOL fWinDll = fTargIsDll && (iUseMFC != NoUseMFC);
	BOOL fAfxDll = (iUseMFC == UseMFCInDll);

 	// our Windows '_WINDLL' define
	strDefine.LoadString(IDS_WINDLL_DEFINE);

	// add or remove all occurances of our /D"_WINDLL" from compiler defines
  	popthdlr->MungeListStrProp(MapLogical(P_MacroNames), strDefine, fWinDll, _T(','));

	// our AFX '_AFXDLL' define
	strDefine.LoadString(IDS_MFCDLL_DEFINE);

	// add or remove all occurances of our /D"_AFXDLL" from compiler defines
  	popthdlr->MungeListStrProp(MapLogical(P_MacroNames), strDefine, fAfxDll, _T(','));

	return CCCompilerTool::PerformSettingsWizard(pProjItem, fDebug, iUseMFC);
}

#if 0

BOOL COLECompilerTool::Filter( LPBUILDFILE file ){
	return 0;
};

int  COLECompilerTool::PerformBuild( int type, int stage, LPENUMACTION pActions ){
	UINT act = ACT_Complete;

	// what stage is this?
	switch (stage)
	{
		// pre?
		case TOB_Pre:
		{
			// need to rebuild?

			if (type == TOB_Clean)
			{
				// delete our .pch on re-build
				// Only deletes if AutoPch. In that case, we need to walk all the files to be compiled,
				// possibly deleting a different header for each file. In the standard case where you have
				// one file that creates the pch and others use it, we don;t need to delete; compiling the
				// file that creates it will rebuild the pch file. bobz 9/5/95

				LPACTION pAction;
				pActions->InitEnum();
				while ( (pAction = pActions->Next()) && (act == ACT_Complete) /* failure */)
				{
/*					FileRegHandle frh = NULL;
					if (GetItemAutoPCHFile(pAction->Item(), frh))
					{
						if (!g_buildengine.DeleteFile(frh, EC) && !(aob & AOB_IgnoreErrors))
							act = ACT_Error;	// failure - stop loop
						delete frh;  // used only temporarily, and not added to registry
					}
*/
				}
			}
			else if( type == TOB_Build )
			{
				// delete the .obj file if we think we should have a .pdb and we don't
				LPACTION pAction;
				pActions->InitEnum();
				while ( (pAction = pActions->Next()) && (act == ACT_Complete) /* failure */)
				{
/*
					LPBUILDFILESET pfrs = pAction->GetOutputs();
					LPBUILDFILE    frh;

					pfrs->InitEnum();
					while ( frh = pfrs->Next() )
					{
						LPCOLESTR pPath = frh->GetFilePath();
						if (_tcsicmp(pPath->GetExtension(), _TEXT(".pdb")) == 0)
						{
							if (!pPath->ExistsOnDisk())
							{
								CProject *pProject = pAction->Item()->GetProject();
								ASSERT(pProject != (CProject *)NULL);

								if (GetItemOutputFile(pAction->Item(), MapLogical(P_OutputDir_Obj), frh, _TEXT(".obj"), &pProject->GetWorkspaceDir()))
									if (!g_buildengine.DeleteFile(frh, EC) && !(aob & AOB_IgnoreErrors))
										act = ACT_Error;	// failure
							}
							break;	// done!
						}
					}
*/
				}
			}
			break;
		}

		// post?
		case TOB_Post:
			break;

		// action?
		case TOB_Stage:
			break;

		default:
			break;
	}

	if (act != ACT_Complete)
		return act;

	return /*CBuildTool::PerformBuildActions(type, stage, lstActions, aob, EC)*/ act;
};

BOOL COLECompilerTool::GenerateCommandLines( LPENUMACTION pIEnumAction ){
	CString strDescription;
	VERIFY(strDescription.LoadString(IDS_DESC_COMPILING));

  	CMapStringToPtr cmstpFlagMap; cmstpFlagMap.InitHashTable(151);
 	cmdrec *pcmdrec;

	CStringList slCommandLines;
	CStringList slDescriptions;

	CStringList strFlagList;
	CString strFilePath;
	TCHAR *pc;

	LPACTION pAction;
	pIEnumAction->InitEnum();
	while (pAction = pIEnumAction->Next())
	{
		LPBUILDFILE pFile = pAction->GetFile();
		ActionState as = pAction->m_state;

		strFilePath = (const TCHAR *)*pFile;

		CString str;
#if 1
		str+="cl /c" + strFilePath;
#else
//		if (!pAction->GetCommandOptions(str))
// 			return FALSE;
#endif


		// replace $(ENV_VARIABLES)
		int i,j;
		char value[256];
		CString newCmdLine;
		i = str.Find( "$(" );
		while( i != -1 ){
			newCmdLine += str.Left( i );
			CString temp;
			temp = str.Right( str.GetLength() - (i + 2) );
			j = temp.Find( ")" );
			if( j != -1 ) {
				CString token;
				token = temp.Left( j );
				str = str.Right( str.GetLength() - (i+j+3) );
				if( GetEnvironmentVariable( token, value, 255 ) ){
					newCmdLine += value;
				}
				i = str.Find( "$(" );
			} else {
				// open without close !!! kill the end of the command line
				// this will likely cause a build failure
				str = "";
				break;
			}
		}
		newCmdLine += str;
		str = newCmdLine;


#pragma message( "Turn on /FD hack" )
		if (Full!=MrSupportLevel())
		{
			int iFD = str.Find("/FD");
			if (-1 != iFD)
				str = str.Left(iFD) + str.Mid(iFD+3);
		}
		
		if (cmstpFlagMap.Lookup(str, (void * &)pcmdrec))
		{
			// N.B. don't re-order if /YX
			if ((as == _SrcOutOfDate) && (pcmdrec->ePchType != PchAuto))
			{	
				pcmdrec->bFront = TRUE;

				if (pcmdrec->strFront.IsEmpty())
					pcmdrec->strFront = strFilePath;	// no prolog for 1st one yet
				else
				{
					if (pcmdrec->strPerFileProlog.IsEmpty ())
					{			
						pcmdrec->strFront = (pcmdrec->strFront + _TEXT ("\"\n\"")) +
						      			strFilePath;
					}
					else
					{
						pcmdrec->strFront = (pcmdrec->strFront +
										_TEXT ("\"\n")) +
										(pcmdrec->strPerFileProlog +
										_TEXT(" \"")) +
					   		   			strFilePath;
					}
				}
			}
			else
			{
				if (pcmdrec->str.IsEmpty())
					pcmdrec->str = strFilePath;	// no prolog for 1st one yet
				else
				{
					if (pcmdrec->strPerFileProlog.IsEmpty ())
					{			
						pcmdrec->str = (pcmdrec->str + _TEXT ("\"\n\"")) +
						      			strFilePath;
					}
					else
					{
						pcmdrec->str = (pcmdrec->str +
										_TEXT ("\"\n")) +
										(pcmdrec->strPerFileProlog +
										_TEXT(" \"")) +
						      			strFilePath;
					}
				}
			}
		}	  
		else	// No existing record, must make one:
		{
			BOOL b;

			pcmdrec = new  cmdrec;
			strFlagList.AddTail(str);
			cmstpFlagMap.SetAt (str, pcmdrec);
/*

			// See if this creates a PCH file:
			// Note: /Yc overrides /Yu overrides /YX (order is important!)
			if ((pItem->GetIntProp (MapLogical(P_PchCreate), b)==valid) && b)
				pcmdrec->ePchType = PchCreate;
			else if ((pItem->GetIntProp(MapLogical(P_PchUse), b)==valid) && b)
				pcmdrec->ePchType = PchUse;
			else if ((pItem->GetIntProp(MapLogical(P_AutoPch), b)==valid) && b)
				pcmdrec->ePchType = PchAuto;
			else
				pcmdrec->ePchType = NoPch;

			// N.B. don't change order if /YX
			if ((as == _SrcOutOfDate) && (pcmdrec->ePchType!=PchAuto))
			{
				pcmdrec->bFront = TRUE;
				pcmdrec->strFront = strFilePath;
			}
			else
			{
				pcmdrec->bFront = FALSE;
				pcmdrec->str = strFilePath;
			}

			// g_prjoptengine must be setup correctly before using it below or
			// we crash.  speed:4030.
			g_prjoptengine.SetOptionHandler (GetOptionHandler());
			g_prjoptengine.SetPropertyBag (pItem);

			// See if it uses /Tc
			if ((pItem->GetIntProp (MapLogical(P_TreatFileAsC), b)==valid) && b)
			{
				str.Empty ();
				g_prjoptengine.GenerateSingleOption (IDOPT_TREATFILEAS_C, str );
				pcmdrec->strPerFileProlog =	str;
			}
			// See if it uses /Tp
			if ((pItem->GetIntProp (MapLogical(P_TreatFileAsCPP), b)==valid) && b)
			{
				if (!pcmdrec->strPerFileProlog.IsEmpty())
						pcmdrec->strPerFileProlog += _T (' ');
				str.Empty ();
				g_prjoptengine.GenerateSingleOption (IDOPT_TREATFILEAS_CPP, str );
				pcmdrec->strPerFileProlog += str;
			}
*/

		}
	}

	CStringList slCreateLines, slCreateDescs;
	CStringList slFrontLines, slFrontDescs;
	POSITION pos = strFlagList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CString str;
		str = strFlagList.GetNext(pos);

		cmstpFlagMap.Lookup(str, (void *&) pcmdrec);
		cmstpFlagMap.RemoveKey(str); // FUTURE: is this really necessary?
			
		// append strFront + prolog + str
		if (!pcmdrec->strFront.IsEmpty())
		{
			if (pcmdrec->str.IsEmpty())
			{
				pcmdrec->str = pcmdrec->strFront;
				pcmdrec->strFront.Empty();
			}
			else
			{
				if (pcmdrec->strPerFileProlog.IsEmpty ())
				{			
					pcmdrec->str = (pcmdrec->strFront + _TEXT ("\"\n\"")) +
					      			pcmdrec->str;
				}
				else
				{
					pcmdrec->str = (pcmdrec->strFront +
									_TEXT ("\"\n")) +
									(pcmdrec->strPerFileProlog +
									_TEXT(" \"")) +
					      			pcmdrec->str;
				}
			}
			pcmdrec->strFront.Empty();
		}

		// If there's a prolog string, nuke it in the main command
		// line.  Note thate we can be hosed if there's more than
		// one argument in strPerFileProlog:
		if (!pcmdrec->strPerFileProlog.IsEmpty ())
		{
			str.GetBuffer (1);
			if (pc = _tcsstr ( (const TCHAR *)str, (const TCHAR *)pcmdrec->strPerFileProlog))
			{
				_tcsnset ( pc, (int) ' ', pcmdrec->strPerFileProlog.GetLength ());
			}
			str.ReleaseBuffer ();				
			pcmdrec->strPerFileProlog += _T (' ');
		}

		str = str + _TEXT ('\n') + pcmdrec->strPerFileProlog +
				 _TEXT ('\"') + pcmdrec->str + _TEXT ("\"\n");

/*
		CPath pathtTemp;
		if (!g_buildengine.CreateTempFile(str, pathtTemp, EC))
			return FALSE;

		// quote response file path if necessary
		CString strRspPath = pathtTemp;
		if (pathtTemp.ContainsSpecialCharacters())
			strRspPath = _T('\"') + strRspPath + _T('\"');

		// use response file
		str = m_strToolExeName + _TEXT ( " @" ) + strRspPath;
*/

		if (pcmdrec->ePchType == PchCreate)
		{
			// copy to temp list (we'll prepend these to the main list later)
			if (pcmdrec->bFront)
			{
				slCreateLines.AddHead(str);
				slCreateDescs.AddHead(strDescription);
			}
			else
			{
				slCreateLines.AddTail(str);
				slCreateDescs.AddTail(strDescription);
			}
		}
		else
		{
			if ( pcmdrec->bFront)
			{
				slFrontLines.AddHead(str);
				slFrontDescs.AddHead(strDescription);
			}
			else
			{
				slCommandLines.AddTail(str);
				slDescriptions.AddTail(strDescription);
			}
		}

		delete (pcmdrec);
	}

	// copy front entries, if any, to the very front of the list
	if (!slFrontLines.IsEmpty())
	{
		slCommandLines.AddHead(&slFrontLines);
		slDescriptions.AddHead(&slFrontDescs);
		slFrontLines.RemoveAll();
		slFrontDescs.RemoveAll();
	}

	// now copy /Yc entries, if any, to the very front of the list
	if (!slCreateLines.IsEmpty())
	{
		slCommandLines.AddHead(&slCreateLines);
		slDescriptions.AddHead(&slCreateDescs);
		slCreateLines.RemoveAll();
		slCreateDescs.RemoveAll();
	}

	strFlagList.RemoveAll();

	ASSERT(slFrontDescs.IsEmpty());
	ASSERT(slCreateDescs.IsEmpty());
	ASSERT(cmstpFlagMap.IsEmpty());

	return TRUE;
	return 0;
};

BOOL COLECompilerTool::GetDependencies( LPENUMACTION pIEnumAction ){
	return 0;
};

// IDEPSCANNER
BOOL COLECompilerTool::GenerateDependencies( LPENUMACTION pIEnumAction ){
	return 0;
};

#endif

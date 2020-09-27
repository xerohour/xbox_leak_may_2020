// CBuildFile
//
// Build file reading and writing.
//
// [matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "bldrfile.h"	// local header
#include "exttarg.h"	// external targets
#include "version.h"    // for makefile versions

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// Convention: bump by 1 for OPT changes or simple makefile changes;
// Bump up to multiple of 10 for incompatible makefile format changes.

///////////////////////////////////////////////////////////////////////////////
// Makefile change history
// ALWAYS update this when bumping the OPT/Makefile version number
//
// Version number					Change							Made by
// 		30000						<reserved for release>			colint
// 		30001						Added targets					colint
//		30002						Fixed mirroring for targitems	colint
//		30003						Added file mapping				billjoy
//		30004						Removed old clw filename		colint
//      40000                       Break all old v3 makefiles      colint
//                                  (target name changes)
//      40001                       Fix INTDIR creation rule        colint
//                                  to use $(NULL), not nul


// Change these
static const UINT nMakeFileFormat = 00;	// internal makefile format number
const UINT nLastCompatibleMakefile = 40000;
const UINT nLastCompatibleOPTFile = 50009;

// Computed values do not alter
const UINT nVersionMajor = rmj;		// whole product major version
const UINT nVersionMinor = rmm;		// whole product minor version
const UINT nVersionNumber = (rmj * 10000) + (rmm * 100) + nMakeFileFormat;
const UINT nLastCompatibleMakefileMinor = ((nLastCompatibleMakefile % 10000) / 100);

#ifndef _SHIP
extern BOOL	g_bUseReleaseVersion;
#endif

// our instance of the build file reader and writer
CBuildFile AFX_DATA_EXPORT g_buildfile;

// builder file delimiters
static const TCHAR * pcBuilderMark = _TEXT("Project");
static const TCHAR * pcSectionMark = _TEXT("Section");
static const TCHAR * pcPropertyPrefix = _TEXT("PROP ");
static const TCHAR * pcEndToken = _TEXT("End");
static BOOL HasPerConfigDeps( CProjItem *pItem );

// defined in projitem.cpp
extern void CountkidsRecursively(CProjItem * pItem, int & rCount);

// helpers
int	CountInterestingChildren(CProjItem * pItem)
{
	int nCount = 1;
	if (pItem->GetContentList())
		for (POSITION pos = pItem->GetHeadPosition (); pos != (POSITION)NULL;)
		{
			CountkidsRecursively((CProjItem *) pItem->GetNext(pos), nCount);
		}

	return nCount;
}


// our tool marker
WORD CBuildFile::m_wToolMarker = 0;

CBuildFile::CBuildFile()
{
}

CBuildFile::~CBuildFile()
{
}

// write out the VC++ builder file
BOOL CBuildFile::WriteBuildFile(const CProject * pBuilder)
{
#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "msvcbldd.dll", "WriteBuildFile()", letypeBegin, 0);
#endif

	ASSERT(g_bExportMakefile || g_bWriteProject || g_bExportDeps);
	if (!g_bExportMakefile && !g_bExportDeps)
		g_bWriteProject = TRUE; // default

	// remember this builder
	m_pBuilder = (CProject *)pBuilder;

	// path of our builder
	const CPath * ppathBuilder = m_pBuilder->GetFilePath();
	ASSERT(ppathBuilder != (const CPath *)NULL);

	CPath pathPJ = *ppathBuilder;
	CPath pathEx = *ppathBuilder;
	CPath pathDep = *ppathBuilder;

	pathPJ.ChangeExtension(BUILDER_EXT);  // for M3
	if (pathPJ != *ppathBuilder)
	{
		// change it if it was something different (eg. .bld)
		m_pBuilder->SetFile(&pathPJ);
		SetWorkspaceDocDirty(); // make sure this gets written out
	}

	pathEx.ChangeExtension("mak");

#if 0	// switched this out to fix bug #873
	// REVIEW: For M1 (at least), make sure a .mak file exists on disk
	// so that we'll have a link to the associated internal project file
	// even after the format/extension changes in M2
	if (!pathEx.ExistsOnDisk())
		g_bExportMakefile = TRUE;
#endif

	// always write a .dep file if we're writing a .mak and it doesn't exist
	pathDep.ChangeExtension("dep");

#ifdef _DEBUG
	if (g_bExportMakefile && !pathDep.ExistsOnDisk())
		ASSERT(g_bExportDeps);
#endif

	// directory of our builder
	const CDir * pdirBuilder = (const CDir *)&m_pBuilder->GetWorkspaceDir();

	// no tool's used yet
	m_lstToolsUsed.RemoveAll();

	// incr. our marker
	m_wToolMarker++;

	// can we create the builder file?
	// o CountInteresingChildren() is used to provide the progression feedback range

 	// file exceptions can occur during builder file write
 	CFileException e;

	// makefile writer
	CMakFileWriter mw, pw, dw;

	int ProjSize = CountInterestingChildren(m_pBuilder);

	if (g_bWriteProject)
	{
		if (!pw.Create(pathPJ, &e, ProjSize))
		{
			CString str;
			MsgBox(Error, MsgText(str, pathPJ, IDS_PROJ_OPEN_FOR_WRITE_FAIL, &e));
			return FALSE;	// couldn't create builder file
		}
	}

	if (g_bExportMakefile)
	{
		UINT attrib = MakRW_Default | MakRW_ShowStatus | MakW_Makefile;
		if (g_bWriteProject)
			attrib = attrib & ~MakRW_ShowStatus;
		if (!mw.Create(pathEx, &e, ProjSize, attrib))
		{
			CString str;
			MsgBox(Error, MsgText(str, pathEx, IDS_PROJ_OPEN_FOR_WRITE_FAIL, &e));
			if (g_bWriteProject)
			{
				// still write the .dsp file even if export fails
				g_bExportMakefile = FALSE;
			}
			else
			{
				return FALSE;	// couldn't create makefile
			}
		}
	}
	if (g_bExportDeps)
	{
		UINT attrib = MakRW_Default | MakRW_ShowStatus | MakW_Depfile;
		if (g_bWriteProject || g_bExportMakefile)
			attrib = attrib & ~MakRW_ShowStatus;

		// REVIEW: ProjSize may not be meaningful/correct in this case:
		if (!dw.Create(pathDep, &e, ProjSize, attrib))
		{
			CString str;
			MsgBox(Error, MsgText(str, pathDep, IDS_PROJ_OPEN_FOR_WRITE_FAIL, &e));
			if (g_bWriteProject || g_bExportMakefile)
			{
				// still write the .dsp/.mak file even if dep export fails
				g_bExportDeps = FALSE;
				g_ScannerCache.EndCache();
			}
			else
			{
				return FALSE;	// couldn't create deps file
			}
		}
	}

	//Update deps if this is not a new project and there is any to be updated.
	if (g_bExportDeps)
	{
		if (!m_pBuilder->m_bProjMarkedForSave || m_pBuilder->m_bProjExtConverted)
			UpdateBeforeWriteOutMakeFile();
	}

	// Add any necessary dependencies for project dependencies to the dep graph
	AddProjDepsToGraph();

	BOOL bRetval = TRUE;

	// remember this writer
	m_ppw = &pw;
	m_pmw = &mw;
	m_pdw = &dw;

	// reset global file error
	SetFileError(CFileException::none);

	// clear cache used by MakeQuotedString()
	ClearMQSCache();

	// no properties can change from here on out
	ConfigCacheEnabler EnableCaching;

	// generic buffer
	CString str;

	// show wait cursor while save is in progress...
	CWaitCursor wc;

	TRY
	{
		//
		// write signature strings
		//

		if (g_bWriteProject)
		{
			TCHAR buf[33];
			CString str1;
			// first write workspace project signature
			VERIFY (str.LoadString (IDS_DSPROJ_SIG_STRING));
			VERIFY (str1.LoadString(IDS_DSPROJ_NAME));
			ConfigurationRecord * pcr = m_pBuilder->GetActiveConfig();
			CString strProjectName;
			if(pcr)
				pcr->GetProjectName(strProjectName);
			str1 += strProjectName;
			str1 += _T("\"");
			str += str1;
			VERIFY (str1.LoadString (IDS_PACKAGE_OWNER));
			_stprintf(buf, _T("%d>"), PACKAGE_VPROJ);
			str += str1;
			str += buf;
			pw.WriteComment(str);

			VERIFY (str.LoadString (IDS_BUILDPRJ_SIG_STRING));
	#ifndef _SHIP
			if (!g_bUseReleaseVersion)
				_ultoa (nVersionNumber, buf, 10);
			else
	#endif
			_stprintf(buf, _T("%x.%02d"), nVersionMajor, nVersionMinor);

			str += buf;
			pw.WriteComment(str);
		}

		if (g_bExportMakefile)
		{
            VERIFY (str.LoadString (IDS_EXPORT_SIG_STRING));
			str += pathPJ.GetFileName();
			mw.WriteComment(str);
		}

		if (g_bExportDeps)
		{
            VERIFY (str.LoadString (IDS_EXPORTDEPS_SIG_STRING));
			str += pathEx.GetFileName();
			dw.WriteComment(str);
			dw.EndLine();
		}

		//
		// write out version information
		//

#ifndef _SHIP
		if (!g_bUseReleaseVersion)
		{
			// Record VPROJ version number for tracking purposes
#ifdef _DEBUG
			str = "MSVCBLDD";
#else
			str = "MSVCBLD";
#endif
			{	// calculate and write VPROJ version number
				TCHAR	rgchModName[_MAX_PATH];
				DWORD	dwHandle, dwVerInfoSize;
				unsigned int dwValueSize;
				VOID *	pvVerInfo;
				VOID *	pvValue;
				if ((::GetModuleFileName(GetModuleHandle((LPCTSTR)str), rgchModName, sizeof(rgchModName) / sizeof(TCHAR)))
				 && ((dwVerInfoSize = ::GetFileVersionInfoSize(rgchModName, &dwHandle)) != 0))
				{
					pvVerInfo = new TCHAR[dwVerInfoSize];
					if ((::GetFileVersionInfo(rgchModName, dwHandle, dwVerInfoSize, pvVerInfo))
						&& (::VerQueryValue(pvVerInfo,"\\StringFileInfo\\040904E4\\FileVersion",&pvValue,&dwValueSize)))
					{
						str = str + _T(": version ") + (TCHAR *)pvValue;

						if (g_bWriteProject)
							pw.WriteComment ( str );
						if (g_bExportMakefile)
							mw.WriteComment ( str );
						if (g_bExportDeps)
							dw.WriteComment ( str );
					}
					delete [] pvVerInfo;
				}
			}
		}
#endif // !_SHIP

		if (g_bWriteProject)
		{
			//
			// write out 'do not edit' comment
			//
			VERIFY(str.LoadString(IDS_DO_NOT_EDIT_MAKEFILE));
			pw.WriteComment(str);

			pw.EndLine();
		}

		// write out the configuration header
		// o list of targets
		
		if (g_bWriteProject)
		{
			if (!m_pBuilder->WriteConfigurationHeader(*m_ppw))
				AfxThrowFileException(CFileException::generic);
		}

		if (g_bExportMakefile)
		{
			if (!m_pBuilder->WriteConfigurationHeader(*m_pmw))
				AfxThrowFileException(CFileException::generic);
		}

		//
		// write out the contents of the builder, eg. files, target referecences etc.
		//

		
		if (g_bWriteProject)
		{
			// write project open comment
			// pw.WriteSeperatorString();
			pw.WriteCommentEx("Begin %s", pcBuilderMark);

			// FUTURE: move this into the 'Builder Settings' section in the builder file

			// construct special 'PROP <name> <value>' line
#if 0
			CString strOutput;
			if (m_pBuilder->GetStrProp(P_ProjSccProjName, strOutput) == valid && !strOutput.IsEmpty())
			{
				strOutput = pcPropertyPrefix + (_T("Source_Control_Project \"") + strOutput + _T('\"'));
				pw.WriteComment(strOutput);
			}

			if (m_pBuilder->GetStrProp(P_ProjSccProjAux, strOutput) == valid && !strOutput.IsEmpty())
			{
				strOutput = pcPropertyPrefix + (_T("Source_Control_ProjAux \"") + strOutput + _T('\"'));
				pw.WriteComment(strOutput);
			}
#endif
		}

		CNameMunger nm;

		// write builder level tools section
		if (!WriteToolForNMake(m_pBuilder, &nm))
			AfxThrowFileException(CFileException::generic);

		// write each of the items the builder contains (not recursive)
		for (POSITION pos = m_pBuilder->GetHeadPosition(); pos != (POSITION)NULL;)
		{
			CProjItem * pProjItem = (CProjItem *)m_pBuilder->GetNext(pos);

			if (!pProjItem->WriteToMakeFile(pw, mw, *pdirBuilder, nm))
				AfxThrowFileException(CFileException::generic);
		}

		if (g_bWriteProject)
		{
			// write builder closing comment
			pw.WriteCommentEx("%s %s", pcEndToken, pcBuilderMark);
			// pw.WriteSeperatorString();
		}

		if (g_bWriteProject)
		{
			// 
			// write out our sections
			//
			CBldrSection * pbldsect; CString strName;
			g_BldSysIFace.InitSectionEnum();
			while ((pbldsect = g_BldSysIFace.GetNextSection(strName)) != (CBldrSection *)NULL)
			{
				if (m_pBuilder == (CProject *)pbldsect->m_hBld)  // only do this if it is our builder
				{
				if (!WriteSectionForNMake(pbldsect))
 					AfxThrowFileException(CFileException::generic);
				}
			}
		}

		if (g_bWriteProject)
		{
			// can we close the builder file?
			bRetval = pw.Close();
		}

		if (g_bExportMakefile)
		{
			// can we close the builder file?
			bRetval = mw.Close() && bRetval;
		}

		if (g_bExportDeps)
		{
			// can we close the builder file?
			bRetval = dw.Close() && bRetval;
		}
		if (!bRetval)
			AfxThrowFileException(CFileException::generic);
	}
	CATCH(CException, e)
	{
		// safely close the writer (if not already closed)
		if (bRetval)	// don't Abort() if Close() failed
		{
			
			if (g_bWriteProject)
			{
				pw.Abort();
			}
			if (g_bExportMakefile)
			{
				mw.Abort();
			}
			if (g_bExportDeps)
			{
				dw.Abort();
			}
			bRetval = FALSE;
		}

		str.Empty ();

		if ((e->IsKindOf(RUNTIME_CLASS(CFileException))) && (((CFileException *)e)->m_cause != CFileException::none))
		{
			if (GetFileError() != CFileException::none && ((CFileException *)e)->m_cause == CFileException::generic)
				((CFileException *)e)->m_cause = GetFileError();

			// REVIEW: not quite right
			// we can show the type of exception
			if (g_bWriteProject)
				MsgBox(Error, MsgText(str, pathPJ, IDS_PROJ_SAVING_FAILED, (CFileException *)e));
			else
				MsgBox(Error, MsgText(str, pathEx, IDS_PROJ_SAVING_FAILED, (CFileException *)e));
		}
		else
		{
			if (GetFileError()==CFileException::none)
				SetFileError(CFileException::generic);

			// can't show the type of exception
			MsgBox(Error, IDS_PROJ_SAVING_FAILED);
		}
	}
	END_CATCH

	// Remove proj deps from the graph
	RemoveProjDepsFromGraph();

	// for all tool's that we used tell them we've finished
	// writing the builder
	POSITION pos = m_lstToolsUsed.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CBuildTool * pTool = (CBuildTool *)m_lstToolsUsed.GetNext(pos);
		pTool->EndProjectWrite(m_pBuilder, *(CDir *)pdirBuilder);
	}

#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "msvcbldd.dll", "WriteBuildFile()", letypeEnd, 0);
#endif

	// null out the writer
	m_ppw = (CMakFileWriter *)NULL;
	m_pmw = (CMakFileWriter *)NULL;
	m_pdw = (CMakFileWriter *)NULL;

	return bRetval;
}

// 'NMake syntax' section write
BOOL CBuildFile::WriteSectionForNMake(CBldrSection * pbldsect)
{
	ASSERT(g_bWriteProject);

	// ask for the section data?
	if (pbldsect->m_pfn)
		if (!(*pbldsect->m_pfn)(pbldsect, SECT_WRITE))
			return FALSE;	// couldn't

 	// file exceptions can occur during builder file write
 	CFileException e;
	BOOL bRetval;

	TRY
	{
		TCHAR * pch = pbldsect->m_pch;
		UINT cch = pbldsect->m_cch;

		// anything to write?
		if (pch != (TCHAR *)NULL && *pch != _T('\0'))
		{
			// write section header
			// m_ppw->WriteSeperatorString();
			m_ppw->WriteCommentEx("%s %s", pcSectionMark, (const TCHAR *)pbldsect->m_strName);

			// write out section
			while (*pch != _T('\0'))
			{
				// write the first line
				m_ppw->WriteCommentEx("\t%s", pch);

				pch += _tcslen(pch);	// skip line
				pch++;					// skip terminator
			}

			// terminate section
			m_ppw->WriteCommentEx("%s %s", pcEndToken, pcSectionMark);
			// m_ppw->WriteSeperatorString();
		}

		bRetval = TRUE;		// success
	}
	CATCH(CException, e)
	{
		bRetval = FALSE;	// failure
	}
	END_CATCH

	// end section write?
	if (pbldsect->m_pfn)
		(void) (*pbldsect->m_pfn)(pbldsect, SECT_END);

	return bRetval;
}

// tool info. struct
typedef struct
{
	CBuildTool *	pTool;
	CActionSlob *	pAction;
	int				iAreas;
} S_ToolRec;

// quick 'n' dirty CFileRegEntry set compare
// used below for comparing dependency file sets
//
// only return TRUE
// if the sets contain exactly the same CFileRegEntrys in the same order
BOOL QuickNDirtyRegSetCompare(CFileRegSet * pregSet, CFileRegSet * pregSet2)
{
	// compare the two sets
#ifndef REFCOUNT_WORK
	const CPtrList * plst = pregSet->GetContent();
	const CPtrList * plst2 = pregSet2->GetContent();
#else
	const CPtrList * plst = &pregSet->m_Contents;
	const CPtrList * plst2 = &pregSet2->m_Contents;
#endif
	if (plst->GetCount() != plst2->GetCount())
		return FALSE;	// not equal

	POSITION pos = plst->GetHeadPosition();
	POSITION pos2 = plst2->GetHeadPosition();
	while (pos != (POSITION)NULL)
		if (plst->GetNext(pos) != plst2->GetNext(pos2))
			return FALSE;	// not equal

	return TRUE;	// equal!
}

int TRcompare( const void *arg1, const void *arg2 )
{
	/* Compare all of both strings: */
	CBuildTool *pTool1 = (*( S_ToolRec ** )arg1)->pTool;
	CBuildTool *pTool2 = (*( S_ToolRec ** )arg2)->pTool;

	return _stricmp( (const TCHAR *)pTool1->m_strToolPrefix,(const TCHAR *)pTool2->m_strToolPrefix  );
}


BOOL CBuildFile::WriteToolForNMake(CProjItem * pItem, CNameMunger * pnm, CPtrArray * pCfgArray /* = NULL */)
{
	// our builder directory
	const CDir * pdirBuilder = (const CDir *)&m_pBuilder->GetWorkspaceDir();

	// are we at the builder-level?
	BOOL fBldrLevel = pItem == m_pBuilder;
	BOOL fUseProjConfig = TRUE;

	// use the builder's configs?
	if (pCfgArray == (CPtrArray *)NULL)
	{
		CTargetItem *pTarget = pItem->GetTarget();
		if (!fBldrLevel && pTarget != NULL && pTarget != pItem)
		{
			fUseProjConfig = FALSE;
			pCfgArray = (CPtrArray *)pTarget->GetConfigArray();
		}
		else
			pCfgArray = (CPtrArray *)m_pBuilder->GetConfigArray();
	}

	// what tools' across *all* configs. care about this item?
	POSITION pos;
	CMapPtrToPtr mapToolToAction;
	S_ToolRec *pListToolRecs[1024];		// note that this is a rediculously large number. Reality will be closer to 4 or 5 max but why take chances
	int nLastRec=0;
	CProjTempConfigChange projTempConfigChange(m_pBuilder);
	int i;

	for ( i = pCfgArray->GetSize(); i > 0; i--)
	{
		// get the actions, transfer to tool's list
		ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(i-1);

		if (fUseProjConfig || pcr->IsValid())
		{
			// get the actions for the item in this builder config.
			projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
			CActionSlobList * pActions = pItem->GetActiveConfig()->GetActionList();


			pos = pActions->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos);

				// if not already in tool's list then add
				S_ToolRec * pToolRec;
				if (!mapToolToAction.Lookup(pAction->m_pTool, (void * &)pToolRec))
				{
					pToolRec = new S_ToolRec;
					pToolRec->pTool = pAction->m_pTool;
					pToolRec->pAction = pAction;

					mapToolToAction.SetAt(pAction->m_pTool, pToolRec);
					pListToolRecs[nLastRec] = pToolRec;
					nLastRec++;
					ASSERT(nLastRec<1024);

					// call begin builder write if not done already
					if (pAction->m_pTool->m_wBuildFileMark != m_wToolMarker)
					{
						pAction->m_pTool->m_wBuildFileMark = m_wToolMarker;
						pAction->m_pTool->BeginProjectWrite(m_pBuilder, *(CDir *)pdirBuilder);

						// note this
						m_lstToolsUsed.AddTail(pAction->m_pTool);
					}
				}
			}
		}
	}

#if 1
	// now go back and add psedo-record for any unused tools at the project level
	// if not already in tool's list then add
	if (fBldrLevel)
	{
		CProjType * pLastType = NULL;
		for ( i = pCfgArray->GetSize(); i > 0; i--)
		{
			// get the actions, transfer to tool's list
			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(i-1);
			ASSERT(fUseProjConfig);
			// if (fUseProjConfig || pcr->IsValid())
			{
				// get the actions for the item in this builder config.
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
				CBuildTool * pTool;
				S_ToolRec * pToolRec;
				CProjType * pProjType = m_pBuilder->GetProjType();
				if ((pProjType == NULL) || (pProjType == pLastType))
					continue; // been there, done that	
				pLastType = pProjType;	// remember this for next time
  				CPtrList * pToolsList = pProjType->GetToolsList();
				POSITION pos = pToolsList->GetHeadPosition();
				while (pos != NULL)
				{
					pTool = (CBuildTool *)pToolsList->GetNext(pos);
					if (!mapToolToAction.Lookup(pTool, (void * &)pToolRec))
					{
						pToolRec = new S_ToolRec;
						pToolRec->pTool = pTool;
						pToolRec->pAction = NULL;  // not handled!

						mapToolToAction.SetAt(pTool, pToolRec);
						pListToolRecs[nLastRec] = pToolRec;
						nLastRec++;
						ASSERT(nLastRec<1024);

						// call begin builder write if not done already
						if (pTool->m_wBuildFileMark != m_wToolMarker)
						{
							pTool->m_wBuildFileMark = m_wToolMarker;
							pTool->BeginProjectWrite(m_pBuilder, *(CDir *)pdirBuilder);

							// note this
							m_lstToolsUsed.AddTail(pTool);
						}
					}
				}
			}
		}
	}
#endif

	// Get a predictable order
	qsort(pListToolRecs,nLastRec,sizeof(S_ToolRec *),TRcompare);

	//
	// create our info. tool rec. list
	//

	// per-config. targets?
	BOOL fPerConfigTarg = FALSE;

	// union of our tool areas 
	int iUnionAreas = NoAreas;

	// get each of our tools
	for( i=0; i<nLastRec; i++)
	{
		// CBuildTool * pTool; S_ToolRec * pToolRec;
		// mapToolToAction.GetNextAssoc(pos, (void * &)pTool, (void * &)pToolRec);
		S_ToolRec * pToolRec = pListToolRecs[i];
		CBuildTool *pTool = pListToolRecs[i]->pTool;

		// FUTURE: improve BeginWritingSession()
		int iAreas = pToolRec->iAreas = pTool->BeginWritingSession(pItem, pToolRec->pAction, m_ppw, m_pmw, (CDir *)pdirBuilder, pnm);

		// if this is a target level tool and we are writing for the project
		// then check for recognise per-config targets at the builder-level
		if (pTool->m_fTargetTool && fBldrLevel && !fPerConfigTarg && iAreas != NoAreas)
			// we always assume per-config. targets
			fPerConfigTarg = TRUE;

		CRuntimeClass* prt = pTool->GetRuntimeClass();
		if (_tcscmp(prt->m_lpszClassName, "CCCompilerJavaTool") == 0)
			fPerConfigTarg = TRUE;

		// perform the union of the areas
		iUnionAreas |= iAreas;
	}

	// make sure we do per-config. writing if we have per-config. 'non-tool' props.
	if ((iUnionAreas & (PerConfig|PerConfigInt)) == 0)
		if (pItem->HasMakefileProperties())
			iUnionAreas |= PerConfigInt;

	if (g_bWriteProject)
	{
		// write out the per-item props.
		pItem->WritePerItemProperties(*m_ppw);
	}

	// dependencies line per-config.?
	// if per-config. targ. then yes!
	BOOL fPerConfigToolDep = fPerConfigTarg;

	if( pItem->IsKindOf(RUNTIME_CLASS(CFileItem)) && HasPerConfigDeps( pItem ) ) 
		fPerConfigToolDep = TRUE;

	UINT nMRDeps = 0;
	BOOL fPerConfigToolDepInt = FALSE;

	if (!fPerConfigToolDep)
	{
		
		CProjTempConfigChange projTempConfigChange(m_pBuilder);
		ConfigurationRecord * pcrComp = (ConfigurationRecord *)NULL;
		for ( i = pCfgArray->GetSize(); i > 0; i--)
		{
			ConfigurationRecord * pcrNext = (ConfigurationRecord *)pCfgArray->GetAt(i-1);

			if (fUseProjConfig || pcrNext->IsValid())
			{
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcrNext->m_pBaseRecord);

				ConfigurationRecord * pcr = (ConfigurationRecord *)pItem->GetActiveConfig();
   				CActionSlobList * pActions = pcr->GetActionList();
 				if (pActions->GetCount() > 1)
				{
						fPerConfigToolDep = fPerConfigToolDepInt = TRUE;
						break;	// per-config tool dependencies
				}

				if (pActions->GetCount() == 0)
				{
						continue; // ignore this one (no tool)
				}

  				CActionSlob * pAction = (CActionSlob *)pActions->GetHead();
 
				if (pAction->m_pTool->HasDependencies(pcr))
				{
					nMRDeps++;
					if (!g_bExportDeps || !m_pBuilder->m_bProj_PerConfigDeps)
						continue;		
				}
				else if (nMRDeps > 0) // inconsistent MRDeps usage
				{
					fPerConfigToolDepInt = TRUE;
					if (fPerConfigToolDep || !g_bExportMakefile)
	 					break;	// per-config tool depedencies
				}
				else if (pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
				{
					fPerConfigToolDep = TRUE;
				}
				else
				{
				
				}

				// have a config. to compare against?
				if ((pcrComp != (ConfigurationRecord *)NULL)
					 && ((pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))) || (m_pBuilder->m_bProj_PerConfigDeps))
					)
				{
					// get our per-item action list
					CActionSlobList * pActionsComp = pcrComp->GetActionList();

					CActionSlob * pActionComp = (CActionSlob *)pActionsComp->GetHead();

					// compare dependencies
					if (!QuickNDirtyRegSetCompare(pAction->GetScannedDep(), pActionComp->GetScannedDep()) ||
						!QuickNDirtyRegSetCompare(pAction->GetMissingDep(), pActionComp->GetMissingDep()) ||
						(pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))&&!QuickNDirtyRegSetCompare(pAction->GetSourceDep(), pActionComp->GetSourceDep()))
					   )
					{
						fPerConfigToolDep = TRUE;
						if (nMRDeps==0)
							fPerConfigToolDepInt = TRUE;

						if (fPerConfigToolDepInt || !g_bWriteProject)
							break;	// per-config tool depedencies
					}
				}

				// compare config.?
				pcrComp = pcr;
			}
		}
		
	}

	// if per-config. tool dep.
#if 0
	if (fPerConfigToolDep)
	{
		iUnionAreas |= PerConfigExt;
	}
#endif
	if (fPerConfigToolDepInt)
	{			
		iUnionAreas |= PerConfigInt;
	}

	if (fBldrLevel)
	{
		// look at all the projtypes in use and check for external target types
		CProjType * pProjType;
		BOOL fHasExternalTargets = FALSE;

		for ( i = pCfgArray->GetSize(); i > 0; i--)
		{
			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(i-1);
			if (g_prjcompmgr.LookupProjTypeByName(pcr->GetOriginalTypeName(), pProjType))
			{
				// do we have an external target type in use?
				// if so we cannot write the tool dep. line outside of the #ifdef blocks
				if (pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
				{
					fHasExternalTargets = TRUE;
					break;
				}
			}
		}

		if (fHasExternalTargets)
			fPerConfigToolDep = TRUE;
	}

	BOOL fRetval = TRUE;

	TRY
	{
		// can we write out the tool dep. line outside of the config. block?
		if (!fPerConfigToolDep)
		{
			CProjTempConfigChange projTempConfigChange(m_pBuilder);
// FAST_EXPORT
			int pcrIndex = m_pBuilder->GetScannedConfigIndex();
			if ((!m_pBuilder->m_bProj_PerConfigDeps) && pcrIndex >= 0 && pcrIndex < pCfgArray->GetSize())
			{
				ConfigurationRecord *pcr = (ConfigurationRecord *)pCfgArray->GetAt(pcrIndex);
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
			}
			else
// FAST_EXPORT
			if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
			{
				// need to ensure we use a valid config for deps
				BOOL bExcluded;
				ConfigurationRecord * pcrBest = (ConfigurationRecord *)NULL;
				ConfigurationRecord * pcrItem;
				ConfigurationRecord * pcr;
				int i, nSize = pCfgArray->GetSize();
				for ( i = 0; i < nSize; i++)
				{
					pcr = (ConfigurationRecord *)pCfgArray->GetAt(i);
					if (pcr->IsSupported()) // REVIEW: probably not necessary
					{
						m_pBuilder->ForceConfigActive((ConfigurationRecord *)pcr->m_pBaseRecord);
						if (pItem->GetIntProp(P_ItemExcludedFromBuild, bExcluded) != valid)
							bExcluded = FALSE;
						pcrItem = pItem->GetActiveConfig();
						ASSERT(pcr->m_pBaseRecord == pcrItem->m_pBaseRecord);
						m_pBuilder->ForceConfigActive();

						if (!bExcluded && (pcrItem->GetBuildTool() != NULL))
						{
							pcrBest = pcr;
							break;
						}
					}
				}
				if (pcrBest != NULL)
					projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcrBest->m_pBaseRecord);
			}

			if (g_bExportDeps)
			{
				// write out any dependencies
				if (!pItem->WriteMakeFileDependencies(*m_pdw, *pnm, FALSE))
					AfxThrowFileException(CFileException::generic);
			}

			if (g_bExportMakefile)
			{
#if 0				// now written as part of custom build rule
				// write out any dependencies, if necessary
				// only used for custom build rules
				if (!pItem->WriteMakeFileDependencies(*m_pmw, *pnm, FALSE))
					AfxThrowFileException(CFileException::generic);
#endif

				// write output directory macros, tool dep. line, and output directory build rule,
				if ((!pItem->WriteOutDirMacro(*m_pmw)) || (fBldrLevel && !m_pBuilder->WriteMasterDepsLine(*m_pmw, *pdirBuilder)) || (!pItem->WriteOutDirDescBlk(*m_pmw)))
				{
					AfxThrowFileException(CFileException::generic);
				}
			}

			if (g_bWriteProject)
			{
				// write out any dependencies, if necessary
				// not required if MRE deps supported
				if (!pItem->WriteMakeFileDependencies(*m_ppw, *pnm, FALSE))
					AfxThrowFileException(CFileException::generic);

				// write tool dep. line
				// if (fBldrLevel && !m_pBuilder->WriteMasterDepsLine(*m_ppw, *pdirBuilder))
				// 	AfxThrowFileException(CFileException::generic);
			}
		}

		// write headers?
		if (iUnionAreas & Header)
		{
			pos = mapToolToAction.GetStartPosition();
			for( i=0; i<nLastRec; i++)
			{
				S_ToolRec * pToolRec = pListToolRecs[i];
				CBuildTool *pTool = pListToolRecs[i]->pTool;

				// write header here if tool used by all configs
				if ((pToolRec->iAreas & (Header | BuildRule)) == (Header | BuildRule))
					pToolRec->pTool->WriteHeaderSection(pToolRec->pAction);
			}
		}

   		int nMaxIndex = iUnionAreas & (PerConfig|PerConfigInt|PerConfigExt) ? pCfgArray->GetSize() - 1 : 0;
		BOOL bFirstCfg = TRUE;

		// 0 = never write; 1 = write once; n = write per config
		int nWriteProject = 0, nExportMakefile = 0, nExportDeps = 0;
		if (g_bWriteProject)
		{
			nWriteProject = 1; 
		  	if ((iUnionAreas & (PerConfig|PerConfigInt))!=0)
				nWriteProject += nMaxIndex;
		}

		// REVIEW: need to fix logic for makefiles vs. deps
		if (g_bExportMakefile)
		{
			nExportMakefile = 1; 
		  	if ((iUnionAreas & (PerConfig|PerConfigExt))!=0)
				nExportMakefile += nMaxIndex;
		}
		if (g_bExportDeps)
		{
			nExportDeps = 1; 
			// REVIEW: can't have deps at BldrLevel
		  	if (fPerConfigToolDep && !fBldrLevel)
				nExportDeps += nMaxIndex;
		}

		CProjTempConfigChange projTempConfigChange(m_pBuilder);

		for (int k = 0; k <= nMaxIndex ; k++)
		{
			// set each configuration active in turn
			ConfigurationRecord *pcr = (ConfigurationRecord *)pCfgArray->GetAt(k);
			if (fUseProjConfig || pcr->IsValid())
			{
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

				// do this?
				if (!pItem->IsKindOf(RUNTIME_CLASS(CProject)) && 
					!pItem->GetTarget()->GetActiveConfig()->IsValid())
				{
					continue;
				}

				if (nMaxIndex)
				{
					// write IF/ELSEIF if writing multiple configs
					pItem->m_cp = bFirstCfg ? CProjItem::FirstConfig : CProjItem::MiddleConfig;
					if (nWriteProject > 1)
						pItem->WriteConfigurationIfDef(*m_ppw, pItem->GetActiveConfig());
					if (nExportMakefile > 1)
						pItem->WriteConfigurationIfDef(*m_pmw, pItem->GetActiveConfig());
					if (nExportDeps > 1)
						pItem->WriteConfigurationIfDef(*m_pdw, pItem->GetActiveConfig());
				}

				// write out properties stored in comments
				if (nWriteProject > k)
				{
					if (!pItem->WriteMakeFileProperties(*m_ppw))
						AfxThrowFileException(CFileException::generic);
				}

				if (fPerConfigToolDep)
				{
					if (nWriteProject > k)
					{
						// write out any dependencies, if necessary
						// not required if MRE deps supported
						if (!pItem->WriteMakeFileDependencies(*m_ppw, *pnm, TRUE))
							AfxThrowFileException(CFileException::generic);
					}

					if (nExportMakefile > k)
					{
#if 0					// now written as part of custom build rule
						// write out any dependencies, if necessary
						// only used for custom build rules
						if (!pItem->WriteMakeFileDependencies(*m_pmw, *pnm, TRUE))
							AfxThrowFileException(CFileException::generic);
#endif
						// write output directory macros, tool dep. line, and output directory build rule,
						if (!pItem->WriteOutDirMacro(*m_pmw) ||
							(fBldrLevel && !m_pBuilder->WriteMasterDepsLine(*m_pmw, *pdirBuilder)) ||
							!pItem->WriteOutDirDescBlk(*m_pmw)
						   )
							AfxThrowFileException(CFileException::generic);
					}
					if (nExportDeps > k)
					{
						// write out any dependencies
						if (!pItem->WriteMakeFileDependencies(*m_pdw, *pnm, TRUE))
							AfxThrowFileException(CFileException::generic);
					}
				}

				bFirstCfg = FALSE;

				if (fBldrLevel)
				{
					// get our per-item action list
					CActionSlobList * pActions = pItem->GetActiveConfig()->GetActionList();
					pos = pActions->GetHeadPosition();
					CBuildTool * pTool;
					CProjType * pProjType = m_pBuilder->GetProjType();
					ASSERT(pProjType);
				
					CPtrList * pToolsList = pProjType->GetToolsList();
					POSITION pos = pToolsList->GetHeadPosition();
					while (pos != NULL)
					{
						// get the tool
						pTool = (CBuildTool *)pToolsList->GetNext(pos);
						CActionSlob * pAction;
						S_ToolRec * pToolRec;

						// find it's area
						if (mapToolToAction.Lookup(pTool, (void * &)pToolRec))
						{
							// get the tools's action
							POSITION pos2 = pActions->Find(pTool);
							if (pos2)
							{
								pAction = (CActionSlob *)pActions->GetAt(pos2);
							}
							else
							{
								pAction = NULL;
							}
							if (pToolRec->iAreas & (PerConfig | PerConfigExt | ToolUsed))
							{
								// write builder-level header here if tool NOT used by all configs
								if (fBldrLevel)
									if ((pToolRec->iAreas & (Header | BuildRule)) == Header)
										pTool->WriteHeaderSection(pAction);
									

								// write out the tool's add/subtract data:
								if (nWriteProject > k)
									pTool->WriteAddSubtractLine(pItem);
							
								if ((nExportMakefile > k) || (pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
									|| (pTool->IsKindOf(RUNTIME_CLASS(CSpecialBuildTool))))
								{
									pTool->WritePerConfigData(pItem, pAction);
								}
							}
						}
					}	
				}
				else // fileitem
				{
					// get our per-item action list
					CActionSlobList * pActions = pItem->GetActiveConfig()->GetActionList();
					pos = pActions->GetHeadPosition();
					while (pos != (POSITION)NULL)
					{
						// get the action
						CActionSlob * pAction = (CActionSlob *)pActions->GetNext(pos);
						// get the action's tool
						CBuildTool * pTool = pAction->m_pTool;
						S_ToolRec * pToolRec;
	
						// find it's area
						if (mapToolToAction.Lookup(pTool, (void * &)pToolRec))
						{
							if (pToolRec->iAreas & (PerConfig | PerConfigExt | ToolUsed))
							{
								// write builder-level header here if tool NOT used by all configs
								if (fBldrLevel)
									if ((pToolRec->iAreas & (Header | BuildRule)) == Header)
										pTool->WriteHeaderSection(pAction);

								// write out the tool's add/subtract data:
								if (nWriteProject > k)
									pTool->WriteAddSubtractLine(pItem);
							
								if ((nExportMakefile > k) || (pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
									|| (pTool->IsKindOf(RUNTIME_CLASS(CSpecialBuildTool))))
									pTool->WritePerConfigData(pItem, pAction);
							}
							// file item?
							else if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
							{
								ASSERT((pToolRec->iAreas & BuildRule)==0);
								if ((nExportMakefile > k) || (pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
									|| (pTool->IsKindOf(RUNTIME_CLASS(CSpecialBuildTool))))
									pTool->WritePerConfigData(pItem, pAction);
							}
						}
					}	
				}
			}
		}

		// write final !ENDIF if writing multiple configs
		if (nMaxIndex)
		{
			pItem->m_cp = CProjItem::EndOfConfigs;
			if (nWriteProject > 1)
				pItem->WriteConfigurationIfDef(*m_ppw, NULL);
			if (nExportMakefile > 1)
				pItem->WriteConfigurationIfDef(*m_pmw, NULL);
			if (nExportDeps > 1)
				pItem->WriteConfigurationIfDef(*m_pdw, NULL);
		}

		// finish up by writing the tool build rule
		// and then ending the writing session
		for( i=0; i<nLastRec; i++)
		{
			S_ToolRec * pToolRec = pListToolRecs[i];
			CBuildTool *pTool = pListToolRecs[i]->pTool;

			if (pToolRec->iAreas & BuildRule)
			{
				ASSERT(!pItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
				ASSERT(pToolRec->pAction != NULL);
				if (pToolRec->pAction != NULL)
					pToolRec->pTool->WriteBuildRule(pToolRec->pAction);
			}

			if (pToolRec->iAreas != NoAreas)
				pToolRec->pTool->EndWritingSession();
		}
		projTempConfigChange.Release();

		// write include dep file here for exported makefile
		if (fBldrLevel && g_bExportMakefile)
		{
			m_pmw->EndLine ();	// REVIEW: needed?

	 		// !IF "$(NO_EXTERNAL_DEPS)" != "1"
			CString str = "\"$(NO_EXTERNAL_DEPS)\" != \"1\"";
			m_pmw->WriteDirective (CMakDirective::DTYP_IF, str);

			CPath makPathDep = *m_pBuilder->GetFilePath(); // REVIEW: get from actual path
			makPathDep.ChangeExtension(".dep");
			str = makPathDep.GetFileName();
			str = "\"" + str + "\"";

			// !IF EXISTS("foo.dep")
			CString str2 = "EXISTS("; str2 += str; str2 += ")";
			m_pmw->WriteDirective (CMakDirective::DTYP_IF, str2);

			// !INCLUDE "foo.dep"
			m_pmw->WriteDirective (CMakDirective::DTYP_INCLUDE, str);

			// !ELSE
			m_pmw->WriteDirective (CMakDirective::DTYP_ELSE, NULL);

			// !MESSAGE Warning: cannot find "devbld.dep"
			str2 = "Warning: cannot find " + str;
			m_pmw->WriteDirective (CMakDirective::DTYP_MESSAGE, str2);

			m_pmw->WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
			m_pmw->WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
			m_pmw->EndLine ();
		}
	}
	CATCH(CException, e)
	{
		fRetval = FALSE;	// failure
	}
	END_CATCH

	// update progress status
	if (g_bWriteProject)
		m_ppw->WroteObject();
	if (g_bExportMakefile)
		m_pmw->WroteObject();

	// Clean up the map.
	for( i=0; i<nLastRec; i++)
	{
		S_ToolRec * pToolRec = pListToolRecs[i];
		delete pToolRec;
	}
	mapToolToAction.RemoveAll();
	
	return fRetval;	// success?
}

static BOOL HasPerConfigDeps( CProjItem *pItem ){

	BOOL bExcluded;
	ConfigurationRecord * pcr;
	CBuildTool *pTool = NULL;
	const CPtrArray & ppcr = *pItem->GetConfigArray();
	int i, nNumConfigs = pItem->GetPropBagCount();
	for (i = 0; i < nNumConfigs; i++)
	{
		pcr = (ConfigurationRecord *)ppcr[i];
		ASSERT_VALID (pcr);
		pItem->ForceConfigActive(pcr);
		if (pItem->GetIntProp(P_ItemExcludedFromBuild, bExcluded) != valid)
			bExcluded = FALSE;
		
		if (!bExcluded)
		{
			if (pTool==NULL)
			{
				pTool = pcr->GetBuildTool();
			}
			else if (pTool != pcr->GetBuildTool())
			{
				// reset the config.
				pItem->ForceConfigActive();
		   		return TRUE;
			}
		}
	}
	
	// reset the config.
	pItem->ForceConfigActive();
	return FALSE;
}


// read in the VC++ builder file
BOOL CBuildFile::ReadBuildFile(const CProject * pBuilder)
{
	// remember this builder
	m_pBuilder = (CProject *)pBuilder;

	// no tool's used yet
	m_lstToolsUsed.RemoveAll();

	// incr. our marker
	m_wToolMarker++;

	BOOL bRetval = TRUE;

	return bRetval;
}

// 'NMake syntax' section read
#define CCH_ALLOC_BLOCK		256
BOOL CBuildFile::ReadSectionForNMake(const TCHAR * pchname)
{
	// dynamic creation of sections?
	BOOL fAddDynaSection = FALSE;

	// get a section with this name?
	CBldrSection * pbldsect = g_BldSysIFace.FindSection(pchname);
	if (pbldsect == (CBldrSection *)NULL)
	{	
		HBUILDER hBld = g_BldSysIFace.GetActiveBuilder(); //dra
		if ((pbldsect = new CBldrSection(hBld)) == (CBldrSection *)NULL)
			return FALSE;

		// set the name, and mark as async.
		pbldsect->m_strName = pchname;
		pbldsect->m_pfn = NULL;

		fAddDynaSection = TRUE;
	}

	// init.
	pbldsect->m_pch = (TCHAR *)NULL;
	pbldsect->m_cch = 0;

 	// file exceptions can occur during builder file read
 	CFileException e;
	BOOL bRetval;

	// builder file object
	CObject * pObject = (CObject *)NULL;

	TRY
	{
		TCHAR * pch;
		UINT cch = 0;
		UINT cchAlloc = CCH_ALLOC_BLOCK;
		pch = (TCHAR *)calloc(cchAlloc + 1, sizeof(TCHAR));	// alloc. one for zero-term.
		if (pch == (TCHAR *)NULL)
			AfxThrowFileException(CFileException::generic);

		pbldsect->m_pch = pch;

		// break out when we reach the end of the section or EOF
		for (;;)
		{
			// read in section
			pObject = m_pmr->GetNextElement();
			ASSERT(pObject != (CObject *)NULL);

			// reached end of builder file prematurely?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
				AfxThrowFileException(CFileException::generic);

			// read in our sections
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
			{
				TCHAR * pchComment = (TCHAR *)(const TCHAR *)((CMakComment *)pObject)->m_strText;

				// end of the section?
				if (!_tcsncmp(pchComment, pcEndToken, _tcslen(pcEndToken)))
				{
					*(pbldsect->m_pch + cch) = _T('\0'); cch++;	// zero. term

					// set out buffer size
					pbldsect->m_cch = cch;

					// provide the section data and get the return code?
					if (pbldsect->m_pfn)
						bRetval = (*pbldsect->m_pfn)(pbldsect, SECT_READ);
					else
						bRetval = TRUE;
					break;
				}

				// data length in section?
				UINT cchLen = _tcslen(pchComment);

			TryAgain:

				if ((cchLen + 1) > (cchAlloc - cch))	// incl zero. term
				{
					// need more memory so re-alloc
					cchAlloc += CCH_ALLOC_BLOCK;
					pch = (TCHAR *)realloc(pbldsect->m_pch, sizeof(TCHAR) * (cchAlloc + 1));		// alloc. one for zero-term.
					if (pch == (TCHAR *)NULL)
						AfxThrowFileException(CFileException::generic);

 					pbldsect->m_pch = pch;

					goto TryAgain;
				}

				// copy data into our buffer
				_tcscpy(pbldsect->m_pch + cch, pchComment);
				cch += cchLen;
				*(pbldsect->m_pch + cch) = _T('\0'); cch++;	// zero. term
			}

			// delete this object
			delete pObject; pObject = (CObject *)NULL;
		}
	}
	CATCH(CException, e)
	{	
		bRetval = FALSE;	// failure
	}
	END_CATCH

	// unget the last builder file element so that
	// our caller can process it
	if (pObject != (CObject *)NULL)
		m_pmr->UngetElement(pObject);

	// got a dyna-section and ok?
	if (fAddDynaSection)
	{
		// ok?
		if (bRetval)
		{
			// register this one
			g_BldSysIFace.RegisterSection((CBldrSection *)pbldsect);
		}
		else
		{
			// free up our data (we used sync. notify)
			if (pbldsect->m_pch != (TCHAR *)NULL)
				free(pbldsect->m_pch);

			// delete our section
			delete pbldsect;
		}
	}
	else
	{
		// free up our data (we used sync. notify)
		if (pbldsect->m_pch != (TCHAR *)NULL){
			free(pbldsect->m_pch);
			pbldsect->m_pch = NULL;
		}
	}

	return bRetval;
}

void CBuildFile::AddProjDepsToGraph()
{
	// not expanded
	CProject * pProject = m_pBuilder;

	const CPtrArray & ppcr = *pProject->GetConfigArray();
	ConfigurationRecord * pcr;

	CProjTempConfigChange projTempConfigChange(pProject);

	CObList ol;
	int fo = CProjItem::flt_OnlyTargRefs;
	pProject->FlattenSubtree(ol, fo);

 	for (POSITION pos = ol.GetHeadPosition(); pos != NULL; )
	{
		CProjectDependency * pProjectDep = (CProjectDependency *) ol.GetNext(pos);
		if (pProjectDep->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		{
			CTargetItem * pTarget = pProjectDep->GetTarget();
			const CPtrArray & ppcr = *pTarget->GetConfigArray();

			int ccr = ppcr.GetSize();
			for (int icr = 0; icr < ccr; icr++)
			{
				pcr = (ConfigurationRecord *)ppcr[icr];
				if (pcr->IsValid())
				{
                    pcr = (ConfigurationRecord *)pcr->m_pBaseRecord;
					projTempConfigChange.ChangeConfig(pcr);

	 				ConfigurationRecord * pcrProjDep = pProjectDep->GetTargetConfig();

					if (pcrProjDep != NULL)
					{
						PROJDEPREFINFO * pProjDepRefInfo = new PROJDEPREFINFO;
						pProjDepRefInfo->pProjDep = pProjectDep;
						pProjDepRefInfo->pcr = pcr;	 // REVIEW

						m_lstProjDepRefs.AddTail(pProjDepRefInfo);
					
						CActionSlob::AddRefFileItem(pProjectDep);
					}
				}
			}
		}
	}
}

void CBuildFile::RemoveProjDepsFromGraph()
{
	CProject * pProject = m_pBuilder;

	CProjTempConfigChange projTempConfigChange(pProject);

	while (!m_lstProjDepRefs.IsEmpty())
	{
		PROJDEPREFINFO * pProjDepRefInfo = (PROJDEPREFINFO *)m_lstProjDepRefs.RemoveHead();

		projTempConfigChange.ChangeConfig (pProjDepRefInfo->pcr);
		CActionSlob::ReleaseRefFileItem(pProjDepRefInfo->pProjDep);
		delete pProjDepRefInfo;
	}
}

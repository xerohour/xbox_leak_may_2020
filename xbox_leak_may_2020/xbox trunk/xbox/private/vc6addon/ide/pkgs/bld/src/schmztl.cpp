
//
//	SCHMZTL.CPP
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
					
#pragma hdrstop

#include "progdlg.h"
#include "dirmgr.h"
#include "projcomp.h"

#include "toollink.h"	// Needed to define CLinkerTool

#include "toolsdlg.h"

IMPLEMENT_DYNAMIC ( CSchmoozeTool, CBuildTool )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CProjComponentMgr g_prjcompmgr;

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ CSchmoozeTool ------------------------------
//
///////////////////////////////////////////////////////////////////////////////

// Default CTOR
CSchmoozeTool::CSchmoozeTool()
{
}

// Default DTOR
CSchmoozeTool::~CSchmoozeTool()
{
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSchmoozeTool::IsConsumableFile(const CPath * pPath)
{
	ASSERT (FALSE);
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSchmoozeTool::IsProductFile(const CPath * pPath)
{
	ASSERT (FALSE);
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSchmoozeTool::IsDelOnRebuildFile(const CPath * pPath)
{
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
int	CSchmoozeTool::BeginWritingSession
(
	CProjItem * pItem,
	CActionSlob * /* pAction */,	// not used
	CMakFileWriter *ppw,
	CMakFileWriter *pmw,
	const CDir *pBaseDir,
	CNameMunger *pnm
)
{
	int usecnt;
	int wa = ToolUsed | Header;	// defaults if used
	CProjItem * pTheItem = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
	// By default, target tools care only about the projects
	if (!pTheItem->IsKindOf(RUNTIME_CLASS(CProject)) ||
		!(usecnt = GetToolUsedCount((CProject *)pTheItem))
	   )
		return NoAreas;

	m_pPrjWriter = ppw;
	m_pMakWriter = pmw;
	m_pBaseDir = pBaseDir;
	m_pNameMunger = pnm;

	// also show these target-level tools per-config.
	wa |= PerConfig;

	return wa;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSchmoozeTool::WritePerConfigData(CProjItem * pItem, CActionSlob * pAction)
{
	if (!g_bExportMakefile)
		return TRUE;	// nothing to do unless makefile

	if (!PresentInActiveConfig(pItem))
		return TRUE;

	if (pAction && !PresentInActiveConfig(pAction->m_pItem))
		return TRUE;

	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CProject)) || pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)));

	TRY
	{
//		if (g_bExportMakefile)
//		{
			CProject * pProject = (CProject *)pItem->GetProject();

			g_prjoptengine.SetOptionHandler(GetOptionHandler());
			g_prjoptengine.SetPropertyBag(pProject);

			CString strOptions;

			g_prjoptengine.GenerateString(strOptions, OBShowMacro | OBShowDefault | OBShowFake);
			m_pMakWriter->WriteMacro(GetMacs().pszFlagsMacro, strOptions);

			if (pAction != NULL)
			{
				// write multi-inputs macro
				if (!WriteMultiInputs(pAction))
					AfxThrowFileException(CFileException::generic);

				// write per-config build-rule
				if (!DoWriteBuildRule(pAction))
					AfxThrowFileException(CFileException::generic);
			}
			//		}
	}
	CATCH (CException, e)
	{
		return FALSE;	// failure
	}	
	END_CATCH

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSchmoozeTool::WriteBuildRule(CActionSlob * pAction)
{
	ASSERT (PresentInActiveConfig(pAction->m_pItem));

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////

void GetSpecialCommandLines( CProjItem* pItem, BOOL bPreLink, CString& strCmd );

BOOL CSchmoozeTool::DoWriteBuildRule(CActionSlob * pAction)
{
	ASSERT(g_bExportMakefile);
	if (!g_bExportMakefile)
		return TRUE;	// nothing to do unless makefile

	CString strTargets, strBuildLine;
	const SchmoozeData& sd = GetMacs();
	ASSERT (PresentInActiveConfig ( pAction->m_pItem ));
	
	TRY
	{
		// If we are trying to write a project level build rule and we have
		// no inputs then this build rule is irrelevant so we don't write it
		if (pAction->m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) &&
#ifndef REFCOUNT_WORK
			pAction->GetInput()->GetContent()->IsEmpty())
#else
			pAction->GetInput()->IsEmpty())
#endif
			return TRUE;
 
		CObList lstItems; lstItems.AddHead(pAction->m_pItem);

		// Do the targets : dependents line:
		MakeQuotedString(
						pAction->GetOutput(),
						strTargets,
						m_pBaseDir,
						TRUE,
						NULL,
						(DWORD)this,
						_TEXT(" "), TRUE, // first
						(const CObList *)&lstItems
						,FALSE // bot sorted
						);

		// outdir pseudo-dep: $(OUTDIR), if defined
		CString strDeps;
		if (pAction->m_pItem->GetOutDirString(strDeps, NULL, FALSE))
			strBuildLine = _TEXT("\"") + strDeps + _TEXT("\"");	// quote for LFN

		strBuildLine += sd.pszSpecialConsumeEx;
		strBuildLine += sd.pszConsumeMacroEx;
		m_pMakWriter->WriteDesc(strTargets, strBuildLine);

		if (_tcsicmp(sd.pszConsumeMacroEx, _TEXT (" $(MFILE32_FILES)") ) == 0)
		{
			CFileRegSet * pset = (CFileRegSet *)pAction->GetInput();
			ASSERT(pset->IsKindOf(RUNTIME_CLASS(CFileRegSet)));
			CDir * pProjDir = &pAction->m_pItem->GetProject()->GetWorkspaceDir();
			CStringList strlstDownLoad;
			strlstDownLoad.AddTail(strBuildLine);
			CPath* ptmpPath = pAction->m_pItem->GetProject()->GetTargetFileName();
			ASSERT(NULL!=ptmpPath);
			CPath pPathTarget = *ptmpPath;
			delete ptmpPath;
			pPathTarget.ChangeExtension(_TEXT("TRG"));

			CString strTempTRG = pPathTarget.GetFileName();

			CString strRemoteTarget;

			strRemoteTarget = _TEXT("\"$(MFILE32_DEST):");

			int i;
			VERIFY(pAction->m_pItem->GetProject()->GetIntProp(P_DeferredMecr, i));

			FileRegHandle frhInput;
			pset->InitFrhEnum();
			CString strInput;
			while ((frhInput = pset->NextFrh()) != (FileRegHandle)NULL)
			{
				const CPath * pPath = pAction->m_pregistry->GetRegEntry(frhInput)->GetFilePath();
				CString strExt = pPath->GetExtension();
			
				// quote this input and make relative to project directory?
				if (!pPath->GetRelativeName(*pProjDir, strInput, TRUE, FALSE))
#ifndef REFCOUNT_WORK
					continue;
#else
				{
					frhInput->ReleaseFRHRef();
					continue;
				}
#endif
				
				// base file name
				CString strBaseName = pPath->GetFileName();

				strBuildLine = sd.pszExeMacroEx;
				strBuildLine +=	sd.pszFlagsMacroEx;
				if (strExt.CompareNoCase(".tlb")==0)
				{
					// special option for .tlb file
					strBuildLine += _TEXT(" -c Ole2 -t OTLB ");
				}
				else
				{
					strBuildLine += _TEXT(" ");
				}

				strBuildLine +=	strInput + _TEXT(" ");
				strBuildLine += strRemoteTarget + strBaseName + _TEXT("\"");
				if (i == dabNotDeferred)
				{
					strBuildLine += _TEXT(">\"$(OUTDIR)\\") + strTempTRG + _TEXT("\"");
				}
				m_pMakWriter->Indent ();
				m_pMakWriter->WriteString (strBuildLine);
				m_pMakWriter->EndLine ();
				strlstDownLoad.AddTail(strBuildLine);
#ifdef REFCOUNT_WORK
				frhInput->ReleaseFRHRef();
#endif
			}
			POSITION pos = strlstDownLoad.GetHeadPosition();
			strBuildLine = strlstDownLoad.GetNext(pos);
			m_pMakWriter->WriteDesc(_TEXT("DOWNLOAD"), strBuildLine);
			while (pos != (POSITION)NULL)
			{
				strBuildLine = strlstDownLoad.GetNext(pos);
				m_pMakWriter->Indent ();
				m_pMakWriter->WriteString (strBuildLine);
				m_pMakWriter->EndLine ();
			}
		}
		else
		{
#ifndef REFCOUNT_WORK
			if (!pAction->GetInput()->GetContent()->IsEmpty())
#else
			if (!pAction->GetInput()->IsEmpty())
#endif
			{
				CString strPrelinkCmd;
				strBuildLine.Empty();

				// Should we write a pre-link step.
				if (IsKindOf(RUNTIME_CLASS(CLinkerTool)))
				{
					GetSpecialCommandLines(pAction->m_pItem,TRUE,strPrelinkCmd);
				}

				if (!strPrelinkCmd.IsEmpty())
				{
					strBuildLine = strPrelinkCmd;
				}
				strBuildLine += sd.pszExeMacroEx;
				strBuildLine += _TEXT (" @<<\r\n ");
				strBuildLine +=	sd.pszFlagsMacroEx;
				if (*sd.pszSpecialFlagsEx)
					strBuildLine += sd.pszSpecialFlagsEx;
				strBuildLine += sd.pszConsumeMacroEx;
				strBuildLine += _TEXT ("\r\n<<");

				m_pMakWriter->Indent ();
				m_pMakWriter->WriteString (strBuildLine);
				m_pMakWriter->EndLine ();
				m_pMakWriter->EndLine ();
			}
		}
		
	}
	CATCH (CException, e)
	{
		return FALSE;	// failure
	}	
	END_CATCH

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSchmoozeTool::WriteHeaderSection(CActionSlob * pAction)
{
	if (g_bWriteProject)
		m_pPrjWriter->WriteMacro(m_strToolPrefix, m_strToolExeName);
	if (g_bExportMakefile)
		m_pMakWriter->WriteMacro(m_strToolPrefix, m_strToolExeName);

	return TRUE;	// success
}

void GetSpecialCommandLines( CProjItem* pItem, BOOL bPreLink, CString& strCmd )
{
	CStringList slCmds;
	CStringList slDesc;

	// generate command-lines for custom-build tool
	CString strFilePath, strCmdLine, strDescription;
	CString strCommand;
	CProject* pProject = pItem->GetProject();

	int idCmds;
	int idDesc;

	strCmd.Empty();

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
		return;
	}

	strDescription.Empty();
	pItem->GetStrProp(idDesc, strDescription);

	// Restore original behavior.
	pItem->SetOptBehaviour(optbehOld);


	COptionList lstCmdSpec(_T('\t'), FALSE,TRUE);
	lstCmdSpec.SetString((const TCHAR *)strCommand);
	POSITION pos = lstCmdSpec.GetHeadPosition();

	BOOL fFirst = TRUE;

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
}

///////////////////////////////////////////////////////////////////////////////


/*
 *
 * toolxbe.cpp
 *
 * Image builder tool
 *
 */

#include "stdafx.h"
#pragma hdrstop
#include "toolxbe.h"

IMPLEMENT_DYNAMIC(CXbeBuilderTool, CSchmoozeTool)

CSchmoozeTool::SchmoozeData g_schdataXbeBuild =
{
		    _TEXT ("XBE"),		 		//pszExeMacro;
		    _TEXT ("XBE_FLAGS"),  		//pszFlagsMacro;
			_TEXT (""),						//pszSpecialFlags;
		    _TEXT ("XBE_OBJS"),			//pszConsumeMacro;
			_TEXT (""),				//pszSpecialConsume;
		    _TEXT (" $(XBE)"),			//pszExeMacroEx;
		    _TEXT (" $(XBE_FLAGS)"),   	//pszFlagsMacroEx;
			_TEXT (""),						//pszSpecialFlagsEx;
		    _TEXT (" $(XBE_OBJS)"), 		//pszConsumeMacroEx;
			_TEXT (""),			//pszSpecialConsumeEx;
		    _TEXT("xbe"),		//pszProductExtensions;
		    _TEXT("xbe"), //pszDelOnRebuildExtensions;
};

CXbeBuilderTool::CXbeBuilderTool()
{
	m_strToolExeName = _TEXT("imagebld.exe");
	m_nIDName = IDS_XBEBUILD_TOOL;
	m_nIDUIName = IDS_XBEBUILD_TOOL_UI;
	m_strToolInput = _TEXT("*.exe;*.dll");
	m_strToolPrefix = _TEXT("XBE");
	m_nOrder = 1;
}

BOOL CXbeBuilderTool::GenerateDependencies(CActionSlob *pAction, FileRegHandle frh, CErrorContext &EC)
{
	return TRUE;
}

BOOL CXbeBuilderTool::GenerateOutput(UINT type, CActionSlobList & lstActions, CErrorContext & EC)
{
	POSITION posAction = lstActions.GetHeadPosition();
	while(posAction != NULL) {
		CActionSlob *pAction = (CActionSlob *)lstActions.GetNext(posAction);
		CProjItem *pItem = pAction->m_pItem;

		if(type & AOGO_Primary) {
			CString strName;

			CDir *pDir = &pItem->GetProject()->GetWorkspaceDir();

			/* Find the output XBE name */
			if(pItem->GetStrProp(MapLogical(P_XbeName), strName) != valid)
				strName = "";

			CPath path;

			if(!path.CreateFromDirAndFilename(*pDir, strName) ||
					!pAction->AddOutput(&path))
				return FALSE;
		}

		/* We have no other file output types */
	}

	return TRUE;
}

BOOL CXbeBuilderTool::AffectsOutput(UINT idPropL)
{
    return idPropL == P_XbeName;
}

BOOL CXbeBuilderTool::GetCommandLines(CActionSlobList & lstActions,
	CPtrList & plCommandLines, DWORD attrib, CErrorContext & EC)
{
	POSITION posAction = lstActions.GetHeadPosition();

	while(posAction != NULL) {
		CActionSlob *pAction = (CActionSlob*)lstActions.GetNext(posAction);
		CProjItem *pItem = pAction->Item();
		CString strCmdLine;

		// project directory of this item
		CDir * pProjDir = &pItem->GetProject()->GetWorkspaceDir();

		/* Start with the options */
		if(!pAction->GetCommandOptions(strCmdLine))
			return FALSE;
		ReplaceEnvVars(strCmdLine);

		/* Now add our inputs */
		CFileRegSet * pset = (CFileRegSet *)pAction->GetInput();
		ASSERT(pset->IsKindOf(RUNTIME_CLASS(CFileRegSet)));

		FileRegHandle frhInput;
		pset->InitFrhEnum();
		CString strInput;
		while ((frhInput = pset->NextFrh()) != (FileRegHandle)NULL)
		{
			const CPath * pPath = pAction->m_pregistry->GetRegEntry(frhInput)->GetFilePath();

			// quote this input and make relative to project directory?
			pPath->GetRelativeName(*pProjDir, strInput, TRUE, FALSE);
			// If GetRelativeName failed, it's probably because the two
			// paths are on different drives, so we just can't make a 
			// relative path.  In this case, however, pPath must contain
			// at least a drive letter (and will probably be a full path)
			// Even if relativization fails, the string will still be quoted

			strCmdLine += _T('\n');
			strCmdLine += strInput;
#ifdef REFCOUNT_WORK
			frhInput->ReleaseFRHRef();
#endif
		}
		strCmdLine += _T('\n');

		// may create a tmp. file if cmdline too long
		if (!g_buildengine.FormCmdLine(m_strToolExeName, strCmdLine, EC))
			return FALSE;	// error

		CCmdLine *pCmdLine = new CCmdLine;
		pCmdLine->slCommandLines.AddHead(strCmdLine);

		// our description
		CString strDescription;
		strDescription.LoadString(IDS_DESC_XBEBUILD);
		pCmdLine->slDescriptions.AddHead(strDescription);
		pCmdLine->nProcessor = 1;

		plCommandLines.AddTail(pCmdLine);
	}
	return TRUE;
}

BOOL CXbeBuilderTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	return TRUE;
}

BOOL CXbeBuilderTool::IsProductFile(const CPath *pPath)
{
	return FileNameMatchesExtension (pPath, g_schdataXbeBuild.pszProductExtensions);
}

BOOL CXbeBuilderTool::IsDelOnRebuildFile ( const CPath *pPath )
{
	return FileNameMatchesExtension(pPath, g_schdataXbeBuild.pszDelOnRebuildExtensions);
}

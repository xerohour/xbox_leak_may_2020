//
// External Target type
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "exttarg.h"	// our local header file

IMPLEMENT_DYNAMIC(CProjTypeExternalTarget, CProjType)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// implementation

BOOL CProjTypeExternalTarget::FInit()
{
	// Tools that we can use.
	// NO TOOLS at all!
 
	return CProjType::FInit();
}

BOOL CProjTypeExternalTarget::SetDefaultDebugTargetOptions(CProject * pProject,  CTargetItem * pTarget, BOOL fUseMFC)
{
	CPath pathT;
	CPath pathTemp;
	CString strProjDir=_TEXT("");

	pathT = *(pProject->GetFilePath());
	pathTemp = pathT;

	// set up per target dir for all the properties
	if (!(pTarget->GetTargDir()).IsEmpty() )
	{
		CDir dirTarg; dirTarg.CreateFromString( pTarget->GetTargDir() );
		CPath path;
		path.CreateFromDirAndFilename( dirTarg,"t");
		strProjDir = GetRelativeName((TCHAR *)(const TCHAR *)path, (TCHAR *)(const TCHAR *)pProject->GetWorkspaceDir());
		strProjDir = strProjDir.Left( strProjDir.GetLength() - 1 );


		// remove the trailing or leading '\\'
		if (!strProjDir.IsEmpty())
		{
			if (strProjDir.GetAt(strProjDir.GetLength()-1) == _T('\\'))
			{
				strProjDir = strProjDir.Left(strProjDir.GetLength()-1);	
			}
			if (!strProjDir.IsEmpty() && strProjDir.GetAt(0) == _T('\\'))
			{
				strProjDir = strProjDir.Right(strProjDir.GetLength()-1);	
			}
		}
	}

	CString str;
	CString strFileName;

	// Set the default properties for the debug configuration:
	str = "NMAKE /f ";

	if (strProjDir.IsEmpty())
	{
		CString strFile = pathT.GetFileName();
		if( strFile.CompareNoCase(_TEXT("makefile")) ){
			pathT.ChangeExtension(".mak");	
		}
		strFileName = pathT.GetFileName();
	}
	else
	{
		strFileName = strProjDir + _TEXT(".mak");
	}

	if (ScanPathForSpecialCharacters(strFileName))
	{
		str += _T('\"');
		str += strFileName;
		str += _T('\"');
	}
	else
	{
		str += strFileName;
	}
	pProject->SetStrProp(P_Proj_CmdLine, str);

	str = "/a";
	pProject->SetStrProp(P_Proj_RebuildOpt, str);

	if (strProjDir.IsEmpty())
	{
 		pathTemp.ChangeExtension(".exe");
		str = pathTemp.GetFileName();
	}
	else
	{
		str = strProjDir + _TEXT("\\");
		str += strProjDir + _TEXT(".exe");
	}
 	pProject->SetStrProp(P_Proj_Targ, str);
	pProject->SetStrProp(P_Caller, str);

	if (strProjDir.IsEmpty())
	{
 		pathTemp.ChangeExtension(".bsc");
		str = pathTemp.GetFileName();
	}
	else
	{
		str = strProjDir + _TEXT("\\");
		str += strProjDir + _TEXT(".bsc");
	}
	pProject->SetStrProp(P_Proj_BscName, str);

	if (strProjDir.IsEmpty())
	{
 		pathTemp.ChangeExtension(".clw");
		str = pathTemp.GetFileName();
	}
	else
	{
		str = strProjDir + _TEXT("\\");
		str += strProjDir + _TEXT(".clw");
	}
	//pProject->SetStrProp(P_ProjClsWzdName, str);

	str = _T("");
	//pProject->SetStrProp(P_Proj_WorkingDir, str);
	pProject->SetStrProp(P_WorkingDirectory, str);
	pProject->SetStrProp(P_Args, str);
	pProject->SetIntProp(P_PromptForDlls, 1);
	pProject->SetStrProp(P_RemoteTarget, str);
 
	return TRUE;
}

BOOL CProjTypeExternalTarget::SetDefaultReleaseTargetOptions(CProject * pProject, CTargetItem * pTarget, BOOL fUseMFC)
{
	// Both debug and release options are equivalent at the moment.
	return SetDefaultDebugTargetOptions(pProject, pTarget, fUseMFC);
}

#ifdef VB_MAKEFILES
BOOL CProjTypeExternalTarget::SetDefaultVBTargetOptions(CProject * pProject,  CTargetItem * pTarget, int projType, VBInfo *pVBProj )
{
	CPath pathT;
	CPath pathTemp;
 	CPath pathOutput;
 
 	CString strProjDir=_TEXT("");

	pathOutput.Create(pVBProj->m_ExeName32);
	pathT = *(pProject->GetFilePath());
	pathTemp = pathT;

	// set up per target dir for all the properties
	if (!(pTarget->GetTargDir()).IsEmpty() )
	{
		CDir dirTarg; dirTarg.CreateFromString( pTarget->GetTargDir() );
		CPath path;
		path.CreateFromDirAndFilename( dirTarg,"t");
		strProjDir = GetRelativeName((TCHAR *)(const TCHAR *)path, (TCHAR *)(const TCHAR *)pProject->GetWorkspaceDir());
		strProjDir = strProjDir.Left( strProjDir.GetLength() - 1 );


		// remove the trailing or leading '\\'
		if (!strProjDir.IsEmpty())
		{
			if (strProjDir.GetAt(strProjDir.GetLength()-1) == _T('\\'))
			{
				strProjDir = strProjDir.Left(strProjDir.GetLength()-1);	
			}
			if (!strProjDir.IsEmpty() && strProjDir.GetAt(0) == _T('\\'))
			{
				strProjDir = strProjDir.Right(strProjDir.GetLength()-1);	
			}
		}
	}

	CString str;
	CString strFileName;

	// Set the default properties for the debug configuration:
	str = "VB5 /make ";

	CString strFile = pathT.GetFileName();
	strFileName = pathT.GetFileName();

	if (ScanPathForSpecialCharacters(strFileName))
	{
		str += _T('\"');
		str += strFileName;
		str += _T('\"');
	}
	else
	{
		str += strFileName;
	}
	pProject->SetStrProp(P_Proj_CmdLine, str);

	str = "";
	pProject->SetStrProp(P_Proj_RebuildOpt, str);

	if( projType == 0 ){
		if (strProjDir.IsEmpty())
		{
			pathTemp.ChangeExtension(".exe");
			if( !pVBProj->m_ExeName32.IsEmpty() )
				str = pathOutput.GetFileName();
			else
				str = pathTemp.GetFileName();
		}
		else
		{
			str = strProjDir + _TEXT("\\");
			str += strProjDir + _TEXT(".exe");
		}
	}
	else if ( projType == 1 ){
		pathTemp.ChangeExtension(".ocx");
		if( !pVBProj->m_ExeName32.IsEmpty() )
			str = pathOutput.GetFileName();
		else
			str = pathTemp.GetFileName();
	}
	else if ( projType == 2 ){
		pathOutput.ChangeExtension(".dll");
		// str = pathTemp.GetFileName();
		str = pathOutput.GetFileName();
	}

	pProject->SetStrProp(P_Proj_Targ, str);

	pProject->SetStrProp(P_Caller, str);


	str = _T("");
	//pProject->SetStrProp(P_Proj_WorkingDir, str);
	pProject->SetStrProp(P_WorkingDirectory, str);
	pProject->SetStrProp(P_Args, str);
	pProject->SetIntProp(P_PromptForDlls, 1);
	pProject->SetStrProp(P_RemoteTarget, str);
 
	return TRUE;
}
#endif


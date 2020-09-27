//
// Intelx86 Compiler Tool
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "x86toolc.h"	// our local header file

IMPLEMENT_DYNAMIC ( CCCompilerX86Tool, CCCompilerNTTool )

CCCompilerX86Tool::CCCompilerX86Tool() : CCCompilerNTTool()
{
	m_strToolExeName = _TEXT("cl.exe");
}

//////////////////////////////////////////////////////////////////////////////
// Default tool options for the Intelx86 compiler tool
BOOL CCCompilerX86Tool::GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption)
{
	// are we a mode we can handle?
	VSASSERT(mode == MOB_Debug || mode == MOB_Release, "Unrecognized mode!");
	
	CString strPart; 

	// Our common tool options
	BOOL bOK = strOption.LoadString(IDS_WIN32_CPLR_COMMON);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");

	// Our mode tool options
 	bOK = strPart.LoadString(mode == MOB_Debug ? IDS_WIN32_CPLR_DBG : IDS_WIN32_CPLR_REL);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	strOption += _T(' '); strOption += strPart;

	// Return appropriate tool options according to the attributes
	// of the target type
	UINT nIDAttrib = (UINT)-1;

	// console? (takes precedence over executable, executable is implicit...)
	if (attrib & SubsystemConsole)
	{
		nIDAttrib = mode == MOB_Debug ? IDS_WIN32CON_CPLR_DBG : IDS_WIN32CON_CPLR_REL;
	}
	// executable
	else if (attrib & ImageExe)
	{
		nIDAttrib = mode == MOB_Debug ? IDS_WIN32EXE_CPLR_DBG : IDS_WIN32EXE_CPLR_REL;
	}
	// dynamic link library
	else if (attrib & ImageDLL)
	{
		nIDAttrib = mode == MOB_Debug ? IDS_WIN32DLL_CPLR_DBG : IDS_WIN32DLL_CPLR_REL;
	}
	// static library
	else if (attrib & ImageStaticLib)
	{
		nIDAttrib = mode == MOB_Debug ? IDS_WIN32LIB_CPLR_DBG : IDS_WIN32LIB_CPLR_REL;
	}	 

	// Our attribute tool options
	if (nIDAttrib != (UINT)-1)
	{
		bOK = strPart.LoadString(nIDAttrib);
		VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
		strOption += _T(' '); strOption += strPart;
	}

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
// Ensure that this project compiler Intelx86 options are ok to use/not use MFC
BOOL CCCompilerX86Tool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	// Set the thread type usage
	pProjItem->SetIntProp(MapLogical(P_Thread), 1);

	return CCCompilerNTTool::PerformSettingsWizard(pProjItem, fDebug, iUseMFC);
}

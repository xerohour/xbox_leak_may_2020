//
// Intelx86 Compiler Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "x86optn.h"	// our local header file

IMPLEMENT_DYNAMIC(CCCompilerX86Tool, CCCompilerNTTool)
IMPLEMENT_DYNAMIC(CLinkerX86Tool, CLinkerNTTool)

//----------------------------------------------------------------
// our INTELx86 compiler option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(CompilerIntelx86, (UINT)NULL, (UINT)NULL, P_Cpux86, P_GZ, TRUE)
    IDOPT_CPUX86,       "G%{3|4|5|6|B}1",				OPTARGS1(P_Cpux86),                     single,
	IDOPT_CALLCONV,		"G%{d|r|z}1",				OPTARGS1(P_CallConv),					single,
	IDOPT_GZ,           "GZ%T1",                    OPTARGS1(P_GZ),                 single,
	IDOPT_BYTE,			"Zp%{2|4|8|16|[1]}1",		OPTARGS1(P_ByteAlign),					single,
	IDOPT_THREAD,		"M%{Ld|Td|Dd|L|T|D}1",	OPTARGS1(P_Thread),						single,

// compatibility with VC++ 1.x compiler options, we'll read them
// in then nuke them ;-)
	IDOPT_FLOATPOINT,	"FP%{i87|i|a|c87|c}1",		OPTARGS1(P_FloatPoint),					single,

// run-time checks
	IDOPT_CPP_RTC,		"RTC%{1|2}1",				OPTARGS1(P_RTC),				single,	
	IDOPT_CPP_RTC_StackFrame,	"RTCs%T1%<3>2",		OPTARGS2(P_RTC_StackFrame, P_RTC),	single,		
	IDOPT_CPP_RTC_MemoryAccess,	"RTCm%T1%<3>2",		OPTARGS2(P_RTC_MemoryAccess, P_RTC),	single,
	IDOPT_CPP_RTC_MemoryLeaks,	"RTCl%T1%<3>2",		OPTARGS2(P_RTC_MemoryLeaks, P_RTC),	single,
	IDOPT_CPP_RTC_SmallerType,	"RTCc%T1%<3>2",		OPTARGS2(P_RTC_SmallerType, P_RTC),	single,	
	IDOPT_CPP_RTC_VectorNew,	"RTCv%T1%<3>2",		OPTARGS2(P_RTC_VectorNew, P_RTC),	single,	
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(CompilerIntelx86)
	OPTDEF_INT(Cpux86, 5)
	OPTDEF_INT(CallConv, 1)
	OPTDEF_INT(ByteAlign, 3)
	OPTDEF_INT(Thread, 4)
	OPTDEF_INT(FloatPoint, -1)
	OPTDEF_INT(RTC, 0)
	OPTDEF_BOOL(RTC_StackFrame, FALSE)
	OPTDEF_BOOL(RTC_MemoryAccess, FALSE)
	OPTDEF_BOOL(RTC_MemoryLeaks, FALSE)
	OPTDEF_BOOL(RTC_SmallerType, FALSE)
	OPTDEF_BOOL(RTC_VectorNew, FALSE)
	OPTDEF_BOOL(GZ, FALSE)
END_OPTDEF_MAP()

//----------------------------------------------------------------
// our INTELx86 linker option strings
//----------------------------------------------------------------

BEGIN_OPTSTR_TABLE(LinkerIntelx86, (UINT)NULL, (UINT)NULL, (UINT)NULL, (UINT)NULL, TRUE)
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerIntelx86)
END_OPTDEF_MAP()

// compiler tool option default map 'faking'
BOOL OLD_OPTION_HANDLER(CompilerIntelx86)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_Thread);
}


GPT OLD_OPTION_HANDLER(CompilerIntelx86)::GetDefIntProp(UINT idProp, int & nVal)
{
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefIntProp(idProp, nVal);

	FormFakeIntProp(idProp, nVal);
	return valid;
}

void OLD_OPTION_HANDLER(CompilerIntelx86)::OnOptionIntPropChange(UINT idProp, int nVal)
{
	VSASSERT(m_pSlob != (CSlob *)NULL, "No slob set!");

	UINT idPropL = MapActual(idProp);
	int nCurrVal;

	switch (idPropL)
	{
	case P_RTC_StackFrame:
	case P_RTC_MemoryAccess:
	case P_RTC_MemoryLeaks:
	case P_RTC_VectorNew:
	case P_RTC_SmallerType:
		// make sure we should be able to do this
		if (m_pSlob->GetIntProp(MapLogical(P_RTC), nCurrVal) == valid && nCurrVal == 3)
			m_pSlob->SetIntProp(MapLogical(P_RTC), 3);
		break;

	case P_RTC:
		// get rid of GZ if we come through here at all
		if (m_pSlob->GetIntProp(MapLogical(P_GZ), nCurrVal) == valid && nCurrVal)
		{
			m_pSlob->SetIntProp(MapLogical(P_GZ), FALSE);
			m_pSlob->SetIntProp(MapLogical(P_RTC_StackFrame), TRUE);
		}
		break;

	default:
		break;
	}
}

BOOL OLD_OPTION_HANDLER(CompilerIntelx86)::CheckDepOK(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fValid = TRUE;			// valid by default

	// placeholders for values
	int nVal;	

	// which prop are we checking the dep. for?
	switch (idPropL)
	{
		// Can't have any custom RTC without 'CustomOpt' on
		case P_RTC_StackFrame:
		case P_RTC_MemoryAccess:
		case P_RTC_MemoryLeaks:
		case P_RTC_VectorNew:
		case P_RTC_SmallerType:
			(void)m_pSlob->GetIntProp(MapLogical(P_RTC), nVal);
			fValid = (nVal == 3);
			break;

		default:
			break;
	}

	return fValid;
}

void OLD_OPTION_HANDLER(CompilerIntelx86)::FormFakeIntProp(UINT idProp, int & nVal)
{
	UINT idPropL = MapActual(idProp);

	// should not call this if it is not a fake property
	VSASSERT (IsFakeProp (idProp), "Cannot call FormFakeIntProp with non-fake prop!") ;

	BOOL bUseDebugLibs = FALSE;
	((CProjItem *)m_pSlob)->GetIntProp(P_UseDebugLibs, bUseDebugLibs);

	if (bUseDebugLibs)
		{
		nVal = 1;   //MLd
		}
	else
		{
		nVal = 4;   //ML
		}
	return;
}

///////////////////////////////////////////////////////////////////////////////////////
// CCompilerX86Tool
///////////////////////////////////////////////////////////////////////////////////////
CCCompilerX86Tool::CCCompilerX86Tool() : CCCompilerNTTool()
{
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

///////////////////////////////////////////////////////////////////////////////////////
// CLinkerX86Tool
///////////////////////////////////////////////////////////////////////////////////////
CLinkerX86Tool::CLinkerX86Tool() : CLinkerNTTool()
{
}

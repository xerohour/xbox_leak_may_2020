// 
// Linker Tool Options
//
// [matthewt]
//

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop

#include "optnlink.h"	// our local header file
#include "project.h"

BEGIN_OPTSTR_TABLE(LinkerCommon, P_LinkUnknownOption, P_LinkUnknownString, P_LinkNoLogo, P_PreventBind, FALSE)
	IDOPT_UNKNOWN_STRING,	"",							NO_OPTARGS,								single,
	IDOPT_LINKNOLOGO,	"nologo%T1",					OPTARGS1(P_LinkNoLogo),					single,
	IDOPT_DERIVED_OPTHDLR,	"",							NO_OPTARGS,								single,
	IDOPT_LINKVERBOSE,	"verbose%T1",					OPTARGS1(P_LinkVerbose), 				single,
	IDOPT_DLLGEN,		"dll%T1",						OPTARGS1(P_GenDll),						single,
	IDOPT_PROFILE,		"profile%T1",					OPTARGS1(P_Profile),					single,
	IDOPT_USEPDBNONE,	"pdb:none%F1",					OPTARGS1(P_UsePDB),						single,
	IDOPT_INCREMENTALLINK, "incremental:%{no|yes}1",	OPTARGS1(P_IncrementalLink),			single,
	IDOPT_USEPDB,		"pdb:%T1%2",					OPTARGS2(P_UsePDB, P_UsePDBName),		single,
	IDOPT_MAPGEN,		"m[ap]%T1[:%2]",				OPTARGS2(P_GenMap, P_MapName), 			single,
	IDOPT_DEBUG,		"debug%T1",						OPTARGS1(P_GenDebug),					single,
	IDOPT_DEBUGTYPE,	"debugtype:%{cv|coff|both}1",	OPTARGS1(P_DebugType),					single,
	IDOPT_MACHINETYPE, 	"machine:%{I386|IX86|M68K|ALPHA|MPPC}1",	OPTARGS1(P_MachineType),	single,
	IDOPT_NODEFAULTLIB,	"nod[efaultlib]:%1",			OPTARGS1(P_NoDefaultLibs),			multiple,
	IDOPT_EXALLLIB,		"nod[efaultlib]%T1",			OPTARGS1(P_ExAllLibs),				single,
	IDOPT_INCLUDESYM,	"include:%1",					OPTARGS1(P_IncludeSym),					multiple,
	IDOPT_DEFNAME,		"def:%1",						OPTARGS1(P_DefName),					single,
	IDOPT_FORCE,		"force%T1",						OPTARGS1(P_Force),						single,
	IDOPT_OUT,			"out:%1",						OPTARGS1(P_OutName),					single,	
	IDOPT_IMPLIB,		"implib:%1",					OPTARGS1(P_ImpLibName),					single,
	IDOPT_LAZYPDB,		"pdbtype:%{sept|con}1",			OPTARGS1(P_LazyPdb),					single,
	IDOPT_LIBPATH,		"libpath:%1",					OPTARGS1(P_LibPath),					multiple,
	IDOPT_DELAY_NOBIND,	"delay:nobind%T1",				OPTARGS1(P_LinkDelayNoBind),			single,	
	IDOPT_DELAY_UNLOAD,	"delay:unload%T1",				OPTARGS1(P_LinkDelayUnLoad),			single,	
	IDOPT_DELAY_LOAD_DLL,"delayload:%1",				OPTARGS1(P_LinkDelayLoadDLL),			multiple,
	IDOPT_LINKER_INPUT_EXT, "",							OPTARGS1(P_Linker_Input_Ext),			single,	
	IDOPT_RESONLY_DLL,	"noentry%T1",					OPTARGS1(P_ResOnlyDLL),					single,
	IDOPT_VIRTUAL_DEVICE_DRIVER,	"vxd%T1",			OPTARGS1(P_VirtualDeviceDriver),		single,
	IDOPT_EXE_DYNAMIC,	"exetype:dynamic%T1",			OPTARGS1(P_ExeDynamic),					single,
	IDOPT_DRIVER,		"driver%{:junk||:uponly|:wdm}1",OPTARGS1(P_Driver),						single,
	IDOPT_50COMPAT,		"link50compat%T1",				OPTARGS1(P_Link50Compat),				single,
	IDOPT_ALIGN,		"align:%1",						OPTARGS1(P_Align),						single,
	IDOPT_SET_CHECKSUM,	"release%T1",					OPTARGS1(P_SetCheckSum),				single,
	IDOPT_MERGE_SECTIONS,	"merge:%1",					OPTARGS1(P_MergeSections),				single,
	IDOPT_ORDER_FUNCS,	"order:%1",						OPTARGS1(P_OrderFuncs),					single,
	IDOPT_TRIM_PROC_MEM, "ws:aggressive%T1",			OPTARGS1(P_TrimProcMem),				single,
	IDOPT_LARGEADDRESS,	"largeaddressaware%{:no|}1",	OPTARGS1(P_LargeAddress),				single,
	IDOPT_FIXED,		"fixed%{:no|}1",				OPTARGS1(P_Fixed),						single,
	IDOPT_EXE_BASE,		"base:%1",						OPTARGS1(P_BaseAddr),					single,
	IDOPT_PREVENT_BIND,	"allowbind%{|:no}1",			OPTARGS1(P_PreventBind),				single,
	IDOPT_UNKNOWN_OPTION,	"",							NO_OPTARGS,								single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerCommon)
	OPTDEF_BOOL(LinkNoLogo, FALSE)
	OPTDEF_BOOL(LinkVerbose, FALSE)
	OPTDEF_BOOL(GenDll, FALSE)
	OPTDEF_BOOL(GenMap, FALSE)
	OPTDEF_PATH(MapName, "")
	OPTDEF_PATH(OutName, "")
	OPTDEF_BOOL(GenDebug, FALSE)
	OPTDEF_INT(DebugType, 1)
	OPTDEF_BOOL(ExAllLibs, FALSE) 
	OPTDEF_LIST(NoDefaultLibs, "")
	OPTDEF_LIST(IncludeSym, "")
	OPTDEF_PATH(DefName, "")
	OPTDEF_BOOL(UsePDB, TRUE)
	OPTDEF_BOOL(IncrementalLink, TRUE)
	OPTDEF_BOOL(LinkDelayNoBind, FALSE)
	OPTDEF_BOOL(LinkDelayUnLoad, FALSE)
	OPTDEF_LIST(LinkDelayLoadDLL, "")
	OPTDEF_PATH(UsePDBName, "")
	OPTDEF_BOOL(Force, FALSE)
	OPTDEF_PATH(ImpLibName, "")
	OPTDEF_BOOL(Profile, FALSE)
	OPTDEF_INT(MachineType, 2)
	OPTDEF_INT(IgnoreExportLib, 0)
	OPTDEF_LIST(LibPath, "")
	OPTDEF_INT(LazyPdb, 0)
	OPTDEF_STRING(Linker_Input_Ext, "*.obj")
	OPTDEF_BOOL(ResOnlyDLL, FALSE)
	OPTDEF_BOOL(VirtualDeviceDriver, FALSE)
	OPTDEF_BOOL(ExeDynamic, FALSE)
	OPTDEF_INT(Driver, 0)
	OPTDEF_BOOL(Link50Compat, FALSE)
	OPTDEF_INT(Align, 0)
	OPTDEF_BOOL(SetCheckSum, FALSE)
	OPTDEF_STRING(MergeSections, "")
	OPTDEF_STRING(OrderFuncs, "")
	OPTDEF_BOOL(TrimProcMem, FALSE)
	OPTDEF_INT(LargeAddress, 0)
	OPTDEF_INT(Fixed, 0)
	OPTDEF_STRING(BaseAddr, "")
	OPTDEF_BOOL(PreventBind, FALSE)
END_OPTDEF_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSchmoozeTool, CBuildTool)
IMPLEMENT_DYNAMIC(CLinkerTool, CSchmoozeTool)
IMPLEMENT_DYNAMIC(CLinkerNTTool, CLinkerTool)

// linker tool option default map 'faking'
BOOL OLD_OPTION_HANDLER(LinkerCommon)::IsFakeProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_MapName || idPropL == P_OutName ||
			idPropL == P_ImpLibName || idPropL == P_UsePDBName ||
			idPropL == P_IgnoreExportLib || idPropL == P_IncrementalLink || 
			idPropL == P_Linker_Input_Ext);
}

UINT OLD_OPTION_HANDLER(LinkerCommon)::GetFakePathDirProp(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_MapName)
		return P_OutDirs_Intermediate;

	// .exe, .lib, .pdb use the target directory
	else if (idPropL == P_OutName || idPropL == P_ImpLibName || idPropL == P_UsePDBName)
		return P_OutDirs_Target;

	return COptionHandler::GetFakePathDirProp(idProp);
} 

void OLD_OPTION_HANDLER(LinkerCommon)::OnOptionIntPropChange(UINT idProp, int nVal)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_GenDll)
		m_pSlob->InformDependants(MapLogical(P_OutName));
}

BOOL OLD_OPTION_HANDLER(LinkerCommon)::AlwaysShowDefault(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	return (idPropL == P_UsePDB || idPropL == P_MachineType);
}

BOOL OLD_OPTION_HANDLER(LinkerCommon)::IsDefaultStringProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	if (idPropL == P_OutName || idPropL == P_ImpLibName)
	{
		CString strFake;
		if (idPropL == P_OutName || idPropL == P_ImpLibName)
		{	// both 'foobar.exe' and 'foobar.dll' are defaults!
			GPT gptVal = m_pSlob->GetIntProp(MapLogical(P_GenDll), m_fProjIsDll);
			VSASSERT(gptVal == valid, "Failed to pick up P_GenDll!");
		}

		FormFakeStrProp(idProp, strFake);
		if (strFake.CompareNoCase(strVal) == 0)
			return TRUE;

		if (idPropL == P_OutName)
		{
			m_fProjIsDll = !m_fProjIsDll;

			FormFakeStrProp(idProp, strFake);
			if (strFake.CompareNoCase(strVal) == 0)
				return TRUE;
		}

		// if we failed to match a faked prop, then is this a default in 
		// the 'default map'?
	}

	return COptionHandler::IsDefaultStringProp(idProp, strVal);
}

void OLD_OPTION_HANDLER(LinkerCommon)::FormFakeIntProp(UINT idProp, int & nVal)
{
	UINT idPropL = MapActual(idProp);

	// should not call this if it is not a fake property
	VSASSERT (IsFakeProp (idProp), "FormFakeIntProp called with non-fake prop!");

	// the default value of Incremental Link depends whether
	// it is a 'debug' or 'release' mode
	if (idPropL == P_IncrementalLink)
		if (m_pSlob->GetIntProp(P_UseDebugLibs, nVal) != valid) 
			nVal = FALSE;

	if (idPropL == P_IgnoreExportLib){
		nVal = TRUE;
		if( m_pSlob->GetIntProp(P_Proj_IgnoreExportLib, nVal) == invalid ){
			if (m_pSlob->GetIntProp(MapLogical(P_GenDll), nVal) == invalid )	// /DLL must be set
				nVal = TRUE;
			else
				nVal = FALSE;
		}
	}
}

GPT OLD_OPTION_HANDLER(LinkerCommon)::GetDefIntProp(UINT idProp, int & nVal)
{
	if (!IsFakeProp(idProp))
		return COptionHandler::GetDefIntProp(idProp, nVal);

	FormFakeIntProp(idProp, nVal);
	return valid;
}

void OLD_OPTION_HANDLER(LinkerCommon)::FormFakeStrProp(UINT idProp, CString & strVal)
{
	UINT idPropL = MapActual(idProp);

	// Assert this a Linker 'fake' string prop?
	VSASSERT(IsFakeProp(idProp), "FormFakeStrProp called with non-fake prop!");

	if (idPropL == P_Linker_Input_Ext)
	{
		strVal = "";	// don't care about this prop during conversion...
		return;
	}

	// form the other fake string props...

	// get the project base-name (ie. without extension)
	CString strBase;

    // If we are faking the output name then we use the projects base-name
    // (without extension) as the basis of the faked name.
    // If we are faking the map name, pdb name, or implib name then we
    // use the base of the output name as the basis of the faked name.
    if (idPropL == P_OutName)
	{
        CProjItem * pItem = ((CProjItem *)m_pSlob);
		const CPath * ppathProj = pItem->GetProject()->GetFilePath();
		ppathProj->GetBaseNameString(strBase);
	}
    else
	{
		CString strPath;
		if (m_pSlob->GetStrProp(MapLogical(P_OutName), strPath) != valid)
			GetDefStrProp(MapLogical(P_OutName), strPath);
	  	
		CPath path;
	 	if (path.Create(strPath))
			path.GetBaseNameString(strBase);
	}

	// which output directory do we want to use?
	UINT idOutDirProp = GetFakePathDirProp(idProp);
	VSASSERT(idOutDirProp != (UINT)-1, "Failed to get proper fake path dir!");

	CString strOut;
	GPT gptVal = m_pSlob->GetStrProp(idOutDirProp, strOut);
	VSASSERT(gptVal == valid, "Failed to pick up idOutDirProp!");

	const TCHAR * pchT;
	strVal = "";

	// If the output directory doesn't end in a forward slash
	// or a backslash, append one.
	if (!strOut.IsEmpty())
	{
		strVal = strOut;

		pchT = (const TCHAR *)strVal + strVal.GetLength();
		pchT = _tcsdec((const TCHAR *)strVal, (TCHAR *)pchT);

		if (*pchT != _T('/') && *pchT != _T('\\'))
			strVal += _T('/');
	}

	// Add the base, and extension prefix
	strVal += strBase;
	strVal += _T('.');

	// Append the extension
	CString strExt;
	if (idPropL == P_MapName)
	{
		strExt = _TEXT("map");
	}
	else if (idPropL == P_OutName || idPropL == P_ImpLibName || idPropL == P_UsePDBName)
	{
		if (idPropL == P_ImpLibName)
		{
			strExt = _TEXT("lib");
		}
		else if (idPropL == P_UsePDBName)
		{
			strExt = _TEXT("pdb");
		}
		else
		{
			// can we get a supplied default target extension?
			if (!m_pSlob->GetStrProp(P_Proj_TargDefExt, strExt))
			{
				BOOL fOldProjIsDll = m_fProjIsDll;
				if (m_pSlob->GetIntProp(MapLogical(P_GenDll), m_fProjIsDll) != valid)
					m_fProjIsDll = fOldProjIsDll;
				strExt = (m_fProjIsDll ? _TEXT("dll") : _TEXT("exe"));	// no
			}
		}
	}
	else	
	{
		VSASSERT(FALSE, "Unhandled FormFakeStrProp case!");
	}

	strVal += strExt;
}

GPT OLD_OPTION_HANDLER(LinkerCommon)::GetDefStrProp(UINT idProp, CString & val)
{
	UINT idPropL = MapActual(idProp);

	BOOL fIgnoreFake = FALSE;

	// we don't have ImpLibName with no /DLL
	if (idPropL == P_ImpLibName)
	{
		int nVal;
		if (m_pSlob->GetIntProp(MapLogical(P_GenDll), nVal) == invalid || !nVal)	// /DLL must be set
			fIgnoreFake = TRUE;	// not a faked prop in this context
	}
	
	// can we ignore the output directories?
	if (fIgnoreFake || !IsFakeProp(idProp))
		return COptionHandler::GetDefStrProp(idProp, val);

	FormFakeStrProp(idProp, val);
	return valid;
}

BOOL OLD_OPTION_HANDLER(LinkerCommon)::CheckDepOK(UINT idProp)
{
	UINT idPropL = MapActual(idProp);

	BOOL fValid = TRUE;			// valid by default
	GPT gptVal = valid;
	
	// placeholders for values
	int nVal, nVal2;	
	CPlatform * pPlatform ;

	// which prop are we checking the dep. for?
	switch (idPropL)
	{
		// Incremental not valid if request to generate a Map
		case P_IncrementalLink:
			pPlatform = g_pActiveProject->GetCurrentPlatform() ;
			if (pPlatform->GetAttributes() & PIA_Supports_IncLink)
			{
				(void) m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal) ;
				(void) m_pSlob->GetIntProp (MapLogical(P_Profile), nVal2) ;
				fValid = nVal && !nVal2;
			}
			else
				fValid = FALSE ;
			break;
		// Use PDB not valid if Profiling is checked 
		case P_UsePDB:
			(void) m_pSlob->GetIntProp (MapLogical(P_Profile), nVal) ;
			fValid = !nVal ;
			break ;

		// Generate Mapfile must be disabled if Profile is checked
		case P_GenMap:
			//(void)m_pSlob->GetIntProp (MapLogical(P_Profile), nVal) ;
			fValid = TRUE;
			break ;

		// Can only type map-name if generating a mapfile!
		case P_MapName:
			(void) m_pSlob->GetIntProp(MapLogical(P_GenMap), nVal);
			fValid = nVal;
			break;

		// Can't specify the debug info type if not gen. debug info
		case P_DebugType:
			(void) m_pSlob->GetIntProp(MapLogical(P_GenDebug), nVal);
			pPlatform = g_pActiveProject->GetCurrentPlatform() ;
			if (pPlatform->GetAttributes() & PIA_Enable_AllDebugType)
				fValid = nVal;
			else
				 fValid = FALSE;
			break;

		// Not allowed to type in a .PDB name without use PDB file
		case P_UsePDBName:
			gptVal = m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal);
			VSASSERT(gptVal == valid, "Failed to pick up P_UsePDB!");
			(void) m_pSlob->GetIntProp(MapLogical(P_Profile), nVal2) ;
			fValid = nVal && !nVal2;
			break;
		// not allowed to select lazy pdb unless pdb in use
		case P_LazyPdb:
			gptVal = m_pSlob->GetIntProp(MapLogical(P_UsePDB), nVal);
			VSASSERT(gptVal == valid, "Failed to pick up P_UsePDB!");
			(void) m_pSlob->GetIntProp(MapLogical(P_Profile), nVal2) ;
			fValid = nVal && !nVal2;
			break;
	/*
		case P_StubName:
			pPlatform = g_pActiveProject->GetCurrentPlatform();
			if (pPlatform->GetAttributes() & PIA_Enable_Stub)
				fValid = TRUE;
			else
				fValid = FALSE;
			break;
	*/
		default:
			break;
	}

	return fValid;
}

//------------------------------------------------------
// NT linker option handler
//------------------------------------------------------

BEGIN_OPTSTR_TABLE(LinkerNT, (UINT)NULL, (UINT)NULL, P_VersionMaj, P_HeapCommit, FALSE)
	IDOPT_DERIVED_OPTHDLR,	"",							NO_OPTARGS,								single,
	IDOPT_VERSION,		"version:%1[.%2]",				OPTARGS2(P_VersionMaj, P_VersionMin),	single,
	IDOPT_STACK,		"st[ack]:%1[,%2]",				OPTARGS2(P_StackReserve, P_StackCommit),single,
	IDOPT_ENTRYPOINT,	"entry:%1",						OPTARGS1(P_EntryName),					single,
	IDOPT_SUBSYSTEM,	"subsystem:%{windows|console}1",OPTARGS1(P_SubSystem),					single,
	IDOPT_HEAP,			"heap:%1[,%2]",					OPTARGS2(P_HeapReserve, P_HeapCommit),	single,
END_OPTSTRTBL()

BEGIN_OPTDEF_MAP(LinkerNT)
	OPTDEF_INT(VersionMaj, -1)	// no default
	OPTDEF_INT(VersionMin, -1)	// no default
	OPTDEF_HEX(StackReserve, 0)
	OPTDEF_HEX(StackCommit, 0)
	OPTDEF_HEX(HeapReserve, 0)
	OPTDEF_HEX(HeapCommit, 0)
	OPTDEF_STRING(EntryName, "")
	OPTDEF_INT(SubSystem, -1)	// no default
END_OPTDEF_MAP()

BOOL OLD_OPTION_HANDLER(LinkerNT)::AlwaysShowDefault(UINT idProp)
{
	return FALSE;	// none
}

BOOL OLD_OPTION_HANDLER(LinkerNT)::CheckDepOK(UINT idProp)
{
	UINT idPropL = MapActual(idProp);
	BOOL fValid = TRUE;			// valid by default

	// placeholders for values
	int nVal;

	// which prop are we checking the dep. for?
	switch (idPropL)
	{
		// Stack Commit only enabled if Reserve size set
		case P_StackCommit:
			(void) m_pSlob->GetIntProp(MapLogical(P_StackReserve), nVal);
			fValid = (nVal != 0);	
			break;

		// Version Minor only enabled if Major version set
		case P_VersionMin:
		{
			CString strVal;
			(void) m_pSlob->GetStrProp(MapLogical(P_VersionMaj), strVal);
			fValid = (strVal != "");
			break;
	   	}

		default:
			break;
	}

	return fValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// CLinkerTool
///////////////////////////////////////////////////////////////////////////////////////////////////

CLinkerTool::CLinkerTool() : CSchmoozeTool()
{
	m_strToolInput = _TEXT("*.obj;*.res;*.lib;*.rsc");
	m_strToolPrefix = _TEXT("LINK32");
	m_strName = _TEXT("link.exe");
}
///////////////////////////////////////////////////////////////////////////////
BOOL CLinkerTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// CLinkerNTTool
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLinkerNTTool::PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC)
{
	COptionHandler * pOptionHandler = GetOptionHandler();
	VSASSERT(pOptionHandler != (COptionHandler *)NULL, "Missing option handler for the linker!");

	CString strLibs; COptionList optlstLibs(_T(' '), FALSE);

	// libraries that are C++ and FORTRAN
	// make sure we add/remove those Core Windows .libs the MFC headers auto-include
	BOOL bOK = strLibs.LoadString(IDS_WIN32_LIBS_CORE);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	optlstLibs.SetString(strLibs);
	VCPOSITION pos = optlstLibs.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
		// add these libs if we are not using MFC
 		pOptionHandler->MungeListStrProp(MapLogical(P_LinkUnknownString), optlstLibs.GetNext(pos), iUseMFC == NoUseMFC);	// these libraries are C++ only

	// make sure we add/remove those Windows .libs the MFC headers auto-include
	bOK = strLibs.LoadString(IDS_WIN32_LIBS);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	optlstLibs.SetString(strLibs);
	pos = optlstLibs.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
		// add these libs if we are not using MFC
 		pOptionHandler->MungeListStrProp(MapLogical(P_LinkUnknownString), optlstLibs.GetNext(pos), iUseMFC == NoUseMFC);

	// make sure we add/remove those Windows OLE2 .libs the MFC headers auto-include
	bOK = strLibs.LoadString(IDS_OLE2_LIBS);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	optlstLibs.SetString(strLibs);
	pos = optlstLibs.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
		// add these libs if we are not using MFC
 		pOptionHandler->MungeListStrProp(MapLogical(P_LinkUnknownString), optlstLibs.GetNext(pos), iUseMFC == NoUseMFC);

	// does this target's platform support ODBC?
	CProjType * pProjType = pProjItem->GetProject()->GetProjType();
	VSASSERT(pProjType != (CProjType *)NULL, "Unsupported projec type!");
	BOOL fSupportsODBC = pProjType->GetPlatform()->SupportsODBC();

	// make sure we add/remove those Windows ODBC .libs the MFC headers auto-include
	bOK = strLibs.LoadString(IDS_ODBC_LIBS);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	optlstLibs.SetString(strLibs);
	pos = optlstLibs.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
		// add these libs if we are not using MFC
 		pOptionHandler->MungeListStrProp(MapLogical(P_LinkUnknownString), optlstLibs.GetNext(pos),
 										 iUseMFC == NoUseMFC && fSupportsODBC);

	return CLinkerTool::PerformSettingsWizard(pProjItem, fDebug, iUseMFC);
}

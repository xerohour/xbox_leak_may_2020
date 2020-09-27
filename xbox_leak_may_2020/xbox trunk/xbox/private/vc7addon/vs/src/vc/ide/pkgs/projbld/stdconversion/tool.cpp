//
// Intelx86 C/C++ AddOn
//
// Intelx86 Platform C/C++ Tools Component Add-On 'package'
//
// [matthewt]
//

#include "stdafx.h" 

#include "bldpack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// our components
#include "platx86.h"
#include "platalf.h"
#include "platmrcd.h"
#include "platXbox.h"

#include "x86optn.h"
#include "axpoptn.h"
#include "mcdoptn.h"

class CIntelx86CAddOn : public CPackage
{
public:
	CIntelx86CAddOn();

	// register the build system components for this platform
	BOOL DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD blc_id);

private:
	WORD wpkgid;
};

CIntelx86CAddOn::CIntelx86CAddOn()
	// o instance
	// o handle to window
	// o package id (* do not use *)
	// o command-table
	// o index into cmdtable
{
}


BOOL CIntelx86CAddOn::DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD bsc_id)
{
	wpkgid = pcompmgr->GenerateBldSysCompPackageId((TCHAR *)szAddOnx86);

	DWORD bsc_idx86Platform = GenerateComponentId(wpkgid, BCID_Platform_x86);
	DWORD bsc_idAxpPlatform = GenerateComponentId(wpkgid, BCID_Platform_Alpha);
	DWORD bsc_idMercedPlatform = GenerateComponentId(wpkgid, BCID_Platform_Merced);
	DWORD bsc_idXboxPlatform = GenerateComponentId(wpkgid, BCID_Platform_Xbox);

	// C++ installed?
	static BOOL fGotCPP = TRUE;
	CPlatform *pPlat;
	// what is the hook?
	switch (blc_type)
	{
		case BLC_Platform:
			pPlat = new CPlatform(&g_PlatformInfoWin32x86);
#if !defined (_X86_)
			pPlat->SetBuildable(FALSE);
#endif
			pcompmgr->RegisterBldSysComp(pPlat, bsc_idx86Platform);

			pPlat = new CPlatform(&g_PlatformInfoWin32ALPHA);
#if !defined (_ALPHA_)
			pPlat->SetBuildable(FALSE);
#endif
			pcompmgr->RegisterBldSysComp(pPlat, bsc_idAxpPlatform);

		 	pPlat = new CPlatform(&g_PlatformInfoWin64MERCED);
			pPlat->SetBuildable(FALSE);
			pcompmgr->RegisterBldSysComp(pPlat, bsc_idMercedPlatform);

			pPlat = new CPlatform(&g_PlatformInfoXbox);
			pcompmgr->RegisterBldSysComp(pPlat, bsc_idXboxPlatform);

			break;

		case BLC_TargetType:
			// only for the Intelx86 platform!
			if (bsc_id == bsc_idx86Platform){
				// our Intelx86 project types
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32Exe);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32Dll);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32Con);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32Lib);
				pcompmgr->RegisterBldSysComp(new CProjTypeGeneric);
				pcompmgr->RegisterBldSysComp(new CProjTypeXboxExe);
				pcompmgr->RegisterBldSysComp(new CProjTypeXboxLib);
			}
			else if (bsc_id == bsc_idAxpPlatform){
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32ALPHAExe);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32ALPHADll);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32ALPHACon);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin32ALPHALib);
			}
			else if (bsc_id == bsc_idMercedPlatform){
				pcompmgr->RegisterBldSysComp(new CProjTypeWin64MERCEDExe);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin64MERCEDDll);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin64MERCEDCon);
				pcompmgr->RegisterBldSysComp(new CProjTypeWin64MERCEDLib);
			}
			break;

		case BLC_Tool:
		{
			DWORD type = pcompmgr->BldSysCompType(bsc_id);

			// for the platform?
			if (type == BLC_Platform)
			{
				// yes
				// only for the Intelx86 platform!
				if (bsc_id == bsc_idx86Platform){
					// our Intelx86 tools
					if (fGotCPP)	// install C/C++ compiler only if have language
					{
						pcompmgr->RegisterBldSysComp(new CCCompilerX86Tool,
													 GenerateComponentId(wpkgid, BCID_Tool_Compiler_x86));
					}
					pcompmgr->RegisterBldSysComp(new CLinkerX86Tool,
												 GenerateComponentId(wpkgid, BCID_Tool_Linker_x86));
				}
				else if (bsc_id == bsc_idAxpPlatform){
					pcompmgr->RegisterBldSysComp(new CCCompilerAlphaTool,
												 GenerateComponentId(wpkgid, BCID_Tool_Compiler_Alpha));
					pcompmgr->RegisterBldSysComp(new CLinkerAlphaTool,
												 GenerateComponentId(wpkgid, BCID_Tool_Linker_Alpha));
				}
				else if (bsc_id == bsc_idMercedPlatform){
					pcompmgr->RegisterBldSysComp(new CCCompilerMercedTool,
												 GenerateComponentId(wpkgid, BCID_Tool_Compiler_Merced));
					pcompmgr->RegisterBldSysComp(new CLinkerMercedTool,
												 GenerateComponentId(wpkgid, BCID_Tool_Linker_Merced));
				}
			break;
			}
			// for a target type?
			else if (fGotCPP && type == BLC_TargetType)	// install C/C++ compiler only if have language
			{		   
				// yes
				CBldSysCmp * pcomp = NULL;
				pcompmgr->LookupBldSysComp(bsc_id, pcomp);
				VSASSERT(pcomp != NULL, "Failed to look build system component!");

				if (pcomp->RelatedCompId() == bsc_idx86Platform)
				{
					if (((CProjType *)pcomp)->GetUniqueTypeId() == CProjType::generic)
						break;
					// add our Intelx86 C/C++ compiler tool to these target types
					((CProjType *)pcomp)->AddTool(wpkgid, BCID_Tool_Compiler_x86);
				}
				else if (pcomp->RelatedCompId() == bsc_idMercedPlatform)
				{
					if (((CProjType *)pcomp)->GetUniqueTypeId() == CProjType::generic)
						break;
					// add our Intelx Merced C/C++ compiler tool to these target types
					((CProjType *)pcomp)->AddTool(wpkgid, BCID_Tool_Compiler_Merced);
				}
				else if (pcomp->RelatedCompId() == bsc_idAxpPlatform)
				{
					if (((CProjType *)pcomp)->GetUniqueTypeId() == CProjType::generic)
						break;
					// add our Alpha C/C++ compiler tool to these target types
					((CProjType *)pcomp)->AddTool(wpkgid, BCID_Tool_Compiler_Alpha);
				}
			}
			break;
		}

		case BLC_OptionHdlr:
			if (bsc_id == bsc_idx86Platform){
				// our Intelx86 option handlers
				if (fGotCPP)	// install C/C++ compiler only if have language
				{
					pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(CompilerIntelx86));
				}
	
				pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(LinkerIntelx86));
			}
			else if (bsc_id == bsc_idAxpPlatform){
				pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(CompilerAlpha));
				pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(LinkerAlpha));
			}
			else if (bsc_id == bsc_idMercedPlatform){
				pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(CompilerMerced));
				pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(LinkerMerced));
			}
			break;

		case BLC_Scanner:
			break;

		default:
			break;
	}

	// nothing left to register
	return FALSE;
}

extern "C" BOOL PASCAL InitPackage(CObList *lstPackages)
{
	// register this package

	// other initialization
    lstPackages->AddTail(new CIntelx86CAddOn);
	return TRUE;	// init'ed ok
}
extern "C" void PASCAL AFX_EXPORT ExitPackage()
{
}

//
// External Target type
//
// [colint]
//
						
#ifndef _INCLUDE_EXTTARGET_H
#define _INCLUDE_EXTTARGET_H

// declare our project types
class CProjTypeExternalTarget : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeExternalTarget)

public:
	CProjTypeExternalTarget(DWORD bsc_id, int nOffset)
		: CProjType(szAddOnGeneric, BCID_ProjType_ExternalTarget + nOffset, /* id */
					IDS_EXTTARG_PROJTYPE, CProjType::exttarget, /* props */
					bsc_id) {} /* our project type platform */

	// create the list of tools we use
	BOOL FInit();

	// Set up the default target options for this project type
	BOOL SetDefaultDebugTargetOptions(CProject * pProject, CTargetItem * pTarget, BOOL fUseMFC);
	BOOL SetDefaultReleaseTargetOptions(CProject * pProject, CTargetItem * pTarget, BOOL fUseMFC);

#ifdef VB_MAKEFILES
	BOOL SetDefaultVBTargetOptions(CProject * pProject, CTargetItem * pTarget, int projType, VBInfo *pVBProj );
#endif

	// External targets have no tools whatsoever (not even the pass through tool!)
	virtual CBuildTool *PickTool(FileRegHandle) { return NULL; }

	int GetAttributes () {  return ImageExe | SubsystemWindows | TargetIsDebugable; }
};


#endif // _INCLUDE_EXTTARGET_H

/*
 *
 * toolxbe.h
 *
 * XBE builder tool definition
 *
 */

#include "..\..\ide\pkgs\bld\src\schmztl.h" // the schmooze tool
#include "optnxbe.h"

extern CSchmoozeTool::SchmoozeData g_schdataXbeBuild;

class CXbeBuilderTool : public CSchmoozeTool
{
	DECLARE_DYNAMIC (CXbeBuilderTool);

public:
	CXbeBuilderTool();

	BOOL HasPrimaryOutput(void) { return TRUE; }

	virtual const SchmoozeData & GetMacs() const {return g_schdataXbeBuild;}

	virtual BOOL IsDelOnRebuildFile ( const CPath *pPath );
	virtual BOOL IsProductFile(const CPath *pPath);

	virtual BOOL PerformSettingsWizard(CProjItem * pProjItem, BOOL fDebug, int iUseMFC);
	virtual BOOL GenerateDependencies(CActionSlob *pAction, FileRegHandle frh, CErrorContext &EC);
	virtual BOOL GenerateOutput(UINT type, CActionSlobList & lstActions, CErrorContext & EC);
    virtual BOOL AffectsOutput(UINT idProp);
	virtual BOOL GetCommandLines(CActionSlobList & lstActions,
		CPtrList & plCommandLines, DWORD attrib, CErrorContext & EC);
};

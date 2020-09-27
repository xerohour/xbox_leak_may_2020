//
// Intelx86 Compiler Tool
//
// [v-danwh],[matthewt]
//

#ifndef _INCLUDE_TOOLCPLR_Xbox_H
#define _INCLUDE_TOOLCPLR_Xbox_H

#include "oletool.h"
#include "projtool.h"			// the CBuildTool classes
#include "..\..\ide\pkgs\bld\src\toolcplr.h"	// our COMMON compiler tool
#include "xbxoptnc.h"			// our Xbox compiler tool options

class CCCompilerXboxTool : public CCCompilerNTTool
{
	DECLARE_DYNAMIC (CCCompilerXboxTool)

public:
	CCCompilerXboxTool();

	// Default tool options for the Intelx86 compiler tool
	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption);

    // generate Xbox-compiler-specific file list
    virtual BOOL GenerateOutput(UINT type, CActionSlobList & lstActions, CErrorContext & EC);

	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	//virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};

#if 0
class COLECompilerTool : public CCmdTarget
{
public:
	COLECompilerTool();
protected:
	// IBUILDTOOL
	BEGIN_INTERFACE_PART(Compiler, IBuildTool)
		INIT_INTERFACE_PART(COLECompilerTool, Compiler)
		STDMETHOD(IsCollectorTool)(void);
		STDMETHOD(Filter)(LPBUILDFILE);
		STDMETHOD(PerformBuild)( int type, int stage,LPENUMBSACTIONS, int *result );
		STDMETHOD(GenerateCommandLines)( LPENUMBSACTIONS, CStringList &, CStringList & );
		STDMETHOD(GenerateOutputs)( LPENUMBSACTIONS );
		STDMETHOD(GetDependencies)( LPENUMBSACTIONS );
		STDMETHOD(EnumOptionStrings)( LPENUMOPTIONSTRINGS * );
		STDMETHOD(EnumOptionTypes)( LPENUMOPTIONTYPES * );
		STDMETHOD(GetID)( UINT * );
	END_INTERFACE_PART(Compiler)

	DECLARE_INTERFACE_MAP()

};
#endif


#endif // _INCLUDE_TOOLCPLR_Xbox_H

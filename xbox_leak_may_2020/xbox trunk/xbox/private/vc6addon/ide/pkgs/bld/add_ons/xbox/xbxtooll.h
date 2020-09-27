//
// Xbox Linker Tool
//
// [colint]
//

#ifndef _INCLUDE_TOOLLINK_Xbox_H
#define _INCLUDE_TOOLLINK_Xbox_H

#include "..\..\ide\pkgs\bld\src\toollink.h"	// our COMMON compiler tool
#include "xbxoptnl.h"			// our Xbox compiler tool options

class CLinkerXboxExeTool : public CLinkerTool
{
	DECLARE_DYNAMIC (CLinkerXboxExeTool)

public:
	CLinkerXboxExeTool();

    //BOOL GetCommandLines(CActionSlobList&, CPtrList&, DWORD, CErrorContext&);
	BOOL HasPrimaryOutput(void) { return FALSE; }

	// Xbox UI for specific tool options
	// FUTURE:	
};

#ifdef XBOXDLL
class CLinkerXboxDllTool: public CLinkerTool
{
	DECLARE_DYNAMIC (CLinkerXboxDllTool)

public:
	CLinkerXboxDllTool();
};
#endif

void GetXbeFromExe(CString &, CString &);

#endif // _INCLUDE_TOOLLINK_Xbox_H

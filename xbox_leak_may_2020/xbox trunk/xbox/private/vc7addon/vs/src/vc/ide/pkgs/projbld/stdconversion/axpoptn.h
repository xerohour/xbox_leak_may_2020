//
// Alpha Tool Options and Tools
//

#pragma once

#include "alpha.h"

//----------------------------------------------------------------
// our ALPHA compiler option data
//----------------------------------------------------------------

// ALPHA compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	CompilerAlpha, /* name */
	szAddOnx86, BCID_OptHdlr_Compiler_Alpha, /* Alpha compiler option handler */
	szAddOnx86, BCID_Tool_Compiler_Alpha, /* Alpha compiler tool */
	szAddOnGeneric, BCID_OptHdlr_Compiler /* base generic compiler option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	virtual BOOL AlwaysShowDefault(UINT idProp);

END_OPTHDLR()

// ALPHA compiler tool properties
#define P_CpuAlpha				 		0x1000
#define P_CallConvAlpha				 	0x1001
#define P_ByteAlignAlpha				0x1002
#define P_ThreadAlpha				 	0x1003
#define P_GtvalueAlpha           		0x1004
#define P_BBThresholdAlpha              0x1005

// ALPHA compiler tool options (option ids)
#define IDOPT_CPU_ALPHA					6000//072199 IDOPT_BASE + 0
#define IDOPT_CALLCONV_ALPHA			6001//072199 IDOPT_BASE + 1
#define IDOPT_BYTE_ALPHA				6002//072199 IDOPT_BASE + 2
#define IDOPT_THREAD_ALPHA				6003//072199 IDOPT_BASE + 3
#define IDOPT_GTVALUE_ALPHA				6004//072199 IDOPT_BASE + 4
#define IDOPT_BBTHRESHOLD_ALPHA			6005//072199 IDOPT_BASE + 5

//----------------------------------------------------------------
// our ALPHA linker option data
//----------------------------------------------------------------

// ALPHA compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	LinkerAlpha, /* name */
	szAddOnx86, BCID_OptHdlr_Linker_Alpha, /* Alpha linker option handler */
	szAddOnx86, BCID_Tool_Linker_Alpha, /* Alpha linker tool */
	szAddOnGeneric, BCID_OptHdlr_LinkerNT /* base NT linker option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

END_OPTHDLR()

#include "optncplr.h"	// our COMMON compiler tool

class CCCompilerAlphaTool : public CCCompilerNTTool
{
	DECLARE_DYNAMIC (CCCompilerAlphaTool)

public:
	CCCompilerAlphaTool();

	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};

#include "optnlink.h"	// our COMMON linker tool

class CLinkerAlphaTool : public CLinkerNTTool
{
	DECLARE_DYNAMIC (CLinkerAlphaTool)

public:
	CLinkerAlphaTool();
};


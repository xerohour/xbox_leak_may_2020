//
// Merced Tool Options and Tools
//

#pragma once

#include "merced.h"

//----------------------------------------------------------------
// our MERCED compiler option data
//----------------------------------------------------------------

// MERCED compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	CompilerMerced, /* name */
	szAddOnx86, BCID_OptHdlr_Compiler_Merced, /* Merced compiler option handler */
	szAddOnx86, BCID_Tool_Compiler_Merced, /* Merced compiler tool */
	szAddOnGeneric, BCID_OptHdlr_Compiler /* base generic compiler option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// should this be shown always?
	virtual BOOL AlwaysShowDefault(UINT idProp);

END_OPTHDLR()

// MERCED compiler tool properties
#define P_CpuMerced				 		0x1000
#define P_CallConvMerced				0x1001
#define P_ByteAlignMerced				0x1002
#define P_ThreadMerced				 	0x1003
#define P_GtvalueMerced           		0x1004
#define P_BBThresholdMerced             0x1005

// MERCED compiler tool options (option ids)
#define IDOPT_CPU_MERCED					4000//072199 IDOPT_BASE + 0
#define IDOPT_CALLCONV_MERCED				4001//072199 IDOPT_BASE + 1
#define IDOPT_BYTE_MERCED					4002//072199 IDOPT_BASE + 2
#define IDOPT_THREAD_MERCED					4003//072199 IDOPT_BASE + 3
#define IDOPT_GTVALUE_MERCED				4004//072199 IDOPT_BASE + 4
#define IDOPT_BBTHRESHOLD_MERCED			4005//072199 IDOPT_BASE + 5

//----------------------------------------------------------------
// our MERCED linker option data
//----------------------------------------------------------------

// MERCED compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	LinkerMerced, /* name */
	szAddOnx86, BCID_OptHdlr_Linker_Merced, /* Merced linker option handler */
	szAddOnx86, BCID_Tool_Linker_Merced, /* Merced linker tool */
	szAddOnGeneric, BCID_OptHdlr_LinkerNT /* base NT linker option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:

END_OPTHDLR()

#include "optncplr.h"	// our COMMON compiler tool
class CCCompilerMercedTool : public CCCompilerNTTool
{
	DECLARE_DYNAMIC (CCCompilerMercedTool)

public:
	CCCompilerMercedTool();
};

#include "optnlink.h"	// our COMMON compiler tool
class CLinkerMercedTool : public CLinkerNTTool
{
	DECLARE_DYNAMIC (CLinkerMercedTool)

public:
	CLinkerMercedTool();
};



//
// Intelx86 Tool Options and Tools
//

#pragma once
#include "optncplr.h"	// our base option handler, the Common Compiler

//----------------------------------------------------------------
// our INTELx86 compiler option data
//----------------------------------------------------------------

// INTELx86 compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	CompilerIntelx86, /* name */
	szAddOnx86, BCID_OptHdlr_Compiler_x86, /* Intelx86 compiler option handler */
	szAddOnx86, BCID_Tool_Compiler_x86, /* Intelx86 compiler tool */
	szAddOnGeneric, BCID_OptHdlr_Compiler /* base generic compiler option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// is this a 'fake' prop?
	virtual BOOL IsFakeProp(UINT idProp);

	// return a 'fake' int prop
	virtual GPT GetDefIntProp(UINT idProp, int & nVal);

	// handle setting of option int props
	virtual void OnOptionIntPropChange(UINT idProp, int nVal);

	// provide invalidation of option props
	virtual BOOL CheckDepOK(UINT idProp);

private:
	// form a 'fake' string prop
	virtual void FormFakeIntProp(UINT idProp, int & nVal);

END_OPTHDLR()

// INTELx86 compiler tool properties
#define P_Cpux86				 		0x1000
#define P_CallConv						0x1001
#define P_ByteAlign				   		0x1002
#define P_FloatPoint					0x1003
#define P_Thread						0x1004
#define P_RTC							0x1005
#define P_RTC_StackFrame				0x1006
#define P_RTC_MemoryAccess				0x1007
#define P_RTC_MemoryLeaks				0x1008
#define P_RTC_SmallerType               0x1009
#define P_RTC_VectorNew					0x100a
#define P_GZ                            0x100b

// INTELx86 compiler tool options (option ids)
#define IDOPT_CPUX86					2000//072199 IDOPT_BASE + 0
#define IDOPT_CALLCONV					2001//072199 IDOPT_BASE + 1
#define IDOPT_BYTE						2002//072199 IDOPT_BASE + 2
#define IDOPT_FLOATPOINT				2003//072199 IDOPT_BASE + 3
#define IDOPT_THREAD					2004//072199 IDOPT_BASE + 4
#define IDOPT_CPP_RTC					2005//072199 IDOPT_BASE + 5
#define IDOPT_CPP_RTC_StackFrame		2006//072199 IDOPT_BASE + 8
#define IDOPT_CPP_RTC_MemoryAccess		2007//072199 IDOPT_BASE + 9
#define IDOPT_CPP_RTC_MemoryLeaks		2008//072199 IDOPT_BASE + 10
#define IDOPT_CPP_RTC_SmallerType		2009//072199 IDOPT_BASE + 11
#define IDOPT_CPP_RTC_VectorNew			2010//072199 IDOPT_BASE + 12
#define IDOPT_GZ                        2011//072199 IDOPT_BASE + 13

#include "optnlink.h"	// our base option handler, the NT Linker 

//----------------------------------------------------------------
// our INTELx86 linker option data
//----------------------------------------------------------------

// INTELx86 compiler option handler
DEFN_OPTHDLR_PLATFORM
(
	LinkerIntelx86, /* name */
	szAddOnx86, BCID_OptHdlr_Linker_x86, /* Intelx86 linker option handler */
	szAddOnx86, BCID_Tool_Linker_x86, /* Intelx86 linker tool */
	szAddOnGeneric, BCID_OptHdlr_LinkerNT /* base generic NT linker option handler */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

END_OPTHDLR()

#include "optncplr.h"			// the CBuildTool classes
class CCCompilerX86Tool : public CCCompilerNTTool
{
	DECLARE_DYNAMIC (CCCompilerX86Tool)

public:
	CCCompilerX86Tool();

	// Default tool options for the Intelx86 compiler tool
	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption);

	//	Ensure that the tool will build an MFC project item.
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC);
};

#include "optnlink.h"	// our COMMON compiler tool
class CLinkerX86Tool : public CLinkerNTTool
{
	DECLARE_DYNAMIC (CLinkerX86Tool)

public:
	CLinkerX86Tool();
};



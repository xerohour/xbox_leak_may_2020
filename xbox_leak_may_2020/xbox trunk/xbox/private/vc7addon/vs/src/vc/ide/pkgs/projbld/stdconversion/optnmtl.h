// Make Type Library Tool Options and Tool
//

#pragma once

//----------------------------------------------------------------
// our mktyplib option data
//----------------------------------------------------------------

// option handler
DEFN_OPTHDLR_COMMON
(
	MkTypLib, /* name */
	szAddOnGeneric, BCID_OptHdlr_MkTypLib, /* generic mktyplib option handler */
	szAddOnGeneric, BCID_Tool_MkTypLib /* our generic associated mktyplib tool */
)

	DECL_OPTSTR_TABLE()
	DECL_OPTDEF_MAP()

public:
	// is this a 'fake' prop?
	virtual BOOL IsFakeProp (UINT idProp);

	// return a 'fake' string prop
	virtual GPT GetDefStrProp (UINT idProp, CString & strVal);

	// return a 'output dir' prop
	virtual UINT GetFakePathDirProp(UINT idProp);

private:
	// form a 'fake' string prop
	virtual void FormFakeStrProp (UINT idProp, CString & strVal);

END_OPTHDLR()


// MkTypLib properties
#define P_MTLNologo				0x0000
#define P_MTLIncludes			0x0001
#define P_MTLMacros				0x0004
#define P_MTLOutputTlb			0x0007
#define P_MTLOutputInc			0x0008
#define P_MTLOutputUuid			0x0009
#define P_MTLOutputDir			0x000a
#define P_MTLMtlCompatible      0x000b
#define P_MTLStublessProxy    	0x000c
#define P_MTLNoClutter		    0x000d
#define P_MTL_Input_Ext			0x000e
#define P_MTLUnknownOption		0x000f
#define P_MTLUnknownString		0x0010
#define P_MTL_Char              0x0011
#define P_MTLDllData            0x0012
#define P_MTL_Client			0x0013
#define P_MTL_Server			0x0014

// MkTypLib options (option ids)				   
#define IDOPT_MTLNOLOGO				IDOPT_BASE + 0
#define IDOPT_MTLINCLUDES 			IDOPT_BASE + 1
#define IDOPT_MTLIGNINC				IDOPT_BASE + 2
#define IDOPT_MTLMACROS				IDOPT_BASE + 3
#define IDOPT_MTLOUTPUTTLB			IDOPT_BASE + 4
#define IDOPT_MTLOUTPUTINC			IDOPT_BASE + 5
#define IDOPT_MTLOUTPUTUUID			IDOPT_BASE + 6
#define IDOPT_MTLMTLCOMPATIBLE		IDOPT_BASE + 7
#define IDOPT_MTL_STUBLESS_PROXY	IDOPT_BASE + 8
#define IDOPT_MTLOUTPUTDIR			IDOPT_BASE + 9
#define IDOPT_MTLNOCLUTTER   		IDOPT_BASE + 10
#define IDOPT_MTL_INPUT_EXT   		IDOPT_BASE + 11
#define IDOPT_MTL_CHAR				IDOPT_BASE + 12
#define IDOPT_MTLDLLDATA            IDOPT_BASE + 13
#define IDOPT_MTL_CLIENT			IDOPT_BASE + 14
#define IDOPT_MTL_SERVER			IDOPT_BASE + 15

#include "projtool.h"	// where base classes are defined

///////////////////////////////////////////////////////////////////////////////
//
// CBuildTool ----> CMkTypLibTool
//
////////////////////////////////////////////////////////////////////////////////
class CMkTypLibTool : public CBuildTool
{
	DECLARE_DYNAMIC (CMkTypLibTool)

public:
	CMkTypLibTool();
};

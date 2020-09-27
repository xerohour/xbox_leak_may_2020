///////////////////////////////////////////////////////////////////////////////
//	bpcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the Cbp2IDETest Class
//

#ifndef __bpcase2_H__
#define __bpcase2_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "bpsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CbpIDETest class

class Cbp2IDETest : public CDbgTestBase
{
	DECLARE_TEST(Cbp2IDETest, CbpSubSuite)

// Attributes
public:

// Operations
public:  
	virtual void PreRun(void);
	virtual void Run();

protected:  // Test Cases

	// BP2Cases
	// for the child w/dll's
	BOOL BreakDLLInit(  );
	BOOL BPTableDLL(  );
	BOOL LoadDLLBreak(  );
};

#endif //__bpcase2_H__

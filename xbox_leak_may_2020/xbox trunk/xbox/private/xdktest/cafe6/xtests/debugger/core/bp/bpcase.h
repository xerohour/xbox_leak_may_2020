///////////////////////////////////////////////////////////////////////////////
//	bpcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CbpIDETest Class
//

#ifndef __bpcase_H__
#define __bpcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "bpsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CbpIDETest class

class CbpIDETest : public CDbgTestBase
{
	DECLARE_TEST(CbpIDETest, CbpSubSuite)

// Attributes
public:

// Operations
public:  
	virtual void PreRun(void);
	virtual void Run();

protected:  // Test Cases
	// BP1Cases
	BOOL BreakAtLine(  );
	BOOL Disable_Enable(  );
	BOOL BreakOnExpressionStaticVars(  );
	BOOL BreakOnCompoundExpression(  );
	BOOL BreakAtVirtualFunc(  );

	// BP2Cases
	// for the static child
	BOOL BPTable(  );
	BOOL LocationBPWithExp(  );
	BOOL MessageBP(  );
	BOOL BreakStepTrace(  );   // stepping, tracing, breakpoints

};

#endif //__bpcase_H__

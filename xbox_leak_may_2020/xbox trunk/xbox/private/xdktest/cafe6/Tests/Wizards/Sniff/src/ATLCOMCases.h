///////////////////////////////////////////////////////////////////////////////
//	ATLCOMCases.h
//
//	Created by:	MichMa		Date:	2/13/98
//
//	Description:
//		Declaration of the CATLCOMCases class

#ifndef __ATLCOMCases_h__
#define __ATLCOMCases_h__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"


///////////////////////////////////////////////////////////////////////////////
//	CATLCOMCases class

class CATLCOMCases : public CTest
{
	DECLARE_TEST(CATLCOMCases, CSniffDriver)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__ATLCOMCases_h__

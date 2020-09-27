///////////////////////////////////////////////////////////////////////////////
//	PRTCASES.H
//
//	Created by :			Date :
//		MichMa					10/5/94
//
//	Description :
//		Declaration of the CPortableDbgCases class
//

#ifndef __PRTCASES_H__
#define __PRTCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"


///////////////////////////////////////////////////////////////////////////////
//	CPortableDbgCases class

class CPortableDbgCases : public CTest

{
	DECLARE_TEST(CPortableDbgCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);
};

#endif //__PRTCASES_H__

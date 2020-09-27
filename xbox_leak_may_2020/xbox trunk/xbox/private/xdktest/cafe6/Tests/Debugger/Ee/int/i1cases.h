///////////////////////////////////////////////////////////////////////////////
//  I1CASES.H
//
//  Created by :            Date :
//      MichMa              	12/03/93
//
//  Description :
//      Declaration of the CInt1Cases class
//

#ifndef __I1CASES_H__
#define __I1CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt1Cases class

class CInt1Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt1Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I1CASES_H__

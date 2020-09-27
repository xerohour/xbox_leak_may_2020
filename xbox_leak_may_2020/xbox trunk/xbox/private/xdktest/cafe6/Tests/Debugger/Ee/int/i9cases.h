///////////////////////////////////////////////////////////////////////////////
//  I9CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt9Cases class
//

#ifndef __I9CASES_H__
#define __I9CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt9Cases class

class CInt9Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt9Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I9CASES_H__

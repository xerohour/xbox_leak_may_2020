///////////////////////////////////////////////////////////////////////////////
//  I10CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt10Cases class
//

#ifndef __I10CASES_H__
#define __I10CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt10Cases class

class CInt10Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt10Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I10CASES_H__

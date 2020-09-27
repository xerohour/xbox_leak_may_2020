///////////////////////////////////////////////////////////////////////////////
//  I11CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt11Cases class
//

#ifndef __I11CASES_H__
#define __I11CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt11Cases class

class CInt11Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt11Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I11CASES_H__

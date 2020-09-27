///////////////////////////////////////////////////////////////////////////////
//  I7CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt7Cases class
//

#ifndef __I7CASES_H__
#define __I7CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt7Cases class

class CInt7Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt7Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I7CASES_H__

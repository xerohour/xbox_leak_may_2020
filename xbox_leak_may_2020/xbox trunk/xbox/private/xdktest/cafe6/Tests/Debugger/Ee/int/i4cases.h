///////////////////////////////////////////////////////////////////////////////
//  I4CASES.H
//
//  Created by :            Date :
//      MichMa              	12/10/93
//
//  Description :
//      Declaration of the CInt4Cases class
//

#ifndef __I4CASES_H__
#define __I4CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt4Cases class

class CInt4Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt4Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I4CASES_H__

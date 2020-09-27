///////////////////////////////////////////////////////////////////////////////
//  I5CASES.H
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Declaration of the CInt5Cases class
//

#ifndef __I5CASES_H__
#define __I5CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt5Cases class

class CInt5Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt5Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I5CASES_H__

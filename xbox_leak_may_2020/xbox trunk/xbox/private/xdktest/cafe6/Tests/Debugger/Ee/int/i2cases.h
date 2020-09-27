///////////////////////////////////////////////////////////////////////////////
//  I2CASES.H
//
//  Created by :            Date :
//      MichMa              	12/10/93
//
//  Description :
//      Declaration of the CInt2Cases class
//

#ifndef __I2CASES_H__
#define __I2CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "intsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CInt2Cases class

class CInt2Cases : public CDebugTestSet

	{
	DECLARE_TEST(CInt2Cases, CIntSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__I2CASES_H__

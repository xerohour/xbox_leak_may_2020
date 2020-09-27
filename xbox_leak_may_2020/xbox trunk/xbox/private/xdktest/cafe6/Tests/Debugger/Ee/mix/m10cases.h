///////////////////////////////////////////////////////////////////////////////
//  M10CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix10Cases class
//

#ifndef __M10CASES_H__
#define __M10CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix10Cases class

class CMix10Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix10Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M10CASES_H__

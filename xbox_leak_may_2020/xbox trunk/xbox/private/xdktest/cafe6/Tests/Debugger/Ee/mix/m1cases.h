///////////////////////////////////////////////////////////////////////////////
//  M1CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix1Cases class
//

#ifndef __M1CASES_H__
#define __M1CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix1Cases class

class CMix1Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix1Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M1CASES_H__

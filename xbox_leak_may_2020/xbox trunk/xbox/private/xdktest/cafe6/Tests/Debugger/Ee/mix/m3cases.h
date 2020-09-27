///////////////////////////////////////////////////////////////////////////////
//  M3CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix3Cases class
//

#ifndef __M3CASES_H__
#define __M3CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix3Cases class

class CMix3Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix3Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M3CASES_H__

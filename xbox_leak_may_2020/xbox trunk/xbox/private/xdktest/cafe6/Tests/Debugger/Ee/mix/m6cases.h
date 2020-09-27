///////////////////////////////////////////////////////////////////////////////
//  M6CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix6Cases class
//

#ifndef __M6CASES_H__
#define __M6CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix6Cases class

class CMix6Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix6Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M6CASES_H__

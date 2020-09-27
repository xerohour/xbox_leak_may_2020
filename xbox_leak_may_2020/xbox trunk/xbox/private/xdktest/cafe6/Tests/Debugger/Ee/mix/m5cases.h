///////////////////////////////////////////////////////////////////////////////
//  M5CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix5Cases class
//

#ifndef __M5CASES_H__
#define __M5CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix5Cases class

class CMix5Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix5Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M5CASES_H__

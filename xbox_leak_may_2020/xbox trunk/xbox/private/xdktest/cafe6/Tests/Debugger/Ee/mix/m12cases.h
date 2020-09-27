///////////////////////////////////////////////////////////////////////////////
//  M12CASES.H
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Declaration of the CMix12Cases class
//

#ifndef __M12CASES_H__
#define __M12CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "mixsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMix12Cases class

class CMix12Cases : public CDebugTestSet

	{
	DECLARE_TEST(CMix12Cases, CMixSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__M12CASES_H__

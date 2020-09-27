///////////////////////////////////////////////////////////////////////////////
//  LSECASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CLShiftEqCases class
//

#ifndef __LSECASES_H__
#define __LSECASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CLShiftEqCases class

class CLShiftEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CLShiftEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__LSECASES_H__

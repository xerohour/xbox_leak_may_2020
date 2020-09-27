///////////////////////////////////////////////////////////////////////////////
//  BAECASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CBitAndEqCases class
//

#ifndef __BAECASES_H__
#define __BAECASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CBitAndEqCases class

class CBitAndEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CBitAndEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__BAECASES_H__

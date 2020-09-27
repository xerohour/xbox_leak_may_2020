///////////////////////////////////////////////////////////////////////////////
//  EQCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CEqCases class
//

#ifndef __EQCASES_H__
#define __EQCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CEqCases class

class CEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__EQCASES_H__

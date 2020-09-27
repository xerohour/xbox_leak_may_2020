///////////////////////////////////////////////////////////////////////////////
//  NEQCASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CNotEqCases class
//

#ifndef __NEQCASES_H__
#define __NEQCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CNotEqCases class

class CNotEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CNotEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__NEQCASES_H__

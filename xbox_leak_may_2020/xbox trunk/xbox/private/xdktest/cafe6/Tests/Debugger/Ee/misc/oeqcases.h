///////////////////////////////////////////////////////////////////////////////
//  OEQCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the COrEqCases class
//

#ifndef __OEQCASES_H__
#define __OEQCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  COrEqCases class

class COrEqCases : public CDebugTestSet

    {
	DECLARE_TEST(COrEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__OEQCASES_H__

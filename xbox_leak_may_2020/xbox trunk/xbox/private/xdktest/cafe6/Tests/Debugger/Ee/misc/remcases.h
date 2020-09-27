///////////////////////////////////////////////////////////////////////////////
//  REMCASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CRemainCases class
//

#ifndef __REMCASES_H__
#define __REMCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CRemainCases class

class CRemainCases : public CDebugTestSet

    {
	DECLARE_TEST(CRemainCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__REMCASES_H__

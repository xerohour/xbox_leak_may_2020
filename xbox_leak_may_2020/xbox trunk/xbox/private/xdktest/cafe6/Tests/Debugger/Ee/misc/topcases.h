///////////////////////////////////////////////////////////////////////////////
//  TOPCASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CTimesOpCases class
//

#ifndef __TOPCASES_H__
#define __TOPCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CTimesOpCases class

class CTimesOpCases : public CDebugTestSet

    {
	DECLARE_TEST(CTimesOpCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__TOPCASES_H__

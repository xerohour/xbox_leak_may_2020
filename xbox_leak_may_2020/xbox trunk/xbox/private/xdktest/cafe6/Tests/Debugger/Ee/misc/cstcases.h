///////////////////////////////////////////////////////////////////////////////
//  CSTCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CCastCases class
//

#ifndef __CSTCASES_H__
#define __CSTCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CCastCases class

class CCastCases : public CDebugTestSet

    {
	DECLARE_TEST(CCastCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__CSTCASES_H__

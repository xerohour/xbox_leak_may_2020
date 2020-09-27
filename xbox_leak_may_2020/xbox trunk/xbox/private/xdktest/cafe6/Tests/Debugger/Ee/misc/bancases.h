///////////////////////////////////////////////////////////////////////////////
//  BANCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CBitAndCases class
//

#ifndef __BANCASES_H__
#define __BANCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CBitAndCases class

class CBitAndCases : public CDebugTestSet

    {
	DECLARE_TEST(CBitAndCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__BANCASES_H__

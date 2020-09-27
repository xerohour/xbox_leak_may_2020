///////////////////////////////////////////////////////////////////////////////
//  NOTCASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CNotCases class
//

#ifndef __NOTCASES_H__
#define __NOTCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CNotCases class

class CNotCases : public CDebugTestSet

    {
	DECLARE_TEST(CNotCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__NOTCASES_H__

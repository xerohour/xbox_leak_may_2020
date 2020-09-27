///////////////////////////////////////////////////////////////////////////////
//  UMNCASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CUMinusCases class
//

#ifndef __UMNCASES_H__
#define __UMNCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CUMinusCases class

class CUMinusCases : public CDebugTestSet

    {
	DECLARE_TEST(CUMinusCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__UMNCASES_H__

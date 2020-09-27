///////////////////////////////////////////////////////////////////////////////
//  RMECASES.H
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Declaration of the CRemainEqCases class
//

#ifndef __RMECASES_H__
#define __RMECASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CRemainEqCases class

class CRemainEqCases : public CDebugTestSet

    {
	DECLARE_TEST(CRemainEqCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__RMECASES_H__

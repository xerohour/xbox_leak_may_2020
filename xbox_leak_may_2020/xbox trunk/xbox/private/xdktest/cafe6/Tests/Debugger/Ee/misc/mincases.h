///////////////////////////////////////////////////////////////////////////////
//  MINCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CMinusCases class
//

#ifndef __MINCASES_H__
#define __MINCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CMinusCases class

class CMinusCases : public CDebugTestSet

    {
	DECLARE_TEST(CMinusCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__MINCASES_H__

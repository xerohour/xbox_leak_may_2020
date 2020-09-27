///////////////////////////////////////////////////////////////////////////////
//  QSTCASES.H
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Declaration of the CQuestCases class
//

#ifndef __QSTCASES_H__
#define __QSTCASES_H__

#ifndef __STDAFX_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CQuestCases class

class CQuestCases : public CDebugTestSet

    {
	DECLARE_TEST(CQuestCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
    };

#endif //__QSTCASES_H__

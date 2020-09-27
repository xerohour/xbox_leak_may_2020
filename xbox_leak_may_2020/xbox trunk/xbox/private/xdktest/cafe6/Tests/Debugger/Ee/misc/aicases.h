///////////////////////////////////////////////////////////////////////////////
//  AICASES.H
//
//  Created by :            Date :
//      MichMa              12/03/93
//
//  Description :
//      Declaration of the CAndIfCases class
//

#ifndef __AICASES_H__
#define __AICASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\core\dbgset.h"
#include "miscsub.h"

///////////////////////////////////////////////////////////////////////////////
//  CAndIfCases class

class CAndIfCases : public CDebugTestSet

	{
	DECLARE_TEST(CAndIfCases, CMiscSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__AICASES_H__

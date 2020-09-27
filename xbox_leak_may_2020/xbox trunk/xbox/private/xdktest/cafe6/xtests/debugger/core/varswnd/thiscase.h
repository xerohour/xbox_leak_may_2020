///////////////////////////////////////////////////////////////////////////////
//	THISCASE.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CThisPaneIDETest Class
//

#ifndef __THISCASE_H__
#define __THISCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#include "..\dbgset.h"
#include "..\..\DbgTestBase.h"
#include "varssub.h"

///////////////////////////////////////////////////////////////////////////////
//	CThisPaneIDETest class

class CThisPaneIDETest : public CDbgTestBase
{
	DECLARE_TEST(CThisPaneIDETest, CVarsWndSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// Test cases
	private:

	// Data
	private:
		EXPR_INFO expr_info_actual[8], expr_info_expected[8];
	};

#endif //__THISCASE_H__

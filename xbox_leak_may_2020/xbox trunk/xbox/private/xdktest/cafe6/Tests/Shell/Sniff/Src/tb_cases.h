///////////////////////////////////////////////////////////////////////////////
//	TB_CASES.H
//
//	Created by :			Date :
//		Enriquep					9/3/93
//
//	Description :
//		Declaration of the CTBTestCases class
//

#ifndef __TB_CASES_H__
#define __TB_CASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CTBTestCases class

class CTBTestCases : public CTest
{
	DECLARE_TEST(CTBTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
	BOOL ViewToolbars(void);
	BOOL HideToolbar(void);
	BOOL ShowToolbar(void);
	BOOL NewToolbar(void);
	BOOL AddButton(void);
	BOOL NewTB(void);
};

#endif //__TB_CASES_H__

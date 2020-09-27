///////////////////////////////////////////////////////////////////////////////
//	WINCASES.H
//
//	Created by :			Date :
//		MichMa					10/5/94
//
//	Description :
//		Declaration of the CWin32DbgCases class
//

#ifndef __WINCASES_H__
#define __WINCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"


///////////////////////////////////////////////////////////////////////////////
//	CWin32DbgCases class

class CWin32DbgCases : public CTest

{
	DECLARE_TEST(CWin32DbgCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);
};

#endif //__WINCASES_H__

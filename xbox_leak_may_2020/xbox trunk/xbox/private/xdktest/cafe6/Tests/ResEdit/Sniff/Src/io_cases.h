///////////////////////////////////////////////////////////////////////////////
//    IO_CASES.H
//
//    Created by :                    Date :
//            DougT                                   8/13/93
//
//    Description :
//            Declaration of the CIO_TestCases class
//
 
#ifndef __IO_CASES_H__
#define __IO_CASES_H__

#ifndef __STDAFX_H__
      #error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CIO_TestCases class

class CIO_TestCases : public CTest
{
	DECLARE_TEST(CIO_TestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Test Cases
protected:
	BOOL IO_OpenRC(void);
	BOOL IO_SaveAsRC(void);
	BOOL IO_CompareRCFile(void);
	BOOL IO_CompareHFile(void);
	BOOL IO_CloseRC(void);
	BOOL IO_DirtyRC(void);
};

#endif //__IO_CASES_H__

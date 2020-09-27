///////////////////////////////////////////////////////////////////////////////
//    TBRCASES.H
//
//    Created by :                    Date :
//            MikePie                 2/28/95
//
//    Description :
//            Declaration of the CTbrTestCases class
//
 
#ifndef __TBRCASES_H__
#define __TBRCASES_H__

#ifndef __STDAFX_H__
      #error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CTbrTestCases class

class CTbrTestCases : public CTest
{
	DECLARE_TEST(CTbrTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Test Cases
protected:
	BOOL NewToolbar(void);
	BOOL ModifyToolbar(void);
	BOOL CheckWidth(void);
	BOOL CheckHeight(void);
	BOOL CloseToolbar(void);
};

#endif //__TBRCASES_H__

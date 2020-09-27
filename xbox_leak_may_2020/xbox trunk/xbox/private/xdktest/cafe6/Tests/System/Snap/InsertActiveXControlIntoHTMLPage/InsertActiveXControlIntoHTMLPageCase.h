///////////////////////////////////////////////////////////////////////////////
//	InsertActiveXControlIntoHTMLPageCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CInsertActiveXControlIntoHTMLPageCase class

#ifndef __InsertActiveXControlIntoHTMLPageCase_H__
#define __InsertActiveXControlIntoHTMLPageCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "InsertActiveXControlIntoHTMLPageSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CInsertActiveXControlIntoHTMLPageCase class

class CInsertActiveXControlIntoHTMLPageCase : public CSystemTestSet
{
	DECLARE_TEST(CInsertActiveXControlIntoHTMLPageCase, CInsertActiveXControlIntoHTMLPageSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__InsertActiveXControlIntoHTMLPageCase_H__

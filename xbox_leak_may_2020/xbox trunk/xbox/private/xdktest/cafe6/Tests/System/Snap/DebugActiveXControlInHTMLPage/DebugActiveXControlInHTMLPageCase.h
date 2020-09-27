///////////////////////////////////////////////////////////////////////////////
//	DebugActiveXControlInHTMLPageCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CDebugActiveXControlInHTMLPageCase class

#ifndef __DebugActiveXControlInHTMLPageCase_H__
#define __DebugActiveXControlInHTMLPageCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "DebugActiveXControlInHTMLPageSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CDebugActiveXControlInHTMLPageCase class

class CDebugActiveXControlInHTMLPageCase : public CSystemTestSet
{
	DECLARE_TEST(CDebugActiveXControlInHTMLPageCase, CDebugActiveXControlInHTMLPageSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__DebugActiveXControlInHTMLPageCase_H__

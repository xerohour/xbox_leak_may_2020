///////////////////////////////////////////////////////////////////////////////
//	CreateHTMLPageCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CCreateHTMLPageCase class

#ifndef __CreateHTMLPageCase_H__
#define __CreateHTMLPageCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "CreateHTMLPageSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CCreateHTMLPageCase class

class CCreateHTMLPageCase : public CSystemTestSet
{
	DECLARE_TEST(CCreateHTMLPageCase, CCreateHTMLPageSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__CreateHTMLPageCase_H__

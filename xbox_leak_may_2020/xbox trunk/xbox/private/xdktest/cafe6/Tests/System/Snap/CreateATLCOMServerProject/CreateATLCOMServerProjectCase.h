///////////////////////////////////////////////////////////////////////////////
//	CreateATLCOMServerProjectCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CCreateATLCOMServerProjectCase class

#ifndef __CreateATLCOMServerProjectCase_H__
#define __CreateATLCOMServerProjectCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "CreateATLCOMServerProjectSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CCreateATLCOMServerProjectCase class

class CCreateATLCOMServerProjectCase : public CSystemTestSet
{
	DECLARE_TEST(CCreateATLCOMServerProjectCase, CCreateATLCOMServerProjectSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__CreateATLCOMServerProjectCase_H__

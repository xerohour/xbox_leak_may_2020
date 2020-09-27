///////////////////////////////////////////////////////////////////////////////
//	DebugATLCOMServerCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CDebugATLCOMServerCase class

#ifndef __DebugATLCOMServerCase_H__
#define __DebugATLCOMServerCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "DebugATLCOMServerSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CDebugATLCOMServerCase class

class CDebugATLCOMServerCase : public CSystemTestSet
{
	DECLARE_TEST(CDebugATLCOMServerCase, CDebugATLCOMServerSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__DebugATLCOMServerCase_H__

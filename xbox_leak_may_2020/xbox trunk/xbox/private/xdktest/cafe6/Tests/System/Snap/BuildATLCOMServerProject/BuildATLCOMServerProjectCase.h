///////////////////////////////////////////////////////////////////////////////
//	BuildATLCOMServerProjectCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CBuildATLCOMServerProjectCase class

#ifndef __BuildATLCOMServerProjectCase_H__
#define __BuildATLCOMServerProjectCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "BuildATLCOMServerProjectSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CBuildATLCOMServerProjectCase class

class CBuildATLCOMServerProjectCase : public CSystemTestSet
{
	DECLARE_TEST(CBuildATLCOMServerProjectCase, CBuildATLCOMServerProjectSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__BuildATLCOMServerProjectCase_H__

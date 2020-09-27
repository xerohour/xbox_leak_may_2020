///////////////////////////////////////////////////////////////////////////////
//	InsertCallToATLCOMServerInAppCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CInsertCallToATLCOMServerInAppCase class

#ifndef __InsertCallToATLCOMServerInAppCase_H__
#define __InsertCallToATLCOMServerInAppCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "InsertCallToATLCOMServerInAppSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CInsertCallToATLCOMServerInAppCase class

class CInsertCallToATLCOMServerInAppCase : public CSystemTestSet
{
	DECLARE_TEST(CInsertCallToATLCOMServerInAppCase, CInsertCallToATLCOMServerInAppSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__InsertCallToATLCOMServerInAppCase_H__

///////////////////////////////////////////////////////////////////////////////
//	BuildActiveXControlProjectCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CBuildActiveXControlProjectCase class

#ifndef __BuildActiveXControlProjectCase_H__
#define __BuildActiveXControlProjectCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "BuildActiveXControlProjectSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CBuildActiveXControlProjectCase class

class CBuildActiveXControlProjectCase : public CSystemTestSet
{
	DECLARE_TEST(CBuildActiveXControlProjectCase, CBuildActiveXControlProjectSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__BuildActiveXControlProjectCase_H__

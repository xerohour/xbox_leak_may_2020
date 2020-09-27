///////////////////////////////////////////////////////////////////////////////
//	CreateActiveXControlProjectCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CCreateActiveXControlProjectCase class

#ifndef __CreateActiveXControlProjectCase_H__
#define __CreateActiveXControlProjectCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "CreateActiveXControlProjectSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CCreateActiveXControlProjectCase class

class CCreateActiveXControlProjectCase : public CSystemTestSet
{
	DECLARE_TEST(CCreateActiveXControlProjectCase, CCreateActiveXControlProjectSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__CreateActiveXControlProjectCase_H__

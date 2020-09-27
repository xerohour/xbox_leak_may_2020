///////////////////////////////////////////////////////////////////////////////
//	ActiveXControlInDialogCase.H
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Declaration of the CActiveXControlInDialogCase class

#ifndef __ActiveXControlInDialogCase_H__
#define __ActiveXControlInDialogCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "ActiveXControlInDialogSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CActiveXControlInDialogCase class

class CActiveXControlInDialogCase : public CSystemTestSet
{
	DECLARE_TEST(CActiveXControlInDialogCase, CActiveXControlInDialogSubsuite)

// Operations
public:
	void OpenMainProject(void);
	void AddControlFromGallery(void);
	void DropControlsToDialog(void);
	void BuildProject(void);
	void DebugProject(void);
//overides
public:
	virtual void PreRun(void);
	virtual void Run(void);
};

#endif //__ActiveXControlInDialogCase_H__

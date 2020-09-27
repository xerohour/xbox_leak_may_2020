///////////////////////////////////////////////////////////////////////////////
//	ATLWizCase.H
//
//	Created by :			Date :
//		ChrisKoz					1/28/98
//
//	Description :
//		Declaration of the CSysATLWizCase class

#ifndef __ATLWizCase_H__
#define __ATLWizCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ATLWizSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CSysATLWizCase class

class CSysATLWizCase : public CTest
{
	DECLARE_TEST(CSysATLWizCase, CSysATLWizSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
private:
	int InsertATLEventHandler(LPCSTR szMessageName, LPCSTR szClasName, LPCSTR szObjectName, LPCSTR szProjectName=NULL);
	int InsertATLMessageHandler(LPCSTR szMessageName, LPCSTR szClasName, LPCSTR szProjectName=NULL);
	int InsertDialogATLObject(LPCSTR szObjectName);
void LogResult(int line, int result,LPCSTR szoperation, int code = 0 , LPCSTR szComment = "" );

};

#endif //__ATLWizCase_H__

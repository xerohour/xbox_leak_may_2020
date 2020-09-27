///////////////////////////////////////////////////////////////////////////////
//	StressCase.H
//
//	Created by :			Date :
//		MichMa					1/28/98
//
//	Description :
//		Declaration of the CSysStressCase class

#ifndef __StressCase_H__
#define __StressCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "StressSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CSysStressCase class

class CSysStressCase : public CTest

{
	DECLARE_TEST(CSysStressCase, CSysStressSubsuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

// Utils
private:
	void InitiateWeb(void);
	void UpdateWeb(void);

// Data
private:
	// the root directory of the stress website.
	CString m_strStressRoot;
	// used as the name of the folder containing an html page showing the runs on that date and the logs.
	CString m_strCurrentDate;
	// used to track how many iterations the test has performed. 
	int m_intIterations;
};

#endif //__StressCase_H__

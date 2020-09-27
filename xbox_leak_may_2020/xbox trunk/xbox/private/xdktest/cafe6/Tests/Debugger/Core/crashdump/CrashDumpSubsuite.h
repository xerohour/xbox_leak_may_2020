///////////////////////////////////////////////////////////////////////////////
//	CrashDumpSubsuite.h
//
//	Created by: MichMa		Date: 9/30/97
//
//	Description :
//		Declaration of the CCrashDumpSubsuite class
//

#ifndef __CRASHDUMPSUBSUITE_H__
#define __CRASHDUMPSUBSUITE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CCrashDumpSubsuite class

class CCrashDumpSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CCrashDumpSubsuite)
	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__CRASHDUMPSUBSUITE_H__

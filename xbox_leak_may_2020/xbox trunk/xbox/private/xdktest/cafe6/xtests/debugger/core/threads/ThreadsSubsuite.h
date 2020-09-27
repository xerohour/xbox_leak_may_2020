///////////////////////////////////////////////////////////////////////////////
//	ThreadsSubsuite.h
//
//	Created by: MichMa		Date: 2/5/98
//
//	Description :
//		Declaration of the CThreadsSubsuite class
//

#ifndef __THREADSSUBSUITE_H__
#define __THREADSSUBSUITE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CThreadsSubsuite class

class CThreadsSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CThreadsSubsuite)
	DECLARE_TESTLIST()
};

#endif //__THREADSSUBSUITE_H__

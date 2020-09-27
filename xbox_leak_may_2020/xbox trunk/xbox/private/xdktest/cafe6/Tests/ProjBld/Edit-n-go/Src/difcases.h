///////////////////////////////////////////////////////////////////////////////
//	DIFCASES.H
//
//	Created by :			Date :
//		WayneBr				10/4/94
//
//	Description :
//		Declaration of the CWinDiffCases class
//

#ifndef __DIFCASES_H__
#define __DIFCASES_H__

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	BOOL CWinDiffCases:: class

class CWinDiffCases : public CPerformanceCases 
{
	DECLARE_TEST(CWinDiffCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);
	void		Initialize_WinDiff();
};


#endif // DIFCASES_H__

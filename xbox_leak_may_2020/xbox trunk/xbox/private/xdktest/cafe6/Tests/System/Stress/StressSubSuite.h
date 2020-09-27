///////////////////////////////////////////////////////////////////////////////
//	StressSubsuite.h
//
//	Created by :		Date:			
//		MichMa				1/28/98
//
//	Description :
//		Declaration of the CSysStressSubsuite class
//

#ifndef __StressSubsuite_H__
#define __StressSubsuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSysStressSubsuite class

class CSysStressSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CSysStressSubsuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__StressSubsuite_H__

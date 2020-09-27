///////////////////////////////////////////////////////////////////////////////
//	BuildATLCOMServerProjectSubsuite.h
//
//	Created by :		Date:			
//		MichMa				5/21/97
//
//	Description :
//		Declaration of the CBuildATLCOMServerProjectSubsuite class
//

#ifndef __BuildATLCOMServerProjectSubsuite_H__
#define __BuildATLCOMServerProjectSubsuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CBuildATLCOMServerProjectSubsuite class

class CBuildATLCOMServerProjectSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CBuildATLCOMServerProjectSubsuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__BuildATLCOMServerProjectSubsuite_H__

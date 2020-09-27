///////////////////////////////////////////////////////////////////////////////
//	CreateATLCOMServerProjectSubsuite.h
//
//	Created by :		Date:			
//		MichMa				5/21/97
//
//	Description :
//		Declaration of the CCreateATLCOMServerProjectSubsuite class
//

#ifndef __CreateATLCOMServerProjectSubsuite_H__
#define __CreateATLCOMServerProjectSubsuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CCreateATLCOMServerProjectSubsuite class

class CCreateATLCOMServerProjectSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CCreateATLCOMServerProjectSubsuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__CreateATLCOMServerProjectSubsuite_H__

///////////////////////////////////////////////////////////////////////////////
//	BuildActiveXControlProjectSubsuite.h
//
//	Created by :		Date:			
//		MichMa				5/21/97
//
//	Description :
//		Declaration of the CBuildActiveXControlProjectSubsuite class
//

#ifndef __BuildActiveXControlProjectSubsuite_H__
#define __BuildActiveXControlProjectSubsuite_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CBuildActiveXControlProjectSubsuite class

class CBuildActiveXControlProjectSubsuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CBuildActiveXControlProjectSubsuite)
	DECLARE_TESTLIST()
protected:
};

#endif //__BuildActiveXControlProjectSubsuite_H__

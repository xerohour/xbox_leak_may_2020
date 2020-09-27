///////////////////////////////////////////////////////////////////////////////
//	localssub.h
//
//	Created by: MichMa		Date: 10/22/97
//
//	Description:
//		Declaration of the CECLocalsSubSuite class
//

#ifndef __LOCALSSUB_H__
#define __LOCALSSUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


///////////////////////////////////////////////////////////////////////////////
// CECLocalsSubSuite class

class CECLocalsSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CECLocalsSubSuite)
	DECLARE_TESTLIST()
};


#endif //__LOCALSSUB_H__

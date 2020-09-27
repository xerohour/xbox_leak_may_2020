///////////////////////////////////////////////////////////////////////////////
//	custsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CCustomBuildSubSuite class
//

#ifndef __custsub_H__
#define __custsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CCustomBuildSubSuite class

class CCustomBuildSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CCustomBuildSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__custsub_H__

///////////////////////////////////////////////////////////////////////////////
//	convsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CConversionSubSuite class
//

#ifndef __convsub_H__
#define __convsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CConversionSubSuite class

class CConversionSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CConversionSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__convsub_H__

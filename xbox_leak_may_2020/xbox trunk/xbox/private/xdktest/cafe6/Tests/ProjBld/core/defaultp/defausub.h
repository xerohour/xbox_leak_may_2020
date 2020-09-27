///////////////////////////////////////////////////////////////////////////////
//	defausub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CDefaultPrjSubSuite class
//

#ifndef __defausub_H__
#define __defausub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDefaultPrjSubSuite class

class CDefaultPrjSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CDefaultPrjSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__defausub_H__

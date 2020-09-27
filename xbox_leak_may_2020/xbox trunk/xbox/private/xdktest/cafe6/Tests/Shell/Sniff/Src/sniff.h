///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		RickKr					8/27/93
//
//	Description :
//		Declaration of the CSniffDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSniffDriver class

class CSniffDriver : public CIDESubSuite
{
	DECLARE_SUBSUITE(CSniffDriver)

	DECLARE_TESTLIST()

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
};

#endif //__SNIFF_H__

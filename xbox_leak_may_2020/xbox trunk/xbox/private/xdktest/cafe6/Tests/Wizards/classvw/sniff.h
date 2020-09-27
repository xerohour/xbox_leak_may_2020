///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		JimGries			4/4/95
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

class CClassViewSubSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CClassViewSubSuite)

	DECLARE_TESTLIST()
};

#endif //__SNIFF_H__

///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//  Created by :                    Date :
//		MarcI & DougMan					1/1/97
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
	virtual void CleanUp(void);
};

#endif //__SNIFF_H__

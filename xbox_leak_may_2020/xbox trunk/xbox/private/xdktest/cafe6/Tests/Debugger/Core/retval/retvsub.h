///////////////////////////////////////////////////////////////////////////////
//	RetVsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CRetValSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CRetValSubSuite class

class CRetValSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CRetValSubSuite)
	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__SNIFF_H__

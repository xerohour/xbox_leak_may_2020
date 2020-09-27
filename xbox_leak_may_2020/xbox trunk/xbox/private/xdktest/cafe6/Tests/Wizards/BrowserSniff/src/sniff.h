///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		RickKr					8/30/93
//
//	Description :
//		Declaration of the CSniffDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define WSPEXTENSION	".DSW"
#define PRJEXTENSION	".DSP"


///////////////////////////////////////////////////////////////////////////////
// CSniffDriver class

class CSniffDriver : public CIDESubSuite
{
	DECLARE_SUBSUITE(CSniffDriver)
	
	BOOL fProjReady;

public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);
	
	int InitProject(void);

	DECLARE_TESTLIST()
};

#endif //__SNIFF_H__

///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		TomWh					11/1/93
//
//	Description :
//		Declaration of the CSniffDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define PROJNAME "Sys Test"
#define PROJDIR "Sys Test\\"

///////////////////////////////////////////////////////////////////////////////
// CSysSubSuite class

class CSysSubSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CSysSubSuite)

	DECLARE_TESTLIST()

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);

// data
public:
	COProject m_prj;
	COResScript m_res;
	CODebug m_dbg;
	COBrowse m_brz;
	COBreakpoints m_bp;
};

#endif //__SNIFF_H__

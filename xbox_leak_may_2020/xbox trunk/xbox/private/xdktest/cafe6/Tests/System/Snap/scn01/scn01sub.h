///////////////////////////////////////////////////////////////////////////////
//	scn01sub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CCreateBuildDebugAppWizAppSubSuite class
//

#ifndef __scn01sub_H__
#define __scn01sub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CCreateBuildDebugAppWizAppSubSuite class

class CCreateBuildDebugAppWizAppSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CCreateBuildDebugAppWizAppSubSuite)

	DECLARE_TESTLIST()

public:
	COProject m_prj;
	COResScript m_res;
/*	COConnection *m_pconnec;
	CODebug m_dbg;
	COBrowse m_brz;
	COBreakpoints m_bp;
*/
protected:
};

#endif //__scn01sub_H__

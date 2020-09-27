///////////////////////////////////////////////////////////////////////////////
//	scn02sub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CDebugAppWizAppSubSuite class
//

#ifndef __scn02sub_H__
#define __scn02sub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDebugAppWizAppSubSuite class

class CDebugAppWizAppSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CDebugAppWizAppSubSuite)

	DECLARE_TESTLIST()

public:
	COProject m_prj;
	COResScript m_res;

protected:
};

#endif //__scn02sub_H__

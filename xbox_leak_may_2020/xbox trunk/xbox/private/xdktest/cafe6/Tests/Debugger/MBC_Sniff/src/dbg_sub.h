///////////////////////////////////////////////////////////////////////////////
//	Dbg_sub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CDbg_mbcSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDbg_mbcSubSuite class

class CDbg_mbcSubSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CDbg_mbcSubSuite)

	DECLARE_TESTLIST()
};

#endif //__SNIFF_H__

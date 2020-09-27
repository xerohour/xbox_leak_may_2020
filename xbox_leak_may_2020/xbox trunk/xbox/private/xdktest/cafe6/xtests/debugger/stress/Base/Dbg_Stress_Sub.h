///////////////////////////////////////////////////////////////////////////////
//	Dbg_Stress_Sub.h
//
//	Created by :			
//		Xbox XDK Test
//
//	Description :
//		Declaration of the CDbgStressSubSuite class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CDbgStressSubSuite class

class CDbgStressSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CDbgStressSubSuite)
//exeSubSuite() { m_strName = "TestWizard Generated script"; m_strOwner = "Xbox XDK Test"; }


	DECLARE_TESTLIST()
protected:
	void CleanUp(void);
};

#endif //__SNIFF_H__

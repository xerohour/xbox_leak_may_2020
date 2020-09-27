///////////////////////////////////////////////////////////////////////////////
//	EECASE.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CEEIDETest Class
//

#ifndef __EESPECIAL_H__
#define __EESPECIAL_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "eesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CEEIDETest class

class CSpecialTypesTest : public CDbgTestBase
	
{
	DECLARE_TEST(CSpecialTypesTest, CEESubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
private:
	void PointerToFun(void);
	void GUIDs(void);
	//xbox - no variants
	//void Variants(void);
	void ErrorCodes(void);
	void MMXRegs(void);

	CString m_TIBValue;
};

#endif //__EESPECIAL_H__

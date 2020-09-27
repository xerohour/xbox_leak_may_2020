///////////////////////////////////////////////////////////////////////////////
//  INTSUB.H
//
//  Created by :            Date :
//      MichMa              	12/03/93
//
//  Description :
//      Declaration of the CIntSubSuite class
//

#ifndef __INTSUB_H__
#define __INTSUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


///////////////////////////////////////////////////////////////////////////////
// CIntSubSuite class

class CIntSubSuite : public CIDESubSuite
	{
	public:
		DECLARE_SUBSUITE(CIntSubSuite)
		DECLARE_TESTLIST()
	protected:
		void CleanUp(void);
	};

extern BOOL fProjectReady;

#endif //__INTSUB_H__

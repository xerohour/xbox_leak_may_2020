///////////////////////////////////////////////////////////////////////////////
//  MISCSUB.H
//
//  Created by :            Date :
//      MichMa              	12/03/93
//
//  Description :
//      Declaration of the CMiscSubSuite class
//

#ifndef __MISCSUB_H__
#define __MISCSUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


///////////////////////////////////////////////////////////////////////////////
// CMiscSubSuite class

class CMiscSubSuite : public CIDESubSuite
	{
	public:
		DECLARE_SUBSUITE(CMiscSubSuite)
		DECLARE_TESTLIST()
	protected:
		void CleanUp(void);
	};

extern BOOL fProjectReady;

#endif //__MISCSUB_H__

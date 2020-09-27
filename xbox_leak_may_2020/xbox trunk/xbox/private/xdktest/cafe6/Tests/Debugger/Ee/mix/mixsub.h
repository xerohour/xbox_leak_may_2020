///////////////////////////////////////////////////////////////////////////////
//  MIXSUB.H
//
//  Created by :            Date :
//      MichMa              	12/03/93
//
//  Description :
//      Declaration of the CMixSubSuite class
//

#ifndef __MIXSUB_H__
#define __MIXSUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


///////////////////////////////////////////////////////////////////////////////
// CMixSubSuite class

class CMixSubSuite : public CIDESubSuite
	{
	public:
		DECLARE_SUBSUITE(CMixSubSuite)
		DECLARE_TESTLIST()
	protected:
		void CleanUp(void);
	};

extern BOOL fProjectReady;

#endif //__MIXSUB_H__

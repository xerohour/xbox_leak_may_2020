///////////////////////////////////////////////////////////////////////////////
//	MPDCASES.H
//
//	Created by :			Date :
//		WayneBr				10/4/94
//
//	Description :
//		Declaration of the CMultipadCases class
//

#ifndef __MPDCASES_H__
#define __MPDCASES_H__

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	BOOL CMultipadCases:: class

class CMultipadCases : public CPerformanceCases 
{
	DECLARE_TEST(CMultipadCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);
	void		Initialize_Multipad();

};


#endif // MPDCASES_H__

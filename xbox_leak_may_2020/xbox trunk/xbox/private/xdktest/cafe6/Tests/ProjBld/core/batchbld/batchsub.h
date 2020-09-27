///////////////////////////////////////////////////////////////////////////////
//	batchsub.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CbatchsubSuite class
//

#ifndef __batchsub_H__
#define __batchsub_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CbatchsubSuite class

class CBatchSubSuite : public CIDESubSuite
{
public:
	DECLARE_SUBSUITE(CBatchSubSuite)

	DECLARE_TESTLIST()
protected:
};

#endif //__batchsub_H__

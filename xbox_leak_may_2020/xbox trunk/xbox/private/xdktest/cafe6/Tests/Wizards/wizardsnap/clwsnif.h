///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		RickKr					8/30/93
//
//	Description :
//		Declaration of the CSniffDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __WBDRV_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSniffDriver class

class CSniffDriver : public CWBDriver
{
public:
	CSniffDriver();

	DECLARE_TESTSET_MAP();

// Event Sequence
public:
 	virtual BOOL InitializeTest( void );
	virtual BOOL RunTests( void );
	virtual BOOL FinalizeTest( void );

// Internal Workings
protected:

// Utility Functions
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSniffDriver)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__SNIFF_H__

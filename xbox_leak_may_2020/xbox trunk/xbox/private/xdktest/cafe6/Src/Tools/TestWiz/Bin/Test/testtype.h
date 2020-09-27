///////////////////////////////////////////////////////////////////////////////
//	testtype.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the TestDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __WBDRV_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// TestDriver class

class TestDriver : public CWBDriver
{
public:
	TestDriver();

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
	//{{AFX_MSG(TestDriver)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__SNIFF_H__

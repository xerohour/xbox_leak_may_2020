// autownd.h : header file
//
#ifndef __AUTOWND_H__
#define __AUTOWND_H__

/////////////////////////////////////////////////////////////////////////////
// CAutoWnd window
#include "tokens.h"

class CAutoWnd : public CDbgGridWnd
{
// Construction
public:
	CAutoWnd(CBaseVarsView *pView);
	virtual ~CAutoWnd();
	typedef CDbgGridWnd CBaseClass;

// Attributes
public:

// Operations
public:
	virtual void UpdateEntries();

	void AddExprsFromSourceLines (const CString&, HSF, LONG);
	void AddExprsFromReturnValues ();
	void DeleteRetValRows();

	BOOL IsInterestingAutoTM(CTM *pTM); // Does this TM have the goodness to be put in the auto tab.

	void Parse(CTokenStream&, CTMArray&, CStrArray&);
	BOOL SkipComments(CTokenStream&, CTokenStreamPos&, BOOL bForward = TRUE);

	// Moves the position to the first location where an auto watch could start.
	BOOL MoveToStartToken(CTokenStream&, CTokenStreamPos&);
	BOOL IsAddressOperator(CTokenStream&, CTokenStreamPos&);	// see comments at begining of function defn.

	BOOL GetInitialOperators(CTokenStream&, CTokenStreamPos&, CString&);
	BOOL CollectIndexStr(CTokenStream&, CTokenStreamPos&, CString&);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bLineInfoValid:1;

	enum { maxLinesScanned = 10 };
	
	HSF m_hsf;			// Handle to the source file which has the current address.
	LONG m_curLineNo;	// Line no within the file where the current CXF is.
	CString m_strFileName;	// The actual filename last time around.

	// Generated message map functions
protected:
	//{{AFX_MSG(CAutoWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	virtual LRESULT OnUpdateDebug(WPARAM, LPARAM);
	virtual LRESULT OnClearDebug(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // __AUTOWND_H__

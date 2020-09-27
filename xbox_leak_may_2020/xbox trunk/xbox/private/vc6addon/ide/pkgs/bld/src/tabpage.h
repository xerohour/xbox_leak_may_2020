// tabpage.h : header file
//

#ifndef __TABPAGE_H__
#define __TABPAGE_H__

#include "dlgbase.h"
#include "utilctrl.h"

//
// CStringListGridWnd_WithChange is a CStringListGridWnd that sends GLN_CHANGE when
// a change has occurred.
//

// This must not collide with any other GLN_* notification defined in utilctrl.h.
#define GLN_CHANGE (12)

class CStringListGridWnd_WithChange : public CStringListGridWnd
{
// Construction
public:
	CStringListGridWnd_WithChange(UINT nModeFlags = 0) : CStringListGridWnd(nModeFlags) {}

// Overrides
protected:
	virtual BOOL OnChange(int nIndex);
	virtual BOOL OnAddString(int nIndex);
	virtual BOOL OnDeleteString(int nIndex);
	virtual BOOL OnMove(int nSrcIndex, int nDestIndex);

protected:
	void Send_GLN_CHANGE();
};


// CSlobPages are CDlgTabs that use the CSlob property mechanism to
// automatically handle the dialog when viewed as part of a tabbed control.

// Derived classes should handle the committing of properties to the 
// approp. CSlob's on an OK, and the ignoring of any property changes
// on a Cancel. This will *not* be handled by CSlobPageTab.

// this is much like the CSheetSlob

class CSlobPageTab;

class CSlobPageWatch : public CSlob
{
public:
	void OnInform(CSlob *, UINT, DWORD);
	CSlobPageTab * m_pSlobPageTab;
};

class CSlobPageTab : public CDlgTab
{
	DECLARE_DYNAMIC(CSlobPageTab)
	DECLARE_IDE_CONTROL_MAP()
	DECLARE_MESSAGE_MAP()

public:
	CSlobPageTab();
	~CSlobPageTab();

	BOOL SetupPage(CSlob * pSlob);

	virtual BOOL OnInitDialog();

	// initialization and termination of the property page
	virtual void InitPage();
	virtual void TermPage();

	// hook into property value changing
	virtual BOOL OnCommand(UINT wParam, LONG lParam);
	virtual BOOL OnPropChange(UINT nProp);

	// contol map dialog id and name extraction
	virtual int GetDlgID() {return GetControlMap()->m_nCtlID;}
	virtual void GetName(CString& strName);

	// property value validation
	virtual BOOL Validate();
	LONG OnValidateReq(UINT, LONG);
	int m_nValidateID;

	// control map searching
	CControlMap* FindControl(UINT nCtlID);
	CControlMap* FindProp(UINT nProp);

	// get our CSlob for this CSlobPageTab
	CSlob * GetSlob() {return m_pSlob;}

protected:
	// override CDlgTab methods
	virtual BOOL Activate(CTabbedDialog * pParentWnd, CPoint position);
	virtual BOOL ValidateTab();
	virtual void CommitTab();
	virtual void CancelTab();
	
	CTabbedDialog * m_pTabbedDialog;
	CPropCheckList m_PropCheckList;
	
	// Our slob which is used to store/retrieve properties to/from
	// You'll need to handle the commit semantics yourself.
	// ie. On Cancel undo property changes to this CSlob, or
	// on OK commit changes from a tmp. CSlob (this one) into the actual one
	CSlob* m_pSlob;

	// Our 'watcher' CSlob
	CSlobPageWatch m_slobWatch;

	// A cache of Check-List objects
	CPtrList m_listCheckList;

	BOOL m_isValidating;

private:
	BOOL m_bIgnoreChange;
	BOOL m_bInit;
};

/////////////////////////////////////////////////////////////////////////////

#endif // __TABPAGE_H__

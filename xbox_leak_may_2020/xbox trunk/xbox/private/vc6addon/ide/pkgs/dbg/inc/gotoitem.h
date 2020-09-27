
#ifndef _GOTOITEM_H_
#define _GOTOITEM_H_

// Helper: return True if pActiveView is a valid CTextView pointer
BOOL FValidPTextView(CPartView * pActiveView);
// Helper: return True if current active view is a valid CTextView pointer
BOOL FValidCurTextView(CGoToDialog * pDlg);
// Helper: returns dialogs current active view if it is a CTextView, otherwise NULL
CMultiEdit *PCurTextView(CGoToDialog * pDlg);

////////////////////////////////////////////////////////////
// CGoToLine

class CGoToLine : public CGoToItem
{

public:
	ILINE m_ilineGoTo;  // computed in CanGoTo, used in OnGoTo;

public:

	// ctor
	CGoToLine(CGoToDialog * pDlg);

	// ***Initialization. These functions are used to initialize the Go To
	// dialog. They will be called when the dialog is invoked but may
	// actually be called at any time.

	// REQUIRED: Return the string to show in the Go To What listbox

	virtual const TCHAR * GetItemString();

	// ***Callbacks. These functions are called as the user manipulates the Go To
	// dialog or active editor view

	// Called when the item is selected in the Go To What listbox,
	// and at CGoToDialog startup and reactivation.
	// CGoToDialog will call CanGoViewActive after returning from this
	// call, so let it handle situational enabling/disabling

	// Note that this function needs to be able to tolerate a NULL m_pDlg->m_pActiveView
	// or one of the incorrect type. It should set what it can and leave other controls empty.

	// Do not put up error messages or dialogs during this function, as it can be called when focus
	// is changing. If you need to check a state that might bring up such a message it would be better
	// to enable the goto button and check at CanGoTo or OnGoTo rather than doing it here

	virtual void OnSelectWhat();

	// Called when the active editor changes, and after OnSelectWhat is called.
	// A FALSE return indicates that the item should not be enabled in the dialog,
	// TRUE indicates that the item may be active.
	// check all situations that may require deactivation here (e.g., deactivate if
	// editor specific and in wrong editor, or if going to an empty list, e.g., of bookmarks)

	virtual BOOL CanGoViewActive(CPartView * pActiveView);

	// Called when the selection control changes (either the user changed
	// the text in the edit control or they selected something from the
	// drop list). For csEdit style selection controls, the fSelInList
	// and dwItemData parameters should be ignored (FALSE/-1 will always
	// be passed here).
	// fSelInList will be true if there is an item selected in the list
	// or combo box.

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

	// generally used for simple test like handling empty strings

	virtual void OnSelectChange(const TCHAR * szSelect,
								BOOL fSelInList,
								DWORD dwItemData);

	// Override this to indicate whether or not a go to operation can be
	// attempted. Done when an action button is hit.
	// For goNext and goPrevious operations, the szSelect
	// and dwItemData items are not used.
	// For goGoTo operations, they describe the string and optional
	// user-data selected in the Go To dialog.
	// fSelInList will be true if there is an item selected in the list
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"


	// pDlg and idDlgItemError are used to return focus to in case of error

	virtual BOOL CanGoTo(int idDlgItemError,
						GOTO_TYPE go,
						const TCHAR * szSelect,
						BOOL fSelInList,
						DWORD dwItemData);


	// Override this to handle a go to operation. For goNext and goPrevious
	// operations, the szSelect and iSelList, and dwItemData items are not used. For goGoTo
	// operations, they describe the string and optional listbox user-data
	// selected in the Go To dialog, and the list selection, if any.
	// fSelInList will be true if there is an item selected in the list
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"


	virtual void OnGoTo(GOTO_TYPE go,
						const TCHAR * szSelect,
						BOOL fSelInList,
						DWORD dwItemData);

};

class CGoToAddress : public CGoToItem
{
public:
	ADDR	m_addr;  // set in CanGoTo, used in OnGoTo

public:

	// ctor
	CGoToAddress(CGoToDialog * pDlg);

	// ***Initialization. These functions are used to initialize the Go To
	// dialog. They will be called when the dialog is invoked but may
	// actually be called at any time.

	// REQUIRED: Return the string to show in the Go To What listbox

	virtual const TCHAR * GetItemString();

	// ***Callbacks. These functions are called as the user manipulates the Go To
	// dialog or active editor view

	// Called when the item is selected in the Go To What listbox,
	// and at CGoToDialog startup and reactivation.
	// CGoToDialog will call CanGoViewActive after returning from this
	// call, so let it handle situational enabling/disabling

	// Do not put up error messages or dialogs during this function, as it can be called when focus
	// is changing. If you need to check a state that might bring up such a message it would be better
	// to enable the goto button and check at CanGoTo or OnGoTo rather than doing it here

	virtual void OnSelectWhat();

	// Called when the active editor changes, and after OnSelectWhat is called.
	// A FALSE return indicates that the item should not be enabled in the dialog,
	// TRUE indicates that the item may be active.
	// check all situations that may require deactivation here (e.g., deactivate if
	// editor specific and in wrong editor, or if going to an empty list, e.g., of bookmarks)

	virtual BOOL CanGoViewActive(CPartView * pActiveView);

	// Called when the selection control changes (either the user changed
	// the text in the edit control or they selected something from the
	// drop list). For csEdit style selection controls, the fSelInList
	// and dwItemData parameters should be ignored (FALSE/-1 will always
	// be passed here).
	// fSelInList will be true if there is an item selected in the list
	// or combo box.

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

	// generally used for simple test like handling empty strings

	virtual void OnSelectChange(const TCHAR * szSelect,
								BOOL fSelInList,
								DWORD dwItemData);

	// Override this to indicate whether or not a go to operation can be
	// attempted. Done when an action button is hit.
	// For goNext and goPrevious operations, the szSelect
	// and dwItemData items are not used.
	// For goGoTo operations, they describe the string and optional
	// user-data selected in the Go To dialog.
	// fSelInList will be true if there is an item selected in the list
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"


	// pDlg and idDlgItemError are used to return focus to in case of error

	virtual BOOL CanGoTo(int idDlgItemError,
						GOTO_TYPE go,
						const TCHAR * szSelect,
						BOOL fSelInList,
						DWORD dwItemData);


	// Override this to handle a go to operation. For goNext and goPrevious
	// operations, the szSelect and iSelList, and dwItemData items are not used. For goGoTo
	// operations, they describe the string and optional listbox user-data
	// selected in the Go To dialog, and the list selection, if any.
	// fSelInList will be true if there is an item selected in the list
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"


	virtual void OnGoTo(GOTO_TYPE go,
						const TCHAR * szSelect,
						BOOL fSelInList,
						DWORD dwItemData);

};

#endif // !_GOTOITEM_H_

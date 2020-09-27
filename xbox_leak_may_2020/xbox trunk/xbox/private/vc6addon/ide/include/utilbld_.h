/////////////////////////////////////////////////////////////////////////////
//	UTILBLD_.H
//		Microsoft only utilities used mostly for the build system.

#ifndef __UTILBLD__H__
#define __UTILBLD__H__

#include "utilctrl.h"
#include "msgboxes.h"
#include "dlgbase.h"

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

interface IPkgProjectProvider;

///////////////////////////////////////////////////////////////////////////////
//	CEnvironmentVariableList
//		Class to hold a list of enviroment variable pairs.  Handy because it
//		resets the variable to there old values on destructions:
	
class CEnvironmentVariableList : public CObject
{
	CStringArray m_VariableNames;
	CStringArray m_VariableValues;
	BOOL m_bHoldingOld;
public:
	CEnvironmentVariableList () { m_bHoldingOld = FALSE; };
	~CEnvironmentVariableList() { if (m_bHoldingOld) ResetVariables (); };

	void AddVariable ( const TCHAR *pName, const TCHAR *pValue );
	// Set the values of the pairs in to the enviroment.  The old values
	// are saved over the new settings according to the flag:
	virtual void SetVariables ( BOOL bSaveOldValues = TRUE );
	// Reset the values saved when SetVariables was called:
	inline void ResetVariables () 
				{ ASSERT (m_bHoldingOld); SetVariables(FALSE);};
	inline void Clear () {  m_VariableNames.RemoveAll ();
							m_VariableValues.RemoveAll (); };
};

/////////////////////////////////////////////////////////////////////////////
//	CErrorContext
//
//	Class for error contexts.  These are things you pass to a function to that
//	it can record any error messages it generates.  Thus a typiccal example is
//
//		BOOL SomeFunc ( argc ... , CErrorContext EC& = g_DummyEC );
//
//	The purpose of these things is to keep messages for the user, not to save
//	information for calling functions to examine.  Thus you should not examine
//	the contents of an EC to figure out what happened, rely on return codes or
//	whatever instead.

class CErrorContext : public CObject
{	 
	DECLARE_DYNCREATE (CErrorContext);
public:
	virtual ~CErrorContext() {};

	//	Add a string to the ErrorConext.  Depending on the context, the string
	//	may or may not be displayed at this point:
	virtual void AddString ( const TCHAR *pstr ) {};
	virtual void AddString ( UINT ResID ) {};

	// Fill a string with all the messages in the context in the order in which
	// they we're added.  This is for _display_ only.  Classes the diplay 
	// messages as they occur may not implement this:
	virtual void GetString ( CString &rstr ) {};
	
	// Fill a string with all the messages in the context in the opposite order 
	// in which they we're added.  This should give a message of increasing 
	// specificity. For example:
	//
	//					Could not load document.
	//					Could not open file "blix.doc."
	//					Disk CRC error in sector la-de-da.
	// This is for _display_ only.  Classes the diplay messages as they occur 
	// may not implement this:
	virtual void GetRevString ( CString &rstr ) {};

	// Bring up a message box with the contents of the context.  Also not 
	// implemented by classes that display imediatetly:
	virtual int DoMessageBox ( MsgBoxTypes MsgBoxType = Error,
								UINT nButtonIDs = DEFAULT_BUTTONS,
								UINT nHelpContext = DEFAULT_HELP )
				 { return MB_OK; };

	//	Clear the content of the context and prepared for a new, unrelated
	//	error:
	virtual void Reset () {};
};

// Global dummy context that can be used as a default argument:
extern CErrorContext AFX_DATA g_DummyEC;

//	Simple error context that just keeps a list of messages:
class CStorageEC : public CErrorContext 
{
	DECLARE_DYNCREATE (CStorageEC);
protected:
	CStringList m_StringList;
public:
	virtual ~CStorageEC() {};

	virtual void AddString ( const TCHAR *pstr ); 
	virtual void AddString ( UINT ResID ); 

	virtual void GetString ( CString &rstr );
	virtual void GetRevString ( CString &rstr );

	virtual int DoMessageBox ( const MsgBoxTypes MsgBoxType = Error,
								UINT nButtonIDs = DEFAULT_BUTTONS,
								UINT nHelpContext = DEFAULT_HELP );

	virtual void Reset ();
}; 

class CProjTypeItem
{
public:
	CProjTypeItem(HICON hIcon, LPCTSTR strName, LPCTSTR strExtension,
		LPCTSTR strTabName, BOOL bSupportDependency, BOOL bUseSubDirectories,
		DWORD dwProject_ID, IPkgProjectProvider *pProjectProvider)
	{
		m_hIcon = hIcon;
		m_strName = strName;
		m_strExtension = strExtension;
		m_strTabName = strTabName;
		m_bSupportDependency = bSupportDependency;
		m_bUseSubDirectories = bUseSubDirectories;
		m_dwProject_ID = dwProject_ID;
		m_pProjectProvider = pProjectProvider;
	}

	~CProjTypeItem() { }

public:
	HICON m_hIcon;
	CString m_strName;
	CString m_strExtension;
	CString m_strTabName;
	BOOL m_bSupportDependency;
	BOOL m_bUseSubDirectories;
	DWORD m_dwProject_ID;
	IPkgProjectProvider *m_pProjectProvider;
};


// Classification for items in the Project Type icon list box
enum TYPE_ITEM_CLASS { appWiz, customWiz, buildSys, test, makefile };

// This represents one item in the Project Type icon list box
class CProjTypeListItem
{
public:
	CProjTypeListItem(const TCHAR* szText, HICON hIcon, TYPE_ITEM_CLASS itemClass, int index=0);
	~CProjTypeListItem();

	const TCHAR* GetText() const
		{ return m_strText; }

	HICON GetIcon() const
		{ return m_hIcon; }

	TYPE_ITEM_CLASS GetClass() const
		{ return m_class; }

	BOOL IsAnAppWizard() const
		{ return m_class == appWiz || m_class == customWiz; }

	BOOL HasCreateDialog() const
		{ return IsAnAppWizard() || m_class == makefile; }

	int GetIndex() const { return m_index; }
	void SetIndex(int newIndex) { m_index = newIndex; }

protected:
	CString m_strText;
	TYPE_ITEM_CLASS m_class;
	HICON m_hIcon;
	int m_index;
};


// The Project Type icon list box  THIS CLASS IS TEMPORARY
class CProjTypeIconList : public CIconList
{
public:
	virtual void SetIcon(int nItem, HICON hIcon);
	virtual HICON GetIcon(int nItem);

	int AddItem(CProjTypeListItem* pItem);
	void InsertItem(int nInsertAt, CProjTypeListItem* pItem);

protected:
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};


// The Project Type list
class CProjTypeList
{
public:
	virtual HICON GetIcon(int nItem);

	int AddItem(CProjTypeListItem* pItem);
	void InsertItem(int nInsertAt, CProjTypeListItem* pItem);

	int GetCount() { ASSERT(m_strings.GetSize() == m_dataPtrs.GetSize());
					return m_strings.GetSize(); }
	int AddString(LPCTSTR str);
	int InsertString(int nItem, LPCTSTR str);
	void GetText(int nItem, CString &str);
	void *GetItemDataPtr(int nItem);
	void SetItemDataPtr(int nItem, void *ptr);

	~CProjTypeList();
	CProjTypeList();

private:
	CStringArray m_strings;
	CPtrArray m_dataPtrs;
};

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif	// __UTILBLD__H__

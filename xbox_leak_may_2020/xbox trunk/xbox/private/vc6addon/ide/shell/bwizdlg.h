// bwizdlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CBWizEnumItem
class CBWizEnumItem
{
public:
	CBWizEnumItem(LPCTSTR szCLSID, LPCTSTR szDesc);
	CBWizEnumItem();

	CString m_strCLSID;
	CString m_strDesc;
};


/////////////////////////////////////////////////////////////////////////////
// Registry class
class CRegLite
{
private:
   HKEY  m_hKey;
   DWORD m_cbSubKeys;         // number of sub keys
   DWORD m_cbMaxSubKeyName;   // length of longest sub key name
   DWORD m_cbMaxClassName;    // length of longest class name
   DWORD m_cbValues;          // number of value entries
   DWORD m_cbMaxValueName;    // lenght of longest value name
   DWORD m_cbMaxValueData;    // lenght of longest value data
   BOOL  m_fFreeKey;          // Should I free the key on close?
   
public:
   CRegLite();
   ~CRegLite();

public:   
   BOOL     Open(HKEY hParent, LPCTSTR szSubKey);
   BOOL     GetValue(LPCTSTR szSubKey, CString* pstrData);
   BOOL     IsValueEqual(LPCTSTR szSubKey, LPCTSTR szValue);
   BOOL     SetHKEY(HKEY hKey);

   HKEY     GetHandle()          { return m_hKey; }; // close only if used SetHKEY
   DWORD    GetCSubKeys()        { return m_cbSubKeys; };
   DWORD    GetCbMaxSubKeyName() { return m_cbMaxSubKeyName + sizeof(TCHAR); };
   DWORD    GetCbMaxClassName()  { return m_cbMaxClassName + sizeof(TCHAR); };
   DWORD    GetCValues()         { return m_cbValues + sizeof(TCHAR); };
   DWORD    GetCbMaxValueName()  { return m_cbMaxValueName + sizeof(TCHAR); };
   DWORD    GetCbMaxValueData()  { return m_cbMaxValueData + sizeof(TCHAR); };

public:
   void     Close();
   
private:
   BOOL  InitData();
};

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrl replacement class
//
class CTreeLite
{
private:
   HWND        m_hWnd;
   int         m_cbMaxImages;
   BOOL        m_fInitialized;
   CImageList  m_iml;
   
public:
   CTreeLite();
   ~CTreeLite();

public:   
   void  SetMaxImages(int cbImages) { m_cbMaxImages = cbImages; };
   BOOL  SetTreeCtrl(HWND hwndDlg, int iChildWindowID);
   BOOL  AddIconImage(UINT uiResource);
	int   AddIconImage(HICON hIcon);

public:   
   inline int        GetImageCount();
   inline HWND       GetHandle() { return m_hWnd; };
   inline HTREEITEM  GetFirstVisible();
   inline HTREEITEM  GetNextVisible(HTREEITEM hti);
   inline LPARAM     GetItemData(HTREEITEM hti);
   inline void       SelectItem(HTREEITEM hti);
   inline HTREEITEM  GetRoot();
   inline HTREEITEM  GetSelectedItem();
   inline HTREEITEM  InsertItem(TV_INSERTSTRUCT* tvis);
   inline BOOL       Expand(HTREEITEM hti);
};

/////////////////////////////////////////////////////////////////////////////
// CBldWizSelectDlg dialog

class CBldWizSelectDlg : public C3dDialog
{
// Construction
public:
   CBldWizSelectDlg(CWnd* pParent = NULL);   // standard constructor
   ~CBldWizSelectDlg();

// Dialog Data
   //{{AFX_DATA(CBldWizSelectDlg)
   enum { IDD = IDD_BWIZ_ENUM };
   //}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CBldWizSelectDlg)
   //not using
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CBldWizSelectDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnDblclkSeltree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeleteItemSeltree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSelchangedSeltree(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   BOOL        m_fInitialized;   // Indicates if Dialog initialized properly
   CRegLite    m_regCATID;       // reg for CATID
   CRegLite    m_regDevCompCat;  // reg for Developer Hive
   int         m_cBuilders;      // # of builders
   CTreeLite   m_tree;
   
public:
   CLSID       m_clsidSel;       // Selected Builder's CLSID
   BOOL        SetCATID(HKEY hKey);

private:
   BOOL        InitTreeCtrl();
   BOOL        AddCategory(CRegLite* pReg, HTREEITEM htiParent=NULL);
   BOOL        AddMetaCategory(CRegLite* pReg, HTREEITEM htiParent=NULL);
   HTREEITEM   DisplayBuilder(LPCTSTR szCLSID, HTREEITEM hParent);
   HTREEITEM   DisplayCategory(LPCTSTR szCategory, HTREEITEM hParent);
};

//UNDONE$: Move this func to bwizmgr.cpp
extern BOOL IsMetaCATID(HKEY hCATID);

#if !defined(AFX_OLEPROPPAGE_H__469F345B_1A8B_11D3_B009_00105AA22F40__INCLUDED_)
#define AFX_OLEPROPPAGE_H__469F345B_1A8B_11D3_B009_00105AA22F40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OlePropPage.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdUpdateObjectNow

// {18890BB4-97EE-465b-B523-2939C44CB2B6}
DEFINE_GUID( IID_IDMUSProdUpdateObjectNow, 0x18890bb4, 0x97ee, 0x465b, 0xb5, 0x23, 0x29, 0x39, 0xc4, 0x4c, 0xb2, 0xb6);

#undef INTERFACE
#define INTERFACE IDMUSProdUpdateObjectNow
DECLARE_INTERFACE_(IDMUSProdUpdateObjectNow, IUnknown)
{
    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)() PURE;
    STDMETHOD_(ULONG, Release)() PURE;

    // IDMUSProdUpdateObjectNow members
    STDMETHOD(UpdateObjectNow)(LPUNKNOWN punkObject) PURE;
};

// COlePropPageTemplate - dialog template class used by COlePropPage
struct COlePropPageTemplate : public DLGTEMPLATE
{
    WCHAR MenuName;
    WCHAR ClassName;
    WCHAR szCaption;
    WORD wPointSize;
    WCHAR szFontName;

    COlePropPageTemplate();
    void SetSize(SIZE& size);
};

/////////////////////////////////////////////////////////////////////////////
// COlePropPage dialog

class COlePropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COlePropPage)

// Construction
public:
	IPropertyPage*				m_pPropPage;
	LPUNKNOWN					m_pObject;
	IDMUSProdUpdateObjectNow*	m_pIDMUSProdUpdateObjectNow;
	CLSID						m_clsidPage;
    COlePropPageTemplate		m_dlgTemplate;
	bool						m_fConstructionSucceeded;

	COlePropPage(CLSID clsidPage, LPUNKNOWN lpUnkPage, IDMUSProdUpdateObjectNow *pIDMUSProdUpdateObjectNow);
	COlePropPage();
	~COlePropPage();

// Dialog Data
	//{{AFX_DATA(COlePropPage)
	enum { IDD = IDD_OLE_PROPPAGE_LARGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COlePropPage)
	public:
	virtual void OnFinalRelease();
	virtual BOOL OnKillActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COlePropPage)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	HWND	m_hwndProducer; // Handle to Producer's main window

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(COlePropPage)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

    BEGIN_INTERFACE_PART(PropertySite, IPropertyPageSite)
        STDMETHOD(GetLocaleID)(LCID* pLocaleID);
        STDMETHOD(OnStatusChange)(DWORD dwFlags);
        STDMETHOD(GetPageContainer)(IUnknown **ppUnk);
        STDMETHOD(TranslateAccelerator)(MSG *pMsg);
	END_INTERFACE_PART(PropertySite)
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLEPROPPAGE_H__469F345B_1A8B_11D3_B009_00105AA22F40__INCLUDED_)

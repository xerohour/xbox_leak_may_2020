#if !defined(AFX_CONDITIONCONFIGTOOLBAR_H__789EFBE2_AC84_44D9_B1DF_20FE23EC4112__INCLUDED_)
#define AFX_CONDITIONCONFIGTOOLBAR_H__789EFBE2_AC84_44D9_B1DF_20FE23EC4112__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// conditionconfigtoolbar.h : header file
//

#include "DMUSProd.h"

/////////////////////////////////////////////////////////////////////////////
// CConditionConfigToolbar window

class CDLSComponent;
class CSystemConfiguration;

class CConditionConfigToolbar : public CToolBarCtrl, public IDMUSProdToolBar
{

	// Construction
public:
	CConditionConfigToolbar(CDLSComponent* pComponent = NULL);

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppv);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();


	// IDMUSProdToolBar methods
	HRESULT STDMETHODCALLTYPE GetInfo(HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle);
	HRESULT STDMETHODCALLTYPE GetMenuText(BSTR* pbstrText);
	HRESULT STDMETHODCALLTYPE GetMenuHelpText(BSTR* pbstrHelpText);
	HRESULT STDMETHODCALLTYPE Initialize(HWND hWndToolBar);

	// Methods to deal with setting up conditions in the combo
	void	RefreshConfigCombo(CPtrList* pListOfConditions);
	void	OnSelchangeCombo();
	void	OnAddButtonClicked();
	void	OnDeleteButtonClicked();
	void	OnEditButtonClicked();
	
	CSystemConfiguration* GetCurrentConfig();
	void SetCurrentConfig(CSystemConfiguration* pConfig);


// Attributes
public:

private:
	CDLSComponent* m_pComponent;

	CFont		m_font;
	CComboBox	m_ConfigsCombo;
	DWORD		m_dwRef;
	
	CMenu*		m_pPopupMenu;
	
	CSystemConfiguration*	m_pCurrentConfig;	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConditionConfigToolbar)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConditionConfigToolbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CConditionConfigToolbar)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDITIONCONFIGTOOLBAR_H__789EFBE2_AC84_44D9_B1DF_20FE23EC4112__INCLUDED_)

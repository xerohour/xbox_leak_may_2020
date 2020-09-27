//
// CTargetCombo
//
// Declarations for the target combo
//
// [colint]
//

#ifndef _INCLUDE_TARGETCOMBO_H
#define _INCLUDE_TARGETCOMBO_H

#include "shlbar.h"

class CBuildCombo : public CDockCombo
{
// Constructors
public:
	virtual CObList * GetComboList()	{ return NULL; }

	BOOL Create(DWORD dwStyle,  const RECT& rect, CWnd * pWnd, UINT nID);
	virtual void PostNcDestroy();

    virtual void UpdateView(){};
	virtual void RefreshTargets(){};

	void SetSize();
	void EnableItem(int iItem, BOOL fEnable = TRUE);

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

private:
	int m_cyItem;
};

class CTargetCombo : public CBuildCombo
{
public:
	CTargetCombo();
	~CTargetCombo();
	virtual CObList * GetComboList()	{ return &s_listCombos; }
    virtual void UpdateView();
	virtual void RefreshTargets();
	static CObList * GetCombos() { return &s_listCombos; }
protected:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * lResult);
	static CObList	s_listCombos;
};

class CConfigCombo : public CBuildCombo
{
public:
	CConfigCombo();
	~CConfigCombo();
	virtual CObList * GetComboList()	{ return &s_listCombos; }
	virtual void UpdateView();
	virtual void RefreshTargets();
	static CObList * GetCombos() { return &s_listCombos; }

protected:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * lResult);
	static CObList	s_listCombos;
};

class CConfigControl : public CWnd
{
public:
	CConfigControl();
	~CConfigControl();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd * pParentWnd, UINT nID);
	virtual void PostNcDestroy();

	static CObList * GetControlList()	{ return &s_listControls; }

    void UpdateView() { m_pConfigCombo->UpdateView(); }
	void RefreshTargets() { m_pConfigCombo->RefreshTargets(); }

    //{{AFX_MSG(CTargetControl)
	afx_msg void OnSelChange();
	afx_msg void OnDropList() ;
    //}}AFX_MSG

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	CConfigCombo *	m_pConfigCombo;
	static CObList	s_listControls;

    DECLARE_MESSAGE_MAP()
};

class CTargetControl : public CWnd
{
public:
	CTargetControl();
	~CTargetControl();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd * pParentWnd, UINT nID);
	virtual void PostNcDestroy();

	static CObList * GetControlList()	{ return &s_listControls; }

    void UpdateView() { m_pTargetCombo->UpdateView(); }
	void RefreshTargets() { m_pTargetCombo->RefreshTargets(); }

    //{{AFX_MSG(CTargetControl)
	afx_msg void OnSelChange();
	afx_msg void OnDropList() ;
    //}}AFX_MSG

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	CTargetCombo *	m_pTargetCombo;
	static CObList	s_listControls;

    DECLARE_MESSAGE_MAP()
};

#endif // _INCLUDE_TARGETCOMBO_H

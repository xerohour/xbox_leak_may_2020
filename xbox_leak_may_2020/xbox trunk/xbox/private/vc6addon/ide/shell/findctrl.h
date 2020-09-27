///////////////////////////////////////////////////////////////////////////////
//	FINDCTRL.H
//		Declarations for the find combo.

#ifndef __FINDCTRL_H__
#define __FINTCTRL_H__

#include "shlbar.h"

HWND CreateFindCombo(DWORD dwStyle, RECT* lpRect, HWND hwndParent, UINT nID);

class CFindCombo : public CToolbarEditCombo
{
// Construction.
public:
	CFindCombo();
	~CFindCombo();

	Create(DWORD dwStyle, const RECT& rect, CWnd* pWnd, UINT nID);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	BOOL UpdateFindInfo(BOOL bUpdateDropDown);
	void UpdateDropDown();
	virtual void UpdateView();
	virtual void OnEditSetFocus(void);
	virtual void OnEditKillFocus(void);
	virtual BOOL OnEditHelp(void);
	static void UpdateAllViews();
	static void SetFindFocus();
	BOOL DoHelp(void);

    DECLARE_MESSAGE_MAP()

// Attributes.
protected:
	static CObList s_listCombos;
	UINT m_nId; // Help ID

public:
	DECLARE_DYNAMIC(CFindCombo);
};

class CFindComboEdit : public CWnd
{
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	CFindCombo* GetFindCombo(void);

public:
	DECLARE_DYNAMIC(CFindComboEdit);
};

#endif	//	__FINDCTRL_H__

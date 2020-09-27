// datatip.h : header file for CDataTip class

#ifndef __DATATIP_H__
#define __DATATIP_H__

class CDataTip : public CWnd
{
// Implementation
public:
	CDataTip();

	static CDataTip *m_pTip;
	static int m_x;
	static int m_y;

	virtual void DestroyTip();

	static CDataTip *GetTip() { return m_pTip; }
	static int GetX() { return m_x; }
	static int GetY() { return m_y; }
	static void SetX(int x) { m_x = x; }
	static void SetY(int y) { m_y = y; }

	//{{AFX_MSG(CDataTip)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nTimerID);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisableModal(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // __DATATIP_H__

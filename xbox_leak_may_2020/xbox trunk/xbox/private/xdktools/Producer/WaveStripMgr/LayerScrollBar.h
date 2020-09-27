#if !defined(AFX_LAYERSCROLLBAR_H__2D72BA9D_7BDD_11D1_885E_00C04FBF8D15__INCLUDED_)
#define AFX_LAYERSCROLLBAR_H__2D72BA9D_7BDD_11D1_885E_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// LayerScrollBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayerScrollBar window

class CLayerScrollBar : public CScrollBar
{
// Construction
public:
	CLayerScrollBar();
	virtual ~CLayerScrollBar();

// Attributes
private:
	class CWaveStrip* m_pWaveStrip;

// Operations
public:
	void SetWaveStrip( class CWaveStrip* pWaveStrip );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerScrollBar)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CLayerScrollBar)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERSCROLLBAR_H__2D72BA9D_7BDD_11D1_885E_00C04FBF8D15__INCLUDED_)

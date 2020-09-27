/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ChildView.h

Abstract:

	Main window

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version
	17-Jul-2001	robheit
		Added support for the application toolbar

--*/
#if !defined(AFX_CHILDVIEW_H__147C6C30_763C_4DA0_B48D_424E36301991__INCLUDED_)
#define AFX_CHILDVIEW_H__147C6C30_763C_4DA0_B48D_424E36301991__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Winsock2.h"
#include "Graph.h"

//------------------------------------------------------------------------------
//	CChildView
//------------------------------------------------------------------------------
class CChildView : public CWnd
{
// Construction
public:

	CChildView();
	virtual ~CChildView();

public:
	
	void EnableMenuItem(CMenu*	pPopupMenu, UINT nIndex);
	BOOL Quit(void);
	LPCTSTR GetStatusBarText(void);
	void Reset(void);
	void UpdateScrollBars(void);
	void TransmitModuleParameters(const CModule* pModule, int index=-1);
	
//	inline const CPoint& GetOffset(void) const { return m_offset; };
//	inline BOOL GetConnected(void) const { return m_bConnected; };

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	void OnUpdateXboxTransmitImageUI(CCmdUI* pCmdUI);
	void OnUpdateFileNewUI(CCmdUI* pCmdUI);
	void OnUpdateFileSaveUI(CCmdUI* pCmdUI);
	void OnUpdateFileOpenUI(CCmdUI* pCmdUI);
	void OnUpdateFileGenSaveImgUI(CCmdUI* pCmdUI);

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileGenerateAndSaveImage();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnXboxTransmitImage();
	afx_msg void OnToolsDirectories();
	afx_msg void OnToolsBuildOptions();
	afx_msg void OnToolsTransmitOptions();
	afx_msg void OnGraphShowGrid();
	afx_msg void OnToolsSnapToGrid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BOOL Connect(LPCTSTR);
	void Disconnect(void);
	DWORD GetCRC(const VOID* pBuffer, ULONG bufferSize) const;

private:

	CGraph*		m_pGraph;
	BOOL		m_bConnected;
	SOCKET		m_socket;
	CString		m_xboxName;
	CString		m_statusBarText;
	CString		m_transmitDestination;
	CPoint		m_offset;
	int			m_width;
	int			m_height;
	DWORD		m_dwParamOffset;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__147C6C30_763C_4DA0_B48D_424E36301991__INCLUDED_)

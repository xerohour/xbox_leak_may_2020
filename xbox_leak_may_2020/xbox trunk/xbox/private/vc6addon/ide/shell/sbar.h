// sbar.h
//
// Header file for the Sushi status bar.

// Copyright (C) 1993 Microsoft Corporation, all rights reserved.

#ifndef __SBAR_H__
#define __SBAR_H__

#include "oldstat.h"

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
// CStatusClock

class CStatusClock
{
public:
	CString Format();
};

/////////////////////////////////////////////////////////////////////////////
// CSushiBar
//

class CSushiBarInfo;

class CSushiBar : public CStatusBar
{
	DECLARE_DYNAMIC(CSushiBar)

public:
	enum {
		MAXPANES = 12,
		BLOCKWIDTH = 6,
		BLOCKMARGIN = 2,
		MAXLEVELS = 4		// We need #3 when converting Caviar proj files
	};

	CSushiBar();
	~CSushiBar();

	void SetPrompt(const char* szMessage, BOOL bScrolling = FALSE);
	void SetPromptColor(COLORREF textColor = 0, COLORREF bkColor = 0);
	void SetIdlePrompt(const char* szMessage);
	void SetPromptUntilIdle(const char* szMessage);
	void OnIdle();

	afx_msg LRESULT	OnSetText(WPARAM, LPARAM lParam);
	afx_msg void  	OnSysColorChange();
	afx_msg int 	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnTimer(UINT nIDEvent);

	void BeginPercentDone(const char* szPrompt);
	void PercentDone(int percent);
	void EndPercentDone();

	BOOL SetIndicators(const UINT FAR* lpIDArray = NULL, int nIDCount = 0,
	                   const STATINFO FAR* pStatInfo = NULL, int nCount = 0);
	BOOL SetIndicators(const CSushiBarInfo* pSushiBarInfo);
	void ResetIndicators();

	virtual void DoPaint(CDC* pDC);
	HBITMAP LoadBitmap(int nGroupUI, int nBitmap);
	void DrawStatusBitmap(HDC hDC, CRect rect, HBITMAP hBitmap, LPCSTR szText, UINT nStyle);
	void DrawStatusText(HDC hDC, CRect rect, LPCSTR lpszText, UINT nStyle, UINT textWidth, UINT extraSpace = 0);
	void DrawStatusPercentDone(HDC hDC, CRect rect, LPCSTR lpszText, int percent, LPCSTR lpszExtraText);
	void DrawStatusHelper(HDC hdcDest, CRect rect, UINT nStyle);

	void DrawIndicatorNow(UINT nIDIndicator, BOOL bBitmapToo = FALSE);
	
	void Reset();
	void SetPosition(const CPoint& pos);
	void SetSize(const CSize& size);
	void SetZoom(int zoom);
	void SetLineColumn(int line, int col, BOOL fForce = FALSE);
	void SetOffsetExtent(long lOffset, long lExtent);
	void SetOverStrike(BOOL bOverStrike);
	void GetText(UINT nID, CString& str);

	BOOL GetOverStrike()
		{ return m_bOverStrike; }

	BOOL IsIndicator(UINT nID);
	BOOL IsSetUntilIdle(void) { return m_bLockedUntilIdle; }

	static CString m_strAfxIdleMessage;

private:
	int	m_rgPercentDone[MAXLEVELS+1];
	int m_iPercentDone;
	CString	m_strPercentDonePrompt;
	CRect	m_rectCounter;

	CStatusClock m_clock;
	HBITMAP m_hbitmapArray [MAXPANES+1];
	const UINT FAR* m_pCurrentIndicators;
	CPoint	m_position;
	CSize	m_size;
	int 	m_zoom;
	int 	m_line;
	int 	m_column;
	long	m_offset;
	long	m_extent;
	BOOL	m_bOverStrike;
	COLORREF	m_textColor;
	COLORREF	m_bkColor;
	BOOL		m_bStandardColors;
	BOOL		m_bScrollingPrompt;

	CString m_strIdlePrompt;
	BOOL	m_bLockedUntilIdle;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // __SBAR_H__

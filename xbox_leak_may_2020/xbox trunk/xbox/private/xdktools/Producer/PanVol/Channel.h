// Channel.h: interface for the CChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANNEL_H__0D0FEB41_960E_11D0_8C10_00A0C92E1CAC__INCLUDED_)
#define AFX_CHANNEL_H__0D0FEB41_960E_11D0_8C10_00A0C92E1CAC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BandEditor.h"

class CGridCtrl;

class CChannel
{
public:
	CChannel(CGridCtrl* pParent);

	// Returns falls on failure to get GDI resources
	void CChannel::Create(IDMUSProdBandPChannel* pInstrument);

	virtual ~CChannel();

	CChannel    *m_pNext;

private:
	int			m_nId;
	int			m_nPan;
	int			m_nVol;

	bool		m_bSelected;
	bool		m_bEnabled;		

	bool		m_bDrums;
	CRect       m_rcBtnArea;
	CGridCtrl*	m_pParent;

public:
	
	IDMUSProdBandPChannel*	m_pInstrument;

	const short  m_cxSize;
	const short  m_cySize;
	BOOL         m_fDepressed;
	BOOL         m_fTop;
	CPoint		 m_ptTouch;
	bool		 m_bDragButton;
	int			 m_nDraggedPan;
	int			 m_nDraggedVol;


	void Draw( CDC *pdc, const CRect& rcInvalid );
	void DrawButton(CDC* pDC, const CRect& rcInvalid, CBitmap* pBitmap, bool bDown, bool bSelected);
	void DrawDrums(CDC* pDC, bool bDown, bool bSelected);
	void DrawNumber(CDC* pDC, bool bDown);

	CPoint	GetMiddle();
	void	SetSelected(bool bSelection);
	void	SetEnabled(bool bEnable);
	bool	IsSelected();
	bool	IsEnabled();
	void	Move( CRect *prcExtent, CPoint *pptNewLoc = NULL );

	void	SetPanVol(short nPan, short nVol);
	void	Refresh();

	// Inline methods
	BOOL	HitTest( CPoint &point );
	void	GetButtonArea( CRect *prc );
	long	GetId();  
	short	GetPan();  
	short	GetVol();  
};

#endif // !defined(AFX_CHANNEL_H__0D0FEB41_960E_11D0_8C10_00A0C92E1CAC__INCLUDED_)

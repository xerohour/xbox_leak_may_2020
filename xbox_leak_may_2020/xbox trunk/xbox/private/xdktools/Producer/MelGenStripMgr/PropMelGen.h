#ifndef __PROPMELGEN_H_
#define __PROPMELGEN_H_

#include <DMUSProd.h>
#include <dmusici.h>
#include <dmusicp.h>

class CMelGenItem;

class CPropMelGen 
{
public:
	CPropMelGen();
	CPropMelGen( const CMelGenItem *pMelGenItem );
	~CPropMelGen();
	void FormatTextForStrip( CString& strText );

    DWORD					m_dwMeasure;	// What measure this melody fragment falls on
    BYTE					m_bBeat;		// What beat this melody fragment falls on
	DMUS_MELODY_FRAGMENT*	m_pRepeat;
	DWORD					m_dwBits;		// Various bits
	DMUS_MELODY_FRAGMENT	m_MelGen;		// Melody fragment
};

#define UD_DRAGSELECT		0x0001
#define UD_CURRENTSELECTION 0x0002
#define UD_FAKE				0x0004
#define UD_FROMPASTE		0x0008

#endif // __PROPMELGEN_H_

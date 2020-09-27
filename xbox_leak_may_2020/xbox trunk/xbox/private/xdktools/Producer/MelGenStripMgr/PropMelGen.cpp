#include "stdafx.h"
#include "PropMelGen.h"
#include "MelGenIO.h"

CPropMelGen::CPropMelGen()
{
	m_dwMeasure = 0;
	m_bBeat = 0;
	m_dwBits = 0;
	m_pRepeat = NULL;

	m_MelGen.mtTime = 0;
	m_MelGen.dwID = 0;
	wcscpy(m_MelGen.wszVariationLabel, L"<unused>");
	m_MelGen.dwVariationFlags = 0;
	m_MelGen.dwRepeatFragmentID = 0;
	m_MelGen.dwFragmentFlags = DMUS_FRAGMENTF_USE_PLAYMODE;
	m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_NONE;
	// new...
	m_MelGen.dwTransposeIntervals = 0;
	ZeroMemory(&m_MelGen.Command, sizeof(m_MelGen.Command));
	ZeroMemory(&m_MelGen.ConnectionArc, sizeof(m_MelGen.ConnectionArc));
}

CPropMelGen::CPropMelGen(const CMelGenItem *pMelGenItem)
{
	ASSERT( pMelGenItem != NULL );

	m_dwMeasure = pMelGenItem->m_dwMeasure;
	m_bBeat = pMelGenItem->m_bBeat;
	m_dwBits = pMelGenItem->m_dwBits;
	m_pRepeat = pMelGenItem->m_pRepeat;

	m_MelGen.mtTime = pMelGenItem->m_MelGen.mtTime;
	m_MelGen.dwID = pMelGenItem->m_MelGen.dwID;
	wcscpy(m_MelGen.wszVariationLabel, pMelGenItem->m_MelGen.wszVariationLabel);
	m_MelGen.dwVariationFlags = pMelGenItem->m_MelGen.dwVariationFlags;
	m_MelGen.dwRepeatFragmentID = pMelGenItem->m_MelGen.dwRepeatFragmentID;
	m_MelGen.dwFragmentFlags = pMelGenItem->m_MelGen.dwFragmentFlags;
	m_MelGen.dwPlayModeFlags = pMelGenItem->m_MelGen.dwPlayModeFlags;
	m_MelGen.dwTransposeIntervals = pMelGenItem->m_MelGen.dwTransposeIntervals;
	m_MelGen.Command = pMelGenItem->m_MelGen.Command;
	m_MelGen.ConnectionArc = pMelGenItem->m_MelGen.ConnectionArc;
}

CPropMelGen::~CPropMelGen( ) 
{ 
}

void CPropMelGen::FormatTextForStrip( CString& strText )
{
	strText.Empty();

	if( !(m_dwBits & UD_FAKE) )
	{
		if( m_pRepeat
		&&  m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT )
		{
			if( m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_REJECT_REPEAT )
			{
				strText.Format( "!%d", m_MelGen.dwRepeatFragmentID );
			}
			else
			{
				strText.Format( "*%d", m_MelGen.dwRepeatFragmentID );
			}
		}
		else
		{
			strText.Format( "%d", m_MelGen.dwID );
		}
	}
}

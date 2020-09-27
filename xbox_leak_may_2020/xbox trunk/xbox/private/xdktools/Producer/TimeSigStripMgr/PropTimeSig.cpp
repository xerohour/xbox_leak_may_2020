#include "stdafx.h"
#include "PropTimeSig.h"
#include "TimeSigIO.h"

CPropTimeSig::CPropTimeSig()
{
	m_dwMeasure = 0;
	m_dwBits = 0;

	m_TimeSignature.mtTime = 0;
	m_TimeSignature.bBeatsPerMeasure = 4;
	m_TimeSignature.bBeat = 4;
	m_TimeSignature.wGridsPerBeat = 4;
}

CPropTimeSig::CPropTimeSig(const CTimeSigItem *pTimeSigItem)
{
	ASSERT( pTimeSigItem != NULL );

	m_dwMeasure = pTimeSigItem->m_dwMeasure;
	m_dwBits = pTimeSigItem->m_dwBits;

	m_TimeSignature.bBeatsPerMeasure = pTimeSigItem->m_TimeSignature.bBeatsPerMeasure;
	m_TimeSignature.bBeat = pTimeSigItem->m_TimeSignature.bBeat;
	m_TimeSignature.wGridsPerBeat = pTimeSigItem->m_TimeSignature.wGridsPerBeat;
}

CPropTimeSig::~CPropTimeSig( ) 
{ 
}

const TCHAR *CPropTimeSig::GetString()
{
	if( m_dwBits & UD_FAKE )
	{
		return _T("");
	}

	TCHAR tcsTmp2[10];

	_itot( m_TimeSignature.bBeatsPerMeasure, m_tcsText, 10 );

	_tcscpy( tcsTmp2, _T(" / ") );
	_tcscat( m_tcsText, tcsTmp2 );

	_itot( m_TimeSignature.bBeat, tcsTmp2, 10 );
	_tcscat( m_tcsText, tcsTmp2 );

	return m_tcsText;
}

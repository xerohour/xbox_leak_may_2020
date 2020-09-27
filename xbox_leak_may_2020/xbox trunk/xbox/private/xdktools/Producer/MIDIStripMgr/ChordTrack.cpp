// ChordTrack.cpp : implementation file
//

#include "stdafx.h"

#include "ChordTrack.h"
#include "StripMgr.h"
#include "MIDIStripMgrApp.h"

CChordTrack::CChordTrack()
{
	m_pIStripMgr = NULL;

	m_dwGroupBits = 1;
	m_dwPosition = 0;
	m_ckid = 0;
	m_fccType = 0;
}

CChordTrack::~CChordTrack()
{
	RELEASE( m_pIStripMgr );
}

void CChordTrack::SetStripMgr( IDMUSProdStripMgr* pIStripMgr )
{
	RELEASE( m_pIStripMgr );

	if( pIStripMgr )
	{
		m_pIStripMgr = pIStripMgr;
		m_pIStripMgr->AddRef();
	}
}

void CChordTrack::GetStripMgr( IDMUSProdStripMgr** ppIStripMgr )
{
	if( ppIStripMgr == NULL )
	{
		return;
	}

	*ppIStripMgr = m_pIStripMgr;
	if( m_pIStripMgr )
	{
		(*ppIStripMgr)->AddRef();
	}
}

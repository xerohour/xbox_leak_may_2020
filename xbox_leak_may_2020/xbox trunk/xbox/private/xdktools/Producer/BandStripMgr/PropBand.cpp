#include "stdafx.h"
#include "PropBand.h"
#include "BandIO.h"

CPropBand::CPropBand()
{
	m_mtTimePhysical = 0;
	m_mtTimeLogical = 0;
	m_dwMeasure = 0;
	m_bBeat = 0;
	m_lTick = 0;
	m_lLogicalMeasure = 0;
	m_lLogicalBeat = 0;
	m_strText.Empty();
	m_dwBits = 0;
	m_wFlags = 0;
	m_pIBandNode = NULL;
}


CPropBand::CPropBand(const CBandItem *pBandItem)
{
	m_mtTimePhysical = pBandItem->m_mtTimePhysical;
	m_mtTimeLogical = pBandItem->m_mtTimeLogical;
	m_dwMeasure = pBandItem->m_dwMeasure;
	m_bBeat = pBandItem->m_bBeat;
	m_lTick = pBandItem->m_lTick;
	m_lLogicalMeasure = pBandItem->m_lLogicalMeasure;
	m_lLogicalBeat = pBandItem->m_lLogicalBeat;
	m_strText = pBandItem->m_strText;
	m_dwBits = 0;
	m_wFlags = 0;

	m_pIBandNode = pBandItem->m_pIBandNode;
	if( m_pIBandNode )
	{
		m_pIBandNode->AddRef();
	}
}

CPropBand::~CPropBand()
{
	if( m_pIBandNode )
	{
		m_pIBandNode->Release();
	}
}

void CPropBand::ApplyToBandItem( CBandItem *pBandItem ) const
{
	ASSERT( pBandItem != NULL );
	if( pBandItem == NULL )
	{
		return;
	}

	pBandItem->m_mtTimePhysical = m_mtTimePhysical;
	pBandItem->m_mtTimeLogical = m_mtTimeLogical;
	pBandItem->m_dwMeasure = m_dwMeasure;
	pBandItem->m_bBeat = m_bBeat;
	pBandItem->m_lTick = m_lTick;
	pBandItem->m_lLogicalMeasure = m_lLogicalMeasure;
	pBandItem->m_lLogicalBeat = m_lLogicalBeat;
	pBandItem->m_strText = m_strText;

	if( pBandItem->m_pIBandNode )
	{
		pBandItem->m_pIBandNode->Release();
	}
	pBandItem->m_pIBandNode = m_pIBandNode;
	if( pBandItem->m_pIBandNode )
	{
		pBandItem->m_pIBandNode->AddRef();
	}
}

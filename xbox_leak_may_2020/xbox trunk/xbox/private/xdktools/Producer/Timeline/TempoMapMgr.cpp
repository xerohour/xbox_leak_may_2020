// TempoMapMgr.cpp : Implementation of CTempoMapMgr
#include "stdafx.h"
#include "Timeline.h"
#include "TempoMapMgr.h"
#include <dmusici.h>

/////////////////////////////////////////////////////////////////////////////
// CTempoMapMgr

HRESULT CTempoMapMgr::ClocksToRefTime(
				/*[in]*/  long		time,
				/*[out]*/ REFERENCE_TIME	*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (NULL == pRefTime)
	{
		return E_POINTER;
	}
	*pRefTime = time * REFCLOCKS_PER_MINUTE;
	*pRefTime /= m_lTempo * DMUS_PPQ;
	return S_OK;
}

HRESULT CTempoMapMgr::MeasureBeatToRefTime(
				/*[in]*/  DWORD		dwGroupBits,
				/*[in]*/  DWORD		dwIndex,
				/*[in]*/  long		iMeasure,
				/*[in]*/  long		iBeat,
				/*[out]*/ REFERENCE_TIME	*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (NULL == pRefTime)
	{
		return E_POINTER;
	}
	if (NULL == m_pTimelineCtl)
	{
		return E_FAIL;
	}

	long lClocks;
	m_pTimelineCtl->MeasureBeatToClocks( dwGroupBits, dwIndex, iMeasure, iBeat, &lClocks );
	ClocksToRefTime( lClocks, pRefTime);
	return S_OK;
}

HRESULT CTempoMapMgr::RefTimeToClocks(
				/*[in]*/  REFERENCE_TIME	RefTime,
				/*[out]*/ long		*pTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (NULL == pTime)
	{
		return E_POINTER;
	}
	REFERENCE_TIME rTmp = RefTime * m_lTempo * DMUS_PPQ;
	rTmp /= REFCLOCKS_PER_MINUTE;
	*pTime = (long) rTmp;
	return S_OK;
}							 

HRESULT CTempoMapMgr::RefTimeToMeasureBeat(
				/*[in]*/  DWORD		dwGroupBits,
				/*[in]*/  DWORD		dwIndex,
				/*[in]*/  REFERENCE_TIME	RefTime,
				/*[out]*/ long		*piMeasure,
				/*[out]*/ long		*piBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if ((NULL == piMeasure)||(NULL == piBeat))
	{
		return E_POINTER;
	}
	if (NULL == m_pTimelineCtl)
	{
		return E_FAIL;
	}

	long lClocks;
	RefTimeToClocks( RefTime, &lClocks);
	m_pTimelineCtl->ClocksToMeasureBeat( dwGroupBits, dwIndex, lClocks, piMeasure, piBeat);
	return S_OK;
}

HRESULT CTempoMapMgr::RefTimeToTempo(
				/*[in]*/  REFERENCE_TIME	RefTime,
				/*[out]*/ long		*plTempo)
{
	UNREFERENCED_PARAMETER( RefTime );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (NULL == plTempo)
	{
		return E_POINTER;
	}
	*plTempo = m_lTempo;
	return S_OK;
}

HRESULT CTempoMapMgr::ClocksToTempo(
				/*[in]*/  long		time,
				/*[out]*/ long		*plTempo)
{
	UNREFERENCED_PARAMETER( time );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (NULL == plTempo)
	{
		return E_POINTER;
	}
	*plTempo = m_lTempo;
	return S_OK;
}

HRESULT CTempoMapMgr::MeasureBeatToTempo(
				/*[in]*/  DWORD		dwGroupBits,
				/*[in]*/  DWORD		dwIndex,
				/*[in]*/  long		iMeasure,
				/*[in]*/  long		iBeat,
				/*[out]*/ long		*plTempo)
{
	UNREFERENCED_PARAMETER( dwGroupBits );
	UNREFERENCED_PARAMETER( dwIndex );
	UNREFERENCED_PARAMETER( iMeasure );
	UNREFERENCED_PARAMETER( iBeat );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (NULL == plTempo)
	{
		return E_POINTER;
	}
	*plTempo = m_lTempo;
	return S_OK;
}

HRESULT CTempoMapMgr::SetTempo(
				/*[in]*/  long		lTempo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_lTempo = lTempo;
	return S_OK;
}

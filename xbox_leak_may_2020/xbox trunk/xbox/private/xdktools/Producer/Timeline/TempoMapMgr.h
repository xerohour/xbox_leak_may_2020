// TempoMapMgr.h : Declaration of the CTempoMapMgr

#ifndef __TEMPOMAPMGR_H_
#define __TEMPOMAPMGR_H_

#ifndef REFCLOCKS_PER_MINUTE
#define REFCLOCKS_PER_MINUTE 600000000
#endif

/////////////////////////////////////////////////////////////////////////////
// CTempoMapMgr
class ATL_NO_VTABLE CTempoMapMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTempoMapMgr, &CLSID_TempoMapMgr>,
	public IDMUSProdTempoMapMgr
{
public:
	CTempoMapMgr()
	{
		m_lTempo = 120;
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_TIMESTRIPMGR)

BEGIN_COM_MAP(CTempoMapMgr)
	COM_INTERFACE_ENTRY(IDMUSProdTempoMapMgr)
END_COM_MAP()


	STDMETHODIMP ClocksToRefTime(
		/*[in]*/  long		time,
		/*[out]*/ REFERENCE_TIME		*pRefTime
		);

	STDMETHODIMP MeasureBeatToRefTime(
		/*[in]*/  DWORD		dwGroupBits,
		/*[in]*/  DWORD		dwIndex,
		/*[in]*/  long		iMeasure,
		/*[in]*/  long		iBeat,
		/*[out]*/ REFERENCE_TIME	*pRefTime
		);

	STDMETHODIMP RefTimeToClocks(
		/*[in]*/  REFERENCE_TIME	RefTime,
		/*[out]*/ long		*pTime
		);
	
	STDMETHODIMP RefTimeToMeasureBeat(
		/*[in]*/  DWORD		dwGroupBits,
		/*[in]*/  DWORD		dwIndex,
		/*[in]*/  REFERENCE_TIME	RefTime,
		/*[out]*/ long		*piMeasure,
		/*[out]*/ long		*piBeat
		);
	
	STDMETHODIMP RefTimeToTempo(
		/*[in]*/  REFERENCE_TIME	RefTime,
		/*[out]*/ long		*piTempo
		);
	
	STDMETHODIMP ClocksToTempo(
		/*[in]*/  long		time,
		/*[out]*/ long		*piTempo
		);
	
	STDMETHODIMP MeasureBeatToTempo(
		/*[in]*/  DWORD		dwGroupBits,
		/*[in]*/  DWORD		dwIndex,
		/*[in]*/  long		iMeasure,
		/*[in]*/  long		iBeat,
		/*[out]*/ long		*piTempo
		);
	
	STDMETHODIMP SetTempo(
		/*[in]*/  long		iTempo
		);

private:
	long				m_lTempo;	// Tempo in Beats per Minute
	IDMUSProdTimeline*	m_pTimelineCtl;
};

#endif //__TEMPOMAP_H_

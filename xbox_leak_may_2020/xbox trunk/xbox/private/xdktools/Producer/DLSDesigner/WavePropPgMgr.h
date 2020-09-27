//////////////////////////////////////////////////////////////////////
//
// WavePropPgMgr.h
//
//////////////////////////////////////////////////////////////////////

#ifndef WAVEPROPPGMGR_H
#define WAVEPROPPGMGR_H

#include "DllBasePropPageManager.h"

class CWavePropPg;
class CWaveInfoPropPg;
class CWaveCompressionPropPage;

#define WAVE_PROP_PAGE			0
#define WAVE_INFO_PAGE			1
#define WAVE_COMPRESSION_PAGE	2

//////////////////////////////////////////////////////////////////////
//  CWavePropPgMgr

class CWavePropPgMgr : public CDllBasePropPageManager 
{
    friend class CWaveCtrl;
	friend class CWave;

public:
	CWavePropPgMgr();
	virtual ~CWavePropPgMgr();

    // IJazzPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );

    HRESULT STDMETHODCALLTYPE RefreshData();
	//additional metheods
public:
	void ShowPropSheet();
	// Member variables

public:
	static DWORD dwLastSelPage;

private:
	CWavePropPg*				m_pWavePage;
    CWaveInfoPropPg*			m_pWaveInfoPage;
	CWaveCompressionPropPage*	m_pWaveCompressionPage;
};

#endif // #ifndef WAVEPROPPGMGR_H
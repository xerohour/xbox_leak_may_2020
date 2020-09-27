// Copyright (c) 1998 Microsoft Corporation
// IDMSynth.h : Declaration of the CDMSynth

#ifndef __DMSYNTH_H_
#define __DMSYNTH_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDMSynth
class ATL_NO_VTABLE CDMSynth : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDMSynth, &CLSID_DMSynth>,
	public IDirectMusicSynth
{
public:
	CDMSynth()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DMSYNTH)

BEGIN_COM_MAP(CDMSynth)
	COM_INTERFACE_ENTRY(IDirectMusicSynth)
END_COM_MAP()

// IDirectMusicSynth
public:
	STDMETHOD(UnloadInstrument)(DWORD dwInstrumentID);
	STDMETHOD(QueryCompactTime)(DWORD* pdwTime);
	STDMETHOD(Compact)();
	STDMETHOD(DownloadInstrument)(DWORD dwInstrumentID, DWORD * pDMInstrument);
	STDMETHOD(UnloadWave)(DWORD dmWaveID);
	STDMETHOD(DownloadWave)(DWORD dmwaveid, DWORD * pdmwave);
};

#endif //__DMSYNTH_H_

//lint -e1510
#pragma once

#include <globals.h>

class CTestTool : public IDirectMusicTool
{
public:

	CTestTool( void );
	~CTestTool( void );

    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, LPVOID* ppVoid );
    ULONG STDMETHODCALLTYPE AddRef( void );
    ULONG STDMETHODCALLTYPE Release( void );

    /*  IDirectMusicTool */
    HRESULT STDMETHODCALLTYPE Init( IDirectMusicGraph* pGraph );
    HRESULT STDMETHODCALLTYPE GetMsgDeliveryType(LPDWORD pdwDeliveryType );
    HRESULT STDMETHODCALLTYPE GetMediaTypeArraySize( LPDWORD pdwNumElements );
    HRESULT STDMETHODCALLTYPE GetMediaTypes( LPDWORD* padwMediaTypes, DWORD dwNumElements );
    HRESULT STDMETHODCALLTYPE ProcessPMsg( IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG );
    HRESULT STDMETHODCALLTYPE Flush( IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG, REFERENCE_TIME rtTime );

private:
	ULONG m_ulRefCount;


};

HRESULT CreateTestTool( IDirectMusicTool** ppTool );
//lint +e1510

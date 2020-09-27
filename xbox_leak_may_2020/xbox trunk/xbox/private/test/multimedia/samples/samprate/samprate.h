#pragma once

static const ULONG MIN_INPUT = 2;

HRESULT XSrcCreateMediaObject( XMediaObject **ppMediaObject, PWAVEFORMATEX pWaveFormat );

class CSrcXMO : public XMediaObject {

public:

	CSrcXMO( void )
	{
		m_cRef = 0;
	}

	~CSrcXMO( void )
	{
		delete m_pWaveFormat;
	}


    STDMETHODIMP_(ULONG) AddRef( void ) 
	{
       return InterlockedIncrement( (long*) &m_cRef );
    }

    STDMETHODIMP_(ULONG) Release( void ) 
	{
       long l = InterlockedDecrement( (long*) &m_cRef );

       if ( l == 0 )
          delete this;

       return l;
    }
    
    HRESULT STDMETHODCALLTYPE Flush( void )
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Discontinuity( void )
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetStatus( LPDWORD pdwStatus )
    {
        if ( pdwStatus ) 
		{
            *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;
        }

        return S_OK;
    }

    HRESULT Initialize( PWAVEFORMATEX pWaveFormat );

	HRESULT STDMETHODCALLTYPE GetInfo( LPXMEDIAINFO pXMediaInfo );
    HRESULT STDMETHODCALLTYPE Process( LPCXMEDIAPACKET pSrcBuffer, LPCXMEDIAPACKET pDstBuffer );

protected:

	BOOL IsValidPcmFormat( LPCWAVEFORMATEX pwfx );

protected:

    ULONG m_cRef;
	PWAVEFORMATEX m_pWaveFormat;
};
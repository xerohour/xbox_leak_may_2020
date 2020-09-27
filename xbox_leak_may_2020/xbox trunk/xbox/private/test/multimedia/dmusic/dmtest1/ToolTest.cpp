#include <globals.h>

CTestTool::CTestTool( void )
	: m_ulRefCount( 1 )
{
}

CTestTool::~CTestTool( void )
{
}

HRESULT STDMETHODCALLTYPE CTestTool::QueryInterface( REFIID iid, LPVOID* ppVoid )
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE CTestTool::AddRef( void )
{
	return ++m_ulRefCount;
}

ULONG STDMETHODCALLTYPE CTestTool::Release( void )
{
	if ( 0 == --m_ulRefCount )
	{
		delete this;
	}
	
	return m_ulRefCount;
}

HRESULT STDMETHODCALLTYPE CTestTool::Init( IDirectMusicGraph* pGraph )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTestTool::GetMsgDeliveryType(LPDWORD pdwDeliveryType )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTestTool::GetMediaTypeArraySize( LPDWORD pdwNumElements )
{
	*pdwNumElements = 0;
    return S_OK;
}


HRESULT STDMETHODCALLTYPE CTestTool::GetMediaTypes( LPDWORD* padwMediaTypes, DWORD dwNumElements )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTestTool::ProcessPMsg( IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG )
{
	Log(FYILOGLEVEL, "Got PMSG!\n" );
	return S_OK;
//	return DMUS_S_REQUEUE;
}

HRESULT STDMETHODCALLTYPE CTestTool::Flush( IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG, REFERENCE_TIME rtTime )
{
	return S_OK;
}

HRESULT CreateTestTool( IDirectMusicTool** ppTool )
{

	if ( NULL == ppTool )
		return E_POINTER;

	HRESULT hr = S_OK;
	IDirectMusicTool* pTool = NULL;

	pTool = new CTestTool();

	if ( NULL == pTool )
	{
		hr = E_OUTOFMEMORY;
	}

	if ( SUCCEEDED( hr ) )
	{
		*ppTool = pTool;
		pTool->AddRef();
	}

	return hr;
}
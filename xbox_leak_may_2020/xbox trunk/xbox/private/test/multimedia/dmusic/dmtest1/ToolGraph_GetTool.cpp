#include <globals.h>

HRESULT ToolGraph_GetTool( CtIDirectMusicPerformance8* ptPerf8, LPDWORD pdwPChannels, DWORD cPChannels, LONG lIndex, DWORD dwNum )
{
	HRESULT hr = S_OK;
	IDirectMusicGraph* pGraph = NULL;
	IDirectMusicTool** ppTools = NULL;

	IDirectMusicTool* pTool = NULL;

	ppTools = new IDirectMusicTool*[dwNum];

	if ( NULL == ppTools )
		return E_OUTOFMEMORY;

	CHECKRUN( dmthCreateToolGraph( ptPerf8, &pGraph ) );
	CHECKALLOC( pGraph );

	for ( DWORD i = 0; i < dwNum && SUCCEEDED( hr ); i++ )
	{
		CHECKRUN( CreateTestTool( ppTools + i ) );
		CHECKALLOC( ppTools + i );

		CHECKRUN( pGraph->InsertTool( ppTools[i], pdwPChannels, cPChannels, lIndex ) );
	}

	for (i = 0; i < dwNum && SUCCEEDED( hr ); i++ )
	{
		CHECKRUN( pGraph->GetTool( 0, &pTool ) );
		CHECKALLOC( pTool );
	}

	for (i = 0; i < dwNum; i++ )
	{
		SAFE_RELEASE( ppTools[i] );
	}

	delete [] ppTools;

	RELEASE( pTool );
	RELEASE( pGraph );

	return hr;
}




HRESULT ToolGraph_GetTool_BVT( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

//	return hr;
	return E_NOTIMPL;
}



HRESULT ToolGraph_GetTool_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	DMTEST_EXECUTE( ToolGraph_GetTool( ptPerf8, NULL, 0, 0, 1 ) );
	DMTEST_EXECUTE( ToolGraph_GetTool( ptPerf8, NULL, 0, 1, 1 ) );
	DMTEST_EXECUTE( ToolGraph_GetTool( ptPerf8, NULL, 0, -1,1 ) );

	DMTEST_EXECUTE( ToolGraph_GetTool( ptPerf8, NULL, 0, 0, 10 ) );
	DMTEST_EXECUTE( ToolGraph_GetTool( ptPerf8, NULL, 0, 1, 10 ) );
	DMTEST_EXECUTE( ToolGraph_GetTool( ptPerf8, NULL, 0, -1,10 ) );

	return hr;
}


#include "script.h"

HRESULT dmthCreateScript( LPSTR szScriptName, CtIDirectMusicLoader8* pLoader, CtIDirectMusicScript** ppScript )
{
	if ( NULL == ppScript || NULL == szScriptName )
		return E_POINTER;

	HRESULT hr = S_OK;

	CHECKRUN( pLoader->LoadObjectFromFile( CLSID_DirectMusicScript, CTIID_IDirectMusicScript, szScriptName, (LPVOID*) ppScript ) );

	return hr;
}

HRESULT dmthInitScriptErrorInfo( DMUS_SCRIPT_ERRORINFO* pInfo )
{
	if ( NULL == pInfo )
		return S_OK;

	HRESULT hr = S_OK;

	ZeroMemory( pInfo, sizeof( DMUS_SCRIPT_ERRORINFO ) );
	pInfo->dwSize = sizeof( DMUS_SCRIPT_ERRORINFO );

	return hr;
}

void dmthVerifyScriptError( DMUS_SCRIPT_ERRORINFO* pInfo )
{
	if ( NULL == pInfo )
		return;

	if ( FAILED( pInfo->hr ) )
	{
		DbgPrint( "DMUS_SCRIPT_ERROR_INFO:\n" );
		DbgPrint( "   hr = 0x%x\n", pInfo->hr );
		DbgPrint( "   Line Number = %u\n", pInfo->ulLineNumber );
		DbgPrint( "   Char Position = %i\n", pInfo->ichCharPosition );
		DbgPrint( "   Source file = %S\n", pInfo->wszSourceFile );
		DbgPrint( "   Source component = %S\n", pInfo->wszSourceComponent );
		DbgPrint( "   Description = %S\n", pInfo->wszDescription );
		DbgPrint( "   Text = %S\n", pInfo->wszSourceLineText );
	}
}
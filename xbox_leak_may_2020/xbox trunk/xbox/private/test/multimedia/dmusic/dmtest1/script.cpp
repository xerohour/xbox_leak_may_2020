#include "globals.h"
//#include "script.h"

HRESULT dmthCreateScript( LPCSTR szScriptName, CtIDirectMusicLoader8* pLoader, CtIDirectMusicScript** ppScript )
{
	CHAR szPath[MAX_PATH] = {0};
    if ( NULL == ppScript || NULL == szScriptName )
		return E_POINTER;

	HRESULT hr = S_OK;

	CHECKRUN( ChopPath(szScriptName, szPath, NULL));
    CHECKRUN( pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, FALSE));
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
		Log(FYILOGLEVEL, "DMUS_SCRIPT_ERROR_INFO:" );
		Log(FYILOGLEVEL, "   hr = 0x%x", pInfo->hr );
		Log(FYILOGLEVEL, "   Line Number = %u", pInfo->ulLineNumber );
		Log(FYILOGLEVEL, "   Char Position = %i", pInfo->ichCharPosition );
		Log(FYILOGLEVEL, "   Source file = %S", pInfo->wszSourceFile );
		Log(FYILOGLEVEL, "   Source component = %S", pInfo->wszSourceComponent );
		Log(FYILOGLEVEL, "   Description = %S", pInfo->wszDescription );
		Log(FYILOGLEVEL, "   Text = %S", pInfo->wszSourceLineText );
	}
}
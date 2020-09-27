/********************************************************************************
	FILE:
		Loader_CollectGarbage.cpp

	PURPOSE:
		CollectGarbage Tests for Loader

	BY:
		DANROSE
********************************************************************************/

#include "globals.h"
#include <loader8.h>

static const LPSTR MEDIA_ROOT = "T:\\DMTest1";

static GUID* guids[] = {
	(GUID*) &CLSID_DirectMusicSegment,
//	(GUID*) &CLSID_DirectMusicBand,
	(GUID*) &CLSID_DirectMusicScript,
	(GUID*) &CLSID_DirectMusicAudioPathConfig
};

static GUID* interfaces[] = { 
	(GUID*) &CTIID_IDirectMusicSegment,
//	(GUID*) &CTIID_IDirectMusicBand,
	(GUID*) &CTIID_IDirectMusicScript,
	(GUID*) &CTIID_IUnknown 
};

struct ListNode {
	ListNode* pNext;
	CtIUnknown* pUnk;
};

static ListNode* g_pList = NULL;

HRESULT Insert( CtIUnknown* pUnk )
{
    HRESULT hr = S_OK;
	ListNode* pTemp = new ListNode;
    CHECKALLOC(pTemp);
    if (SUCCEEDED(hr))
    {
	    pTemp->pUnk = pUnk;
	    pTemp->pNext = g_pList;
	    g_pList = pTemp;
    }

    return hr;
}

void Delete()
{
	for( ListNode* pNode = g_pList; pNode != NULL; pNode = g_pList )
	{
		g_pList = pNode->pNext;
		RELEASE( pNode->pUnk );
		delete pNode;
		pNode = NULL;
	}
}

HRESULT dmthLoadFile( LPSTR szFileName, CtIDirectMusicLoader8* pLoader, CtIUnknown** ppUnk )
{
	if ( NULL == szFileName || NULL == pLoader || NULL == ppUnk )
		return E_POINTER;

    CHAR szPath[MAX_PATH] = {NULL};
    CHAR szName[MAX_PATH] = {NULL};


	HRESULT hr = S_OK;

    CHECKRUN(ChopPath(szFileName, szPath, szName));
    CHECKRUN(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes ,szPath, FALSE));
    if (FAILED(hr))
        return hr;
    

	for ( ULONG i = 0; i < NUMELEMS( interfaces ); i++ )
	{

		CHECKRUN( pLoader->LoadObjectFromFile( *guids[i], *interfaces[i], szFileName, (LPVOID*) ppUnk ) );
	
		if ( SUCCEEDED( hr ) )
		{
			break;
		}
        else
        {
			hr = S_OK;
		}
	}

	return hr;
}

HRESULT LoadOneFile( LPSTR szFile, CtIDirectMusicLoader8* pLoader )
{
	HRESULT hr = S_OK;

	CtIUnknown* pUnk = NULL;

	CHECKRUN( dmthLoadFile( szFile, pLoader, &pUnk ) );

	if ( pUnk && SUCCEEDED( hr ) )
	{
		CHECKRUN(Insert( pUnk ));

		Log(FYILOGLEVEL, "********************* %s\n", szFile );
	}

	RELEASE( pUnk );

	return hr;
}

HRESULT RecurseDirectory( LPSTR szDir, CtIDirectMusicLoader8* pLoader  )
{
	HRESULT hr = S_OK;

	WIN32_FIND_DATA findData;
	BOOL bMore = TRUE;
	HANDLE hFiles;

	CHAR szFullPath[MAX_PATH];
	CHAR szFiles[MAX_PATH];

	sprintf( szFiles, "%s\\*", szDir ); 

	hFiles = FindFirstFile( szFiles, &findData );

	while ( INVALID_HANDLE_VALUE != hFiles && TRUE == bMore && SUCCEEDED( hr ) )
	{
		sprintf( szFullPath, "%s\\%s", szDir, findData.cFileName );

		if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			CHECKRUN( RecurseDirectory( szFullPath, pLoader ) );
		}

		else
		{
			CHECKRUN( LoadOneFile( szFullPath, pLoader ) );
		}

		bMore = FindNextFile( hFiles, &findData );
	}

	FindClose( hFiles );

	return hr;
}

HRESULT Loader_CollectGarbage_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	CtIDirectMusicLoader8* pLoader = NULL;

	CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &pLoader ) );
	CHECKALLOC( pLoader );

	CHECKRUN( RecurseDirectory( MEDIA_ROOT, pLoader ) );
//	CHECKRUN( LoadOneFile( "T:\\DMTest1\\old\\SonicImplants Live Band.sgt", pLoader ) );

	for ( ListNode* pNode = g_pList; pNode != NULL && SUCCEEDED( hr ); pNode = pNode->pNext )
	{
		CHECKRUN( pLoader->ReleaseObjectByUnknown( pNode->pUnk ) );
	}

	CHECK(pLoader->CollectGarbage());

	Delete();

	RELEASE( pLoader );

	return hr;
}
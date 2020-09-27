#include <globals.h>



HRESULT ToolGraph_StampPMsg_BVT( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;

//return hr;
return E_NOTIMPL;
};


HRESULT ToolGraph_StampPMsg_Valid( CtIDirectMusicPerformance8* ptPerf8 , DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;

	IDirectMusicGraph* pGraph = NULL;
	IDirectMusicTool* pTool = NULL;

    CICMusic Music;

    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));


	CHECKRUN( dmthCreateToolGraph( ptPerf8, &pGraph ) );
	CHECKALLOC( pGraph );

    pTool = new CEchoTool;

//	CHECKRUN( CreateTestTool( &pTool ) );
	CHECKALLOC( pTool );

	CHECKRUN( pGraph->InsertTool( pTool, NULL, 0, 0 ) );

/*
    MUSIC_TIME mtTime = 0;
    CHECKRUN(ptPerf8->GetTime(NULL, &mtTime));
    DMUS_TEMPO_PMSG* pTempo = NULL;
	CHECKRUN( ptPerf8->AllocPMsg( sizeof( DMUS_TEMPO_PMSG), (DMUS_PMSG**)&pTempo ) );
	CHECKALLOC( pTempo );
    pTempo->dwSize = sizeof(DMUS_TEMPO_PMSG);
    pTempo->dblTempo = 120; 
    pTempo->mtTime = mtTime;
    pTempo->dwFlags = DMUS_PMSGF_MUSICTIME;
    pTempo->dwType = DMUS_PMSGT_TEMPO;
	CHECKRUN( pGraph->StampPMsg( (DMUS_PMSG*)pTempo ) );
	CHECKRUN( ptPerf8->SendPMsg( (DMUS_PMSG*)pTempo ) );
*/


	Wait( 5000 );

//	CHECKRUN( ptPerf8->FreePMsg( (DMUS_PMSG*)pTempo ) );

	ptPerf8->SetDefaultAudioPath(NULL);
    if (pGraph && pTool)
    {
        pGraph->RemoveTool(pTool);
    }
	SAFE_RELEASE( pTool );
	SAFE_RELEASE( pGraph );

	return hr;
}


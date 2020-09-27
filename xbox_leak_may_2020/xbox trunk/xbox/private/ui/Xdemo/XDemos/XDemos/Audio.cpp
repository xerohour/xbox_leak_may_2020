//=============================================================================
// File: Audio.cpp
//
// Desc: This is the audio module for the XDemos project
// Created: 07/23/2001 by Michael Lyons (mlyons@microsoft.com)
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================
#include "XDemos.h"


//=============================================================================
//=============================================================================
#define WMASTRM_LOOKAHEAD_SIZE			65536
#define WMASTRM_PACKET_COUNT			8
#define WMASTRM_SOURCE_PACKET_BYTES		8192


//=============================================================================
// Global variables
//=============================================================================
IDirectSound8 *			g_pDSnd			= NULL;
IDirectSoundBuffer8 *	g_pMenuSnd		= NULL;
IDirectSoundBuffer8 *	g_pLaunchSnd	= NULL;
IDirectSoundStream *	g_pAmbient		= NULL;
XFileMediaObject *		g_pSourceFilter	= NULL;
DWORD					g_adwPacketStatus[WMASTRM_PACKET_COUNT]; // Packet status array
BYTE					g_SndBuffer[WMASTRM_PACKET_COUNT][WMASTRM_SOURCE_PACKET_BYTES] = {0};





//==============================================================================================
//==============================================================================================
typedef struct tagWAVEHDR
{
	DWORD			sRIFF;			// 'RIFF'
	DWORD			dwRIFFSize;		// rest of file size
	DWORD			sWAVE;			// 'WAVE'
	DWORD			sFMT;			// 'fmt '
	DWORD			dwFMTSize;		// rest of format size
	WAVEFORMATEX	wf;			// format
	DWORD			dwDATASize;
} WAVEHEADER;


//=============================================================================
//=============================================================================
IDirectSoundBuffer8 * LoadSound(char *szFileName)
{
	IDirectSoundBuffer8 *pSnd = NULL;

	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	DWORD dwSize = GetFileSize(hFile, NULL);
	DWORD dwRead;

	DSBUFFERDESC	dsbd;
	WAVEHEADER		wf;

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, &wf, sizeof(wf), &dwRead, NULL);

	dsbd.dwSize				= sizeof(dsbd);
	dsbd.dwFlags			= 0;
	dsbd.dwBufferBytes		= wf.dwDATASize;
	dsbd.lpwfxFormat		= &wf.wf;
	dsbd.dwMixBinMask		= 0;
	dsbd.dwInputMixBinMask	= 0;

	if (FAILED(DirectSoundCreateBuffer(&dsbd, &pSnd)))
	{
		CloseHandle(hFile);
		return NULL;
	}

	LPVOID	lpvWrite;
	DWORD	dwLength;

	if (FAILED(pSnd->Lock(0, 0, &lpvWrite, &dwLength, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
	{
		CloseHandle(hFile);
		pSnd->Release();
		return NULL;
	}

	ReadFile(hFile, lpvWrite, dwLength, &dwSize, NULL);
	CloseHandle(hFile);
	pSnd->Unlock(lpvWrite, dwLength, NULL, 0);

	return pSnd;
}

//=============================================================================
//=============================================================================
HRESULT InitAudio(void)
{
	if (FAILED(DirectSoundCreate(NULL, &g_pDSnd, NULL)))
		return -1;

	g_pMenuSnd = LoadSound(FindPath(g_pszMenuSound));
	g_pLaunchSnd = LoadSound(FindPath(g_pszLaunchSound));

	if (true)
	{
		WAVEFORMATEX wf;
		DSSTREAMDESC dss;

		WmaCreateDecoder(FindPath(g_pszAmbientSound), NULL, FALSE, WMASTRM_LOOKAHEAD_SIZE, WMASTRM_PACKET_COUNT, 0, &wf, &g_pSourceFilter);

		if (!g_pSourceFilter)
			return S_OK;

		dss.dwFlags					= 0;
		dss.dwMaxAttachedPackets	= WMASTRM_PACKET_COUNT;
		dss.lpwfxFormat				= &wf;
		dss.lpfnCallback			= 0;
		dss.lpvContext				= 0;
		dss.dwMixBinMask			= 0;

		if (FAILED(DirectSoundCreateStream(&dss, &g_pAmbient)))
			return -1;

		XMEDIAINFO xmi;
		g_pSourceFilter->GetInfo(&xmi);

		g_pAmbient->GetInfo(&xmi);


	}

	return S_OK;
}

//=============================================================================
//=============================================================================
BOOL FindFreePacket( DWORD* pdwPacketIndex )
{
    for (DWORD dwPacketIndex = 0 ; dwPacketIndex < WMASTRM_PACKET_COUNT ; dwPacketIndex++ )
    {
        if (XMEDIAPACKET_STATUS_PENDING != g_adwPacketStatus[dwPacketIndex])
        {
            if (pdwPacketIndex)
                (*pdwPacketIndex) = dwPacketIndex;

            return TRUE;
        }
    }

    return FALSE;
}


//=============================================================================
//=============================================================================
void ProcessSource(DWORD dwPacketIndex)
{
	XMEDIAPACKET xmp = {0};
	DWORD dwSourceUsed;
	DWORD dwTotalSourceUsed = 0;

    xmp.pvBuffer			= g_SndBuffer[dwPacketIndex];
    xmp.dwMaxSize			= WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwCompletedSize	= &dwSourceUsed;

    while( dwTotalSourceUsed < WMASTRM_SOURCE_PACKET_BYTES )
    {
        // Read from the source
        if (FAILED(g_pSourceFilter->Process(NULL, &xmp)))
            return;

        // Add the amount read to the total
        dwTotalSourceUsed += dwSourceUsed;

        // If we read less than the amount requested, it's because we hit
        // the end of the file.  Seek back to the start and keep going.
        if( dwSourceUsed < xmp.dwMaxSize )
        {
            xmp.pvBuffer  = (BYTE*)xmp.pvBuffer + dwSourceUsed;
            xmp.dwMaxSize = xmp.dwMaxSize - dwSourceUsed;
            
            if (FAILED(g_pSourceFilter->Flush()))
                return;
        };
    }
}


//=============================================================================
//=============================================================================
void ProcessRenderer(DWORD dwPacketIndex)
{
    XMEDIAPACKET xmp = {0};

    xmp.pvBuffer		= g_SndBuffer[dwPacketIndex];
    xmp.dwMaxSize		= WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwStatus		= &g_adwPacketStatus[dwPacketIndex];

    g_pAmbient->Process( &xmp, NULL );
}

//=============================================================================
//=============================================================================
void UpdateAudio(void)
{
	if (!g_pSourceFilter)
		return;

	DirectSoundDoWork();

    DWORD   dwPacketIndex;
    
    while( FindFreePacket( &dwPacketIndex ) )
    {
         ProcessSource(dwPacketIndex);
         ProcessRenderer(dwPacketIndex);
    }
}

//=============================================================================
//=============================================================================
void PlayLaunchSound(void)
{
	g_pLaunchSnd->Play(0, 0, 0);

	while (true)
	{
		DWORD dwStatus;

		g_pLaunchSnd->GetStatus(&dwStatus);

		if ((dwStatus & DSBSTATUS_PLAYING) == 0)
			break;
	}
}

//=============================================================================
//=============================================================================
void PlayMenuSound(void)
{
	g_pMenuSnd->Play(0, 0, 0);
}


//=============================================================================
//=============================================================================
void StopAudio(void)
{
	if (!g_pSourceFilter)
		return;

	g_pAmbient->Pause(DSSTREAMPAUSE_PAUSE);
}

//=============================================================================
//=============================================================================
void StartAudio(void)
{
	if (!g_pSourceFilter)
		return;

	g_pAmbient->Pause(DSSTREAMPAUSE_RESUME);
}


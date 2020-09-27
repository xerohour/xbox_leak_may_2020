/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "quakedef.h"
#include "xquake.h"

typedef enum {SIS_SUCCESS, SIS_FAILURE, SIS_NOTAVAIL} sndinitstat;

static qboolean	dsound_init;
static qboolean	snd_firsttime = true, snd_isdirect;
static qboolean	primary_format_set;

static int	sample16;
static int	snd_sent, snd_completed;


/* 
 * Global variables. Must be visible to window-procedure function 
 *  so it can unlock and free the data block after it has been played. 
 */ 

DWORD	gSndBufSize;

LPDIRECTSOUND pDS;
LPDIRECTSOUNDBUFFER pDSBuf;

qboolean SNDDMA_InitDirect (void);


/*
==================
S_BlockSound
==================
*/
void S_BlockSound (void)
{
}


/*
==================
S_UnblockSound
==================
*/
void S_UnblockSound (void)
{
}


/*
==================
FreeSound
==================
*/
void FreeSound (void)
{
	int		i;

	if (pDSBuf)
	{
		IDirectSoundBuffer_Release(pDSBuf);
	}

	if (pDS)
	{
		IDirectSound_Release(pDS);
	}

	pDS = NULL;
	pDSBuf = NULL;
	dsound_init = false;
}


/*
==================
SNDDMA_InitDirect

Direct-Sound support
==================
*/
sndinitstat SNDDMA_InitDirect (void)
{
	DSBUFFERDESC	dsbuf;
	WAVEFORMATEX	format, pformat; 
	HRESULT			hresult;
	int				reps;

	memset ((void *)&sn, 0, sizeof (sn));

	shm = &sn;

	shm->channels = 2;
	shm->samplebits = 16;
	shm->speed = 11025;

	memset (&format, 0, sizeof(format));
	format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = (unsigned short)shm->channels;
    format.wBitsPerSample = (unsigned short)shm->samplebits;
    format.nSamplesPerSec = shm->speed;
    format.nBlockAlign = format.nChannels
		*format.wBitsPerSample / 8;
    format.cbSize = 0;
    format.nAvgBytesPerSec = format.nSamplesPerSec
		*format.nBlockAlign; 

    Con_SafePrintf("Initializing DirectSound\n");

	while ((hresult = DirectSoundCreate(0, &pDS, NULL)) != DS_OK)
	{
    	Con_SafePrintf("DirectSound creation failure");

		return SIS_FAILURE;
	}

#ifndef _XBOX
	if (DS_OK != IDirectSound_SetCooperativeLevel (pDS, mainwindow, DSSCL_EXCLUSIVE))
	{
		Con_SafePrintf ("Set coop level failed\n");
		FreeSound ();
		return SIS_FAILURE;
	}

#endif
	// create the secondary buffer we'll actually work with
	memset (&dsbuf, 0, sizeof(dsbuf));
	dsbuf.dwSize = sizeof(DSBUFFERDESC);
	dsbuf.dwBufferBytes = 0x8000;
	dsbuf.lpwfxFormat = &format;

	if (DS_OK != IDirectSound_CreateSoundBuffer(pDS, &dsbuf, &pDSBuf, NULL))
	{
		Con_SafePrintf ("DS:CreateSoundBuffer Failed");
		FreeSound ();
		return SIS_FAILURE;
	}

	shm->channels = format.nChannels;
	shm->samplebits = format.wBitsPerSample;
	shm->speed = format.nSamplesPerSec;

	if (snd_firsttime)
		Con_SafePrintf ("Using secondary sound buffer\n");

	// Make sure mixer is active
	if (snd_firsttime)
		Con_SafePrintf("   %d channel(s)\n"
		               "   %d bits/sample\n"
					   "   %d bytes/sec\n",
					   shm->channels, shm->samplebits, shm->speed);

    
	gSndBufSize = dsbuf.dwBufferBytes;

	IDirectSoundBuffer_Play(pDSBuf, 0, 0, DSBPLAY_LOOPING);

	shm->soundalive = true;
	shm->splitbuffer = false;
	shm->samples = gSndBufSize/(shm->samplebits/8);
	shm->samplepos = 0;
	shm->submission_chunk = 1;
	shm->buffer = NULL;
	sample16 = (shm->samplebits/8) - 1;

	dsound_init = true;

	return SIS_SUCCESS;
}


/*
==================
SNDDMA_Init

Try to find a sound device to mix for.
Returns false if nothing is found.
==================
*/

// #define DISABLE_SOUND

int SNDDMA_Init(void)
{
#ifdef DISABLE_SOUND
	dsound_init = 0;
	snd_firsttime = false;
	Con_SafePrintf ("Sound disabled.\n");

	return 0;
#else

	sndinitstat	stat;

	dsound_init = 0;

	stat = SIS_FAILURE;	// assume DirectSound won't initialize

	/* Init DirectSound */
	if (snd_firsttime || snd_isdirect)
	{
		stat = SNDDMA_InitDirect ();;

		if (stat == SIS_SUCCESS)
		{
			snd_isdirect = true;

			if (snd_firsttime)
				Con_SafePrintf ("DirectSound initialized\n");
		}
		else
		{
			snd_isdirect = false;
			Con_SafePrintf ("DirectSound failed to init\n");
		}
	}

	if (!dsound_init)
	{
		if (snd_firsttime)
			Con_SafePrintf ("No sound device initialized\n");

		return 0;
	}

	return 1;
#endif
}

/*
==============
SNDDMA_GetDMAPos

return the current sample position (in mono samples read)
inside the recirculating dma buffer, so the mixing code will know
how many sample are required to fill it up.
===============
*/
int SNDDMA_GetDMAPos(void)
{
    int s = 0;

	if (dsound_init) 
	{
		IDirectSoundBuffer_GetCurrentPosition(pDSBuf, &s, NULL);
	}

	s >>= sample16;

	s &= (shm->samples-1);

	return s;
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/
void SNDDMA_Submit(void)
{
}

/*
==============
SNDDMA_Shutdown

Reset the sound device for exiting
===============
*/
void SNDDMA_Shutdown(void)
{
	FreeSound ();
}




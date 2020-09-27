#include "XAudio.h"

/****************************************************************************************************************
 *
 * FUNCTION:  xbGetHandle(void*, DWORD, DWORD)
 *
 * PURPOSE:	  This function returns handle to a sound effect for later use.
 *
 ****************************************************************************************************************/


void* __stdcall xbGetHandle(void* pvData, DWORD dwBufSiz, DWORD dwFlags)
{
	static int i = 0;

	//Sorry, only 256 different sound effects supported
	if(i > NUMHANDLES)
		return NULL;


	g_HandleTable[i].dwFlags	= dwFlags;
	g_HandleTable[i].dwBufSiz	= dwBufSiz;
	g_HandleTable[i].pvData		= pvData;

	if(dwFlags && THREEDSND)
	{	
		g_HandleTable[i].nID	= (g_SNDID3D%NUM3DSTRUCTS);
		g_SndArr[g_SNDID3D%NUM3DSTRUCTS].pvData = pvData;
		g_SNDID3D++;
	}else{
		g_HandleTable[i].nID	= (g_SNDID2D%NUM3DSTRUCTS);
		g_2DSndArr[g_SNDID2D%NUM3DSTRUCTS].pvData = pvData;
		g_SNDID2D++;
	}

	//Increment the counter

	return (void*)&(g_HandleTable[i++]);
}


/****************************************************************************************************************
 *
 * FUNCTION:  xbInitAudio(void)
 *
 * PURPOSE:	  This function will initialize my Sound Handle system, Listeners, etc.  Currently I will have 
 *			  NUMHANDLES handles available, and NUMSNDSTRUCTS actually able to be played at one time.  
 *			  The handle system will keep track of the Handle value as well as the actual sound data associated
 *			  with that handle.  The actual playing sounds will have more data available for them.
 *
 * USE:		  This should be called initially to 'start up' the sound system.  It should be around for the life
 *			  of the .xbe.  If clean up is needed, I have provided an xbUninitAudio() function-- although this
 *			  should not be necessary, as was recomended in the white binder given out at XFest.
 *
 ****************************************************************************************************************/


HRESULT __stdcall xbInitAudio()
{
	HRESULT			hr = S_OK;
	DSBUFFERDESC	bufDesc;
	DSBUFFERDESC	bufDesc2D;
	WAVEFORMATEX	waveFmt;

	//Initialize the counters
	g_SNDID3D = 0;
	g_SNDID2D = 0;

	//Create the DSound Interface... we can use this later, so keep it around for the life of the app

	hr = DirectSoundCreate(NULL,
						   &g_pDSnd,
						   NULL);


	assert(SUCCEEDED(hr));

	if(FAILED(hr))
		return hr; 	//We are in big trouble if this fails... pass back the DSound Error code

	//Set up the default wave format
	waveFmt.cbSize			= 0;				//Size of extra data;  PCM == 0 (no extra data)
	waveFmt.nChannels		= 1;				//One channel
	waveFmt.wBitsPerSample	= 16;				//We will always use 16;  8 sounds crappy
	waveFmt.nBlockAlign		= 2;				//Product of bits/sample & channels divided by 8
	waveFmt.nAvgBytesPerSec = 44100;			//Product of samples/sec & block align
    waveFmt.nSamplesPerSec  = 22050;
	waveFmt.wFormatTag		= WAVE_FORMAT_PCM;  //Use PCM for now



	//Set up the default Buffer Description
	bufDesc.dwBufferBytes = 0;
	bufDesc.dwFlags		  = DSBCAPS_CTRLVOLUME |//DSBCAPS_NOMEMORYALLOC |	//Derek says we don't need this flag; 0 buf size is sufficient 
							DSBCAPS_CTRL3D |			//3D sound buffer-- no Pan available
							DSBCAPS_CTRLFREQUENCY; //|		//JJBUG If we want to control the frequency, we *cannot* use fx... address this
							//DSBCAPS_CTRLFX; //JJBUG-- need XDK update

	bufDesc.dwSize		  = sizeof(DSBUFFERDESC);
	bufDesc.lpwfxFormat   = &waveFmt;

	//Set up the default 2D Buffer Description
	bufDesc2D.dwBufferBytes = 0;
	bufDesc2D.dwFlags		= DSBCAPS_CTRLVOLUME;//DSBCAPS_CTRLPAN |
							  DSBCAPS_CTRLFREQUENCY;
	bufDesc2D.dwSize		= sizeof(DSBUFFERDESC);
	bufDesc2D.lpwfxFormat	= &waveFmt;

	int i = 0;
	for(i = 0; i < NUMHANDLES; i++)
	{
		//Initialize the Handle table
		g_HandleTable[i].pvData				= NULL;
		g_HandleTable[i].nID				= INVALID;
		g_HandleTable[i].dwInConeAngle		= DS3D_DEFAULTCONEANGLE;
		g_HandleTable[i].dwOutConeAngle		= DS3D_DEFAULTCONEANGLE;
		g_HandleTable[i].lOuterVolume		= DSBVOLUME_MAX;
		g_HandleTable[i].lVolume			= DSBVOLUME_MAX;
		g_HandleTable[i].nID				= 0x0;
		g_HandleTable[i].pPosition.x		= 0;
		g_HandleTable[i].pPosition.y		= 0;
		g_HandleTable[i].pPosition.z		= 0;


		if(i < NUM3DSTRUCTS)
		{
			//Initialzie the Sound Struct Array
			g_SndArr[i].dwFlags			= 0x0;

			//Get a Buffer
			hr = g_pDSnd->CreateSoundBuffer(&bufDesc,
			  								&(g_SndArr[i].pBuffer),
											NULL);

			assert(SUCCEEDED(hr));
	
			//So we are screwed... uninit everything, and return failure
			if(FAILED(hr))
			{
				i -= 1; //Get the last successful allocation & roll back from there
				for(i; i >= 0; i--)
				{
					//Clean up
					g_SndArr[i].pBuffer->Release();
					g_SndArr[i].pBuffer = NULL;

					//And Clean up 2D...subtract 1, becaue the ith one hasn't been created yet.
					if((i - 1) >= 0)
					{
						g_2DSndArr[i-1].pBuffer->Release();
						g_2DSndArr[i-1].pBuffer = NULL;
					}
				}
				g_pDSnd->Release();
				g_pDSnd = NULL;

				return hr;
			}
		}

		//Get the 2D Handles taken care of as well
		if(i < NUM2DSTRUCTS)
		{
			hr = g_pDSnd->CreateSoundBuffer(&bufDesc2D,
											&(g_2DSndArr[i].pBuffer),
											NULL);

			assert(SUCCEEDED(hr));

			if(FAILED(hr))
			{
				for(i; i >=0; i--)
				{
					//CleanUp
					g_2DSndArr[i].pBuffer->Release();
					g_2DSndArr[i].pBuffer = NULL;

					//CleanUp 3D too
					g_SndArr[i].pBuffer->Release();
					g_SndArr[i].pBuffer = NULL;

				}
				g_pDSnd->Release();
				g_pDSnd = NULL;

				return hr;
			}
		}
				
				

	}

	//OK... So our Handle/Structure data is all nicely initialized;  so now set up the global listener stuff
	g_Listener.dwSize			= sizeof(DS3DLISTENER);
	g_Listener.flDistanceFactor = DISTANCE;		//# of meters in a vector unit JJBUG-- verify this w/ Ed
	g_Listener.flDopplerFactor	= DOPPLER;		//This is a multiple of real world doppler-- default is real world
	g_Listener.flRolloffFactor	= ROLLOFF;		//This is a multiple of real world roll off-- default is real world
	g_Listener.vOrientFront.x	= 0;			//Start off looking down the -Z axis
	g_Listener.vOrientFront.y	= 0;
	g_Listener.vOrientFront.z   = -1;
	g_Listener.vOrientTop.x		= 0;			//Up is positive Y axis
	g_Listener.vOrientTop.y     = 1;
	g_Listener.vOrientTop.z		= 0;
	g_Listener.vPosition.x		= 0;			//Origin is default
	g_Listener.vPosition.y		= 0;
	g_Listener.vPosition.z		= 0;
	g_Listener.vVelocity.x		= 0;			//Velocity is 0 at first
	g_Listener.vVelocity.y		= 0;
	g_Listener.vVelocity.z		= 0;

	return S_OK;


	//Apply these settings
	hr = g_pDSnd->SetAllParameters(&g_Listener,
							       DS3D_IMMEDIATE);

	assert(SUCCEEDED(hr));

    hr = DownloadScratch(g_pDSnd, "Add scratch file here");

	//Init the Streaming system
	InitStreams();

	//Init the Channel system
	InitChannels();
	
	return S_OK;
}


/**************************************************************************************************************
 *
 * FUNCTION:  xbPlaySound(SNDHANDLE)
 *
 * PURPOSE:	  This function plays a sound.
 *
 * USE:		  If this is the first time playing the sound, passing the data along with NULL for SNDHANDLE is 
 *			  sufficient.  If not, then passing a valid SNDHANDLE with a NULL void* is sufficient.
 *
 * ASSUMPTIONS:  I am assuming that for efficency sake, the caller will save my HANDLES that I generate, and when
 *				 playing the sound later on, I will have to do minimal adjustments, and this makes for the most
 *				 efficient playing of sounds.  The first time that the sound is played, the caller should pass
 *				 a pointer to NULL for the PSNDHANDLE, and a vaild void* pvData.  Any future calls to play that sound
 *				 need only pass the PSNDHANDLE that I filled in for the caller.  *If* all the sounds channels are 
 *				 currently in use, the system will stop the sound that is closest to finishing of the 32 playing.
 *
 **************************************************************************************************************/

HRESULT __stdcall xbPlaySound(void* pHandle, bool bObstruct)
{
	//Cast
	SNDHANDLE pHndl = (SNDHANDLE)pHandle;

	PHSOUND pSnd	= NULL;
	bool	bCache	= false;  //See if we need to set any values or not
	DWORD	dwTicks = 0x0;	  //# of ticks for this sound to play
	

	//This function doesn't do things that should be streamed
	if((pHndl->dwFlags & ISVOICE) || (pHndl->dwFlags & ISMUSIC))
		return E_FAIL;										//JJBUG-- Come up w/ Custom error codes for this stuff


	//If PSNDHANDLE != NULL, then it must be a valid handle
	if(NULL == pHndl)
		return E_FAIL;			//JJBUG-- Custom Error code
	

	//See if we have a valid Read pointer
	if(IsBadReadPtr(pHndl, sizeof(SNDHANDLE)))
		return E_FAIL;

	pSnd = SoundFromHandle(pHndl);

	//Set the format of the buffer...
	if(!((pHndl->dwFlags & BUFFER_MASK) == (pSnd->dwFlags & BUFFER_MASK)))
	{
		//Wosrt case...The format is different
		switch(pHndl->dwFlags & SAMPLE_MASK)
		{
		case TWOTWOKHZ:
			pSnd->pBuffer->SetFrequency(22100);
			break;
		case FORFORKHZ:
			pSnd->pBuffer->SetFrequency(44200);
			break;
		}

		switch(pHndl->dwFlags & FORMAT_MASK)
		{
		case PCM:
			break;
		case ADPCM:
			break;
		//There should be *NO* WMA sounds calling *this* function ever.
		case WMA:
			break;
		}
	}

	//Set the new flags & data
	pSnd->dwFlags = pHndl->dwFlags;
	pSnd->pvData  = pHndl->pvData;

	//PIII 733 -- 733Million Ticks/Second
	if(pHndl->dwFlags & TWOTWOKHZ)
		dwTicks = 733000000 * (pHndl->dwBufSiz)/(22050 * 2);
	else
		dwTicks = 733000000 * (pHndl->dwBufSiz)/(44100 * 2);
	//Set the new buffer data
	pSnd->pBuffer->SetBufferData(pHndl->pvData, pHndl->dwBufSiz);

	if(pSnd->dwFlags && RANDOM)
	{
		//JJBUG-- Apply the random effect to the buffer
	}

	//Apply changes... make sure that this buffer plays the sound the right way...
	DS3DBUFFER	dsChanges;
	HRESULT		hr;

	if(pSnd->dwFlags & THREEDSND)
	{
		dsChanges.dwInsideConeAngle = pHndl->dwInConeAngle;
		dsChanges.dwOutsideConeAngle= pHndl->dwOutConeAngle;
		dsChanges.dwSize			= sizeof(DS3DBUFFER);
		dsChanges.vVelocity.x		= pHndl->pVelocity.x;
		dsChanges.vVelocity.y		= pHndl->pVelocity.y;
		dsChanges.vVelocity.z		= pHndl->pVelocity.z;
		dsChanges.vPosition.x		= pHndl->pPosition.x;
		dsChanges.vPosition.y		= pHndl->pPosition.y;
		dsChanges.vPosition.z		= pHndl->pPosition.z;
		dsChanges.vConeOrientation.x= pHndl->vConeOrientation.x;
		dsChanges.vConeOrientation.y= pHndl->vConeOrientation.y;
		dsChanges.vConeOrientation.z= pHndl->vConeOrientation.z;
		dsChanges.dwMode			= DS3DMODE_HEADRELATIVE;//DS3DMODE_NORMAL;
		dsChanges.lConeOutsideVolume= pHndl->lOuterVolume;
		dsChanges.flMaxDistance		= DS3D_DEFAULTMAXDISTANCE;
		dsChanges.flMinDistance		= DS3D_DEFAULTMINDISTANCE;

		hr = pSnd->pBuffer->SetAllParameters(&dsChanges,DS3D_IMMEDIATE);

		assert(SUCCEEDED(hr));
	}

	//2D & 3D... make sure that the volume is correct
	pSnd->pBuffer->SetVolume(pHndl->lVolume);
	

	//Time stamp what the tick count will be when this is over.
	dwTicks += GetTickCount();
	pSnd->dwEndTime = dwTicks;

	if(pSnd->dwFlags & LOOPING)
		return pSnd->pBuffer->Play(0,0,DSBPLAY_LOOPING);
	else
		return pSnd->pBuffer->Play(0,0,0);
}


/*************************************************************************************************************
 *
 * FUNCTION: xbGetChannel()
 *
 * PURPOSE:  Returns a 3D sound channel that can have 2D sounds routed to it
 *
 * USE:		 Same as purpose
 *
 *************************************************************************************************************/

void* __stdcall xbGetChannel()
{
	int i = 0;
	for(i = 0; i < NUMCHANNELS; i++)
	{
		if(!(g_ChannelTable[i].nID))
		{
			//For channels, the ID determines if it is currently in use or not...
			g_ChannelTable[i].nID = 1;
			return (void*)&g_ChannelTable[i];
		}
	}

	//All Channels are currently in use
	return NULL;
}


/*************************************************************************************************************
 *
 * FUNCTION: xbFreeChannel(HCHANNEL)
 *
 * PURPOSE:  Returns a 3D sound channel that can have 2D sounds routed to it
 *
 * USE:		 Same as purpose
 *
 *************************************************************************************************************/

HRESULT __stdcall xbFreeChannel(void* hC)
{
	for(int i = 0; i < NUMCHANNELS; i++)
	{
		if(&g_ChannelTable[i] == hC)
		{
			//Mark it as Free
			g_ChannelTable[i].nID = 0;
			return S_OK;
		}
	}

	//This must have not been a channel pointer
	return E_FAIL;
}


HRESULT __stdcall InitChannels()
{
	DSBUFFERDESC bufDesc;
	WAVEFORMATEX waveFmt;
	HRESULT hr = S_OK;

	bufDesc.dwBufferBytes	= 0;
	bufDesc.dwFlags			= DSBCAPS_CTRL3D | DSBCAPS_MIXIN | DSBCAPS_CTRLVOLUME;
	bufDesc.dwSize			= sizeof(DSBUFFERDESC);
	bufDesc.lpwfxFormat		= &waveFmt;

	//Assume this for now...
	waveFmt.cbSize			= 0;				//Size of extra data;  PCM == 0 (no extra data)
	waveFmt.nChannels		= 1;				//One channel
	waveFmt.wBitsPerSample	= 16;				//We will always use 16;  8 sounds crappy
	waveFmt.nBlockAlign		= 2;				//Product of bits/sample & channels divided by 8
	waveFmt.nAvgBytesPerSec = 44100;			//Product of samples/sec & block align
    waveFmt.nSamplesPerSec  = 22050;
	waveFmt.wFormatTag		= WAVE_FORMAT_PCM;  //Use PCM for now


	int i = 0;
	for(i = 0; i < NUMCHANNELS; i++)
	{
		g_ChannelTable[i].dwBufSiz = 0;
		g_ChannelTable[i].dwInConeAngle = DS3D_DEFAULTCONEANGLE;
		g_ChannelTable[i].dwOutConeAngle= DS3D_DEFAULTCONEANGLE;
		g_ChannelTable[i].lOuterVolume	= DS3D_DEFAULTCONEOUTSIDEVOLUME;
		g_ChannelTable[i].lVolume		= DSBVOLUME_MAX;
		g_ChannelTable[i].nID			= 0;  //Not currently in use
		g_ChannelTable[i].pPosition.x	= 0;
		g_ChannelTable[i].pPosition.y	= 0;
		g_ChannelTable[i].pPosition.z	= 0;
		g_ChannelTable[i].pVelocity.x	= 0;
		g_ChannelTable[i].pVelocity.y	= 0;
		g_ChannelTable[i].pVelocity.z	= 0;
		g_ChannelTable[i].vConeOrientation.x = 0;
		g_ChannelTable[i].vConeOrientation.y = 0;
		g_ChannelTable[i].vConeOrientation.z = 0;
		hr = g_pDSnd->CreateSoundBuffer(&bufDesc,
								        ((IDirectSoundBuffer8**)(&g_ChannelTable[i].pvData)),
								        NULL);
		assert(SUCCEEDED(hr));
		if(FAILED(hr))
		{
			//This last one didn't work... so don't need to undo it
			i-=1;
			for(i; i >= 0; i--)
			{
				((IDirectSoundBuffer8*)(g_ChannelTable[i].pvData))->Release();
			}

			return E_FAIL;
		}

	}

	return S_OK;
}

/*************************************************************************************************************
 *
 * FUNCTION: xbPlay3DSoundChannel(CHANNEL3D)
 *
 * PURPOSE:  Plays a buffer that has had multiple sounds routed to it
 *
 * USE:		 Same as purpose
 *
 *************************************************************************************************************/

HRESULT __stdcall xbPlay3DSoundChannel(HCHANNEL3D hChannel)
{

	SNDHANDLE pHndl = (SNDHANDLE)hChannel;

	//See if we have a valid Read pointer
	if(IsBadReadPtr(pHndl, sizeof(SNDSTRUCT)))
		return E_FAIL;


	//This function doesn't do things that should be streamed
	if((pHndl->dwFlags && ISVOICE) || (pHndl->dwFlags && ISMUSIC))
		return E_FAIL;

	if(pHndl->dwFlags & LOOPING)
		return ((IDirectSoundBuffer8*)(pHndl->pvData))->Play(0,0,DSBPLAY_LOOPING);
	else
		return ((IDirectSoundBuffer8*)(pHndl->pvData))->Play(0,0,0);


}

/*************************************************************************************************************
 *
 * FUNCTION: xbRoute2DSound(SNDHANDLE)
 *
 * PURPOSE:  Routes a 2D sound into a 3D buffer
 *
 * USE:		 Same as purpose
 *
 *************************************************************************************************************/
	
HRESULT __stdcall xbRoute2DSound(SNDHANDLE pHandle, HCHANNEL3D hChannel)
{
	SNDHANDLE pHndl	 = (SNDHANDLE)pHandle;
	SNDHANDLE pCHndl = (SNDHANDLE)hChannel;
	//See if we have a valid Read pointer
	if(IsBadReadPtr(pHndl, sizeof(SNDHANDLE)))
		return E_FAIL;


	//This function doesn't do things that should be streamed
	if((pHndl->dwFlags && ISVOICE) || (pHndl->dwFlags && ISMUSIC))
		return E_FAIL;


	PHSOUND pSnd   = SoundFromHandle(pHndl);

	//PHSOUND pSndCh = SoundFromHandle(pCHndl);

	if((NULL != pSnd) && (pCHndl != NULL))
	{
		//pSnd->pBuffer->SetOutputBuffer(pSndCh->pBuffer);
		return (((IDirectSoundBuffer8*)pCHndl->pvData)->Play(0,0,0));
	}

	return E_FAIL;
}


/*************************************************************************************************************
 *
 * FUNCTION: xbStopSound(SNDHANDLE)
 *
 * PURPOSE:  Stops a sound if it is currently playing
 *
 * USE:		 Same as purpose
 *
 *************************************************************************************************************/

HRESULT __stdcall xbStopSound(void* hSound)
{
	SNDHANDLE hSnd = (SNDHANDLE)hSound;

	HRESULT hr = S_OK;
	PHSOUND pSnd = NULL;

	//Not much we can do if we cannot stop a sound that is playing... but
	if(hSnd->dwFlags && THREEDSND)
	{
		if(hSnd->pvData == g_SndArr[hSnd->nID].pvData)
			g_SndArr[hSnd->nID].pBuffer->Stop();
	}else{
		if(hSnd->pvData == g_2DSndArr[hSnd->nID].pvData)
			g_SndArr[hSnd->nID].pBuffer->Stop();
	}

	return S_OK;
}

/*************************************************************************************************************
 *
 * FUNCTION: xbSetVolume(SNDHANDLE, LONG)
 *
 * PURPOSE:  Changes the volume of a sound.
 *
 * USE:		 If the sound is currently playing, the volume changes immediately;  otherwise, the volume will get
 *			 set when it is next played.
 *
 *************************************************************************************************************/


HRESULT __stdcall xbSetVolume(void* hSound, LONG lVol)
{
	SNDHANDLE hSnd		= (SNDHANDLE)hSound;
	DWORD	  dwStatus	= 0x0;
	HRESULT hr			= S_OK;  //JJBUG-- initialize to a 'not currently playing error'
	
	//Set the volume on the sound buffer
	if(hSnd->pvData == g_SndArr[hSnd->nID].pvData)
	{
		//See if the sound is playing...
		hr = g_SndArr[hSnd->nID].pBuffer->GetStatus(&dwStatus);

		//Just assert for Debug purposes; doesn't really matter otherwise
		assert(SUCCEEDED(hr));
	
		if(dwStatus & DSBSTATUS_PLAYING)
		{
			hr = g_SndArr[hSnd->nID].pBuffer->SetVolume(lVol);
			//If this fails we want to know about in the debugger, otherwise, there is not
			//much else we can do
			assert(SUCCEEDED(hr));
		}
	}

	hSnd->lVolume		= lVol;

	return hr;
}

/*************************************************************************************************************
 *
 * FUNCTION: xbSetConeOuterVolume(SNDHANDLE, LONG)
 *
 * PURPOSE:  Changes the Outer Cone Volume of a sound.
 *
 * USE:		 If the sound is currently playing, the outer cone volume changes immediately; otherwise, the volume will get
 *			 set when it is next played.
 *
 *************************************************************************************************************/

HRESULT __stdcall xbSetConeOuterVolume(void* hSound, LONG lVol)
{
	SNDHANDLE hSnd		= (SNDHANDLE)hSound;
	DWORD	  dwStatus	= 0x0;
	HRESULT hr			= S_OK;  //JJBUG-- initialize to a 'not currently playing error'

	hSnd->lOuterVolume	= lVol;

	//If it's not 3D we don't care
	if(!(hSnd->dwFlags & THREEDSND))
		return S_OK;

	if(hSnd->pvData == g_SndArr[hSnd->nID].pvData)
	{
		hr = g_SndArr[hSnd->nID].pBuffer->GetStatus(&dwStatus);

		//Just assert for Debug purposes; doesn't really matter otherwise
		assert(SUCCEEDED(hr));
	
		if(dwStatus & DSBSTATUS_PLAYING)
		{
			hr = g_SndArr[hSnd->nID].pBuffer->SetConeOutsideVolume(lVol,DS3D_IMMEDIATE);
			//If this fails we want to know about in the debugger, otherwise, there is not
			//much else we can do
			assert(SUCCEEDED(hr));
		}
	}

	return hr;
}




//JJBUG-- Finish this when the envelope generator API for XDK is in...
HRESULT __stdcall xbFadeVolume(void* hSound, LONG lVol, int nMilliSeconds)
{
	SNDHANDLE		hSnd		= (SNDHANDLE)hSound;

	HRESULT			hr			= S_OK;
	PHSOUND			pSnd		= NULL;
	DWORD			dwStatus	= 0x0;
/*	DSENVELOPEDESC	envDesc;
	DWORD			dwAttack	= 0x0;


	//Make sure it is currently cached..if not, it can't be playing so do nothing.
	if(hSnd->pvData == g_SndArr[hSnd->nID].pvData)
	{
		pSnd = &(g_SndArr[hSnd->nID]);

		//Make sure we have a buffer
		if(pSnd->pBuffer)
		{
			hr = pSnd->pBuffer->GetStatus(&dwStatus);

			//Make sure it is currently playing
			if(dwStatus && DSBSTATUS_PLAYING)
			{
				if(pSnd->dwFlags & TWOTWOKHZ)
				{
					dwAttack = 220500/32;  //samples per second divided by samples per frames gives us frames/second
				}else{
					dwAttack = 44100/32;
				}

				//Frames/second * Fade time / 1000 == # of sample frames to fade over
				dwAttack = (dwAttack * nMilliSeconds)/1000;

				envDesc.wEnvelopeDelay  = 0;		//Start Immediately
				envDesc.wEnvelopeAttack = 0;		//# of sample frames
				envDesc.wEnvelopeHold	= 0xfff;	//How many frames to 'hold' the envelope
				envDesc.wEnvelopeDecay	= dwDecay;	//# of sample frames
				envDesc.wEnvelopeSustain= 0;
				envDesc.wEnvelopeRelease= 0;

				//Use the envelope generator
				hr = pSnd->pBuffer->SetEG1(&envDesc);
			}

		}


	}
*/
	return hr;
}

/*************************************************************************************************************
 *
 * FUNCTION: xbSet3DPosition(SNDHANDLE, Vector)
 *
 * PURPOSE:  Sets the position of a sound in 3D space
 *
 * USE:		 If the sound is currently playing, the position is set immediately.  Otherwise it will save that
 *			 positional data & apply it the next time the sound is played.
 *
 *************************************************************************************************************/


HRESULT __stdcall xbSet3DPosition(void* hSound, Vector v)
{
	SNDHANDLE hSnd	= (SNDHANDLE)hSound;

	HRESULT hr		= S_OK;
	PHSOUND pSnd	= &(g_SndArr[hSnd->nID]);
	DWORD	dwStatus= 0x0;

	if(hSnd->pvData == pSnd->pvData)
	{
		pSnd->pBuffer->GetStatus(&dwStatus);

		//See if it is playing
		if(dwStatus && DSBSTATUS_PLAYING)
		{
			hr = pSnd->pBuffer->SetPosition(v.x,v.y,v.z,DS3D_IMMEDIATE);
		}
	}

	hSnd->pPosition.x = v.x;
	hSnd->pPosition.y = v.y;
	hSnd->pPosition.z = v.z;

	return hr;
}



/*************************************************************************************************************
 *
 * FUNCTION: xbSet3DVelocity(SNDHANDLE, Vector)
 *
 * PURPOSE:  Sets the velocity of a sound in 3D space
 *
 * USE:		 If the sound is currently playing, the velocity is set immediately.  Otherwise it will save that
 *			 velocity data & apply it the next time the sound is played.
 *
 *************************************************************************************************************/


HRESULT __stdcall xbSet3DVelocity(void* hSound, Vector v)
{
	SNDHANDLE hSnd	= (SNDHANDLE)hSound;

	HRESULT hr		= S_OK;
	PHSOUND pSnd	= &(g_SndArr[hSnd->nID]);
	DWORD	dwStatus= 0x0;

	if(hSnd->pvData == pSnd->pvData)
	{
		pSnd->pBuffer->GetStatus(&dwStatus);

		//See if it is playing
		if(dwStatus && DSBSTATUS_PLAYING)
		{
			hr = pSnd->pBuffer->SetVelocity(v.x,v.y,v.z,DS3D_IMMEDIATE);
		}
	}

	hSnd->pVelocity.x = v.x;
	hSnd->pVelocity.y = v.y;
	hSnd->pVelocity.z = v.z;

	return hr;
}


/*************************************************************************************************************
 *
 * FUNCTION: xbSet3DCone(SNDHANDLE, float, float, float)
 *
 * PURPOSE:  Sets the Cone attributes of a sound in 3D space
 *
 * USE:		 If the sound is currently playing, the cone attributes is set immediately.  Otherwise it will save that
 *			 cone data & apply it the next time the sound is played.
 *
 *************************************************************************************************************/


HRESULT __stdcall xbSet3DCone(void* hSound, DWORD dwInsideAngle, DWORD dwOutsideAngle, Vector vOrientation, LONG lOuterVolume)
{
	SNDHANDLE hSnd	= (SNDHANDLE)hSound;

	HRESULT hr		= S_OK;
	PHSOUND pSnd	= &(g_SndArr[hSnd->nID]);
	DWORD	dwStatus= 0x0;

	if(hSnd->pvData == pSnd->pvData)
	{
		pSnd->pBuffer->GetStatus(&dwStatus);

		//See if it is playing
		if(dwStatus && DSBSTATUS_PLAYING)
		{
			hr = pSnd->pBuffer->SetConeAngles(dwInsideAngle, dwOutsideAngle,DS3D_IMMEDIATE);
			assert(SUCCEEDED(hr));
			hr = pSnd->pBuffer->SetConeOutsideVolume(lOuterVolume,DS3D_IMMEDIATE);
			assert(SUCCEEDED(hr));
			hr = pSnd->pBuffer->SetConeOrientation(vOrientation.x,vOrientation.y,vOrientation.z,DS3D_IMMEDIATE);
			assert(SUCCEEDED(hr));
		}
	}

	hSnd->dwInConeAngle		= dwInsideAngle;
	hSnd->dwOutConeAngle	= dwOutsideAngle;
	
	hSnd->vConeOrientation.x= vOrientation.x;
	hSnd->vConeOrientation.y= vOrientation.y;
	hSnd->vConeOrientation.z= vOrientation.z;

	hSnd->lOuterVolume		= lOuterVolume;

	return hr;
}

/*************************************************************************************************************
 *
 * FUNCTION: xbIsSoundPlaying(void*,bool*)
 *
 * PURPOSE:  Tells the caller if a sound is currently playing
 *
 * USE:		 Same as purpose.
 *
 *************************************************************************************************************/


HRESULT __stdcall xbIsSoundPlaying(void* hSound, bool* bPlaying)
{
	SNDHANDLE hSnd	= (SNDHANDLE)hSound;

	PHSOUND pSnd	= &(g_SndArr[hSnd->nID]);
	DWORD	dwStatus= 0x0;

	if(hSnd->pvData == pSnd->pvData)
	{
		pSnd->pBuffer->GetStatus(&dwStatus);

		//See if it is playing
		if(dwStatus && DSBSTATUS_PLAYING)
			*bPlaying = true;
		else
			*bPlaying = false;
	}else{
		*bPlaying = false;
	}

	return S_OK;
}

/*************************************************************************************************************
 *
 * FUNCTION: xbGetFlags(SNDHANDLE, DWORD*)
 *
 * PURPOSE:  Gets the flags that describe this sound
 *
 * USE:		 Use this to get information about the sound.
 *
 *************************************************************************************************************/
		

HRESULT __stdcall xbGetFlags(void* hSound, DWORD* dwFlags)
{
	SNDHANDLE hSnd = (SNDHANDLE)hSound;

	*dwFlags = 0x0;

	if(NULL == hSnd)
		return E_FAIL;

	if(IsBadReadPtr((void*)hSnd, sizeof(SNDSTRUCT)))
		return E_FAIL;

	*dwFlags = hSnd->dwFlags;

	return S_OK;
}

/*************************************************************************************************************
 *
 * FUNCTION: xbSetFlags(SNDHANDLE, DWORD)
 *
 * PURPOSE:  Sets the flags that describe this sound
 *
 * USE:		 Use this to change information about the sound (like whether it is gonna loop or not, randomize, etc...)
 *
 *************************************************************************************************************/
		

HRESULT __stdcall xbSetFlags(void* hSound, DWORD dwFlags)
{
	SNDHANDLE hSnd = (SNDHANDLE)hSound;
	
	if(NULL == hSnd)
		return E_FAIL;

	if(IsBadReadPtr((void*)hSnd, sizeof(SNDSTRUCT)))
		return E_FAIL;

	hSnd->dwFlags = dwFlags;

	return S_OK;
}


//JJBUG-- Waiting for an XDK update...
HRESULT __stdcall xbCreateFX(DWORD dwFXFlags)
{
	HRESULT hr = S_OK;
//	DWORD	dwMixBin;

	if(dwFXFlags & FXAMPMOD)
	{
		//g_pDSnd->CreateFX(1,&dwFXFlags,dwMixBin,NULL,NULL);

	}

	if(dwFXFlags & FXCHORUS)
	{

	}

	if(dwFXFlags & FXCOMPRESSOR)
	{

	}

	if(dwFXFlags & FXDISTORTION)
	{

	}

	if(dwFXFlags & FXECHO)
	{

	}

	if(dwFXFlags & FXFLANGER)
	{

	}

	if(dwFXFlags & FXREVERB)
	{

	}




	return hr;
}

//JJBUG-- Waiting for an XDK update...
HRESULT __stdcall xbDeleteFX(DWORD dwFlags)
{
	HRESULT hr = S_OK;


	return hr;

}

//JJBUG-- Waiting for an XDK update...
HRESULT __stdcall xbApplyFX(void* hSound, DWORD dwFlags)
{
	HRESULT hr = S_OK;

	
	return hr;
}

//Listener API

/**************************************************************************************************************
 *
 * LISTENER API FUNCTIONS
 *
 * This collection of functions is here to immediately change the position, velocity, and orientation of the
 * listener.
 *
 **************************************************************************************************************/

HRESULT __stdcall xbSetListenerPosition(Vector v)
{
	HRESULT hr = S_OK;

	hr = g_pDSnd->SetPosition(v.x,v.y,v.z, DS3D_IMMEDIATE);

	//Only change our data if we were successful changing it for real
	if(SUCCEEDED(hr))
	{
		g_Listener.vPosition.x = v.x;
		g_Listener.vPosition.y = v.y;
		g_Listener.vPosition.z = v.z;
	}

	return hr;
}


HRESULT __stdcall xbSetListenerOrientation(Vector vFront, Vector vUp)
{
	HRESULT hr = S_OK;

	hr = g_pDSnd->SetOrientation(vFront.x, vFront.y, vFront.z, vUp.x, vUp.y, vUp.z,DS3D_IMMEDIATE);

	//If we succeeded, then update
	if(SUCCEEDED(hr))
	{
		g_Listener.vOrientFront.x = vFront.x;
		g_Listener.vOrientFront.y = vFront.y;
		g_Listener.vOrientFront.z = vFront.z;
		g_Listener.vOrientTop.x = vUp.x;
		g_Listener.vOrientTop.y = vUp.y;
		g_Listener.vOrientTop.z = vUp.z;
	}

	return hr;
}

HRESULT __stdcall xbSetListenerVelocity(Vector v)
{
	HRESULT hr = S_OK;

	hr = g_pDSnd->SetVelocity(v.x,v.y,v.z,DS3D_IMMEDIATE);

	if(SUCCEEDED(hr))
	{
		g_Listener.vVelocity.x = v.x;
		g_Listener.vVelocity.y = v.y;
		g_Listener.vVelocity.z = v.z;
	}

	return hr;
}



//Internally used utility functions

/********************************************************************************************************************
 *
 * UTILITY FUNCTIONS:
 *
 * PHSOUND SoundFromHandle(PSNDHANDLE);
 *
 ********************************************************************************************************************/


//Given a Handle, this gives a pointer to the actual structure data
inline PHSOUND SoundFromHandle(SNDHANDLE pHndl)
{
	DWORD	dwStatus = 0x0;
	int		nCounter = pHndl->nID;
	DWORD	dwFinish = 0xffffffff;
	int		nIDLast	 = pHndl->nID;
	DWORD	dwCurTime= 0xffffffff;
	int		nIterate = 0x0;

	dwCurTime = GetTickCount();

	//3D Sound case
	if(pHndl->dwFlags && THREEDSND)
	{
		//See if the buffer is in use
		g_SndArr[nCounter].pBuffer->GetStatus(&dwStatus);
			
		//Get a valid buffer that is not in use...
		while(dwStatus & DSBSTATUS_PLAYING)
		{
			g_SndArr[++nCounter%NUM3DSTRUCTS].pBuffer->GetStatus(&dwStatus);
			if(dwFinish > (g_SndArr[nCounter%NUM3DSTRUCTS]).dwEndTime - dwCurTime)
			{
				//Keep track of which one is closest to finishing
				dwFinish = (g_SndArr[nCounter%NUM3DSTRUCTS]).dwEndTime - dwCurTime;
				nIDLast  = nCounter%NUM3DSTRUCTS;
			}

			if(nIterate++ > NUM3DSTRUCTS)
			{
				//They are all in use...Stop this sound as it is closest to finishing
				g_SndArr[nIDLast].pBuffer->Stop();
				pHndl->nID = nIDLast;
				return &g_SndArr[nIDLast];
			}
		}

		pHndl->nID = nCounter%NUM3DSTRUCTS;

		return &(g_SndArr[nCounter%NUM3DSTRUCTS]);
	}else{
		//2D Sound case
		g_2DSndArr[nCounter].pBuffer->GetStatus(&dwStatus);

		//Get a valid buffer that is not in use...
		while(dwStatus & DSBSTATUS_PLAYING)
		{
			g_2DSndArr[++nCounter%NUM2DSTRUCTS];
			if(dwFinish > (g_2DSndArr[nCounter%NUM2DSTRUCTS]).dwEndTime - dwCurTime)
			{
				dwFinish = (g_2DSndArr[nCounter%NUM2DSTRUCTS]).dwEndTime - dwCurTime;
				nIDLast  = nCounter%NUM2DSTRUCTS;
			}

			if(nIterate++ > NUM2DSTRUCTS)
			{
				//Stop this sound
				g_2DSndArr[nIDLast].pBuffer->Stop();
				pHndl->nID = nIDLast;
				return &g_2DSndArr[nIDLast];
			}
		}

		pHndl->nID = nCounter%NUM2DSTRUCTS;

		return &(g_2DSndArr[nCounter%NUM2DSTRUCTS]);
	}
}


int GetFreeStream()
{
	DWORD dwStatus = 0x0;

	for(int i = 0; i < NUMMUSICHANDLES; i++)
	{
		if(!(g_MusicArr[i].bPlaying))
			return i;
	}

	//There is no free Handle
	return -1;
}



HVOICE GetVoiceStream()
{
	DWORD dwStatus = 0x0;

	for(int i = 0; i < NUMVOICEHANDLES; i++)
	{
		if(!(g_VoiceArr[i].bPlaying))
			return &g_VoiceArr[i];
	}
	return NULL;
}


void InitStreams()
{
	int i = 0;

	//Init the counters
	g_MUSICCNT = 0;
	g_nVoiceCnt= 0;
	
	g_lMusicVol = DSBVOLUME_MAX;
	g_lVoiceVol = DSBVOLUME_MAX;

	//Init the internal music handles
	for(i = 0; i < NUMMUSICHANDLES; i++)
	{
		g_MusicArr[i].bPlaying			= false;
		ZeroMemory(g_MusicArr[i].adwPacketStatus, sizeof(DWORD) * FILESTRM_PACKET_COUNT);
		g_MusicArr[i].dwFileLength		= 0;
		ZeroMemory(g_MusicArr[i].pvRenderBuffer, FILESTRM_RENDER_PACKET_BYTES*FILESTRM_PACKET_COUNT);
		ZeroMemory(g_MusicArr[i].pvSourceBuffer, FILESTRM_SOURCE_PACKET_BYTES);
		g_MusicArr[i].pSourceFilter		= NULL;
		//g_MusicArr[i].pTransformFilter	= NULL;
		g_MusicArr[i].pRenderFilter		= NULL;
		ZeroMemory(g_MusicArr[i].szFileName, 256);
		g_MusicArr[i].dwFileProgress	= 0x0;
	}

	//Init the handles for streaming voices
	for(i = 0; i < NUMVOICEHANDLES; i++)
	{
		g_VoiceArr[i].bPlaying			= false;
		ZeroMemory(g_VoiceArr[i].adwPacketStatus, sizeof(DWORD) * FILESTRM_PACKET_COUNT);
		g_VoiceArr[i].dwFileLength		= 0;
		ZeroMemory(g_VoiceArr[i].pvRenderBuffer, FILESTRM_RENDER_PACKET_BYTES*FILESTRM_PACKET_COUNT);
		ZeroMemory(g_VoiceArr[i].pvSourceBuffer, FILESTRM_SOURCE_PACKET_BYTES);
		g_VoiceArr[i].pSourceFilter		= NULL;
		//g_VoiceArr[i].pTransformFilter	= NULL;
		g_VoiceArr[i].pRenderFilter		= NULL;
		ZeroMemory(g_VoiceArr[i].szFileName, 256);
		g_VoiceArr[i].dwFileProgress	= 0x0;
		g_VoiceArr[i].lVol				= g_lVoiceVol;
	}

	//Init the stream handles for the music
	for(i = 0; i < NUMSTREAMS; i++)
	{
		g_StreamArr[i].dwFlags = 0;
		g_StreamArr[i].nID	    = 0;
		ZeroMemory(g_StreamArr[i].szFileName, 256);
	}

	//Init our WMA stream
	g_WMAStream.lVol = g_lMusicVol;
	g_WMAStream.bPlaying = false;
	g_WMAStream.dwFlags = 0x0;
	g_WMAStream.hsRef	= NULL;
	g_WMAStream.m_dwBytesRead = 0x0;
	g_WMAStream.m_dwMaxBufferCount = 0x0;
	g_WMAStream.m_dwMinPacketSize  = 0x0;
	g_WMAStream.m_dwPacketSize	   = 0x0;
	g_WMAStream.m_dwPercentCompleted = 0x0;
	g_WMAStream.m_dwSourceLength	= 0x0;
	g_WMAStream.m_hOpenResult		= S_OK;
	g_WMAStream.m_pSourceXMO		= NULL;
	g_WMAStream.m_pTargetXMO		= NULL;
	ZeroMemory(g_WMAStream.m_pTransferBuffer,MAXBUFSIZE);
	ZeroMemory(g_WMAStream.m_pPacketContextPool,PACKET_CNT * sizeof(MEDIA_PACKET_CTX));

}


HSTREAM GetCurrentMusic()
{
	int i = 0;
	for(i = 0; i < NUMMUSICHANDLES; i++)
	{
		if(g_MusicArr[i].bPlaying)
			return g_MusicArr[i].hsRef;
	}

	if(g_WMAStream.bPlaying)
		return g_WMAStream.hsRef;
	else
		return NULL;
}


/********************************************************************************************************************
 *
 * Streaming Functions (Music & Voice System)
 *
 * MHANDLE xbGetStreamHandle(char*);											//JJBUG -- TODO
 * HRESULT xbIsStreamValid(MHANDLE);											//JJBUG -- TODO
 * HRESULT xbPlayMusic(MHANDLE);												//JJBUG -- TODO
 * HRESULT xbContinueMusic(MHANDLE);											//JJBUG -- TODO
 * HRESULT xbFlourishMusic(MHANDLE);											//JJBUG -- TODO
 * HRESULT xbStopMusic(MHANDLE);												//JJBUG -- TODO
 * HRESULT xbSetMusicRequestCallback(MHANDLE, void callBackFunc(MHANDLE));		//JJBUG -- TODO
 * HRESULT xbSetMusicContinueCallback(MHANDLE, void callBackFunc(MHANDLE));		//JJBUG -- TODO
 * HRESULT xbPlayVoiceStream(MHANDLE);											//JJBUG -- TODO
 *
 *********************************************************************************************************************/

/***************************************************************************************************************
 *
 * FUCNTION:  xbSetMusicRequestCallback(void (WINAPI*)(void*))
 *
 * PURPOSE:	  This function takes in a function pointer that will be the Music Request Callback.
 *
 ****************************************************************************************************************/


HRESULT __stdcall xbSetMusicRequestCallback(void (WINAPI* pFunc)(void* hM))
{
	g_pRequestFunc = pFunc;
	return S_OK;
}

/***************************************************************************************************************
 *
 * FUCNTION:  xbSetMusicContinueCallback(void (WINAPI*)(void*))
 *
 * PURPOSE:	  This function takes in a function pointer that will be the Music Continue Callback.
 *
 ****************************************************************************************************************/

HRESULT __stdcall xbSetMusicContinueCallback(void (WINAPI* pFunc)(void* hM))
{
	g_pContinueFunc = pFunc;
	return S_OK;
}


HRESULT __stdcall xbContinueMusic(void* hMusic)
{
	HSTREAM	hStream = GetCurrentMusic();
	if(hStream)
		xbStopMusic((void*)hStream);

	return xbPlayMusic(hMusic);
}

HRESULT __stdcall xbFlourishMusic(void* hMusic)
{
	HSTREAM hStream = GetCurrentMusic();
	if(hStream)
		xbStopMusic((void*)hStream);

	return xbPlayMusic(hMusic);
}



/***************************************************************************************************************
 *
 * FUCNTION:  xbGetMusicStream(const char*, DWORD)
 *
 * PURPOSE:	  This function takes in the file name of the music file to be streamed, and flags.  This function
 *			  will return which ever of the 2 handles is not currently playing.  Once a piece is done playing it
 *			  is invalid.  The assumption here is that there will not ever be > 2 pieces of music playing at the
 *			  same time.
 *
 ****************************************************************************************************************/


void* __stdcall xbGetMusicStream(const char* szFileName, DWORD dwFlags)
{
	HRESULT hr		 = 0x0;
	int		i		 = 0;
	DWORD	dwStatus = 0x0;

	if(g_MUSICCNT < NUMSTREAMS)
	{
		g_StreamArr[g_MUSICCNT].dwFlags = dwFlags;
		assert(strlen(szFileName) < 256);
		strcpy(g_StreamArr[g_MUSICCNT].szFileName, szFileName);
		return &(g_StreamArr[g_MUSICCNT++]);
	}else
		return NULL;
}

HRESULT __stdcall xbPlayMusic(void* hStr)
{
	HRESULT hr		= S_OK;
	HSTREAM hStream = HSTREAM(hStr);
	int nID			= GetFreeStream();

	//If this is a WMA, then we do something totally different for now... this will change once hardware ADPCM is in place
	if(hStream->dwFlags & WMA)
	{
		//Stop our current stream
		if(g_WMAStream.bPlaying)
		{
			g_WMAStream.bPlaying = false;
			g_WMAStream.m_pTargetXMO->Flush();
			g_WMAStream.m_pSourceXMO->Flush();
			g_WMAStream.m_pSourceXMO->Release();
			g_WMAStream.m_pSourceXMO = NULL;
		}
		g_WMAStream.m_dwPercentCompleted = 0;
		g_WMAStream.Initialize(hStream->szFileName);
		g_WMAStream.bPlaying = true;
		g_WMAStream.dwFlags  = hStream->dwFlags;
		g_WMAStream.hsRef	 = hStream;
		if(g_WMAStream.m_pTargetXMO)
			hr = ((LPDIRECTSOUNDSTREAM)g_WMAStream.m_pTargetXMO)->SetVolume(g_lMusicVol);

		return hr;
	}

	hStream->nID = nID;
	strcpy(g_MusicArr[nID].szFileName, hStream->szFileName);
	g_MusicArr[nID].hsRef = hStream;
	g_MusicArr[nID].bDone = false;
	g_MusicArr[nID].dwFlags = hStream->dwFlags;

	//Set the proper volume

	g_MusicArr[nID].lVol = g_lMusicVol;

	hr = InitStream(hStream->szFileName, &g_MusicArr[nID], hStream->dwFlags);

	return hr;
}

HRESULT __stdcall xbSetMusicVolume(LONG lVol)
{
	g_lMusicVol = lVol;
	return S_OK;
}

HRESULT __stdcall xbStopMusic(void* hStr)
{
	HSTREAM hStream = (HSTREAM)(hStr);

	//Make sure that we are stopping the right thing...
	//Special Case the WMA for now
	if(hStream->dwFlags & WMA)
	{
		g_WMAStream.bPlaying = false;
		g_WMAStream.m_pSourceXMO->Flush();
		g_WMAStream.m_pTargetXMO->Flush();
		return S_OK;
	}


	if(0 == strcmp(hStream->szFileName, g_MusicArr[hStream->nID].szFileName))
	{
		g_MusicArr[hStream->nID].bPlaying = false;
		g_MusicArr[hStream->nID].pRenderFilter->Flush();
	}

	return S_OK;
}

void* __stdcall xbPlayVoiceStream(char* szFileName, bool bDipMusic, PVOICE_INFO pVI, DWORD dwFlags)
{
	HVOICE hVoice	= GetVoiceStream();
	
	//Remember to raise the volume when the music is done...
	if(bDipMusic)
	{
		g_lDipVol = g_lMusicVol;
		xbSetMusicVolume(DSBVOLUME_MIN/5);
	}

	if(hVoice)
	{
		
		strcpy(hVoice->szFileName, szFileName);
		hVoice->hsRef = NULL;
		hVoice->bDone = false;
		hVoice->dwFlags = dwFlags;

		g_nVoiceCnt++;
		strcpy(hVoice->szFileName, szFileName);
		InitStream(szFileName, hVoice, dwFlags);
		hVoice->pRenderFilter->SetPosition(pVI->pPosition.x,pVI->pPosition.y,pVI->pPosition.z, DS3D_IMMEDIATE);
		hVoice->pRenderFilter->SetVelocity(pVI->pVelocity.x,pVI->pVelocity.y,pVI->pVelocity.z, DS3D_IMMEDIATE);
		hVoice->pRenderFilter->SetConeOutsideVolume(pVI->lOuterVolume,DS3D_IMMEDIATE);
		hVoice->pRenderFilter->SetConeOrientation(pVI->vConeOrientation.x,pVI->vConeOrientation.y,pVI->vConeOrientation.z,DS3D_IMMEDIATE);
		hVoice->pRenderFilter->SetConeAngles(pVI->dwInConeAngle,pVI->dwOutConeAngle,DS3D_IMMEDIATE);
		hVoice->pRenderFilter->SetVolume(g_lVoiceVol);
	}


	return (void*)hVoice;
}

HRESULT __stdcall xbPauseVoices()
{
	g_bVoicePause = true;
	return S_OK;

}

HRESULT __stdcall xbResumeVoices()
{
	g_bVoicePause = false;
	return S_OK;
}

HRESULT __stdcall xbPauseMusic()
{
	g_bMusicPause = true;
	return S_OK;
}

HRESULT __stdcall xbResumeMusic()
{
	g_bMusicPause = false;
	return S_OK;
}

HRESULT __stdcall xbPauseAllStreams()
{
	xbPauseVoices();
	xbPauseMusic();
	return S_OK;
}

HRESULT __stdcall xbResumeAllStreams()
{
	xbResumeVoices();
	xbResumeMusic();
	return S_OK;
}

HRESULT __stdcall xbSetVoicePostion(void* hv, Vector v)
{
	HVOICE hVoice = (HVOICE)hv;

	if(!(hVoice->bPlaying))
		return E_FAIL;

	return hVoice->pRenderFilter->SetPosition(v.x,v.y,v.z,DS3D_IMMEDIATE);
}

HRESULT __stdcall xbSetVoiceVelocity(void* hv, Vector v)
{
	HVOICE hVoice = (HVOICE)hv;
	
	if(!(hVoice->bPlaying))
		return E_FAIL;

	return hVoice->pRenderFilter->SetVelocity(v.x,v.y,v.z,DS3D_IMMEDIATE);
}

HRESULT __stdcall xbSetVoiceCone(void* hv ,DWORD dwIn ,DWORD dwOut,Vector vOrientation,LONG lOutVol)
{
	HVOICE hVoice = (HVOICE)hv;

	HRESULT hr	  = S_OK;

	//Make sure that it is currently playing
	if(!(hVoice->bPlaying))
		return E_FAIL;

	hr = hVoice->pRenderFilter->SetConeOutsideVolume(lOutVol,DS3D_IMMEDIATE);
	assert(SUCCEEDED(hr));

	hr = hVoice->pRenderFilter->SetConeAngles(dwIn,dwOut,DS3D_IMMEDIATE);
	assert(SUCCEEDED(hr));

	hr = hVoice->pRenderFilter->SetConeOrientation(vOrientation.x,vOrientation.y,vOrientation.z,DS3D_IMMEDIATE);
	assert(SUCCEEDED(hr));

	return hr;
}

void inline __stdcall VoiceDone()
{
	//If there are no more active voices, put the music back to whereit was
	if(--g_nVoiceCnt == 0)
		xbSetMusicVolume(g_lDipVol);
}
/**********************************************************************************************************
 *
 * FUNCTION:  xbProcess()
 *
 * PURPOSE:	  Call this in the Render loop to continue streaming anything that needs to be streamed.
 *
 **********************************************************************************************************/

HRESULT __stdcall xbProcess()
{
	
	int i = 0;
	DWORD dwPercentComplete = 0x0;
	HRESULT hr = S_OK;

	//If we have paused all streams, don't process anything
	if(g_bMusicPause && g_bVoicePause)
		return S_OK;


	if(!g_bVoicePause)
	{
		for(i = 0; i < NUMVOICEHANDLES; i++)
		{
			if(g_VoiceArr[i].bPlaying)
			{
				if(g_VoiceArr[i].bDone)
				{
					g_VoiceArr[i].pRenderFilter->Flush();
					g_VoiceArr[i].pSourceFilter->Flush();
					//g_VoiceArr[i].pTransformFilter->Flush();
					g_VoiceArr[i].bPlaying = false;
				}else{
					if(g_lVoiceVol != g_VoiceArr[i].lVol)
					{
						g_VoiceArr[i].pRenderFilter->SetVolume(g_lVoiceVol);
						g_VoiceArr[i].lVol = g_lVoiceVol;
					}

					hr = Process(&dwPercentComplete, &g_VoiceArr[i]);
					assert(SUCCEEDED(hr));
				}
			}
		}
	}


	if(!g_bMusicPause)
	{
		for(i = 0; i < NUMMUSICHANDLES; i++)
		{
			if(g_MusicArr[i].bPlaying)
			{
				if(g_MusicArr[i].bDone)
				{
					xbStopMusic(g_MusicArr[i].hsRef);
				}else{
					if(g_lMusicVol != g_MusicArr[i].lVol)
					{
						g_MusicArr[i].pRenderFilter->SetVolume(g_lMusicVol);
						g_MusicArr[i].lVol = g_lMusicVol;
					}

					hr = Process(&dwPercentComplete, &g_MusicArr[i]);
					assert(SUCCEEDED(hr));
				}
			}
		}


		if(g_WMAStream.bPlaying)
		{
			if( !FAILED(g_WMAStream.m_hOpenResult) ) 
			{
			
				// Instruct routine to poll the filter graph so completed packets can be
				// moved through the streaming chain.

				 //If the volume changed, change it!
				if(g_lMusicVol != g_WMAStream.lVol)
				{
					 ((IDirectSoundStream*)(g_WMAStream.m_pTargetXMO))->SetVolume(g_lMusicVol);
					g_WMAStream.lVol = g_lMusicVol;
				}

				if( FAILED( g_WMAStream.TransferData() ) )
					return E_FAIL;

			
				// Because of compression, we have to ask the filter how much
				// we've processed
				g_WMAStream.m_pSourceXMO->Seek(0,FILE_CURRENT,&g_WMAStream.m_dwBytesRead);

				if(100 == g_WMAStream.m_dwPercentCompleted)
				{
					xbStopMusic(g_WMAStream.hsRef);

					if(NULL != g_pContinueFunc)
						g_pContinueFunc(g_WMAStream.hsRef);
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////THIS STUFF WAS TAKEN FROM A SAMPLE-- It is software ADPCM Decompression... it will go away////////////
//-----------------------------------------------------------------------------
// Debugging help
//-----------------------------------------------------------------------------
#define MY_ASSERT(x) if( !(x) ) _asm int 3;

BOOL g_bDoDebugChecks = TRUE;

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the wave file streaming subsystem.
//-----------------------------------------------------------------------------
HRESULT InitStream(const CHAR* strFileName, HMUSIC hMusic, DWORD dwFlags)
{
    LPCWAVEFORMATEX pwfxSourceFormat;
    WAVEFORMATEX    wfxRendererFormat;
    DSSTREAMDESC    dssd;
    HRESULT         hr;
    
    // Create the source (wave file) filter

	//Don't Leak Anything!
	if(hMusic->pSourceFilter)
	{
		hMusic->pSourceFilter->Release();
		hMusic->pSourceFilter = NULL;
	}

    hr = XWaveFileCreateMediaObject(strFileName, &pwfxSourceFormat, &hMusic->pSourceFilter);
    if(FAILED(hr))
        return hr;

    // The file we're opening needs to be IMA ADPCM
//    MY_ASSERT( WAVE_FORMAT_IMA_ADPCM == pwfxSourceFormat->wFormatTag );

    // Create the transform (ADPCM decompressor) filter

	//Don't Leak anything!
	//if(hMusic->pTransformFilter)
	//{
		//hMusic->pTransformFilter->Release();
		//hMusic->pTransformFilter = NULL;
	//}

    //hr = AdpcmCreateDecoder( (LPCIMAADPCMWAVEFORMAT)pwfxSourceFormat, 
    //                         &wfxRendererFormat, &hMusic->pTransformFilter );
    //if( FAILED(hr) )
    //    return hr;

    // Create the render (DirectSoundStream) filter
    ZeroMemory( &dssd, sizeof(dssd) );
    //dssd.dwSize               = sizeof(dssd);
    dssd.dwMaxAttachedPackets = FILESTRM_PACKET_COUNT;
    dssd.lpwfxFormat          = &wfxRendererFormat;
	dssd.dwFlags			  = DSBCAPS_CTRLVOLUME;

	if(hMusic->dwFlags & ISVOICE)
		dssd.dwFlags |= DSBCAPS_CTRL3D;

	if(hMusic->pRenderFilter)
	{
		hMusic->pRenderFilter->Release();
		hMusic->pRenderFilter = NULL;
	}

    hr = DirectSoundCreateStream(&dssd, &hMusic->pRenderFilter);
    if( FAILED(hr) )
        return hr;

	//Set the Volume
	hMusic->pRenderFilter->SetVolume(hMusic->lVol);

//    if( g_bDoDebugChecks )
//    {

//#ifdef DEBUG

        XMEDIAINFO xmi;

        // We expect the source filter to be synchronous and read-only, the
        // transform filter to be synchronous and read/write and the render
        // filter to be asynchronous write-only.  Assert that all of this 
        // is true and check the packet sizes for compatibility.

        hr = hMusic->pSourceFilter->GetInfo( &xmi );
        if( FAILED(hr) )
            return hr;
        MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE) );
        MY_ASSERT( !xmi.dwMaxLookahead );
        MY_ASSERT( !xmi.dwInputSize );
        MY_ASSERT( !(FILESTRM_SOURCE_PACKET_BYTES % xmi.dwOutputSize) );

        //hr = hMusic->pTransformFilter->GetInfo( &xmi );
        //if( FAILED(hr) )
        //    return hr;
        //MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE) );
        //MY_ASSERT( !xmi.dwMaxLookahead );
        //MY_ASSERT( !(FILESTRM_SOURCE_PACKET_BYTES % xmi.dwInputSize) );
//        MY_ASSERT( !(FILESTRM_RENDER_PACKET_BYTES % xmi.dwOutputSize) );

        hr = hMusic->pRenderFilter->GetInfo( &xmi );
        if( FAILED(hr) )
            return hr;
        MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC) );
        MY_ASSERT( FILESTRM_RENDER_PACKET_BYTES * FILESTRM_PACKET_COUNT >= xmi.dwMaxLookahead );
        MY_ASSERT( !(FILESTRM_RENDER_PACKET_BYTES % xmi.dwInputSize) );
        MY_ASSERT( !xmi.dwOutputSize );
   // }

//#endif

    // Get the total wave file size.  We'll use this to calculate how far
    // along in the file we are during processing.
    hr = hMusic->pSourceFilter->GetLength( &hMusic->dwFileLength );
    if( FAILED(hr) )
        return hr;

	//Start Processing this music
	hMusic->bPlaying = true;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Performs any work necessary to keep the stream playing.
//-----------------------------------------------------------------------------
HRESULT Process(DWORD* pdwPercentCompleted, HMUSIC hMusic)
{
    DWORD   dwPacketIndex;
    HRESULT hr;
    
    // Find a free packet.  If there's none free, we don't have anything
    // to do
    while( FindFreePacket(&dwPacketIndex, hMusic) )
    {
        // Read from the source filter
        hr = ProcessSource(hMusic);
        if( FAILED(hr) )
            return hr;
        
        // Pass the data through the transform
        hr = ProcessTransform(dwPacketIndex, hMusic);
        if( FAILED(hr) )
            return hr;

        // Send the data to the renderer
        hr = ProcessRenderer(dwPacketIndex, hMusic);
        if( FAILED(hr) )
            return hr;
    }

	//If it is a streamed voice, we don't need the request/continue callbacks
	if(!(hMusic->dwFlags & ISVOICE))
	{

	    // Calculate the completion percentage based on the total amount of
	    // data we've read from the source.
		if(pdwPercentCompleted )
        (*pdwPercentCompleted) = hMusic->dwFileProgress * 100 / hMusic->dwFileLength;


		//Use the callbacks
		if((*pdwPercentCompleted > 75) && (*pdwPercentCompleted < 100) && (NULL != g_pRequestFunc))
			g_pRequestFunc(hMusic->hsRef);
		
		if(hMusic->bDone && (NULL != g_pContinueFunc))//(*pdwPercentCompleted == 100) && (NULL != g_pContinueFunc))
			g_pContinueFunc(hMusic->hsRef);
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FindFreePacket()
// Desc: Finds a render packet available for processing.
//-----------------------------------------------------------------------------
BOOL FindFreePacket(DWORD* pdwPacketIndex, HMUSIC hMusic)
{
    for( DWORD dwPacketIndex = 0; dwPacketIndex < FILESTRM_PACKET_COUNT; dwPacketIndex++ )
    {
        if( XMEDIAPACKET_STATUS_PENDING != hMusic->adwPacketStatus[dwPacketIndex] )
        {
            if( pdwPacketIndex )
                (*pdwPacketIndex) = dwPacketIndex;

            return TRUE;
        }
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: ProcessSource()
// Desc: Reads data from the source filter.
//-----------------------------------------------------------------------------
HRESULT ProcessSource(HMUSIC hMusic)
{
    DWORD        dwTotalSourceUsed   = 0;
    DWORD        dwSourceUsed;
    XMEDIAPACKET xmp;
    HRESULT      hr;
    
    // We're going to read a full packet's worth of data into the source
    // buffer.  Since we're playing in an infinite loop, we'll just spin
    // until we've read enough data, even if that means wrapping around the
    // end of the file.

    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer         = hMusic->pvSourceBuffer;
    xmp.dwMaxSize        = FILESTRM_SOURCE_PACKET_BYTES;
    xmp.pdwCompletedSize = &dwSourceUsed;

    while( dwTotalSourceUsed < FILESTRM_SOURCE_PACKET_BYTES )
    {
        // Read from the source
        hr = hMusic->pSourceFilter->Process(NULL, &xmp);
        if( FAILED(hr) )
            return hr;

        // Add the amount read to the total
        dwTotalSourceUsed = dwSourceUsed;

        // If we read less than the amount requested, it's because we hit
        // the end of the file.  Seek back to the start and keep going.
        if( dwSourceUsed < xmp.dwMaxSize )
        {
            xmp.pvBuffer  = (BYTE*)xmp.pvBuffer + dwSourceUsed;
            xmp.dwMaxSize = xmp.dwMaxSize - dwSourceUsed;
            
			if(!(hMusic->dwFlags & LOOPING))
			{
				hMusic->bDone = true;
				if(hMusic->dwFlags & ISVOICE)
				{
					VoiceDone();
				}
				return S_OK;
			}

            hr = hMusic->pSourceFilter->Seek(0,FILE_BEGIN,NULL);
            if( FAILED(hr) )
                return hr;
        };
    }

    // Update the file progress
    hMusic->dwFileProgress += dwTotalSourceUsed;
    hMusic->dwFileProgress %= hMusic->dwFileLength;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ProcessTransform()
// Desc: Transforms data in the source buffer into the render buffer.
//-----------------------------------------------------------------------------
HRESULT ProcessTransform(DWORD dwPacketIndex, HMUSIC hMusic)
{
    XMEDIAPACKET xmpSource;
    XMEDIAPACKET xmpRender;
    DWORD        dwSourceUsed;
    DWORD        dwRenderUsed;
    
    // The source buffer is full and we're ready to transform it's data into
    // the render buffer
    ZeroMemory( &xmpSource, sizeof(xmpSource) );
    xmpSource.pvBuffer         = hMusic->pvSourceBuffer;
    xmpSource.dwMaxSize        = FILESTRM_SOURCE_PACKET_BYTES;
    xmpSource.pdwCompletedSize = &dwSourceUsed;

    ZeroMemory( &xmpRender, sizeof(xmpRender) );
    xmpRender.pvBuffer         = (BYTE*)hMusic->pvRenderBuffer + (dwPacketIndex * FILESTRM_RENDER_PACKET_BYTES);
    xmpRender.dwMaxSize        = FILESTRM_RENDER_PACKET_BYTES;
    xmpRender.pdwCompletedSize = &dwRenderUsed;

    //hr = hMusic->pTransformFilter->Process(&xmpSource,&xmpRender);
    //if( FAILED(hr) )
    //    return hr;

     MY_ASSERT( FILESTRM_SOURCE_PACKET_BYTES == dwSourceUsed );
//    MY_ASSERT( FILESTRM_RENDER_PACKET_BYTES == dwRenderUsed );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ProcessRenderer()
// Desc: Sends data to the renderer.
//-----------------------------------------------------------------------------
HRESULT ProcessRenderer(DWORD dwPacketIndex, HMUSIC hMusic)
{
    XMEDIAPACKET xmp;
    HRESULT      hr;

    // There's a full packet's worth of data ready for us to send to the
    // renderer.  We want to track the status of this packet since the
    // render filter is asychronous and we need to know when the packet is
    // completed.
    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer  = (BYTE*)hMusic->pvRenderBuffer + (dwPacketIndex * FILESTRM_RENDER_PACKET_BYTES);
    xmp.dwMaxSize = FILESTRM_RENDER_PACKET_BYTES;
    xmp.pdwStatus = &hMusic->adwPacketStatus[dwPacketIndex];

    hr = hMusic->pRenderFilter->Process( &xmp, NULL );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}

///////////////////////////////////////////WMA STUFF//////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// File: AsyncXMO.cpp
//
// Desc: A sample to show how to stream data between 3 XMOs( 1 async source, 
//       one sychronous in the middle, one async target.
//
// Hist: 02.20.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

//
// Debug macros
//
#define ASSERT(w) {if(w){}}
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp)
#define DBG_ERROR(_exp)
#define DBG_WARN(_exp)


//
// Simple macro to verify that the packet size is OK for a particular XMO
//
inline HRESULT XMOFILTER_VERIFY_PACKET_SIZE( XMEDIAINFO& xMediaInfo, DWORD dwPacketSize )
{
    if( xMediaInfo.dwOutputSize > dwPacketSize )
        return E_FAIL;
    if( xMediaInfo.dwOutputSize ) 
        if( dwPacketSize % xMediaInfo.dwOutputSize != 0 )
            return E_FAIL;
    if( dwPacketSize < xMediaInfo.dwMaxLookahead )
        return E_FAIL;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: AttachPacketToSourceXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       source XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the source XMO (first xmo in the chain)
    hr = m_pSourceXMO->Process( NULL, &xmb );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our source list
    m_SourcePendingList.Add( &pCtx->ListEntry );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: AttachPacketToTargetXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       target XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer         
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the target XMO (first xmo in the chain)
    hr = m_pTargetXMO->Process( &xmb, NULL );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our target list
    m_TargetPendingList.Add( &pCtx->ListEntry );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Initialize
//
// Desc: Creates a streaming graph between 3 XMOs: a source, a in-between
//       (codec in this case) and a target. We only support one intermediate
//       xmo, but you could actually have multiple ones that manipulate the
//       same stream passing packets between them
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::Initialize(char* szFileName)
{
    HRESULT        hr = S_OK;
    DWORD          dwMinSize;
    XMEDIAINFO     xMediaInfo;
    DSSTREAMDESC   dssd;

    m_dwBytesRead = 0;
    m_dwPacketSize     = PACKET_SIZE;
    m_dwMaxBufferCount = PACKET_CNT;

	//Make sure we don't leak anything...
	if(m_pSourceXMO)
	{
		m_pSourceXMO->Release();
		m_pSourceXMO = NULL;
	}

    // Create src media object
	m_hOpenResult = WmaCreateDecoder(szFileName,//"D:\\MEDIA\\sound\\test.wma",
		                             NULL,
			                         TRUE,
				                     4096*16,
                                     16,    //max packets, let's guess 16 for now
                                     1,     //always yield, does this work?
					                 &m_wfxAudioFormat,
						             (XFileMediaObject **)&m_pSourceXMO );


    if( FAILED(m_hOpenResult) ) 
    {
        // If we failed to create a decoder, return success so we keep running.
        // FrameMove and render will check this error code and display an error
        // message instead of attempting to process the WMA
        DBG_ERROR( ("XmoFilter: WmaCreateDecoder failed with %x", m_hOpenResult) );
        return S_OK;
    }

    m_pSourceXMO->GetLength( &m_dwSourceLength );

    // Now create the target XMO, the end part of the streaming chain
    ZeroMemory( &dssd, sizeof(dssd) );
    //dssd.dwSize               = sizeof(dssd);
    dssd.dwFlags              = DSBCAPS_CTRLVOLUME;
    dssd.dwMaxAttachedPackets = PACKET_CNT;
    dssd.lpwfxFormat          = &m_wfxAudioFormat;

	//NO LEAKING!!!
	if(!m_pTargetXMO)
    {
		hr = DirectSoundCreateStream(&dssd, (LPDIRECTSOUNDSTREAM*)&m_pTargetXMO);
	}

    if( FAILED(hr) ) 
    {          
        DBG_ERROR( ("XmoFilter: DirectSoundCreateXMOStream failed with %x", hr) );
        DBG_ERROR( ("Init: Could not open a target audio XMO\n") );
        return hr;
    }

    //
    // At this point we should have all the XMOs.
    // Figure out the minimum packet size we need to pass media buffers
    // in between XMOs. All we do is verifying that our size (PACKET_SIZE)
    // is both larger than the min of all XMOs and its length aligned.
    // As a last check we make sure its also larger than the maxLookahead
    //
    // At the end of all this, m_dwMinPacketSize is the minimum packet size
    // for this filter graph.
    //
    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    if( m_pSourceXMO )
    {
        m_pSourceXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = xMediaInfo.dwOutputSize;

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    if( m_pTargetXMO )
    {
        m_pTargetXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = max( xMediaInfo.dwOutputSize, m_dwMinPacketSize );

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    ASSERT( m_pTargetXMO );
    ASSERT( m_pSourceXMO );

    // Initialize the linked lists
    m_SourcePendingList.Initialize();
    m_TargetPendingList.Initialize();

    //
    // Allocate one large buffer for incoming/outgoing data. The buffer is
    // going to be divided among our media context packets, which will be 
    // used to stream data between XMOs
    //
    dwMinSize = MAXBUFSIZE;
    //m_pTransferBuffer = new BYTE[dwMinSize];
    ZeroMemory( m_pTransferBuffer, dwMinSize );

    //
    // Allocate a context pool. A context is associated with a packet buffer
    // and used to track the pending packets attached to the source or target
    // XMO.
    dwMinSize = ( PACKET_CNT * sizeof(MEDIA_PACKET_CTX) );
    //m_pPacketContextPool = new BYTE[dwMinSize];
    ZeroMemory( m_pPacketContextPool, dwMinSize );

    //
    // We attach all available packet buffers in the beginning to prime the
    // graph: As each packet is completed by the source XMO, the status is
    // udpated in its packet context and we pass the packet to the next XMO.
    //
    for( DWORD i = 0; i < PACKET_CNT; i++ ) 
    {
        MEDIA_PACKET_CTX* pCtx = &((MEDIA_PACKET_CTX*)m_pPacketContextPool)[i];

        ZeroMemory( pCtx, sizeof(MEDIA_PACKET_CTX) );
        pCtx->dwCompletedSize = m_dwPacketSize;
        pCtx->pBuffer         = &m_pTransferBuffer[i*PACKET_SIZE];

        AttachPacketToSourceXMO( pCtx );
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Clean up resources
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::Cleanup()
{
    // Cleanup: delete any XMO's
    if( m_pSourceXMO )       
        m_pSourceXMO->Release();
    if( m_pTargetXMO )       
        m_pTargetXMO->Release();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates, such as checking async xmo status, and
//       updating source file position for our progress bar
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::FrameMove()
{
   if( !FAILED(m_hOpenResult) ) 
   {
        // Instruct routine to poll the filter graph so completed packets can be
        // moved through the streaming chain.
        if( FAILED( TransferData() ) )
            return E_FAIL;

        // Because of compression, we have to ask the filter how much
        // we've processed
        m_pSourceXMO->Seek( 0, FILE_CURRENT, &m_dwBytesRead  );
   }
        
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: TransferData
// Desc: If we have a packet completed from our source XMO, pass it through
//       the intermediate XMO and on to the target.
//       If we have a packet completed from our target XMO, recycle it back
//       to the source XMO.
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::TransferData()
{
    HRESULT        hr=S_OK;
    DWORD          index = 0;
    XMEDIAPACKET   xmb;
    MEDIA_PACKET_CTX* pSrcCtx = NULL;
    MEDIA_PACKET_CTX* pDstCtx = NULL;
    DWORD             dwSize  = 0;

    // Setup media buffer
    ZeroMemory( &xmb, sizeof(xmb) );

    //
    // Always deal with the oldest packet first.  Oldest packets are at the 
    // head, since we insert at the tail, and remove from head
    //
    if( !m_SourcePendingList.IsListEmpty() ) 
    {
        pSrcCtx = (MEDIA_PACKET_CTX*)m_SourcePendingList.pNext; // Head
    }

    if( !m_TargetPendingList.IsListEmpty() ) 
    {
        pDstCtx = (MEDIA_PACKET_CTX*)m_TargetPendingList.pNext; // Head
    }        

    // Do we have a completed source packet?
    if( pSrcCtx && pSrcCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING ) 
    {
        // A src buffer completed.
        // Remove this context from the Src pending list.
        // Send it to a codec, if present, then send it to the Target xmo
        pSrcCtx->ListEntry.Remove();

        // Before we pass it on, set the context packet to amount of data we received
        xmb.pvBuffer         = pSrcCtx->pBuffer;            
        xmb.dwMaxSize        = pSrcCtx->dwCompletedSize;

        DBG_SPAM(("Context %x received %x bytes, xmb %x\n",pSrcCtx, xmb.dwMaxSize, &xmb));
        ASSERT( xmb.dwMaxSize );

        // Rset the state of our context
        pSrcCtx->dwCompletedSize = 0;
        pSrcCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

        // Remember how much data we got from the source XMO
        dwSize = xmb.dwMaxSize;

        pSrcCtx->dwCompletedSize = m_dwPacketSize;

        if (dwSize < PACKET_SIZE) {
    
            //
            // the source xmo produced less than asked. this signals end of file for wma xmo
            // flush it so it starts at the begininning again...
            // Pass at least a sample worth of data to dsound so it will not RIP
            //
			if(!(g_WMAStream.dwFlags & LOOPING))
			{
				m_pSourceXMO->Flush();
				pSrcCtx->dwCompletedSize = max(4,dwSize);
				m_dwPercentCompleted = 100;
			}else{
				m_pSourceXMO->Flush();
				pSrcCtx->dwCompletedSize = max(4,dwSize);
			}

        }

        // Pass media buffer to the target xmo.
        AttachPacketToTargetXMO( pSrcCtx );
    }

    // Check if we have a completed packet from the target
    if( pDstCtx && pDstCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING )
    {       
        DBG_SPAM(("Target XMO completed context %x\n",pDstCtx));

        // Remove the entry from the list
        pDstCtx->ListEntry.Remove();

        ASSERT(pDstCtx->dwCompletedSize != 0);

        // The target XMO is done with a buffer.
        pDstCtx->dwCompletedSize = PACKET_SIZE;
        AttachPacketToSourceXMO( pDstCtx );
    } 

    return S_OK;
}





//-----------------------------------------------------------------------------

// Name: DownloadScratch

// Desc: Downloads a DSP scratch image to the DSP

//-----------------------------------------------------------------------------

HRESULT DownloadScratch(IDirectSound8* pDSnd, PCHAR pszScratchFile)
{

    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC pIDesc;

    // open scratch image file generated by xps2 tool
    hFile = CreateFile( pszScratchFile,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

    if( hFile == INVALID_HANDLE_VALUE )
    {
        DWORD err = GetLastError();

//        OutputDebugString( _T("Failed to open the dsp image file.\n") );
        hr = HRESULT_FROM_WIN32(err);
    }

    if( SUCCEEDED(hr) )
    {
        // Determine the size of the scratch image by seeking to
        // the end of the file
        dwSize = SetFilePointer( hFile, 0, NULL, FILE_END );
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

        // Allocate memory to read the scratch image from disk
        pBuffer = new BYTE[dwSize];

        // Read the image in
        DWORD dwBytesRead;
        BOOL bResult = ReadFile( hFile,
                                 pBuffer,
                                 dwSize,
                                 &dwBytesRead,
                                 0 );

        if (!bResult)
        {
            DWORD err = GetLastError();

            //      OutputDebugString( _T("\n Failed to open the dsp image file.\n") );
            hr = HRESULT_FROM_WIN32(err);
        }
    }

    if( SUCCEEDED(hr) )
    {
        // call dsound api to download the image..
        // call dsound api to download the image..
        pIDesc.dwI3DL2ReverbIndex = DSFX_IMAGELOC_UNUSED;//0;//I3DL2_CHAIN_I3DL2_REVERB;
        pIDesc.dwCrosstalkIndex = 1;//I3DL2_CHAIN_XTALK;

        if (SUCCEEDED(hr))
        {
            hr = pDSnd->DownloadEffectsImage( pBuffer,
                                              dwSize,
                                              NULL,
                                              &pDesc );
        }
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }

    return hr;
}

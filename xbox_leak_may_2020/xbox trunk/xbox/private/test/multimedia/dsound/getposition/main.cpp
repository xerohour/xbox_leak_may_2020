#include <xtl.h>
#include <xdbg.h>
#include <dsound.h>
 
void 
__cdecl 
main(void)
{
	DWORD				i;
	HRESULT				hr, hr1, hr2;
	LPDIRECTSOUND8		ds;
	LPDIRECTSOUNDBUFFER	dsb1, dsb2;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
	LPVOID				lpvwr;
	DWORD				len;
	DWORD				rp_byte1, rp_byte2, wr_byte1, wr_byte2;
	DWORD				stat1, stat2;

	hr = DirectSoundCreate(NULL, &ds, NULL);
	if FAILED(hr) 
	{
		DbgPrint("Can not create DirectSound.\n");
		return;
	}

	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= 2;
	wfx.nSamplesPerSec	= 48000;
	wfx.nBlockAlign		= 4;
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample	= 16;

	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCDEFER;
	dsbd.dwBufferBytes	= wfx.nAvgBytesPerSec;
	dsbd.lpwfxFormat	= (LPWAVEFORMATEX)&wfx;

	hr = ds->CreateSoundBuffer(&dsbd, &dsb1, NULL);
	if FAILED(hr) 
	{
		DbgPrint("Can not create DirectSoundBuffer.\n");
		return;
	}
	hr = ds->CreateSoundBuffer(&dsbd, &dsb2, NULL);
	if FAILED(hr) 
	{
		DbgPrint("Can not create DirectSoundBuffer.\n");
		return;
	}

	hr = dsb1->Lock(0, 0, &lpvwr, &len, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if FAILED(hr) 
	{
		DbgPrint("Can not lock DirectSoundBuffer.\n");
		return;
	}
	ZeroMemory(lpvwr, len);
	dsb1->Unlock(lpvwr, len, NULL, 0);
	hr = dsb2->Lock(0, 0, &lpvwr, &len, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if FAILED(hr) 
	{
		DbgPrint("Can not lock DirectSoundBuffer.\n");
		return;
	}
	ZeroMemory(lpvwr, len);
	dsb2->Unlock(lpvwr, len, NULL, 0);

	hr1 = dsb1->Play(0, 0, DSBPLAY_LOOPING);
	hr2 = dsb2->Play(0, 0, DSBPLAY_LOOPING);
	if ((hr1 != DS_OK) || (hr2 != DS_OK)) 
	{
		DbgPrint("Can not play DirectSoundBuffer.\n");
		return;
	}

	for (;;) 
	{
		for (i = 0; i < 10; i++) 
		{
			hr1 = IDirectSoundBuffer_GetCurrentPosition(dsb1, &rp_byte1, &wr_byte1);
			hr2 = IDirectSoundBuffer_GetCurrentPosition(dsb2, &rp_byte2, &wr_byte2);
			if ((hr1 != DS_OK) || (hr2 != DS_OK)) 
			{
				DbgPrint("Fail to GetCurrentPosition\n");
				continue;
			}

			DbgPrint("%d, %d, %d, %d\n", rp_byte1, wr_byte1, rp_byte2, wr_byte2);


			/* Workaround */
//			Sleep(10);
//			DirectSoundDoWork();
		}
	
		hr1 = IDirectSoundBuffer_Stop(dsb1);
		hr2 = IDirectSoundBuffer_Stop(dsb2);
		if ((hr1 != DS_OK) || (hr2 != DS_OK)) 
			DbgPrint("Fail to Stop\n");
//		DirectSoundDoWork();
		for (;;) 
		{
			hr1 = IDirectSoundBuffer_GetStatus(dsb1, &stat1);
			hr2 = IDirectSoundBuffer_GetStatus(dsb2, &stat2);
			if ((hr1 != DS_OK) || (hr2 != DS_OK)) 
			{
//				DirectSoundDoWork();
				continue;
			}
			if ((stat1 == 0) && (stat2 == 0)) 
				break;
		}

		Sleep(1);
//		DirectSoundDoWork();

		hr1 = IDirectSoundBuffer_Play(dsb1, 0, 0, DSBPLAY_LOOPING);
		hr2 = IDirectSoundBuffer_Play(dsb2, 0, 0, DSBPLAY_LOOPING);
		if ((hr1 != DS_OK) || (hr2 != DS_OK)) 
			DbgPrint("Fail to Play\n");

		for (;;) 
		{
			hr1 = IDirectSoundBuffer_GetStatus(dsb1, &stat1);
			hr2 = IDirectSoundBuffer_GetStatus(dsb2, &stat2);
			if ((hr1 != DS_OK) || (hr2 != DS_OK)) 
				continue;
			if ((stat1 != 0) && (stat2 != 0)) 
				break;
		}
	}

	return;
}

#define INITGUID
#include <xtl.h>
#include <waveldr.h>
#include "xsound.h"

typedef struct {
	int num;
	LPDIRECTSOUNDBUFFER* buffers;
} SOUNDBUFFER;

class CXsound : public IXsound {
public:
	SOUNDBUFFER sb;
};

DEFINE_GUID(GUID_NULL, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0);

//LPDIRECTSOUNDBUFFER g_apSoundBuffers[] = { NULL, NULL, NULL, NULL };




HRESULT
CreateBufferFromFile
(
    LPDIRECTSOUND           pDirectSound,
    LPCSTR                  pszFile,
    DWORD                   dwFlags,
    LPDIRECTSOUNDBUFFER *   ppBuffer
)
{
    LPDIRECTSOUNDBUFFER     pBuffer     = NULL;
    CWaveFile               WaveFile;
    WAVEFORMATEX            wfx;
    DWORD                   dwDuration;
    DSBUFFERDESC            dsbd;
    LPVOID                  pvLock;
    DWORD                   dwLockSize;
    HRESULT                 hr;
    
    hr = WaveFile.Open( pszFile );

    // Get the file format
    if(SUCCEEDED(hr))
    {   
        hr = WaveFile.GetFormat(&wfx, sizeof(wfx), NULL);
    }

    // Get the file data size
    if(SUCCEEDED(hr))
    {   
        hr = WaveFile.GetDuration(&dwDuration);
    }

    // Create the sound buffer
    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dsbd, sizeof(dsbd));
        
        dsbd.dwSize = sizeof(dsbd);
        dsbd.dwFlags = dwFlags;
        dsbd.dwBufferBytes = dwDuration;
        dsbd.lpwfxFormat = &wfx;

        if(pDirectSound)
        {
            hr = pDirectSound->CreateSoundBuffer(&dsbd, &pBuffer, NULL);
        }
        else
        {

#ifdef    SILVER
            hr = DirectSoundCreateBuffer(NULL, &dsbd, &pBuffer, NULL);
#else  // SILVER
			hr = DirectSoundCreateBuffer( &dsbd, &pBuffer );
#endif // SILVER
        }
    }

    // Read audio data
    if(SUCCEEDED(hr))
    {

        hr = pBuffer->Lock(0, 0, &pvLock, &dwLockSize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    }

    if(SUCCEEDED(hr))
    {

        hr = WaveFile.ReadSample(0, pvLock, dwLockSize, NULL);
    }

    // Success
    if(SUCCEEDED(hr))
    {
        *ppBuffer = pBuffer;
    }
    else if(pBuffer)
    {
        pBuffer->Release();
    }

    return hr;
}

bool IXsound::InitSound( CHAR** sounds, IXsound** ixsound) 
{
	HRESULT hr = S_OK;
//	_asm int 3;
	if(!ixsound) return false;
	*ixsound = 0;
	ULONG max, i;

	CXsound* cxsound = new CXsound;
	if(!cxsound) return false;

	for (max = 0; sounds[max]; max++ ) {
	}

	cxsound->sb.num = max;
	cxsound->sb.buffers = new LPDIRECTSOUNDBUFFER [max];

	for(i = 0; i < max; i++) {
		hr = CreateBufferFromFile( NULL, sounds[i], 0, &(cxsound->sb.buffers[i]) );

		if ( FAILED( hr ) )
			return false;
	}

	*ixsound = (IXsound*)cxsound;
	return true;
}

IXsound::~IXsound() 
{
	for ( ULONG i = 0; i < 4; i++ ) {
		if (((CXsound*)this)->sb.buffers[i]) {
			((CXsound*)this)->sb.buffers[i]->Release();
		}
	}
	delete[] ((CXsound*)this)->sb.buffers;
}

bool IXsound::XPlaySound(UINT sound) {
	CXsound * cxsound = (CXsound*)this;

	if ( NULL == cxsound->sb.buffers[sound])
		return false;

	HRESULT hr = cxsound->sb.buffers[sound]->SetCurrentPosition( 0 );

	if ( FAILED( hr ) )
		return false;
	
	hr = cxsound->sb.buffers[sound]->Play(0, 0, 0);

	if ( FAILED( hr ) )
		return false;

	return true;
}


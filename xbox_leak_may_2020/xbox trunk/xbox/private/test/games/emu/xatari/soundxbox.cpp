#include "SoundXBOX.h"
#include "TIASound.h"

#define NUM_BUFFERS 1

SoundXBOX::SoundXBOX()
{
	m_pBuffer = NULL;
}

HRESULT SoundXBOX::Initialize()
{
	WAVEFORMATEX wfx;
	wfx.cbSize = 0;
	wfx.nAvgBytesPerSec = 31400;
	wfx.nBlockAlign = 1;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 31400;
	wfx.wBitsPerSample = 8;
	wfx.wFormatTag = WAVE_FORMAT_PCM;

	DSBUFFERDESC dsbd;
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.dwFlags = 0;
	dsbd.dwBufferBytes = 31400 * NUM_BUFFERS;
	dsbd.lpwfxFormat = &wfx;
	dsbd.dwMixBinMask = DSMIXBIN_FRONT_LEFT | DSMIXBIN_FRONT_RIGHT;
    dsbd.dwInputMixBinMask = 0;
	
	HRESULT hr = DirectSoundCreateBuffer( &dsbd, &m_pBuffer );

	Tia_sound_init( 31400, 31400 );

	hr = m_pBuffer->PlayEx( 0, DSBPLAY_LOOPING );

	m_dwStart = 0;

	return hr;
}

SoundXBOX::~SoundXBOX()
{
	if ( m_pBuffer )
		m_pBuffer->Release();
}

void SoundXBOX::set(Sound::Register reg, uInt8 val)
{
	LPVOID pMem = NULL;
	DWORD dwBytes = 31400;
	
	Update_tia_sound( reg, val );

	m_pBuffer->Lock( 31400 * m_dwStart, 31400, &pMem, &dwBytes, NULL, NULL, 0 );
	Tia_process( (LPBYTE) pMem, 31400 );

	m_dwStart = ( m_dwStart + 1 ) % NUM_BUFFERS;

}

void SoundXBOX::mute(bool state)
{
	if ( state )
		m_pBuffer->Stop();
	else
		m_pBuffer->PlayEx( 0, DSBPLAY_LOOPING );
}
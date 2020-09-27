#pragma once

#include <dsound.h>

class CSoundSource
{
public:
	CSoundSource();
	~CSoundSource();

	HRESULT CreateBuffer(WAVEFORMATEX* pwfx, int nBufferBytes);
	HRESULT LoadBuffer(int nBufferPos, const void* pbSoundData, int nSoundBytes);
	HRESULT ClearBuffer();
	bool CanWrite();
	HRESULT Write(const void* pbSoundData, int nSoundBytes);
	HRESULT Play();
	void Stop();
	void Move(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);

	HRESULT RestoreBuffers();

	LPDIRECTSOUNDBUFFER m_pDSBuffer;
	DWORD m_dwBufferBytes;
	DWORD m_dwBufferWrite;
	HANDLE m_hEvent;
	int m_nBuffersInUse;
	bool m_bBuffered;

	DS3DBUFFER m_dsBufferParams;
};


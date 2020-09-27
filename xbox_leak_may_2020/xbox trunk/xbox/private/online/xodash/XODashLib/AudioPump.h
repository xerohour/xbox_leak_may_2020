#pragma once

#define _D3DTYPES_H_


#include <dsound.h>

// Static audio buffer for sound effects...
class CAudioBuf
{
public:
	CAudioBuf();
	virtual ~CAudioBuf();

	HRESULT Initialize(WAVEFORMATEX* pWaveFormat, int nBufferBytes, const void* pvSamples = NULL);
	void* Lock();
	void Unlock(void* pvBuffer);

	virtual bool Play(bool bLoop = false);
	virtual void Stop();
	virtual float GetPlaybackTime();
	virtual float GetPlaybackLength(); // NOTE: will return 0 if unknown!
	virtual bool IsPlaying();

	virtual void Pause(bool bPause);
	inline bool IsPaused() const { return m_bPaused; }
	
	virtual void* GetSampleBuffer();
	virtual DWORD GetSampleBufferSize();

	void SetAttenuation(float nAttenuation); // 0..100 dB
	void SetPan(float nPan); // -100..100 dB
	void SetFrequency(float nFrequency); // 0 (normal), or 100..100,000

protected:
	bool m_bLoop;
	bool m_bPaused;
	int m_nBufferBytes;
	int m_nBytesPerSecond;
	LPDIRECTSOUNDBUFFER m_pDSBuffer;
};


// Double-buffered Audio Buffer for streaming...
class CAudioPump : public CAudioBuf
{
public:
	CAudioPump();
	virtual ~CAudioPump();

	HRESULT Initialize(DWORD dwStackSize, WAVEFORMATEX* pWaveFormat, int nBufferBytes, int nSegmentsPerBuffer = 4, int nPrebufferSegments = 1);

	virtual bool Play(bool bLoop = false);
	virtual void Stop();
	virtual bool IsPlaying();
	virtual float GetPlaybackTime();
	virtual void Pause(bool bPause);
	virtual float GetPlaybackLength(); // NOTE: will return 0 if unknown!

protected:
	enum
    {
        PUMPSTATE_STOPPED,
        PUMPSTATE_BUFFERING,
        PUMPSTATE_RUNNING,
        PUMPSTATE_STOPPING,
    };

    static DWORD CALLBACK StartThread(LPVOID pvContext);
	DWORD ThreadProc();

	HANDLE m_hPlayThread;
	HANDLE m_hTerminate;
	HANDLE m_hRun;
	HANDLE* m_ahNotify; // m_nSegmentsPerBuffer events
    HANDLE m_hMutex;

    DWORD m_dwPrevCursor;
	int m_nCompletedBuffers;
    int m_nFilledBuffers;
    bool* m_pfBufferFilled; // m_nSegmentsPerBuffer bools
    int m_nPumpState;

    int m_nSegmentsPerBuffer;
    int m_nPrebufferSegments;

	bool FillBuffer(int nBuffer);
	virtual int GetData(BYTE* pbBuffer, int cbBuffer) = 0;
	virtual void OnAudioEnd() {} 
};


// Audio Buffer for streaming large files...
class CFilePump : public CAudioPump
{
public:
	CFilePump();
	virtual ~CFilePump();

	HRESULT Initialize(HANDLE hFile, int nFileBytes, WAVEFORMATEX* pFormat);
	virtual void Stop();

	virtual float GetPlaybackLength();

	virtual void* GetSampleBuffer();
	virtual DWORD GetSampleBufferSize();

protected:
	virtual int GetData(BYTE* pbBuffer, int cbBuffer);
	HANDLE m_hFile;
	void* m_pvBuffer;
	float m_nPlaybackLength;
	DWORD m_dwStartPos;
    DWORD m_dwBufferSize;
};

#ifdef _XBOX

// WMA decoding Audio Buffer for soundtracks...
class CWMAPump : public CAudioPump
{
public:
	CWMAPump();
	virtual ~CWMAPump();

	HRESULT Initialize(const TCHAR* szFileName, HANDLE hFile, WAVEFORMATEX* pFormat);
	HRESULT Initialize(DWORD dwSongID, WAVEFORMATEX* pFormat);

	virtual float GetPlaybackLength();

	virtual void* GetSampleBuffer();
	virtual DWORD GetSampleBufferSize();

    virtual void Stop();

protected:
	virtual int GetData(BYTE* pbBuffer, int cbBuffer);
	HANDLE m_hFile;
	XFileMediaObject* m_pSourceFilter;
	void* m_pvBuffer;
	float m_nPlaybackLengthInSeconds;	// Used to store the length of the WMA song from the Database
};

#endif

#pragma once
#include "cdda.h"
#include "AudioPump.h"

class XCDPlayer : public CAudioPump
{
public:
    XCDPlayer();
    ~XCDPlayer();

    HRESULT Initialize(int nTrack, WAVEFORMATEX* pFormat);
//    void Seek(BOOL fForward);
    void Stop();

	void* GetSampleBuffer();
	DWORD GetSampleBufferSize();

	float GetPlaybackTime();
	float GetPlaybackLength();
	virtual void OnAudioEnd();

private:
   	int GetData(BYTE* pbBuffer, int cbBuffer);
	bool SelectTrack(int nTrack);
    DWORD GetPosition();
    void SetPosition(DWORD dwPosition);

	DWORD m_dwStartPosition;
	DWORD m_dwStopPosition;
	CCDDAStreamer m_streamer;
    LPVOID m_pvBuffer;
	int m_nCurrentTrack;
	int m_nTotalTracks;
};


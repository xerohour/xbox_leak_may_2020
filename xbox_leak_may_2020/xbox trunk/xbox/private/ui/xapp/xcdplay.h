#pragma once
#include "cdda.h"
#include "AudioPump.h"

class XCDPlayer : public CAudioPump
{
public:
    XCDPlayer();
    ~XCDPlayer();

    bool Initialize(int nTrack, WAVEFORMATEX* pFormat);
//    void Seek(BOOL fForward);
    void Stop();

	void* GetSampleBuffer();
	DWORD GetSampleBufferSize();

	float GetPlaybackTime();
	float GetPlaybackLength();

private:
   	int GetData(BYTE* pbBuffer, int cbBuffer);

    DWORD GetPosition();
    void SetPosition(DWORD dwPosition);

	DWORD m_dwStartPosition;
	DWORD m_dwStopPosition;
	CCDDAStreamer m_streamer;
    LPVOID m_pvBuffer;
};


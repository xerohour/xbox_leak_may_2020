#pragma once
#include "ntiosvc.h"

#define CDDA_MAX_FRAMES_PER_READ 16
#define CDDA_BUFFER_SIZE (CDDA_MAX_FRAMES_PER_READ * CDAUDIO_BYTES_PER_FRAME)

class CCDDAStreamer
{
public:
	CCDDAStreamer( CNtIoctlCdromService* pDrive, DWORD dwRetries = 0 );
	~CCDDAStreamer();

	int Read( void* pvBuffer, int nBytes );
	
	inline DWORD GetFrame() const
	{
		return m_dwCurFrame;
	}
	
	inline void SetFrame( DWORD dwFrame )
	{
        DebugPrint( "CCDDAStreamer::SetFrame():Reseting CDDAStreamer frame to %d\n", dwFrame );
		m_dwCurFrame = dwFrame;
		m_ibChunk = 0;
	}

protected:
	int ReadFrames( void* pvBuffer, DWORD nFrameCount );

	BYTE* m_chunk;
	int m_ibChunk;

	CNtIoctlCdromService* m_pDrive;
    DWORD m_dwRetries;
	DWORD m_dwCurFrame;
};

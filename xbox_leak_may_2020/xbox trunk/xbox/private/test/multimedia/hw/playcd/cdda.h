#pragma once
#include "ntiosvc.h"

class CCDDAStreamer
{
public:
	CCDDAStreamer(CNtIoctlCdromService* pDrive);
	~CCDDAStreamer();

	int Read(void* pvBuffer, int nBytes);
	
	inline DWORD GetFrame() const
	{
		return m_dwCurFrame;
	}
	
	inline void SetFrame(DWORD dwFrame)
	{
		if (dwFrame != m_dwCurFrame)
		{
			m_dwCurFrame = dwFrame;
			m_ibChunk = 0;
		}
	}

	inline bool HadError() const
	{
		return m_bError;
	}

	inline void ResetError()
	{
		m_bError = false;
	}

protected:
	int ReadChunk(void* pvBuffer);

	BYTE* m_chunk /*[BYTES_PER_CHUNK]*/;
	int m_ibChunk;

	CNtIoctlCdromService* m_pDrive;
	DWORD m_dwCurFrame;

	bool m_bError;
};

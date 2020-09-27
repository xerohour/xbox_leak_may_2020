#pragma once
#include "xcddefs.h"

class CNtIoctlCdromService
{
private:
	HANDLE m_hDevice;

public:
	CNtIoctlCdromService();
	~CNtIoctlCdromService();

	HRESULT Open(DWORD dwDriveNumber);
	void Close();
	HRESULT Read(DWORD dwReadStart, DWORD dwReadLength, LPVOID pvBuffer, DWORD dwRetries = 0);

	inline bool IsOpen() const
	{
		return m_hDevice != INVALID_HANDLE_VALUE;
	}

	inline int GetTrackCount() const
	{
		if (!IsOpen())
			return 0;

		return m_toc.LastTrack;
	}

	bool GetTotalLength(int* pnMinutes, int* pnSeconds, int* pnFrames);
	bool GetTrackLength(int nTrack, int* pnMinutes, int* pnSeconds, int* pnFrames);

#ifdef _CD_INFO
	const TCHAR* GetTitle();
	const TCHAR* GetArtist();
	const TCHAR* GetTrackName(int nTrack);
#endif

	inline DWORD GetTrackFrame(int nTrack) const
	{
		if (!IsOpen())
			return 0;

		ASSERT(nTrack >= 0 && nTrack <= m_toc.LastTrack);
		return m_toc.TrackAddr[nTrack];
	}

	inline int GetTrackFromFrame(DWORD dwFrame) const
	{
		return m_toc.GetTrackFromFrame(dwFrame);
	}

protected:
	XCDROM_TOC m_toc;
	bool GetTableOfContents();
	HRESULT DeviceIoControl(DWORD dwControlCode, LPVOID pvInBuffer = NULL, DWORD dwInBufferSize = 0, LPVOID pvOutBuffer = NULL, DWORD dwOutBufferSize = 0, LPDWORD pdwBytesReturned = NULL);
};

extern CNtIoctlCdromService g_cdrom;
extern bool OpenCDROM();

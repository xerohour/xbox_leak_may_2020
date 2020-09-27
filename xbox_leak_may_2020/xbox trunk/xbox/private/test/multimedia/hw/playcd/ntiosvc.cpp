#include "ntiosvc.h"

#define TOC_DATA_TRACK              (0x04)

CNtIoctlCdromService g_cdrom;

////////////////////////////////////////////////////////////////////////////

CNtIoctlCdromService::CNtIoctlCdromService()
{
	m_hDevice = INVALID_HANDLE_VALUE;
}

CNtIoctlCdromService::~CNtIoctlCdromService()
{
	Close();
}

void CNtIoctlCdromService::Close()
{
	if (m_hDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDevice);
		m_hDevice = INVALID_HANDLE_VALUE;
	}

	m_toc.Delete();
}

HRESULT CNtIoctlCdromService::Open(DWORD dwDriveNumber)
{
//	Close();

	ASSERT(m_hDevice == INVALID_HANDLE_VALUE);

	CHAR szPath [] = "CDROM0:";

	m_hDevice = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());

		return hr;
	}

	if (!GetTableOfContents())
		Close();

	return S_OK;
}


bool CNtIoctlCdromService::GetTableOfContents()
{
	HRESULT hr;
	CDROM_TOC toc;

	for (int nRetry = 0; nRetry < 10; nRetry += 1)
	{
		hr = DeviceIoControl(IOCTL_CDROM_READ_TOC, NULL, 0, &toc, sizeof (toc));

		if (SUCCEEDED(hr))
			break;

		if (hr != HRESULT_FROM_WIN32(ERROR_NOT_READY))
		{
			return false;
		}

		Sleep(10);
	}

	if (FAILED(hr))
		return false;

	// BLOCK: Convert TOC format...
	{
		int nAudioTrack = 0;

		for (DWORD i = 0; i <= toc.LastTrack; i++)
		{
			DWORD msf = TocValToMsf(toc.TrackData[i].Address);
//			TRACE(_T("\t%d: %d:%02d\n"), i + 1, MCI_MSF_MINUTE(msf), MCI_MSF_SECOND(msf));

			if ((toc.TrackData[i].Control & TOC_DATA_TRACK) == 0)
			{
				m_toc.TrackAddr[nAudioTrack] = TocValToFrames(toc.TrackData[i].Address);
				nAudioTrack += 1;
			}
		}

		m_toc.LastTrack = nAudioTrack - 1;
	}

	return m_toc.LastTrack > 0;
}

HRESULT CNtIoctlCdromService::Read(DWORD  dwReadStart, DWORD  dwReadLength, LPVOID pvBuffer)
{
	DWORD dwAttemptsAllowed = 10;
	RAW_READ_INFO rri;
	HRESULT hr;

	// NT drivers seem to want the disk offset to be (sector - 150) * 2048,
	// regardless of what the actual disk lead-in amount is.
	rri.DiskOffset.QuadPart = (ULONGLONG)(dwReadStart - 150) * (ULONGLONG)2048;
	rri.SectorCount = dwReadLength;
	rri.TrackMode = CDDA;

	while (dwAttemptsAllowed--)
	{
		hr = DeviceIoControl(IOCTL_CDROM_RAW_READ, &rri, sizeof (rri), pvBuffer, CDAUDIO_BYTES_PER_FRAME * dwReadLength);

		if (SUCCEEDED(hr))
			break;

		if (hr == HRESULT_FROM_WIN32(ERROR_NOT_READY))
		{
			Close();
			return hr;
		}

	}

	return hr;    
}

HRESULT CNtIoctlCdromService::DeviceIoControl(DWORD dwControlCode, LPVOID pvInBuffer, DWORD dwInBufferSize, LPVOID pvOutBuffer, DWORD dwOutBufferSize, LPDWORD pdwBytesReturned)
{
	DWORD dwBytesReturned;

	if (pdwBytesReturned == NULL)
		pdwBytesReturned = &dwBytesReturned;

	if (!::DeviceIoControl(m_hDevice, dwControlCode, pvInBuffer, dwInBufferSize, pvOutBuffer, dwOutBufferSize, pdwBytesReturned, NULL))
		return HRESULT_FROM_WIN32(GetLastError());

	return S_OK;
}

bool CNtIoctlCdromService::GetTotalLength(int* pnMinutes, int* pnSeconds, int* pnFrames)
{
	if (!IsOpen())
		return false;

	DWORD dwFrames = m_toc.TrackAddr[GetTrackCount()] - m_toc.TrackAddr[0];
	DWORD dwMsf = FramesToMsf(dwFrames);

	if (pnMinutes != NULL)
		*pnMinutes = MCI_MSF_MINUTE(dwMsf);
	
	if (pnSeconds != NULL)
		*pnSeconds = MCI_MSF_SECOND(dwMsf);
	
	if (pnFrames != NULL)
		*pnFrames = MCI_MSF_FRAME(dwMsf);

	return true;
}

bool CNtIoctlCdromService::GetTrackLength(int nTrack, int* pnMinutes, int* pnSeconds, int* pnFrames)
{
	if (!IsOpen() || nTrack < 0 || nTrack > GetTrackCount() - 1)
		return false;

	DWORD dwFrames = m_toc.TrackAddr[nTrack + 1] - m_toc.TrackAddr[nTrack];
	DWORD dwMsf = FramesToMsf(dwFrames);

	if (pnMinutes != NULL)
		*pnMinutes = MCI_MSF_MINUTE(dwMsf);
	
	if (pnSeconds != NULL)
		*pnSeconds = MCI_MSF_SECOND(dwMsf);
	
	if (pnFrames != NULL)
		*pnFrames = MCI_MSF_FRAME(dwMsf);

	return true;
}

////////////////////////////////////////////////////////////////////////////

XCDROM_TOC::XCDROM_TOC()
{
	Clear();
}

XCDROM_TOC::~XCDROM_TOC()
{
	Delete();
}

void XCDROM_TOC::Delete()
{
	Clear();
}

void XCDROM_TOC::Clear()
{
	LastTrack = 0;
}

int XCDROM_TOC::GetTrackFromFrame(DWORD dwPosition) const
{
	for (int i = 0; i < LastTrack; i += 1)
	{
		if (dwPosition < TrackAddr[i + 1])
			return i;
	}

	return 0;
}



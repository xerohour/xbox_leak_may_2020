// host.cpp

#include "stdafx.h"

void CHost::SetUhcHwnd (LPCTSTR str)
{
	LPTSTR end;
	m_uhcHwnd = (HWND) _tcstoul (str, &end, 0);
	if (str == end)
		m_uhcHwnd = NULL;
}

void CHost::CloseIniFile ()
{
	if (m_start)
	{
		delete [] m_start;
		m_start = 0;
	}
}

#ifdef STAND_ALONE_MODE  // Only include this in stand-alone mode

static LPCTSTR XmsgTypes [] = {	
							_T("invalid command"),
							_T("starting"),
							_T("ending"),
							_T("active"),
							_T("started"),
							_T("ended"),
							_T("stat"),
							_T("warning"),
							_T("debug"),
							_T("error"),
							NULL
						};

int CHost::iSendHost (
						DWORD dwMID,
						char *pcBufferToHost,
						DWORD dwBufferToHostLength,
						CHostResponse& CHR,
						DWORD *pdwBytesReceivedFromHost,
						DWORD *pdwErrorCodeFromHost,
						DWORD dwTimeout)
/*
int CHost::iSendHost (
						char *pcBufferToHost, 
						int iBufferToHostLength, 
						char *pcBufferFromHost, 
						int iBufferFromHostLength, 
						int *piActualBytesReceived, 
						int *piBufferFromHostOverflow, 
						int iTimeout)
*/
{
	UNREFERENCED_PARAMETER (CHR);
	UNREFERENCED_PARAMETER (pdwBytesReceivedFromHost);
	UNREFERENCED_PARAMETER (pdwErrorCodeFromHost);
	UNREFERENCED_PARAMETER (dwTimeout);

	CAutoLeave lock (&m_section);

/*
// This was used for UHC debugging but is no longer needed
	if (GetUhcHwnd ())
	{
		int length = iBufferToHostLength;
		void *heap = HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, length);
		memcpy (heap, pcBufferToHost, length);

		COPYDATASTRUCT cds;
		cds.dwData = *(DWORD *) &pcBufferToHost; // peel off the type value
		cds.cbData = length;
		cds.lpData = heap;

		::SendMessage (GetUhcHwnd(), WM_COPYDATA, (WPARAM) hWnd, (LPARAM) &cds);
			
		HeapFree (GetProcessHeap (), 0, heap);
		return 0;
	}
*/
	CStore blob ((LPBYTE) pcBufferToHost, dwBufferToHostLength);

//	int type;
//	blob >> type;

	ASSERT (pF != NULL);
	if (dwMID < 12)
		_ftprintf (pF, _T("<%s>"), XmsgTypes [dwMID]);
	else
		_ftprintf (pF, _T("<MID=0x%x>"), dwMID);

// Warning: Do not access time(NULL) in any message that could be sent from a test
// thread.  XSS hangs when time(NULL) is called from a test thread for some reason.

	switch (dwMID)
	{
		case MID_UUT_STARTING:
			{
				CMsg::TestingStarted msg;
				msg.Load (blob);
				_ftprintf (pF, _T("\n"));
			}
			break;
		case MID_UUT_ACTIVE:
			{
				CMsg::TestingActive msg;
				msg.Load (blob);
				_ftprintf (pF, _T("\n"));
			}
			break;
		case MID_UUT_ENDING:
			{
				CMsg::TestingEnded msg;
				msg.Load (blob);
				_ftprintf (pF, _T("\n"));
			}
			break;
		case MID_UUT_TESTSTART:
			{
				CMsg::TestStarted msg;
				msg.Load (blob);
				_ftprintf (pF, _T("<%s>\n"), msg.Name);
			}
			break;
		case MID_UUT_TESTEND:
			{
				CMsg::TestEnded msg;
				msg.Load (blob);
				_ftprintf (pF, _T("<%s><%d><%s>\n"), msg.Name, msg.Elapsed, 
					msg.Result? _T("passed"):_T("failed"));
			}
			break;
		case MID_UUT_WARNING:
			{
				CMsg::Warning msg;
				msg.Load (blob);
				_ftprintf (pF, _T("<%s><%s>\n"), msg.Name, msg.Message);
			}
			break;
		case MID_UUT_DEBUG:
			{
				CMsg::Debug msg;
				msg.Load (blob);
				_ftprintf (pF, _T("<%s><%s>\n"), msg.Name, msg.Message);
			}
			break;
		case MID_UUT_STAT:
			{
				CMsg::Statistic msg;
				msg.Load (blob);
				_ftprintf (pF, _T("<%s><%s><%s>\n"), msg.Name, msg.Key, msg.Message);
			}
			break;
		case MID_UUT_ERROR:
			{
				CMsg::Error msg;
				msg.Load (blob);
				_ftprintf (pF, _T("<%s><0x%08X><%s>\n"), msg.Name, msg.ErrorCode, msg.Message);
			}
			break;
	}

	return 0;
}

LPTSTR CHost::OpenIniFile (LPCTSTR filename)
{
	char szFName[256];

	// Try to open the file
	//

	sprintf(szFName, "%S", filename);	
	CAutoClose hFile = CreateFileA (szFName,
		GENERIC_READ, //|GENERIC_WRITE,
		0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (!hFile.IsValid())
	{
		g_error.ReportFileOpenFailed (filename, (LPCTSTR) CGetLastError());
		return NULL;
	}

	// We are reading an ASCII file and converting it to UNICODE.
	// The following algorithm is designed to use a single buffer
	// for reading and conversion.  To do this we create a buffer large
	// enough to hold the converted file, then write the original
	// data to the upper half of the file.
	//
	DWORD fileSize = GetFileSize (hFile, NULL);
	m_start = new _TCHAR [fileSize + 2];
	char *middle = (char *) &m_start [(fileSize + 2)/2];

	DWORD bytesRead;
	if (!ReadFile (hFile, middle, fileSize, &bytesRead, NULL))
	{
		g_error.ReportReadFileFailed (filename, (LPCTSTR) CGetLastError());
		return NULL;
	}
    
	int len = MultiByteToWideChar (CP_ACP, 0, middle, bytesRead, m_start, fileSize + 1);
	if (len == 0)
	{
		g_error.ReportUnicodeConversionFailed (filename, (LPCTSTR) CGetLastError());
		return NULL;
	}

	m_start [fileSize] = _T('\0');
	LPTSTR current = m_start;

	// Skip over initial unicode designator character
	//
	if (0xfffe == (int) *current)
		return NULL; // This code represents little endian format
	if (0xfeff == (int) *current)
		current++;

	return current;
}

DWORD CHost::GetHostDword (DWORD /* mid */, DWORD alternate)
{
	return alternate;
}

#endif // ifdef STAND_ALONE_MODE



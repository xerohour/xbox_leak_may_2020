
#include "stdafx.h"
#include "afxdllx.h"
#include "bldtools.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
//	DLL support

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("Build Tools DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extensionDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("Build Tools DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}
	return 1;   // ok

	UNREFERENCED_PARAMETER(lpReserved);
}



// REVIEW(CFlaat): can this function be generalized to handle profiling?
BOOL CConsoleTarget::CreateTargetProcess(LPCSTR szCmdLine, DWORD dwCreationFlags, LPCSTR szEnvBlock, LPCSTR szWorkDir)
{
  char chReadBuffer[2000];  /* pipe read buffer */
  BOOL bSuccess;  /* BOOL return code for APIs */
  /* handles to the anonymous pipe */
  HANDLE hReadPipe, hWritePipe, hWritePipe2;
  DWORD cchReadBuffer;  /* number of bytes read or to be written */
  STARTUPINFO si;  /* for CreateProcess call */
  PROCESS_INFORMATION pi;  /* for CreateProcess call */
  SECURITY_ATTRIBUTES saPipe;  /* security for anonymous pipe */

  /* set up the security attributes for the anonymous pipe */
  saPipe.nLength = sizeof(SECURITY_ATTRIBUTES);
  saPipe.lpSecurityDescriptor = NULL;
  /* In order for the child to be able to write to the pipe, the handle */
  /* must be marked as inheritable by setting this flag: */
  saPipe.bInheritHandle = TRUE;

  /* create the anonymous pipe */
  bSuccess = CreatePipe(&hReadPipe,  /* read handle */
      &hWritePipe,  /* write handle, used as stdout by child */
      &saPipe,  /* security descriptor */
      0);  /* pipe buffer size */

  /* Now we need to change the inheritable property for the readable
  end of the pipe so that the child will not inherit that handle as
  a "garbage" handle. This will keep us from having extra,
  unclosable handles to the pipe. Alternatively, we could have
  opened the pipe with saPipe.bInheritHandle = FALSE and changed the
  inherit property on the *write* handle of the pipe to TRUE. */

  bSuccess = DuplicateHandle(GetCurrentProcess(), /* source process */
      hReadPipe, /* handle to duplicate */
      GetCurrentProcess(), /* destination process */
      NULL, /* new handle - don't want one, change original handle */
      0, /* new access flags - ignored since DUPLICATE_SAME_ACCESS */
      FALSE, /* make it *not* inheritable */
      DUPLICATE_SAME_ACCESS);

  /* I most cases you can get away with using the same anonymous
  pipe write handle for both the child's standard output and
  standard error, but this may cause problems if the child app
  explicitly closes one of its standard output or error handles. If
  that happens, the anonymous pipe will close, since the child's
  standard output and error handles are really the same handle. The
  child won't be able to write to the other write handle since the
  pipe is now gone, and parent reads from the pipe will return
  ERROR_BROKEN_PIPE and child output will be lost. To solve this
  problem, simply duplicate the write end of the pipe to create
  another distinct, separate handle to the write end of the pipe.
  One pipe write handle will serve as standard out, the other as
  standard error. Now *both* write handles must be closed before the
  write end of the pipe actually closes. */

  bSuccess = DuplicateHandle(GetCurrentProcess(), /* source process */
      hWritePipe, /* handle to duplicate */
      GetCurrentProcess(), /* destination process */
      &hWritePipe2, /* new handle, used as stderr by child */
      0, /* new access flags - ignored since DUPLICATE_SAME_ACCESS */
      TRUE, /* it's inheritable */
      DUPLICATE_SAME_ACCESS);

  /* Set up the STARTUPINFO structure for the CreateProcess() call */
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);

  /* If using the STARTUPINFO STARTF_USESTDHANDLES flag, be sure to
  set the CreateProcess fInheritHandles parameter too TRUE so that
  the file handles specified in the STARTUPINFO structure will be
  inheritied by the child. Note that we don't specify a standard
  input handle; the child will not inherit a valid input handle, so
  if it reads from stdin, it will encounter errors. */

  si.hStdOutput = hWritePipe; /* write end of the pipe */
  si.hStdError = hWritePipe2; /* duplicate of write end of the pipe */
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  si.wShowWindow = SW_HIDE;

  /* Now create the child process, inheriting handles */

  bSuccess = CreateProcess(NULL,  /* filename */
      (LPTSTR)szCmdLine,  /* full command line for child */
      NULL,  /* process security descriptor */
      NULL,  /* thread security descriptor */
      TRUE,  /* inherit handles? Also use if STARTF_USESTDHANDLES */
      dwCreationFlags,  /* creation flags */
      (void*)szEnvBlock,  /* inherited environment address */
      szWorkDir,  /* startup dir; NULL = start in current */
      &si,  /* pointer to startup info (input) */
      &pi);  /* pointer to process info (output) */

  #ifdef _DEBUG
  
  DWORD dwError = GetLastError();

  #endif

  /* We can close the returned child process handle and thread
  handle as we won't be needing them; you could, however, wait on
  the process handle to wait until the child process terminates. */


  /* We need to close our instances of the inheritable pipe write
  handle now that it's been inherited so that all open handles to
  the pipe are closed when the child process ends and closes its
  handles to the pipe. */

  bSuccess = CloseHandle(hWritePipe);
  bSuccess = CloseHandle(hWritePipe2);

  CString strInfo, strExeName;
  static const char cMark = '\xff';

  m_psettingsTarget -> GetTextValue(settingFilename, strExeName);

  strInfo.Format("%c5[Begin '%s']\n%c1",cMark, szCmdLine, cMark);

  WriteFile(
    m_hViewport,
    (LPCSTR)strInfo,
    strInfo.GetLength(),
    &cchReadBuffer,
    0);


  m_bufOutput.ResetContents();

  //TODO(CFlaat): make this code resistant to hangs; determine timeout limit

  /* read from the pipe until we get an ERROR_BROKEN_PIPE */
  while (1)
  {
    bSuccess = ReadFile(hReadPipe,  /* read handle */
        chReadBuffer,  /* buffer for incoming data */
        sizeof(chReadBuffer),  /* number of bytes to read */
        &cchReadBuffer,  /* number of bytes actually read */
        NULL);  /* no overlapped reading */
    if (!bSuccess && (GetLastError() == ERROR_BROKEN_PIPE))
      break;  /* child has died */
    if (bSuccess && cchReadBuffer)
    {
      /* write the data from the child to the file */
      bSuccess = WriteFile(m_hViewport,  /* write handle */
          chReadBuffer,  /* buffer to write */
          cchReadBuffer,  /* number of bytes to write */
          &cchReadBuffer,  /* number of bytes actually written */
          NULL);  /* no overlapped writing */

      m_bufOutput.Append(chReadBuffer, cchReadBuffer);  // append to our internal buffer
    }
  }

  CloseHandle(hReadPipe);

  strInfo.Format("%c6[End '%s']\n",cMark, szCmdLine, cMark);

  WriteFile(
    m_hViewport,
    (LPCSTR)strInfo,
    strInfo.GetLength(),
    &cchReadBuffer,
    0);


  DWORD dw = WaitForSingleObject(pi.hProcess, 60000);

  if (WAIT_OBJECT_0 != dw)
  {
    dw = GetLastError();
//    ASSERT(FALSE);
    return FALSE;
  }

  if (!GetExitCodeProcess(pi.hProcess, &m_dwExitCode))
  {
    ASSERT(FALSE);
    return FALSE;
  }
  

  bSuccess = CloseHandle(pi.hThread);
  bSuccess &= CloseHandle(pi.hProcess);

  // REVIEW(CFlaat): we should use bSuccess more than we presently do
  return bSuccess;
}


LPCSTR CConsoleTarget::GetLastOutput(void)
{
  return m_bufOutput.GetAsSZ();
}

LPCSTR CConsoleTarget::GetLastCmdLine(void)
{
  return m_strLastCmdLine;
}


void CConsoleTarget::COutputBuffer::ResetContents(void)
{
  m_pcNext = m_acBuf;
  m_bFull = FALSE;
}

LPCSTR CConsoleTarget::COutputBuffer::GetAsSZ(void)
{
  ASSERT((m_pcNext - m_acBuf) < BufSize);  // ensure we're not past the end
  ASSERT(m_pcNext);
  *m_pcNext = '\0';
  return m_acBuf;
}

BOOL CConsoleTarget::COutputBuffer::Append(LPCSTR szText, UINT cbRequested)
{
  if (m_bFull)
    return FALSE;

  static LPCSTR szTrunc = " <Output truncated>";
  static UINT cbTrunc = strlen(szTrunc);
  UINT cbAvail = BufSize - (m_pcNext - m_acBuf) - cbTrunc - 1;  // we save space for the truncation message
  UINT cbToWrite;

  if (cbAvail < cbRequested)  // if we don't have enough space available
  {
    cbToWrite = cbAvail;
    memcpy(m_pcNext, szText, cbAvail);  // do the copy
    m_pcNext += cbAvail;
    memcpy(m_pcNext, szTrunc, cbTrunc); // add the truncation notification message
    m_pcNext += cbTrunc;
    m_bFull = TRUE;
    return TRUE;
  }
  else
  {
    cbToWrite = cbRequested; // otherwise, write all that's requested
    memcpy(m_pcNext, szText, cbToWrite);  // do the copy
    m_pcNext += cbToWrite;
    return TRUE;
  }
}

BOOL CConsoleTarget::COutputBuffer::IsEmpty(void)
{
  return (m_acBuf == m_pcNext);
}


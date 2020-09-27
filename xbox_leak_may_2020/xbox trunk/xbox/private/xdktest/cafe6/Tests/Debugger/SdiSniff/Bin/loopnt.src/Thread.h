// thread.h
// history:
//  written by bartonp 4/96

#ifndef THREAD_H
#define THREAD_H
#include <windows.h>


//HANDLE CreateThread(
//    LPSECURITY_ATTRIBUTES lpThreadAttributes,// pointer to thread security attributes
//    DWORD dwStackSize,          // initial thread stack size, in bytes
//    LPTHREAD_START_ROUTINE lpStartAddress,   // pointer to thread function
//    LPVOID lpParameter,   // argument for new thread
//    DWORD dwCreationFlags,// creation flags
//    LPDWORD lpThreadId    // pointer to returned thread identifier);
//typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
//typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;


class CThread
{
 public:
//  CThread(DWORD (WINAPI * ptfn)( LPVOID)): m_ptfn(ptfn) {}
  CThread(LPTHREAD_START_ROUTINE ptfn,LPVOID lpv): m_ptfn(ptfn),
                                        m_dwThreadID(0),
                                        m_hThread(NULL),
                                        m_lpvThreadParm(lpv)
                                        {};
  CThread(void): m_ptfn(NULL),
                 m_dwThreadID(0),
                 m_hThread(NULL),
                 m_lpvThreadParm(NULL) {};
  virtual ~CThread(void) { if(m_hThread != 0) CloseHandle(m_hThread); }

  BOOL  bCreate(LPSECURITY_ATTRIBUTES lpsa=NULL,DWORD dwss = 0);
  BOOL  bCreate(LPTHREAD_START_ROUTINE ptfn, 
	            LPVOID lpv,
				LPSECURITY_ATTRIBUTES lpsa=NULL,
				DWORD dwss = 0);
  BOOL  bStart(void) { return (dwResume() != 0xFFFFFFFF);}
  DWORD dwSuspend(void);
  DWORD dwResume(void);
  BOOL  bTerminate(void);
  DWORD dwGetTID(void) { return m_dwThreadID; }
 private:
//   DWORD (WINAPI *m_ptfn)( LPVOID  ); // pointer to thread fn
   LPTHREAD_START_ROUTINE m_ptfn;       // pointer to thread fn
   HANDLE                 m_hThread;    // thread handle
   DWORD                  m_dwThreadID; // thread id
   LPVOID                 m_lpvThreadParm; // thread parm


 public:  // debug calls
	unsigned long  debugCall(LPVOID lpv) 
	{ 
		return( m_ptfn(lpv)); 
	}
	unsigned long  debugCall(void)
	{
		return( m_ptfn(m_lpvThreadParm));
	}
};

void WaitForKeystroke(void);

#endif

/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

typedef DWORD (WINAPI *PTHREAD_SMART_ROUTINE)(LPVOID lpThreadParameter,HANDLE hShutdownEvent);
typedef PTHREAD_SMART_ROUTINE LPTHREAD_SMART_ROUTINE;

enum enumEvents{EVENTS_TRIGGER=0,EVENTS_SHUTDOWN,EVENTS_FINISHED,EVENTS_ENDENUM};

/////////////////////////////////////////////////////////////////////////////
// WorkerThreadPool - Generic thread pool class that can be used to
// distribute work across multiple worker threads.  Construct the object with
// the number of threads that you want and the static worker function that
// you have define. Your static worker function must conform to the
// LPTHREAD_SMART_ROUTINE function type.  Then call DoWork when ever you have
// work that needs to be processed.  As defined by the LPTHREAD_SMART_ROUTINE
// function type, the parameters you pass to your static worker function are
// of type LPVOID.  Since the parameters are of type LPVOID it is the
// responsiblity of this object to delete that data when finished.  So, make
// sure that the parameter you pass can be deleted via delete [] pData.  It
// is suggested that pData be a pointer to a class with an appriopriate
// destructor defined.  If a work function takes time be sure to check the
// hShutdownEvent or use it in combination with any WaitForObject processing.
/////////////////////////////////////////////////////////////////////////////

class WorkerThreadPool{

	public:
		//Constructor
		WorkerThreadPool(unsigned int cThreads, LPTHREAD_SMART_ROUTINE pWorkerFunc, HANDLE hShutdownEvent=NULL);
		//Destructor
		~WorkerThreadPool();
		//Perform work
		HRESULT DoWork(LPVOID);
		//Is class initialized
		bool IsInit(){return m_fInit;}
		//Obtain access to the shutdown event
		HANDLE GetShutdownEvent(){return m_hShutdown;}

	private:
		//Structure used to pass events and parameters to worker thread
		struct ThreadParam{LPTHREAD_SMART_ROUTINE pFunc;LPVOID pData;HANDLE rgEvents[EVENTS_ENDENUM];};

	private: 
		bool m_fInit;  //Instance successfully initialized
		unsigned int m_cThreads;  //Number of worker threads
		HANDLE* m_rghThreads;  //Array of worker threads
		ThreadParam* m_rgThreadParams;  //Array of thread parameters and events
		HANDLE* m_rghFinishedEvents;  //Duplicate finished event array for WaitForMultipleObjects
		HANDLE m_hShutdown;  //Shutdown event
		bool m_fOwnShutdownEvent;  //Was the shutdown event passed in or do we own it?

	private:
		//Internal worker thread function that wraps user function
		static DWORD WINAPI WorkerThread(LPVOID);
		//Initialization routine
		unsigned int Init(unsigned int cThreads, LPTHREAD_SMART_ROUTINE pWorkerFunc, HANDLE hShutdownEvent);

};//endclass

#endif
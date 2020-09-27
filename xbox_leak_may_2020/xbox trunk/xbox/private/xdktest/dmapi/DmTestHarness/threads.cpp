//
// thread routines
// for both Win32 and Xbox APIs
//

#include "tests.h"
#include "logging.h"

#define INVALID_THREADID 9999
void testThreadSupport()
{
	HRESULT				hr;
	DWORD				Threads[512];
	DWORD				numThreads;
	DM_THREADINFO		threadInfo;
	DM_THREADSTOP		threadStop;
	CONTEXT				context;

#ifdef _XBOX
	// TODO: testThreadSupport - create a whole slew of threads which we can verify results against
#endif

	// DmGetThreadList
	// - valid and invalid parameters

	numThreads = sizeof Threads;
	VERIFY_HRESULT(DmGetThreadList(Threads, &numThreads), XBDM_NOERR);
	numThreads = sizeof Threads;
	VERIFY_HRESULT(DmGetThreadList(NULL, &numThreads), E_INVALIDARG);
	VERIFY_HRESULT(DmGetThreadList(Threads, NULL), E_INVALIDARG);
	numThreads = 0;
	VERIFY_HRESULT(DmGetThreadList(Threads, &numThreads), XBDM_BUFFER_TOO_SMALL);
	if (numThreads > 1)
	{
		numThreads = 1;
		VERIFY_HRESULT(DmGetThreadList(Threads, &numThreads), XBDM_BUFFER_TOO_SMALL);
	}

	// DmGetThreadContext/DmSetThreadContext

	numThreads = sizeof Threads;
	DmGetThreadList(Threads, &numThreads);
	ZeroMemory(&context, sizeof context);
	context.ContextFlags = CONTEXT_FULL;

	// Get/Set with an active thread
	VERIFY_HRESULT(DmGetThreadContext(Threads[0], &context), XBDM_NOERR);
	VERIFY_HRESULT(DmSetThreadContext(Threads[0], &context), XBDM_NOTSTOPPED);

	// Get/Set with all threads stopped
	VERIFY_HRESULT(DmStop(), XBDM_NOERR);
	VERIFY_HRESULT(DmGetThreadContext(Threads[0], &context), XBDM_NOERR);
	VERIFY_HRESULT(DmSetThreadContext(Threads[0], &context), XBDM_NOERR);
	VERIFY_HRESULT(DmGo(), XBDM_NOERR);

	// Get/Set with invalid parameters
	VERIFY_HRESULT(DmStop(), XBDM_NOERR);
	VERIFY_HRESULT(DmGetThreadContext(Threads[0], NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmGetThreadContext(INVALID_THREADID, &context), XBDM_NOTHREAD);
	VERIFY_HRESULT(DmSetThreadContext(Threads[0], NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmSetThreadContext(INVALID_THREADID, &context), XBDM_NOTHREAD);
	VERIFY_HRESULT(DmGo(), XBDM_NOERR);

	// Get/Set with suspended threads
	VERIFY_HRESULT(DmSuspendThread(Threads[0]), XBDM_NOERR);
	VERIFY_HRESULT(DmGetThreadContext(Threads[0], &context), XBDM_NOERR);
	VERIFY_HRESULT(DmSetThreadContext(Threads[0], &context), XBDM_NOERR);
	VERIFY_HRESULT(DmResumeThread(Threads[0]), XBDM_NOERR);

	if (numThreads > 1)
	{
		VERIFY_HRESULT(DmStop(), XBDM_NOERR);
		VERIFY_HRESULT(DmGetThreadContext(Threads[1], &context), XBDM_NOERR);
		VERIFY_HRESULT(DmSetThreadContext(Threads[1], &context), XBDM_NOERR);
		VERIFY_HRESULT(DmGetThreadContext(Threads[numThreads - 1], &context), XBDM_NOERR);
		VERIFY_HRESULT(DmSetThreadContext(Threads[numThreads - 1], &context), XBDM_NOERR);
		VERIFY_HRESULT(DmGo(), XBDM_NOERR);
	}

	// DmGetThreadInfo
	// - valid and invalid parameters

	numThreads = sizeof Threads;
	DmGetThreadList(Threads, &numThreads);
	VERIFY_HRESULT(DmGetThreadInfo(Threads[0], &threadInfo), XBDM_NOERR);
	VERIFY_HRESULT(DmGetThreadInfo(Threads[numThreads - 1], &threadInfo), XBDM_NOERR);

	ZeroMemory(&(Threads[0]), sizeof (Threads[0]));
	VERIFY_HRESULT(DmGetThreadInfo(Threads[0], &threadInfo), XBDM_NOTHREAD);
	VERIFY_HRESULT(DmGetThreadInfo(NULL, &threadInfo), XBDM_NOTHREAD);
	numThreads = sizeof Threads;
	DmGetThreadList(Threads, &numThreads);
	VERIFY_HRESULT(DmGetThreadInfo(Threads[0], NULL), E_INVALIDARG);

	// DmSuspendThread and DmResumeThread
	// - suspend and resume
	// - suspend and resume with delay
	// - multiple suspend and resume
	// - suspend suspended thread
	// - resume non-suspended thread
	// - suspend/resume invalid thread

	VERIFY_HRESULT(DmSuspendThread(Threads[0]), XBDM_NOERR);
	VERIFY_HRESULT(DmResumeThread(Threads[0]), XBDM_NOERR);

	VERIFY_HRESULT(DmSuspendThread(Threads[0]), XBDM_NOERR);
	Sleep(250);
	VERIFY_HRESULT(DmResumeThread(Threads[0]), XBDM_NOERR);

	if (numThreads > 1)
	{
		VERIFY_HRESULT(DmSuspendThread(Threads[numThreads - 1]), XBDM_NOERR);
		VERIFY_HRESULT(DmSuspendThread(Threads[numThreads - 2]), XBDM_NOERR);
		Sleep(250);
		VERIFY_HRESULT(DmResumeThread(Threads[numThreads - 1]), XBDM_NOERR);
		VERIFY_HRESULT(DmResumeThread(Threads[numThreads - 2]), XBDM_NOERR);
	}

	VERIFY_HRESULT(DmSuspendThread(Threads[numThreads / 2]), XBDM_NOERR);
	Sleep(250);
	VERIFY_HRESULT(DmSuspendThread(Threads[numThreads / 2]), XBDM_NOERR);
	Sleep(250);
	VERIFY_HRESULT(DmResumeThread(Threads[numThreads / 2]), XBDM_NOERR);
	Sleep(250);
	VERIFY_HRESULT(DmResumeThread(Threads[numThreads / 2]), XBDM_NOERR);

	VERIFY_HRESULT(DmSuspendThread(INVALID_THREADID), XBDM_NOTHREAD);
	VERIFY_HRESULT(DmResumeThread(INVALID_THREADID), XBDM_NOTHREAD);

	// DmIsThreadStopped
	// - valid and invalid parameters

	// interaction with DmStop/DmGo

	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOTSTOPPED);
	VERIFY_HRESULT(DmStop(), XBDM_NOERR);
	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOERR);
	VERIFY_HRESULT(DmGo(), XBDM_NOERR);
	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOTSTOPPED);

	// interaction with DmSuspendThread/DmResumeThread

	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOTSTOPPED);
	VERIFY_HRESULT(DmSuspendThread(Threads[0]), XBDM_NOERR);
	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOERR);
	VERIFY_HRESULT(DmResumeThread(Threads[0]), XBDM_NOERR);
	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOTSTOPPED);

	// interaction with DmHaltThread/DmContinueThread

	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOTSTOPPED);
	VERIFY_HRESULT(DmHaltThread(Threads[0]), XBDM_NOERR);
	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOERR);
	VERIFY_HRESULT(DmContinueThread(Threads[0], FALSE), XBDM_NOERR);
	VERIFY_HRESULT(DmGo(), XBDM_NOERR);
	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], &threadStop), XBDM_NOTSTOPPED);

	// invalid parameters

	VERIFY_HRESULT(DmIsThreadStopped(INVALID_THREADID, &threadStop), XBDM_NOTHREAD);
	VERIFY_HRESULT(DmIsThreadStopped(Threads[0], NULL), E_INVALIDARG);
}

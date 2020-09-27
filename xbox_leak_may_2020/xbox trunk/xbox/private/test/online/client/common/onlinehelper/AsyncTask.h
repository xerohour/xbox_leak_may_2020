// AsyncTask.h: interface for the CAsyncTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <winsockx.h>
#include <stdio.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xonlinep.h>

#define LOGTASKINFO(log, string) log ? xLog(log, XLL_INFO, "%s", string) : 0;
#define LOGTASKINFOHR(log, string, hr) log ? xLog(log, XLL_INFO, "%s:%s(0x%08x)", string, CAsyncTask::GetTaskErrorString(hr), hr) : 0;

class CAsyncTask  
{
public:
	BOOL WaitForWork(DWORD dwUntilTickCount);
	BOOL IsTaskComplete();
	HRESULT GetLastResult();
	HRESULT TaskContinue();
	static char * GetTaskErrorString(HRESULT hr);
	void RegisterLogHandle(HANDLE hLog);
	CAsyncTask();
	virtual ~CAsyncTask();

protected:
	HANDLE m_hLog;
	HRESULT m_hrLastResult;
	HANDLE m_hWorkEvent;
	XONLINETASK_HANDLE m_hTask;
};

HRESULT WaitAndPump(DWORD dwWorkUntil, CAsyncTask *pTask);

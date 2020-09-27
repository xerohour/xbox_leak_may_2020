// thread.cpp
#include "thread.h"
#include <iostream.h>
#include <conio.h>    // _kbhit, _getch



BOOL
CThread::
bCreate(LPSECURITY_ATTRIBUTES lpsa/*=NULL*/,
		DWORD dwss/*=0*/)
{
	if(m_hThread == 0)
	{
		m_hThread = CreateThread(
			lpsa,     // pointer to thread security attributes
			dwss,     // initial thread stack size, in bytes  0 == default size
			m_ptfn,   // pointer to thread function
			m_lpvThreadParm,  // argument for new thread
			CREATE_SUSPENDED, // creation flags
			&m_dwThreadID);   // pointer to returned thread identifier);
	}
	else return FALSE;   // already created
	return(m_hThread!= 0);
}

BOOL
CThread::
bCreate(LPTHREAD_START_ROUTINE ptfn, 
        LPVOID lpv,
		LPSECURITY_ATTRIBUTES lpsa/*=NULL*/,
		DWORD dwss/*=0*/)
{
	if(m_hThread == 0)
	{
		m_ptfn = ptfn;
		m_lpvThreadParm=lpv;
		
		m_hThread = CreateThread(
			lpsa,     // pointer to thread security attributes
			dwss,     // initial thread stack size, in bytes  0 == default size
			m_ptfn,   // pointer to thread function
			m_lpvThreadParm,  // argument for new thread
			CREATE_SUSPENDED, // creation flags
			&m_dwThreadID);   // pointer to returned thread identifier);
	}
	else return FALSE;   // already created
	return(m_hThread!= 0);
}

DWORD
CThread::
dwResume(void)
{
	return ResumeThread( m_hThread);
}

DWORD
CThread::
dwSuspend(void)
{
	return SuspendThread( m_hThread);
}

BOOL
CThread::
bTerminate(void)
{
	BOOL bRC = TerminateThread(m_hThread,0);
	if(bRC)
	{
		m_dwThreadID = 0;
		m_hThread = 0;
	}
	return bRC;
}


DWORD  dwtestfn(int n)
{
	cout << "dwtestfn " << n <<endl;
	return(n);
}

void WaitForKeystroke(void)
{
	while( !_kbhit() )
		Sleep(100);
	_getch();
}


/*
#if defined CTHREAD_MAIN_TEST

int main(void)
{
	CThread ct((PTHREAD_START_ROUTINE)dwtestfn,(LPVOID) 2);
	CThread ct2((PTHREAD_START_ROUTINE)dwtestfn,(LPVOID) 3);
	cout << "Main" << endl;
	//  cout << ct.debugCall((LPVOID) 1) << endl;
	
	if (ct.bCreate())
	{
		cout << "Created Thread "<< dwGetTID() << endl;
		if(ct.bStart())
			cout << "Started Thread "<< dwGetTID() << endl;
		else
			cout << "Could Not Start Thread "<< dwGetTID() << endl;
	}
	else cout << "Did Not Create Thread" << endl;
	
	if(ct.dwSuspend()!= 0xFFFFFFFF) cout << "Thread Suspended" << endl;
	else cout << "Error Thread not Suspended" << endl;
	//  cout << hex << ct.dwSuspend() << endl;
	
	Sleep(1000);
	
	if(ct.dwResume()!= 0xFFFFFFFF) cout << "Thread Resumed" << endl;
	else cout << "Error Thread not Resumed" << endl;
	//  cout << hex <<ct.dwResume() << endl;
	
	if (ct2.bCreate())
	{
		if(ct2.bStart())
			cout << "Started Thread2 "<< dwGetTID() <<endl;
		else
			cout << "Could Not Start Thread2 "<< dwGetTID() <<endl;
	}
	else cout << "Did Not Create Thread 2" << endl;
	cout << "Press any key to Exit"<< endl;
	Sleep(2000);
	WaitForKeystroke();

	return 0;
}

#endif
*/
/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"  //Precompiled header
#include "../threadpool.h"  //Local

/////////////////////////////////////////////////////////////////////////////
// Constructor - Initializes member variables and call initialization
// procedure.
/////////////////////////////////////////////////////////////////////////////

WorkerThreadPool::WorkerThreadPool(unsigned int cThreads, LPTHREAD_SMART_ROUTINE pWorkerFunc, HANDLE hShutdownEvent/*=NULL*/):
m_fInit(false),
m_cThreads(0),
m_rghThreads(NULL),
m_rgThreadParams(NULL),
m_rghFinishedEvents(NULL),
m_hShutdown(hShutdownEvent){
	Init(cThreads, pWorkerFunc, hShutdownEvent);
}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Destructor - signals shutdown event then waits on all threads finished
// events before closing all handles and freeing allocated memory for member
// variables.
/////////////////////////////////////////////////////////////////////////////

WorkerThreadPool::~WorkerThreadPool(){

	//If we are not initialized, no need to cleanup
	if (!m_fInit)
		return;

	//Signal shutdown
	SetEvent(m_hShutdown);

	//Make sure that both events have been thrown, this signifies proper shutdown, not just being "finished"
	for (unsigned int i=0; i<m_cThreads; i++)
		if ((WAIT_TIMEOUT==WaitForSingleObject(m_rgThreadParams[i].rgEvents[EVENTS_FINISHED],1000))||
			(WAIT_TIMEOUT==WaitForSingleObject(m_rgThreadParams[i].rgEvents[EVENTS_TRIGGER],1000)))
			if (NULL!=m_rghThreads[i])
				TerminateThread(m_rghThreads[i],-1);

	//Close thread handles and free thread list
	if (NULL!=m_rghThreads){
		for (unsigned int i=0; i<m_cThreads; i++)
			CloseHandle(m_rghThreads[i]);
		delete [] m_rghThreads;
	}//endif

	//Close trigger and shutdown events, delete thread data and then free parameter structure list
	if (NULL!=m_rgThreadParams){
		for (unsigned int i=0; i<m_cThreads; i++){
			if (NULL!=m_rgThreadParams[i].rgEvents[EVENTS_TRIGGER])
				CloseHandle(m_rgThreadParams[i].rgEvents[EVENTS_TRIGGER]);
			if (NULL!=m_rgThreadParams[i].rgEvents[EVENTS_FINISHED])
				CloseHandle(m_rgThreadParams[i].rgEvents[EVENTS_FINISHED]);
			if (NULL!=m_rgThreadParams[i].pData)
				delete [] m_rgThreadParams[i].pData;
		}//endfor
		delete [] m_rgThreadParams;
	}//endif

	//Free finished events list, no need to close handles since that was done in the parameter structure list; this is a copy
	if (NULL!=m_rghFinishedEvents){
		delete [] m_rghFinishedEvents;
	}//endif

	//Close shutdown event
	if (true==m_fOwnShutdownEvent){
		CloseHandle(m_hShutdown);
	}//endif

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Init - Store how many threads we have, create all events and event lists, 
// and start all threads.
/////////////////////////////////////////////////////////////////////////////

unsigned int WorkerThreadPool::Init(unsigned int cThreads, LPTHREAD_SMART_ROUTINE pWorkerFunc, HANDLE hShutdownEvent){
	
	//Check parameters or whether we have been initialized before somehow
	if ((true==m_fInit) || (NULL==pWorkerFunc) || (0==cThreads))
		return m_cThreads;  //return number of worker threads

	DWORD dwID=0;  //Temp variable for thread ID

	//Store number of threads
	m_cThreads=cThreads;

	//Allocate memory for lists
	m_rghThreads=new HANDLE[m_cThreads];
	m_rgThreadParams=new ThreadParam[m_cThreads];
	m_rghFinishedEvents=new HANDLE[m_cThreads];

	//Create shutdown event
	if (NULL!=m_hShutdown){
		m_fOwnShutdownEvent=false;
	}else{
		m_fOwnShutdownEvent=true;
		m_hShutdown=CreateEvent(NULL,true,false,NULL);
	}//endif

	//Populate lists and start threads
	for (unsigned int i=0; i<m_cThreads; i++){
		m_rgThreadParams[i].rgEvents[EVENTS_FINISHED]=m_rghFinishedEvents[i]=CreateEvent(NULL,true,true,NULL);
		m_rgThreadParams[i].rgEvents[EVENTS_TRIGGER]=CreateEvent(NULL,true,false,NULL);
		m_rgThreadParams[i].rgEvents[EVENTS_SHUTDOWN]=m_hShutdown;
		m_rgThreadParams[i].pFunc=pWorkerFunc;
		m_rgThreadParams[i].pData=NULL;
		m_rghThreads[i]=CreateThread(NULL,0,&WorkerThread,(LPVOID)&m_rgThreadParams[i],0, &dwID);
	}//endfor
	
	//We have successfully initialized	
	m_fInit=true;

	return m_cThreads; //Return number of threads

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// WorkerThread - The worker thread which wraps the user worker function.
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI WorkerThreadPool::WorkerThread(LPVOID pParam){

	//If parameter pointer is NULL then shutdown
	if (NULL==pParam)
		ExitThread(E_FAIL);

	//Cast back the parameters
	HANDLE* rgEvents; 
	rgEvents=((ThreadParam*)pParam)->rgEvents;

	DWORD dwReturn=0; //temp return value from worker function
	DWORD dwObject=0; //signaled object
	while (true){
		//Wait to be triggered or shutdown
		dwObject=WaitForMultipleObjects(2,&rgEvents[0],false,INFINITE);
		if (EVENTS_SHUTDOWN==(dwObject-WAIT_OBJECT_0))
			break;
		//Reset the trigger
		ResetEvent(rgEvents[EVENTS_TRIGGER]);
		//Do work
		dwReturn=((ThreadParam*)pParam)->pFunc(((ThreadParam*)pParam)->pData,rgEvents[EVENTS_SHUTDOWN]);
		//Signal finished
		SetEvent(rgEvents[EVENTS_FINISHED]);
	}//endwhile

	//We are shutting down, trigger both events when done
	SetEvent(rgEvents[EVENTS_FINISHED]);
	SetEvent(rgEvents[EVENTS_TRIGGER]);

	ExitThread(S_OK);
	return S_OK;  //Success

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// DoWork - Assign work to one of the available threads in the thread pool.
// This operation blocks until the task has been assigned to an available 
// thread.  If no thread is available, the task blocks until one is.
/////////////////////////////////////////////////////////////////////////////

HRESULT WorkerThreadPool::DoWork(LPVOID pData){

	//Check if we have been initialized
	if (!m_fInit){
		return E_FAIL;  //Not initialized
	}//endif

	//Wait for any thread to become available
	DWORD dwRet=0;
	unsigned int dIndex=0;
	dwRet=WaitForMultipleObjects(m_cThreads,m_rghFinishedEvents,false,INFINITE);
	dIndex=dwRet-WAIT_OBJECT_0;

	//Shutdown time
	if (WAIT_OBJECT_0==WaitForSingleObject(m_hShutdown,0))
		return S_FALSE;

	//Reset the finished event on this thread so we know when it is done
	ResetEvent(m_rghFinishedEvents[dIndex]);

	//Delete previous data used by this thread
	if (NULL!=m_rgThreadParams[dIndex].pData){
		delete [] m_rgThreadParams[dIndex].pData;
		m_rgThreadParams[dIndex].pData=NULL;
	}//enidf

	//Assign new data for thread to process
	m_rgThreadParams[dIndex].pData=pData;

	//Signal thread to start processing
	SetEvent(m_rgThreadParams[dIndex].rgEvents[EVENTS_TRIGGER]);

	return S_OK;  //Success

}//endmethod

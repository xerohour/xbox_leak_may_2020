#ifndef __TNCONTROL_JOBQUEUE__
#define __TNCONTROL_JOBQUEUE__
//#pragma message("Defining __TNCONTROL_JOBQUEUE__")






//==================================================================================
// Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG




//==================================================================================
// Master jobs
//==================================================================================
// Handle a report from the specified slave.
#define TNMJ_HANDLEREPORT		1

// Get the next command for the specified slave.
#define TNMJ_GETNEXTCMD			2

// Ping all the slaves currently in the session to make sure they're alive.
#define TNMJ_PINGSLAVES			3

// Indicate the specified slave is now ready to test.
#define TNMJ_READYTOTEST		4

// Close the session to prevent new joiners.
#define TNMJ_CLOSESESSION		5




//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNJob:public LLITEM
{
	public:
		DWORD				m_dwOperation; // ID of work to be performed
		DWORD				m_dwTime; // time the job should be performed
		TNCTRLMACHINEID		m_idSlave; // ID of slave to perform operation on, if any
		PVOID				m_pvContext; // job specific context for operation


		CTNJob(DWORD dwOperation, DWORD dwTime, PTNCTRLMACHINEID pSlaveID, PVOID pvContext);
		virtual ~CTNJob(void);
};

class DLLEXPORT CTNJobQueue:public LLIST
{
	// These are friends so they can access the protected members.
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend DWORD WINAPI MasterJobThread(LPVOID lpvParameter);


	protected:
		HANDLE		m_hNewJobEvent; // there is something new in the queue


		CTNJobQueue(void);
		virtual ~CTNJobQueue(void);

		HRESULT AddJob(DWORD dwOperation, DWORD dwDelay,
						PTNCTRLMACHINEID pSlaveID, PVOID pvContext);
};






#else //__TNCONTROL_JOBQUEUE__
//#pragma message("__TNCONTROL_JOBQUEUE__ already included!")
#endif //__TNCONTROL_JOBQUEUE__

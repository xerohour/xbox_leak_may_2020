#ifndef __TNCONTROL_TESTSTATS__
#define __TNCONTROL_TESTSTATS__
//#pragma message("Defining __TNCONTROL_TESTSTATS__")






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
// External Classes
//==================================================================================
class DLLEXPORT CTNTestStats
{
	// These are friends so they can access the event.
#ifndef _XBOX // ! no master supported
	friend class CTNMaster;
#endif // ! XBOX
	friend class CTNSlave;

	private:
		CRITICAL_SECTION	m_cs; // critical section used to prevent simultaneous modification
		DWORD				m_dwSuccesses; // number of successes for this test/machine/all tests
		DWORD				m_dwFailures; // number of failure for this test/machine/all tests
		DWORD				m_dwWarnings; // number of warnings for this test/machine/all tests


	public:
		DLLSAFE_NEWS_AND_DELETES;

		CTNTestStats(void);
		virtual ~CTNTestStats(void);

		void Lock(void);
		void Unlock(void);

		DWORD GetSuccesses(void);
		DWORD GetFailures(void);
		DWORD GetWarnings(void);
		DWORD GetTotalComplete(void);

		void IncrementSuccesses(void);
		void IncrementFailures(void);
		void IncrementWarnings(void);

		HRESULT Copy(PTNTESTSTATS pStatsToCopy);
};






#else //__TNCONTROL_TESTSTATS__
//#pragma message("__TNCONTROL_TESTSTATS__ already included!")
#endif //__TNCONTROL_TESTSTATS__

#ifndef __TNCONTROL_CONTROL__
#define __TNCONTROL_CONTROL__
//#pragma message("Defining __TNCONTROL_CONTROL__")






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
class DLLEXPORT CTNControlLayer
{
	// These are friends so they can access the protected members
	friend CTNExecutor;
	friend CTNExecutorPriv;


	protected:
		PTNLOGSTRINGPROC	m_pfnLogString; // callback to use to log strings for the user
		PTNCTRLCOMM			m_pCtrlComm; // pointer to control method we're using
		HANDLE				m_hCompletionOrUpdateEvent; // handle to event to set when session status changes or stats are updated


	public:
		DLLSAFE_NEWS_AND_DELETES;

		CTNControlLayer(void);
		virtual ~CTNControlLayer(void);


		HRESULT Log(DWORD dwLogStringType, char* szFormatString, DWORD dwNumParms, ...);

		// This is a pure virtual function, derived classes must implement it because
		// we sure don't.
		virtual HRESULT HandleMessage(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									  PVOID pbData, DWORD dwDataSize) = 0;
};






#else //__TNCONTROL_CONTROL__
//#pragma message("__TNCONTROL_CONTROL__ already included!")
#endif //__TNCONTROL_CONTROL__

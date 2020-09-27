#ifndef _XBOX // reports not supported
#ifndef __DNTESTTOOLS_REPORTITEM__
#define __DNTESTTOOLS_REPORTITEM__
//#pragma message("Defining __DNTESTTOOLS_REPORTITEM__")






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
// Defines
//==================================================================================
#define RT_ADDMACHINE						1
#define RT_REMOVEMACHINE					2
#define RT_CLOSEDSESSION					3
#define RT_ALLTESTINGCOMPLETE				4

#define RT_TESTSKIPPED						5

#define RT_TESTASSIGNED						101
#define RT_TESTASSIGNED_SUBTEST				102
#define RT_TESTCOMPLETELYASSIGNED			103
#define RT_TESTCOMPLETELYASSIGNED_SUBTEST	104
#define RT_SUCCESS							105
#define RT_SUCCESS_SUBTEST					106
#define RT_FAILURE							107
#define RT_FAILURE_SUBTEST					108
#define RT_WARNING							109
#define RT_WARNING_SUBTEST					110
#define RT_SUCCESSCOMPLETE					111
#define RT_SUCCESSCOMPLETE_SUBTEST			112
#define RT_FAILURECOMPLETE					113
#define RT_FAILURECOMPLETE_SUBTEST			114


#define RRM_DROPPED						0x00000001
#define RRM_LEFT						0x00000002
#define RRM_DONETESTING					0x00000003
#define RRM_TIMEREXPIRED				0x00000004



//==================================================================================
// Classes
//==================================================================================
class DLLEXPORT CTNReport:public LLITEM
{
	// Declare ReportList and master objects as friends so they can access
	// everything.
	friend class CTNReportsList;
#ifndef _XBOX // ! no master supported
	friend class CTNMaster;
#endif // ! XBOX


	protected:
		DWORD				m_dwType; // type of report, see RT_xxx above for possible values
		tm					m_time; // time the report was logged
		CTNSlavesList		m_machines; // list of machines this report pertains to
		PTNTESTINSTANCEM	m_pTest; // tests this report pertains to, if any
		HRESULT				m_hresult; // result code for this report
		DWORD				m_dwDataSize; // size of user data
		PVOID				m_pvData; // user specific data


	public:
		CTNReport(void);
		virtual ~CTNReport(void);


		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose, BOOL fBVTFormat);
};







#else //__DNTESTTOOLS_REPORTITEM__
//#pragma message("__DNTESTTOOLS_REPORTITEM__ already included!")
#endif //__DNTESTTOOLS_REPORTITEM__
#endif // ! XBOX
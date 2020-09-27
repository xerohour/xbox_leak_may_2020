#ifndef __TNCONTROL_STOREDATA__
#define __TNCONTROL_STOREDATA__
//#pragma message("Defining __TNCONTROL_STOREDATA__")






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




// These are the possible data locations for a stored data item

#define TNTSDL_REPORTASSIGN			1 // The strings came from/are initial data (from an tester assignment report)
#define TNTSDL_REPORTSUCCESS		2 // The strings came from/are going to a tester's success report
#define TNTSDL_REPORTFAILURE		3 // The strings came from/are going to a tester's failure report
#define TNTSDL_REPORTWARNING		4 // The strings came from/are going to a tester's warning
#define TNTSDL_REPORTFINALSUCCESS	5 // The strings came from/are going to a final success report
#define TNTSDL_REPORTFINALFAILURE	6 // The strings came from/are going to a final failure report

#define TNTSDL_SCRIPTSECTION		100 // The strings came from the script section




//==================================================================================
// External Classes
//==================================================================================
class DLLEXPORT CTNStoredData:public LLITEM
{
	// These are friends so they can access the protected data
	friend CTNStoredDataList;
#ifndef _XBOX // Reports not supported
	friend CTNReport;
#endif
	friend CTNTestInstanceM;
	friend HRESULT MasterReadFileCB(char* szLine, PFILELINEINFO pInfo,
									PVOID pvContext, BOOL* pfStopReading);


	protected:
		DWORD			m_dwLocation; // location where this item was stored (a script, a report, etc.)
		int				m_iTesterNum; // tester number this item is associated with
		CLStringList	m_strings; // the stored data strings


	public:
		CTNStoredData(void);
		virtual ~CTNStoredData(void);

		HRESULT AddString(char* szString);
};

class DLLEXPORT CTNStoredDataList:public LLIST
{
	public:
		CTNStoredDataList(void);
		virtual ~CTNStoredDataList(void);


		PLSTRINGLIST GetScriptInputDataStrings(void);

		PLSTRINGLIST GetReportStringsForTester(DWORD dwReportType, int iTesterNum,
												int iIndex);
};






#else //__TNCONTROL_STOREDATA__
//#pragma message("__TNCONTROL_STOREDATA__ already included!")
#endif //__TNCONTROL_STOREDATA__

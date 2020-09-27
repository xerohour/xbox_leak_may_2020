#ifndef __TNCONTROL_TESTFROMFILES__
#define __TNCONTROL_TESTFROMFILES__
//#pragma message("Defining __TNCONTROL_TESTFROMFILES__")






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
class DLLEXPORT CTNTestFromFile:public LLITEM
{
	// These are friends so they can access the protected members
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNTestFromFilesList;
#ifndef _XBOX // reports not supported
	friend CTNReport;
#endif // ! XBOX
	friend CTNTestInstanceM;


	protected:
		DWORD				m_dwUniqueID; // ID used to identify the test in the file, if any
		PTNTESTTABLECASE	m_pCase; // pointer to the test case as defined by the module
		char*				m_pszInstanceID; // user defined unique ID of this test instance
		char*				m_pszFriendlyName; // user specified friendly name of this test instance
		int					m_iNumMachines; // the number of machines actually going to run this test
		DWORD				m_dwPermutationMode; // permutation mode this test should be run in
		//DWORD				m_dwOptions; // options for this instance (TNTIO_xxx)
		PTNSTOREDDATALIST	m_pStoredDataList; // pointer to list of data strings loaded from file associated with this test
		PLSTRINGLIST		m_pTesterAssignments; // pointer to list of strings loaded from file that designate tester assignments
		DWORD				m_dwRepsRemaining; // number of repetitions of this test that are left to be run


		CTNTestFromFile(DWORD dwUniqueID,
						PTNTESTTABLECASE pCase,
						char* pszInstanceID,
						char* pszFriendlyName,
						int iNumMachines,
						DWORD dwPermutationMode,
						//DWORD dwOptions,
						PTNSTOREDDATA pStoredData,
						PLSTRINGLIST pTesterAssignments,
						DWORD dwRepsRemaining);

		virtual ~CTNTestFromFile(void);
};

class DLLEXPORT CTNTestFromFilesList:public LLIST
{
	// These are friends so they can access the protected members
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	//friend CTNSlaveInfo;


	protected:
		//CTNTestFromFilesList(void);
		//virtual ~CTNTestFromFilesList(void);

		PTNTESTFROMFILE GetTest(DWORD dwUniqueID);
};




#else //__TNCONTROL_TESTFROMFILES__
//#pragma message("__TNCONTROL_TESTFROMFILES__ already included!")
#endif //__TNCONTROL_TESTFROMFILES__

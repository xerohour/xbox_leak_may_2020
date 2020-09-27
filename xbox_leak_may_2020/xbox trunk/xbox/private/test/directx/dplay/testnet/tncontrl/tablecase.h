#ifndef __TNCONTROL_TESTTABLECASE__
#define __TNCONTROL_TESTTABLECASE__
//#pragma message("Defining __TNCONTROL_TESTTABLECASE__")






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
class DLLEXPORT CTNTestTableCase:public CTNTestTableItem
{
	public:
		char*					m_pszDescription; // pointer to this item's constant description string
		char*					m_pszInputDataHelp; // pointer to this item's constant input data help string

		int						m_iNumMachines; // the number of machines required to run this test
		DWORD					m_dwOptionFlags; // test options

		PTNCANRUNPROC			m_pfnCanRun; // callback for master to use to check if a given machine can run this
		PTNGETINPUTDATAPROC		m_pfnGetInputData; // callback for master to use to retrieve initial data
		PTNEXECCASEPROC			m_pfnExecCase; // callback for slave to use to execute the test case
		PTNWRITEDATAPROC		m_pfnWriteData; // callback for master to use when writing report or initial data to strings
		PTNFILTERSUCCESSPROC	m_pfnFilterSuccess; // callback for master to use when double checking that a success is really a success

		PTNGRAPHITEM			m_paGraphs; // array of graph items
		DWORD					m_dwNumGraphs; // number of graph items in the following array

		CTNTestStats			m_stats; // statistics for this case


		CTNTestTableCase(void);
		virtual ~CTNTestTableCase(void);


		// Virtual implementations
		HRESULT CalculateChecksum(LPCHECKSUM lpChecksum);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, DWORD dwOptions);
#endif // ! XBOX
};






#else //__TNCONTROL_TESTTABLECASE__
//#pragma message("__TNCONTROL_TESTTABLECASE__ already included!")
#endif //__TNCONTROL_TESTTABLECASE__

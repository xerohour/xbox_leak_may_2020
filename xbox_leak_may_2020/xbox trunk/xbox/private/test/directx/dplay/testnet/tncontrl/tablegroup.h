#ifndef __TNCONTROL_TESTTABLEGROUP__
#define __TNCONTROL_TESTTABLEGROUP__
//#pragma message("Defining __TNCONTROL_TESTTABLEGROUP__")






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
class DLLEXPORT CTNTestTableGroup:public CTNTestTableItem
{
	public:
		LLIST	m_items; // all table items contained in this group


		CTNTestTableGroup(void);
		//virtual ~CTNTestTableGroup(void);


		HRESULT CalculateChecksum(LPCHECKSUM lpChecksum);

		HRESULT GetNumberOfCases(int* piTotal);

		HRESULT GetCaseIndex(int* piNumItemsLeft, PTNTESTTABLECASE* ppCase);

		PTNTESTTABLEGROUP NewGroup(char* pszID, char* pszName);
		HRESULT NewSubGroup(char* pszID, char* pszName, PTNTESTTABLEGROUP* ppNewSubGroup);

		HRESULT AddTest(char* pszCaseID, char* pszName, int iNumMachines,
						DWORD dwOptionFlags, PTNTESTCASEPROCS pProcs,
						DWORD dwNumGraphs, PTNGRAPHITEM paGraphs);
		HRESULT AddTest(PTNADDTESTDATA pAddTestData);

		PTNTESTTABLECASE GetTest(char* szID);


#ifndef _XBOX // no file printing
		// Virtual implementations
		HRESULT PrintToFile(HANDLE hFile, DWORD dwOptions);
#endif // ! XBOX
};






#else //__TNCONTROL_TESTTABLEGROUP__
//#pragma message("__TNCONTROL_TESTTABLEGROUP__ already included!")
#endif //__TNCONTROL_TESTTABLEGROUP__

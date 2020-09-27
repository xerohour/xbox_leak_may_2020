#ifndef __TNCONTROL_RESULTCHAIN__
#define __TNCONTROL_RESULTCHAIN__
//#pragma message("Defining __TNCONTROL_RESULTCHAIN__")






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
class DLLEXPORT CTNResult:public LLITEM
{
	// These are friends so they can access the protected members
	friend CTNResultsChain;
	friend CTNExecutorPriv;
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
#ifndef _XBOX // no IPC supported
	friend CTNLeech;
#endif // ! XBOX


	private:
		PTNTESTINSTANCES	m_pTest; // test that generated this result



	protected:
		HRESULT				m_hresult; // success or error code
		BOOL				m_fComplete; // is this the last result for the test?
		BOOL				m_fSuccess; // is the result a success?
		PVOID				m_pvOutputData; // pointer to output data
		DWORD				m_dwOutputDataSize; // size of output data
		BOOL				m_fUnderDontSaveTest; // is this result for a subtest under a DONTSAVERESULTS test?
		CTNOutputVarsList	m_vars; // list of variables set inside output data


		CTNResult(PTNTESTINSTANCES pTest);
		virtual ~CTNResult(void);


	public:
		// These are public because we want the module's ExecCase functions to be
		// be able to retrieve them.

		HRESULT SetResultCodeAndBools(HRESULT hresult, BOOL fTestComplete, BOOL fSuccess);
		PVOID CreateOutputDataBuffer(DWORD dwBufferSize);
		void DestroyOutputDataBufferAndVars(void);
		HRESULT GetOutputData(PVOID* ppvData, DWORD* pdwDataSize);
		HRESULT SetOutputVariable(char* szName, char* szType, PVOID pvValueLocation,
								DWORD dwValueSize);
		HRESULT GetOutputVariable(char* szName, char* szType, PVOID* ppvValueLocation,
								DWORD* pdwValueSize);
		HRESULT GetOutputVariableOfType(char* szType, char** ppszName,
										PVOID* ppvValueLocation, DWORD* pdwValueSize);
		BOOL IsCase(char* szID);
		BOOL IsSuccess(void);
		HRESULT GetResultCode(void);
};

class DLLEXPORT CTNResultsChain:public LLIST
{
	// These are friends so they can access the protected members.

#ifndef _XBOX // no IPC supported
	friend CTNLeech;
#endif // ! XBOX

	//friend CTNSlave;
	friend CTNExecutorPriv;
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX


	protected:
		PTNRESULT NewResult(PTNTESTINSTANCES pTest);
		PTNRESULT GetResultForTest(PTNTESTINSTANCES pTest);


	public:
		/*
		CTNResultsChain(void);
		virtual ~CTNResultsChain(void);
		*/

		PTNRESULT GetMostRecentResult(char* szTestCaseID);
		HRESULT GetMostRecentOutputVar(char* szName, char* szType,
										PVOID* ppvValueLocation,
										DWORD* pdwValueSize);
		HRESULT GetMostRecentOutputVarOfType(char* szType, char** ppszName,
											PVOID* ppvValueLocation,
											DWORD* pdwValueSize);
};






#else //__TNCONTROL_RESULTCHAIN__
//#pragma message("__TNCONTROL_RESULTCHAIN__ already included!")
#endif //__TNCONTROL_RESULTCHAIN__

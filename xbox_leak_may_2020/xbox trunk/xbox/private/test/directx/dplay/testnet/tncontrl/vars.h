#ifndef __TNCONTROL_VARS__
#define __TNCONTROL_VARS__
//#pragma message("Defining __TNCONTROL_VARS__")






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
class DLLEXPORT CTNOutputVar:public CLString
{
	// These are friends so they can access the protected members
	friend CTNOutputVarsList;
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX
	friend CTNExecutorPriv;


	protected:
		char*		m_pszType; // string representatin of the value
		LPVOID		m_pvValue; // pointer to value
		DWORD		m_dwValueSize; // size of value


	public:
		CTNOutputVar(char* szName, char* szType, PVOID pvValue, DWORD dwValueSize);
		virtual ~CTNOutputVar(void);
};

class DLLEXPORT CTNOutputVarsList:public CLStringList
{
	// These are friends so they can access the protected members
	friend CTNResult;
#ifndef _XBOX // ! no master supported
	friend CTNMaster;
#endif // ! XBOX

#ifndef _XBOX // no IPC supported
	friend CTNLeech;
#endif // ! XBOX


	protected:
		PVOID	m_pvOutputDataStart; // current starting point for output data buffer (used for offsets)


	public:
		CTNOutputVarsList(void);


		HRESULT AddVariable(char* szName, char* szType, PVOID pvValueLocation,
							DWORD dwValueSize);
		HRESULT GetVariable(char* szName, char* szType, PVOID* ppvValueLocation,
							DWORD* pdwValueSize);
		HRESULT GetFirstVariableOfType(char* szType, char** ppszName,
										PVOID* ppvValueLocation,
										DWORD* pdwValueSize);

		HRESULT PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize);
		HRESULT UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize);
};






#else //__TNCONTROL_VARS__
//#pragma message("__TNCONTROL_VARS__ already included!")
#endif //__TNCONTROL_VARS__

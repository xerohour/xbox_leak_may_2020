#ifndef __TNSYMBOLS__
#define __TNSYMBOLS__





//==================================================================================
// Defines
//==================================================================================
#ifdef LOCAL_TNCOMMON

	// define LOCAL_TNCOMMON when including this code directly into your project
	#ifdef DLLEXPORT
		#undef DLLEXPORT
	#endif // DLLEXPORT defined
	#define DLLEXPORT

#else // ! LOCAL_TNCOMMON

	#ifdef TNCOMMON_EXPORTS

		// define TNCOMMON_EXPORTS only when building the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllexport)

	#else // ! TNCOMMON_EXPORTS

		// default behavior is to import the functions from the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllimport)

	#endif // ! TNCOMMON_EXPORTS
#endif // ! LOCAL_TNCOMMON

#ifndef DEBUG
	#ifdef _DEBUG
		#define DEBUG
	#endif // _DEBUG
#endif // DEBUG not defined


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// You tried to use a TNSym function before successfully calling TNSymInitialize
#define TNSYMERR_UNINITIALIZED		0x88770140 // DPERR_UNINITIALIZED










//==================================================================================
// External Prototypes
//==================================================================================
DLLEXPORT HRESULT TNSymInitialize(void);

DLLEXPORT HRESULT TNSymCleanup(void);

DLLEXPORT HRESULT TNSymSetOptions(BOOL fResolveUndecorated);

DLLEXPORT HRESULT TNSymResolve(PVOID pvPointer, char* szString, DWORD dwStringSize);

DLLEXPORT HRESULT TNSymLoadSymbolsForModules(char* szModulesList);

DLLEXPORT HRESULT TNSymGetModuleNameAndOffsetForPointer(PVOID pvPointer,
														char* pszName,
														DWORD dwNameSize,
														PDWORD_PTR pdwOffset);

DLLEXPORT BOOL TNSymGetDisableSymResolutionSetting(void);

DLLEXPORT void TNSymSetDisableSymResolutionSetting(BOOL fDisable);






#endif // __TNSYMBOLS__
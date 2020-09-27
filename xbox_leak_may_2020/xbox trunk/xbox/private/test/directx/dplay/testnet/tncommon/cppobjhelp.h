#ifndef __CPP_OBJECT_HELP__
#define __CPP_OBJECT_HELP__





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




//==================================================================================
// DLL safe memory/object allocation routines and macros.
//==================================================================================
#ifdef DEBUG
#ifndef NO_CPPOBJHELP_LEAKCHECK
DLLEXPORT void InitCPPObjHelp(void);
DLLEXPORT void CheckForCPPObjLeaks(void);
DLLEXPORT void CleanupCPPObjHelp(void);
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // DEBUG

DLLEXPORT void* DLLSafeNew(unsigned int cb);
DLLEXPORT void DLLSafeDelete(void* p);

// You should use this macro in your own classes if they don't inherit from a class
// that already uses this (i.e. not derived from the classes in this header).
#define DLLSAFE_NEWS_AND_DELETES	inline void* operator new(size_t cb)\
									{\
										return (DLLSafeNew(cb));\
									};\
									inline void* operator new[](size_t cb)\
									{\
										return (DLLSafeNew(cb));\
									};\
									inline void operator delete(void* p)\
									{\
										DLLSafeDelete(p);\
									};\
									inline void operator delete[](void* p)\
									{\
										DLLSafeDelete(p);\
									}




#endif //__CPP_OBJECT_HELP__

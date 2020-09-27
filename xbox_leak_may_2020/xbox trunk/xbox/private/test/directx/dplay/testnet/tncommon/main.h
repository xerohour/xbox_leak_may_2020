#ifndef __TNCOMMON_MAIN__
#define __TNCOMMON_MAIN__
//#pragma message("Defining __TNCOMMON_MAIN__")




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

#undef DEBUG_MODULE
#define DEBUG_MODULE	"TNCOMMON"




//==================================================================================
// Debugging help
//==================================================================================
// This is the version of the DLL
extern char		g_szTNCOMMONVersion[];





//==================================================================================
// External Statics
//==================================================================================
extern HINSTANCE		s_hInstance;







#else //__TNCOMMON_MAIN__
//#pragma message("__TNCOMMON_MAIN__ already included!")
#endif //__TNCOMMON_MAIN__

#include "resource.h"


// The following ifdef block is the standard way of creating macros which make exporting 
// from a dll simpler. All files within this DLL are compiled with the NOTHING_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NOTHING_API functions as being imported from a DLL, wheras this Dll sees symbols 
// defined with this macro as being exported.
#ifdef NOTHING_EXPORTS
#define NOTHING_API __declspec(dllexport)
#else
#define NOTHING_API __declspec(dllimport)
#endif

// This class is exported from the Nothing.dll
class NOTHING_API CNothing {
public:
	CNothing(void);
	// TODO: add your methods here.
};

extern NOTHING_API int nNothing;

NOTHING_API int fnNothing(void);


#ifndef __TNCONTROL_EXCEPTIONS__
#define __TNCONTROL_EXCEPTIONS__
//#pragma message("Defining __TNCONTROL_EXCEPTIONS__")






//==================================================================================
// Common Defines
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
// Exceptino handling helper functions for macros
//==================================================================================
DLLEXPORT HRESULT TNHandleCatchExceptionList(PTNEXCEPTIONSLIST pExceptions,
											PTNTESTRESULT pTestResult);
DLLEXPORT HRESULT TNHandleCatchGeneric(void);









#else //__TNCONTROL_EXCEPTIONS__
//#pragma message("__TNCONTROL_EXCEPTIONS__ already included!")
#endif //__TNCONTROL_EXCEPTIONS__

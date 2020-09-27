#ifndef __TEMPLATE_MAIN__
#define __TEMPLATE_MAIN__
//#pragma message("Defining __TEMPLATE_MAIN__")






//==================================================================================
// Common Defines
//==================================================================================
#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)


//#ifdef DPVTEST_EXPORTS

//#ifdef DLLEXPORT
//#undef DLLEXPORT
//#endif // ! DLLEXPORT
//#define DLLEXPORT __declspec(dllexport)

//#else // ! DPVTEST_EXPORTS

//#ifdef DLLEXPORT
//#undef DLLEXPORT
//#endif // ! DLLEXPORT
//#define DLLEXPORT __declspec(dllimport)

//#endif // ! DPVTEST_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG




//==================================================================================
// Defines
//==================================================================================
#undef DEBUG_MODULE
#define DEBUG_MODULE	"TEMPLATE"









#else //__TEMPLATE_MAIN__
//#pragma message("__TEMPLATE_MAIN__ already included!")
#endif //__TEMPLATE_MAIN__

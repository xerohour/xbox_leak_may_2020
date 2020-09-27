#ifndef __TNEXCEPTIONS__
#define __TNEXCEPTIONS__





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
// Class type definitions
//==================================================================================
typedef class CTNException		CTNException,		* PTNEXCEPTION;
typedef class CTNExceptionsList	CTNExceptionsList,	* PTNEXCEPTIONSLIST;





//==================================================================================
// Macros
//==================================================================================
#ifdef USE_TRY_CATCH


#define BEGIN_CONVERTEXCEPTIONS_BLOCK	DNB("Entering convertexceptions block.");\
										__try

#define END_CONVERTEXCEPTIONS_BLOCK		__except(TNExcptnConvertToClassFilter((PEXCEPTION_POINTERS) _exception_info(),\
																				&g_TNExceptions))\
										{\
											throw (&g_TNExceptions);\
										}\
										DNB("Leaving convertexceptions block.");


#else // ! USE_TRY_CATCH


#define BEGIN_CONVERTEXCEPTIONS_BLOCK	DNB("Entering block.");

#define END_CONVERTEXCEPTIONS_BLOCK		DNB("Leaving block.");


#endif // ! USE_TRY_CATCH




//==================================================================================
// External Prototypes
//==================================================================================
DLLEXPORT int TNExcptnConvertToClassFilter(PEXCEPTION_POINTERS pExceptionInfo,
											PTNEXCEPTIONSLIST pExceptionsList);




//==================================================================================
// Classes
//==================================================================================
class DLLEXPORT CTNException:public LLITEM
{
	public:
		CONTEXT		m_context;
		DWORD		m_dwExceptionCode;
		DWORD		m_dwExceptionFlags;
		PVOID		m_pvExceptionAddress;
		DWORD		m_dwNumberParameters;
		DWORD		m_adwExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];


		CTNException(LPCONTEXT lpContext,
					DWORD dwExceptionCode,
					DWORD dwExceptionFlags,
					PVOID pvExceptionAddress,
					DWORD dwNumberParameters,
					DWORD_PTR* padwExceptionInformation);

		//HRESULT GetDescription(void);
		HRESULT PrintSelf(void);
};

class DLLEXPORT CTNExceptionsList:public LLIST
{
};



//==================================================================================
// External globals
//==================================================================================
extern DLLEXPORT CTNExceptionsList	g_TNExceptions;






#endif // __TNEXCEPTIONS__
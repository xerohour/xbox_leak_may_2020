#ifdef XBOX
extern long g_cComponent;
#define IncrementDLLCount() 
#define DecrementDLLCount()
#else
extern long g_cComponent;
#define IncrementDLLCount() InterlockedIncrement(&g_cComponent)
#define DecrementDLLCount() InterlockedDecrement(&g_cComponent)
extern bool g_fInitCS;
extern CRITICAL_SECTION g_CritSec;
#endif


#ifdef XBOX
#define ENTER_CRITICAL_SECTION(cr) EnterCriticalSection(cr);	
#define LEAVE_CRITICAL_SECTION(cr) LeaveCriticalSection(cr); 
#define INITIALIZE_CRITICAL_SECTION(cr) InitializeCriticalSection(cr);
#define DELETE_CRITICAL_SECTION(cr) DeleteCriticalSection(cr);
extern  CRITICAL_SECTION		g_APICriticalSection;	
extern  long					g_lCritSecCount;
#ifdef DBG
extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);
#define ENTER_API_CRITICAL_SECTION   /*EnterCriticalSection(&g_APICriticalSection);	*/
#define LEAVE_API_CRITICAL_SECTION   /*LeaveCriticalSection(&g_APICriticalSection); */
#else
#define ENTER_API_CRITICAL_SECTION   /*EnterCriticalSection(&g_APICriticalSection)  */
#define LEAVE_API_CRITICAL_SECTION   /*LeaveCriticalSection(&g_APICriticalSection)  */
#endif
#else
#define ENTER_CRITICAL_SECTION(cr) EnterCriticalSection(cr)
#define LEAVE_CRITICAL_SECTION(cr) LeaveCriticalSection(cr)
#define INITIALIZE_CRITICAL_SECTION(cr) InitializeCriticalSection(cr);
#define DELETE_CRITICAL_SECTION(cr) DeleteCriticalSection(cr);
#define ENTER_GLOBAL_CRITICAL_SECTION   
#define LEAVE_GLOBAL_CRITICAL_SECTION   
#endif
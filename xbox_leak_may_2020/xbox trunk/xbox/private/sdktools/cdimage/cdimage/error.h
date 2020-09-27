
#define GETLASTERROR 0xFFFFFFFF

void __cdecl WarnFailedSourceOpen( DWORD dwGLE, const char *szFormat, ... );
void __cdecl ErrorExit(  DWORD dwGLE, const char *szFormat, ... );
void ErrorExitV( DWORD dwGLE, const char *szFormat, va_list vaArgs );


LPSTR
ErrorTextFromErrorCode(
    IN  DWORD ErrorCode,
    OUT LPSTR ErrorText
    );


#ifdef DEBUG
    BOOL __inline Assert( const char *szText,
                          const char *szFile,
                          unsigned    line ) {
        ErrorExit( 0, "ASSERT( %s ) FAILED, %s (%d)\n", szText, szFile, line );
        return FALSE;
        }
#endif

typedef VOID ( *PTERMINATION_HANDLER )( PVOID pContext );

VOID
RegisterTerminationHandler(
    PTERMINATION_HANDLER pHandler,
    PVOID                pContext
    );

extern volatile BOOL bGlobalErrorExitInProgress;

void InitializeErrorHandler( void );



#include "precomp.h"
#pragma hdrstop

VOID
CallTerminationHandlers(
    VOID
    );


DWORD GetLastErrorFromNtStatus( DWORD NtStatus ) {

    OVERLAPPED Overlapped;

    Overlapped.Internal = NtStatus;
    Overlapped.hEvent   = NULL;

    GetOverlappedResult( NULL, &Overlapped, &Overlapped.InternalHigh, FALSE );

    return GetLastError();

    }


volatile BOOL bGlobalErrorExitInProgress;

CRITICAL_SECTION ErrorExitCritSect;


BOOL
WINAPI
MyConsoleCtrlHandler(
    DWORD dwCtrlType
    )
    {
    ErrorExit( 0, "Process terminated\r\n" );
    UNREFERENCED( dwCtrlType );
    return FALSE;
    }


void InitializeErrorHandler( void ) {
    InitializeCriticalSection( &ErrorExitCritSect );
    SetConsoleCtrlHandler( MyConsoleCtrlHandler, TRUE );
    }


void ErrorExitV( DWORD dwGLE, const char *szFormat, va_list vaArgs ) {

    CHAR Buffer[ 256 ];

    bGlobalErrorExitInProgress = TRUE;

    if ( dwGLE == GETLASTERROR )
         dwGLE = GetLastError();

    EnterCriticalSection( &ErrorExitCritSect );

    printf( "\r\n" );
    vfprintf( stdout, szFormat, vaArgs );

    if ( dwGLE )
        printf( "%s\r\n", ErrorTextFromErrorCode( dwGLE, Buffer ));

    fflush( stdout );

    CallTerminationHandlers();

#ifdef DEBUG

    if ( MyIsDebuggerPresent() )
        DebugBreak();

#endif

    ExitProcess( 1 );

    }


void __cdecl ErrorExit( DWORD dwGLE, const char *szFormat, ... ) {

    va_list vaArgs;

    bGlobalErrorExitInProgress = TRUE;

    if ( dwGLE == GETLASTERROR )
         dwGLE = GetLastError();

    va_start( vaArgs, szFormat );

    ErrorExitV( dwGLE, szFormat, vaArgs );  // never returns

    va_end( vaArgs );

    }


void __cdecl WarnFailedSourceOpen( DWORD dwGLE, const char *szFormat, ... ) {

    CHAR Buffer[ 256 ];
    va_list vaArgs;

    if ( dwGLE == GETLASTERROR )
         dwGLE = GetLastError();

    va_start( vaArgs, szFormat );

    if ( bContinueAfterFailedSourceOpen ) {

        printf( "\r\nWARNING: " );

        vfprintf( stdout, szFormat, vaArgs );

        if ( dwGLE )
            printf( "%s\r\n", ErrorTextFromErrorCode( dwGLE, Buffer ));

        printf( "(skipping file)\r\n" );

        }
    else {

        printf( "\r\nERROR: " );

        vfprintf( stdout, szFormat, vaArgs );

        ErrorExitV( dwGLE, "", vaArgs );    // never returns
        }

    va_end( vaArgs );
    }



typedef struct _TERM_HANDLER_NODE TERM_HANDLER_NODE, *PTERM_HANDLER_NODE;

struct _TERM_HANDLER_NODE {
    PTERM_HANDLER_NODE   pNextNode;
    PTERMINATION_HANDLER pHandler;
    PVOID                pContext;
    };

TERM_HANDLER_NODE TermHandlerList;

VOID
RegisterTerminationHandler(
    PTERMINATION_HANDLER pHandler,
    PVOID                pContext
    )
    {
    PTERM_HANDLER_NODE pNode = MyAllocNeverFree( sizeof( TERM_HANDLER_NODE ));

    pNode->pNextNode = TermHandlerList.pNextNode;
    pNode->pHandler  = pHandler;
    pNode->pContext  = pContext;

    TermHandlerList.pNextNode = pNode;

    }

VOID
CallTerminationHandlers(
    VOID
    )
    {
    PTERM_HANDLER_NODE pNode = TermHandlerList.pNextNode;

    while ( pNode ) {
        ( *pNode->pHandler )( pNode->pContext );
        pNode = pNode->pNextNode;
        }
    }


LPSTR
ErrorTextFromErrorCode(
    IN  DWORD ErrorCode,
    OUT LPSTR ErrorText
    )
    {
    UCHAR  Buffer[ 256 ];
    PUCHAR p, q;

    sprintf( ErrorText, ((LONG)ErrorCode > 0 ) ? "Error %d" : "Error 0x%x", ErrorCode );

    if ( FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        ErrorCode,
                        0x409,
                        (LPSTR) Buffer,
                        sizeof( Buffer ),
                        NULL )) {

        p = Buffer;                             //  source
        q = (PUCHAR) strchr( ErrorText, 0 );    //  target (append to ErrorText)

        *q++ = ':';                             //  append ": "
        *q++ = ' ';

        for (;;) {

            while ( *p > ' ' )                  //  copy word up to whitespace
                *q++ = *p++;

            while (( *p ) && ( *p <= ' ' ))     //  skip whitespace
                p++;

            if ( *p )                           //  if another word remaining
                *q++ = ' ';                     //    append space, continue
            else                                //  else
                break;                          //    break

            }

        *q = 0;                                 //  terminate string

        }

    return ErrorText;
    }



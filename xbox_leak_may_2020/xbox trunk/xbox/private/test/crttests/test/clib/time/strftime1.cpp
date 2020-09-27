
#if defined(_MT)

// Compile /MT or /MD
//
// Tests that setlocale() and strftime() and be used together in a multi-threaded 
// program.
//
#include <process.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <tchar.h>

#ifdef _M_IA64
#pragma warning( disable : 4244)
#endif

unsigned __stdcall work( void* pArg ) // This uses the locale info pointer.
{
    _TCHAR          str256[256];
    time_t          t;
    struct tm*      ptm;

    t = time(0);
    ptm = localtime( &t );

    for (int i = 0; i < 100; i++) 
        _tcsftime( str256, 256, _T("%a%b%c%d%x%y"), ptm );
    return 0;
}

int main()
{
    unsigned thrdid = 0;
    _beginthreadex( 0, 0, work, 0, 0, &thrdid );

    for (int i = 0; i < 1000; i++) // This changes the locale info pointer.
    {
        _tsetlocale(LC_ALL,_T("english"));
        _tsetlocale(LC_ALL,_T("german"));
        _tsetlocale(LC_ALL,_T("french"));
        _tsetlocale(LC_ALL,_T("spannis"));
        _tsetlocale(LC_ALL,_T("russian"));
    }
    _tprintf(_T("passed\n"));
    return 0;
}

#else

int main() { return 0; }

#endif 

#include <stdio.h>

extern "C" _declspec (dllexport) void _cdecl ProcTrace( const char * szName )
{
   //Just print out the name to stdout, and flush

   printf("%s\n", szName);
   fflush(stdout);
}

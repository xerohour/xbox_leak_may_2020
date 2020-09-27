
#include "deviceSrv.h"


/*

Routine Description:

    Formatted print function that sends the output through a connected SOCKET.

Arguments:

    SOCKET sock - connected socket
    char* format - printf style format specifier
    ... - arguments specified by the format specifier

Return Value:

    int - number of bytes sent out

Notes:

*/
int SockPrint(SOCKET sock, char* format, ...)
    {
    if(sock == INVALID_SOCKET) return 0;

    va_list args;
    va_start(args, format);

    char szBuffer[1024];

    int len = vsprintf(szBuffer, format, args);
    send(sock, szBuffer, len, 0);

    va_end(args);

    return len;
    }

void DebugPrint(char* format, ...)
    {
    va_list args;
    va_start(args, format);

    char szBuffer[1024];

    vsprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    va_end(args);
    }


bool stristr(char *string, char *substr, size_t len/*=0*/)
    {
    if(!string || ! substr) return false;
    if(!*string) return false;
    if(!len) len = strlen(substr);
    if(_strnicmp(string, substr, len) == 0) return true;

    return stristr(string+1, substr, len);
    }

#include <xtl.h>
#include <xdbg.h>
#include <httpsget.h>

#define    XRL_TEST_BIG1       "xolab-1:/read/00100000.000"
#define    XRL_TEST            "xolab-1:/read/00000001.001"
#define    XRL_TEST_BIG1       "xolab-1:/read/00100000.000"
#define    XRL_TEST_BIG2       "xolab-1:/read/00400000.001"
#define    XRL_HELLO           "boydm-xs:/XRLTest/hello.txt"
#define    XRL_ELDORADO        "boydm-xs:/XRLTest/Eldorado.mp3"

#define DEST_FILE1          L"t:\\test\\Eldo.mp3"
#define DEST_FILE2          L"t:\\test\\00100000.000"

//======================== The main function
//void __cdecl main()
extern "C" int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow)
{
    WSADATA WsaData;
    DWORD err;
    BYTE                membuff[2000];
    DWORD               memread;
    HRESULT             herr;
    DWORD               dwHTTPerr;

    XDBGWRN("Xonline", "Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if ( err != NO_ERROR)
    {
        XDBGWRN("Xonline", "Failed XnetInitialize with %d\n", err);
        return 1;
    }

    XDBGWRN("Xonline", "Calling WSAStartup...\n");
    if(WSAStartup(0x0101, &WsaData) == SOCKET_ERROR)
    {
        XDBGWRN("Xonline", "Error %d returned by WSAStartup\n", GetLastError());
        return 1;
    }
    
    _asm int 3;

    HTTPSGet(L"tonychen2", L"test.htm");

    // Shutdown WinSock subsystem.
    WSACleanup();

    XDBGWRN("Xonline", "Unloading XBox network stack...\n");
    XnetCleanup();

    return 0;
}

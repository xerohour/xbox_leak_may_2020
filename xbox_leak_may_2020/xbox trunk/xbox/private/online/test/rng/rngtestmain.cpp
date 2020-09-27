#include <xtl.h>
#include <xdbg.h>
#include <winsockx.h>


#if DBG
extern "C" ULONG XDebugOutLevel;
#endif


//======================== The main function
void __cdecl main()
{
    BYTE randomData[30];
    int i;
    
#if DBG
    XDebugOutLevel = XDBG_EXIT;
#endif

    XNetStartup(NULL);
    
    for (i=0; i<16; ++i)
    {
        XNetRandom( randomData, sizeof(randomData) );

        XDBGTRC("Xonline", ",0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X,0x%.2X",
            randomData[0],       randomData[1],        randomData[2],        randomData[3],        randomData[4],        randomData[5],        randomData[6],        randomData[7],
            randomData[8],        randomData[9],        randomData[10],        randomData[11],        randomData[12],        randomData[13],        randomData[14],        randomData[15]
            );
    }
    
    for (i=0; i<16; ++i)
    {
        XNetRandom( randomData, sizeof(randomData) );

        XDBGTRC("Xonline", "\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X\\x%.2X",
            randomData[0],       randomData[1],        randomData[2],        randomData[3],        randomData[4],        randomData[5],        randomData[6],        randomData[7],
            randomData[8],        randomData[9],        randomData[10],        randomData[11],        randomData[12],        randomData[13],        randomData[14],        randomData[15]
            );
    }
    
    XNetCleanup();

    return;
}

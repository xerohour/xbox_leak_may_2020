/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    savegame.c

Abstract:

    test XBox saved game related APIs

Author:

    John Daly (johndaly) 22-June-2000

Environment:

    X-Box

Notes:

    savegame - test these API's
        XCreateSaveGame()
        XDeleteSaveGame()
        XFindFirstSaveGame()
        XFindNextSaveGame()
        XFindClose()

        I could have done all kinds of fancy wrappers, etc to make this a
        bit smaller, but this style makes things very easy to follow

Revision History:

    22-June-2000     johndaly
    created

Current Status:

    APIs not really final yet!!!
    we only go to 1 disk here, we'll have to support going to all save devices
    in the final version of this test, and maybe the net team will want to port
    it to online storage

    Also, add some simple multithreaded scenarios when time available

--*/

//
// includes
//

#include "savegame.h"

using namespace SaveGameNameSpace;

VOID
WINAPI
test_XSignature(
    HANDLE LogHandle
    )
/*++

Routine Description:

    test Signature functions
    XCalculateSignatureBegin()
    XCalculateSignatureUpdate()
    XCalculateSignatureEnd()

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs
    char uDrive - drive to use

Return Value:

    None

Notes:


--*/
{
    LONG ID;
    int x;
    DWORD y;
    DWORD BufferSize;
    DWORD RetVal;
    DWORD LastError;
    char *tmpPtr;
    HANDLE hSignatureIn;
    HANDLE hSignatureOut;
    XCALCSIG_SIGNATURE SignatureIn;
    XCALCSIG_SIGNATURE SignatureOut;
    BYTE *TestBuffer;
    DWORD MAX_ITER = 100;

    //
    // positive test cases
    //

    xSetFunctionName(LogHandle, "test_XSignature");
    xStartVariation(LogHandle, "goodparam1");

    //
    // seed random number generator
    // may want to conside saving the seed sometime
    //

    xtsrand((unsigned)GetTickCount());

    //
    // test #1
    // 2-200k, sign and check
    // basically, do I always get the same result?
    // swap key types
    // we re-use the same buffer because we never change it
    //

    //
    // make a buffer and fill it with data
    // note - we don't use the end of the buffer, it is a spare block for buffer overrun testing
    //

    BufferSize = MAX_ITER * 2048;
    TestBuffer = (BYTE *)HeapAlloc(SaveGameNameSpace::HeapHandle, 0, BufferSize);
    if (NULL == TestBuffer) {
        OutputDebugString(L"Out of memory!");
        xEndVariation( LogHandle );
        return;
    }
    tmpPtr = (char *)TestBuffer;
    for (y = 1; y < BufferSize; y++) {
        *tmpPtr = (char)(y % 16);
        ++tmpPtr;
    }

    for (x = 1; x < (int)MAX_ITER; x++) {

        BufferSize = x * 2048;

        //
        // sign it (alternate key types 0 and 1)
        //

        hSignatureIn = XCalculateSignatureBegin(x%2);
        xLog(LogHandle,
             (INVALID_HANDLE_VALUE == hSignatureIn) ? XLL_FAIL : XLL_PASS,
             "XCalculateSignatureBegin(%d):", x%2);

        RetVal = XCalculateSignatureUpdate(hSignatureIn, TestBuffer, BufferSize);
        xLog(LogHandle,
             (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
             "XCalculateSignatureUpdate:hSignatureIn:0x%8.8x buffersize:0x%8.8x",
             hSignatureIn,
             BufferSize);

        if (ERROR_SUCCESS != XCalculateSignatureEnd(hSignatureIn, &SignatureIn)) {
            LastError = GetLastError();
            xLog(LogHandle,
                 XLL_FAIL,
                 "XCalculateSignatureEnd:hSignatureIn:0x%8.8x buffersize:0x%8.8x GetLastError:0x%8.8x",
                 hSignatureIn,
                 BufferSize,
                 LastError);
        } else {
            xLog(LogHandle,
                 XLL_PASS,
                 "XCalculateSignatureEnd()");
        }

        //
        // do it again
        //

        //
        // look for buffer overrun
        //
        
        tmpPtr = (char *)TestBuffer + BufferSize;
        ++*tmpPtr;

        hSignatureOut = XCalculateSignatureBegin(x%2);
        xLog(LogHandle,
             (INVALID_HANDLE_VALUE == hSignatureOut) ? XLL_FAIL : XLL_PASS,
             "XCalculateSignatureBegin(%d):", x%2);

        RetVal = XCalculateSignatureUpdate(hSignatureOut, TestBuffer, BufferSize);
        xLog(LogHandle,
             (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
             "XCalculateSignatureUpdate:hSignatureOut:0x%8.8x buffersize:0x%8.8x",
             hSignatureOut,
             BufferSize);

        if (ERROR_SUCCESS != XCalculateSignatureEnd(hSignatureOut, &SignatureOut)) {
            LastError = GetLastError();
            xLog(LogHandle,
                 XLL_FAIL,
                 "XCalculateSignatureEnd:hSignatureOut:0x%8.8x buffersize:0x%8.8x GetLastError:0x%8.8x",
                 hSignatureOut,
                 BufferSize,
                 LastError);
        } else {
            xLog(LogHandle,
                 XLL_PASS,
                 "XCalculateSignatureEnd()");
        }

        //
        // fix buffer
        //
        
        --*tmpPtr;

        //
        // Validate the signature
        //

        if (0 != memcmp(&SignatureIn, &SignatureOut, sizeof(XCALCSIG_SIGNATURE))) {
            xLog(LogHandle,
                 XLL_FAIL,
                 "signature Mismatch!:\nSignatureIn:%*.*c \nSignatureOut:%*.*c",
                 XCALCSIG_SIGNATURE_SIZE,
                 XCALCSIG_SIGNATURE_SIZE,
                 SignatureIn,
                 XCALCSIG_SIGNATURE_SIZE,
                 XCALCSIG_SIGNATURE_SIZE,
                 SignatureOut);
        } else {
            xLog(LogHandle,
                 XLL_PASS,
                 "signature Match");
        }
    }

    xEndVariation(LogHandle);

    //
    // negative test cases
    //

    xSetFunctionName(LogHandle, "test_XSignature");
    xStartVariation(LogHandle, "badparam1");

    //
    // randomly corrupt buffer memory, make sure we fail
    // 

    for (x = 1; x < (int)MAX_ITER; x++) {

        BufferSize = x * 2048;

        //
        // sign it (alternate key types 0 and 1)
        //

        hSignatureIn = XCalculateSignatureBegin(x%2);
        xLog(LogHandle,
             (INVALID_HANDLE_VALUE == hSignatureIn) ? XLL_FAIL : XLL_PASS,
             "XCalculateSignatureBegin(%d):", x%2);

        RetVal = XCalculateSignatureUpdate(hSignatureIn, TestBuffer, BufferSize);
        xLog(LogHandle,
             (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
             "XCalculateSignatureUpdate:hSignatureIn:0x%8.8x buffersize:0x%8.8x",
             hSignatureIn,
             BufferSize);

        if (ERROR_SUCCESS != XCalculateSignatureEnd(hSignatureIn, &SignatureIn)) {
            LastError = GetLastError();
            xLog(LogHandle,
                 XLL_FAIL,
                 "XCalculateSignatureEnd:hSignatureIn:0x%8.8x buffersize:0x%8.8x GetLastError:0x%8.8x",
                 hSignatureIn,
                 BufferSize,
                 LastError);
        } else {
            xLog(LogHandle,
                 XLL_PASS,
                 "XCalculateSignatureEnd()");
        }

        //
        // do it again, but change a random place in the buffer
        // make sure this works when changing the first byte in the buffer
        // make sure this works when changing the last byte in the buffer
        //

        switch (x) {
            // make sure this works when changing the first byte in the buffer
            case 1:
                tmpPtr = (char *)TestBuffer;
                break;

                // make sure this works when changing the last byte in the buffer
            case 2:
                tmpPtr = (char *)TestBuffer + BufferSize - 1;
                break;

                // change a random place in the buffer
            default:
                tmpPtr = (char *)TestBuffer + xtrand()%(BufferSize - 1);
                break;
        }
        ++*tmpPtr;

        hSignatureOut = XCalculateSignatureBegin(x%2);
        xLog(LogHandle,
             (INVALID_HANDLE_VALUE == hSignatureOut) ? XLL_FAIL : XLL_PASS,
             "XCalculateSignatureBegin(%d):", x%2);

        RetVal = XCalculateSignatureUpdate(hSignatureOut, TestBuffer, BufferSize);
        xLog(LogHandle,
             (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
             "XCalculateSignatureUpdate:hSignatureOut:0x%8.8x buffersize:0x%8.8x",
             hSignatureOut,
             BufferSize);

        if (ERROR_SUCCESS != XCalculateSignatureEnd(hSignatureOut, &SignatureOut)) {
            LastError = GetLastError();
            xLog(LogHandle,
                 XLL_FAIL,
                 "XCalculateSignatureEnd:hSignatureOut:0x%8.8x buffersize:0x%8.8x GetLastError:0x%8.8x",
                 hSignatureOut,
                 BufferSize,
                 LastError);
        } else {
            xLog(LogHandle,
                 XLL_PASS,
                 "XCalculateSignatureEnd()");
        }

        //
        // repair the buffer
        //

        --*tmpPtr;

        //
        // Validate the signature - should be different
        //

        if (0 == memcmp(&SignatureIn, &SignatureOut, sizeof(XCALCSIG_SIGNATURE))) {
            xLog(LogHandle,
                 XLL_FAIL,
                 "signature Match!:\nSignatureIn:%*.*x \nSignatureOut:%*.*x",
                 XCALCSIG_SIGNATURE_SIZE,
                 XCALCSIG_SIGNATURE_SIZE,
                 SignatureIn,
                 XCALCSIG_SIGNATURE_SIZE,
                 XCALCSIG_SIGNATURE_SIZE,
                 SignatureOut);
        } else {
            xLog(LogHandle,
                 XLL_PASS,
                 "signature Mismatch (on purpose...)");
        }
    }

    xEndVariation(LogHandle);

    //
    // free the buffer
    //

    HeapFree(SaveGameNameSpace::HeapHandle, 0, TestBuffer);

    return;
}

VOID
WINAPI
test_XCreateSaveGame(
    HANDLE LogHandle,
    char uDrive
    )
/*++

Routine Description:

    test XCreateSaveGame
    - test with 0 saved games
    - 1 saved game
    - until the disk runs out of space, and then some
    - bad params where they make sense
    - lpRootPathName 0...N long w/ UNICODE chars
    - lpSaveGameName 0...N long w/ UNICODE chars
    - lpIdentity 0...N long w/ UNICODE chars
    - 0...5+ identities
    - lpPathBuffer normal, NULL, too short
    - dwCreationDisposition = CREATE_NEW
    - dwCreationDisposition = OPEN_ALWAYS
    - dwCreationDisposition = OPEN_EXISTING

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs
    char uDrive - drive to use

Return Value:

    None

Notes:

    these APIs are preliminary
    for a 'saved game' we'll use files that vary in length from 10k to 1mb

--*/
{
    CHAR SaveRoot[] = {"U:\\"};
    WCHAR gamename[_MAX_DIR + 2] = {0};
    CHAR PathBuffer[_MAX_DIR + 2] = {0};
    DWORD RetVal = 0;
    DWORD x = 0;
    PWCHAR pwTestBuffer = NULL;
    PWCHAR pwTestBuffer1 = NULL;
    PWCHAR pwName;
    DWORD dwLoopCount;
    BOOL Flag;

    //
    // point to the correct drive
    //

    SaveRoot[0] = uDrive;
    
    //
    // positive test cases
    //

    xSetFunctionName( LogHandle, "XCreateSaveGame" );
    xStartVariation( LogHandle, "goodparam1" );

    //
    // #1
    // test with 0 saved games
    // there should be 0 saved games when we get here
    //

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game1",
                             OPEN_ALWAYS,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    xLog(LogHandle,
         (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
         "XCreateSaveGame(1): path = %s (Error=%d)",
         PathBuffer,
         RetVal);

    RetVal = SaveAGame(PathBuffer,
                       "test1",
                       "sav",
                       10000,
                       1);

    xLog(LogHandle,
         (RetVal) ? XLL_PASS : XLL_FAIL,
         "test_XCreateSaveGame(#1): path = %s",
         PathBuffer);

    //
    // #2
    // 1 saved game
    // there will be one if the first test succeeded
    //

    PathBuffer[0] = 0;

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game2",
                             OPEN_ALWAYS,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    xLog(LogHandle,
         (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
         "XCreateSaveGame(2): path = %s (Error=%d)",
         PathBuffer,
         RetVal);

    RetVal = SaveAGame(PathBuffer,
                       "test2",
                       "sav",
                       10000,
                       1);

    xLog(LogHandle,
         (RetVal) ? XLL_PASS : XLL_FAIL,
         "test_XCreateSaveGame(#2): path = %s",
         PathBuffer);

    //
    // #3
    // lpRootPathName 0...N long w/ UNICODE chars
    // this will fail early on
    // first pass should succeed, the rest should fail
    // start with a valid path and append a lot of junk and call
    //
    // this case is now invalid, since we only allow the root in the path name
    // so chop it out
    //

#if 0

#define TEST_BUFFER_LEN 1000

    Flag = TRUE;
    pwTestBuffer = (PWCHAR)HeapAlloc(SaveGameNameSpace::HeapHandle, HEAP_ZERO_MEMORY, TEST_BUFFER_LEN * 2);
    if (NULL == pwTestBuffer) {
        xLog(LogHandle,XLL_BLOCK,"Out of memory!");
        return;
    }
    wcscpy(pwTestBuffer, SaveRoot);
    pwTestBuffer1 = (PWCHAR)((size_t)pwTestBuffer + (2 * wcslen(pwTestBuffer)));

#if DBG_RIP
    dwLoopCount = 1;
#else
    dwLoopCount = TEST_BUFFER_LEN - (pwTestBuffer1 - pwTestBuffer);
#endif

    for (x = 0; x < dwLoopCount; x++) {
        swprintf(gamename,L"GAME#%x", x);

        PreRIPSaveRegs
        RetVal = XCreateSaveGame(pwTestBuffer,
                                 gamename,
                                 OPEN_ALWAYS,
                                 0,
                                 PathBuffer,
                                 sizeof(PathBuffer));

        if (ERROR_SUCCESS == RetVal && 0 != x) {
            swprintf(wcDbgMsgBuffer,
                     L"Savegame : XCreateSaveGame unexpectedly succeeded: %s\n",
                     PathBuffer);
            OutputDebugString(wcDbgMsgBuffer);
            Flag = FALSE;
        }

        //
        // append junk to end of pwTestBuffer
        //

        pwTestBuffer1 = AppendRandUnicodeChar(pwTestBuffer1);

    }
    xLog(LogHandle, (Flag) ? XLL_PASS : XLL_FAIL, "test_XCreateSaveGame(#3)");

    HeapFree(SaveGameNameSpace::HeapHandle, 0, pwTestBuffer);
#endif // if 0

    //
    // #4
    // lpSaveGameName 0...N long w/ UNICODE chars
    //

#define TEST_BUFFER_LEN 1000

    Flag = TRUE;
    pwTestBuffer = (PWCHAR)HeapAlloc(SaveGameNameSpace::HeapHandle, HEAP_ZERO_MEMORY, TEST_BUFFER_LEN * 2);
    if (NULL == pwTestBuffer) {
        xLog(LogHandle,XLL_BLOCK,"Out of memory!");
        return;
        }
    wcscpy(pwTestBuffer, L"GAMEZ");
    pwTestBuffer1 = (PWCHAR)((size_t)pwTestBuffer + (2 * wcslen(pwTestBuffer)));

#if DBG_RIP
    dwLoopCount = TEST_BUFFER_LEN - (pwTestBuffer1 - pwTestBuffer);
#else
    dwLoopCount = MAX_GAMENAME - (pwTestBuffer1 - pwTestBuffer);
#endif

    for (x = 0; x < dwLoopCount; x++) {
        PreRIPSaveRegs
        RetVal = XCreateSaveGame(SaveRoot,
                                 pwTestBuffer,
                                 OPEN_ALWAYS,
                                 0,
                                 PathBuffer,
                                 sizeof(PathBuffer));

        //
        // the length a game name can be is MAX_GAMENAME - all the other path stuff
        // like the GUID and other top level directories
        // initial testing shows this to be 239 unicode chars long
        //

        if (ERROR_SUCCESS != RetVal &&
            wcslen(pwTestBuffer) < MAX_GAMENAME ) {
            swprintf(wcDbgMsgBuffer,
                     L"Savegame : XCreateSaveGame failed: lpSaveGameName: %S\n",
                     PathBuffer);
            //OutputDebugString(wcDbgMsgBuffer);
            Flag = FALSE;
        }

        //
        // append junk to end of pwTestBuffer
        //

        pwTestBuffer1 = AppendRandUnicodeChar(pwTestBuffer1);

    }

    xLog(LogHandle, (Flag) ? XLL_PASS : XLL_FAIL, "test_XCreateSaveGame(#4)");

    HeapFree(SaveGameNameSpace::HeapHandle, 0, pwTestBuffer);

    //
    // #5
    // lpIdentity 0...N long w/ UNICODE chars
    // note: MAX_NICKNAME 32
    //

#if DBG_RIP
    #define TEST_BUFFER_LEN1  (MAX_NICKNAME + 2)
#else
    #define TEST_BUFFER_LEN1  MAX_NICKNAME
#endif

    Flag = TRUE;
    pwTestBuffer = (PWCHAR)HeapAlloc(SaveGameNameSpace::HeapHandle, HEAP_ZERO_MEMORY, TEST_BUFFER_LEN * 2);
    if (NULL == pwTestBuffer) {
        xLog(LogHandle,XLL_BLOCK,"Out of memory!");
        return;
    }

    pwTestBuffer1 = AppendRandUnicodeChar(pwTestBuffer);

    for (x = 0; x < TEST_BUFFER_LEN1 - 1; x++) {

        _try {
            PreRIPSaveRegs
            RetVal = XCreateSaveGame(SaveRoot,
                                     gamename,
                                     OPEN_ALWAYS,
                                     0,
                                     PathBuffer,
                                     sizeof(PathBuffer));
        }
        _except ( Eval_Exception( GetExceptionInformation(), 24)) {
            OutputDebugString(L"_except function body hit - bug?\n");
            // No code; this block never executed.
        }

        //
        // the length a Identity can be: MAX_NICKNAME 32
        // when this is exceeded we'll RIP...
        // so wrap it with the RIP Wrapper
        //

        if (ERROR_SUCCESS != RetVal &&
            wcslen(pwTestBuffer) < MAX_NICKNAME) {
            swprintf(wcDbgMsgBuffer,
                     L"Savegame : XCreateSaveGame failed: pwTestBuffer1: %s  Error: %d\n",
                     pwTestBuffer1,
                     RetVal);
            //OutputDebugString(wcDbgMsgBuffer);
            Flag = FALSE;
        }

        if (ERROR_SUCCESS != RetVal &&
            ERROR_INVALID_PARAMETER != RetVal) {
            swprintf(wcDbgMsgBuffer,
                     L"Savegame : XCreateSaveGame returned bad error code: %d\n",
                     RetVal);
            OutputDebugString(wcDbgMsgBuffer);
            Flag = FALSE;
        }

        //
        // append junk to end of pwTestBuffer
        //

        pwTestBuffer1 = AppendRandUnicodeChar(pwTestBuffer1);

    }

    xLog(LogHandle, (Flag) ? XLL_PASS : XLL_FAIL, "test_XCreateSaveGame(#5)");

    HeapFree(SaveGameNameSpace::HeapHandle, 0, pwTestBuffer);

    //
    // #6
    // lpPathBuffer normal, too short
    // just manipulate the size
    // this case is irrelevant because the buffer length never gets checked
    // so pass on 0 lenght
    //

    x = 0;
    do {
        RetVal = XCreateSaveGame(SaveRoot,
                                 L"game6",
                                 OPEN_ALWAYS,
                                 0,
                                 PathBuffer,
                                 x++);
    } while (strlen(PathBuffer) == x);
    --x;

    xLog(LogHandle,
         ((ERROR_SUCCESS == RetVal) && (x > 0)) ? XLL_FAIL : XLL_PASS,
         "XCreateSaveGame(#6): PathBuffer Length=%d chars : (Error=%d)",
         x,
         RetVal);

    //
    // #8
    // dwCreationDisposition = CREATE_NEW
    // create new, then try creating again to make sure it fails
    //

    PathBuffer[0] = 0;

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game8new",
                             CREATE_NEW,
                             XSAVEGAME_NOCOPY,
                             PathBuffer,
                             sizeof(PathBuffer));

    xLog(LogHandle,
         (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
         "XCreateSaveGame(8): path = %s (Error=%d)",
         PathBuffer,
         RetVal);

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game8new",
                             CREATE_NEW,
                             XSAVEGAME_NOCOPY,
                             PathBuffer,
                             sizeof(PathBuffer));

    xLog(LogHandle,
         (ERROR_ALREADY_EXISTS == RetVal) ? XLL_PASS : XLL_FAIL,
         "XCreateSaveGame(8): path = %s (Error=%d)",
         PathBuffer,
         RetVal);

    //
    // #9
    // dwCreationDisposition = OPEN_ALWAYS
    // re-use the last game
    //

    PathBuffer[0] = 0;

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game8new",
                             OPEN_ALWAYS,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    xLog(LogHandle,
         (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
         "XCreateSaveGame(9): path = %s (Error=%d)",
         PathBuffer,
         RetVal);

    //
    // #10
    // dwCreationDisposition = OPEN_EXISTING
    // re-use the last game
    //

    PathBuffer[0] = 0;

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game8new",
                             OPEN_EXISTING,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    xLog(LogHandle,
         (ERROR_SUCCESS == RetVal) ? XLL_PASS : XLL_FAIL,
         "XCreateSaveGame(10): path = %s (Error=%d)",
         PathBuffer,
         RetVal);

    //
    // the fail cases
    //

#if DBG_RIP
    PathBuffer[0] = 0;

    _try {
        PreRIPSaveRegs
        RetVal = XCreateSaveGame(SaveRoot,
                             L"game8newXXX",
                             OPEN_EXISTING,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

        xLog(LogHandle,
             (ERROR_FILE_NOT_FOUND == RetVal) ? XLL_PASS : XLL_FAIL,
             "XCreateSaveGame(10a): path = %s (Error=%d)",
             PathBuffer,
             RetVal);

        PreRIPSaveRegs
        RetVal = XCreateSaveGame(SaveRoot,
                                 L"game8newXXX",
                                 OPEN_EXISTING | OPEN_ALWAYS | CREATE_NEW,
                                 0,
                                 PathBuffer,
                                 sizeof(PathBuffer));

        xLog(LogHandle,
             (ERROR_INVALID_PARAMETER == RetVal) ? XLL_PASS : XLL_FAIL,
             "XCreateSaveGame(10b): path = %s (Error=%d)",
             PathBuffer,
             RetVal);

        //
        // dwCreationDisposition = OPEN_EXISTING
        // re-use the last game, call with 1 name, should fail
        //

        PathBuffer[0] = 0;

        PreRIPSaveRegs
        RetVal = XCreateSaveGame(SaveRoot,
                                 L"game8new",
                                 OPEN_EXISTING,
                                 0,
                                 PathBuffer,
                                 sizeof(PathBuffer));

        xLog(LogHandle,
             (ERROR_INVALID_PARAMETER == RetVal) ? XLL_PASS : XLL_FAIL,
             "XCreateSaveGame(10c): path = %s (Error=%d)",
             PathBuffer,
             RetVal);

    }
        _except ( Eval_Exception( GetExceptionInformation(), 28)) {
            OutputDebugString(L"_except function body hit - bug?\n");
            // No code; this block never executed.
    }


#else
#endif

    //
    // #11
    // until the disk runs out of space, and then some
    // we do this last since the next test variation deletes games
    // if MaxFiles != -1 then only make MaxFiles number of files
    //
    // note:
    // from FatCreateNewDirent()  [fastfat\dirsup.c]:
    //
    //  A reason why we might fail, unrelated to physical reasons,
    //  is that we constrain to 64k directory entries to match the
    //  restriction on Win95.  There are fundamental reasons to do
    //  this since searching a FAT directory is a linear operation
    //  and to allow FAT32 to toss us over the cliff is not permissable.
    //
    // So it looks like you should be able to fit 65535 DIRENTs in a FAT32 directory.
    // This includes files and directories.  If you end up using LFNs, each file or
    // directory will be at least 2 DIRENTs (and may be as many as 25 DIRENTs).  SFNs
    // are 1 DIRENT per file or directory.
    //



    if (MaxFiles == -1) {
        do {
            swprintf(gamename,L"GAME#%x", x++);
            RetVal = XCreateSaveGame(SaveRoot,
                                     gamename,
                                     OPEN_ALWAYS,
                                     0,
                                     PathBuffer,
                                     sizeof(PathBuffer));

            if (ERROR_SUCCESS != RetVal) {
                break;
            }

            RetVal = SaveAGame(PathBuffer,
                               "test11",
                               "sav",
                               10000,
                               1);
        } while ( RetVal );
    } else {
        for (x = 0; x < (DWORD)MaxFiles; x++) {
            swprintf(gamename,L"GAME#%x", x);
            RetVal = XCreateSaveGame(SaveRoot,
                                     gamename,
                                     OPEN_ALWAYS,
                                     0,
                                     PathBuffer,
                                     sizeof(PathBuffer));

            if (ERROR_SUCCESS != RetVal) {
                break;
            }

            #if 0    //DBG
            OutputDebugStringA(SaveRoot);
            OutputDebugStringW(TEXT("\\"));
            OutputDebugStringW(gamename);
            OutputDebugStringW(TEXT("\r\n"));
            #endif  // DBG

            RetVal = SaveAGame(PathBuffer,
                               "test11",
                               "sav",
                               10000,
                               1);
        }
    }

    xLog(LogHandle,
         (x) ? XLL_PASS : XLL_FAIL,
         "test_XCreateSaveGame(#11) : XCreateSaveGame called = %d times",
         x);

    //
    // #12
    // try saving 100 times - should fail 100 times
    // this could get tricky during stress since other things
    // could free up disk space, so not neccessarisarily an error
    // if it works...
    // also, if MaxFiles != -1, skip, since there will probably be space left.
    //

    if (-1 == MaxFiles) {
        for (x = 0; x < 100; x++) {
            swprintf(gamename,L"GAMEz%x", x);
            RetVal = XCreateSaveGame(SaveRoot,
                                     gamename,
                                     0,
                                     0,
                                     PathBuffer,
                                     sizeof(PathBuffer));

            if (ERROR_SUCCESS == RetVal) {
                swprintf(wcDbgMsgBuffer,
                         L"Savegame : XCreateSaveGame unexpectedly succeeded: %s\n",
                         PathBuffer);
                OutputDebugString(wcDbgMsgBuffer);
                xLog(LogHandle,
                     XLL_FAIL,
                     "test_XCreateSaveGame(#12) : XCreateSaveGame unexpectedly succeeded");
            }
        }
    }

    xEndVariation( LogHandle );

    return;
}

VOID
WINAPI
test_XDeleteSaveGame(
    HANDLE LogHandle,
    char uDrive    
    )
/*++

Routine Description:

    test XDeleteSaveGame
    - a disk full of saved games (thousands!)
    - test with 0 saved games
    - 1 saved game
    - bad params where they make sense
    - saved games with 0 files
    - saved games with 1 files
    - saved games with N files

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs
    char uDrive - drive to use

Return Value:

    None

Notes:

    this function expects that test_XCreateSaveGame() ran first and that
    there are thousands of saved games on the disk to delete

--*/
{
    XGAME_FIND_DATA xgData;
    HANDLE hSavedGames;
    CHAR PathBuffer[_MAX_DIR + 2] = {0};
    CHAR SaveRoot[] = {"U:\\"};
    DWORD RetVal = 0;
    BOOL Flag;

    //
    // point to the correct drive
    //

    SaveRoot[0] = uDrive;
    
    //
    // positive test cases
    //

    xSetFunctionName( LogHandle, "XDeleteSaveGame" );
    xStartVariation( LogHandle, "goodparam1" );

    //
    // #1
    // enumerate and delete games
    //

    Flag = TRUE;
    hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);

    if (INVALID_HANDLE_VALUE == hSavedGames) {
        OutputDebugString( TEXT("XFindFirstSaveGame returned INVALID_HANDLE_VALUE\n"));
        xLog(LogHandle,
             XLL_BLOCK,
             "test_XDeleteSaveGame(#1) : XFindFirstSaveGame failed (LastError=%d)",
             GetLastError());
        goto XD1;
    }

    do {
        RetVal = XDeleteSaveGame(SaveRoot, xgData.szSaveGameName);

        if (ERROR_SUCCESS != RetVal) {
            swprintf(wcDbgMsgBuffer,
                     L"Savegame : XDeleteSaveGame failed: %d, dir = %s : name = %S\n",
                     RetVal,
                     xgData.szSaveGameDirectory,
                     xgData.szSaveGameName);
            OutputDebugString(wcDbgMsgBuffer);
            Flag = FALSE;
        }
    } while (XFindNextSaveGame(hSavedGames, &xgData));

    xLog(LogHandle, (Flag) ? XLL_PASS : XLL_FAIL, "test_XDeleteSaveGame(#1)");

    XFindClose(hSavedGames);

    XD1:

    //
    // #2
    // test with 0 saved games
    //

    RetVal = XDeleteSaveGame(SaveRoot, xgData.szSaveGameName);

    if (ERROR_SUCCESS == RetVal) {
        swprintf(wcDbgMsgBuffer,
                 L"Savegame : XDeleteSaveGame unexpectedly succeeded: %d, dir = %s : name = %S\n",
                 RetVal,
                 xgData.szSaveGameDirectory,
                 xgData.szSaveGameName);
        xLog(LogHandle, XLL_FAIL, "test_XDeleteSaveGame(#2)");
    } else {
        xLog(LogHandle, XLL_PASS, "test_XDeleteSaveGame(#2)");
    }

    //
    // #3
    // 1 saved game
    //

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game3",
                             OPEN_ALWAYS,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    if (ERROR_SUCCESS == RetVal) {
        RetVal = SaveAGame(PathBuffer,
                           "test3",
                           "sav",
                           10000,
                           1);

        //
        // also add a directory here so we can make sure the delete works properly
        //

        strcat( PathBuffer, "Subdir" );
        if (FALSE == CreateDirectory( PathBuffer, NULL )) {
            OutputDebugString( TEXT("CreateDirectory returned FALSE\n"));
            xLog(LogHandle,
                 XLL_BLOCK,
                 "test_XDeleteSaveGame(#3) : CreateDirectory returned FALSE (LastError=%d)",
                 GetLastError());
        }
        
        Flag = TRUE;
        hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);

        if (INVALID_HANDLE_VALUE == hSavedGames) {
            OutputDebugString( TEXT("XFindFirstSaveGame returned INVALID_HANDLE_VALUE\n"));
            xLog(LogHandle,
                 XLL_BLOCK,
                 "test_XDeleteSaveGame(#3) : XFindFirstSaveGame failed (LastError=%d)",
                 GetLastError());
            goto XD3;
        }

        do {
            RetVal = XDeleteSaveGame(SaveRoot, xgData.szSaveGameName);

            if (ERROR_SUCCESS != RetVal) {
                swprintf(wcDbgMsgBuffer,
                         L"Savegame : XDeleteSaveGame failed: %d, dir = %s : name = %S\n",
                         RetVal,
                         xgData.szSaveGameDirectory,
                         xgData.szSaveGameName);
                OutputDebugString(wcDbgMsgBuffer);
                Flag = FALSE;
            }
        } while (XFindNextSaveGame(hSavedGames, &xgData));

        xLog(LogHandle, (Flag) ? XLL_PASS : XLL_FAIL, "test_XDeleteSaveGame(#3)");

        XFindClose(hSavedGames);
    }

    XD3:

    //
    // #4
    // saved games with 0 files
    //

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game4",
                             OPEN_ALWAYS,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    if (ERROR_SUCCESS == RetVal) {

        Flag = TRUE;
        hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);

        if (INVALID_HANDLE_VALUE == hSavedGames) {
            OutputDebugString( TEXT("XFindFirstSaveGame returned INVALID_HANDLE_VALUE\n"));
            xLog(LogHandle,
                 XLL_BLOCK,
                 "test_XDeleteSaveGame(#4) : XFindFirstSaveGame failed (LastError=%d)",
                 GetLastError());
            goto XD4;
        }

        do {
            RetVal = XDeleteSaveGame(SaveRoot, xgData.szSaveGameName);

            if (ERROR_SUCCESS != RetVal) {
                swprintf(wcDbgMsgBuffer,
                         L"Savegame : XDeleteSaveGame failed: %d, dir = %s : name = %S\n",
                         RetVal,
                         xgData.szSaveGameDirectory,
                         xgData.szSaveGameName);
                OutputDebugString(wcDbgMsgBuffer);
                Flag = FALSE;
            }
        } while (XFindNextSaveGame(hSavedGames, &xgData));

        xLog(LogHandle, (Flag) ? XLL_PASS : XLL_FAIL, "test_XDeleteSaveGame(#4)");
        
        XFindClose(hSavedGames);
    }

    XD4:

    //
    // * skip this case (it will be more relevant in the XFindFirstSaveGame() test)
    // saved games with 1 files
    // this was already covered, test_XCreateSaveGame() creates games with 1 file
    //

    //
    // #5
    // saved games with N (1000 just to make this finish sometime in my lifetime....) files
    //

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game5",
                             OPEN_ALWAYS,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    if (ERROR_SUCCESS == RetVal) {
        RetVal = SaveAGame(PathBuffer,
                           "test5",
                           "sav",
                           1000,
                           1000);

        Flag = TRUE;
        hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);

        if (INVALID_HANDLE_VALUE == hSavedGames) {
            OutputDebugString( TEXT("XFindFirstSaveGame returned INVALID_HANDLE_VALUE\n"));
            xLog(LogHandle,
                 XLL_BLOCK,
                 "test_XDeleteSaveGame(#5) : XFindFirstSaveGame failed (LastError=%d)",
                 GetLastError());
            goto XD5;
        }

        do {
            RetVal = XDeleteSaveGame(SaveRoot, xgData.szSaveGameName);

            if (ERROR_SUCCESS != RetVal) {
                swprintf(wcDbgMsgBuffer,
                         L"Savegame : XDeleteSaveGame failed: %d, dir = %s : name = %S\n",
                         RetVal,
                         xgData.szSaveGameDirectory,
                         xgData.szSaveGameName);
                OutputDebugString(wcDbgMsgBuffer);
                Flag = FALSE;
            }
        } while (XFindNextSaveGame(hSavedGames, &xgData));

        xLog(LogHandle, (Flag) ? XLL_PASS : XLL_FAIL, "test_XDeleteSaveGame(#5)");
        XFindClose(hSavedGames);
    }

    XD5:

    xEndVariation( LogHandle );

    //
    // negative test cases
    //

#if DBG_RIP
    _try {
        xSetFunctionName( LogHandle, "XDeleteSaveGame" );
        xStartVariation( LogHandle, "badparam1" );

        //
        // #n1
        // lpSaveGameName = NULL
        //

        PreRIPSaveRegs
        RetVal = XDeleteSaveGame(SaveRoot, NULL);

        // should RIP so don't log
        //xLog(LogHandle,
        //     (ERROR_SUCCESS != RetVal ||
        //      ERROR_INVALID_PARAMETER != GetLastError())  ? XLL_PASS : XLL_FAIL,
        //     "XDeleteSaveGame(SaveRoot, NULL): unexpectedly succeeded: (LastError=%d)",
        //     GetLastError());

        //
        // #n2
        // lpRootPathName = NULL
        //

        PreRIPSaveRegs
        RetVal = XCreateSaveGame(SaveRoot,
                                 L"gamen2",
                                 OPEN_ALWAYS,
                                 0,
                                 PathBuffer,
                                 sizeof(PathBuffer));

        if (ERROR_SUCCESS == RetVal) {

            Flag = TRUE;
            hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);

            if (INVALID_HANDLE_VALUE == hSavedGames) {
                OutputDebugString( TEXT("XFindFirstSaveGame returned INVALID_HANDLE_VALUE\n"));
                xLog(LogHandle,
                     XLL_BLOCK,
                     "test_XDeleteSaveGame(#n2) : XFindFirstSaveGame failed (LastError=%d)",
                     GetLastError());
                goto XDn2;
            }

            PreRIPSaveRegs
            RetVal = XDeleteSaveGame(NULL, L"gamen2");
            // should RIP so don't log
            //xLog(LogHandle,
            //     (ERROR_SUCCESS != RetVal ||
            //      ERROR_INVALID_PARAMETER != GetLastError())  ? XLL_PASS : XLL_FAIL,
            //     "XDeleteSaveGame(SaveRoot, NULL): unexpectedly succeeded: (LastError=%d)",
            //     GetLastError());
            
            XFindClose(hSavedGames);
        }

        XDn2:

        //
        // now really delete it...
        //

        PreRIPSaveRegs
        RetVal = XDeleteSaveGame(SaveRoot, L"gamen2");

        //
        // #n3
        // lpRootPathName = NULL
        // lpSaveGameName = NULL
        //

        PreRIPSaveRegs
        RetVal = XDeleteSaveGame(NULL, NULL);
        // should RIP so don't log
        //xLog(LogHandle,
        //     (ERROR_SUCCESS != RetVal  ||
        //      ERROR_INVALID_PARAMETER != GetLastError()) ? XLL_PASS : XLL_FAIL,
        //     "XDeleteSaveGame(NULL, NULL): unexpectedly succeeded: (LastError=%d)",
        //     GetLastError());

        xEndVariation( LogHandle );
    }
    _except ( Eval_Exception( GetExceptionInformation(), 8)) {
        OutputDebugString(L"_except function body hit - bug?\n");
        // No code; this block never executed.
    }
#endif  //DBG

    return;
}

VOID
WINAPI
test_XFindFirstSaveGame(
    HANDLE LogHandle,
    char uDrive
    )
/*++

Routine Description:

    test XFindFirstSaveGame
    - a disk full of saved games (thousands!) test_XDeleteSaveGame()
    - test with 0 saved games
    - 1 saved game - done in test_XDeleteSaveGame()
    - bad params where they make sense

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs
    char uDrive - drive to use

Return Value:

    None

--*/
{
    XGAME_FIND_DATA xgData;
    HANDLE hSavedGames;
    CHAR SaveRoot[] = {"U:\\"};

    //
    // point to the correct drive
    //

    SaveRoot[0] = uDrive;
    
    //
    // positive test cases
    //

    xSetFunctionName( LogHandle, "XFindFirstSaveGame" );
    xStartVariation( LogHandle, "goodparam1" );

    //
    // test with 0 saved games
    //

    DeleteAllSavedGames(uDrive);

    hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);

    if (INVALID_HANDLE_VALUE == hSavedGames) {
        xLog(LogHandle,
             XLL_PASS,
             "XFindFirstSaveGame(#1) : XFindFirstSaveGame failed correctly");
    } else {
        xLog(LogHandle,
             XLL_FAIL,
             "XFindFirstSaveGame(#1) : XFindFirstSaveGame succeeded unexpectedly (LastError=%d) Path: %s Name %S",
             GetLastError(),
             xgData.szSaveGameDirectory,
             xgData.szSaveGameName);
    }

    xEndVariation( LogHandle );

    //
    // negative test cases
    // need to stop RIPs here
    //

#if DBG_RIP
    //
    // #n1
    // lpRootPathName = NULL
    //

    _try {
        xSetFunctionName( LogHandle, "XFindFirstSaveGame" );
        xStartVariation( LogHandle, "badparam1" );

        PreRIPSaveRegs
        hSavedGames = XFindFirstSaveGame(NULL, &xgData);

        // should RIP so don't log
        //xLog(LogHandle,
        //     INVALID_HANDLE_VALUE != hSavedGames ||
        //     ERROR_INVALID_PARAMETER != GetLastError() ? XLL_FAIL : XLL_PASS,
        //     "XFindFirstSaveGame(#n1) : XFindFirstSaveGame : handle: %d : (LastError=%d)",
        //     hSavedGames,
        //     GetLastError());

        //
        // #n2
        // pFindGameData = NULL
        //

        PreRIPSaveRegs
        hSavedGames = XFindFirstSaveGame(SaveRoot, NULL);

        // should RIP so don't log
        //xLog(LogHandle,
        //     INVALID_HANDLE_VALUE != hSavedGames ||
        //     ERROR_INVALID_PARAMETER != GetLastError() ? XLL_FAIL : XLL_PASS,
        //     "XFindFirstSaveGame(#n2) : XFindFirstSaveGame : handle: %d : (LastError=%d)",
        //     hSavedGames,
        //     GetLastError());

        //
        // #n3
        // pFindGameData = NULL
        // lpRootPathName = NULL
        //

        PreRIPSaveRegs
        hSavedGames = XFindFirstSaveGame(NULL, NULL);

        // should RIP so don't log
        //xLog(LogHandle,
        //     INVALID_HANDLE_VALUE != hSavedGames ||
        //     ERROR_INVALID_PARAMETER != GetLastError() ? XLL_FAIL : XLL_PASS,
        //     "XFindFirstSaveGame(#n3) : XFindFirstSaveGame : handle: %d : (LastError=%d)",
        //     hSavedGames,
        //     GetLastError());

        xEndVariation( LogHandle );
    }
    _except ( Eval_Exception( GetExceptionInformation(), 8)) {
        OutputDebugString(L"_except function body hit - bug?\n");
        // No code; this block never executed.
    }

#endif  // DBG

    return;
}

VOID
WINAPI
test_XFindNextSaveGame(
    HANDLE LogHandle,
    char uDrive
    )
/*++

Routine Description:

    test XFindNextSaveGame
    - test with 0 saved games
    - 1 saved game - done in test_XDeleteSaveGame()
    - a disk full of saved games (thousands!) done in test_XDeleteSaveGame()
    - bad params where they make sense

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs
    char uDrive - drive to use

Return Value:

    None

--*/
{
    XGAME_FIND_DATA xgData;
    BOOL RetVal;

    //
    // positive test cases (none here!)
    //

    //xSetFunctionName( LogHandle, "XFindNextSaveGame" );
    //xStartVariation( LogHandle, "goodparam1" );
    //xEndVariation( LogHandle );

    //
    // negative test cases
    //

#if DBG_RIP
    _try {
        xSetFunctionName( LogHandle, "XFindNextSaveGame" );
        xStartVariation( LogHandle, "badparam1" );

        //
        // test with 0 saved games - all other cases are covered in other tests
        // do it the easy way
        //

        PreRIPSaveRegs
        RetVal = XFindNextSaveGame(INVALID_HANDLE_VALUE, &xgData);
        // should RIP so don't log
        //xLog(LogHandle,
        //     (FALSE == RetVal  &&
        //      ERROR_INVALID_PARAMETER == GetLastError()) ? XLL_PASS : XLL_FAIL,
        //     "XFindNextSaveGame(INVALID_HANDLE_VALUE, &xgData): unexpectedly succeeded: (LastError=%d) (RetVal=%d)",
        //     GetLastError(),
        //     RetVal);

        //
        // hFindGame = NULL
        //

        PreRIPSaveRegs
        RetVal = XFindNextSaveGame(NULL, &xgData);
        // should RIP so don't log
        //xLog(LogHandle,
        //     (FALSE == RetVal  &&
        //      ERROR_INVALID_PARAMETER == GetLastError()) ? XLL_PASS : XLL_FAIL,
        //     "XFindNextSaveGame(NULL, &xgData): unexpectedly succeeded: (LastError=%d) (RetVal=%d)",
        //     GetLastError(),
        //     RetVal);

        //
        // pFindGameData = NULL
        // do not build up a valid hSavedGames, just lie
        //

        PreRIPSaveRegs
        RetVal = XFindNextSaveGame((HANDLE)1, NULL);
        // should RIP so don't log
        //xLog(LogHandle,
        //     (FALSE == RetVal  &&
        //      ERROR_INVALID_PARAMETER == GetLastError()) ? XLL_PASS : XLL_FAIL,
        //     "XFindNextSaveGame(1, NULL): unexpectedly succeeded: (LastError=%d) (RetVal=%d)",
        //     GetLastError(),
        //     RetVal);

        //
        // hFindGame = NULL
        // pFindGameData = NULL
        //

        PreRIPSaveRegs
        RetVal = XFindNextSaveGame(NULL, NULL);
        // should RIP so don't log
        //xLog(LogHandle,
        //     (FALSE == RetVal  ||
        //      ERROR_INVALID_PARAMETER == GetLastError()) ? XLL_PASS : XLL_FAIL,
        //     "XFindNextSaveGame(NULL, NULL): unexpectedly succeeded: (LastError=%d) (RetVal=%d)",
        //     GetLastError(),
        //     RetVal);
        xEndVariation( LogHandle );
    }

    _except ( Eval_Exception( GetExceptionInformation(), 8)) {
        OutputDebugString(L"_except function body hit - bug?\n");
        // No code; this block never executed.
    }

#endif  // DBG
    return;
}

VOID
WINAPI
test_XFindClose(
    HANDLE LogHandle,
    char uDrive
    )
/*++

Routine Description:

    test XFindClose
    - good handle
    - bad handle
    - double close

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs
    char uDrive - drive to use

Return Value:

    None

Notes:

    Should be called with a clean disk

--*/
{
    XGAME_FIND_DATA xgData;
    HANDLE hSavedGames;
    CHAR PathBuffer[_MAX_DIR + 2] = {0};
    CHAR SaveRoot[] = {"U:\\"};
    DWORD RetVal = 0;
    BOOL Flag;

    //
    // point to the correct drive
    //

    SaveRoot[0] = uDrive;
    
    //
    // positive test cases
    //

    xSetFunctionName( LogHandle, "XFindClose" );
    xStartVariation( LogHandle, "goodparam1" );

    //
    // #1
    // good handle
    //

    RetVal = XCreateSaveGame(SaveRoot,
                             L"game1",
                             OPEN_ALWAYS,
                             0,
                             PathBuffer,
                             sizeof(PathBuffer));

    if (ERROR_SUCCESS == RetVal) {
        RetVal = SaveAGame(PathBuffer,
                           "test1",
                           "sav",
                           10000,
                           1);

        hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);

        if (INVALID_HANDLE_VALUE == hSavedGames) {
            OutputDebugString( TEXT("XFindFirstSaveGame returned INVALID_HANDLE_VALUE\n"));
            xLog(LogHandle,
                 XLL_BLOCK,
                 "test_XFindClose(#1) : XFindFirstSaveGame failed (LastError=%d)",
                 GetLastError());
            goto XD1;
        }
    } else {
        xLog(LogHandle,
             XLL_BLOCK,
             "test_XFindClose(#1) : XCreateSaveGame failed (Error=%d)",
             RetVal);
        goto XD1;
    }

    RetVal = XFindClose(hSavedGames);
    xLog(LogHandle, RetVal ? XLL_PASS : XLL_FAIL, "test_XFindClose(#1)");

    //
    // #2
    // double close
    // expecting 0 for return value
    // need to handle exception this will likely cause
    // NOTE: this can cause random crahshe, don't do it
    //

    //Flag = TRUE;
    //__try {
    //    RetVal = XFindClose(hSavedGames);
    //}
    //__except(EXCEPTION_EXECUTE_HANDLER) {
    //    xLog(LogHandle, XLL_PASS, "test_XFindClose(#2) - exception hit");
    //    Flag = FALSE;
    //}
    //
    //if (Flag == FALSE) {
    //    xLog(LogHandle, RetVal ? XLL_FAIL : XLL_PASS, "test_XFindClose(#2) - exception not hit");
    //}

    XD1:

    xEndVariation( LogHandle );

    //
    // negative test cases
    //

#if DBG_RIP
    _try {
        xSetFunctionName( LogHandle, "XFindClose" );
        xStartVariation( LogHandle, "badparam1" );

        //
        // #n1
        // use INVALID_HANDLE_VALUE
        //

        PreRIPSaveRegs
        RetVal = XFindClose(INVALID_HANDLE_VALUE);
        // should RIP so don't log
        //xLog(LogHandle,
        //     (RetVal || (ERROR_INVALID_HANDLE != GetLastError())) ? XLL_FAIL : XLL_PASS,
        //     "test_XFindClose(#n1)");

        //
        // #n2
        // use 0
        //

        PreRIPSaveRegs
        RetVal = XFindClose(NULL);
        // should RIP so don't log
        //xLog(LogHandle,
        //    (RetVal || (ERROR_INVALID_HANDLE != GetLastError())) ? XLL_FAIL : XLL_PASS,
        //    "test_XFindClose(#n2)");

        xEndVariation( LogHandle );
    }
   _except ( Eval_Exception( GetExceptionInformation(), 4)) {
        OutputDebugString(L"_except function body hit - bug?\n");
        // No code; this block never executed.
    }

#endif  // DBG

    return;
}

VOID
WINAPI
SaveGameStartTest(
    HANDLE LogHandle
    )
/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/
{
    LONG ID;
    DWORD x;
    char *tmpPtr;
    unsigned y = 0;

    OutputDebugString( TEXT("savegame: StartTest is called\n") );

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample]. Everytime
    // the harness calls StartTest, ThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    //

    ID = InterlockedIncrement( &SaveGameNameSpace::SaveGameThreadID );

    //
    // get parameters
    // MaxFiles - maximum number of files to create
    // -1 means creat until failure - this takes too long for BVT's
    //

    MaxFiles = GetPrivateProfileIntA("savegame",
                                     "Files",
                                     1000,
                                     "testini.ini");

    xSetComponent( LogHandle, "xapi", "savegame" );

    //
    // seed random number generator
    // may want to conside saving the seed sometime
    //

    xtsrand( (unsigned)GetTickCount() );

    //
    // create our test data buffer
    //

    FileDataBuffer = HeapAlloc(SaveGameNameSpace::HeapHandle, 0, FILE_DATA_SIZE);
    if (NULL == FileDataBuffer) {
        OutputDebugString(L"Out of memory!");
        return;
    }
    tmpPtr = (char *)FileDataBuffer;
    for (x = 1; x < FILE_DATA_SIZE; x++) {
        *tmpPtr = (char)(x % 16);
        ++tmpPtr;
    }

    //
    // mount MUs
    //

    MountMUs();
    
    //
    // run the tests
    //

    //
    // this test is independant of the drives
    //

    test_XSignature(LogHandle);

    //
    // these tests require the drives
    //

    while ( UDataDrives[y] ) {

        //
        // attempt to get everything to a known state
        //

        DeleteAllSavedGames(UDataDrives[y]);

        //
        // call the tests
        // these 3 need to be called in order, for each saved game device
        //

        test_XFindClose(LogHandle, UDataDrives[y]);         // must be 0th
    
        test_XCreateSaveGame(LogHandle, UDataDrives[y]);    // must be first
        test_XDeleteSaveGame(LogHandle, UDataDrives[y]);    // must be second
    
        test_XFindFirstSaveGame(LogHandle, UDataDrives[y]);
        test_XFindNextSaveGame(LogHandle, UDataDrives[y]);
    
        ++y;
    }
    
    //
    // clean up the mess on the last drive
    //

    if (y) {
        DeleteAllSavedGames(UDataDrives[--y]);
    }
    
    //
    // done testing, cleanup
    //
    
    HeapFree(SaveGameNameSpace::HeapHandle, 0, FileDataBuffer);

    return;
}

VOID
WINAPI
SaveGameEndTest(
    VOID
    )
/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/
{
    SaveGameNameSpace::SaveGameThreadID = 0;
    OutputDebugString( TEXT("savegame: EndTest is called\n") );
}

BOOL
WINAPI
SaveGameDllMain(
    HINSTANCE   hInstance,
    DWORD       fdwReason,
    LPVOID      lpReserved
    )
{
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( lpReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {

        SaveGameNameSpace::HeapHandle = HeapCreate( 0, 0, 0 );

        if ( !SaveGameNameSpace::HeapHandle ) {
            OutputDebugString( TEXT("savegame: Unable to create heap\n") );
            return FALSE;
        }

    } else if ( fdwReason == DLL_PROCESS_DETACH ) {

        if ( SaveGameNameSpace::HeapHandle ) {
            HeapDestroy( SaveGameNameSpace::HeapHandle );
        }
    }

    return TRUE;
}

BOOL
WINAPI
DeleteAllSavedGames(
    char uDrive
    )
/*++

Routine Description:

    quickly blow away all saved games

Arguments:

    char uDrive

Return Value:

    TRUE if any saved games were found and deleted
    FALSE otherwise

Notes:

    this may change as the APIs change
    this may make a good SDK sample

--*/
{
    BOOL bGamesDeleted = FALSE;
    XGAME_FIND_DATA xgData;
    HANDLE hSavedGames;
    // BUGBUG - hardcoded to drive U for now
    CHAR SaveRoot[] = {"U:\\"};

    //
    // point to the correct drive
    //

    SaveRoot[0] = uDrive;
    
    //
    // enumerate and delete games
    //

    hSavedGames = XFindFirstSaveGame(SaveRoot, &xgData);
    if(INVALID_HANDLE_VALUE == hSavedGames){
        OutputDebugString( TEXT("XFindFirstSaveGame returned INVALID_HANDLE_VALUE\n"));
        return(bGamesDeleted);
    }

    bGamesDeleted = TRUE;

    do {
         XDeleteSaveGame(SaveRoot, xgData.szSaveGameName);
    } while (XFindNextSaveGame(hSavedGames, &xgData));

    XFindClose(hSavedGames);

    return(bGamesDeleted);
}

BOOL
WINAPI
SaveAGame(
    PCHAR SavePath,
    PCHAR BaseFileName,
    PCHAR FileNameExt,
    DWORD FileSize,
    DWORD FileCount
    )
/*++

Routine Description:

    save a game using the specified path

Arguments:

    PWCHAR SavePath - path to save to
    PWCHAR BaseFileName - base string to form filename with
    PWCHAR FileNameExt - file name extension
    DWORD FileSize - how many bytes long the file needs to be
    DWORD FileCount - number of files

Return Value:

    TRUE file created successfully
    FALSE otherwise

Notes:

    we'll limit the number of files created to 0xFFFF

--*/
{
    HANDLE hFile;
    CHAR wcGameNamePath[MAX_PATH + 2] = {0};
    DWORD dwBytesWritten;
    DWORD dwBytesToWrite;
    BOOL bRetVal = TRUE;
    DWORD count;
    //char SavePathA[MAX_PATH] = {0};
    //char BaseFileNameA[64] = {0};
    //char FileNameExtA[4] = {0};

    //
    // check params
    // we allow 0-length files
    //

    if (NULL == SavePath ||
        NULL == BaseFileName ||
        0 == FileCount) {
        return(FALSE);
    }

    //
    // convert things to ANSI from Unicode here
    // otherwise we have to do it everywhere and it will get ugly, do later if time
    //
    //
    //WideCharToMultiByte(CP_ACP,
    //                    0,
    //                    SavePath,
    //                    wcslen(SavePath),
    //                    SavePathA,
    //                    MAX_PATH,
    //                    NULL,
    //                    NULL);
    //
    //WideCharToMultiByte(CP_ACP,
    //                    0,
    //                    BaseFileName,
    //                    wcslen(BaseFileName),
    //                    BaseFileNameA,
    //                    64,
    //                    NULL,
    //                    NULL);
    //
    //WideCharToMultiByte(CP_ACP,
    //                    0,
    //                    FileNameExt,
    //                    wcslen(FileNameExt),
    //                    FileNameExtA,
    //                    4,
    //                    NULL,
    //                    NULL);

    for (count = 0; count < FileCount; count++) {

        //
        // check lengths, make fully qualified path name
        // the + 5 is for the XXXX. added to the base filename
        //

        //if ((strlen(SavePathA) +
        //     strlen(BaseFileNameA) +
        //     strlen(FileNameExtA) + 5) > MAX_PATH) {
        //    OutputDebugString( TEXT("path name too long!\n"));
        //    return(FALSE);
        //}

        if ((strlen(SavePath) +
             strlen(BaseFileName) +
             strlen(FileNameExt) + 5) > MAX_PATH) {
            OutputDebugString( TEXT("path name too long!\n"));
            return(FALSE);
        }

        sprintf(wcGameNamePath,
                "%s%s%X.%s",
                SavePath,
                BaseFileName,
                count,
                FileNameExt);

        hFile = CreateFile(wcGameNamePath,
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

        if (INVALID_HANDLE_VALUE == hFile) {
            swprintf(wcDbgMsgBuffer,L"Savegame : CreateFile failed: path = %S: error = %d\n",
                     wcGameNamePath,
                     GetLastError());
            OutputDebugString(wcDbgMsgBuffer);
            return(FALSE);
        }

        //
        // write some easily understood pattern of data to the file
        //

        dwBytesToWrite = FileSize;
        while (dwBytesToWrite) {

            if (0 == WriteFile(hFile,
                               FileDataBuffer,
                               min(dwBytesToWrite, FILE_DATA_SIZE),
                               &dwBytesWritten,
                               NULL)) {
                swprintf(wcDbgMsgBuffer,
                         L"Savegame : WriteFile failed: path = %S: error = %d\n",
                         wcGameNamePath,
                         GetLastError());
                //OutputDebugString(wcDbgMsgBuffer);
                bRetVal = FALSE;
                CloseHandle(hFile);
                break;
            }

            if ( min(dwBytesToWrite, FILE_DATA_SIZE) != dwBytesWritten ) {

                swprintf(wcDbgMsgBuffer,
                         L"Savegame : WriteFile write length mismatch: path = %S: to write = %d : written = %d\n",
                         wcGameNamePath,
                         min(dwBytesToWrite, FILE_DATA_SIZE),
                         dwBytesWritten);
                OutputDebugString(wcDbgMsgBuffer);

                //
                // I don't expect this to happen, but I am paranoid...
                //

                DebugBreak();
                CloseHandle(hFile);
                bRetVal = FALSE;
                break;
            }

            if (dwBytesToWrite >= dwBytesWritten) {
                dwBytesToWrite -= dwBytesWritten;
            } else {
                dwBytesToWrite = 0;
            }
        }
        CloseHandle(hFile);
    }
    return(bRetVal);
}

PWCHAR
WINAPI
AppendRandUnicodeChar(
    PWCHAR pwStr
    )
/*++

Routine Description:

    Append a random unicode char, make sure that 0x0000 0x0000 is not appended
    append junk to end of pwTestBuffer
    junk == a random number between 0x0000 and 0xffff
    since rand() is 0-0x7fff we'll just use the last byte

Arguments:

    PWCHAR pwStr - string to append to

Return Value:

    pointer to next position

Notes:

    don't accidentally null terminate the string

--*/
{
    //
    // give it a random valid unicode value
    //

    *pwStr = xtrand() & 0x7fff;

    //
    // make sure it isn't a terminator
    //

    while (0 == *pwStr)
    {
        *pwStr = xtrand() & 0x7fff;
    }

    return(++pwStr);
}

//
// stolen from CRT's to make my life easier
// I made it un-thread safe
//

/***
*void srand(seed) - seed the random number generator
*
*Purpose:
*   Seeds the random number generator with the int given.  Adapted from the
*   BASIC random number generator.
*
*Entry:
*   unsigned seed - seed to seed rand # generator with
*
*Exit:
*   None.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl xtsrand (
    unsigned int seed
    )
{
    holdrand = (long)seed;
}


/***
*int rand() - returns a random number
*
*Purpose:
*   returns a pseudo-random number 0 through 32767.
*
*Entry:
*   None.
*
*Exit:
*   Returns a pseudo-random number 0 through 32767.
*
*Exceptions:
*
*******************************************************************************/

int __cdecl xtrand (
    void
    )
{
    return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

int
Eval_Exception (
    PEXCEPTION_POINTERS pExceptionPointers,
    DWORD ArgBytes
    )
{
    BYTE *JumpOffset;
    BYTE *pJumpOffset = (BYTE *)&JumpOffset;


    //_asm int 3

    if ( pExceptionPointers->ExceptionRecord->ExceptionCode != STATUS_BREAKPOINT) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    OutputDebugString(L"Eval_Exception function hit - STATUS_BREAKPOINT\n");

    //
    // there is some indirection that has to happen here, because if we modify
    // the int 3, we will mangle the code for the next call, which breaks everything
    // so what we do is:
    //
    // 1.) save the contents of the code we will overwrite
    // 2.) make that code jump (not call) to a dummy code buffer
    // 3.) the dummy code buffer re-writes the int 3 and following bytes back to the function
    // 4.) this dummy code gets called when the 'bad' code gets continued
    // 5.) the dummy code than executes the correct pop+return instruction
    //

    //
    // save the address of the faulting instruction
    //

    Address = (BYTE *)pExceptionPointers->ExceptionRecord->ExceptionAddress;

    //
    // save the code that will get over written
    //

    Aspace[0] = Address[0];
    Aspace[1] = Address[1];
    Aspace[2] = Address[2];
    Aspace[3] = Address[3];
    Aspace[4] = Address[4];

    //
    // modify the code so that the function returns
    // after the bp instead of continuing with the bad parameters
    // synthesize :
    // 0xc9     leave
    // c2XXXX   ret ArgBytes
    //

    //
    // may need a few variations of this depending on the types of return codes we see
    //

    Bspace[0] = 0xc9;   // leave
    Bspace[1] = 0xc2;   // ret
    Bspace[2] = (BYTE)(ArgBytes & 0xFF);     // low byte of number
    Bspace[3] = (BYTE)((ArgBytes >> 16) & 0xFF);     // high byte of number

    //
    // write jump to DummyCode to the address that now contains the BP
    //

    //
    // 1 is size of opcode for int 3, 4 to skip entry for DummyCode
    //

    JumpOffset = (BYTE *)((BYTE *)DummyCode - Address - 2);

    Address[0] = 0xe9;  //jmp rel32
    Address[1] = pJumpOffset[0];
    Address[2] = pJumpOffset[1];
    Address[3] = pJumpOffset[2];
    Address[4] = pJumpOffset[3];

    //
    // the logging for the API may want to do some logging in the handler
    // before returning to the now-fixed code
    //

    return EXCEPTION_CONTINUE_EXECUTION ;
}

void
DummyCode(
    void
    )
{

    //
    // fix the calling code (saved in Aspace) at address (saved in Address)
    //

    //
    // the following code will be jumped to
    // examine any changes in a debugger to make sure there are no
    // pushes / pops that would mess up the synthesized return
    //

    Address[0] = Aspace[0];
    Address[1] = Aspace[1];
    Address[2] = Aspace[2];
    Address[3] = Aspace[3];
    Address[4] = Aspace[4];

    //
    // restore saved off registers
    //

    RIPRestoreSaveRegs

    //
    // execute the correct return (in Bspace)
    //

    _asm jmp jump_target;

}

void
MountMUs(
    void    
    )
/*++

Routine Description:

    log formatted text

Arguments:

    None

Return Value:

    None
    
Notes

    find and mount all MUs
    I believe the filesystem will now see them when they are present
    We will ignore unmounting them between titles 
    (unless this proves to be a bad thing to do)
    
--*/ 
{
    DWORD insertions;
    unsigned port, slot, i, x, UDataDrivesIndex = 0, DrivesIndex = 0;
    char Drive[10] = {10};
    char DeviceName[5];
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    BOOL bRetVal;

    for ( x = 0; x < 26; x++ ) {
        sprintf(DeviceName, "%c:\\", DeviceLetters[x]);
        bRetVal = GetDiskFreeSpaceEx(DeviceName,
                                     &FreeBytesAvailable,
                                     &TotalNumberOfBytes,
                                     &TotalNumberOfFreeBytes);

        if ( bRetVal ) {

            Drives[DrivesIndex] = DeviceLetters[x];

            //
            // take care of this in 1 place, rather than splitting it 
            // between here and some of the other functions that deal with drives
            //

            if ( (DeviceLetters[x] == 'U') ||
                 (DeviceLetters[x] == 'F') ||
                 (DeviceLetters[x] == 'G') ||
                 (DeviceLetters[x] == 'H') ||
                 (DeviceLetters[x] == 'I') ||
                 (DeviceLetters[x] == 'J') ||
                 (DeviceLetters[x] == 'K') ||
                 (DeviceLetters[x] == 'L') ||
                 (DeviceLetters[x] == 'M') ) {
                UDataDrives[UDataDrivesIndex++] = DeviceLetters[x];
            }
        }
    }
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( savegame )
#pragma data_seg()

BEGIN_EXPORT_TABLE( savegame )
    EXPORT_TABLE_ENTRY( "StartTest", SaveGameStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", SaveGameEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", SaveGameDllMain )
END_EXPORT_TABLE( savegame )

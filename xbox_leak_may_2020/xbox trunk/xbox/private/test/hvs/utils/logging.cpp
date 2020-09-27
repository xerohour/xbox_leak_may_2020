/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    logging.cpp

Abstract:

    HVS Test Helper functions

Notes:

*****************************************************************************/

#include <stdio.h>
#include <xtl.h>
#include <crtdbg.h>

#include "hvsUtils.h"

// private kernel API
extern "C" void FlushDiskCache(void);

static void FindSaveGame(void);


/*****************************************************************************

Routine Description:

    SetSaveName

    Sets the filename and Save Game Name for the log file. This function 
    MUST be called prior to calling LogPrint or LogFlush.

Arguments:

    char *format,
    ...

Return Value:

    none

*****************************************************************************/
static char *logFilename = NULL;
static XGAME_FIND_DATA saveGameData;
static WCHAR saveGameName[MAX_GAMENAME];
static char saveFileName[128];
static bool saveNameSet = false;
void SetSaveName(char *saveName, char *fileName)
    {
    saveNameSet = true;

    strcpy((char*)saveGameName, saveName);
    Ansi2UnicodeHack((char*)saveGameName);

    strcpy(saveFileName, fileName);
    }


/*****************************************************************************

Routine Description:

    LogPrint

    Writes data to the debugger and a log file save game using printf style
    symantics.

Arguments:

    char *format,
    ...

Return Value:

    none

*****************************************************************************/
void LogPrint(char* format, ...)
    {
    if(!format) return;

    if(saveNameSet == false)
        {
        _ASSERT(0 && "Must call SetSaveName() prior to calling LogPrint()");
        }

    char *szBuffer = new char[1024*1024];
    DWORD err = GetLastError(); // we dont want LogPrint to tromp on the last error

    va_list args;
    va_start(args, format);

    vsprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    if(!logFilename)
        {
        FindSaveGame();
        }

    FILE *f = fopen(logFilename, "a+");
    if(f)
        {
        fprintf(f, "%s", szBuffer);
        fclose(f);
        FlushDiskCache();
        }

    va_end(args);

    delete[] szBuffer;
    SetLastError(err);
    }

/*****************************************************************************

Routine Description:

    LogFlush

    Deletes the log file

Arguments:

    none

Return Value:

    none

*****************************************************************************/
void LogFlush(void)
    {
    if(saveNameSet == false)
        {
        _ASSERT(0 && "Must call SetSaveName() prior to calling LogFlush()");
        }

    if(!logFilename)
        {
        FindSaveGame();
        }

    // remove old copy of file
    DeleteFile(logFilename);
    FlushDiskCache();
    }


/*****************************************************************************

Routine Description:

    FindSaveGame

    Private API

*****************************************************************************/
static void FindSaveGame(void)
    {
    HANDLE h = XFindFirstSaveGame("U:\\", &saveGameData);
    while(h != INVALID_HANDLE_VALUE)
        {
        if(wcscmp(saveGameData.szSaveGameName, saveGameName) == 0)
            {
            logFilename = saveGameData.szSaveGameDirectory;
            strcat(logFilename, saveFileName);
            XFindClose(h);
            return;
            }

        if(!XFindNextSaveGame(h, &saveGameData)) break;
        }

    if(h != INVALID_HANDLE_VALUE) XFindClose(h);

    // create a new save game
    logFilename = saveGameData.szSaveGameDirectory;
    XCreateSaveGame("U:\\", saveGameName, OPEN_ALWAYS, 0, logFilename, MAX_PATH);
    strcat(logFilename, saveFileName);
    }

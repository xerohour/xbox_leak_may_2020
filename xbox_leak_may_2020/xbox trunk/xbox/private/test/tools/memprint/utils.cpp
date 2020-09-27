/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Helper functions

Notes:

*****************************************************************************/

#include "memprint.h"


/*****************************************************************************

Routine Description:

    Ansi2UnicodeHack

    In-place Pseudo Ansi to Unicode (char to wide char) conversion.

Arguments:

    IN char* str - char string to convert to wide char string

Return Value:

    WCHAR* - pointer to Unicode string

Note:
    
    Because a bunch of Unicode functions expect a WCHAR string to be on an
    even boundry, the returned string may be moved 1 character over.

*****************************************************************************/
WCHAR* Ansi2UnicodeHack(char *str)
    {
    if(!str) return NULL;

    int align = 0;
    int len = strlen(str)+1;

    // put string on an even boundry because some freak put a bunch of ASSERTs
    // that check for even boundries in Unicode functions like 
    // RtlEqualUnicodeString()
    if(((DWORD)str & 1) != 0)
        {
        align = 1;
        }

    for(; len>=0; len--)
        {
        str[len*2+align] = str[len];
        str[len*2+align+1] = '\0';
        }

    str += align;

    return (WCHAR*)str;
    }



static char *logFilename = NULL;
static XGAME_FIND_DATA saveGameData;
void DebugPrint(char* format, ...)
    {
    char szBuffer[1024];
    DWORD err = GetLastError(); // we dont want DebugPrint to tromp on the last error

    va_list args;
    va_start(args, format);

    vsprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    if(!logFilename)
        {
        HANDLE h = XFindFirstSaveGame("U:\\", &saveGameData);
        if(h == INVALID_HANDLE_VALUE)
            {
            logFilename = saveGameData.szSaveGameDirectory;
            XCreateSaveGame("U:\\", L"Test Results Log", OPEN_ALWAYS, 0, logFilename, MAX_PATH);
            strcat(logFilename, "\\memprint.txt");
            }
        else
            {
            logFilename = saveGameData.szSaveGameDirectory;
            strcat(logFilename, "\\memprint.txt");
            if(!warmBoot) DeleteFile(logFilename); // remove old copy of file
            XFindClose(h);
            }
        }

    FILE *f = fopen(logFilename, "a+");
    if(f)
        {
        fprintf(f, "%s", szBuffer);
        fclose(f);
        }

    va_end(args);

    SetLastError(err);
    }


void HexDump(const unsigned char * buffer, DWORD length)
    {
    DWORD i, count, index;
    char digits[]="0123456789ABCDEF";
    char line[100];
    unsigned cbLine;

    for(index = 0; length; length -= count, buffer += count, index += count)
        {
        count = (length > 16) ? 16:length;

        cbLine = sprintf(line, "%08X  ", index);

        for(i=0;i<count;i++)
            {
            line[cbLine++] = digits[buffer[i] >> 4];
            line[cbLine++] = digits[buffer[i] & 0x0f];
            if(i == 7)
                {
                line[cbLine++] = ' ';
                line[cbLine++] = '|';
                line[cbLine++] = ' ';
                }
            else
                {
                line[cbLine++] = ' ';
                }
            }
        for(; i < 16; i++)
            {
            line[cbLine++] = ' ';
            line[cbLine++] = ' ';
            line[cbLine++] = ' ';
            }

        line[cbLine++] = ' ';

        for(i = 0; i < count; i++)
            {
            if(buffer[i] < 32 || buffer[i] > 126)
                {
                line[cbLine++] = '.';
                }
            else
                {
                line[cbLine++] = buffer[i];
                }
            }

        line[cbLine++] = 0;
        DebugPrint("%s\n", line);
        }
    }




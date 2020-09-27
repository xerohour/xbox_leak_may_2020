/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    hvsUtils.cpp

Abstract:

    HVS Test Helper functions

Notes:

*****************************************************************************/

#include <stdio.h>
#include <xtl.h>
#include <crtdbg.h>
#include <malloc.h>

#include "hvsUtils.h"



/*****************************************************************************

Routine Description:

    DebugPrint

    Writes data to the debugger.

Arguments:

    char *format,
    ...

Return Value:

    none

*****************************************************************************/
void DebugPrint(char* format, ...)
    {
    if(!format) return;

    char *szBuffer;
    try
        {
        // alloca can throw if no mem on stack
        szBuffer = (char*)_alloca(4*1024*sizeof(char));

        va_list args;
        va_start(args, format);

        vsprintf(szBuffer, format, args);
        OutputDebugStringA(szBuffer);

        va_end(args);
        }
    catch(...)
        {
        OutputDebugStringA(format); // just write out as much as we can
        }
    }

/*****************************************************************************

Routine Description:

    DebugPrint

    Writes data to the debugger.

Arguments:

    WCHAR *format,
    ...

Return Value:

    none

*****************************************************************************/
void DebugPrint(WCHAR* format, ...)
    {
    if(!format) return;

    WCHAR *wszBuffer;
    try
        {
        // alloca can throw if no mem on stack
        wszBuffer = (WCHAR*)_alloca(4*1024*sizeof(WCHAR));

        va_list args;
        va_start(args, format);

        vswprintf(wszBuffer, format, args);
        OutputDebugStringW(wszBuffer);

        va_end(args);
        }
    catch(...)
        {
        OutputDebugStringW(format); // just write out as much as we can
        }
    }


/*****************************************************************************

Routine Description:

    HexDump

    Writes binary data as a hex dump using LogPrint

Arguments:

    unsigned char * buffer
    DWORD length

Return Value:

    none

*****************************************************************************/
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
        LogPrint("%s\n", line);
        }
    }

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


/*****************************************************************************

Routine Description:

    GetNumber

    Parses a number from a string that is either base 10 or 16 (must be 
    prefixed by "0x")

Arguments:

    IN char* string - string representation of the number

Return Value:

    long

*****************************************************************************/
long GetNumber(char *string)
    {
    char *tmp;
    if((tmp = strchr(string, 'x')) != NULL)
        {
        long a=0;
        if(sscanf(tmp+1, "%X", &a) != 1) return 0;
        return a;
        }

    return atol(string);
    }

char* RightTrim(char *string)
    {
    char *white = " \n\r\t";
    unsigned len = strlen(string);
    while(--len)
        {
        if(strchr(white, string[len]) != NULL)
            string[len] = '\0';
        else break;
        }

    return string;
    }

WCHAR* RightTrim(WCHAR *string)
    {
    WCHAR *white = L" \n\r\t";
    unsigned len = wcslen(string);
    while(--len)
        {
        if(wcschr(white, string[len]) != NULL)
            string[len] = L'\0';
        else break;
        }

    return string;
    }

/*****************************************************************************

Routine Description:

    ParseConfigSettings

    Parses config settings from a buffer (new line delimited) and adds them
    to a CoreConfigSettings struct.

Arguments:

    char *config - config settings to parse
    CoreConfigSettings *output - where to stick the settings
    USERSETTING_FUNC LocalTestConfigs - user supplied function for parsing 
        custom settings
    void *userParam - user supplied pointer thats passed to LocalTestConfigs

Return Value:

    none

*****************************************************************************/
void ParseConfigSettings(char *config, CoreConfigSettings *output, USERSETTING_FUNC LocalTestConfigs, void *userParam)
    {
    char value[256];
    char variable[64];
    char *end;

    do
        {
        end = strchr(config, '\n');
        if(end) *end = '\0';

        char *c = strchr(config, ';');
        if(c) *c = '\0';
        c = strchr(config, '[');
        if(c) *c = '\0';
        RightTrim(config);

        if(sscanf(config, "%63[^=]= %255[^\0]", variable, value) == 2)
            {
            _strlwr(variable);

            // core settings
            if(strstr(variable, "resultsserver") != NULL) strcpy(output->resultsServer, value);
            else if(strstr(variable, "savename") != NULL) strcpy(output->saveName, value);
            else if(strstr(variable, "stopafter") != NULL)
                {
                output->stopAfter.duration = GetNumber(value);
                if(strstr(value, "min")) output->stopAfter.type = STOPAFTER_MINUTES;
                else if(strstr(value, "hour")) output->stopAfter.type = STOPAFTER_HOURS;
                else if(strstr(value, "iter")) output->stopAfter.type = STOPAFTER_ITERATIONS;
                }

            // test settings
            else LocalTestConfigs(variable, value, userParam);
            }

        if(end) config = end+1;
        } while(end);
    }

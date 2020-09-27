/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    dos.cpp

Abstract:

    MS-DOS style commands. Any non DOS commands are in cmd.cpp

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-31-2000  Created

*/
#include "stdafx.h"
#include "launcherx.h"

namespace Launcher {

DWORD ATTRIB(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "ATTRIB");
        return ERR_SYNTAX;
        }

    WIN32_FIND_DATA findData;
    HANDLE hFind;

    hFind = FindFirstFile(nextToken, &findData);

    if(hFind == INVALID_HANDLE_VALUE)
        {
        SockPrint(sock, "No files found (ec: %lu)\r\n", GetLastError());
        return 1;
        }

    do
        {
        if(strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;

        SockPrint(sock, "%c%c%c%c%c %s\r\n", 
            findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY?'D':' ',
            findData.dwFileAttributes&FILE_ATTRIBUTE_READONLY?'R':' ',
            findData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN?'H':' ',
            findData.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM?'S':' ',
            findData.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE?'A':' ',
            findData.cFileName);
        } while(FindNextFile(hFind, &findData) == TRUE);

    FindClose(hFind);

    return ERR_OK;
    }


DWORD CALL(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "CALL");
        return ERR_SYNTAX;
        }

    size_t tokenLen;
    char *token;
    char *subToken;
    char *buffer = new char[1024];
    if(!buffer) return ERR_NOMEM;

    FILE *f = fopen(nextToken, "rb");
    if(!f)
        {
        delete[] buffer;
        if(strstr(nextToken, "autoexec.bat") == NULL) // NOTE the lowercase!
            {
            // dont display the error message if looking for the main autoexec
            // files (if user calls it they will still get the error message)
            SockPrint(sock, "Could not open file\r\n");
            }
        return 1;
        }
    while(fgets(buffer, 1024, f))
        {
        token = strstr(buffer, "\r\n");
        if(token) *token = '\0';
        _strupr(buffer);
        GetToken(buffer, &token, &tokenLen, &subToken);
        token[tokenLen] = '\0';
        ExecuteCommand(sock, token, subToken);
        }
    fclose(f);
    delete[] buffer;
    return ERR_OK;
    }


DWORD COPY(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "COPY");
        return ERR_SYNTAX;
        }

    WIN32_FIND_DATA findData;
    HANDLE hFind;
    unsigned numFiles=0;
    char *file1, *file2;

    char* newFile = strchr(nextToken, ' ');
    if(!newFile)
        {
        HELP(sock, "COPY");
        return ERR_SYNTAX;
        }
    *newFile = '\0';
    newFile += 1;

    char *path1 = new char[1024];
    char *path2 = new char[1024];

    if(!path1 || !path2)
        {
        delete[] path1;
        delete[] path2;
        return ERR_NOMEM;
        }

    strcpy(path1, nextToken);
    strcpy(path2, newFile);

    file1 = xPathFindFileName(path1);
    file2 = xPathFindFileName(path2);
    if(!file1 || !file2)
        {
        delete[] path1;
        delete[] path2;
        SockPrint(sock, "No files found.\r\n");
        return 1;
        }
    *file1 = '\0';
    *file2 = '\0';

    hFind = FindFirstFile(nextToken, &findData);
    if(hFind == INVALID_HANDLE_VALUE)
        {
        delete[] path1;
        delete[] path2;
        SockPrint(sock, "No files found (ec: %lu)\r\n", GetLastError());
        return 1;
        }

    do
        {
        if(strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;

        strcpy(file1, findData.cFileName);
        strcpy(file2, findData.cFileName);

        if(CopyFile(path1, path2, false) != TRUE)
            {
            SockPrint(sock, "        Unable to copy the file: %s (ec: %lu)\r\n", path1, GetLastError());
            }
        else ++numFiles;
        } while(FindNextFile(hFind, &findData) == TRUE);

    FindClose(hFind);

    SockPrint(sock, "        %6u file(s) copied\r\n", numFiles);

    delete[] path1;
    delete[] path2;

    return ERR_OK;
    }


DWORD DEL(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "DEL");
        return ERR_SYNTAX;
        }

    WIN32_FIND_DATA findData;
    HANDLE hFind;
    unsigned numFiles=0;
    char *buffer = new char[1024];

    if(!buffer)
        {
        return ERR_NOMEM;
        }

    hFind = FindFirstFile(nextToken, &findData);
    char *name = xPathFindFileName(nextToken);
    if(!name)
        {
        delete[] buffer;
        SockPrint(sock, "No files found.\r\n");
        return 1;
        }
    *name = '\0';

    if(hFind == INVALID_HANDLE_VALUE)
        {
        delete[] buffer;
        SockPrint(sock, "No files found (ec: %lu)\r\n", GetLastError());
        return 1;
        }

    do
        {
        if(strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
        sprintf(buffer, "%s%s", nextToken, findData.cFileName);

        // clear any readonly settings (otherwise delete will fail)
        DWORD sattrib = GetFileAttributes(buffer);
        SETFLAG(sattrib, FILE_ATTRIBUTE_NORMAL);
        CLEARFLAG(sattrib, FILE_ATTRIBUTE_READONLY);
        SetFileAttributes(buffer, sattrib);

        if(DeleteFile(buffer) != TRUE)
            {
            SockPrint(sock, "        Unable to delete the file: %s (ec: %lu)\r\n", buffer, GetLastError());
            }
        else ++numFiles;
        } while(FindNextFile(hFind, &findData) == TRUE);
    FindClose(hFind);

    delete[] buffer;
    SockPrint(sock, "        %6u File(s) Deleted\r\n", numFiles);

    return ERR_OK;
    }


DWORD DIR(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "DIR");
        return ERR_SYNTAX;
        }

    WIN32_FIND_DATA findData;
    SYSTEMTIME sysTime;
    ULARGE_INTEGER available;
    ULARGE_INTEGER total;
    ULARGE_INTEGER free;
    HANDLE hFind;
    unsigned numFiles=0;
    unsigned numDirs=0;
    DWORD totalSize=0;

    hFind = FindFirstFile(nextToken, &findData);

    nextToken[3] = '\0'; // just leave the drive
    SockPrint(sock, "\r\n Directory of %s\r\n\r\n", nextToken);

    if(hFind == INVALID_HANDLE_VALUE)
        {
        SockPrint(sock, "No files found (ec: %lu)\r\n", GetLastError());
        }

    do
        {
        if(hFind == INVALID_HANDLE_VALUE) break;

        FileTimeToSystemTime(&findData.ftLastWriteTime, &sysTime);
        SockPrint(sock, "%02d/%02d/%04d  %02d:%02d%c %5s %9lu %s\r\n", sysTime.wMonth, sysTime.wDay, sysTime.wYear, (sysTime.wHour>12?sysTime.wHour-12:sysTime.wHour), sysTime.wMinute, (sysTime.wHour>12?'p':'a'), (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY?"<DIR>":""), findData.nFileSizeLow, findData.cFileName);

        if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) ++numDirs;
        else ++numFiles;
        totalSize += findData.nFileSizeLow;
        } while(FindNextFile(hFind, &findData) == TRUE);
    FindClose(hFind);

    available.QuadPart = 0;
    GetDiskFreeSpaceEx(nextToken, &available, &total, &free);

    SockPrint(sock, "        %6u File(s)  %12lu bytes\r\n", numFiles, totalSize);
    SockPrint(sock, "        %6u Dir(s)   %12I64u bytes free\r\n", numDirs, (unsigned __int64)available.QuadPart);

    return ERR_OK;
    }


DWORD ECHO(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        return ERR_SYNTAX;
        }

    SockPrint(sock, "%s\r\n", nextToken);
    DebugPrint("%s\n", nextToken);

    return ERR_OK;
    }


DWORD FORMAT(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "FORMAT");
        return ERR_SYNTAX;
        }
    bool mu = false;        // are we formatting an MU?
    DWORD port, slot;

    OBJECT_STRING deviceName;
    char muName[128];

    if(sscanf(nextToken, "\\Device\\MemoryUnit%02x_%02x", &port, &slot) == 2)
        {
        mu = true;
        if(usbmanager->devices[port][slot] != NULL && usbmanager->devices[port][slot]->type == XDEVICE_TYPE_MEMORY_UNIT)
            {
            delete (DeviceMU*)(usbmanager->devices[port][slot]);
            usbmanager->devices[port][slot] = NULL;
            }
        }

    RtlInitObjectString(&deviceName, nextToken);

    if(mu) OpenMU(port-1, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, muName);

    //DebugPrint("FORMAT: FormatFATVolume\n");
    if(XapiFormatFATVolume(&deviceName))
        {
        SockPrint(sock, "Format succeeded\r\n");
        }
    else
        {
        SockPrint(sock, "Format failed (ec: %lu)\r\n", GetLastError());
        }

    if(mu) CloseMU(port-1, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT);

    return ERR_OK;
    }


DWORD MKDIR(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "MKDIR");
        return ERR_SYNTAX;
        }

    if(CreateDirectory(nextToken, NULL) == FALSE)
        {
        SockPrint(sock, "        Unable to create directory (ec: %lu)\r\n", GetLastError());
        }

    return ERR_OK;
    }


DWORD MOVE(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "MOVE");
        return ERR_SYNTAX;
        }

    WIN32_FIND_DATA findData;
    HANDLE hFind;
    unsigned numFiles=0;
    char *file1, *file2;

    char* newFile = strchr(nextToken, ' ');
    if(!newFile)
        {
        HELP(sock, "MOVE");
        return ERR_SYNTAX;
        }
    *newFile = '\0';
    newFile += 1;

    char *path1 = new char[1024];
    char *path2 = new char[1024];

    if(!path1 || !path2)
        {
        delete[] path1;
        delete[] path2;
        return ERR_NOMEM;
        }

    strcpy(path1, nextToken);
    strcpy(path2, newFile);

    file1 = xPathFindFileName(path1);
    file2 = xPathFindFileName(path2);
    if(!file1 || !file2)
        {
        delete[] path1;
        delete[] path2;
        SockPrint(sock, "No files found.\r\n");
        return 1;
        }
    *file1 = '\0';
    *file2 = '\0';

    hFind = FindFirstFile(nextToken, &findData);
    if(hFind == INVALID_HANDLE_VALUE)
        {
        delete[] path1;
        delete[] path2;
        SockPrint(sock, "No files found (ec: %lu)\r\n", GetLastError());
        return 1;
        }

    do
        {
        if(strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;

        strcpy(file1, findData.cFileName);
        strcpy(file2, findData.cFileName);

        if(MoveFile(path1, path2) != TRUE)
            {
            SockPrint(sock, "        Unable to move the file: %s (ec: %lu)\r\n", path1, GetLastError());
            }
        else ++numFiles;
        } while(FindNextFile(hFind, &findData) == TRUE);

    FindClose(hFind);

    SockPrint(sock, "        %6u file(s) moved\r\n", numFiles);

    delete[] path1;
    delete[] path2;

    return ERR_OK;
    }


DWORD REM(SOCKET sock, char *nextToken)
    {
    return ERR_OK;
    }


DWORD RMDIR(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "RMDIR");
        return ERR_SYNTAX;
        }

    if(RemoveDirectory(nextToken) == FALSE)
        {
        SockPrint(sock, "        Unable to remove directory (ec: %lu)\r\n", GetLastError());
        }

    return ERR_OK;
    }


void PrintVariable(char *name, void *p, void *param)
    {
    if(!p) return;
    SOCKET sock = (SOCKET)param;
    Variable *v = *(Variable**)p;
    SockPrint(sock, "%s=%s\r\n", name, v->value);
    }
DWORD SET(SOCKET sock, char *nextToken)
    {
    extern HashTable<Variable*, 13> variables;

    if(!*nextToken)
        {
        variables.ForAll(PrintVariable, (void*)sock);
        SockPrint(sock, "\r\n");
        return ERR_OK;
        }

    char *value = strchr(nextToken, '=');
    if(!value)
        {
        HELP(sock, "SET");
        return ERR_SYNTAX;
        }

    *value = '\0';
    ++value;

    variables.Add(nextToken, new Variable(value));

    return ERR_OK;
    }


DWORD TIMEDATE(SOCKET sock, char *nextToken)
    {
    SYSTEMTIME sysTime;

    /* TODO: SetSystemTime is currently not exported
    char ampm;
    if(*nextToken)
        {
        int items = sscanf(nextToken, "%02d/%02d/%04d %02d:%02d%c", &sysTime.wMonth, &sysTime.wDay, &sysTime.wYear, &sysTime.wHour, &sysTime.wMinute, &ampm);
        if(items == 6)
            {
            if(ampm == 'p' || ampm == 'P') sysTime.wHour += 12;
            SetSystemTime(&sysTime);
            }
        else
            {
            SockPrint(sock, "Set failed: use this syntax: MM/DD/YYYY HH:MM[a|p]\r\n");
            }
        }*/

    GetSystemTime(&sysTime);
    SockPrint(sock, "%02d/%02d/%04d %02d:%02d%c\r\n", sysTime.wMonth, sysTime.wDay, sysTime.wYear, (sysTime.wHour>12?sysTime.wHour-12:sysTime.wHour), sysTime.wMinute, (sysTime.wHour>12?'p':'a'));

    return ERR_OK;
    }


DWORD TYPE(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "TYPE");
        return ERR_SYNTAX;
        }

    DWORD len = 0;
    const unsigned chunkSize = 1024;
    unsigned char *buff = new unsigned char[chunkSize];
    if(!buff)
        {
        SockPrint(sock, "Error: Not enough memory.\r\n");
        return ERR_NOMEM;
        }

    HANDLE hFile = CreateFile(nextToken, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        {
        SockPrint(sock, "Could not open file (ec: %lu)\r\n", GetLastError());
        delete[] buff;
        return ERR_FILE;
        }
    for(DWORD i=0; i<GetFileSize(hFile, NULL); i+=chunkSize)
        {
        ReadFile(hFile, buff, chunkSize, &len, NULL);
        send(sock, (char*)buff, len, 0);
        }

    CloseHandle(hFile);
    delete[] buff;

    return ERR_OK;
    }


} // namespace Launcher

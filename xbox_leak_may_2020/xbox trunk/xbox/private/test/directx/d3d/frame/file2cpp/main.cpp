
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                                    LPTSTR szCmdLine, int nCmdShow) 
{
    int                     ret = -1;
    HANDLE                  hFile;
    PBYTE                   pdata;
    DWORD                   dwSize, dwRead, dwWritten;
    LPSTR                   pszFile, psz;
    char                    szName[512];
    char                    szNewFile[512];
    char                    szWrite[4096];
    char                    szIndent[] = "\r\n    ";
    UINT                    i;
    DWORD                   dwAttributes;

    dwAttributes = GetFileAttributes(TEXT("modulex.h"));
    if ((dwAttributes != 0xFFFFFFFF) && (dwAttributes & FILE_ATTRIBUTE_READONLY)) {
        MessageBox(NULL, TEXT("The file modulex.h has read-only attributes\n"), TEXT("Abort"), MB_ICONSTOP);
        return 0;
    }

    // Open the file
    hFile = CreateFile(szCmdLine, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, TEXT("The specified resource file was not found\n"), TEXT("Abort"), MB_ICONSTOP);
        return 0;
    }

    // Get the size of the file
    dwSize = GetFileSize(hFile, NULL);
    if (dwSize == 0xFFFFFFFF) {
        CloseHandle(hFile);
        return 0;
    }

    // Allocate a buffer for the file data
    pdata = new BYTE[dwSize];
    if (!pdata) {
        CloseHandle(hFile);
        return 0;
    }                

    // Read the file into the buffer
    if (!ReadFile(hFile, pdata, dwSize, &dwRead, NULL)
        || dwSize != dwRead)
    {
        delete [] pdata;
        CloseHandle(hFile);
        return 0;
    }

    // Close the file
    CloseHandle(hFile);

    for (pszFile = szCmdLine + strlen(szCmdLine) - 1; pszFile > szCmdLine && *pszFile != TEXT('\\'); pszFile--);
    if (*pszFile == TEXT('\\')) {
        pszFile++;
    }

    strcpy(szName, pszFile);
    for (psz = szName; *psz != TEXT('\0') && *psz != TEXT('.'); psz++);
    *psz = TEXT('\0');

    sprintf(szNewFile, "%s.cpp", pszFile);

    hFile = CreateFile(szNewFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        delete [] pdata;
        return 0;
    }

    sprintf(szWrite, "/*******************************************************************************\r\n\r\nCopyright (c) 2000 Microsoft Corporation.  All rights reserved.\r\n\r\nFile Name:\r\n\r\n    %s\r\n\r\nDescription\r\n\r\n    Data file for the resource %s.\r\n\r\n*******************************************************************************/\r\n\r\n#include \"d3dlocus.h\"\r\n\r\nstatic BYTE g_%s[] = {\r\n", szNewFile, pszFile, szName);
    WriteFile(hFile, szWrite, strlen(szWrite), &dwWritten, NULL);

    for (i = 0; i < dwSize; i++) {
        if (!(i % 16)) {
            WriteFile(hFile, szIndent, 5, &dwWritten, NULL);
        }
        sprintf(szWrite, "0x%02X, ", pdata[i]);
        WriteFile(hFile, szWrite, strlen(szWrite), &dwWritten, NULL);
    }

    sprintf(szWrite, "\r\n};\r\n\r\nstatic RESOURCEDATA g_rd_%s(TEXT(MODULE_STRING), TEXT(\"%s\"), g_%s, sizeof(g_%s));\r\n\r\n", szName, pszFile, szName, szName);
    WriteFile(hFile, szWrite, strlen(szWrite), &dwWritten, NULL);

    sprintf(szWrite, "extern \"C\" LPVOID PREPEND_MODULE(_%s) = (LPVOID)&g_rd_%s;\r\n", szName, szName);
    WriteFile(hFile, szWrite, strlen(szWrite), &dwWritten, NULL);

    CloseHandle(hFile);

    // Delete the file buffer
    delete [] pdata;

    hFile = CreateFile(TEXT("modulex.h"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, TEXT("Failed to create/open file linkinc.h\n"), TEXT("Abort"), MB_ICONSTOP);
        return 0;
    }

    if (SetFilePointer(hFile, 0, NULL, FILE_END) == 0xFFFFFFFF) {
        MessageBox(NULL, TEXT("Failed to advance to the end of file linkinc.h\n"), TEXT("Abort"), MB_ICONSTOP);
        CloseHandle(hFile);
        return 0;
    }

    sprintf(szWrite, "#pragma comment(linker, \"/include:_module_%s\")\r\n", szName);
    WriteFile(hFile, szWrite, strlen(szWrite), &dwWritten, NULL);

    CloseHandle(hFile);

    return ret;
}




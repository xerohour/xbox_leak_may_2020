/*++

Copyright (c) Microsoft Corporation.  All right reserved.

Module Name:

    command.cpp

Abstract:

    This module contains routine to process external commands

--*/

#include "certspyp.h"

typedef struct {

    PCSTR Command;
    PDM_CMDPROC CommandProc;

} COMMAND_TABLE, *PCOMMAND_TABLE;

LPSTR _stristr(
    IN LPCSTR str1,
    IN LPCSTR str2
    )
{
    LPSTR cp = (LPSTR)str1;
    LPSTR s1, s2;

    if (!*str2) {
        return (LPSTR)str1;
    }

    while (*cp) {
        s1 = cp;
        s2 = (LPSTR)str2;

        while (*s1 && *s2 && _tolower(*s1) == _tolower(*s2)) {
            s1++, s2++;
        }

        if (!*s2) {
            return cp;
        }

        cp++;
    }

    return NULL;
}

LPSTR
GetParamPointer(
    IN LPCSTR szCmd,
    IN LPCSTR szName
    )
{
    if (!szCmd) {
        return NULL;
    }

    LPSTR p = _stristr(szCmd, szName);

    if (p && (p = strchr(p, '=')) != NULL) {
        p++;
        while (isspace(*p)) {
            p++;
        }
    }

    return p;
}

BOOL
GetDwordParam(
    IN LPCSTR szCmd,
    IN LPCSTR szName,
    IN LPDWORD pdwResult
    )
{
    LPSTR p = GetParamPointer(szCmd, szName);

    if (p) {
        *pdwResult = strtoul(p, NULL, 0);
    }

    return p != NULL;
}

BOOL
GetStringParam(
    IN LPCSTR szCmd,
    IN LPCSTR szName,
    OUT LPSTR* pszResult
    )
{
    *pszResult = GetParamPointer(szCmd, szName);
    return *pszResult != NULL;
}

HRESULT
WINAPI
CertSpyHookKernelImport(
    IN  PCSTR   szCommand,
    OUT PSTR    szResponse,
    IN  SIZE_T  cchResponse,
    IN  PDM_CMDCONT pdmcc
    )
{
    PSTR OrdinalList;
    ULONG OrdinalNumber;
    PSTR Token;
    HRESULT hr = S_OK;
    NTSTATUS Status;

    if (!GetStringParam(szCommand, "ordinal", &OrdinalList)) {
        return E_INVALIDARG;
    }

    Token = strtok(OrdinalList, ",");

    while (Token) {

        OrdinalNumber = strtoul(Token, NULL, 0);

        if (OrdinalNumber >= KernelExportSize) {
            hr = E_INVALIDARG;
            break;
        }

        Status = CertSpyHookKernelImportOrdinal(OrdinalNumber);

        if (!NT_SUCCESS(Status)) {
            hr = HRESULT_FROM_NT(Status);
            break;
        }

        Token = strtok(NULL, ",");
    }

    return hr;
}

HRESULT
WINAPI
CertSpyUnhookKernelImport(
    IN  PCSTR   szCommand,
    OUT PSTR    szResponse,
    IN  SIZE_T  cchResponse,
    IN  PDM_CMDCONT pdmcc
    )
{
    PSTR OrdinalList;
    ULONG OrdinalNumber;
    PSTR Token;
    HRESULT hr = S_OK;
    NTSTATUS Status;

    if (!GetStringParam(szCommand, "ordinal", &OrdinalList)) {
        return E_INVALIDARG;
    }

    Token = strtok(OrdinalList, ",");

    while (Token) {

        OrdinalNumber = strtoul(Token, NULL, 0);

        if (OrdinalNumber >= KernelExportSize) {
            hr = E_INVALIDARG;
            break;
        }

        Status = CertSpyUnhookKernelImportOrdinal(OrdinalNumber);

        if (!NT_SUCCESS(Status)) {
            hr = HRESULT_FROM_NT(Status);
            break;
        }

        Token = strtok(NULL, ",");
    }

    return hr;
}

HRESULT
WINAPI
CertSpyReportVersionInfo(
    IN PDM_CMDCONT pdmcc,
    IN PSTR szResponse,
    IN DWORD cchResponse
    )
{
    if (pdmcc->BytesRemaining++ == 1) {
        int n = _snprintf((char*)pdmcc->Buffer, pdmcc->BufferSize,
                    "version=%s maxordinal=%d",
                    VER_PRODUCTVERSION_STR, KernelExportSize);
        return n < 0 ? XBDM_BUFFER_TOO_SMALL : XBDM_NOERR;
    }

    return XBDM_ENDOFLIST;
}

HRESULT
WINAPI
CertSpyGetVersionInfo(
    IN  PCSTR   szCommand,
    OUT PSTR    szResponse,
    IN  SIZE_T  cchResponse,
    IN  PDM_CMDCONT pdmcc
    )
{
    pdmcc->HandlingFunction = CertSpyReportVersionInfo;
    pdmcc->BytesRemaining = 1;
    return XBDM_MULTIRESPONSE;
}

HRESULT
WINAPI
CertSpyReportHookList(
    IN PDM_CMDCONT pdmcc,
    IN PSTR szResponse,
    IN DWORD cchResponse
    )
{
    const size_t MaxOrdinalPerLine = 15;
    size_t NumberOfOrdinals = 0;
    size_t BufferSize = pdmcc->BufferSize;
    PORDINAL_THUNK Thunk;
    ULONG Ordinal;
    int n;

    ASSERT(pdmcc->BytesRemaining <= KernelExportSize);

    n = _snprintf((char*)pdmcc->Buffer, BufferSize, "ordinal=");

    if (n < 0) {
        return XBDM_BUFFER_TOO_SMALL;
    }

    BufferSize -= n;

    while (pdmcc->BytesRemaining) {

        pdmcc->BytesRemaining--;
        Ordinal = pdmcc->BytesRemaining;
        Thunk = InterceptThunks[Ordinal];

        if (Thunk) {

            n = _snprintf(&((char*)pdmcc->Buffer)[pdmcc->BufferSize-BufferSize],
                    BufferSize, "%s%d", NumberOfOrdinals ? "," : "", Ordinal);

            if (n < 0) {
                return XBDM_BUFFER_TOO_SMALL;
            }

            BufferSize -= n;

            if (++NumberOfOrdinals > MaxOrdinalPerLine) {
                break;
            }
        }
    }

    return NumberOfOrdinals ? XBDM_NOERR : XBDM_ENDOFLIST;
}

HRESULT
WINAPI
CertSpyGetHookList(
    IN  PCSTR   szCommand,
    OUT PSTR    szResponse,
    IN  SIZE_T  cchResponse,
    IN  PDM_CMDCONT pdmcc
    )
{
    pdmcc->HandlingFunction = CertSpyReportHookList;
    pdmcc->BytesRemaining = KernelExportSize;
    return XBDM_MULTIRESPONSE;
}

HRESULT
WINAPI
CertSpyReportImportList(
    IN PDM_CMDCONT pdmcc,
    IN PSTR szResponse,
    IN DWORD cchResponse
    )
{
    const size_t MaxOrdinalPerLine = 15;
    size_t NumberOfOrdinals = 0;
    size_t BufferSize = pdmcc->BufferSize;
    PIMAGE_THUNK_DATA Thunk;
    ULONG Ordinal;
    int n;

    ASSERT(pdmcc->BytesRemaining <= KernelExportSize);

    n = _snprintf((char*)pdmcc->Buffer, BufferSize, "ordinal=");

    if (n < 0) {
        return XBDM_BUFFER_TOO_SMALL;
    }

    BufferSize -= n;

    while (pdmcc->BytesRemaining) {

        pdmcc->BytesRemaining--;
        Ordinal = pdmcc->BytesRemaining;
        Thunk = ImageThunks[Ordinal];

        //
        // Only list function imports, won't list pointer imports
        //

        if (Thunk && KernelExports[Ordinal].FunctionName) {

            n = _snprintf(&((char*)pdmcc->Buffer)[pdmcc->BufferSize-BufferSize],
                    BufferSize, "%s%d", NumberOfOrdinals ? "," : "", Ordinal);

            if (n < 0) {
                return XBDM_BUFFER_TOO_SMALL;
            }

            BufferSize -= n;

            if (++NumberOfOrdinals > MaxOrdinalPerLine) {
                break;
            }
        }
    }

    return NumberOfOrdinals ? XBDM_NOERR : XBDM_ENDOFLIST;
}

HRESULT
WINAPI
CertSpyGetImportList(
    IN  PCSTR   szCommand,
    OUT PSTR    szResponse,
    IN  SIZE_T  cchResponse,
    IN  PDM_CMDCONT pdmcc
    )
{
    pdmcc->HandlingFunction = CertSpyReportImportList;
    pdmcc->BytesRemaining = KernelExportSize;
    return XBDM_MULTIRESPONSE;
}

//
// Command table lookup, this list must be sorted
//

const COMMAND_TABLE CertSpyCommandTable[] = {
    { "hookkernel",     CertSpyHookKernelImport     },
    { "hooklist",       CertSpyGetHookList          },
    { "importlist",     CertSpyGetImportList        },
    { "unhookkernel",   CertSpyUnhookKernelImport   },
    { "version",        CertSpyGetVersionInfo       },
};

HRESULT
WINAPI
CertSpyCommandProcessor(
    IN  PCSTR   szCommand,
    OUT PSTR    szResponse,
    IN  SIZE_T  cchResponse,
    IN  PDM_CMDCONT pdmcc
    )
{
    const COMMAND_TABLE* pCmdTable;
    int min, middle, max;

    PCSTR pCmd = strchr(szCommand, '!');

    if (!pCmd) {
        return E_UNEXPECTED;
    } else {
        pCmd++;
    }

    //
    // Binary search for command procedure from command table
    //

    min = 0;
    max = ARRAYSIZE(CertSpyCommandTable);

    while (min < max) {

        middle = (min + max) / 2;
        pCmdTable = &CertSpyCommandTable[middle];

        int n = _strnicmp(pCmdTable->Command, pCmd, strlen(pCmdTable->Command));

        if (n == 0) {
            break;
        } else if (n < 0) {
            min = middle + 1;
        } else {
            max = middle;
        }
    }

    if (min >= max) {
        return XBDM_INVALIDCMD;
    }

    return pCmdTable->CommandProc(szCommand, szResponse, cchResponse, pdmcc);
}


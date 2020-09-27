/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbflash.cpp

Abstract:

    This is internal tool used to flash the Xbox ROM image.  The tool sends
    the kernel image using debug channel.

--*/

#include "xbflashp.h"

VOID
XbFlashDisplayUsageAndExit(
    VOID
    )
/*++

Routine Description:

    This routine prints usage information about how to use the program and
    terminates the process.  The routine will not return.

Argument:

    None

Return Value:

    None, this routine will terminate the process

--*/
{
    fprintf(stderr,
        "\nUsage:\txbflash [-x name or IP [-y]] image\n\n"
        "\t-x\tIP address or name of Xbox\n"
        "\t-y\tUseful for batch file to suppress prompting\n"
        "\t\tTo prevent innocent Xbox from being flashed accidentally,\n"
        "\t\tthe parameter -y requires -x\n"
        "\timage\tValid Xbox kernel image, "
        "xbflash will not validate the image\n\n");
    exit(1);
}

VOID
XbFlashTranslateError(
    IN HRESULT Hr
    )
/*++

Routine Description:

    This routine translates HRESULT from DM APIs, prints to stardard error

Arguments:

    Hr - HRESULT returned vt various DM APIs

Return Value:

    None

--*/
{
    CHAR Buffer[512];

    DmTranslateError(Hr, Buffer, ARRAYSIZE(Buffer));
    fprintf(stderr, "%s\n", Buffer);
}

PCSTR
XbFlashFormatErrorMessage(
    IN DWORD Win32ErrorCode
    )
/*++

Routine Description:

    This routine formats Win32 error code to a message string.  It uses
    FormatMessage to search the system's message table resource(s) for
    the message definition.

    Note: this routine is not thread-safe.

Arguments:

    Win32ErrorCode - Win32 error code, usually returned from GetLastError

Return Value:

    Pointer to static formatted buffer

--*/
{
    static CHAR MsgBuf[256];

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, Win32ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        MsgBuf, sizeof(MsgBuf), NULL);

    return MsgBuf;
}

PVOID
ReadKernelImage(
    IN  PCSTR FileName,
    OUT SIZE_T* ImageSize
    )
/*++

Routine Description:

    This routine allocates memory and read the content of file specified by
    the parameter FileName to allocated buffer.  The CRC32 of the image is
    also calculated and stored at the first 32-bit of buffer.  The actual
    content of image start after the CRC32.

Argument:

    FileName - Pointer to null-terminated ANSI string of file name

    ImageSize - Pointer to SIZE_T to retreive size of kernel image

Return Value:

    Buffer contains content of image or NULL if error occurred.  The buffer
    is allocated using LocalAlloc, the caller must free the buffer using
    LocalFree.

--*/
{
    SIZE_T FileSize;
    PDWORD ImageBuffer = NULL;
    DWORD BytesRead = 0;
    HANDLE hFile;

    hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, 0,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE ) {
        fprintf(stderr, "Unable to open kernel image file: %s\n",
            XbFlashFormatErrorMessage(GetLastError()));
        return NULL;
    }

    FileSize = GetFileSize(hFile, 0 );

    if ((FileSize & (FileSize - 1)) != 0) {
        fprintf(stderr, "Invalid kernel image size\n");
        goto cleanup;
    }

    ImageBuffer = (PDWORD)LocalAlloc(LPTR, FileSize);

    if (!ImageBuffer) {
        fprintf(stderr, "Unable to allocate memory\n");
        goto cleanup;
    }

    ReadFile(hFile, ImageBuffer, FileSize, &BytesRead, NULL);

    if (BytesRead != FileSize) {
        fprintf(stderr, "Error reading kernel image file: %s\n",
            XbFlashFormatErrorMessage(GetLastError()));
        LocalFree(ImageBuffer);
        ImageBuffer = NULL;
    } else {
        *ImageSize = FileSize;
    }

cleanup:
    CloseHandle(hFile);
    return ImageBuffer;
}

HRESULT
XbFlashImage(
    IN PVOID ImageBuffer,
    IN SIZE_T ImageSize,
    IN BOOL IgnoreVersionChecking
    )
/*++

Routine Description:

    This routine sends request along with kernel image to debug monitor
    asking it to flash to flash ROM.

Argument:

    ImageBuffer - Pointer to memory buffer contains kernel image. The first
        DWORD of the buffer is CRC32 of the kernel image.

    ImageSize - Size of buffer containing kernel image

Return Value:

    HRESULT of operation

--*/
{
    HRESULT hr;
    CHAR szResp[256];
    CHAR szCmd[64];
    PDM_CONNECTION pdmc;
    DWORD cchResp, Crc;

    hr = DmOpenConnection(&pdmc);

    if (FAILED(hr)) {
        return hr;
    }

    Crc = Crc32(0xFFFFFFFF, ImageBuffer, ImageSize);
    sprintf(szCmd, "flash length=0x%x crc=0x%x ignoreversionchecking=0x%x",
        ImageSize, Crc, IgnoreVersionChecking);
    cchResp = ARRAYSIZE(szResp);

    hr = DmSendCommand(pdmc, szCmd, szResp, &cchResp);

    if (hr != XBDM_READYFORBIN) {
        fputs("Mismatch version of xbdm.dll on the specified Xbox\n", stderr);
        return XBDM_NOERR;
    }

    fprintf(stdout, "Sending image file...\n");
    hr = DmSendBinary(pdmc, ImageBuffer, ImageSize);

    if (FAILED(hr)) {
        XbFlashTranslateError(hr);
    } else {
        fprintf(stdout, "Flashing, this might take couple minutes...\n");

        cchResp = ARRAYSIZE(szResp);
        hr = DmReceiveStatusResponse(pdmc, szResp, &cchResp);
        fprintf(stdout, "%s\n", &szResp[5]);

        if (SUCCEEDED(hr)) {
            fprintf(stdout, "New kernel will be used in next cold reboot\n");
        }
    }

    DmCloseConnection(pdmc);

    return hr;
}

int __cdecl main(int argc, char *argv[])
{
    int  i;
    char *arg;
    HRESULT hr = S_OK;
    DWORD len;
    CHAR XboxName[256];
    BOOL fPrompt = TRUE;
    BOOL fFlash = TRUE;
    BOOL fIgnoreVersionChecking = FALSE;
    PCSTR pszFileName = NULL;
    PCSTR pszXboxName = NULL;
    PVOID ImageBuffer;
    SIZE_T ImageSize;

    //
    // Process and validate command lines
    //

    pszXboxName = getenv("XBOXIP");
    if (pszXboxName) {
        DmSetXboxNameNoRegister(pszXboxName);
        pszXboxName = NULL;
    }

    for (i=1; i<argc; i++) {
        arg = argv[i];
        if (*arg == '/' || *arg == '-') {
            arg++;
            if (*arg == 'x' || *arg == 'X') {
                pszXboxName = (++i < argc ? argv[i] : NULL);
            } else if (*arg == 'y' || *arg == 'Y') {
                fPrompt = FALSE;
            } else if (*arg == 'i' || *arg == 'I') {
                fIgnoreVersionChecking = TRUE;
            }
        } else if (!pszFileName) {
            pszFileName = arg;
        } else {
            XbFlashDisplayUsageAndExit();
        }
    }

    //
    // To prevent innocent Xbox from being flashed accidentally, the
    // parameter -y requires -x
    //

    if (!pszFileName || (!pszXboxName && !fPrompt)) {
        XbFlashDisplayUsageAndExit();
    }

    ImageBuffer = ReadKernelImage(pszFileName, &ImageSize);

    if (!ImageBuffer) {
        return 1;
    }

    //
    // Validate the Xbox machine name and prompt if necessary
    //

    if (pszXboxName) {
        hr = DmSetXboxName(pszXboxName);
    }

    if (SUCCEEDED(hr)) {
        len = ARRAYSIZE(XboxName);
        hr = DmGetXboxName(XboxName, &len);
    }

    if (SUCCEEDED(hr)) {
        fprintf(stdout, "\nFlash kernel image to '%s'\n", XboxName);
        if (fPrompt) {
            fprintf(stdout, "Continue? ");
            if (toupper(getchar()) != 'Y') {
                fFlash = FALSE;
            }
        }
        if (fFlash) {
            hr = XbFlashImage(ImageBuffer, ImageSize, fIgnoreVersionChecking);
        }
    }

    if (FAILED(hr)) {
        XbFlashTranslateError(hr);
    }

    LocalFree(ImageBuffer);
    return 0;
}

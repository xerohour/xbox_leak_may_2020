/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vpshader.cpp

Author:

    Matt Bronder

Description:

    Direct3D vertex and pixel shader routines.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// Vertex shader functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateVertexShader
//
// Description:
//
//     Create a vertex shader using the given declaration, function, and
//     usage.  The given function is specified in shader source code in a
//     file or resource of the given name.
//
// Arguments:
//
//     CDevice8* pDevice                    - Pointer to the device object
//
//     LPDWORD pdwDeclaration               - Shader declaration token array
//
//     LPCTSTR szFunction                   - Shader function source code
//
//     DWORD dwUsage                        - Usage controls
//
// Return Value:
//
//     A handle to the created vertex shader on success, INVALID_SHADER_HANDLE
//     on failure.
//
//******************************************************************************
DWORD CreateVertexShader(CDevice8* pDevice, LPDWORD pdwDeclaration,
                            LPCTSTR szFunction, DWORD dwUsage)
{
    DWORD   dwHandle;
    LPBYTE  pData;
    DWORD   dwSize;
    HRESULT hr;

    if (!LoadResourceData(szFunction, (LPVOID*)&pData, &dwSize)) {

#ifndef UNDER_XBOX

        HGLOBAL     hg;
        HRSRC       hrsrc;

        if (hrsrc = FindResource(GetTestInstance(), szFunction, RT_RCDATA)) {
            if (hg = LoadResource(GetTestInstance(), hrsrc)) {
                if (pData = (LPBYTE)LockResource(hg)) {
                    dwSize = SizeofResource(GetTestInstance(), hrsrc);
                }
            }
        }
#else
        pData = NULL;
#endif // UNDER_XBOX
    }

    if (pData) {

        hr = pDevice->CreateVertexShaderAsm(pdwDeclaration, (LPVOID)pData, dwSize, &dwHandle, dwUsage);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            return INVALID_SHADER_HANDLE;
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

            hr = pDevice->CreateVertexShaderAsm(pdwDeclaration, pData, dwSize, &dwHandle, dwUsage);

            pDevice->DeleteVertexShader(INVALID_SHADER_HANDLE);

            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
                return INVALID_SHADER_HANDLE;
            }
        }

        UnloadResourceData(szFunction);
    }

    else {

        // Attempt to load the shader as a file
        HANDLE  hFile;
        DWORD   dwRead;
#ifdef UNDER_XBOX
        char    aszFunction[MAX_PATH];

        wcstombs(aszFunction, szFunction, MAX_PATH);
        hFile = CreateFile(aszFunction, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
        hFile = CreateFile(szFunction, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif // !UNDER_XBOX

        if (hFile == INVALID_HANDLE_VALUE) {
            DebugString(TEXT("Shader file %s not found"), szFunction);
            return INVALID_SHADER_HANDLE;
        }

        // Get the size of the file
        dwSize = GetFileSize(hFile, NULL);
        if (dwSize == 0xFFFFFFFF) {
            CloseHandle(hFile);
            return INVALID_SHADER_HANDLE;
        }

        // Allocate a buffer for the file data
        pData = (LPBYTE)MemAlloc(dwSize);
        if (!pData) {
            CloseHandle(hFile);
            return INVALID_SHADER_HANDLE;
        }                

        // Read the file into the buffer
        if (!ReadFile(hFile, pData, dwSize, &dwRead, NULL)
            || dwSize != dwRead)
        {
            MemFree(pData);
            CloseHandle(hFile);
            return INVALID_SHADER_HANDLE;
        }

        // Close the file
        CloseHandle(hFile);

        hr = pDevice->CreateVertexShaderAsm(pdwDeclaration, (LPVOID)pData, dwSize, &dwHandle, dwUsage);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            MemFree(pData);
            return INVALID_SHADER_HANDLE;
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

            hr = pDevice->CreateVertexShaderAsm(pdwDeclaration, pData, dwSize, &dwHandle, dwUsage);

            pDevice->DeleteVertexShader(INVALID_SHADER_HANDLE);

            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
                MemFree(pData);
                return INVALID_SHADER_HANDLE;
            }
        }

        MemFree(pData);
    }

    return dwHandle;
}

//******************************************************************************
//
// Function:
//
//     ReleaseVertexShader
//
// Description:
//
//     Delete the vertex shader corresponding to the given handle.
//
// Arguments:
//
//     CDevice8* pDevice                    - Pointer to the device object
//
//     DWORD dwHandle                       - Shader handle
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseVertexShader(CDevice8* pDevice, DWORD dwHandle) {

    if (dwHandle != INVALID_SHADER_HANDLE) {
        pDevice->DeleteVertexShader(dwHandle);
    }
}

//******************************************************************************
// Pixel shader functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreatePixelShader
//
// Description:
//
//     Create a pixel shader using the given function.  The given function is 
//     specified in shader source code in a file or resource of the given name.
//
// Arguments:
//
//     CDevice8* pDevice                    - Pointer to the device object
//
//     LPCTSTR szFunction                   - Shader function source code
//
// Return Value:
//
//     A handle to the created pixel shader on success, INVALID_SHADER_HANDLE
//     on failure.
//
//******************************************************************************
DWORD CreatePixelShader(CDevice8* pDevice, LPCTSTR szFunction) {

    DWORD           dwHandle;
    HRSRC           hrsrc;
    LPBYTE          pData;
    DWORD           dwSize;
    HRESULT         hr;

    if (!LoadResourceData(szFunction, (LPVOID*)&pData, &dwSize)) {

#ifndef UNDER_XBOX

        HGLOBAL     hg;
        HRSRC       hrsrc;

        if (hrsrc = FindResource(GetTestInstance(), szFunction, RT_RCDATA)) {
            if (hg = LoadResource(GetTestInstance(), hrsrc)) {
                if (pData = (LPBYTE)LockResource(hg)) {
                    dwSize = SizeofResource(GetTestInstance(), hrsrc);
                }
            }
        }
#else
        pData = NULL;
#endif // UNDER_XBOX
    }

    if (pData) {

        hr = pDevice->CreatePixelShaderAsm((LPVOID)pData, dwSize, &dwHandle);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            return INVALID_SHADER_HANDLE;            
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

            hr = pDevice->CreatePixelShaderAsm((LPVOID)pData, dwSize, &dwHandle);

            pDevice->DeletePixelShader(INVALID_SHADER_HANDLE);

            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
                return INVALID_SHADER_HANDLE;
            }
        }

        UnloadResourceData(szFunction);
    }

    else {

        // Attempt to load the shader as a file
        HANDLE  hFile;
        DWORD   dwRead;
#ifdef UNDER_XBOX
        char    aszFunction[MAX_PATH];

        wcstombs(aszFunction, szFunction, MAX_PATH);
        hFile = CreateFile(aszFunction, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
        hFile = CreateFile(szFunction, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif // !UNDER_XBOX

        if (hFile == INVALID_HANDLE_VALUE) {
            DebugString(TEXT("Shader file %s not found"), szFunction);
            return INVALID_SHADER_HANDLE;
        }

        // Get the size of the file
        dwSize = GetFileSize(hFile, NULL);
        if (dwSize == 0xFFFFFFFF) {
            CloseHandle(hFile);
            return INVALID_SHADER_HANDLE;
        }

        // Allocate a buffer for the file data
        pData = (LPBYTE)MemAlloc(dwSize);
        if (!pData) {
            CloseHandle(hFile);
            return INVALID_SHADER_HANDLE;
        }                

        // Read the file into the buffer
        if (!ReadFile(hFile, pData, dwSize, &dwRead, NULL)
            || dwSize != dwRead)
        {
            MemFree(pData);
            CloseHandle(hFile);
            return INVALID_SHADER_HANDLE;
        }

        // Close the file
        CloseHandle(hFile);

        hr = pDevice->CreatePixelShaderAsm((LPVOID)pData, dwSize, &dwHandle);

        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            MemFree(pData);
            return INVALID_SHADER_HANDLE;            
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

            hr = pDevice->CreatePixelShaderAsm((LPVOID)pData, dwSize, &dwHandle);

            pDevice->DeletePixelShader(INVALID_SHADER_HANDLE);

            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
                MemFree(pData);
                return INVALID_SHADER_HANDLE;
            }
        }

        MemFree(pData);
    }

    return dwHandle;
}

//******************************************************************************
//
// Function:
//
//     ReleasePixelShader
//
// Description:
//
//     Delete the pixel shader corresponding to the given handle.
//
// Arguments:
//
//     CDevice8* pDevice                    - Pointer to the device object
//
//     DWORD dwHandle                       - Shader handle
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleasePixelShader(CDevice8* pDevice, DWORD dwHandle) {

    if (dwHandle != INVALID_SHADER_HANDLE) {
        pDevice->DeletePixelShader(dwHandle);
    }
}

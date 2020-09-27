/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vpshader.cpp

Author:

    Matt Bronder

Description:

    Direct3D vertex and pixel shader routines.

*******************************************************************************/

#include "d3dbase.h"

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
//     LPDIRECT3DDEVICE8 pDevice            - Pointer to the device object
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
DWORD CreateVertexShader(LPDIRECT3DDEVICE8 pDevice, LPDWORD pdwDeclaration,
                            LPCTSTR szFunction, DWORD dwUsage)
{
#ifndef UNDER_XBOX
    LPD3DXBUFFER    pxbufShader, pxbufErrors;
#else
    LPXGBUFFER      pxbufShader, pxbufErrors;
#endif // UNDER_XBOX
    LPVOID          pData;
    DWORD           dwSize;
    DWORD           dwHandle;
    HRESULT         hr;

    if (LoadResourceFile(szFunction, &pData, &dwSize)) {

#ifndef UNDER_XBOX
        hr = D3DXAssembleShader(pData, dwSize, 0, NULL, &pxbufShader, &pxbufErrors);
#else
        hr = AssembleShader("<memory>", pData, dwSize, 0, NULL, 
                            &pxbufShader, &pxbufErrors, NULL, NULL, NULL, NULL);
#endif // UNDER_XBOX

        UnloadResourceFile(szFunction);

        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("AssembleShader"));
#ifndef UNICODE
            DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
#else
            DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxbufErrors->GetBufferPointer());
#endif
            pxbufErrors->Release();
            return INVALID_SHADER_HANDLE;
        }

        pxbufErrors->Release();

        hr = pDevice->CreateVertexShader(pdwDeclaration, (LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle, dwUsage);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            pxbufShader->Release();
            return INVALID_SHADER_HANDLE;            
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

            hr = pDevice->CreateVertexShader(pdwDeclaration, (LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle, dwUsage);

            pDevice->DeleteVertexShader(INVALID_SHADER_HANDLE);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
                pxbufShader->Release();
                return INVALID_SHADER_HANDLE;
            }
        }

        pxbufShader->Release();
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
        pData = MemAlloc(dwSize);
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

#ifndef UNDER_XBOX
        hr = D3DXAssembleShader(pData, dwSize, 0, NULL, &pxbufShader, &pxbufErrors);
#else
        hr = AssembleShader("<memory>", pData, dwSize, 0, NULL, 
                            &pxbufShader, &pxbufErrors, NULL, NULL, NULL, NULL);
#endif // UNDER_XBOX
        MemFree(pData);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("AssembleShader"));
#ifndef UNICODE
            DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
#else
            DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxbufErrors->GetBufferPointer());
#endif
            pxbufErrors->Release();
            return INVALID_SHADER_HANDLE;
        }

        pxbufErrors->Release();

        hr = pDevice->CreateVertexShader(pdwDeclaration, (LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle, dwUsage);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
            pxbufShader->Release();
            return INVALID_SHADER_HANDLE;            
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

            hr = pDevice->CreateVertexShader(pdwDeclaration, (LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle, dwUsage);

            pDevice->DeleteVertexShader(INVALID_SHADER_HANDLE);

            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexShader"))) {
                pxbufShader->Release();
                return INVALID_SHADER_HANDLE;
            }
        }

        pxbufShader->Release();
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
//     LPDIRECT3DDEVICE8 pDevice            - Pointer to the device object
//
//     DWORD dwHandle                       - Shader handle
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseVertexShader(LPDIRECT3DDEVICE8 pDevice, DWORD dwHandle) {

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
//     LPDIRECT3DDEVICE8 pDevice            - Pointer to the device object
//
//     LPCTSTR szFunction                   - Shader function source code
//
// Return Value:
//
//     A handle to the created pixel shader on success, INVALID_SHADER_HANDLE
//     on failure.
//
//******************************************************************************
DWORD CreatePixelShader(LPDIRECT3DDEVICE8 pDevice, LPCTSTR szFunction) {

    DWORD           dwHandle;
#ifndef UNDER_XBOX
    LPD3DXBUFFER    pxbufShader, pxbufErrors;
#else
    LPXGBUFFER      pxbufShader, pxbufErrors;
#endif // UNDER_XBOX
    HRSRC           hrsrc;
    LPVOID          pData;
    DWORD           dwSize;
    HRESULT         hr;

    if (LoadResourceFile(szFunction, &pData, &dwSize)) {

#ifndef UNDER_XBOX
        hr = D3DXAssembleShader(pData, dwSize, 0, NULL, &pxbufShader, &pxbufErrors);
#else
        hr = AssembleShader("<memory>", pData, dwSize, 0, NULL, 
                            &pxbufShader, &pxbufErrors, NULL, NULL, NULL, NULL);
#endif // UNDER_XBOX

        UnloadResourceFile(szFunction);

        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("AssembleShader"));
#ifndef UNICODE
            DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
#else
            DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxbufErrors->GetBufferPointer());
#endif
            pxbufErrors->Release();
            return INVALID_SHADER_HANDLE;
        }

        pxbufErrors->Release();

#ifndef UNDER_XBOX
        hr = pDevice->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle);
#else
        hr = pDevice->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwHandle);
#endif // UNDER_XBOX
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            pxbufShader->Release();
            return INVALID_SHADER_HANDLE;            
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

#ifndef UNDER_XBOX
            hr = pDevice->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle);
#else
            hr = pDevice->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwHandle);
#endif // UNDER_XBOX

            pDevice->DeletePixelShader(INVALID_SHADER_HANDLE);

            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
                pxbufShader->Release();
                return INVALID_SHADER_HANDLE;
            }
        }

        pxbufShader->Release();
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
        pData = MemAlloc(dwSize);
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

#ifndef UNDER_XBOX
        hr = D3DXAssembleShader(pData, dwSize, 0, NULL, &pxbufShader, &pxbufErrors);
#else
        hr = AssembleShader("<memory>", pData, dwSize, 0, NULL, 
                            &pxbufShader, &pxbufErrors, NULL, NULL, NULL, NULL);
#endif // UNDER_XBOX
        MemFree(pData);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("AssembleShader"));
#ifndef UNICODE
            DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
#else
            DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxbufErrors->GetBufferPointer());
#endif
            pxbufErrors->Release();
            return INVALID_SHADER_HANDLE;
        }

        pxbufErrors->Release();

#ifndef UNDER_XBOX
        hr = pDevice->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle);
#else
        hr = pDevice->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwHandle);
#endif // UNDER_XBOX
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
            pxbufShader->Release();
            return INVALID_SHADER_HANDLE;            
        }

        if (dwHandle == INVALID_SHADER_HANDLE) {

#ifndef UNDER_XBOX
            hr = pDevice->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), &dwHandle);
#else
            hr = pDevice->CreatePixelShader((D3DPIXELSHADERDEF*)pxbufShader->GetBufferPointer(), &dwHandle);
#endif // UNDER_XBOX

            pDevice->DeletePixelShader(INVALID_SHADER_HANDLE);

            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
                pxbufShader->Release();
                return INVALID_SHADER_HANDLE;
            }
        }

        pxbufShader->Release();
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
//     LPDIRECT3DDEVICE8 pDevice            - Pointer to the device object
//
//     DWORD dwHandle                       - Shader handle
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleasePixelShader(LPDIRECT3DDEVICE8 pDevice, DWORD dwHandle) {

    if (dwHandle != INVALID_SHADER_HANDLE) {
        pDevice->DeletePixelShader(dwHandle);
    }
}

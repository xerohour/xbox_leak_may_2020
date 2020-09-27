/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    mesh.cpp

Description:

    Direct3D mesh routines.

*******************************************************************************/

#include "d3dbase.h"

//******************************************************************************
// Mesh functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateMesh
//
// Description:
//
//     Create vertex and index geometry for use with DrawIndexedPrimitve from
//     a given .xdg file or resources.
//
// Arguments:
//
//     LPCTSTR szMesh                       - File or resource name of the 
//                                            .xdg file to create the mesh 
//                                            from
//
//     PMESH pmesh (Out)                    - Structure to be initialized
//                                            with the geometry of the mesh
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CreateMesh(LPCTSTR szMesh, PMESH pmesh) {

    MESH    mesh;
    LPBYTE  pData;
    DWORD   dwSize;

    if (!pmesh) {
        return FALSE;
    }

    memset(pmesh, 0, sizeof(MESH));

    if (LoadResourceFile(szMesh, (LPVOID*)&pData, &dwSize)) {

        // Get the number of vertices
        memcpy(&(mesh.uNumVertices), pData, sizeof(DWORD));
        pData += sizeof(DWORD);

        // Get the vertices
        mesh.prVertices = (PVERTEX)MemAlloc32(mesh.uNumVertices * 
                                                  sizeof(VERTEX));
        if (!mesh.prVertices) {
            UnloadResourceFile(szMesh);
            return FALSE;
        }

        memcpy(mesh.prVertices, pData, mesh.uNumVertices * sizeof(VERTEX));
        pData += (mesh.uNumVertices * sizeof(VERTEX));

        // Get the number of indices
        memcpy(&(mesh.uNumIndices), pData, sizeof(DWORD));
        pData += sizeof(DWORD);

        // Get the indices
        mesh.pwIndices = (LPWORD)MemAlloc32(mesh.uNumIndices * sizeof(WORD));
        if (!mesh.pwIndices) {
            MemFree32(mesh.prVertices);
            UnloadResourceFile(szMesh);
            return FALSE;
        }

        memcpy(mesh.pwIndices, pData, mesh.uNumIndices * sizeof(WORD));

        UnloadResourceFile(szMesh);
    }
    else {

        // The mesh is a file
        DWORD   dwRead;
        HANDLE  hFile;
#ifdef UNDER_XBOX
        char    aszMesh[MAX_PATH];

        wcstombs(aszMesh, szMesh, MAX_PATH);
        hFile = CreateFile(aszMesh, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
                                  FILE_ATTRIBUTE_NORMAL, NULL);
#else
        hFile = CreateFile(szMesh, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
                                  FILE_ATTRIBUTE_NORMAL, NULL);
#endif // !UNDER_XBOX

        if (hFile == INVALID_HANDLE_VALUE) {
            DebugString(TEXT("Mesh %s was not found"), szMesh);
            return FALSE;
        }

        // Get the number of vertices
        if (!ReadFile(hFile, &(mesh.uNumVertices), sizeof(DWORD), &dwRead, NULL) 
            || dwRead != sizeof(DWORD)) 
        {
            CloseHandle(hFile);
            return FALSE;
        }

        // Get the vertices
        mesh.prVertices = (PVERTEX)MemAlloc32(mesh.uNumVertices * 
                                                  sizeof(VERTEX));
        if (!mesh.prVertices) {
            CloseHandle(hFile);
            return FALSE;
        }

        if (!ReadFile(hFile, mesh.prVertices, 
            mesh.uNumVertices * sizeof(VERTEX), &dwRead, NULL) 
            || dwRead != mesh.uNumVertices * sizeof(VERTEX)) 
        {
            MemFree32(mesh.prVertices);
            CloseHandle(hFile);
            return FALSE;
        }

        // Get the number of indices
        if (!ReadFile(hFile, &(mesh.uNumIndices), sizeof(DWORD), &dwRead, NULL) 
            || dwRead != sizeof(DWORD)) 
        {
            MemFree32(mesh.prVertices);
            CloseHandle(hFile);
            return FALSE;
        }

        // Get the indices
        mesh.pwIndices = (LPWORD)MemAlloc32(mesh.uNumIndices * sizeof(WORD));
        if (!mesh.pwIndices) {
            MemFree32(mesh.prVertices);
            CloseHandle(hFile);
            return FALSE;
        }

        if (!ReadFile(hFile, mesh.pwIndices, 
            mesh.uNumIndices * sizeof(WORD), &dwRead, NULL) 
            || dwRead != mesh.uNumIndices * sizeof(WORD)) 
        {
            MemFree32(mesh.prVertices);
            MemFree32(mesh.pwIndices);
            CloseHandle(hFile);
            return FALSE;
        }

        CloseHandle(hFile);
    }

    memcpy(pmesh, &mesh, sizeof(MESH));
    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ReleaseMesh
//
// Description:
//
//     Release the allocated vertex and index geometry of a created mesh.
//
// Arguments:
//
//     PMESH pmesh                          - Pointer to the mesh structure
//                                            to release
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseMesh(PMESH pmesh) {

    if (pmesh) {
        if (pmesh->prVertices) {
            MemFree32(pmesh->prVertices);
        }
        if (pmesh->pwIndices) {
            MemFree32(pmesh->pwIndices);
        }
    }
    memset(pmesh, 0, sizeof(MESH));
}

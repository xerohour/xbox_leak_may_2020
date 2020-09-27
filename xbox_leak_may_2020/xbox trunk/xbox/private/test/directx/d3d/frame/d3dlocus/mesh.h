/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    mesh.h

Description:

    Direct3D mesh routines.

*******************************************************************************/

#ifndef __MESH_H__
#define __MESH_H__

//******************************************************************************
// Structures
//******************************************************************************

typedef struct _MESH {
    PVERTEX         prVertices;
    UINT            uNumVertices;
    LPWORD          pwIndices;
    UINT            uNumIndices;
} MESH, *PMESH;

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL                CreateMesh(LPCTSTR szMesh, PMESH pmesh);
void                ReleaseMesh(PMESH pmesh);

#endif //__MESH_H__

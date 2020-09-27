/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vpshader.h

Author:

    Matt Bronder

Description:

    Direct3D vertex and pixel shader routines.

*******************************************************************************/

#ifndef __VPSHADER_H__
#define __VPSHADER_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define INVALID_SHADER_HANDLE   0xFFFFFFFE

//******************************************************************************
// Function prototypes
//******************************************************************************

DWORD                   CreateVertexShader(CDevice8* pDevice,
                                            LPDWORD pdwDeclaration,
                                            LPCTSTR szFunction = NULL, 
                                            DWORD dwUsage = 0);
void                    ReleaseVertexShader(CDevice8* pDevice, DWORD dwHandle);

DWORD                   CreatePixelShader(CDevice8* pDevice,
                                            LPCTSTR szFunction);
void                    ReleasePixelShader(CDevice8* pDevice, DWORD dwHandle);

#endif //__VPSHADER_H__

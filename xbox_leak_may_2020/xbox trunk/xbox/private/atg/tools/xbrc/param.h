//-----------------------------------------------------------------------------
//
//  File: param.h
//
//  Copyright (C) 2001 Microsoft Corporation.
//  All rights reserved.
//
//  Author: Jerome E. Lengyel
//  Description: Parameter setting from application to XDX resources.
//
//-----------------------------------------------------------------------------

// One parameter type for each kind of parameter that may be set
struct Parameter_VertexShaderConstant {
	DWORD m_VertexShaderResource;	// vertex shader resource to write constant for
	DWORD m_Index;					// index into constants
	DWORD m_Count;					// count of constants to write
	VOID *m_pSource;				// source data
};

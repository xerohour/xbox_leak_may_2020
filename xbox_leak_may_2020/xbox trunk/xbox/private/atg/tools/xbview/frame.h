//-----------------------------------------------------------------------------
//  
//  File: frame.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  A frame hierarchy for nested transformations.
//
//-----------------------------------------------------------------------------
#pragma once

#ifdef _XBOX
#include <xtl.h>
#else
#include <d3dx8math.h>
typedef __declspec(align(16)) D3DXMATRIX XGMATRIX;
#define XGMatrixMultiply D3DXMatrixMultiply
#endif

#define FRAME_IDENTIFIER_SIZE 64

// hierarchies can be no wider or deeper than FRAME_MAX_STACK
#define FRAME_MAX_STACK 200

struct Frame {
	XGMATRIX   m_Matrix;							// local transformation
    CHAR       m_strName[FRAME_IDENTIFIER_SIZE];	// name of this frame
	Frame *    m_pChild;							// child frame
	Frame *    m_pNext;								// sibling frame
};

struct Skeleton {
	DWORD       m_FrameCount;	// total number of frames
	XGMATRIX *  m_rMatrix;		// cumulative transformations computed from frame hierarchy in depth-first traversal
	Frame *		m_pRoot;		// root of frame hierarchy
	
	//////////////////////////////////////////////////////////////////////
	// Set root of frame hierarchy and allocate matrix array
	// to hold one matrix per node in the hierarchy.
	//
	HRESULT SetRoot(Frame *pRoot);

	//////////////////////////////////////////////////////////////////////
	// Do depth-first traversal of frame hierarchy to compute the
	// matrix palette
	//
	HRESULT UpdateTransformations();
};

//////////////////////////////////////////////////////////////////////
// Debugging output
//
HRESULT PrintMatrix(XGMATRIX *pMat);
HRESULT PrintFrame(Frame *pFrame);


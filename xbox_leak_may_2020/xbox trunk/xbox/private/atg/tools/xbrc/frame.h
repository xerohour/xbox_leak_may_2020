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
typedef XGMATRIX FrameMatrix;

#define FRAME_IDENTIFIER_SIZE 64

// hierarchies can be no wider or deeper than FRAME_MAX_STACK
#define FRAME_MAX_STACK 200

struct Frame {
	FrameMatrix m_Matrix;							// local transformation
    CHAR       m_strName[FRAME_IDENTIFIER_SIZE];	// name of this frame
	Frame     *m_pChild;							// child frame
	Frame     *m_pNext;								// sibling frame
};

struct Skeleton {
	DWORD       m_FrameCount;	// total number of frames
	FrameMatrix *m_rMatrix;		// cumulative transformations computed by frame hierarchy depth-first traversal
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

#if 1

//////////////////////////////////////////////////////////////////////
// Add extra information to frame to make parsing and
// file writing easier.
//
struct AnimFrame;	// forward reference
struct FrameExtra : public Frame {
	AnimFrame *m_pAnim;	                            // accumulated animation expression
	AnimFrame *m_pAnimLocal;						// current animation defined by <animate> block
	DWORD	m_Offset;								// file offset

	FrameExtra();
	~FrameExtra();
	HRESULT AddChild(FrameExtra *pFrame);
	HRESULT Premult(AnimFrame *pAnim);
	static HRESULT DeleteHierarchy(FrameExtra *pFrame);
};

#endif




//-----------------------------------------------------------------------------
//  
//  File: anim.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Animation using key frames.
//
//-----------------------------------------------------------------------------
#pragma once

#include "frame.h"

//////////////////////////////////////////////////////////////////////
// When we go to the codegen model, all of this interpretation code
// can go away.  For now, we skip the compile/link step and write out
// a token stream that is interpreted in xbview.
//
#ifndef ANIM_INTERPRETED
#define ANIM_INTERPRETED 1
#endif

#if ANIM_INTERPRETED
enum AnimType {
	ANIM_MATRIX,
	ANIM_ROTATE,
	ANIM_TRANSLATE,
	ANIM_SCALE,
	ANIM_ROTATEBLEND,
	ANIM_TRANSLATEBLEND,
	ANIM_SCALEBLEND,
	ANIM_CONCAT,
};
#endif

////////////////////////////////////////////////////////////////////////////////
// Generic frame animation
//

struct AnimFrame
{
#if !ANIM_INTERPRETED
	
    //////////////////////////////////////////////////////////////////////
    // Compute the transformation matrix based on the current animation keys.
    virtual HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime) = 0;
	
#else	
	AnimType m_Type; // Instead of using virtual functions, we key off the type, which can be written to the preview file.

    //////////////////////////////////////////////////////////////////////
    // Compute the transformation matrix based on the current animation keys.
	//
	// This version does not use virtual functions, but instead
	// uses the m_Type field to choose which subclass to cast the
	// 'this' pointer to.
	HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);

    //////////////////////////////////////////////////////////////////////
	// For writing to a file, we need to compute the size of the
	// structure data and the size of the key data (which may need to be
	// aligned.)
	//
	DWORD GetSize();

    //////////////////////////////////////////////////////////////////////
	// Write the expression to the given memory location, adjusting
	// pointer offsets to be relative to pBase, i.e. pDest - pBase.
	// The number of bytes written must match the value returned by
	// GetSize().
	//
	HRESULT RelocateCopy(BYTE *pDest, const BYTE *pBase);

    //////////////////////////////////////////////////////////////////////
	// Patch file offsets to memory offsets, i.e. convert file offsets
	// to pointers relative to pBase
	//
	HRESULT Patch(const BYTE *pBase);
#endif
};

//////////////////////////////////////////////////////////////////////
// Basic key-framed animation, with variable size keys.  Based on
// the SMIL2.0 semantics for the <animate> tag.
//
struct AnimCurve {
    
    // animation information
    UINT m_KeyCount;    // number of keys in each array
    FLOAT *m_rKeyTimes; // m_KeyCount length
    UINT m_KeySize;     // number of floats in a key
    FLOAT *m_rKeys;     // m_KeySize * m_KeyCount length
    // FLOAT *m_rKeySplines;

    // Local time transformation, fLocal = (fGlobalTime - m_fBegin) * m_fDurationInverse;
    float m_fBegin;             // start of animation
    float m_fDurationInverse;   // inverse duration of animation
    // TODO: more SMIL stuff
    // enum { CalcMode_discrete, CalcMode_linear, CalcMode_paced, CalcMode_spline } CalcMode;
    // float m_fRepeatCount;
    // float m_fRepeatDur;

    //////////////////////////////////////////////////////////////////////
    // Compute local time
    FLOAT LocalTime(FLOAT fGlobalTime) CONST
    {
        // TODO: more SMIL stuff, repeats, fill mode, etc.
        return (fGlobalTime - m_fBegin) * m_fDurationInverse;
    }
    
    //////////////////////////////////////////////////////////////////////
    // Find the bracketing key times
    HRESULT FindKeys(FLOAT fLocalTime, UINT *piKey1, UINT *piKey2, FLOAT *pfLerpValue) CONST;

    //////////////////////////////////////////////////////////////////////
    // Find the bracketing key times, then lerp the values
    HRESULT Sample(FLOAT fLocalTime, FLOAT *rfValue) CONST;

#if ANIM_INTERPRETED
	AnimCurve()
	{
		m_KeyCount = 0;
		m_rKeyTimes = NULL;
		m_KeySize = 0;
		m_rKeys = NULL;
		m_fBegin = 0.f;
		m_fDurationInverse = 1.f;
	}
	
    //////////////////////////////////////////////////////////////////////
	// For writing to a file, we need to compute the size of the key data.
	//
	DWORD GetSizeKeys() CONST;

    //////////////////////////////////////////////////////////////////////
	// Write the keys to the given memory location, then adjust
	// pointer offsets to be relative to pBase, i.e. pDest - pBase.
	// The number of bytes written must match the value returned by
	// GetSizeKeys().
	//
	HRESULT RelocateCopyKeys(BYTE *pDest, const BYTE *pBase) CONST;

    //////////////////////////////////////////////////////////////////////
	// Patch file offsets to memory offsets, i.e. convert file offsets
	// to pointers relative to pBase
	//
	HRESULT PatchKeys(const BYTE *pBase);
#endif
};

////////////////////////////////////////////////////////////////////////////////
// Frame animation types
//
struct AnimMatrix : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimMatrix()
	{
		m_Type = ANIM_MATRIX;
		m_curve.m_KeySize = 16;
	}
	AnimMatrix(D3DXMATRIX *pMatrix)
	{
		// Create a static matrix
		m_Type = ANIM_MATRIX;
		m_curve.m_KeySize = 16;
		m_curve.m_KeyCount = 1;
		m_curve.m_rKeyTimes = new FLOAT [ 1 ];
		m_curve.m_rKeyTimes[0] = 0.f;
		m_curve.m_rKeys = new FLOAT [ 1 * 16 ];
		memcpy(m_curve.m_rKeys, &pMatrix->m[0][0], sizeof(FLOAT) * 16);
	}
#endif	
	AnimCurve m_curve;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);
};

struct AnimRotate : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimRotate()
	{
		m_Type = ANIM_ROTATE;
		m_curve.m_KeySize = 4;
	}
	AnimRotate(D3DXQUATERNION *pQuaternion)
	{
		// Create a static rotation
		m_Type = ANIM_ROTATE;
		m_curve.m_KeySize = 4;
		m_curve.m_KeyCount = 1;
		m_curve.m_rKeyTimes = new FLOAT [ 1 ];
		m_curve.m_rKeyTimes[0] = 0.f;
		m_curve.m_rKeys = new FLOAT [ 1 * 4 ];
		memcpy(m_curve.m_rKeys, pQuaternion, sizeof(FLOAT) * 4);
	}
#endif
	AnimCurve m_curve;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);
    HRESULT SetTime(D3DXQUATERNION *pQuaternion, float fGlobalTime);
};

struct AnimTranslate : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimTranslate()
	{
		m_Type = ANIM_TRANSLATE;
		m_curve.m_KeySize = 3;
	}
	AnimTranslate(D3DXVECTOR3 *pTranslate)
	{
		// Create a static translate
		m_Type = ANIM_TRANSLATE;
		m_curve.m_KeySize = 3;
		m_curve.m_KeyCount = 1;
		m_curve.m_rKeyTimes = new FLOAT [ 1 ];
		m_curve.m_rKeyTimes[0] = 0.f;
		m_curve.m_rKeys = new FLOAT [ 1 * 3 ];
		memcpy(m_curve.m_rKeys, pTranslate, sizeof(FLOAT) * 3);
	}
#endif	
	AnimCurve m_curve;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);
    HRESULT SetTime(D3DXVECTOR3 *pTranslate, float fGlobalTime);
};

struct AnimScale : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimScale()
	{
		m_Type = ANIM_SCALE;
		m_curve.m_KeySize = 3;
	}
	AnimScale(D3DXVECTOR3 *pScale)
	{
		// Create a static scale
		m_Type = ANIM_SCALE;
		m_curve.m_KeySize = 3;
		m_curve.m_KeyCount = 1;
		m_curve.m_rKeyTimes = new FLOAT [ 1 ];
		m_curve.m_rKeyTimes[0] = 0.f;
		m_curve.m_rKeys = new FLOAT [ 1 * 3 ];
		memcpy(m_curve.m_rKeys, pScale, sizeof(FLOAT) * 3);
	}
#endif	
	AnimCurve m_curve;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);
    HRESULT SetTime(D3DXVECTOR3 *pScale, float fGlobalTime);
};



////////////////////////////////////////////////////////////////////////////////
// Parameter blending.  Before creating the matrix, parameters are interpolated.
//
struct AnimRotateBlend : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimRotateBlend()
	{
		m_Type = ANIM_ROTATEBLEND;
	}
#endif	
	float m_fBlend;
	AnimRotate *m_pAnimRotateA;
	AnimRotate *m_pAnimRotateB;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);
    HRESULT SetTime(D3DXQUATERNION *pQuaternion, float fGlobalTime);
};

struct AnimTranslateBlend : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimTranslateBlend()
	{
		m_Type = ANIM_TRANSLATEBLEND;
	}
#endif	
	float m_fBlend;
	AnimTranslate *m_pAnimTranslateA;
	AnimTranslate *m_pAnimTranslateB;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);
    HRESULT SetTime(D3DXVECTOR3 *pTranslate, float fGlobalTime);
};

struct AnimScaleBlend : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimScaleBlend()
	{
		m_Type = ANIM_SCALEBLEND;
	}
#endif	
	float m_fBlend;
	AnimScale *m_pAnimScaleA;
	AnimScale *m_pAnimScaleB;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime);
    HRESULT SetTime(D3DXVECTOR3 *pScale, float fGlobalTime);
};



////////////////////////////////////////////////////////////////////////////////
// Matrix concatenation. Returns m_AnimA * m_AnimB
//
struct AnimConcat : public AnimFrame
{
#if ANIM_INTERPRETED
	AnimConcat()
	{
		m_Type = ANIM_CONCAT;
	}
#endif	
	AnimFrame *m_pAnimA;
	AnimFrame *m_pAnimB;
    HRESULT SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
	{
		HRESULT hr;
		D3DXMATRIX matA;
		hr = m_pAnimA->SetTime(&matA, fGlobalTime);
		if (FAILED(hr))
			return hr;
		D3DXMATRIX matB;
		hr = m_pAnimB->SetTime(&matB, fGlobalTime);
		if (FAILED(hr))
			return hr;
		D3DXMatrixMultiply(pMatrix, &matA, &matB);
		return S_OK;
	}
};




////////////////////////////////////////////////////////////////////////////////
// AnimLink attaches matrix animation to a frame.
//
struct AnimLink
{
    AnimLink      *m_pNext;         // next animation in list
    AnimFrame 	  *m_pAnimFrame;    // animation keys
    Frame         *m_pFrame;        // frame we're applying the animation to
};



////////////////////////////////////////////////////////////////////////////////
// Vertex shader parameter setting
//
// TODO: rework this whole class to cleanly separate animations from application of animations, 
// skeleton hierarchy from use of concatenated transformation

struct AnimVertexShaderParameter
{
    AnimVertexShaderParameter *m_pNext; // next animated vertex shader parameter in list
    CHAR *m_strVertexShaderReference;   // name of vertex shader
    UINT m_Index;                       // index into constants
    UINT m_Count;                       // count of constants to write
    CHAR *m_strExpression;              // expression to evaluate
    // TODO: fix this ugliness
    VOID *m_pSource; // this is the only kind of vertex shader animation we currently support
};

struct AnimVSP
{
    DWORD m_VertexShaderReference;  // vertex shader resource to write constant for
    DWORD m_Index;                  // index into constants
    DWORD m_Count;                  // count of constants to write
    VOID *m_pSource;                // pointer to source matrix
};

////////////////////////////////////////////////////////////////////////////////
// Animation resource that holds a list of vertex shader parameters to be set,
// and a list of animation expressions to evaluate.
//
struct Animation
{
    DWORD m_VSPCount;
    AnimVSP *m_rVSP;        // array of vertex shader parameter settings
    DWORD m_AnimCount;
    AnimLink *m_pAnimList;
};


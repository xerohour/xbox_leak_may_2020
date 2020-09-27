//-----------------------------------------------------------------------------
// File: face.h
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __FACE_H
#define __FACE_H

#include "xobj.h"

// face animation class
class CFaceAni
{
public:
	char *name;
	DWORD m_dwNumFrames;			// # of frames 
	DWORD m_dwNumControls;			// # of control points
	D3DXVECTOR3 *m_vecOffsets;		// frame offsets
	float *m_fJawFrames;			// jaw animation info

	CFaceAni();
	~CFaceAni();

	DWORD LoadAni(char *anifname, char *jawfname);
	DWORD LoadJaw(char *fname);
};

// control weight structure
struct _cweights
{
	DWORD cidx;						// control index
	float weight;					// weight
};

struct _cwptrs
{
	struct _cweights *ptr;			// pointer into control weight array
	DWORD cnt;						// number of weights for this vertex
};

// eyelid weights
struct _seyewt
{
	DWORD flags;
	DWORD vidx;
	float weight;
};

// face class
class CFace
{
public:
	CXObject m_objFace;				// face object

	float m_fBlink;					// blink rotation
	CXObject m_objREye;				// right eye object
	D3DXMATRIX m_matREye;
	CXObject m_objLEye;				// left eye object
	D3DXMATRIX m_matLEye;
	LPDIRECT3DTEXTURE8 m_pEyeRef;	// eye reflection texture

	D3DXMATRIX m_matUTeeth;
	CXObject m_objUTeeth;			// upper teeth
	D3DXMATRIX m_matLTeeth;			// lower mouth position/orientation
	D3DXMATRIX m_matLTeethAni;		// lower mouth animation matrix
	CXObject m_objLTeeth;			// lower teeth

	D3DXVECTOR3 *m_vecVerts;			// vertices for deformation

	// weight info
	_cwptrs *m_pWPtrs;				// array of pointers into weights array
	_cweights *m_pWeights;			// list of all weights for every vertex
	DWORD m_dwNEyeWt;				// # of blink vertices
	_seyewt *m_EyeWt;				// eyelid blink weights

	// animations
	CFaceAni *m_Ani;				// current playing animation

	CFace();
	~CFace();

	HRESULT Load(char *fname, char *wfname);
	HRESULT LoadWeights(char *wfname);
	void PlayAni(char *name);
	void Deform(D3DXVECTOR3 *off);
	void Render();
};

#endif
//-----------------------------------------------------------------------------
// File: face.cpp
//
// Desc: routines for loading and displaying facial animations
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include "xbapp.h"
#include "xpath.h"
#include "xparse.h"
#include "face.h"
#include "main.h"

void getmatrix(D3DMATRIX *m, int startidx);		// gets matrix info from text file

// stuff to get eye weights from .m file
// this is a callback function from CXModel->Read_M
int neyewts;
struct _seyewt *feyewts;
void mfunc(int cmd, int ip, float fp);


//-----------------------------------------------------------------------------
// Constructor and destructor
//-----------------------------------------------------------------------------
CFace::CFace()
{
	memset(this, 0, sizeof(CFace));
}

CFace::~CFace()
{
	delete m_EyeWt;
	delete m_vecVerts;
	m_pEyeRef->Release();
}

//-----------------------------------------------------------------------------
// Name: Load
// Desc: Main function to load facial animation using a command file. 
//       The format for the command file is:
//       modelfname imgfname                - model and image
//       weightfname                        - model control point weights
//       eyemodel eyeimage eye3x3 eyetrans  - eye model and image
//                                          - 3x3 orientation matrix
//                                          - eye translation
//       eyereflection                      - eye reflection image
//       upteeth ut3x3 uttrans              - upper teeth model, matrix, & translation
//       lowteeth lt3x3 lttrans             - lower teeth model, matrix, & translation
//-----------------------------------------------------------------------------
HRESULT CFace::Load(char *fname, char *wfname)
{
	FILE *fp;
	char buf[255];
	byte *bptr;
	CXModel *xm;
	D3DXMATRIX m;

	// open the command file
	fp = fopen(fname, "rt");
	if(!fp)
        return E_FAIL;
	
	// face mesh
	fgets(buf, 80, fp);
	Parse(buf, NULL);

	// init eyelid weight memory
	neyewts = 0;
	feyewts = new _seyewt[10000];
	memset(feyewts, 0, 10000*sizeof(_seyewt));

	// do the load
	m_objFace.m_Model = new CXModel;
	xm = m_objFace.m_Model;
	xm->Read_M(_FNA(tokens[0].str), tokens[1].str, FVF_XYZNORMTEX1, 0, mfunc);

	// copy the eyelid weight info
	m_EyeWt = new _seyewt[neyewts];
	memcpy(m_EyeWt, feyewts, neyewts*sizeof(struct _seyewt));
	m_dwNEyeWt = neyewts;
	delete feyewts;

	// get vertices for deformation
	m_vecVerts = new D3DXVECTOR3[xm->m_dwNumVertices];
	xm->LockVB(&bptr, 0L);
	FVF_GetVert(bptr, m_vecVerts, 0, xm->m_dwNumVertices, xm->m_dwFVF);
	xm->UnlockVB();

	// load control point weights
	fgets(buf, 255, fp);			
	Parse(buf, NULL);
	if(LoadWeights(_FNA(tokens[0].str))<0)
	{
		fclose(fp);
		return E_FAIL;
	}

	// right eye
	fgets(buf, 255, fp);			
	Parse(buf, NULL);

	// get the eye matrix and position the eye object
	getmatrix(&m_matREye, 2);
	D3DXMatrixRotationY(&m, D3DX_PI);
	D3DXMatrixMultiply(&m_matREye, &m, &m_matREye);
	m_objREye.m_Model = new CXModel;
	xm = m_objREye.m_Model;
	xm->Sphere(0.0142f, 16, 16, FVF_XYZNORMTEX1, tokens[1].str, 1.0f);

	// left eye
	fgets(buf, 255, fp);			
	Parse(buf, NULL);

	// get the eye matrix and position the eye object
	getmatrix(&m_matLEye, 2);
	D3DXMatrixRotationY(&m, D3DX_PI);
	D3DXMatrixMultiply(&m_matLEye, &m, &m_matLEye);
	m_objLEye.m_Model = new CXModel;
	xm = m_objLEye.m_Model;
	xm->Sphere(0.0142f, 16, 16, FVF_XYZNORMTEX1, tokens[1].str, 1.0f);

	// eye reflection map
	fgets(buf, 255, fp);			
	Parse(buf, NULL);
	XBUtil_CreateTexture(g_pd3dDevice, _FNA(tokens[0].str), &m_pEyeRef);

	// upper teeth
	fgets(buf, 255, fp);			
	Parse(buf, NULL);
	getmatrix(&m_matUTeeth, 1);
	m_objUTeeth.m_Model = new CXModel;
	xm = m_objUTeeth.m_Model;
	xm->Read_M(_FNA(tokens[0].str), NULL, FVF_XYZNORMDIFF, 0, NULL);

	// lower teeth
	fgets(buf, 255, fp);			
	Parse(buf, NULL);
	getmatrix(&m_matLTeeth, 1);
	m_objLTeeth.m_Model = new CXModel;
	xm = m_objLTeeth.m_Model;
	xm->Read_M(_FNA(tokens[0].str), NULL, FVF_XYZNORMDIFF, 0, NULL);

	// init lower jaw animation matrix
	D3DXMatrixIdentity(&m_matLTeethAni);

	fclose(fp);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadWeights
// Desc: Loads control point weights for animation data
//-----------------------------------------------------------------------------
#define MAX_WEIGHTS 100000
HRESULT CFace::LoadWeights(char *wfname)
{
	FILE *fp;
	DWORD vidx, nwt, wdiff;
	DWORD nt, i, j, nvert, wtidx, totnw;
	char buf[512];
	_cweights *wtp;

	fp = fopen(wfname, "rt");
	if(!fp)
        return XBAPPERR_MEDIANOTFOUND;

	// read nverts & verify that it matches face model nverts
	fgets(buf, 512, fp);
	nvert = atoi(buf);
	if((nvert+1)!=m_objFace.m_Model->m_dwNumVertices)
	{
		fclose(fp);
		return E_FAIL;
	}

	// allocate memory for weight pointers
	m_pWPtrs = new _cwptrs[m_objFace.m_Model->m_dwNumVertices];
	memset(m_pWPtrs, 0, m_objFace.m_Model->m_dwNumVertices*sizeof(_cwptrs));

	// allocate temp memory for weights
	wtp = new _cweights[MAX_WEIGHTS];
	wtidx = 0;
	totnw = 0;
	vidx = 1;

	// load the weights
	for(i=1; i<nvert; i++)
	{
		fgets(buf, 512, fp);

		nt = Parse(buf, NULL);
		nwt = atoi(tokens[0].str);			// get # of weights

		// set pointer
		m_pWPtrs[vidx].ptr = &wtp[wtidx];	// pointer
		m_pWPtrs[vidx].cnt = nwt;			// # weights

		// set vertex/weights
		for(j=0; j<nwt; j++)
		{
			fgets(buf, 512, fp);		// read control/weight
			Parse(buf, NULL);

			// add weights
			wtp[wtidx].cidx = atoi(tokens[0].str);
			wtp[wtidx].weight = (float)atof(tokens[1].str);
			wtidx++;
		}

		vidx++;							// bump vertex count
		totnw += nwt;					// count weights
	}

	fclose(fp);

	// allocate memory for the weights
	m_pWeights = new _cweights[totnw];
	memcpy(m_pWeights, wtp, totnw*sizeof(_cweights));

	// adjust pointers because we moved memory
	wdiff = m_pWeights - wtp;
	for(i=0; i<vidx; i++)
		m_pWPtrs[i].ptr += wdiff;

	delete wtp;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: getmatrix
// Desc: Loads a 4x4 matrix with data from the tokens returned by
//       the parser. 
//-----------------------------------------------------------------------------
void getmatrix(D3DMATRIX *m, int idx)
{
	// orientation matrix
	m->_11 = (float)atof(tokens[idx].str);
	m->_12 = (float)atof(tokens[idx+1].str);
	m->_13 = (float)atof(tokens[idx+2].str);
	m->_14 = 0.0f;
	m->_21 = (float)atof(tokens[idx+3].str);
	m->_22 = (float)atof(tokens[idx+4].str);
	m->_23 = (float)atof(tokens[idx+5].str);
	m->_24 = 0.0f;
	m->_31 = (float)atof(tokens[idx+6].str);
	m->_32 = (float)atof(tokens[idx+7].str);
	m->_33 = (float)atof(tokens[idx+8].str);
	m->_34 = 0.0f;
	m->_41 = (float)atof(tokens[idx+9].str);
	m->_42 = (float)atof(tokens[idx+0xa].str);
	m->_43 = (float)atof(tokens[idx+0xb].str);
	m->_44 = 1.0f;
}

//-----------------------------------------------------------------------------
// Name: mfunc
// Desc: Callback invoked by CXModel::Read_M to fill the feyewts array 
//       with data from the m file.
//-----------------------------------------------------------------------------
void mfunc(int cmd, int ip, float fp)
{
	switch(cmd)
	{
		case MFP_EYEWT:								// got an eye weight
			feyewts[neyewts].flags = ip>>16;		// or in flags
			feyewts[neyewts].vidx = ip&0xffff;		// add it to our array
			feyewts[neyewts].weight = fp;
			neyewts++;
			break;
	}
}

//-----------------------------------------------------------------------------
// Name: Deform
// Desc: Uses the animation data to offset the vertices of the face
//       to perform the animation.
//-----------------------------------------------------------------------------
void CFace::Deform(D3DXVECTOR3 *ctrl)
{
	DWORD i, j;
	BYTE *bptr;
	FVFT_XYZNORMTEX1 *vbuf;
	D3DVECTOR *vert;
	D3DVECTOR off, pt;
	_cwptrs *cwpp;
	_cweights *cwp;
	_seyewt *ew;
	CXModel *xm;

	vert = m_vecVerts;					// original vertices
	xm = m_objFace.m_Model;				// face model
	assert(xm->m_dwFVF==FVF_XYZNORMTEX1);	// make sure we have right type of verts
	xm->LockVB(&bptr, 0);
	vbuf = (FVFT_XYZNORMTEX1 *)bptr;

	// calculate the new vertices
	cwpp = m_pWPtrs;
	for(i=0; i<xm->m_dwNumVertices; i++)
	{
		if(cwpp->cnt!=0)
		{
			off.x = 0.0f;
			off.y = 0.0f;
			off.z = 0.0f;

			cwp = cwpp->ptr;
			for(j=0; j<cwpp->cnt; j++)
			{
				off.x += cwp->weight*ctrl[cwp->cidx].x;
				off.y += cwp->weight*ctrl[cwp->cidx].y;
				off.z += cwp->weight*ctrl[cwp->cidx].z;
				cwp++;
			}

			vbuf[i].v.x = vert[i].x + off.x;
			vbuf[i].v.y = vert[i].y + off.y;
			vbuf[i].v.z = vert[i].z + off.z;
		}
		else
		{
			vbuf[i].v.x = vert[i].x;
			vbuf[i].v.y = vert[i].y;
			vbuf[i].v.z = vert[i].z;
		}

		cwpp++;
	}

	// include eye blinks
	D3DXMATRIX m;
	D3DXVECTOR3 vl, vr;

	// rotation axis for the eyelids
	vl.x = -0.948069f; vl.y = 0.026956f; vl.z = 0.316921f;
	vr.x = 0.962104f; vr.y = 0.030789f; vr.z = 0.270939f;

	ew = m_EyeWt;
	for(i=0; i<m_dwNEyeWt; i++)
	{
		off.x = vbuf[ew[i].vidx].v.x;
		off.y = vbuf[ew[i].vidx].v.y;
		off.z = vbuf[ew[i].vidx].v.z;

		if(ew[i].flags&0x0001)		// left eye
		{
			off.x -= m_matLEye._41;		// subtract out eye position
			off.y -= m_matLEye._42;
			off.z -= m_matLEye._43;

			D3DXMatrixRotationAxis(&m, &vl, -m_fBlink*ew[i].weight);
			D3DXVec3TransformCoord((D3DXVECTOR3 *)&pt, (D3DXVECTOR3 *)&off, &m);

			vbuf[ew[i].vidx].v.x = pt.x + m_matLEye._41;		// add eye position back in
			vbuf[ew[i].vidx].v.y = pt.y + m_matLEye._42;
			vbuf[ew[i].vidx].v.z = pt.z + m_matLEye._43;
		}
		else	// right eye
		{
			off.x -= m_matREye._41;
			off.y -= m_matREye._42;
			off.z -= m_matREye._43;

			D3DXMatrixRotationAxis(&m, &vr, m_fBlink*ew[i].weight);
			D3DXVec3TransformCoord((D3DXVECTOR3 *)&pt, (D3DXVECTOR3 *)&off, &m);

			vbuf[ew[i].vidx].v.x = pt.x + m_matREye._41;		// add eye position back in
			vbuf[ew[i].vidx].v.y = pt.y + m_matREye._42;
			vbuf[ew[i].vidx].v.z = pt.z + m_matREye._43;
		}
	}

	xm->UnlockVB();
}


//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the face.
//-----------------------------------------------------------------------------
void CFace::Render()
{
	D3DXMATRIX *m, m2;
	float *ja;
	static DWORD curframe = 0;
	static DWORD flag = 0;

	// animate the face

	// this is a kludge to have the face do something interesting.
	// normally you would build an animation system instead of just
	// playing a canned animation over and over. 
	Deform(m_Ani->m_vecOffsets + m_Ani->m_dwNumControls*curframe);
	curframe++;
	if(curframe==m_Ani->m_dwNumFrames)
		curframe = 0;

	// do jaw animation
	ja = m_Ani->m_fJawFrames;
	ja += curframe*12;

	m_matLTeethAni._11 = ja[0];
	m_matLTeethAni._12 = ja[1];
	m_matLTeethAni._13 = ja[2];
	m_matLTeethAni._21 = ja[3];
	m_matLTeethAni._22 = ja[4];
	m_matLTeethAni._23 = ja[5];
	m_matLTeethAni._31 = ja[6];
	m_matLTeethAni._32 = ja[7];
	m_matLTeethAni._33 = ja[8];
	m_matLTeethAni._41 = ja[9];
	m_matLTeethAni._42 = ja[10];
	m_matLTeethAni._43 = ja[11];
	m_matLTeethAni._44 = 1.0f;

	// enable specular lighting
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	if(g_bLafActive)				// check for the lafortune shader
	{
		// set the vertex shader constants

		// transpose(world) c[0-3]
		D3DXMATRIX mat;
		m_objFace.CrunchMatrix();
		D3DXMatrixTranspose(&mat, &m_objFace.m_matOrientation);
		g_pd3dDevice->SetVertexShaderConstant(0, &mat, 4);		// world matrix

		// transpose((world*view)*proj) c[4-7]
		D3DXMATRIX wvmat, wvpmat;
		D3DXMatrixMultiply(&wvmat, &m_objFace.m_matOrientation, &g_mView);
		D3DXMatrixMultiply(&wvpmat, &wvmat, &g_mProj);		
		D3DXMatrixTranspose(&wvpmat, &wvpmat);
		g_pd3dDevice->SetVertexShaderConstant(4, &wvpmat, 4);	// transformation matrix

		// eye position c[8]
		g_pd3dDevice->SetVertexShaderConstant(8, &g_vEyePos, 4);

		// light position and color c[9-11]
		g_pd3dDevice->SetVertexShaderConstant(9, &g_Light0.Position, 1);
		g_pd3dDevice->SetVertexShaderConstant(10, &g_Light0.Diffuse, 1);
		g_pd3dDevice->SetVertexShaderConstant(11, &g_Light0.Specular, 1);

		// lafortune constants c[12-14]
		g_pd3dDevice->SetVertexShaderConstant(12, &g_vLafConst, 4);

		// do it
		m_objFace.m_Model->m_dwVShader = g_vsLafortune;
		m_objFace.m_Model->Render();
	}
	else							// use the fixed pipeline
	{
		// set texture stage state for the face
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		m_objFace.m_Model->m_dwVShader = m_objFace.m_Model->m_dwFVF;
		m_objFace.Render(0);

	}

	// draw the rest of the face
	m = &m_objFace.m_matOrientation;

	// disable specular
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

	//
	// draw eyes
	//

	// calculate right eye matrix
	D3DXMatrixRotationYawPitchRoll(&m_objREye.m_matOrientation, m_objREye.m_vRotation.y, m_objREye.m_vRotation.x, m_objREye.m_vRotation.z);
	m_objLEye.m_matOrientation = m_objREye.m_matOrientation;
	D3DXMatrixMultiply(&m2, &m_matREye, m);
	D3DXMatrixMultiply(&m_objREye.m_matOrientation, &m_objREye.m_matOrientation, &m2);

	// calculate left eye matrix
	D3DXMatrixMultiply(&m2, &m_matLEye, m);
	D3DXMatrixMultiply(&m_objLEye.m_matOrientation, &m_objLEye.m_matOrientation, &m2);

	// set the texture stages for the eyes and reflections
	// stage 0 is just the eye texture
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// stage 1 adds in the eye reflection
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	// stage 2 modulates with diffuse for lighting effect
    g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// set the stage 1 reflection texture
	// (stage 0 texture is set by the CXModel::Render function)
	g_pd3dDevice->SetTexture(1, m_pEyeRef); 

	// render the eyes using multitexturing
	m_objREye.Render(OBJ_NOMCALCS);
	m_objLEye.Render(OBJ_NOMCALCS);

	// reset the stage 1 texture
	g_pd3dDevice->SetTexture(1, NULL); 

	// disable additional texture stages
    g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

	// disable reflection map
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	//
	// draw the teeth
	//

	// teeth get diffuse only
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	m_objUTeeth.m_Model->m_pTexture = m_pEyeRef;

	// upper teeth	
	D3DXMatrixMultiply(&m_objUTeeth.m_matOrientation, &m_matUTeeth, m);
	m_objUTeeth.Render(OBJ_NOMCALCS);

	// lower teeth
	D3DXMatrixMultiply(&m2, &m_matLTeeth, &m_matLTeethAni);
	D3DXMatrixMultiply(&m_objLTeeth.m_matOrientation, &m2, m);
	m_objLTeeth.Render(OBJ_NOMCALCS);
}


//-----------------------------------------------------------------------------
// facial animation class
// holds the vertex offsets and jaw frames for an animation
//
// Constructor and destructor
//-----------------------------------------------------------------------------
CFaceAni::CFaceAni()
{
	memset(this, 0, sizeof(CFaceAni));
}

CFaceAni::~CFaceAni()
{
	if(m_vecOffsets)
		delete m_vecOffsets;
	if(m_fJawFrames)
		delete m_fJawFrames;
}

//-----------------------------------------------------------------------------
// Name: LoadAni
// Desc: Loads a facial animation data file generated using MSR facial
//       animation capture system.
//-----------------------------------------------------------------------------
DWORD CFaceAni::LoadAni(char *anifname, char *jawfname)
{
	FILE *fp;
	DWORD cnt;

	fp = fopen(anifname, "rb");
	if(!fp)
        return XBAPPERR_MEDIANOTFOUND;

	// read nframes and ncontrol points
	fread(&m_dwNumFrames, sizeof(DWORD), 1, fp);
	fread(&m_dwNumControls, sizeof(DWORD), 1, fp);

	// get memory
	m_vecOffsets = new D3DXVECTOR3[m_dwNumFrames*m_dwNumControls];

	cnt = fread(m_vecOffsets, sizeof(D3DXVECTOR3), m_dwNumFrames*m_dwNumControls, fp);
	assert(cnt==(m_dwNumFrames*m_dwNumControls));
	fclose(fp);

	if(jawfname)
		LoadJaw(jawfname);

	return 1;
}

//-----------------------------------------------------------------------------
// Name: LoadJaw
// Desc: Loads a jaw (mouth) animation data file generated using MSR facial
//       animation capture system.
//-----------------------------------------------------------------------------
DWORD CFaceAni::LoadJaw(char *jawfname)
{
	char buf[255];
	FILE *fp;
	DWORD i, nt, jidx, nframes;

	fp = fopen(jawfname, "rt");
	if(!fp)
        return XBAPPERR_MEDIANOTFOUND;

	// get nframes
	fgets(buf, 255, fp);
	Parse(buf, NULL);
	nframes = atoi(tokens[0].str);

	// verify that nframes matches animation
	if(nframes!=m_dwNumFrames)
	{
		fclose(fp);
		return -1;
	}

	// allocate memory for four sets of 3 floats * nframes
	m_fJawFrames = new float[nframes*12];
	jidx = 0;

	// load the data
	while(!feof(fp))
	{
		fgets(buf, 255, fp);
		if(feof(fp))
			break;

		nt = Parse(buf, NULL);
		
		for(i=2; i<14; i++)
			m_fJawFrames[jidx++] = (float)atof(tokens[i].str);
	}

	fclose(fp);
	return 1;
}


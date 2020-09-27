//-----------------------------------------------------------------------------
// File: xfvf.cpp
//
// Desc: routines for manipulating d3d fixed vertex formats
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "xfvf.h"

//-----------------------------------------------------------------------------
//
// Name: FVF_Get Routines
//
// Desc: gets cnt items starting at vertex first from fvf memory
//       starting at verts which has FVF format of fvf.
//       the vertices are output to vout.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Name: FVF_Set Routines
//
// Desc: sets cnt items starting at vertex first from fvf memory
//       starting at verts which has FVF format of fvf.
//       the vertices are set from vin.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: FVF_VSize
// Desc: returns the size of an FVF
//-----------------------------------------------------------------------------
DWORD FVF_VSize(DWORD fvf)
{
	int size;
	
	size = 0;
	
	if(fvf&D3DFVF_XYZ)
		size += sizeof(D3DVECTOR);
	if(fvf&D3DFVF_XYZRHW)
		size += sizeof(D3DVECTOR)+sizeof(float);
	if(fvf&D3DFVF_NORMAL)
		size += sizeof(D3DVECTOR);
	if(fvf&D3DFVF_DIFFUSE)
		size += sizeof(DWORD);
	if(fvf&D3DFVF_TEX1)
		size += 2*sizeof(float);

	return size;
}

void FVF_GetVert(void *verts, D3DVECTOR *vout, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZDIFF *fvf1;
	FVFT_XYZNORM *fvf2;
	FVFT_XYZTEX1 *fvf3;
	FVFT_XYZRHWTEX1 *fvf4;
	FVFT_XYZDIFFTEX1 *fvf5;
	FVFT_XYZNORMTEX1 *fvf6;
	FVFT_XYZNORMDIFF *fvf7;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZDIFF:
			fvf1 = (FVFT_XYZDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf1[i].v, sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORM:
			fvf2 = (FVFT_XYZNORM *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf2[i].v, sizeof(D3DVECTOR));
			break;
		case FVF_XYZTEX1:
			fvf3 = (FVFT_XYZTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf3[i].v, sizeof(D3DVECTOR));
			break;
		case FVF_XYZRHWTEX1:
			fvf4 = (FVFT_XYZRHWTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf4[i].v, sizeof(D3DVECTOR));
			break;
		case FVF_XYZDIFFTEX1:
			fvf5 = (FVFT_XYZDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf5[i].v, sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMTEX1:
			fvf6 = (FVFT_XYZNORMTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf6[i].v, sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFF:
			fvf7 = (FVFT_XYZNORMDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf7[i].v, sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&vout[idx++], &fvf8[i].v, sizeof(D3DVECTOR));
			break;
	}
}

void FVF_GetNorm(void *verts, D3DVECTOR *nout, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZNORM *fvf2;
	FVFT_XYZNORMTEX1 *fvf6;
	FVFT_XYZNORMDIFF *fvf7;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZNORM:
			fvf2 = (FVFT_XYZNORM *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&nout[idx++], &fvf2[i].norm, sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMTEX1:
			fvf6 = (FVFT_XYZNORMTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&nout[idx++], &fvf6[i].norm, sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFF:
			fvf7 = (FVFT_XYZNORMDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&nout[idx++], &fvf7[i].norm, sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&nout[idx++], &fvf8[i].norm, sizeof(D3DVECTOR));
			break;
	}
}

void FVF_GetDiff(void *verts, DWORD *dout, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZDIFF *fvf1;
	FVFT_XYZDIFFTEX1 *fvf5;
	FVFT_XYZNORMDIFF *fvf7;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZDIFF:
			fvf1 = (FVFT_XYZDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				dout[idx++] = fvf1[i].diff;
			break;
		case FVF_XYZDIFFTEX1:
			fvf5 = (FVFT_XYZDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				dout[idx++] = fvf5[i].diff;
			break;
		case FVF_XYZNORMDIFF:
			fvf7 = (FVFT_XYZNORMDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				dout[idx++] = fvf7[i].diff;
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				dout[idx++] = fvf8[i].diff;
			break;
	}
}

void FVF_GetUV(void *verts, float *uvout, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZTEX1 *fvf3;
	FVFT_XYZRHWTEX1 *fvf4;
	FVFT_XYZDIFFTEX1 *fvf5;
	FVFT_XYZNORMTEX1 *fvf6;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZTEX1:
			fvf3 = (FVFT_XYZTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				uvout[idx*2] = fvf3[i].tu;
				uvout[idx*2+1] = fvf3[i].tv;
				idx++;
			}
			break;
		case FVF_XYZRHWTEX1:
			fvf4 = (FVFT_XYZRHWTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				uvout[idx*2] = fvf4[i].tu;
				uvout[idx*2+1] = fvf4[i].tv;
				idx++;
			}
			break;
		case FVF_XYZDIFFTEX1:
			fvf5 = (FVFT_XYZDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				uvout[idx*2] = fvf5[i].tu;
				uvout[idx*2+1] = fvf5[i].tv;
				idx++;
			}
			break;
		case FVF_XYZNORMTEX1:
			fvf6 = (FVFT_XYZNORMTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				uvout[idx*2] = fvf6[i].tu;
				uvout[idx*2+1] = fvf6[i].tv;
				idx++;
			}
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				uvout[idx*2] = fvf8[i].tu;
				uvout[idx*2+1] = fvf8[i].tv;
				idx++;
			}
			break;
	}
}

void FVF_SetVert(void *verts, D3DVECTOR *vin, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZDIFF *fvf1;
	FVFT_XYZNORM *fvf2;
	FVFT_XYZTEX1 *fvf3;
	FVFT_XYZRHWTEX1 *fvf4;
	FVFT_XYZDIFFTEX1 *fvf5;
	FVFT_XYZNORMTEX1 *fvf6;
	FVFT_XYZNORMDIFF *fvf7;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZDIFF:
			fvf1 = (FVFT_XYZDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf1[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORM:
			fvf2 = (FVFT_XYZNORM *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf2[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZTEX1:
			fvf3 = (FVFT_XYZTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf3[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZRHWTEX1:
			fvf4 = (FVFT_XYZRHWTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf4[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZDIFFTEX1:
			fvf5 = (FVFT_XYZDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf5[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMTEX1:
			fvf6 = (FVFT_XYZNORMTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf6[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFF:
			fvf7 = (FVFT_XYZNORMDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf7[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf8[i].v, &vin[idx++], sizeof(D3DVECTOR));
			break;
	}
}

void FVF_SetNorm(void *verts, D3DVECTOR *nin, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZNORM *fvf2;
	FVFT_XYZNORMTEX1 *fvf6;
	FVFT_XYZNORMDIFF *fvf7;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZNORM:
			fvf2 = (FVFT_XYZNORM *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf2[i].norm, &nin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMTEX1:
			fvf6 = (FVFT_XYZNORMTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf6[i].norm, &nin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFF:
			fvf7 = (FVFT_XYZNORMDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf7[i].norm, &nin[idx++], sizeof(D3DVECTOR));
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				memcpy(&fvf8[i].norm, &nin[idx++], sizeof(D3DVECTOR));
			break;
	}
}

void FVF_SetDiff(void *verts, DWORD *din, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZDIFF *fvf1;
	FVFT_XYZDIFFTEX1 *fvf5;
	FVFT_XYZNORMDIFF *fvf7;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZDIFF:
			fvf1 = (FVFT_XYZDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				fvf1[i].diff = din[idx++];
			break;
		case FVF_XYZDIFFTEX1:
			fvf5 = (FVFT_XYZDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				fvf5[i].diff = din[idx++];
			break;
		case FVF_XYZNORMDIFF:
			fvf7 = (FVFT_XYZNORMDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				fvf7[i].diff = din[idx++];
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				fvf8[i].diff = din[idx++];
			break;
	}
}

void FVF_SetAllDiff(void *verts, DWORD din, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i;

	FVFT_XYZDIFF *fvf1;
	FVFT_XYZDIFFTEX1 *fvf5;
	FVFT_XYZNORMDIFF *fvf7;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	switch(fvf)
	{
		case FVF_XYZDIFF:
			fvf1 = (FVFT_XYZDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				fvf1[i].diff = din;
			break;
		case FVF_XYZDIFFTEX1:
			fvf5 = (FVFT_XYZDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				fvf5[i].diff = din;
			break;
		case FVF_XYZNORMDIFF:
			fvf7 = (FVFT_XYZNORMDIFF *)verts;
			for(i=first; i<first+cnt; i++)
				fvf7[i].diff = din;
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
				fvf8[i].diff = din;
			break;
	}
}

void FVF_SetUV(void *verts, float *uvin, DWORD first, DWORD cnt, DWORD fvf)
{
	DWORD i, idx;

	FVFT_XYZTEX1 *fvf3;
	FVFT_XYZRHWTEX1 *fvf4;
	FVFT_XYZDIFFTEX1 *fvf5;
	FVFT_XYZNORMTEX1 *fvf6;
	FVFT_XYZNORMDIFFTEX1 *fvf8;

	idx = 0;

	switch(fvf)
	{
		case FVF_XYZTEX1:
			fvf3 = (FVFT_XYZTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				fvf3[i].tu = uvin[idx*2];
				fvf3[i].tv = uvin[idx*2+1];
				idx++;
			}
			break;
		case FVF_XYZRHWTEX1:
			fvf4 = (FVFT_XYZRHWTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				fvf4[i].tu = uvin[idx*2];
				fvf4[i].tv = uvin[idx*2+1];
				idx++;
			}
			break;
		case FVF_XYZDIFFTEX1:
			fvf5 = (FVFT_XYZDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				fvf5[i].tu = uvin[idx*2];
				fvf5[i].tv = uvin[idx*2+1];
				idx++;
			}
			break;
		case FVF_XYZNORMTEX1:
			fvf6 = (FVFT_XYZNORMTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				fvf6[i].tu = uvin[idx*2];
				fvf6[i].tv = uvin[idx*2+1];
				idx++;
			}
			break;
		case FVF_XYZNORMDIFFTEX1:
			fvf8 = (FVFT_XYZNORMDIFFTEX1 *)verts;
			for(i=first; i<first+cnt; i++)
			{
				fvf8[i].tu = uvin[idx*2];
				fvf8[i].tv = uvin[idx*2+1];
				idx++;
			}
			break;
	}
}


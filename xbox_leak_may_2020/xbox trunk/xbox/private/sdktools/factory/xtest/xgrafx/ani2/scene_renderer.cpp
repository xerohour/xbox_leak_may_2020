//
//	scene_renderer.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#include "precomp.h"
#include "xbs_app.h"
#include "scene_renderer.h"
#include "scene_geometry.h"
#include "renderer.h"
#include "tex_gen.h"
	
const float LO_Z_VAL =   (00.f);
const float CEIL_Z =	   (40.f);
const float FLOOR_Z =    (-30.f);
#define SB_WIDTH   (512)
#define SB_HEIGHT  (512)


DWORD FtoDW(float f)
{
	return *((DWORD *)(&f));
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::Init()
{
	nInstances = 0;
	nVersions  = 0;

	aInstRecs = NULL;
	aVersRecs = NULL;
	
	pIB = NULL;
	pBaseStream = NULL;
	pExtraStream = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::UnInit()
{
	if( pBaseStream )
	{
		pBaseStream->Release();
		pBaseStream = NULL;
	}

	if( pExtraStream )
	{
		pExtraStream->Release();
		pExtraStream = NULL;
	}

	if( pIB )
	{
		pIB->Release();
		pIB = NULL;
	}

	if( aInstRecs )
	{
		MemFree(aInstRecs);
		aInstRecs = NULL;
	}

	if( aVersRecs )
	{
		MemFree(aVersRecs);
		aVersRecs = NULL;
	}

	nInstances = 0;
	nVersions  = 0;
}

///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::createSphereVersion( const SphereVers  *psphere, 
									    BYTE *pbase, BYTE *pextra,
										unsigned short *pi,
										int idx,int ndet_bias )
{
	int nSegs   = (int) psphere->nSegs>>ndet_bias;
	int nSlices = (int) nSegs/2;

	float fDeltaTheta = (2.f*Pi)/((float)nSegs);
	float fDeltaPhi   = Pi/((float)nSlices-1);

	int i;

	BaseStream *pbase_stream = (BaseStream *)pbase;

	for(i = 0; i < nSlices; i++)
	{
		float fPhi = fDeltaPhi * ((float)i);

		float fSinPhi,fCosPhi;
		SinCos(fPhi,&fSinPhi,&fCosPhi);
		
		float fV = fPhi/Pi;

		for(int j = 0; j < nSegs; j++)
		{	
			float fTheta = fDeltaTheta * ((float)j);

			float fSinTheta,fCosTheta;
			SinCos(fTheta,&fSinTheta,&fCosTheta);

			Set(&pbase_stream->p,fCosPhi,fCosTheta * fSinPhi,fSinTheta * fSinPhi);
			
			switch(shaderType)
			{
			case st_Phong:
				{
					PhongStream *pps = (PhongStream *)pextra;
					Set( &pps->s, 0.f, -fSinTheta, fCosTheta );
					pps->n = pbase_stream->p;
					Cross( pps->s, pps->n, &pps->t );

					pextra += sizeof(PhongStream);
				}
				break;

			case st_Bump:
				{
					BumpStream *pbs = (BumpStream *)pextra;
					
					Set( &pbs->s, 0.f, -fSinTheta, fCosTheta );
					pbs->n = pbase_stream->p;
					Cross( pbs->s, pbs->n, &pbs->t );
					
					pbs->u = fTheta / (2.f*Pi); 
					pbs->v = fV;

					pextra += sizeof(BumpStream);
				}
				break;
			}

			++pbase_stream; 
		}
	}

	WORD wCurVert = (WORD)aVersRecs[idx].dwVertexStart;

	for(i = 0; i < nSlices - 1; i++)
	{
		WORD wStartVert = wCurVert;

		for(int j = 0; j < nSegs + 1; j++)
		{
			*pi++ = wCurVert + nSegs;
			*pi++ = wCurVert;

			if(j < nSegs - 1)
				++wCurVert;
			else
				wCurVert = wStartVert;
		}

		wCurVert += (WORD)nSegs;
	}
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::createCylinderVersion( const CylinderVers  * pcyl,
										  BYTE *pbase, BYTE *pextra,
										  unsigned short *pi, 
										  int idx,int ndet_bias)
{
	int nsides = pcyl->nSides>>ndet_bias;

	float fDeltaZ = 1.f/((float)pcyl->nHeightSeg);
	float fDeltaTheta = (2.f * Pi)/((float)nsides);

	D3DVECTOR *ppts = (D3DVECTOR*)MemAlloc(sizeof(D3DVECTOR)*(nsides+1));

	BaseStream *pbase_stream = (BaseStream *)pbase;
	
	int i;
	for(i = 0; i < nsides+1; i++)
	{
		float fTheta = fDeltaTheta * ((float)i);
		float fSinTheta,fCosTheta;

		SinCos(fTheta,&fSinTheta,&fCosTheta);
		
		Set( &ppts[i],fCosTheta,fSinTheta,fTheta/(2.f*Pi));
	}

	for(i = nsides-1; i >= 0; i--)
	{
		Set(&pbase_stream->p,ppts[i].x,ppts[i].y,1.f);
		
		switch(shaderType)
		{
		case st_Phong:
			{
				PhongStream *pps = (PhongStream *)pextra;
				
				Set(&pps->s,1.f,0.f,0.f);
				Set(&pps->t,0.f,1.f,0.f);
				Set(&pps->n,0.f,0.f,1.f);

				pextra += sizeof(PhongStream);
			}
			break;

		case st_Bump:
			{
				BumpStream *pbs = (BumpStream *)pextra;
				pbs->u = ppts[i].x + 0.5f;
				pbs->v = ppts[i].y + 0.5f;

				Set(&pbs->s,1.f,0.f,0.f);
				Set(&pbs->t,0.f,1.f,0.f);
				Set(&pbs->n,0.f,0.f,1.f);

				pextra += sizeof(BumpStream);
			}
			break;
		}

		++pbase_stream;
	}

	for(i = 0; i < nsides; i++)
	{
		Set(&pbase_stream->p,ppts[i].x,ppts[i].y,0.f);
		
		switch(shaderType)
		{
		case st_Phong:
			{
				PhongStream *pps = (PhongStream *)pextra;
				
				Set(&pps->s,1.f,0.f,0.f);
				Set(&pps->t,0.f,-1.f,0.f);
				Set(&pps->n,0.f,0.f,-1.f);

				pextra += sizeof(PhongStream);
			}
			break;

		case st_Bump:
			{
				BumpStream *pbs = (BumpStream *)pextra;
				pbs->u = ppts[i].x + 0.5f;
				pbs->v = ppts[i].y + 0.5f;

				Set(&pbs->s,1.f,0.f,0.f);
				Set(&pbs->t,0.f,-1.f,0.f);
				Set(&pbs->n,0.f,0.f,-1.f);

				pextra += sizeof(BumpStream);
			}
			break;
		}

		++pbase_stream;
	}

	for(i = 0; i < pcyl->nHeightSeg+1; i++)
	{
		float fZ = fDeltaZ * ((float)i);

		for(unsigned short j = 0; j < nsides+1; j++)
		{
			Set(&pbase_stream->p,ppts[j].x,ppts[j].y,fZ);
			
			switch(shaderType)
			{
			case st_Phong:
				{
					PhongStream *pps = (PhongStream *)pextra;
					
					Set(&pps->s,-ppts[j].y,ppts[j].x,0.f);
					Set(&pps->t,0.f,0.f,1.f);
					Set(&pps->n,ppts[j].x,ppts[j].y,0.f);

					pextra += sizeof(PhongStream);
				}
				break;

			case st_Bump:
				{
					BumpStream *pbs = (BumpStream *)pextra;

					pbs->u = 4.0f * ppts[j].z;
					pbs->v = fZ * 32.0f;
		
					Set(&pbs->s,-ppts[j].y,ppts[j].x,0.f);
					Set(&pbs->t,0.f,0.f,1.f);
					Set(&pbs->n,ppts[j].x,ppts[j].y,0.f);

					pextra += sizeof(BumpStream);
				}
				break;
			}

			++pbase_stream;
		}
	}

	MemFree(ppts);

	
	WORD wStartCap = (WORD)aVersRecs[idx].dwVertexStart;
	for(i = 0; i < nsides; i++)
	{
		*pi++ = wStartCap++;
	}

	for(i = 0; i < nsides; i++)
	{
		*pi++ = wStartCap++;
	}

	CreateTristripForMesh(pi, nsides, pcyl->nHeightSeg, false, false, wStartCap);
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::createConeVersion( const ConeVers * pcone,
									  BYTE *pbase,BYTE *pextra,
									  unsigned short *pi, 
									  int idx,int ndet_bias )
{
	int nsides = pcone->nSides>>ndet_bias;

	float fDeltaTheta = (2.f * Pi)/((float)nsides);

	D3DVECTOR *ppts_bot = (D3DVECTOR*)MemAlloc(sizeof(D3DVECTOR)*(nsides+1));
	D3DVECTOR *ppts_top = (D3DVECTOR*)MemAlloc(sizeof(D3DVECTOR)*(nsides+1));

	BaseStream *pbase_stream = (BaseStream *)pbase;

	int i;
	for(i = 0; i < nsides+1; i++)
	{
		float fTheta = fDeltaTheta * ((float)i);
		float fSinTheta,fCosTheta;

		SinCos(fTheta,&fSinTheta,&fCosTheta);
		
		Set(&ppts_bot[i],fCosTheta * pcone->fRad1,fSinTheta * pcone->fRad1,fTheta/(2.f*Pi));
		Set(&ppts_top[i],fCosTheta * pcone->fRad2,fSinTheta * pcone->fRad2,fTheta/(2.f*Pi));
	}

	float fDeltaZ = 1.f/((float)(pcone->nHeightSeg));

	for(i = nsides-1; i >= 0; i--)
	{
		Set(&pbase_stream->p,ppts_top[i].x,ppts_top[i].y,pcone->fHeight);
		
		switch(shaderType)
		{
		case st_Phong:
			{
				PhongStream *pps = (PhongStream *)pextra;
				
				Set(&pps->s,1.f,0.f,0.f);
				Set(&pps->t,0.f,1.f,0.f);
				Set(&pps->n,0.f,0.f,1.f);

				pextra += sizeof(PhongStream);
			}
			break;

		case st_Bump:
			{
				BumpStream *pbs = (BumpStream *)pextra;
				pbs->u = ppts_top[i].x + 0.5f;
				pbs->v = ppts_top[i].y + 0.5f;

				Set(&pbs->s,1.f,0.f,0.f);
				Set(&pbs->t,0.f,1.f,0.f);
				Set(&pbs->n,0.f,0.f,1.f);

				pextra += sizeof(BumpStream);
			}
			break;
		}

		++pbase_stream;
	}

	for(i = 0; i < nsides; i++)
	{
		Set(&pbase_stream->p,ppts_bot[i].x,ppts_bot[i].y,0.f);
		
		switch(shaderType)
		{
		case st_Phong:
			{
				PhongStream *pps = (PhongStream *)pextra;
				
				Set(&pps->s,1.f,0.f,0.f);
				Set(&pps->t,0.f,-1.f,0.f);
				Set(&pps->n,0.f,0.f,-1.f);

				pextra += sizeof(PhongStream);
			}
			break;

		case st_Bump:
			{
				BumpStream *pbs = (BumpStream *)pextra;
				pbs->u = ppts_bot[i].x + 0.5f;
				pbs->v = ppts_bot[i].y + 0.5f;

				Set(&pbs->s,1.f,0.f,0.f);
				Set(&pbs->t,0.f,-1.f,0.f);
				Set(&pbs->n,0.f,0.f,-1.f);

				pextra += sizeof(BumpStream);
			}
			break;
		}

		++pbase_stream;
	}

	for(i = 0; i < pcone->nHeightSeg + 1; i++)
	{
		float fZ = fDeltaZ * ((float)i);

		for(unsigned short j = 0; j < nsides+1; j++)
		{	
			Set( &pbase_stream->p,
				 ppts_bot[j].x + fZ * ( ppts_top[j].x - ppts_bot[j].x ),
				 ppts_bot[j].y + fZ * ( ppts_top[j].y - ppts_bot[j].y ),
				 fZ * pcone->fHeight );


			switch(shaderType)
			{
			case st_Phong:
				{
					PhongStream *pps = (PhongStream *)pextra;
					
					Set(&pps->s, -ppts_top[j].y, ppts_top[j].x, 0.f );
					Normalize(&pps->s);
					Set(&pps->t,ppts_top[j].x - ppts_bot[j].x,ppts_top[j].y - ppts_bot[j].y,pcone->fHeight);
					Normalize(&pps->t);
					Cross(pps->s,pps->t,&pps->n);

					pextra += sizeof(PhongStream);
				}
				break;

			case st_Bump:
				{
					BumpStream *pbs = (BumpStream *)pextra;

					Set(&pbs->s, -ppts_top[j].y, ppts_top[j].x, 0.f );
					Normalize(&pbs->s);
					Set(&pbs->t,ppts_top[j].x - ppts_bot[j].x,ppts_top[j].y - ppts_bot[j].y,pcone->fHeight);
					Normalize(&pbs->t);
					Cross(pbs->s,pbs->t,&pbs->n);

					pbs->u = ppts_top[j].z;
					pbs->v = fZ;

					pextra += sizeof(BumpStream);
				}
				break;
			}
			
			++pbase_stream;
		}
	}

	MemFree(ppts_bot);
	MemFree(ppts_top);
	

	WORD wStartCap = (WORD)aVersRecs[idx].dwVertexStart;
	for(i = 0; i < nsides; i++)
	{
		*pi++ = wStartCap++;
	}

	for(i = 0; i < nsides; i++)
	{
		*pi++ = wStartCap++;
	}	

	CreateTristripForMesh(pi, nsides, pcone->nHeightSeg, false, false, wStartCap);
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::createBoxVersion( BYTE *pbase, BYTE *pextra,
									 unsigned short *pi,int idx)
{
	float fUV[4][2] = 
	{ 
		{ 0.f,1.f }, 
		{ 1.f,1.f }, 
		{ 1.f,0.f },
		{ 0.f,0.f } 
	};

	BaseStream *pbase_stream = (BaseStream *)pbase;

	for(int i = 0; i < 6; i++)
	{
		D3DVECTOR vS,vT,vN;
		D3DVECTOR vPos[4];

		switch(i)
		{
			case 0: 
			{
				Set(&vS,0.f,1.f,0.f);
				Set(&vT,0.f,0.f,1.f);
				Set(&vN,1.f,0.f,0.f);
				
				Set(&vPos[0],0.5f,-0.5f, 0.5f);
				Set(&vPos[1],0.5f, 0.5f, 0.5f);
				Set(&vPos[2],0.5f, 0.5f,-0.5f);
				Set(&vPos[3],0.5f,-0.5f,-0.5f);
				break;
			}
			case 1: 
			{
				Set(&vS,-1.f,0.f,0.f);
				Set(&vT, 0.f,0.f,1.f);
				Set(&vN, 0.f,1.f,0.f);
			
				Set(&vPos[0], 0.5f,0.5f, 0.5f);
				Set(&vPos[1],-0.5f,0.5f, 0.5f);
				Set(&vPos[2],-0.5f,0.5f,-0.5f);
				Set(&vPos[3], 0.5f,0.5f,-0.5f);
				break;
			}

			case 2: 
			{
				Set(&vS,0.f,-1.f,0.f);
				Set(&vT,0.f, 0.f,1.f);
				Set(&vN,0.f,-1.f,0.f);

				Set(&vPos[0],-0.5f, 0.5f, 0.5f);
				Set(&vPos[1],-0.5f,-0.5f, 0.5f);
				Set(&vPos[2],-0.5f,-0.5f,-0.5f);
				Set(&vPos[3],-0.5f, 0.5f,-0.5f);
				break;
			}

			case 3: 
			{
				Set(&vS,1.f, 0.f,0.f);
				Set(&vT,0.f, 0.f,1.f);
				Set(&vN,0.f,-1.f,0.f);

				Set(&vPos[0],-0.5f,-0.5f, 0.5f);
				Set(&vPos[1], 0.5f,-0.5f, 0.5f);
				Set(&vPos[2], 0.5f,-0.5f,-0.5f);
				Set(&vPos[3],-0.5f,-0.5f,-0.5f);
				break;
			}

			case 4: 
			{
				Set(&vS,1.f,0.f,0.f);
				Set(&vT,0.f,1.f,0.f);
				Set(&vN,0.f,0.f,1.f);

				Set(&vPos[0],-0.5f, 0.5f,0.5f);
				Set(&vPos[1], 0.5f, 0.5f,0.5f);
				Set(&vPos[2], 0.5f,-0.5f,0.5f);
				Set(&vPos[3],-0.5f,-0.5f,0.5f);
				break;
			}

			case 5: 
			{
				Set(&vS,1.f, 0.f, 0.f);
				Set(&vT,0.f,-1.f, 0.f);
				Set(&vN,0.f, 0.f,-1.f);

				Set(&vPos[0],-0.5f,-0.5f,-0.5f);
				Set(&vPos[1], 0.5f,-0.5f,-0.5f);
				Set(&vPos[2], 0.5f, 0.5f,-0.5f);
				Set(&vPos[3],-0.5f, 0.5f,-0.5f);
				break;
			}
		}

		for(int j = 0; j < 4; j++)
		{
			pbase_stream->p = vPos[j];
			
			switch(shaderType)
			{
			case st_Phong:
				{
					PhongStream *pps = (PhongStream *)pextra;
					
					pps->s = vS;
					pps->t = vT;
					pps->n = vN;

					pextra += sizeof(PhongStream);
				}
				break;

			case st_Bump:
				{
					BumpStream *pbs = (BumpStream *)pextra;

					pbs->u = fUV[j][0];
					pbs->v = fUV[j][1];
					pbs->s = vS;
					pbs->t = vT;
					pbs->n = vN;

					pextra += sizeof(BumpStream);
				}
				break;
			}

			++pbase_stream;
		}

		WORD wVertStart = i * 4;

		*pi++ = wVertStart;
		*pi++ = wVertStart+1;
		*pi++ = wVertStart+2;
		*pi++ = wVertStart;
		*pi++ = wVertStart+2;
		*pi++ = wVertStart+3;
	}
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::createTorusVersion( const TorusVers  * ptorus,
									   BYTE *pbase, BYTE *pextra,
									   unsigned short *pi, 
									   int idx,int ndet_bias)
{
	int nsegs = ptorus->nSegs>>ndet_bias;
	int nsides = ptorus->nSides>>ndet_bias;

	float fDeltaTheta = (2.f*Pi)/((float)nsegs);
	float fDeltaPhi   = (2.f*Pi)/((float)nsides);

	BaseStream *pbase_stream = (BaseStream *)pbase;

	int i;
	for(i = 0; i < nsides; i++)
	{
		float fPhi = fDeltaPhi * ((float)i);
		float fSinPhi,fCosPhi;

		SinCos(fPhi,&fSinPhi,&fCosPhi);

		float fRad = 1.f + (fCosPhi * ptorus->fRatio);
		float fV   = fPhi/(2.f * Pi);
		float fZ   = fSinPhi * ptorus->fRatio; 
		
		for(unsigned short j = 0; j < nsegs; j++)
		{
			float fTheta = fDeltaTheta * ((float)j);
			float fSinTheta,fCosTheta;

			SinCos(fTheta,&fSinTheta,&fCosTheta);

			Set(&pbase_stream->p,fCosTheta * fRad,fSinTheta * fRad,fZ);

			switch(shaderType)
			{
			case st_Phong:
				{
					PhongStream *pps = (PhongStream *)pextra;
					
					Set(&pps->s,-fSinTheta,fCosTheta,0.f);
					Set(&pps->t, fCosTheta * -fSinPhi,fSinTheta * -fSinPhi,fCosPhi);

					Cross(pps->s,pps->t,&pps->n);

					pextra += sizeof(PhongStream);
				}
				break;

			case st_Bump:
				{
					BumpStream *pbs = (BumpStream *)pextra;

					pbs->u = fTheta/(2.f*Pi);
					pbs->v = fV;

					Set(&pbs->s,-fSinTheta,fCosTheta,0.f);
					Set(&pbs->t, fCosTheta * -fSinPhi,fSinTheta * -fSinPhi,fCosPhi);

					Cross(pbs->s,pbs->t,&pbs->n);

					pextra += sizeof(BumpStream);
				}
				break;
			}

			++pbase_stream;
		}
	}

	WORD wVersionStart = (WORD)aVersRecs[idx].dwVertexStart;
	WORD wLoVert = wVersionStart;
	WORD wHiVert = wVersionStart + nsegs;

	for(i = 0; i < nsides; i++)
	{
		WORD wStripStartLo = wLoVert;
		WORD wStripStartHi = wHiVert;
		
		for(unsigned short j = 0; j < nsegs + 1; j++)
		{
			*pi++ = wLoVert;
			*pi++ = wHiVert;
			
			if(j < nsegs - 1)
			{
				++wLoVert;
				++wHiVert;
			}
			else
			{
				wLoVert = wStripStartLo;
				wHiVert = wStripStartHi;
			}
		}

		wLoVert += (WORD)nsegs;

		if ( (i+1) < (nsides - 1) )
		{
			wHiVert += (WORD)nsegs;
		}
		else
		{
			wHiVert = wVersionStart;	
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::createSurfOfRevVersion( const SurfOfRevVers  * psurf,
										   BYTE *pbase, BYTE *pextra, 
										   unsigned short *pi, 
										   int idx,int ndet_bias )
{		
	DWORD dwPolyPts = psurf->nPts;
	for (int i = 0; i < psurf->nPts; i++)
	{
		if (!(psurf->pts[i].flags & sr_Smooth))
			++dwPolyPts;
	}

	D3DVECTOR *pSegNorms = (D3DVECTOR *) _alloca(dwPolyPts * sizeof(D3DVECTOR));
	D3DVECTOR *pVerts    = (D3DVECTOR *) _alloca(dwPolyPts * sizeof(D3DVECTOR));

	enum NormalCalcFlags
	{
		nf_PrevSeg,	
		nf_NextSeg, 
		nf_BothSeg  
	};

	DWORD * dwVertFlags = (DWORD *)_alloca(dwPolyPts * sizeof(DWORD));

	D3DVECTOR vAxis;
	D3DVECTOR ptOnAxis;

	Set(&vAxis,psurf->ax,psurf->ay,psurf->az);
	Set(&ptOnAxis,psurf->px,psurf->py,psurf->pz);

	int ntot = 0;
	for (i = 0; i < psurf->nPts; i++)
	{
		Set(&pVerts[ntot],psurf->pts[i].x,psurf->pts[i].y,psurf->pts[i].z); 

		if (!(psurf->pts[i].flags & sr_Smooth))
		{
			dwVertFlags[ntot] = nf_PrevSeg;
			Set(&pSegNorms[ntot],0.f,0.f,0.f);
			ntot++;
			
			Set(&pVerts[ntot],psurf->pts[i].x,psurf->pts[i].y,psurf->pts[i].z); 
			dwVertFlags[ntot] = nf_NextSeg;
		}
		else 
		{
			dwVertFlags[ntot] = nf_BothSeg;
		}

		DWORD dwNextVert = i + 1;
		if(dwNextVert == psurf->nPts)
			dwNextVert = 0;

		D3DVECTOR seg;
		Set(&seg, psurf->pts[dwNextVert].x - psurf->pts[i].x,
			      psurf->pts[dwNextVert].y - psurf->pts[i].y,
				  psurf->pts[dwNextVert].z - psurf->pts[i].z );

		D3DVECTOR axisToPt;
		Set(&axisToPt,psurf->pts[dwNextVert].x - ptOnAxis.x,
					  psurf->pts[dwNextVert].y - ptOnAxis.y,
					  psurf->pts[dwNextVert].z - ptOnAxis.z );

		D3DVECTOR tang;
		Cross( vAxis,axisToPt,&tang );
		Cross( tang, seg, &pSegNorms[ntot] );
		Normalize(&pSegNorms[ntot]);

		ntot++;
	}

	D3DVECTOR *pVertNorms = (D3DVECTOR *) _alloca(dwPolyPts * sizeof(D3DVECTOR));
	for (i = 0; i < (int)dwPolyPts; i++)
	{
		switch(dwVertFlags[i])
		{
			case nf_PrevSeg:
				pVertNorms[i] = pSegNorms[(i + dwPolyPts - 1) % dwPolyPts];
				break;

			case nf_NextSeg:
				pVertNorms[i] = pSegNorms[i];
				break;

			case nf_BothSeg:
				Add( pSegNorms[(i + dwPolyPts - 1) % dwPolyPts], pSegNorms[i], &pVertNorms[i] );
				Normalize(&pVertNorms[i]);
				break;
		}
	}

	int nsegs = psurf->nSegs>>ndet_bias;

	float fDeltaTheta = (2.f * Pi)/((float)nsegs);
	float fDeltaV     = 1.f/((float)psurf->nPts-1);

	BaseStream *pbase_stream = (BaseStream *)pbase;

	for (i = 0; i < (nsegs+1); i++)
	{
		float fTheta = fDeltaTheta * ((float)i);
		
		D3DVECTOR4 quat;
		SetQuatFromAxis(vAxis,fTheta,&quat);

		D3DMATRIX rotMat;
		SetRotationFromRHQuat(quat,&rotMat);

		float fU = fTheta/(2.f * Pi);

		unsigned unique_vert_count = 0;
		for (unsigned int j = 0; j < dwPolyPts; j++)
		{
			float fV = fDeltaV * ((float)unique_vert_count);

			if(dwVertFlags[j] != nf_PrevSeg)
				++unique_vert_count;

			D3DVECTOR pt;
			Sub(pVerts[j],ptOnAxis,&pt);
			TransformVector(pt,rotMat,&pbase_stream->p);
			
			pbase_stream->p.x += ptOnAxis.x;
			pbase_stream->p.y += ptOnAxis.y;
			pbase_stream->p.z += ptOnAxis.z;

			switch(shaderType)
			{
			case st_Phong:
				{
					PhongStream *pps = (PhongStream *)pextra;
					
					D3DVECTOR tang;
					Cross(vAxis,pt,&tang);
					TransformVector(tang,rotMat,&pps->s);
					Normalize(&pps->s);

					TransformVector(pVertNorms[j],rotMat,&pps->n);
					Cross(pps->n,pps->s,&pps->t);

					pextra += sizeof(PhongStream);
				}
				break;

			case st_Bump:
				{
					BumpStream *pbs = (BumpStream *)pextra;

					D3DVECTOR tang;
					Cross(vAxis,pt,&tang);
					TransformVector(tang,rotMat,&pbs->s);
					Normalize(&pbs->s);

					TransformVector(pVertNorms[j],rotMat,&pbs->n);
					Cross(pbs->n,pbs->s,&pbs->t);

					pbs->u = fU * 10.f;
					pbs->v = fV * 10.f;

					pextra += sizeof(BumpStream);
				}
				break;
			}

			++pbase_stream;
		}
	}

	WORD wLeftVert  = (WORD)aVersRecs[idx].dwVertexStart;
	WORD wRightVert = (WORD)(wLeftVert + dwPolyPts);

	for (i = 0; i < nsegs; i++)
	{
		WORD wStartStripRight = wRightVert;
		WORD wStartStripLeft  = wLeftVert;

		for (unsigned int j = 0; j < dwPolyPts + 1; j++)
		{
			*pi++ = wRightVert;
			*pi++ = wLeftVert;

			if( j < dwPolyPts - 1 )
			{
				++wRightVert;
				++wLeftVert;
			}
			else
			{
				wRightVert = wStartStripRight;
				wLeftVert  = wStartStripLeft;
			}
		}

		wLeftVert = wRightVert;
		wRightVert += (WORD)dwPolyPts;
	}
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initSphereInstance(const SphereInst *psphere, int idx)
{
	SetScale( psphere->fRad, psphere->fRad, psphere->fRad, &aInstRecs[idx].matScale );
	Set( &aInstRecs[idx].vObjScale,psphere->fRad,psphere->fRad,psphere->fRad,1.f );
	
	SetIdentity( &aInstRecs[idx].matOffset );
	aInstRecs[idx].matOffset._41 = ((float)psphere->tx) * OO_PRIM_TRANS_SCALE_X + PRIM_TRANS_DELTA_X;
	aInstRecs[idx].matOffset._42 = ((float)psphere->ty) * OO_PRIM_TRANS_SCALE_Y + PRIM_TRANS_DELTA_Y;
	aInstRecs[idx].matOffset._43 = ((float)psphere->tz) * OO_PRIM_TRANS_SCALE_Z + PRIM_TRANS_DELTA_Z;

	SetIdentity(&aInstRecs[idx].matInvOffset);
	aInstRecs[idx].matInvOffset._41 = -(((float)psphere->tx) * OO_PRIM_TRANS_SCALE_X + PRIM_TRANS_DELTA_X);
	aInstRecs[idx].matInvOffset._42 = -(((float)psphere->ty) * OO_PRIM_TRANS_SCALE_Y + PRIM_TRANS_DELTA_Y);
	aInstRecs[idx].matInvOffset._43 = -(((float)psphere->tz) * OO_PRIM_TRANS_SCALE_Z + PRIM_TRANS_DELTA_Z);

	MulMats(aInstRecs[idx].matScale,aInstRecs[idx].matOffset,&aInstRecs[idx].matScaleOffset);

	aInstRecs[idx].idxVersion = psphere->idVersion;

	aInstRecs[idx].idxPosAnim = psphere->idPosAnim;
	aInstRecs[idx].idxRotAnim = psphere->idRotAnim;

	SceneRenderer &scene = gApp.sceneGeom;

	D3DVECTOR v;
	Set(&v,aInstRecs[idx].matOffset._41,aInstRecs[idx].matOffset._42,aInstRecs[idx].matOffset._43);

	if(aInstRecs[idx].idxRotAnim >= 0)
	{
		D3DVECTOR tmp = v;
		D3DVECTOR4 &q = scene.pQuats[scene.pQuatIdSeq[aInstRecs[idx].idxRotAnim][0]];
		
		D3DMATRIX mat;
		SetRotationFromLHQuat(q,&mat);

		TransformVector(tmp,mat,&v);
	}
	if(aInstRecs[idx].idxPosAnim >= 0)
	{
		v.x += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].x;
		v.y += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].y;
		v.z += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].z;
	}

	aInstRecs[idx].bHiZ = v.z > LO_Z_VAL;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initCylinderInstance( const CylinderInst *pcyl, int idx )
{
	SetScale(pcyl->fRad,pcyl->fRad,pcyl->fHalfHeight*2.f,&aInstRecs[idx].matScale);
	Set(&aInstRecs[idx].vObjScale,pcyl->fRad,pcyl->fRad,pcyl->fHalfHeight*2.f,1.f);


	SetRotationFromLHQuat(gApp.sceneGeom.pQuats[pcyl->idQuat],&aInstRecs[idx].matOffset);
	aInstRecs[idx].matOffset._41 = ((float)pcyl->tx) * OO_PRIM_TRANS_SCALE_X + PRIM_TRANS_DELTA_X;
	aInstRecs[idx].matOffset._42 = ((float)pcyl->ty) * OO_PRIM_TRANS_SCALE_Y + PRIM_TRANS_DELTA_Y;
	aInstRecs[idx].matOffset._43 = ((float)pcyl->tz) * OO_PRIM_TRANS_SCALE_Z + PRIM_TRANS_DELTA_Z;
	SetInverse(aInstRecs[idx].matOffset,&aInstRecs[idx].matInvOffset);

	MulMats(aInstRecs[idx].matScale,aInstRecs[idx].matOffset,&aInstRecs[idx].matScaleOffset);

	aInstRecs[idx].idxVersion = pcyl->idVersion;

	aInstRecs[idx].idxPosAnim = pcyl->idPosAnim;
	aInstRecs[idx].idxRotAnim = pcyl->idRotAnim;

	SceneRenderer &scene = gApp.sceneGeom;

	D3DVECTOR v;
	Set(&v,aInstRecs[idx].matOffset._41,aInstRecs[idx].matOffset._42,aInstRecs[idx].matOffset._43);

	if(aInstRecs[idx].idxRotAnim >= 0)
	{
		D3DVECTOR tmp = v;
		D3DVECTOR4 &q = scene.pQuats[scene.pQuatIdSeq[aInstRecs[idx].idxRotAnim][0]];
		
		D3DMATRIX mat;
		SetRotationFromLHQuat(q,&mat);

		TransformVector(tmp,mat,&v);
	}
	if(aInstRecs[idx].idxPosAnim >= 0)
	{
		v.x += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].x;
		v.y += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].y;
		v.z += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].z;
	}

	aInstRecs[idx].bHiZ = v.z > LO_Z_VAL;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initConeInstance( const ConeInst *pcone, int idx )
{
	SetScale(1.f,1.f,1.f,&aInstRecs[idx].matScale);
	Set(&aInstRecs[idx].vObjScale,1.f,1.f,1.f,1.f);

	SetRotationFromLHQuat(gApp.sceneGeom.pQuats[pcone->idQuat],&aInstRecs[idx].matOffset);
	aInstRecs[idx].matOffset._41 = ((float)pcone->tx) * OO_PRIM_TRANS_SCALE_X + PRIM_TRANS_DELTA_X;
	aInstRecs[idx].matOffset._42 = ((float)pcone->ty) * OO_PRIM_TRANS_SCALE_Y + PRIM_TRANS_DELTA_Y;
	aInstRecs[idx].matOffset._43 = ((float)pcone->tz) * OO_PRIM_TRANS_SCALE_Z + PRIM_TRANS_DELTA_Z;
	SetInverse(aInstRecs[idx].matOffset,&aInstRecs[idx].matInvOffset);
	
	aInstRecs[idx].matScaleOffset = aInstRecs[idx].matOffset;

	aInstRecs[idx].idxVersion = pcone->idVersion;

	aInstRecs[idx].idxPosAnim = pcone->idPosAnim;
	aInstRecs[idx].idxRotAnim = pcone->idRotAnim;


	SceneRenderer &scene = gApp.sceneGeom;

	D3DVECTOR v;
	Set(&v,aInstRecs[idx].matOffset._41,aInstRecs[idx].matOffset._42,aInstRecs[idx].matOffset._43);

	if(aInstRecs[idx].idxRotAnim >= 0)
	{
		D3DVECTOR tmp = v;
		D3DVECTOR4 &q = scene.pQuats[scene.pQuatIdSeq[aInstRecs[idx].idxRotAnim][0]];
		
		D3DMATRIX mat;
		SetRotationFromLHQuat(q,&mat);

		TransformVector(tmp,mat,&v);
	}
	if(aInstRecs[idx].idxPosAnim >= 0)
	{
		v.x += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].x;
		v.y += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].y;
		v.z += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].z;
	}

	aInstRecs[idx].bHiZ = v.z > LO_Z_VAL;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initBoxInstance( const BoxInst *pbox, int idx )
{
	SetScale(pbox->fWidth,pbox->fLen,pbox->fHeight,&aInstRecs[idx].matScale);
	Set(&aInstRecs[idx].vObjScale,pbox->fWidth,pbox->fLen,pbox->fHeight,1.f);	

	SetRotationFromLHQuat(gApp.sceneGeom.pQuats[pbox->idQuat],&aInstRecs[idx].matOffset);
	aInstRecs[idx].matOffset._41 = ((float)pbox->tx) * OO_PRIM_TRANS_SCALE_X + PRIM_TRANS_DELTA_X;
	aInstRecs[idx].matOffset._42 = ((float)pbox->ty) * OO_PRIM_TRANS_SCALE_Y + PRIM_TRANS_DELTA_Y;
	aInstRecs[idx].matOffset._43 = ((float)pbox->tz) * OO_PRIM_TRANS_SCALE_Z + PRIM_TRANS_DELTA_Z;	
	SetInverse(aInstRecs[idx].matOffset,&aInstRecs[idx].matInvOffset);
	
	MulMats(aInstRecs[idx].matScale,aInstRecs[idx].matOffset,&aInstRecs[idx].matScaleOffset);

	aInstRecs[idx].idxVersion = 0;

	aInstRecs[idx].idxPosAnim = pbox->idPosAnim;
	aInstRecs[idx].idxRotAnim = pbox->idRotAnim;

	SceneRenderer &scene = gApp.sceneGeom;

	D3DVECTOR v;
	Set(&v,aInstRecs[idx].matOffset._41,aInstRecs[idx].matOffset._42,aInstRecs[idx].matOffset._43);

	if(aInstRecs[idx].idxRotAnim >= 0)
	{
		D3DVECTOR tmp = v;
		D3DVECTOR4 &q = scene.pQuats[scene.pQuatIdSeq[aInstRecs[idx].idxRotAnim][0]];
		
		D3DMATRIX mat;
		SetRotationFromLHQuat(q,&mat);

		TransformVector(tmp,mat,&v);
	}
	if(aInstRecs[idx].idxPosAnim >= 0)
	{
		v.x += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].x;
		v.y += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].y;
		v.z += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].z;
	}

	aInstRecs[idx].bHiZ = v.z > LO_Z_VAL;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initTorusInstance( const TorusInst *ptorus, int idx )
{	
	SetScale( ptorus->fRad1, ptorus->fRad1, ptorus->fRad1, &aInstRecs[idx].matScale );
	Set(&aInstRecs[idx].vObjScale,ptorus->fRad1,ptorus->fRad1,ptorus->fRad1,1.f);

	SetRotationFromLHQuat(gApp.sceneGeom.pQuats[ptorus->idQuat],&aInstRecs[idx].matOffset);		
	aInstRecs[idx].matOffset._41 = ((float)ptorus->tx) * OO_PRIM_TRANS_SCALE_X + PRIM_TRANS_DELTA_X;
	aInstRecs[idx].matOffset._42 = ((float)ptorus->ty) * OO_PRIM_TRANS_SCALE_Y + PRIM_TRANS_DELTA_Y;
	aInstRecs[idx].matOffset._43 = ((float)ptorus->tz) * OO_PRIM_TRANS_SCALE_Z + PRIM_TRANS_DELTA_Z;
	SetInverse(aInstRecs[idx].matOffset,&aInstRecs[idx].matInvOffset);

	MulMats(aInstRecs[idx].matScale,aInstRecs[idx].matOffset,&aInstRecs[idx].matScaleOffset);

	aInstRecs[idx].idxVersion = ptorus->idVersion;

	aInstRecs[idx].idxPosAnim = ptorus->idPosAnim;
	aInstRecs[idx].idxRotAnim = ptorus->idRotAnim;

	SceneRenderer &scene = gApp.sceneGeom;

	D3DVECTOR v;
	Set(&v,aInstRecs[idx].matOffset._41,aInstRecs[idx].matOffset._42,aInstRecs[idx].matOffset._43);

	if(aInstRecs[idx].idxRotAnim >= 0)
	{
		D3DVECTOR tmp = v;
		D3DVECTOR4 &q = scene.pQuats[scene.pQuatIdSeq[aInstRecs[idx].idxRotAnim][0]];
		
		D3DMATRIX mat;
		SetRotationFromLHQuat(q,&mat);

		TransformVector(tmp,mat,&v);
	}
	if(aInstRecs[idx].idxPosAnim >= 0)
	{
		v.x += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].x;
		v.y += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].y;
		v.z += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].z;
	}

	aInstRecs[idx].bHiZ = v.z > LO_Z_VAL;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initSurfOfRevInstance( const SurfOfRevInst *psurf, int idx )
{
	SetScale(1.f,1.f,1.f,&aInstRecs[idx].matScale);
	Set( &aInstRecs[idx].vObjScale, 1.f,1.f,1.f,1.f );

	SetRotationFromLHQuat(gApp.sceneGeom.pQuats[psurf->idQuat],&aInstRecs[idx].matOffset);
	aInstRecs[idx].matOffset._41 = ((float)psurf->tx) * OO_PRIM_TRANS_SCALE_X + PRIM_TRANS_DELTA_X;
	aInstRecs[idx].matOffset._42 = ((float)psurf->ty) * OO_PRIM_TRANS_SCALE_Y + PRIM_TRANS_DELTA_Y;
	aInstRecs[idx].matOffset._43 = ((float)psurf->tz) * OO_PRIM_TRANS_SCALE_Z + PRIM_TRANS_DELTA_Z;
	SetInverse(aInstRecs[idx].matOffset,&aInstRecs[idx].matInvOffset);
	
	aInstRecs[idx].matScaleOffset = aInstRecs[idx].matOffset;

	aInstRecs[idx].idxVersion = psurf->idVersion;

	aInstRecs[idx].idxPosAnim = psurf->idPosAnim;
	aInstRecs[idx].idxRotAnim = psurf->idRotAnim;

	SceneRenderer &scene = gApp.sceneGeom;

	D3DVECTOR v;
	Set(&v,aInstRecs[idx].matOffset._41,aInstRecs[idx].matOffset._42,aInstRecs[idx].matOffset._43);

	if(aInstRecs[idx].idxRotAnim >= 0)
	{
		D3DVECTOR tmp = v;
		D3DVECTOR4 &q = scene.pQuats[scene.pQuatIdSeq[aInstRecs[idx].idxRotAnim][0]];
		
		D3DMATRIX mat;
		SetRotationFromLHQuat(q,&mat);

		TransformVector(tmp,mat,&v);
	}
	if(aInstRecs[idx].idxPosAnim >= 0)
	{
		v.x += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].x;
		v.y += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].y;
		v.z += scene.pPos[scene.pPosIdSeq[aInstRecs[idx].idxPosAnim][0]].z;
	}

	aInstRecs[idx].bHiZ = v.z > LO_Z_VAL;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initSphereVersion(const SphereVers *psphere, int idx,int ndet_bias)
{
	DWORD dwSegs   = psphere->nSegs>>ndet_bias;
	DWORD dwSlices = dwSegs/2;

	aVersRecs[idx].dwVertexCount = dwSegs * dwSlices; 
	aVersRecs[idx].dwPrimCount   = (dwSlices-1) * (dwSegs+1) * 2 - 2;
	aVersRecs[idx].dwIndexCount  = aVersRecs[idx].dwPrimCount + 2;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initCylinderVersion(const CylinderVers *pcyl, int idx,int ndet_bias)
{
	int nsides = pcyl->nSides>>ndet_bias;

	aVersRecs[idx].dwVertexCount = (nsides+1) * (pcyl->nHeightSeg+1) + (nsides+1) * 2;
	aVersRecs[idx].dwPrimCount   = 0;
	aVersRecs[idx].dwIndexCount  = 2*nsides + GetNumberOfIndicesForTristripMesh(nsides, pcyl->nHeightSeg);
	aVersRecs[idx].dwParameter   = nsides;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initConeVersion(const ConeVers *pcone,int idx,int ndet_bias)
{
	int nsides = pcone->nSides>>ndet_bias;

	aVersRecs[idx].dwVertexCount = (nsides+1) * (pcone->nHeightSeg+1) + (nsides+1) * 2;
	aVersRecs[idx].dwPrimCount   = 0;
	aVersRecs[idx].dwIndexCount  = 2*nsides + GetNumberOfIndicesForTristripMesh(nsides, pcone->nHeightSeg);
	aVersRecs[idx].dwParameter   = nsides;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initBoxVersion( int idx )
{
	aVersRecs[idx].dwVertexCount = 24;
	aVersRecs[idx].dwPrimCount   = 12;
	aVersRecs[idx].dwIndexCount  = 36;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initTorusVersion(const TorusVers *ptorus,int idx,int ndet_bias)
{
	int nsides = ptorus->nSides>>ndet_bias;
	int nsegs = ptorus->nSegs>>ndet_bias;

	aVersRecs[idx].dwVertexCount = nsegs * nsides; 
	aVersRecs[idx].dwPrimCount   = (nsegs + 1) * nsides * 2 - 2;
	aVersRecs[idx].dwIndexCount  = aVersRecs[idx].dwPrimCount + 2;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::initSurfOfRevVersion(const SurfOfRevVers *psurf,int idx,int ndet_bias)
{
	DWORD dwDupVerts = 0;

	for (unsigned short i = 0; i < psurf->nPts; i++)
	{
		if (!(psurf->pts[i].flags & sr_Smooth))
			++dwDupVerts;
	}

	int nsegs = psurf->nSegs>>ndet_bias;

	aVersRecs[idx].dwVertexCount = (dwDupVerts + psurf->nPts) * (nsegs+1);
	aVersRecs[idx].dwPrimCount   = (psurf->nPts + dwDupVerts +1) * 2 * nsegs - 2;
	aVersRecs[idx].dwIndexCount  = aVersRecs[idx].dwPrimCount + 2;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::renderZ()
{
	ShaderTypes shader = shaderType;
	shaderType = st_Depth;
	
	render(gApp.blobLight,false);
	
	shaderType = shader;
}

///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::renderShadowMap(bool b_hi_z)
{
	ShaderTypes shader = shaderType;
	shaderType = st_ShadowMap;
	
	bHiZ = b_hi_z;
	
	render(gApp.blobLight,false);
	
	shaderType = shader;
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::render( const D3DLIGHT8 &light,bool b_query_blob )
{
	gpd3dDev->SetStreamSource( 0, pBaseStream, sizeof(BaseStream) );
	gpd3dDev->SetIndices( pIB, 0 );

	bool b_no_lighting = false;

	switch( shaderType )
	{
	case st_Bump:
		{
			gpd3dDev->SetTexture(0,pNormalMap);
			gpd3dDev->SetTexture(1,pCubeMap);
			gpd3dDev->SetTexture(2,pCubeMap);
			
			gpd3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);
			gpd3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);
			gpd3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
			gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
			gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
			gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSW,  D3DTADDRESS_WRAP );

			gpd3dDev->SetStreamSource( 1, pExtraStream, sizeof(BumpStream) );
			gpd3dDev->SetVertexShader(gApp.dwVShader[st_Bump]);
			gpd3dDev->SetPixelShader (gApp.dwPShader[st_Bump]);
		}
		break;

	case st_Phong:
		{
			gpd3dDev->SetTexture(1,pCubeMap);
			gpd3dDev->SetTexture(2,pCubeMap);
			gpd3dDev->SetStreamSource( 1, pExtraStream, sizeof(PhongStream) );
			gpd3dDev->SetVertexShader( gApp.dwVShader[st_Phong] );
			gpd3dDev->SetPixelShader ( gApp.dwPShader[st_Phong] );
		}
		break;

	case st_Depth:
		{
			b_no_lighting = true;
		}
		break;

	case st_ShadowMap:
		{
			b_no_lighting = true;
			gpd3dDev->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
			gpd3dDev->SetRenderState( D3DRS_ZENABLE,			TRUE );
		}
		break;
	}

	D3DVECTOR4 light_val[3];

	if( !b_query_blob  && !b_no_lighting)
	{
		Set(&light_val[0],vAmbient.x * light.Ambient.r,
						  vAmbient.y * light.Ambient.g,
						  vAmbient.z * light.Ambient.b, 1.f );

		Set(&light_val[1],vDiffuse.x * light.Diffuse.r,
						  vDiffuse.y * light.Diffuse.g,
						  vDiffuse.z * light.Diffuse.b, 1.f );

		Set(&light_val[2],vSpecular.x * light.Specular.r,
						  vSpecular.y * light.Specular.g,
						  vSpecular.z * light.Specular.b, 1.f );

		gpd3dDev->SetPixelShaderConstant ( 0,(CONST void *)&light_val, 3 );

		D3DVECTOR4 vAtten;
		Set(&vAtten,light.Attenuation0,light.Attenuation1,light.Attenuation2, 1.f );
		gpd3dDev->SetVertexShaderConstant( 7, (CONST void *)&vAtten,1 );	
	}

	const D3DMATRIX  &matWTP = gApp.theCamera.getWTP();

	SceneRenderer & scene = gApp.sceneGeom;

	for(int i = 0; i < nInstances; i++)
	{
		const PrimitiveInstanceRecord &inst = aInstRecs[i];

		D3DMATRIX world_mat;
		D3DMATRIX inv_world_mat;
		
		if (inst.idxPosAnim >= 0 || inst.idxRotAnim >= 0)
		{
			D3DMATRIX tmp;
			if(inst.idxRotAnim >= 0)
			{
			
				MulMats( inst.matOffset,scene.pRotAnims[inst.idxRotAnim], &tmp );
			}
			else
			{
				tmp = inst.matOffset;
			}

			if(inst.idxPosAnim >= 0)
			{
				tmp._41 += scene.pPosAnims[inst.idxPosAnim].x;
				tmp._42 += scene.pPosAnims[inst.idxPosAnim].y;
				tmp._43 += scene.pPosAnims[inst.idxPosAnim].z;
			}
			

			SetInverse(tmp,&inv_world_mat);
			MulMats(inst.matScale,tmp,&world_mat);
		}
		else
		{
			world_mat = inst.matScaleOffset;
			inv_world_mat = inst.matInvOffset;
		}

		if(shaderType == st_ShadowMap)
		{
			if(inst.bHiZ != bHiZ)
				continue;

			gpd3dDev->SetTransform(D3DTS_WORLD,&world_mat);
		}
		else
		{
			D3DMATRIX matTmp,matFinal;

			MulMats(world_mat,matWTP,&matTmp);
			SetTranspose(matTmp,&matFinal);

			gpd3dDev->SetVertexShaderConstant(0,&matFinal,4);
		}

		D3DVECTOR4 vObjEyePos;
		D3DVECTOR4 vObjLightPos;

		if( b_query_blob && !b_no_lighting )
		{
			D3DVECTOR4 tmp;

			float fIntensity;

			tmp.w = 1.0f;
			gApp.vblob.getLightForPosition(&tmp,&fIntensity,*((D3DVECTOR *) &world_mat._41));
			
			fIntensity = gApp.getBlobIntensity() * 2.f ;

			Set(&light_val[0],vAmbient.x * light.Ambient.r * fIntensity,
							  vAmbient.y * light.Ambient.g * fIntensity,
							  vAmbient.z * light.Ambient.b * fIntensity, 1.f );
		
			Set(&light_val[1],vDiffuse.x * light.Diffuse.r * fIntensity,
							  vDiffuse.y * light.Diffuse.g * fIntensity,
							  vDiffuse.z * light.Diffuse.b * fIntensity, 1.f );

  			Set(&light_val[2],vSpecular.x * light.Specular.r * fIntensity,
							  vSpecular.y * light.Specular.g * fIntensity,
							  vSpecular.z * light.Specular.b * fIntensity, 1.f );


			gpd3dDev->SetPixelShaderConstant ( 0,(CONST void *)&light_val, 3 );

			TransformPoint( tmp, inv_world_mat, &vObjLightPos);

			D3DVECTOR4 vAtten;

			float f_oo_intensity = 1.f/fIntensity;
			Set(&vAtten,light.Attenuation0,light.Attenuation1 * f_oo_intensity, 
										   light.Attenuation2 * f_oo_intensity, 1.f );
			gpd3dDev->SetVertexShaderConstant( 7, (CONST void *)&vAtten,1 );
		}
		else if(shaderType != st_ShadowMap && shaderType != st_Depth)
		{
			TransformPoint( light.Position, inv_world_mat, &vObjLightPos);	
		}
		
		if( shaderType != st_ShadowMap && shaderType != st_Depth )
		{
			vObjLightPos.w = 1.f;
			gpd3dDev->SetVertexShaderConstant(4,&vObjLightPos,1);

			TransformPoint(*((D3DVECTOR *)&gApp.theCamera.matCTW._41),inv_world_mat,&vObjEyePos);
			vObjEyePos.w = 1.f;
		
			gpd3dDev->SetVertexShaderConstant(5,&vObjEyePos,1);
			gpd3dDev->SetVertexShaderConstant(6,&inst.vObjScale,1);

			D3DMATRIX matOTS,finalSMat;
			MulMats(world_mat, (inst.bHiZ) ? scene.matWTSHi : scene.matWTSLo , &matOTS );
			SetTranspose(matOTS,&finalSMat);
			gpd3dDev->SetVertexShaderConstant(8,&finalSMat,4);

			gpd3dDev->SetTexture(3,(inst.bHiZ) ? scene.pSBufDepthHi: scene.pSBufDepthLo );
		}
		
		const PrimitiveVersionRecord &vs = aVersRecs[aInstRecs[i].idxVersion];

		if ((primitiveType==pt_Cylinder) || (primitiveType==pt_Cone))
		{
			int num_indices_in_cap = vs.dwParameter;
			if (num_indices_in_cap)
			{
				gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLEFAN,vs.dwVertexStart,vs.dwVertexCount,
												vs.dwIndexStart + 0, num_indices_in_cap-2 );
				gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLEFAN,vs.dwVertexStart,vs.dwVertexCount,
												vs.dwIndexStart + num_indices_in_cap, num_indices_in_cap-2 );
			}

			gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP,vs.dwVertexStart,vs.dwVertexCount,
											vs.dwIndexStart + 2*num_indices_in_cap,vs.dwIndexCount - 2*num_indices_in_cap - 2 );
		}
		else
		{
			gpd3dDev->DrawIndexedPrimitive( d3dType,vs.dwVertexStart,vs.dwVertexCount,
											vs.dwIndexStart,vs.dwPrimCount );
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void PrimitiveSet::create( const void *				version_data, 
						   int						num_versions, 
						   const void *				instance_data,
						   int						num_insts,
						   int						ndet_bias,
						   ShaderTypes				shader,
						   PrimitiveTypes			primitive,
						   LPDIRECT3DTEXTURE8		pnormal_map,
						   LPDIRECT3DCUBETEXTURE8	pcube_map)
{
	nVersions  = num_versions;
	nInstances = num_insts;

	shaderType = shader;
	primitiveType = primitive;
	pNormalMap = pnormal_map;
	pCubeMap   = pcube_map;

	aVersRecs = (PrimitiveVersionRecord*)MemAlloc(sizeof(PrimitiveVersionRecord)*nVersions);
	ZeroMemory(aVersRecs,sizeof(PrimitiveVersionRecord)*nVersions);

	DWORD dwVertCount  = 0;
	DWORD dwIndexCount = 0;

	unsigned short i;
	for(i = 0; i < nVersions; i++)
	{
		aVersRecs[i].dwVertexStart = dwVertCount;
		aVersRecs[i].dwIndexStart  = dwIndexCount;
		
		switch(primitiveType)
		{
		case pt_Sphere:		
			initSphereVersion( ((const SphereVers *)version_data)+i,i,ndet_bias);
			break;
		case pt_Cone:		
			initConeVersion( ((const ConeVers *)version_data)+i,i,ndet_bias);				
			break;
		case pt_Cylinder:	
			initCylinderVersion( ((const CylinderVers *)version_data)+i,i,ndet_bias);
			break;
		case pt_Box:		
			initBoxVersion(i);													
			break;
		case pt_Torus:		
			initTorusVersion( ((const TorusVers *)version_data)+i,i,ndet_bias);			
			break;
		case pt_SurfOfRev:	
			initSurfOfRevVersion( ((const SurfOfRevVers *)version_data)+i,i,ndet_bias);	
			break;
		}

		dwVertCount  += aVersRecs[i].dwVertexCount;
		dwIndexCount += aVersRecs[i].dwIndexCount;
	}

	DWORD dwExtraStride = 0;

	switch(shaderType)
	{
	case st_Bump:  dwExtraStride = sizeof(BumpStream);  break;
	case st_Phong: dwExtraStride = sizeof(PhongStream); break;
	}

	gpd3dDev->CreateVertexBuffer( sizeof(BaseStream) * dwVertCount, NULL, NULL, NULL, &pBaseStream );
	gpd3dDev->CreateVertexBuffer( dwExtraStride * dwVertCount, NULL, NULL, NULL, &pExtraStream );

	gpd3dDev->CreateIndexBuffer ( sizeof(unsigned short) * dwIndexCount,NULL,D3DFMT_INDEX16,NULL,&pIB);

	BYTE *pbase,*pextra;

	pBaseStream->Lock(0,sizeof(BaseStream) * dwVertCount,&pbase,NULL);
	pExtraStream->Lock(0,dwExtraStride * dwVertCount,&pextra,NULL);

	unsigned short *pi;
	pIB->Lock(0,sizeof(unsigned short) * dwIndexCount,(BYTE **)&pi,NULL);

	for(i = 0; i < nVersions; i++)
	{
		switch(primitiveType)
		{
		case pt_Sphere:		
			createSphereVersion( ((const SphereVers *)version_data)+i,pbase, pextra, pi, i,ndet_bias ); 
			break;		  
		case pt_Cone:		
			createConeVersion( ((const ConeVers *)version_data)+i,pbase,pextra,pi,i,ndet_bias );	
			break;
		case pt_Cylinder:	
			createCylinderVersion( ((const CylinderVers *)version_data)+i,pbase,pextra,pi,i,ndet_bias);  
			break;
		case pt_Box:		
			createBoxVersion(pbase,pextra,pi,i);
			break;
		case pt_Torus:		
			createTorusVersion( ((const TorusVers *)version_data)+i,pbase,pextra,pi,i,ndet_bias);
			break;
		case pt_SurfOfRev:	
			createSurfOfRevVersion( ((const SurfOfRevVers *)version_data)+i,pbase,pextra,pi,i,ndet_bias );
			break;
		}
		
		pbase  += (aVersRecs[i].dwVertexCount * sizeof(BaseStream));
		pextra += (aVersRecs[i].dwVertexCount * dwExtraStride );
		pi     += aVersRecs[i].dwIndexCount;
	}

	pBaseStream->Unlock();
	pExtraStream->Unlock();

	pIB->Unlock();

	aInstRecs  = (PrimitiveInstanceRecord*)MemAlloc(sizeof(PrimitiveInstanceRecord)*nInstances);
	ZeroMemory(aInstRecs,sizeof(PrimitiveInstanceRecord)*nInstances);

	for(i = 0; i < nInstances; i++)
	{
		switch(primitiveType)
		{
		case pt_Sphere:		
			initSphereInstance(((const SphereInst *)instance_data)+i,i);		
			break;
		case pt_Cone:		
			initConeInstance( ((const ConeInst *)instance_data)+i,i);			
			break;
		case pt_Cylinder:	
			initCylinderInstance( ((const CylinderInst *)instance_data)+i,i);   
			break;
		case pt_Box:		
			initBoxInstance( ((const BoxInst *)instance_data)+i,i);			    
			break;											  
		case pt_Torus:		
			initTorusInstance( ((const TorusInst *)instance_data)+i,i);         
			break;
		case pt_SurfOfRev:	
			initSurfOfRevInstance( ((const SurfOfRevInst *)instance_data)+i,i); 
			break;
		}	
	}

	switch(primitiveType)
	{	
	case pt_Box:
		d3dType = D3DPT_TRIANGLELIST;
		break;

	case pt_Cone:	
	case pt_Cylinder:	
	case pt_Sphere:	
	case pt_Torus:		
	case pt_SurfOfRev:
		d3dType = D3DPT_TRIANGLESTRIP;
		break;
	}	

	Set(&vAmbient, 0.2079f,1.f,0.100f,1.f );
	Set(&vDiffuse, 0.2079f,1.f,0.100f,1.f );
	Set(&vSpecular,0.2079f,1.f,0.100f,1.f );
}
///////////////////////////////////////////////////////////////////////////////
D3DVECTOR4 * SceneRenderer::decompressQuats(short *p_quat_data,DWORD *p_sign_data,int nquats)
{
	D3DVECTOR4 *pquats = (D3DVECTOR4 *)MemAlloc(sizeof(D3DVECTOR4)*nquats);
	float f_oo_scale = 1.f/32750.f;
	for(int i = 0; i < nquats; i++,p_quat_data+=3)
	{
		pquats[i].x = ((float)p_quat_data[0]) * f_oo_scale;
		pquats[i].y = ((float)p_quat_data[1]) * f_oo_scale;
		pquats[i].z = ((float)p_quat_data[2]) * f_oo_scale;

		pquats[i].w = fast_sqrt(1.f - pquats[i].x*pquats[i].x - 
						          pquats[i].y*pquats[i].y - 
								  pquats[i].z*pquats[i].z );
		
		int idw  = i >> 5;
		int bpos = i & 31;
		bool b_pos_w = ((p_sign_data[idw] & (1<<bpos))) != 0;
		if(!b_pos_w) 
			pquats[i].w *= -1.f;
	}

	return pquats;
}
///////////////////////////////////////////////////////////////////////////////
D3DVECTOR * SceneRenderer::decompressVecs(short *p_vec_data,int nvecs)
{
	D3DVECTOR * pvecs = (D3DVECTOR *)MemAlloc(sizeof(D3DVECTOR)*nvecs);
	for(int i = 0; i < nvecs; i++,p_vec_data+=3)
	{
		pvecs[i].x = ((float)p_vec_data[0]) * OO_POS_ANIM_SCALE_X + POS_ANIM_DELTA_X;
		pvecs[i].y = ((float)p_vec_data[1]) * OO_POS_ANIM_SCALE_Y + POS_ANIM_DELTA_Y;
		pvecs[i].z = ((float)p_vec_data[2]) * OO_POS_ANIM_SCALE_Z + POS_ANIM_DELTA_Z;
	}
	return pvecs;
}
///////////////////////////////////////////////////////////////////////////////
short * SceneRenderer::decompressIndices(char * p_indices,int nindices)
{
	short * pindices = (short *)MemAlloc(sizeof(short)*nindices);

	if(p_indices[0] == 127)
	{
		char hi = p_indices[1];
		char lo = p_indices[2];

		pindices[0] = ((((short)hi)&0xff)<<8) | (((short)lo)&0xff);
		p_indices += 2;
	}
	else
	{
		pindices[0] = p_indices[0];
	}

	for(int i = 1; i < nindices; i++)
	{
		if(p_indices[i] == 127)
		{
			char hi = p_indices[i+1];
			char lo = p_indices[i+2];

			pindices[i] = ((((short)hi)&0xff)<<8) | (((short)lo)&0xff);
			pindices[i] += pindices[i-1];
			
			p_indices   += 2;
		}
		else
		{
			pindices[i] = pindices[i-1] + p_indices[i];
		}
	}

	return pindices;
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::create()
{
	for(int i = 0; i < pt_NoTypes; i++)
	{
		pSetHiDet[i] = (PrimitiveSet *)MemAlloc(sizeof(PrimitiveSet));
        pSetHiDet[i]->Init();

		pSetLoDet[i] = (PrimitiveSet *)MemAlloc(sizeof(PrimitiveSet));
        pSetLoDet[i]->Init();
	}

	pQuats = decompressQuats(theQuats,theQuatSigns,numQuats);
	for(i = 0; i < NUM_ROT_SEQ; i++)
	{
		pQuatIdSeq[i] = decompressIndices(theRotAnimSeq[i].quatIds,MAX_ROT_SAMPLES);	
	}

	pPos = decompressVecs(thePos,numPos);
	for(i = 0; i < NUM_POS_SEQ; i++)
	{
		pPosIdSeq[i] = decompressIndices(thePosAnimSeq[i].posIds,MAX_POS_SAMPLES);
	}

	createSceneGeometry(pSetHiDet,0);
	createSceneGeometry(pSetLoDet,1);
	bUseLoDetail = false;

    gpd3dDev->CreateTexture(SB_WIDTH,SB_HEIGHT,1,0,D3DFMT_LIN_D16,0,&pSBufDepthLo);
	gpd3dDev->CreateTexture(SB_WIDTH,SB_HEIGHT,1,0,D3DFMT_LIN_D16,0,&pSBufDepthHi);

    memset(&fakeTarget, 0, sizeof(fakeTarget));

    XGSetSurfaceHeader(SB_WIDTH,
                       SB_HEIGHT,
                       D3DFMT_LIN_R5G6B5,
                       &fakeTarget,
                       0,
                       0);

    pSBufTarget = &fakeTarget;

    gpd3dDev->GetDepthStencilSurface(&pZBuffer);
    gpd3dDev->GetRenderTarget(&pRenderTarget);

	fZOffset = 10.f;
    fZSlopeScale = 4.0f;

	nRotAnims = sizeof(theRotAnimSeq)/sizeof(RotAnimSeq);
	nPosAnims = sizeof(thePosAnimSeq)/sizeof(PosAnimSeq);

	pRotAnims = (D3DMATRIX *)MemAlloc(sizeof(D3DMATRIX)*nRotAnims);
	pPosAnims = (D3DVECTOR *)MemAlloc(sizeof(D3DVECTOR)*nPosAnims);
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::createSceneGeometry(PrimitiveSet **pp_sets,int ndet_bias)
{
	pp_sets[pt_Sphere]->create(
		(const void *)theSphereVers, 
		sizeof(theSphereVers)/sizeof(SphereVers),
		(const void *)theSphereInsts, 
		sizeof(theSphereInsts)/sizeof(SphereInst),
		ndet_bias,
		st_Bump,
		pt_Sphere,
		gApp.pRoughBumpMap,
		gApp.pNormalCubeMapLoRes ); 

	pp_sets[pt_SurfOfRev]->create(
		(const void *)theSurfOfRevVers, 
		sizeof(theSurfOfRevVers)/sizeof(SurfOfRevVers),
		(const void *)theSurfOfRevInsts, 
		sizeof(theSurfOfRevInsts)/sizeof(SurfOfRevInst),
		ndet_bias,
		st_Bump, 
		pt_SurfOfRev,
		gApp.pRoughBumpMap,
		gApp.pNormalCubeMapHiRes );

	pp_sets[pt_Cone]->create(
		(const void *)theConeVers, 
		sizeof(theConeVers)/sizeof(ConeVers),
		(const void *)theConeInsts, 
		sizeof(theConeInsts)/sizeof(ConeInst),
		ndet_bias,
		st_Phong,
		pt_Cone,
		NULL, 
		gApp.pNormalCubeMapLoRes );

	pp_sets[pt_Box]->create( 
		NULL,1,
		(const void *)theBoxInsts, 
		sizeof(theBoxInsts)/sizeof(BoxInst),
		ndet_bias,
		st_Phong,
		pt_Box,
		NULL,
		gApp.pNormalCubeMapLoRes );

	pp_sets[pt_Cylinder]->create(
		(const void *)theCylinderVers, 
		sizeof(theCylinderVers)/sizeof(CylinderVers),
		(const void *)theCylinderInsts, 
		sizeof(theCylinderInsts)/sizeof(CylinderInst),
		ndet_bias,
		st_Phong,
		pt_Cylinder,
		NULL,gApp.pNormalCubeMapHiRes );

	pp_sets[pt_Torus]->create(
		(const void *)theTorusVers, 
		sizeof(theTorusVers)/sizeof(TorusVers),
		(const void *)theTorusInsts, 
		sizeof(theTorusInsts)/sizeof(TorusInst),
		ndet_bias,
		st_Phong,
		pt_Torus,
		NULL,
		gApp.pNormalCubeMapLoRes );	
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::destroy()
{
	for(int i = 0; i < pt_NoTypes; i++)
	{
        pSetHiDet[i]->UnInit();
        MemFree(pSetHiDet[i]);
        pSetHiDet[i] = NULL;

		pSetLoDet[i]->UnInit();
        MemFree(pSetLoDet[i]);
        pSetLoDet[i] = NULL;
	}

	MemFree(pRotAnims);
	MemFree(pPosAnims);

	MemFree(pQuats);
	for(i = 0; i < NUM_ROT_SEQ; i++)
	{
		MemFree(pQuatIdSeq[i]);
	}

	MemFree(pPos);
	for(i = 0; i < NUM_POS_SEQ; i++)
	{
		MemFree(pPosIdSeq[i]);
	}

#define XBS_RESOURCE_RELEASE(a) if (a) a->Release(); a = NULL;
    XBS_RESOURCE_RELEASE(pSBufDepthLo);
    XBS_RESOURCE_RELEASE(pSBufDepthHi);
#undef XBS_RESOURCE_RELEASE
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::render(bool b_with_shadows,bool b_use_blob_intensity)
{
	gpd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	gpd3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	gpd3dDev->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    gpd3dDev->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	gpd3dDev->SetTextureStageState( 1, D3DTSS_ADDRESSW,  D3DTADDRESS_CLAMP );

	gpd3dDev->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    gpd3dDev->SetTextureStageState( 2, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    gpd3dDev->SetTextureStageState( 2, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    gpd3dDev->SetTextureStageState( 2, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	gpd3dDev->SetTextureStageState( 2, D3DTSS_ADDRESSW,  D3DTADDRESS_CLAMP );
	
	gpd3dDev->SetTextureStageState(3,D3DTSS_ADDRESSU,    D3DTADDRESS_BORDER );
	gpd3dDev->SetTextureStageState(3,D3DTSS_ADDRESSV,    D3DTADDRESS_BORDER );
	gpd3dDev->SetTextureStageState(3,D3DTSS_BORDERCOLOR, 0xffffffff );
	gpd3dDev->SetTextureStageState(3,D3DTSS_MINFILTER,   D3DTEXF_LINEAR );
	gpd3dDev->SetTextureStageState(3,D3DTSS_MAGFILTER,   D3DTEXF_LINEAR );
	gpd3dDev->SetTextureStageState(3,D3DTSS_MIPFILTER,   D3DTEXF_NONE );

	if(b_with_shadows)
	{
		gpd3dDev->SetRenderState(D3DRS_SHADOWFUNC,D3DCMP_GREATER);
	}

	for(int i = 0; i < pt_NoTypes; i++)
	{
		if(bUseLoDetail)
			pSetLoDet[i]->render(gApp.blobLight,b_use_blob_intensity);
		else
			pSetHiDet[i]->render(gApp.blobLight,b_use_blob_intensity);
	}
	if(b_with_shadows)
	{
		gpd3dDev->SetRenderState(D3DRS_SHADOWFUNC,D3DCMP_ALWAYS);
	}

	gpd3dDev->SetTexture(3,NULL);
	gpd3dDev->SetPixelShader(NULL);
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::renderZ()
{
	for(int i = 0; i < pt_NoTypes; i++)
	{
		if(bUseLoDetail)
			pSetLoDet[i]->renderZ();
		else
			pSetHiDet[i]->renderZ();
	}
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::advanceTime(float fElapsedTime, float fDt)
{
	float fpos = ((fElapsedTime-SCENE_ANIM_START_TIME) / SCENE_ANIM_LEN);
	float ffrac_pos =  fpos * ((float)MAX_POS_SAMPLES-2);
	int   pos_id;

    __asm
    {
        cvttss2si eax, ffrac_pos
        mov pos_id, eax
    }

	float ffrac = ffrac_pos - (float)pos_id;
    
	for(int i = 0; i < nPosAnims; i++)
	{
		if(fpos <= 0.f)
		{
			pPosAnims[i] = pPos[pPosIdSeq[i][0]];
		}
		else if(fpos >= 1.f)
		{
			pPosAnims[i] = pPos[pPosIdSeq[i][MAX_POS_SAMPLES-1]];
		}
		else
		{
			D3DVECTOR &a = pPos[pPosIdSeq[i][pos_id]];
			D3DVECTOR &b = pPos[pPosIdSeq[i][pos_id+1]];

			pPosAnims[i].x = a.x * (1.f-ffrac) + b.x * ffrac; 
			pPosAnims[i].y = a.y * (1.f-ffrac) + b.y * ffrac; 
			pPosAnims[i].z = a.z * (1.f-ffrac) + b.z * ffrac; 
		}
	}

	ffrac_pos =  fpos * ((float)MAX_ROT_SAMPLES-2);

    __asm
    {
        cvttss2si eax, ffrac_pos
        mov pos_id, eax
    }

	ffrac = ffrac_pos - (float)pos_id;

	for(i = 0; i < nRotAnims; i++)
	{
		if(fpos <= 0.f)
		{
			D3DVECTOR4 &q = pQuats[pQuatIdSeq[i][0]];
			SetRotationFromLHQuat(q,&pRotAnims[i]);
		}
		else if(fpos >= 1.f)
		{
			D3DVECTOR4 &q = pQuats[pQuatIdSeq[i][MAX_ROT_SAMPLES-1]];
			SetRotationFromLHQuat(q,&pRotAnims[i]);
		}
		else
		{
			D3DVECTOR4 &a = pQuats[pQuatIdSeq[i][pos_id]];
			D3DVECTOR4 &b = pQuats[pQuatIdSeq[i][pos_id+1]];

			D3DVECTOR4 res;
			SlerpQuats(a,b,ffrac,&res);
			SetRotationFromLHQuat(res,&pRotAnims[i]);
		}	
	}

	bUseLoDetail = fElapsedTime >= SCENE_LO_DETAIL_START;
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::updateShadows()
{
	Camera cam = gApp.theCamera;
	updateSBuffer( false);
	updateSBuffer( true );
	gApp.theCamera = cam;
}
///////////////////////////////////////////////////////////////////////////////
void SceneRenderer::updateSBuffer(bool b_hi_z)
{
	Camera &cam = gApp.theCamera;

	D3DVECTOR eye_pos,look_pt,up;

	D3DVECTOR pos;
	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = b_hi_z ? CEIL_Z : FLOOR_Z;

	float fi;
	gApp.vblob.getLightForPosition(&eye_pos,&fi,pos);

	D3DVECTOR dir ;
	Set(&dir,0.f,0.f,(b_hi_z) ? 1.f : -1.f);
	Add(eye_pos,dir,&look_pt);
	Set(&up,0.f,(b_hi_z) ? -1.f : 1.f,0.f);
	
	cam.lookAt(eye_pos,look_pt,up);
	cam.setProjection(Pi/1.5f,1.f,1.f,500.f);

	D3DMATRIX shadow_proj_mat = cam.matProj;
	D3DMATRIX shadow_vp_mat;
    SetIdentity(&shadow_vp_mat);

    shadow_vp_mat._11 = SB_WIDTH   * 0.5f;
    shadow_vp_mat._22 = -SB_HEIGHT * 0.5f;
    shadow_vp_mat._33 = D3DZ_MAX_D16;

    shadow_vp_mat._41 = SB_WIDTH  * 0.5f + 0.5f;
    shadow_vp_mat._42 = SB_HEIGHT * 0.5f + 0.5f;

	D3DMATRIX mat;
	MulMats( cam.matWTC,shadow_proj_mat,&mat );
    MulMats( mat, shadow_vp_mat, (b_hi_z) ? &matWTSHi : &matWTSLo );
	
	if( gpd3dDev->BeginScene() == D3D_OK )
	{
		IDirect3DSurface8 *psurf;

		gpd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		gpd3dDev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		gpd3dDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		
		if(b_hi_z) 
			pSBufDepthHi->GetSurfaceLevel(0,&psurf);
		else	   
			pSBufDepthLo->GetSurfaceLevel(0,&psurf);

		gpd3dDev->SetRenderTarget(pSBufTarget,psurf);

		D3DVIEWPORT8 viewport = { 0, 0, SB_WIDTH, SB_HEIGHT, 0.0f, 1.0f };
		gpd3dDev->SetViewport(&viewport);

		gpd3dDev->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,1.0f,0);
		gpd3dDev->SetRenderState(D3DRS_COLORWRITEENABLE,0);

		gpd3dDev->SetRenderState(D3DRS_SOLIDOFFSETENABLE, TRUE);
		gpd3dDev->SetRenderState(D3DRS_POLYGONOFFSETZOFFSET, FtoDW(fZOffset));
		gpd3dDev->SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, FtoDW(fZSlopeScale));

		gpd3dDev->SetVertexShader(D3DFVF_XYZ);
		gpd3dDev->SetPixelShader (NULL);

		gpd3dDev->SetTransform(D3DTS_VIEW,&cam.matWTC);
		gpd3dDev->SetTransform(D3DTS_PROJECTION,&shadow_proj_mat);

		gpd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		gpd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
		gpd3dDev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

		for(int i = 0; i < 4; i++)
			gpd3dDev->SetTexture(i,NULL);

		const PrimitiveTypes sb_types[] = { pt_Torus, pt_Cone, pt_Box, pt_Cylinder };
		const int num_sb_types = sizeof(sb_types)/sizeof(PrimitiveTypes); 

		for(i = 0; i < num_sb_types; i++)
		{
			if(bUseLoDetail)
				pSetLoDet[sb_types[i]]->renderShadowMap(b_hi_z);
			else
				pSetHiDet[sb_types[i]]->renderShadowMap(b_hi_z);
		}

		gpd3dDev->SetRenderTarget(pRenderTarget, pZBuffer);
		gpd3dDev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL);
		gpd3dDev->SetRenderState(D3DRS_SOLIDOFFSETENABLE, FALSE);
		
		psurf->Release();

		gpd3dDev->EndScene();
	} 
}


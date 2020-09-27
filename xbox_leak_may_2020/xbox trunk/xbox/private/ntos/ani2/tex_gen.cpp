//
//	tex_gen.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#include "precomp.h"
#include "tex_gen.h"
#include "xbs_app.h"
#include "qrand.h"


#ifndef STARTUPANIMATION
#define ALLOC_TEX_MEM(nb) MemAlloc(nb)
#define FREE_TEX_MEM(pb)  MemFree(pb) 
#else
#define ALLOC_TEX_MEM(nb) MemAllocContiguous(nb,D3DTEXTURE_ALIGNMENT)
#define FREE_TEX_MEM(pb)  MemFreeContiguous(pb)
#endif


inline DWORD GetMaxMipCountFromSize(DWORD size )
{
	DWORD c = 0;
	
	while(size >>= 1) 
		++c;
	
	return c;
}

inline D3DCOLOR VectorToRGBA( const D3DVECTOR* v)
{
    D3DCOLOR color;

    FLOAT r = ( ( v->x + 1.0f ) * 127.5f );
    FLOAT g = ( ( v->y + 1.0f ) * 127.5f );
    FLOAT b = ( ( v->z + 1.0f ) * 127.5f );
    FLOAT a = ( 255.0f);

    __asm
    {
        cvttss2si edx, a       
        cvttss2si ecx, r       
        cvttss2si ebx, g       
        cvttss2si eax, b       
        shl ebx, 8             
        or  eax, ebx           
        shl ecx, 16            
        or  eax, ecx           
        shl edx, 24            
        or  eax, edx           
        mov color, eax       
    }

    return color;
}

LPDIRECT3DTEXTURE8 CreateHighlightTexture(int size,int power,bool b_falloff_alpha, float f_linear_w, float f_cos_w)
{
	IDirect3DTexture8 *pTexture;
	gpd3dDev->CreateTexture( size,size,1,0,D3DFMT_A8R8G8B8,NULL,&pTexture );

	D3DSURFACE_DESC desc;
	pTexture->GetLevelDesc(0,&desc);

	D3DLOCKED_RECT rc;
	pTexture->LockRect(0,&rc,NULL,0);
	int tmp = 4096/size;
	float	ooRadius = 1.0f/(float)(size/2);
	int cntrx = (size-1) / 2;
	int cntry = (size-1) / 2;

	unsigned int *pData;
	pData = (unsigned int *)rc.pBits;

	DWORD *pSourceBits = (DWORD *)ALLOC_TEX_MEM(sizeof(DWORD)*size*size);

	unsigned char cosTable[257];
	for (int i=0; i < 256; i++)
	{
		float Cos,Sin;
		SinCos((float)i/256.f,&Sin,&Cos);
		for (int k = power; k; --k)
			Cos *=Cos;
		float f_sum = 255.f * (Cos * f_cos_w + (float(256-i) / 256.f) * f_linear_w);

		__asm 
		{
			cvttss2si eax, f_sum
			mov		ebx,i
			mov	cosTable[ebx],al
		}
	}

	for (int y=0; y < size; y++)
	{
		unsigned int *pPixel = (unsigned int *)pSourceBits + y*size;
		for (int x=0; x < y; x++)
		{
			float f_dist = fast_sqrt((float)((x-cntrx)*(x-cntrx) + (y-cntry)*(y-cntry))) * ooRadius;
			if (f_dist < 1.f)
			{
				unsigned int c;
				float indexShift = f_dist * 256.f;
				__asm 
				{
					cvttss2si ebx, indexShift;
					movzx	  eax, byte ptr cosTable[ebx]
					mov		  c,eax
				}

				unsigned char a = b_falloff_alpha ? c : 255;

				DWORD P = c | c<<8 | c<<16 | a<<24;
				*pPixel++ = P;
			} 
			else 
			{
				unsigned char a = b_falloff_alpha ? 0 : 0xff000000;
				*pPixel++ = a;
			}
		}
	}
		
	XGSwizzleRect(pSourceBits,
				  0,
				  NULL,
				  rc.pBits,
				  size,
				  size,
				  NULL,
				 sizeof(DWORD));
	pTexture->UnlockRect(0);

	FREE_TEX_MEM(pSourceBits);

	return pTexture;	
}


LPDIRECT3DTEXTURE8 CreateGlowTexture(int width,int height,int colorScale,int noise,int seed)
{
	IDirect3DTexture8 *pTexture;
	int mipLevels = max(GetMaxMipCountFromSize(width),GetMaxMipCountFromSize(height));
	gpd3dDev->CreateTexture(
		width,
		height,
		mipLevels,			
		0,					
		D3DFMT_A8R8G8B8,	
		D3DPOOL_MANAGED,	
		&pTexture);

	for (int i=0; i < mipLevels; i++)
	{
		D3DSURFACE_DESC desc;
		pTexture->GetLevelDesc(i,&desc);

		D3DLOCKED_RECT rc;
		pTexture->LockRect(i,&rc,NULL,0);
		int sWidth = width >> i;
		int tmp = 4096/sWidth;
		int sHeight = height >> i;
		int scale=1;	
		while (tmp!=1) {
			scale++;
			tmp=tmp>>1;
		}
		int cntrx = (sWidth-1) / 2;
		int cntry = (sHeight-1) / 2;

		unsigned int *pData;
		pData = (unsigned int *)rc.pBits;

		DWORD *pSourceBits = (DWORD *)ALLOC_TEX_MEM(sizeof(DWORD)*sWidth*sHeight);

		unsigned int *pPixel = (unsigned int *)pSourceBits;

		for (int y=0; y < sHeight; y++)
		{
			for (int x=0; x < sWidth; x++)
			{
				_asm
				{
					mov		ecx,scale
					mov		eax,x
					mov		ebx,y
					sub		eax,cntrx
					sub		ebx,cntry
					sal		eax,cl
					imul	eax
					sal		ebx,cl
					xchg	eax,ebx
					mov		edi,pPixel
					imul	eax
					mov		edx,4096*4096
					add		ebx,eax
					sub		edx,ebx
					jnc		noOverflow1
					xor		edx,edx
noOverflow1:		mov		ebx,edx
					mov		eax,ebx
					mul		noise
					mov		ecx,seed
					mov		eax,edx
					mov		edx,ecx
					rcl		ecx,13
					sub		edx,11
					sub		ecx,edx
					mov		seed,ecx
					mul		ecx
					shl		edx,15
					sub		ebx,edx	
					jge		bxOk1
					xor		ebx,ebx
					
bxOk1:				and		ebx,0x1ff0000
					rcl		ebx,8
					sbb		ebx,0
					mov		eax,ebx
					shr		eax,24
					mul		al
					mul		eax
					shr		eax,16
					mul		eax
					shr		eax,16

					and		eax,0xff00
					mov		ecx,eax
					shr		ecx,8
					or		ecx,eax
					mov		eax,ecx
					shl		ecx,16
					or		ecx,eax

					mov		[edi],ecx
					add		edi,4
					mov		pPixel,edi
				}
			}
		}		

		XGSwizzleRect(pSourceBits,
					  0,
					  NULL,
					  rc.pBits,
					  sWidth,
					  sHeight,
					  NULL,
					  sizeof(DWORD));
		pTexture->UnlockRect(i);
	
		FREE_TEX_MEM(pSourceBits);
	}
	return pTexture;
}

LPDIRECT3DTEXTURE8 CreateGradientTexture( DWORD dwWidth,    DWORD dwHeight,
										  DWORD dwClrStart, DWORD dwClrEnd)
{
	LPDIRECT3DTEXTURE8 pTex = NULL;
	gpd3dDev->CreateTexture( dwWidth, dwHeight, 1, 0, D3DFMT_A8R8G8B8, NULL, &pTex );

	DWORD *pSourceBits = (DWORD *)ALLOC_TEX_MEM(sizeof(DWORD)*dwWidth*dwHeight);

	D3DCOLORVALUE cv_start,cv_end;

	cv_start.a = ((float)(dwClrStart>>24))/255.f;
	cv_start.r = ((float)((dwClrStart>>16)&0xff))/255.f;
	cv_start.g = ((float)((dwClrStart>>8)&0xff))/255.f;
	cv_start.b = ((float)(dwClrStart&0xff))/255.f;

	cv_end.a = ((float)(dwClrEnd>>24))/255.f;
	cv_end.r = ((float)((dwClrEnd>>16)&0xff))/255.f;
	cv_end.g = ((float)((dwClrEnd>>8)&0xff))/255.f;
	cv_end.b = ((float)(dwClrEnd&0xff))/255.f;

	float del = 1.f/((float)(dwHeight-1));

	for(DWORD r = 0; r < dwHeight; r++)
	{
		float t = del * ((float)r);

		D3DCOLOR color;

		FLOAT _r = (cv_start.r * (1.f-t) + cv_end.r * t) * 255.f;
		FLOAT _g = (cv_start.g * (1.f-t) + cv_end.g * t) * 255.f;
		FLOAT _b = (cv_start.b * (1.f-t) + cv_end.b * t) * 255.f;
		FLOAT _a = (cv_start.a * (1.f-t) + cv_end.a * t) * 255.f;

		__asm
		{
			cvttss2si edx, _a       
			cvttss2si ecx, _r       
			cvttss2si ebx, _g       
			cvttss2si eax, _b       
			shl ebx, 8             
			or  eax, ebx           
			shl ecx, 16            
			or  eax, ecx           
			shl edx, 24            
			or  eax, edx           
			mov color, eax       
		}

		DWORD *ppix = pSourceBits + r * dwWidth;
		for(DWORD x = 0; x < dwWidth; x++)
		{
			*ppix++ = color;
		}
	};

	D3DLOCKED_RECT rc;
	pTex->LockRect(0,&rc,NULL,0);

	XGSwizzleRect(pSourceBits,
				  0,
				  NULL,
				  rc.pBits,
				  dwWidth,
				  dwHeight,
				  NULL,
				  sizeof(DWORD));

	FREE_TEX_MEM(pSourceBits);

	pTex->UnlockRect(0);
	return pTex;
}

LPDIRECT3DTEXTURE8 CreateIntensityTexture(
							int           size,
							bool          b_convert_to_normal_map,
							float         f_height_scale,
							int           noise,
							int           seed,
							int           clr_mask,
							int           intensity_seed,
							bool          b_use_intensity_seed,
							DWORD         intensity_max,
							int			  negative_prob
							)
{
	LPDIRECT3DTEXTURE8 pTex;
	gpd3dDev->CreateTexture(size,size,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pTex);
	
	D3DLOCKED_RECT rc;
	pTex->LockRect(0,&rc,NULL,0);

	DWORD *pSourceBits = (DWORD *)ALLOC_TEX_MEM(sizeof(DWORD)*size*size);

	DWORD *pPixel = pSourceBits;
	memset(pPixel,0,sizeof(DWORD)*size*size);
	
	QRand rng;
    rng.Init(seed);

	DWORD i = (b_use_intensity_seed) ? intensity_seed : rng.Rand(intensity_max);
	*pPixel = (i<<16)|(i<<8)|(i);

	int curSize    = size>>1;
	int curX       = curSize;
	int curY       = curSize;
	int curNoise   = noise>>1;
	int curStep    = size;

	bool bSquare      = true;
	bool bSecondPass  = false;
	
	while(curSize > 0)
	{
		int lx = curX - curSize;
		int rx = curX + curSize;
		int ly = curY - curSize;
		int uy = curY + curSize;

		if(lx < 0)      lx += size;
		if(rx >= size) rx -= size;
		if(ly < 0)      ly += size;
		if(uy >= size) uy -= size;

		if(bSquare)
		{
			DWORD crnSW = *(pPixel + size*ly + lx);
			DWORD crnSE = *(pPixel + size*ly + rx);
			DWORD crnNW = *(pPixel + size*uy + lx);
			DWORD crnNE = *(pPixel + size*uy + rx);

			DWORD dwI = ((crnSW&0xff) + (crnSE&0xff) + (crnNW&0xff) + (crnNE&0xff) ) >> 2;
			
			if(rng.Rand(100) > negative_prob)
			{
				dwI += rng.Rand(curNoise);
				if(dwI > intensity_max) 
					dwI = intensity_max;
			}
			else
			{
				dwI -= rng.Rand(curNoise);
				if(dwI > 255) 
					dwI = 0;
			}
			

			*(pPixel + size*curY + curX) = (dwI<<16)|(dwI<<8)|(dwI);

			curX += curStep;
			if(curX >= size)
			{
				curY += curStep;
				if(curY >= size)
				{
					curX = curSize;
					curY = 0;
					bSquare = false;
					continue;	
				}
				curX = curSize;
			}
		}
		else
		{
			DWORD crnN = *(pPixel + size*uy   + curX);
			DWORD crnS = *(pPixel + size*ly   + curX);
			DWORD crnW = *(pPixel + size*curY + lx);
			DWORD crnE = *(pPixel + size*curY + rx);

			DWORD dwI = ((crnN&0xff) + (crnS&0xff) + (crnE&0xff) + (crnW&0xff)) >> 2;
			
			if(rng.Rand(100) > negative_prob)
			{
				dwI += rng.Rand(curNoise);
				if(dwI > intensity_max) 
					dwI = intensity_max;
			}
			else
			{
				dwI -= rng.Rand(curNoise);
				if(dwI > 255) 
					dwI = 0;
			}


			*(pPixel + size*curY + curX) = (dwI<<16)|(dwI<<8)|(dwI);

			curX += curStep;
			if(curX >= size)
			{
				curY += curStep;
				if(curY >= size)
				{
					if(bSecondPass)
					{
						curStep = curSize;
						curSize  >>= 1;
						curNoise >>= 1;
						curX = curSize;
						curY = curSize;
						bSquare = true;	
					}
					else
					{
						curX = 0;
						curY = curSize;
					}
					bSecondPass = !bSecondPass;
					continue;
				}

				curX = bSecondPass ? 0 : curSize;
			}	
		}
	}

	DWORD *pTooFar = pPixel + size*size;
	while(pPixel != pTooFar)
	{
		*pPixel = (((*pPixel)&0xff)<<24) | ((*pPixel)&clr_mask);
        pPixel++;
	}
	if(b_convert_to_normal_map)
	{
		for( int y=0; y<size; y++ )
		{
			DWORD *prow0 = pSourceBits + size*y;
			DWORD *prow1 = pSourceBits + size*(y+1);

			if(prow1 >= pTooFar)
				prow1 = pTooFar;

			for( int x = 0; x < size; x++ )
			{
				DWORD* p00 = prow0 + x;
				DWORD* p10 = prow0 + x + 1;
				if(p10 >= pTooFar)
					p10 = pTooFar;

				DWORD* p01 = prow1 + x;

				FLOAT fHeight00 = (FLOAT)(((*p00)&0x00ff0000)>>16) * f_height_scale;
				FLOAT fHeight10 = (FLOAT)(((*p10)&0x00ff0000)>>16) * f_height_scale;
				FLOAT fHeight01 = (FLOAT)(((*p01)&0x00ff0000)>>16) * f_height_scale;

				D3DVECTOR vPoint00;
				Set(&vPoint00, x+0.0f, y+0.0f, fHeight00 );

				D3DVECTOR vPoint10;
				Set(&vPoint10,x+0.1f, y+0.0f, fHeight10 );

				D3DVECTOR vPoint01;
				Set(&vPoint01, x+0.0f, y+0.1f, fHeight01 );
				
				D3DVECTOR v10;
				Sub(vPoint10,vPoint00,&v10);
				
				D3DVECTOR v01;
				Sub(vPoint01,vPoint00,&v01);

				D3DVECTOR v;
				Cross(v10, v01, &v);
				Normalize(&v);

				*p00 = VectorToRGBA( &v );
			}
		}
	}

	XGSwizzleRect(pSourceBits,
				  0,
				  NULL,
				  rc.pBits,
				  size,
				  size,
				  NULL,
				  sizeof(DWORD));

	FREE_TEX_MEM(pSourceBits);

	pTex->UnlockRect(0);

	return pTex;
}

void CreateIntensityTexture_8Bit(
							LPDIRECT3DTEXTURE8 ppTextures[],
							int   num,
							int   size,
							int   noise,
							int   seed,
							int   intensity_seed,
							int   intensity_max
							)
{
#define MAX_TEXTURES (3)
	if (num > MAX_TEXTURES) return;
	D3DLOCKED_RECT rcs[MAX_TEXTURES];

	int tex_size = size*size;

	int i;
	for (i=0; i<num; i++)
	{
		gpd3dDev->CreateTexture(size,size,1,0,D3DFMT_A8,D3DPOOL_MANAGED,&ppTextures[i]);
		ppTextures[i]->LockRect(0,&rcs[i],NULL,0);
	}
	
	BYTE *pSourceBits = (BYTE *)ALLOC_TEX_MEM(sizeof(BYTE)*size*size * num);
	// byte = pSourceBits[tex_num*tex_size + y*size + x];


	BYTE *pPixel = pSourceBits;
	memset(pPixel,0,sizeof(BYTE)*tex_size*num);
	
	QRand rng;
    rng.Init(seed);

	*pPixel = (BYTE)intensity_seed;

	int curSize    = size>>1;
	int curX       = curSize;
	int curY       = curSize;
	int curNoise   = noise>>1;
	int curStep    = size;

	bool bSquare      = true;
	bool bSecondPass  = false;
	
	while(curSize > 0)
	{
		int lx = curX - curSize;
		int rx = curX + curSize;
		int ly = curY - curSize;
		int uy = curY + curSize;

		if(lx < 0)      lx += size;
		if(rx >= size) rx -= size;
		if(ly < 0)      ly += size;
		if(uy >= size) uy -= size;

		if(bSquare)
		{
			for (i=0; i<num; i++)
			{
				int crnSW = *(pPixel + tex_size * i + size*ly + lx);
				int crnSE = *(pPixel + tex_size * i + size*ly + rx);
				int crnNW = *(pPixel + tex_size * i + size*uy + lx);
				int crnNE = *(pPixel + tex_size * i + size*uy + rx);

				int dwI = (crnSW + crnSE + crnNW + crnNE) >> 2;
				
				dwI += rng.Rand(curNoise*2) - curNoise;

				*(pPixel + tex_size * i + size*curY + curX) = (BYTE) (max(0, min(intensity_max, dwI)));
			}

			curX += curStep;
			if(curX >= size)
			{
				curY += curStep;
				if(curY >= size)
				{
					curX = curSize;
					curY = 0;
					bSquare = false;
					continue;	
				}
				curX = curSize;
			}
		}
		else
		{
			for (i=0; i<num; i++)
			{
				int crnN = *(pPixel + tex_size * i + size*uy   + curX);
				int crnS = *(pPixel + tex_size * i + size*ly   + curX);
				int crnW = *(pPixel + tex_size * i + size*curY + lx);
				int crnE = *(pPixel + tex_size * i + size*curY + rx);

				int dwI = ((crnN&0xff) + (crnS&0xff) + (crnE&0xff) + (crnW&0xff)) >> 2;

				dwI += rng.Rand(curNoise*2) - curNoise;

				*(pPixel + tex_size * i + size*curY + curX) = (BYTE) (max(0, min(intensity_max, dwI)));
			}

			curX += curStep;
			if(curX >= size)
			{
				curY += curStep;
				if(curY >= size)
				{
					if(bSecondPass)
					{
						curStep = curSize;
						curSize  >>= 1;
						curNoise >>= 1;
						curX = curSize;
						curY = curSize;
						bSquare = true;	
					}
					else
					{
						curX = 0;
						curY = curSize;
					}
					bSecondPass = !bSecondPass;
					continue;
				}

				curX = bSecondPass ? 0 : curSize;
			}	
		}
	}

	for (i=0; i<num; i++)
	{
		XGSwizzleRect(pSourceBits,
					  0,
					  NULL,
					  rcs[i].pBits,
					  size,
					  size,
					  NULL,
					  sizeof(BYTE));
		ppTextures[i]->UnlockRect(0);
	}

	FREE_TEX_MEM(pSourceBits);
}





LPDIRECT3DCUBETEXTURE8 CreateNormalizationCubeMap(DWORD dwSize)
{
	LPDIRECT3DCUBETEXTURE8 pCubeMap;     
    gpd3dDev->CreateCubeTexture( dwSize, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pCubeMap );

	DWORD * pSourceBits = (DWORD *)ALLOC_TEX_MEM(sizeof(DWORD)*dwSize*dwSize);

    for( DWORD i=0; i<6; i++ )
    {
        LPDIRECT3DSURFACE8 pCubeMapFace;
        pCubeMap->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &pCubeMapFace );
        DWORD*      pPixel = pSourceBits;
        D3DVECTOR   n;
        FLOAT       w, h;

        for( DWORD y = 0; y < dwSize; y++ )
        {
            h  = (FLOAT)y / (FLOAT)(dwSize-1);  // 0 to 1
            h  = ( h * 2.0f ) - 1.0f;           // -1 to 1
            
            for( DWORD x = 0; x < dwSize; x++ )
            {
                w = (FLOAT)x / (FLOAT)(dwSize-1);   // 0 to 1
                w = ( w * 2.0f ) - 1.0f;            // -1 to 1

                switch( i )
                {
                    case D3DCUBEMAP_FACE_POSITIVE_X:    // +x
                        n.x = +1.0;
                        n.y = -h;
                        n.z = -w;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_X:    // -x
                        n.x = -1.0;
                        n.y = -h;
                        n.z = +w;
                        break;
                        
                    case D3DCUBEMAP_FACE_POSITIVE_Y:    // y
                        n.x = +w;
                        n.y = +1.0;
                        n.z = +h;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_Y:    // -y
                        n.x = +w;
                        n.y = -1.0;
                        n.z = -h;
                        break;
                        
                    case D3DCUBEMAP_FACE_POSITIVE_Z:    // +z
                        n.x = +w;
                        n.y = -h;
                        n.z = +1.0;
                        break;
                        
                    case D3DCUBEMAP_FACE_NEGATIVE_Z:    // -z
                        n.x = -w;
                        n.y = -h;
                        n.z = -1.0;
                        break;
                }

				Normalize(&n);
                *pPixel++ = VectorToRGBA( &n );
            }
        }
        
        D3DLOCKED_RECT lock;
        pCubeMapFace->LockRect( &lock, 0, 0L );
        XGSwizzleRect( pSourceBits, 0, NULL, lock.pBits, dwSize, dwSize,
                       NULL, sizeof(DWORD) );
        pCubeMapFace->UnlockRect();
        pCubeMapFace->Release();
    }
    
	FREE_TEX_MEM(pSourceBits);

    return pCubeMap;
}


LPDIRECT3DCUBETEXTURE8 CreateStaticReflectionCubeMap( DWORD dwSize )
{
	Camera old_camera = gApp.theCamera;

	LPDIRECT3DCUBETEXTURE8 pCubeMap;

	gpd3dDev->CreateCubeTexture(dwSize,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,NULL,&pCubeMap);
	
	LPDIRECT3DSURFACE8 pOldRT,pOldZ;
	gpd3dDev->GetRenderTarget(&pOldRT);
	gpd3dDev->GetDepthStencilSurface(&pOldZ);

	LPDIRECT3DSURFACE8 pNewZ;
	gpd3dDev->CreateDepthStencilSurface(dwSize,dwSize,D3DFMT_LIN_D24S8,
										D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,
										&pNewZ);

	D3DMATRIX oldProjMat = gApp.theCamera.matProj;
	D3DMATRIX oldViewMat = gApp.theCamera.matWTC;

	gApp.theCamera.setProjection(Pi/2.f,1.f,0.1f,400.f);
	
	D3DVECTOR pos;
	Set(&pos,0.f,0.f,0.f);

	for(DWORD dwFace = 0; dwFace < 6; dwFace++)
	{
		LPDIRECT3DSURFACE8 pNewRT;
		pCubeMap->GetCubeMapSurface( (D3DCUBEMAP_FACES)dwFace, 0, &pNewRT );
		
		gpd3dDev->SetRenderTarget(pNewRT,pNewZ);
		
		if( gpd3dDev->BeginScene() == D3D_OK )
		{
			gpd3dDev->Clear(0,NULL,
							D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
							0xff000000,
							1.f,
							0 );

			D3DMATRIX viewMat;
			SetCubeMapView( dwFace, pos, &viewMat );

			gApp.theCamera.setWTC(viewMat);
			gApp.sceneGeom.advanceTime(SCENE_ANIM_START_TIME + SCENE_ANIM_LEN,0.f);
			gApp.sceneGeom.render(false,false);

			pNewRT->Release();

			gpd3dDev->EndScene();
		}
	}

	gpd3dDev->SetRenderTarget(pOldRT,pOldZ);
	
	pOldRT->Release();
	pOldZ->Release();
	pNewZ->Release();

	gApp.theCamera = old_camera;

	return pCubeMap;
}

int GetNumberOfIndicesForTristripMesh(int x_quads, int y_quads, bool b_d_tap_1, bool b_d_tap_2)
{
	if (x_quads <= 14)
	{
		// Single stack is sufficient.
		// Each mesh is composed of x_quads*2 + 1 priming vertices,
		// and for each row there are (x_quads+1)*2 + 2 CR vertices, minus one on the last row.
		return	((b_d_tap_1) ? 1 : 0) +
				x_quads*2 + 1 +
				y_quads*( 2*(x_quads+1) + 2) - 1 +
				((b_d_tap_2) ? 1 : 0);
	}

	return
		GetNumberOfIndicesForTristripMesh(        14, y_quads, b_d_tap_1, true) +
		GetNumberOfIndicesForTristripMesh(x_quads-14, y_quads, true, b_d_tap_2);
}


// returns number of indices added
int CreateTristripForMesh(	WORD* p_index_buffer,
							int x_quads,			// number of quad columns
							int y_quads,			// number of quad rows
							bool b_double_tap_first,
							bool b_double_tap_last,
							int start_index,		// starting index of lower-left corner
							int vstride,			// vertex difference between rows
							int hstride				// vertex difference between columns
							)
{
	if (!vstride) vstride = x_quads+1;
	if (!hstride) hstride = 1;

	if (x_quads > 14)
	{
		int num = 0;
		num += CreateTristripForMesh(&p_index_buffer[num],         14, y_quads, b_double_tap_first, true, start_index           , vstride, hstride);
		num += CreateTristripForMesh(&p_index_buffer[num], x_quads-14, y_quads, true,  b_double_tap_last, start_index+14*hstride, vstride, hstride);
		return num;
	}

	int num = 0;

	if (b_double_tap_first) p_index_buffer[num++] = (WORD) (start_index);
	p_index_buffer[num++] = (WORD) (start_index);

	int i;
	for (i=1; i<=x_quads; i++)
	{
		p_index_buffer[num++] = (WORD) (start_index + i*hstride);
		p_index_buffer[num++] = (WORD) (start_index + i*hstride);
	}

	for (int j=0; j<y_quads; j++)
	{
		p_index_buffer[num++] = (WORD) (start_index + j*vstride);				// double tap beginning of line
		for (int i=0; i<=x_quads; i++)
		{
			p_index_buffer[num++] = (WORD) (start_index + j*vstride + i*hstride);
			p_index_buffer[num++] = (WORD) (start_index + (j+1)*vstride + i*hstride);
		}
		if (j<y_quads-1) p_index_buffer[num++] = (WORD) (start_index + (j+1)*vstride + x_quads*hstride);	// double tap end of line
	}

	if (b_double_tap_last) p_index_buffer[num++] = (WORD) (start_index + y_quads*vstride + x_quads*hstride);

	return num;
}


LPDIRECT3DINDEXBUFFER8 CreateTristripForMesh(int x_quads, int y_quads, int* p_num_indices)
{
	LPDIRECT3DINDEXBUFFER8 p_ib;
	int dummy;
	if (!p_num_indices) p_num_indices = &dummy;
	*p_num_indices = GetNumberOfIndicesForTristripMesh(x_quads, y_quads);
	gpd3dDev->CreateIndexBuffer(
		*p_num_indices * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT,
		&p_ib
		);

	WORD* p_indices;
	p_ib->Lock(0, 0, (BYTE**)&p_indices, 0);
	CreateTristripForMesh(p_indices, x_quads, y_quads);
	p_ib->Unlock();	
	return p_ib;
}

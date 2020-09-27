//
//	bs_texgen.h
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#ifndef __TEX_GEN_H__
#define __TEX_GEN_H__


LPDIRECT3DTEXTURE8 CreateIntensityTexture(
							int   size,
							bool  b_convert_to_normal_map = false,
							float f_height_scale		  = 1.f/512.f,
							int   noise					  = 1024,
							int   seed				      = 0,
							int   clr_mask				  = 0x00ffffff,
							int   intensity_seed          = 255,
							bool  b_use_intensity_seed    = false,
							DWORD intensity_max           = 255,
							int   negative_prob           = 50
							);

void CreateIntensityTexture_8Bit(
							LPDIRECT3DTEXTURE8 ppTextures[],
							int   num,
							int   size,
							int   noise,
							int   seed,
							int   intensity_seed,
							int   intensity_max
							);



LPDIRECT3DTEXTURE8 CreateGlowTexture(int width,int height,int colorScale,int noise,int seed);
LPDIRECT3DTEXTURE8 CreateGradientTexture(DWORD dwWidth,DWORD dwHeight,DWORD dwClrStart,DWORD dwClrEnd);
LPDIRECT3DCUBETEXTURE8 CreateNormalizationCubeMap(DWORD dwSize);
LPDIRECT3DCUBETEXTURE8 CreateStaticReflectionCubeMap(DWORD dwSize);
LPDIRECT3DTEXTURE8 CreateHighlightTexture(int size,int power,
										  bool b_falloff_alpha, 
										  float f_linear_w, float f_cos_w);

int						GetNumberOfIndicesForTristripMesh(
												int x_quads,
												int y_quads,
												bool b_d_tap_1=false,
												bool b_d_tap_2=false
											);
int						CreateTristripForMesh(	WORD* p_index_buffer,
												int x_quads,			// number of quad columns
												int y_quads,			// number of quad rows
												bool b_double_tap_first=false,
												bool b_double_tap_last=false,
												int start_index = 0,	// starting index of lower-left corner
												int vstride = 0,		// vertex difference between rows
												int hstride = 0			// vertex difference between columns
											);	// returns number of indices added
LPDIRECT3DINDEXBUFFER8	CreateTristripForMesh(int x_quads, int y_quads, int* p_num_indices);



#endif	//__TEX_GEN_H__
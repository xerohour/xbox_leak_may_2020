
#if !defined _PERLIN_H_INCLUDED_
#define _PERLIN_H_INCLUDED_

//#include "d3dutils.h"

void		InitPerlinNoise(int seed);
float 		PerlinNoise1D (D3DXVECTOR3 & vec);
float 		PerlinTurbulence1D (D3DXVECTOR3 & point, int terms);
D3DVECTOR	PerlinNoise3D (D3DXVECTOR3 & vec);
D3DVECTOR	PerlinTurbulence3D (D3DXVECTOR3 & point, int terms);

#endif	// _PERLIN_H_INCLUDED_



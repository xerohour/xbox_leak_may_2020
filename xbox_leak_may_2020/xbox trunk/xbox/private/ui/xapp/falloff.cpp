#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Camera.h"


extern D3DXMATRIX g_matPosition;

#include "effect.h"
#include "effect2.h"
#include "effect3.h"
#include "effect4.h"
#include "aniso.h"


struct SHADEDESC
{
	int m_nEffect;
	DWORD* m_rgdwMicrocode;
	DWORD m_fvf;
	DWORD m_dwShader;
	bool m_bReportedError;
};

#define SHADERNAME(name) dw##name##VertexShader

SHADEDESC fixed_shaders [] =
{
	{
		0,
		SHADERNAME(Effect),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3 | D3DFVF_DIFFUSE
	},
	{
		1,
		SHADERNAME(Effect),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3
	},
	{
		2,
		SHADERNAME(Effect),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3 | D3DFVF_TEX1
	},
};

SHADEDESC shaders [] =
{
	{
		1,
		SHADERNAME(Effect),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3 | D3DFVF_DIFFUSE
	},
	{
		1,
		SHADERNAME(Effect),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3
	},
	{
		1,
		SHADERNAME(Effect2),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3 | D3DFVF_TEX1
	},
	{
		2,
		SHADERNAME(Aniso),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3
	},
	{
		3,
		SHADERNAME(Effect3),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3 | D3DFVF_TEX1
	},
	{
		4,
		SHADERNAME(Effect4),
		D3DFVF_XYZ | D3DFVF_NORMPACKED3
	},
    // Temporary shaders for uncompressed normals
	{
		1,
		SHADERNAME(Effect),
		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE
	},
	{
		1,
		SHADERNAME(Effect),
		D3DFVF_XYZ | D3DFVF_NORMAL
	},
	{
		1,
		SHADERNAME(Effect2),
		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	},
	{
		2,
		SHADERNAME(Aniso),
		D3DFVF_XYZ | D3DFVF_NORMAL
	},
	{
		3,
		SHADERNAME(Effect3),
		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	},
	{
		4,
		SHADERNAME(Effect4),
		D3DFVF_XYZ | D3DFVF_NORMAL
	},
};

DWORD GetEffectShader(int nEffect, DWORD fvf)
{
	if (nEffect == 0)
		return fvf;

	SHADEDESC* pShadeDesc = shaders;
	for (int i = 0; pShadeDesc->m_nEffect != nEffect || pShadeDesc->m_fvf != fvf; i += 1, pShadeDesc += 1)
	{
		if (i >= countof (shaders) - 1)
		{
			TRACE(_T("\001Cannot find effect %d shader for fvf: 0x%08x\n"), nEffect, fvf);
			return fvf;
		}
	}

	if (pShadeDesc->m_dwShader == 0)
	{
		const DWORD* rgdwFunction = NULL;

		ASSERT(pShadeDesc->m_rgdwMicrocode != NULL);
		rgdwFunction = pShadeDesc->m_rgdwMicrocode;

		static DWORD decl1 [] = 
		{
			D3DVSD_STREAM( 0 ),
			D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
			D3DVSD_REG( 3, D3DVSDT_NORMPACKED3 ), // Normal
			D3DVSD_END()
		};

		static DWORD decl2 [] = 
		{
			D3DVSD_STREAM( 0 ),
			D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
			D3DVSD_REG( 3, D3DVSDT_NORMPACKED3 ), // Normal
			D3DVSD_REG( 6, D3DVSDT_FLOAT2 ), // Tex coords
			D3DVSD_END()
		};

		static DWORD decl3 [] = 
		{
			D3DVSD_STREAM( 0 ),
			D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
			D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
			D3DVSD_END()
		};

		static DWORD decl4 [] = 
		{
			D3DVSD_STREAM( 0 ),
			D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
			D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
			D3DVSD_REG( 6, D3DVSDT_FLOAT2 ), // Tex coords
			D3DVSD_END()
		};

//		DWORD decl [MAX_FVF_DECL_SIZE];
//		VERIFYHR(D3DXDeclaratorFromFVF(fvf, decl));

		if (fvf & D3DFVF_NORMPACKED3) 
		{
			XAppCreateVertexShader(/*decl*/ (fvf & D3DFVF_TEX1) ? decl2 : decl1, rgdwFunction, &pShadeDesc->m_dwShader, 0);
		}
		else 
		{
			XAppCreateVertexShader(/*decl*/ (fvf & D3DFVF_TEX1) ? decl4 : decl3, rgdwFunction, &pShadeDesc->m_dwShader, 0);
		}
	}

	return pShadeDesc->m_dwShader;
}

DWORD GetFixedFunctionShader(DWORD fvf)
{
	SHADEDESC* pShadeDesc = fixed_shaders;
	for (int i = 0; pShadeDesc->m_fvf != fvf; i += 1, pShadeDesc += 1)
	{
		if (i >= countof (fixed_shaders) - 1)
		{
			if (fvf & D3DFVF_NORMAL)
			{
//				TRACE(_T("\001Cannot find compressed fixed function shader for fvf: 0x%08x\n"), fvf);
			}

			return fvf;
		}
	}

	if (pShadeDesc->m_dwShader == 0)
	{
		static DWORD decl[3][5] = 
		{
			{
				D3DVSD_STREAM( 0 ),
				D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ), // Position of first mesh
				D3DVSD_REG( D3DVSDE_NORMAL, D3DVSDT_NORMPACKED3 ), // Normal
				D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ), // Diffuse color
				D3DVSD_END()
			},
			{
				D3DVSD_STREAM( 0 ),
				D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ), // Position of first mesh
				D3DVSD_REG( D3DVSDE_NORMAL, D3DVSDT_NORMPACKED3 ), // Normal
				D3DVSD_END()
			},
			{
				D3DVSD_STREAM( 0 ),
				D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ), // Position of first mesh
				D3DVSD_REG( D3DVSDE_NORMAL, D3DVSDT_NORMPACKED3 ), // Normal
				D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2 ), // Tex coords
				D3DVSD_END()
			},
		};

		XAppCreateVertexShader(&decl[i][0], NULL, &pShadeDesc->m_dwShader, 0);
	}

	return pShadeDesc->m_dwShader;
}

DWORD CompressNormal(float* pvNormal) {

    float vNormal[3];
    float fLength;

    fLength = (float)sqrt(pvNormal[0] * pvNormal[0] + pvNormal[1] * pvNormal[1] + pvNormal[2] * pvNormal[2]);

    vNormal[0] = pvNormal[0] / fLength;
    vNormal[1] = pvNormal[1] / fLength;
    vNormal[2] = pvNormal[2] / fLength;

    return ((((DWORD)(vNormal[0] * 1023.0f) & 0x7FF) << 0)  |
            (((DWORD)(vNormal[1] * 1023.0f) & 0x7FF) << 11) |
            (((DWORD)(vNormal[2] *  511.0f) & 0x3FF) << 22));
}


extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matProjection;

void SetReflectShaderFrameValues()
{
    D3DXMATRIX mat, worldView;

    D3DXMatrixMultiply(&worldView, XAppGetWorld(), &g_matView);

     // World/View
    D3DXMatrixTranspose(&mat, &worldView);
    XAppSetVertexShaderConstant(40, &mat(0,0), 4);

    // Projection
    D3DXMatrixTranspose(&mat, &g_matProjection);
    XAppSetVertexShaderConstant(44, &mat(0,0), 4);

    XAppSetVertexShaderConstant(48, &D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.5f), 1);

	D3DXVECTOR4 lightDir(1.0f, 1.0f, -1.0f, 0.0f);
	D3DXVec4Normalize(&lightDir, &lightDir);
	D3DXMatrixTranspose(&mat, &worldView);
	D3DXVec3TransformNormal((D3DXVECTOR3*)&lightDir, (D3DXVECTOR3*)&lightDir, &mat);
	D3DXVec4Normalize(&lightDir, &lightDir);
	XAppSetVertexShaderConstant(49, &lightDir, 1);

    XAppSetRenderState(D3DRS_LIGHTING, FALSE);
}



void SetFalloffShaderFrameValues()
{
	D3DXMATRIX mat, worldView;

	D3DXMatrixMultiply(&worldView, XAppGetWorld(), &g_matView);

	// World/View/Projection
	D3DXMatrixMultiply(&mat, &worldView, &g_matProjection);
	D3DXMatrixTranspose(&mat, &mat);
	XAppSetVertexShaderConstant(0, &mat(0,0), 4);

	// Position Transform
	D3DXMatrixTranspose(&mat, &worldView);
	XAppSetVertexShaderConstant(10, &mat(0,0), 4);

	// Normal Transform
	D3DXMatrixInverse(&mat, NULL, &worldView);
	XAppSetVertexShaderConstant(5, &mat(0,0), 4);

	D3DXVECTOR4 v(0.0f, 0.5f, 1.0f, -1.0f);
	XAppSetVertexShaderConstant(9, &v, 1);
	
	D3DXVECTOR4 lightDir(1.0f, 1.0f, -1.0f, 0.0f);
	D3DXVec4Normalize(&lightDir, &lightDir);
	D3DXMatrixTranspose(&mat, &worldView);
	D3DXVec3TransformNormal((D3DXVECTOR3*)&lightDir, (D3DXVECTOR3*)&lightDir, &mat);
	D3DXVec4Normalize(&lightDir, &lightDir);
	XAppSetVertexShaderConstant(4, &lightDir, 1);

	XAppSetTexture(0, NULL);
	XAppSetRenderState(D3DRS_LIGHTING, FALSE);
}

float g_nEffectAlpha = 1.0f;

void SetFalloffShaderValues(const D3DXCOLOR& sideColor, const D3DXCOLOR& frontColor)
{
	D3DXVECTOR4 v;

	v.x = sideColor.r;
	v.y = sideColor.g;
	v.z = sideColor.b;
	v.w = sideColor.a * g_nEffectAlpha;
	XAppSetVertexShaderConstant(15, &v, 1);

	v.x = frontColor.r - sideColor.r;
	v.y = frontColor.g - sideColor.g;
	v.z = frontColor.b - sideColor.b;
	v.w = (frontColor.a - sideColor.a) * g_nEffectAlpha;
	XAppSetVertexShaderConstant(16, &v, 1);
}

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "VShader.h"

#define     DECL_START  { dwIndex = 0; }
#define     DECL(x)     { dwDecl[dwIndex++] = (x); }
#define     DECL4(x, y, z, w)   DECL((*(DWORD*)&(x))) DECL((*(DWORD*)&(y))) DECL((*(DWORD*)&(z))) DECL((*(DWORD*)&(w)))
#define     FUNC_START  { dwIndex = 0; }
#define     FUNC(x)     { dwFunc[dwIndex++] = (x); }
#define     END         \
    if (FAILED(m_pDevice->CreateVertexShaderTok(dwDecl, dwFunc, &m_pShaders[m_dwShaders].dwHandle, 0)))   \
    {                                                                       \
        WriteToLog("VShader: Device failed to create vertex shader\n");     \
        return false;                                                       \
    }                                                                       \
	m_pShaders[m_dwShaders].dwVersion = D3DVS_VERSION(1,0);					\
	m_pShaders[m_dwShaders].bSupported = true;								\
    m_dwShaders++;

bool VShader::CreateShaders()
{
    DWORD   dwCount = 0;
    DWORD   dwDecl[133];
    DWORD   dwFunc[133];
    DWORD   dwIndex;
    float   zOffset = 0.5f;
    float   fLightDir[4] = { 0.577350f, 0.577350f, -0.577350f, 0.f };
    float   fLightPos[4] = { 2.f, 2.f, -2.f, 0.f};
    float   fCameraPos[4] = { 0.f, 0.f, -5.f, 0.f };
    float   fMaterial[4] = {0.9f, 0.9f, 0.9f, 1.f};
    float   fRed[4] =   {0.0f, 0.1f, 1.0f, 1.f };
    float   fGreen[4] = {0.3f, 0.1f, 0.1f, 1.f };
    float   fBlue[4] =  {0.0f, 1.0f, 0.1f, 1.f };
    float   fPower[4] = {0.f, 0.f, 0.f, 500.f};
    float   fZero = 0.f;
    float   fOne = 1.f;
    float   fTwo = 2.f;
    float   fNC = 2.f / 256.f;
    float   fOffset = 0.5f;
	float	fIC = 1.f / 10000.f;
	float	fExpp = 2.9f;
	

    if (m_dwShaders)
        ReleaseShaders();

/*
    // First 96 (D3DVS_CONSTREG_MAXREG_V1_0) shaders to test 
    // MOV instruction from constant registers   
    for (dwCount = 0; dwCount < D3DVS_CONSTREG_MAXREG_V1_0; dwCount++)
    {
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_SKIP(D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(dwCount, 1) )
        DECL( ((dwCount & 0x01) ? (*(DWORD*)(&zOffset)) : 0) )
        DECL( ((dwCount & 0x02) ? (*(DWORD*)(&zOffset)) : 0) )
        DECL( ((dwCount & 0x04) ? (*(DWORD*)(&zOffset)) : 0) )
        DECL( 0 )
        DECL( D3DVSD_END() )
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
        FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL )
        FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE )
        FUNC( D3DSIO_MOV )
        FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL )
        FUNC( D3DSPR_CONST | dwCount | D3DVS_NOSWIZZLE )
        FUNC( D3DSIO_END )
        END
    }
*/
        // 1 MOV
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 2) )
        DECL4( zOffset, zOffset, zOffset, zOffset )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL( D3DVSD_END() )

        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
        // 2 MUL
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
        // 3 MAD
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 4) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL4( fZero, fOffset, fZero, fZero )
        DECL( D3DVSD_END() )
        
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MAD )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

        // 4 RCP
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_RCP )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_RCP )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
        // 5 RSQ
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_RSQ )
            FUNC( D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_RCP )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
        // 6 DP3
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_DP3 )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL)
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
        // 7 DP4
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_DP4 )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MUL)
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

        // 8 MIN
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fOne, fZero, fOne, fOne )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MIN )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

        // 9 MAX
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fOne, fZero, fOne, fOne )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MAX )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_X_Y | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

        // 10 SLT        
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fOne )
        DECL( D3DVSD_END() )
        
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_SLT )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
            FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_Y_X | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

        // 11 SGE        
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fOne )
        DECL( D3DVSD_END() )
        
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_SGE )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
            FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_Y_X | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END


        // 12 EXP
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_EXP )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
        // 13 LOG
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_LOG )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT |  D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
        // 14 LIT
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 7) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fLightPos[0], fLightPos[1], fLightPos[2], fLightPos[3] )
        DECL4( fCameraPos[0], fCameraPos[1], fCameraPos[2], fCameraPos[3] )
        DECL4( fRed[0], fRed[1], fRed[2], fRed[3] )
        DECL4( fGreen[0], fGreen[1], fGreen[2], fGreen[3] )
        DECL4( fBlue[0], fBlue[1], fBlue[2], fBlue[3] )
        DECL4( fPower[0], fPower[1], fPower[2], fPower[3] )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        // Send vertex coordinates to the output
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Calculate Ld in r[1]
        FUNC( D3DSIO_ADD )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP )
        FUNC( D3DSIO_DP4 )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_3 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_RSQ )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Calculate VPe in r[2]
        FUNC( D3DSIO_ADD )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP )
        FUNC( D3DSIO_DP4 )
            FUNC( D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_3 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_RSQ )
            FUNC( D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Calculate H in r[3]
        FUNC( D3DSIO_ADD )
            FUNC( D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_DP4 )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_3 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_RSQ )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Put N*L to the r[0].x
        FUNC( D3DSIO_DP3 )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Put N*H to the r[0].y
        FUNC( D3DSIO_DP3 )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Put Power to r[0].w
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_3 | D3DVS_OP )
            FUNC( D3DSPR_CONST | 6 | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Put LIT result to r[5]
        FUNC( D3DSIO_LIT )
            FUNC( D3DSPR_TEMP | 5 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        // Calculate output colors
        FUNC( D3DSIO_DP3 )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 5 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_DP3 )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 4 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_DP3 )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_2 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 5 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

        // 15 DST
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 2) )
        DECL4( fZero, fOne, fZero, fZero )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL( D3DVSD_END() )

        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_ADD )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP )
        FUNC( D3DSIO_DP3 )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_RSQ )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_3 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_Y | D3DVS_OP )
        FUNC( D3DSIO_DST )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

        // 16 FRC
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 2) )
        DECL4( fOne, fZero, fZero, fZero )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL( D3DVSD_END() )

        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_SGE )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_SLT )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_X_W | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MAD )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_FRC )
            FUNC( D3DSPR_TEMP | 3 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 3 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MAD )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

		
        // Macro Instructions
        // 17 D3DSIO_M4x4
        // 18 D3DSIO_M4x3
        // 19 D3DSIO_M3x4
        // 20 D3DSIO_M3x3
        // 21 D3DSIO_M3x2


        // 17 EXPP (t.z)
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_EXPP )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

		// 18 EXPP (t.xy)
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fExpp, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_EXPP )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_ADD )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
		// 19 EXPP (all registers)
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fExpp, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_EXPP )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_ADD )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DSPSM_NEG | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
		// 20 EXPP (t.y)
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fExpp, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_EXPP )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y | D3DVS_OP )
		FUNC( D3DSIO_LOGP )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_2 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 1 | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEMP | 2 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
   
		// 21 EXPP (t.xyw read)
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fExpp, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
		FUNC( D3DSIO_EXPP )
            FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_3 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_END )
        END
 

        // 22 LOGP
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_REG(0, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(1, D3DVSDT_FLOAT3) )
        DECL( D3DVSD_REG(2, D3DVSDT_FLOAT2) )
        DECL( D3DVSD_CONST(0, 3) )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fZero, fZero, fZero, fZero )
        DECL( D3DVSD_END() )
     
        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_LOGP )
            FUNC( D3DSPR_TEMP | 4 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT |  D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_X | D3DVS_W_X | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_TEMP | 4 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
    
		// More data formats
		// 23 FLOAT1 shader
        DECL_START
        DECL( D3DVSD_STREAM(1) )
		DECL( D3DVSD_REG(3,  D3DVSDT_FLOAT1) )
        DECL( D3DVSD_REG(4,  D3DVSDT_FLOAT1) )
        DECL( D3DVSD_REG(5,  D3DVSDT_FLOAT1) )
        DECL( D3DVSD_REG(6,  D3DVSDT_FLOAT1) )
        DECL( D3DVSD_REG(7,  D3DVSDT_FLOAT1) )
        DECL( D3DVSD_REG(8,  D3DVSDT_FLOAT1) )
        DECL( D3DVSD_REG(9,  D3DVSDT_FLOAT1) )
        DECL( D3DVSD_REG(10, D3DVSDT_FLOAT1) )
        DECL( D3DVSD_CONST(0, 2) )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL( D3DVSD_END() )

        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
		FUNC( D3DSIO_MOV )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_ADD )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 4 | D3DVS_X_Y | D3DVS_Y_X | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP )
		FUNC( D3DSIO_ADD )
			FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 5 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_X | D3DVS_W_Y | D3DVS_OP )
		FUNC( D3DSIO_MOV )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 6 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_ADD )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 7 | D3DVS_X_Y | D3DVS_Y_X | D3DVS_Z_Z | D3DVS_W_Z | D3DVS_OP )
		FUNC( D3DSIO_ADD )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 8 | D3DVS_X_Z | D3DVS_Y_Y | D3DVS_Z_X | D3DVS_W_Y | D3DVS_OP )
        FUNC( D3DSIO_DP3 )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_0 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 9 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 10 | D3DVS_X_X | D3DVS_Y_X | D3DVS_Z_Z | D3DVS_W_W | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

		// 24 FLOAT4 and D3DCOLOR shader
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_SKIP(6) )
        DECL( D3DVSD_REG(2,  D3DVSDT_FLOAT2) )
        DECL( D3DVSD_STREAM(2) )
        DECL( D3DVSD_REG(11, D3DVSDT_FLOAT4) )
        DECL( D3DVSD_REG(12, D3DVSDT_D3DCOLOR) )
        DECL( D3DVSD_CONST(0, 4) )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fTwo, fTwo, fTwo, fTwo )
        DECL4( fOne, fOne, fOne, fOne )
        DECL( D3DVSD_END() )

        FUNC_START
        FUNC( D3DVS_VERSION(1,0) )
		FUNC( D3DSIO_MOV )
			FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 11 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MOV )
			FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MAD )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_INPUT | 12 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSPR_CONST | 3 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP )
		FUNC( D3DSIO_DP3 )
			FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
		FUNC( D3DSIO_MUL )
            FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
			FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
            FUNC( D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
		
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END

		
		// 25 SHORT2, SHORT4, UBYTE4 shader
		if (m_d3dcaps.VertexProcessingCaps & D3DVTXPCAPS_NO_VSDT_UBYTE4)
		{
			m_pShaders[m_dwShaders].bSupported = false;
			m_dwShaders++;
		}
#ifndef UNDER_XBOX
		else
		{
			DECL_START
			DECL( D3DVSD_STREAM(3) )
			DECL( D3DVSD_REG(13, D3DVSDT_SHORT4) )
			DECL( D3DVSD_REG(14, D3DVSDT_UBYTE4) )
			DECL( D3DVSD_REG(15, D3DVSDT_SHORT2) )
			DECL( D3DVSD_CONST(0, 5) )
			DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
			DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
			DECL4( fIC, fIC, fIC, fIC )
			DECL4( fNC, fNC, fNC, fNC )
			DECL4( fOne, fOne, fOne, fOne )
			DECL( D3DVSD_END() )

			FUNC_START
			FUNC( D3DVS_VERSION(1,0) )
			
			FUNC( D3DSIO_MUL )
				FUNC( D3DSPR_RASTOUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
				FUNC( D3DSPR_INPUT | 13 | D3DVS_NOSWIZZLE | D3DVS_OP )
				FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSIO_MOV )
				FUNC( D3DSPR_TEMP | 1 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
				FUNC( D3DSPR_CONST | 3 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSIO_MAD )
				FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
				FUNC( D3DSPR_INPUT | 14 | D3DVS_NOSWIZZLE | D3DVS_OP )
				FUNC( D3DSPR_TEMP | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
				FUNC( D3DSPR_CONST | 4 | D3DVS_NOSWIZZLE | D3DSPSM_NEG | D3DVS_OP )
			FUNC( D3DSIO_DP3 )
				FUNC( D3DSPR_TEMP | 0 | D3DSP_WRITEMASK_ALL | D3DVS_OP )
				FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
				FUNC( D3DSPR_CONST | 1 | D3DVS_NOSWIZZLE | D3DVS_OP )
			FUNC( D3DSIO_MUL )
				FUNC( D3DSPR_ATTROUT | D3DSP_WRITEMASK_ALL | D3DVS_OP )
				FUNC( D3DSPR_TEMP | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )
				FUNC( D3DSPR_CONST | 0 | D3DVS_NOSWIZZLE | D3DVS_OP )

			FUNC( D3DSIO_MUL )
				FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DVS_OP )
				FUNC( D3DSPR_INPUT | 15 | D3DVS_NOSWIZZLE | D3DVS_OP )
				FUNC( D3DSPR_CONST | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )

			FUNC( D3DSIO_END )
			END
		}
#else
        Log(LOG_FAIL, TEXT("D3DVTXPCAPS_NO_VSDT_UBYTE4 not included in caps bits"));
		m_pShaders[m_dwShaders].bSupported = false;
		m_dwShaders++;
#endif // UNDER_XBOX
/*
		// 26 Registry corruption
        DECL_START
        DECL( D3DVSD_STREAM(0) )
        DECL( D3DVSD_SKIP(6) )
        DECL( D3DVSD_REG(2,  D3DVSDT_FLOAT2) )
        DECL( D3DVSD_STREAM(2) )
        DECL( D3DVSD_REG(11, D3DVSDT_FLOAT4) )
        DECL( D3DVSD_REG(12, D3DVSDT_D3DCOLOR) )
        DECL( D3DVSD_CONST(0, 4) )
        DECL4( fMaterial[0], fMaterial[1], fMaterial[2], fMaterial[3] )
        DECL4( fLightDir[0], fLightDir[1], fLightDir[2], fLightDir[3] )
        DECL4( fTwo, fTwo, fTwo, fTwo )
        DECL4( fOne, fOne, fOne, fOne )
        DECL( D3DVSD_END() )

		FUNC(
		
		FUNC( D3DSIO_MOV )
            FUNC( D3DSPR_TEXCRDOUT | D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DVS_OP )
            FUNC( D3DSPR_INPUT | 2 | D3DVS_NOSWIZZLE | D3DVS_OP )
        FUNC( D3DSIO_END )
        END
*/
        return true;
}

void VShader::ReleaseShaders()
{
    for (DWORD dwCount = 0; dwCount < m_dwShaders; dwCount++)
    {
        if (m_pShaders[dwCount].dwHandle) {
            m_pDevice->DeleteVertexShader(m_pShaders[dwCount].dwHandle);
            m_pShaders[dwCount].dwHandle = 0;
        }
    }
    m_dwShaders = 0;
}


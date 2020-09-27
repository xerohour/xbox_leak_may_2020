
 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#ifndef _nvVShad_h
#define _nvVShad_h

#include "../../../common/inc/vtxpgmcomp.h"
#include "nvDbg.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct v_shader_register{  //replace with DX define when I find it HMH
    float x;
    float y;
    float z;
    float w;
} VSHADERREGISTER, *PVSHADERREGISTER;

// vertex attribute source info
typedef struct _VASOURCEINFO {
    DWORD dwStream;    // the stream from which this attribute gets its values
    DWORD dwOffset;    // the offset (in bytes) within a component from which the values come
    DWORD dwType;      // the type of data loaded into this attribute
    DWORD dwSrc;       // for tessellator types -- src register for a normal/uv autocalc
    DWORD flag;        // type of autocalc   0=none, 1=normal, 2=autotex
} VASOURCEINFO;

//---------------------------------------------------------------------------

class CVertexShader
{
public:

    enum {
        AUTONONE      = 0,
        AUTONORMAL    = 1,
        AUTOTEXCOORD  = 2,
    };

    enum {
        VA_STREAM_NONE = 0xFFFFFFFF
    };

    VASOURCEINFO        m_vaSourceData[NV_CAPS_MAX_STREAMS];  // sources of the data loaded into the 16 vertex attribute registers

    ParsedProgram       m_ParsedProgram;
    VertexProgramOutput m_ProgramOutput;
    VSHADERREGISTER     m_Attrib[16];
    VSHADERREGISTER     m_Result[15];

    char                m_numConsts            [D3DVS_CONSTREG_MAX_V1_1]; // number of constants set from each index
    VSHADERREGISTER     m_vertexShaderConstants[D3DVS_CONSTREG_MAX_V1_1]; // actual constants

#if (NVARCH >= 0x020)
    PKELVIN_PROGRAM       m_pKelvinProgram;      // compiled kelvin meta-code
#endif

protected:

    DWORD             m_dwHandle;
    DWORD             m_dwFVF;
    DWORD             m_dwCodeSize;          // size of code   declaration
    DWORD            *m_pCodeData;           // ptr to code    declaration data
    DWORD             m_dwStride;            // the total size (in bytes) of all vertex attributes pulled from streams by this shader
    float             m_one, m_zero;
    BOOL              m_bHasConsts;

    VtxProgCompileX86    m_Caller;
    VtxProgCompileKelvin m_CallerKelvin;

public:

    BOOL create (PNVD3DCONTEXT pContext, DWORD dwHandle, DWORD dwStreamSize, DWORD dwCodeSize, DWORD *lpStream, DWORD *lpCode);
    BOOL create (PNVD3DCONTEXT pContext, DWORD dwFVF, DWORD dwHandle);
    BOOL reset ( void );

    CVertexShader(void);
    ~CVertexShader(void);

    inline BOOL  bVAExists            (DWORD dwReg)   {   return ((getVAStream(dwReg) != VA_STREAM_NONE) ? TRUE : FALSE);   }

    inline DWORD getVAStream          (DWORD dwReg)   {   return (m_vaSourceData[dwReg].dwStream);   }
    inline DWORD getVAOffset          (DWORD dwReg)   {   return (m_vaSourceData[dwReg].dwOffset);   }
    inline DWORD getVAType            (DWORD dwReg)   {   return (m_vaSourceData[dwReg].dwType);     }
    inline DWORD getVASrc             (DWORD dwReg)   {   return (m_vaSourceData[dwReg].dwSrc);      }
    inline char  getVAFlag            (DWORD dwReg)   {   return (m_vaSourceData[dwReg].flag);       }

    inline DWORD getAutoNormalStream  (void)          {   for (int i=0; i<NV_CAPS_MAX_STREAMS; i++) {
                                                              if (m_vaSourceData[i].flag == CVertexShader::AUTONORMAL) {
                                                                  return i;
                                                              }
                                                          }
                                                          return VA_STREAM_NONE;
                                                      }

    inline DWORD getAutoTextureStream (DWORD dwCount) {   for (int i=0; i<NV_CAPS_MAX_STREAMS; i++) {
                                                              if (m_vaSourceData[i].flag == CVertexShader::AUTOTEXCOORD) {
                                                                  if (dwCount == 0) return i;
                                                                  else dwCount--;
                                                              }
                                                          }
                                                          return VA_STREAM_NONE;
                                                      }

    inline DWORD getVASize            (DWORD dwReg)   {   if (bVAExists(dwReg)) {
                                                              switch (getVAType(dwReg)) {
                                                              case D3DVSDT_FLOAT1:   // 0
                                                                  return 1 * sizeof(DWORD);
                                                              case D3DVSDT_FLOAT2:   // 1
                                                                  return 2 * sizeof(DWORD);
                                                              case D3DVSDT_FLOAT3:   // 2
                                                                  return 3 * sizeof(DWORD);
                                                              case D3DVSDT_FLOAT4:   // 3
                                                                  return 4 * sizeof(DWORD);
                                                              case D3DVSDT_D3DCOLOR: // 4
                                                                  return 4 * sizeof(BYTE);
                                                              case D3DVSDT_UBYTE4:   // 5
                                                                  return 4 * sizeof(BYTE);
                                                              case D3DVSDT_SHORT2:   // 6
                                                                  return 2 * sizeof(SHORT);
                                                              case D3DVSDT_SHORT4:   // 7
                                                                  return 4 * sizeof(SHORT);
                                                              default:
                                                                  DPF("Unknown vertex array type: %d", getVAType(dwReg));
                                                                  nvAssert(0);
                                                                  return 0;
                                                              }
                                                          } else {
                                                              return 0;
                                                          }
                                                      }

    inline BOOL  hasProgram           (void)   const  {   return m_dwCodeSize ? TRUE : FALSE; }
    inline BOOL  hasConstants         (void)   const  {   return m_bHasConsts;                }
    inline DWORD getStride            (void)   const  {   return m_dwStride;                  }
    inline DWORD getHandle            (void)   const  {   return m_dwHandle;                  }
    inline DWORD getFVF               (void)   const  {   return m_dwFVF;                     }
    inline BOOL  isFvfShader          (void)   const  {   return !(m_dwHandle & 0x1);         }

    // This is temporary.  For the short-term we will have all FVFs share the
    // same vertex shader 0 ala DX7.
    // This may be replaced with a hash table lookup in Rel20.
    static inline DWORD getHandleFromFvf     (DWORD dwFvf)   { return 0ul; }

};

#ifdef __cplusplus
}
#endif

#endif // _nvVShad_h


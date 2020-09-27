// **************************************************************************
//
//       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvSuperTri.h
//      SuperTri state header file
//
// **************************************************************************
//
//  History:
//      Scott Kephart           08Nov00             Kelvin Development
//
// **************************************************************************


#if (NVARCH >= 0x10)
// xPipe / Super-tri constants
#define NV_CULLFLAG_SUPERTRI            0x00000001
#define NV_CULLFLAG_MODELOK             0x00000002
#define NV_CULLFLAG_XFORMOK             0x00000004

// xPipe / Super-tri inner loop flags. These ought to be in the inner loop flags above, but we don't have enough
// flag bits.


#define NV_STILFLAG_STDEFVB             0x00000001  // default VB is used (affects supertri logic)
#define NV_STILFLAG_MODEL               0x00000002  // Model Culling
#define NV_STILFLAG_XFORM               0x00000004  // Xform / clipcheck / cull
#define NV_STILFLAG_HIBW                0x00000008  // High bandwidth mode -- check all triangles
#define NV_STILFLAG_NOCULL              0x00000010  // Disable cull-check

class CSuperTri;
class CVertexShader;

typedef bool (*PST_STRAT_FN)(CSuperTri *, DWORD dwDP2Operation, DWORD primcount, CVertexShader *pVertexShader);

class CSuperTri
{
//private:
public:
    // Number of flips to allow pending before using SUPER_TRI code.
    DWORD                       dwSuperTriPendingFlips;
    DWORD                       dwSuperTriThrottle;
    DWORD                       dwSuperTriMask;
    DWORD                       cullsign;
    D3DVECTOR4                  eye;                                 // supertri eye in model space
//bank
    DWORD                       dwCullFlags;                         // culling state flags
    DWORD                       dwSTILFlags;
    DWORD                       dwSTLBStride;
    PST_STRAT_FN                pStrategy;
    PNVD3DCONTEXT               pContext;
    DWORD                       dwNumLights;
    DWORD                       _pad0[2];
public:
    void SuperTriInit(PNVD3DCONTEXT pContext);
    __forceinline void setSuperTri() {dwCullFlags |= NV_CULLFLAG_SUPERTRI;}
    __forceinline void clrSuperTri() {dwCullFlags &= ~(NV_CULLFLAG_SUPERTRI|NV_CULLFLAG_MODELOK);}
    __forceinline bool isSuperTri() {return ((dwCullFlags & NV_CULLFLAG_SUPERTRI) != 0);}
    __forceinline void setModelCullOK() {dwCullFlags |= NV_CULLFLAG_MODELOK;}
    __forceinline void clrModelCullOK() {dwCullFlags &= ~(NV_CULLFLAG_MODELOK);}
    __forceinline bool isModelCullOK() {return (dwCullFlags & NV_CULLFLAG_MODELOK) != 0;}
    __forceinline void setXFormCullOK() {dwCullFlags |= NV_CULLFLAG_XFORMOK;}
    __forceinline void clrXFormCullOK() {dwCullFlags &= ~(NV_CULLFLAG_XFORMOK);}
    __forceinline bool isXFormCullOK() {return (dwCullFlags & NV_CULLFLAG_XFORMOK) != 0;}
    __forceinline void setFixedLBStride() {dwSTILFlags &= ~NV_STILFLAG_STDEFVB;};
    __forceinline bool isFixedLBStride() {return (dwSTILFlags & NV_STILFLAG_STDEFVB) == 0;}
    __forceinline void setVarLBStride() {dwSTILFlags |= NV_STILFLAG_STDEFVB;};
    __forceinline void setModelCull() {dwSTILFlags |= NV_STILFLAG_MODEL; dwSTILFlags &= ~NV_STILFLAG_XFORM;}
    __forceinline void clrCullILFlags() {dwSTILFlags &= ~(NV_STILFLAG_MODEL | NV_STILFLAG_XFORM | NV_STILFLAG_STDEFVB | NV_STILFLAG_HIBW | NV_STILFLAG_NOCULL);}
    __forceinline void setXformCull() {dwSTILFlags |= NV_STILFLAG_XFORM; dwSTILFlags &= ~NV_STILFLAG_MODEL;}
    __forceinline bool isModelCull() {return (dwSTILFlags & NV_STILFLAG_MODEL) != 0;}
    __forceinline bool isXFormCull() {return (dwSTILFlags & NV_STILFLAG_XFORM) != 0;}
    __forceinline void setCheckAll()  {dwSTILFlags |= NV_STILFLAG_HIBW;}
    __forceinline void clrCheckAll()  {dwSTILFlags &= ~NV_STILFLAG_HIBW;}
    __forceinline bool isCheckAll() {return (dwSTILFlags & NV_STILFLAG_HIBW) != 0;}
    __forceinline void setNoCull() {dwSTILFlags |= NV_STILFLAG_NOCULL;}
    __forceinline void clrNoCull() {dwSTILFlags &= ~NV_STILFLAG_NOCULL;}
    __forceinline bool isNoCull() {return (dwSTILFlags & NV_STILFLAG_NOCULL) != 0;}
    __forceinline void setNumLights(DWORD lights) {dwNumLights = lights;}
    __forceinline DWORD getNumLights() {return dwNumLights;}

    void setSTLBStride();

    __forceinline void setSTLBStride (DWORD stride) { dwSTLBStride = stride; }

    __forceinline DWORD getSTLBStride(){ return dwSTLBStride; }

    void setStrategy();
    __forceinline bool Strategy(DWORD dwDP2Operation, DWORD PrimCount, CVertexShader *pVertexShader) { return (*pStrategy)(this, dwDP2Operation, PrimCount, pVertexShader); }
    void setEye(D3DMATRIX *MV);
    __forceinline DWORD getEyeAddr() {return (DWORD) (&eye);}
    __forceinline void setCullsign(DWORD c) {cullsign = c;}
    __forceinline DWORD getCullsignAddr() {return (DWORD) (&cullsign);}

    friend bool ST_Strat_P3(CSuperTri *st, DWORD dwDP2Operation, DWORD PrimCount, CVertexShader *pVertexShader);
    friend bool ST_Strat_P4(CSuperTri *st, DWORD dwDP2Operation, DWORD PrimCount, CVertexShader *pVertexShader);
    friend bool ST_Strat_Null(CSuperTri *st, DWORD dwDP2Operation, DWORD PrimCount, CVertexShader *pVertexShader);
};

#endif


// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
//  Module: nvDbgSurf.cpp
//      Debug surface placement
//
// **************************************************************************
//
//  History:
//      Scott Kephart           01Dec00         NV20 development
//
// **************************************************************************


#ifdef DEBUG_SURFACE_PLACEMENT
#define CSD_MAX_BACK_BUFFERS 2
#define CSD_MAX_SURFACES 8

enum SurfDbgType {
FrontBuffer=0,
BackBuffer,
Z_Buffer,
AA_RenderTarget,
AA_Z_Buffer
};

class CSurfDebugOrder {
public:
    SurfDbgType     SurfaceType;
    CSimpleSurface  *pSurf;
    CSimpleSurface  Surf;
    bool Deleted;
    bool ReCreated;
};

#define ZTAG_MAX    1024

class CKelvinFBRegs {
public:
    DWORD nv_pfb_cfg0;
    DWORD nv_pfb_cfg1;
    DWORD nv_pfb_cstatus;
    DWORD nv_pfb_timing0;
    DWORD nv_pfb_timing1;
    DWORD nv_pfb_timing2;
    DWORD nv_pfb_tile[8];
    DWORD nv_pfb_tlimit[8];
    DWORD nv_pfb_tsize[8];
    DWORD nv_pfb_tstatus[8];
    DWORD nv_pfb_zcomp[8];
    DWORD nv_pfb_zcomp_max_tag;
    DWORD nv_pfb_zcomp_offset;
    DWORD nv_ztag0[ZTAG_MAX];
    DWORD nv_ztag1[ZTAG_MAX];
    DWORD nv_ztag2[ZTAG_MAX];
    DWORD nv_ztag3[ZTAG_MAX];
    
};

//---------------------------------------------------------------------------
class CSurfaceDebug 
{
private:
    int             FileNum;
    int             surf_cnt;
    BYTE           *pHWRegisters;
    CSurfDebugOrder Surfaces[CSD_MAX_SURFACES];
    bool            bInitialized;
    bool            bValidated;
    bool            bFileOpen;
    HANDLE          CSDFile;
    char outbuf[1024];
public:
    CSurfaceDebug() {};
    ~CSurfaceDebug() {};
    void Init();
    void Destroy();
    void DumpSurfaceInfo();
    void DumpCSimpleSurface(CSimpleSurface *p);
    void ValidateSurfacePlacement();
    void SetFrontBuffer(CSimpleSurface *pBuff);
    void SetBackBuffer(CSimpleSurface *pBuff);
    void SetZBuffer(CSimpleSurface *pBuff);
    void ReCreateZBuffer(CSimpleSurface *pBuff);
    void SetAARenderBuffer(CSimpleSurface *pBuff);
    void SetAAZBuffer(CSimpleSurface *pBuff);
    void DeleteSurface(CSimpleSurface *pBuff);
    void CaptureFBRegs();
private:
    void REG_WR32(DWORD addr, DWORD data);
    void REG_RD32(DWORD addr, DWORD &data);
    void RDI_REG_WR32(DWORD select, DWORD addr, DWORD data);
    void RDI_REG_RD32(DWORD select, DWORD addr, DWORD &data);
};
#endif //DEBUG_SURFACE_PLACEMENT

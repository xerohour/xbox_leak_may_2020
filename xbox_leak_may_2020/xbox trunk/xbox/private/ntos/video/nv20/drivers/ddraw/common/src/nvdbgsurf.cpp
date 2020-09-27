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


#include "nvprecomp.h"
#pragma hdrstop

#ifdef DEBUG_SURFACE_PLACEMENT

void CSurfaceDebug::Init()
{
    surf_cnt = 0;
    bInitialized = true;
    pHWRegisters = reinterpret_cast<BYTE *>(pDriverData->NvBaseFlat);
}    


void CSurfaceDebug::Destroy()
{

    DumpSurfaceInfo();
    bInitialized = false;
}    

void CSurfaceDebug::SetFrontBuffer(CSimpleSurface *pBuff)
{
    if (!bInitialized)
        Init();

    if (pBuff && surf_cnt < (CSD_MAX_SURFACES)) 
    {
        Surfaces[surf_cnt].SurfaceType = FrontBuffer;
        Surfaces[surf_cnt].Deleted = false;
        Surfaces[surf_cnt].ReCreated = false;
        Surfaces[surf_cnt].pSurf = pBuff;
        Surfaces[surf_cnt++].Surf = *pBuff;
        
    }
}    

void CSurfaceDebug::SetBackBuffer(CSimpleSurface *pBuff)
{
    if (!bInitialized)
        Init();

    if (pBuff  && (surf_cnt < CSD_MAX_SURFACES))
    {
        Surfaces[surf_cnt].SurfaceType = BackBuffer;
        Surfaces[surf_cnt].Deleted = false;
        Surfaces[surf_cnt].ReCreated = false;
        Surfaces[surf_cnt].pSurf = pBuff;
        Surfaces[surf_cnt++].Surf = *pBuff;
        
    }
}    

void CSurfaceDebug::SetZBuffer(CSimpleSurface *pBuff)
{
    if (!bInitialized)
        Init();

    if (pBuff && (surf_cnt < CSD_MAX_SURFACES)) 
    {
        Surfaces[surf_cnt].SurfaceType = Z_Buffer;
        Surfaces[surf_cnt].Deleted = false;
        Surfaces[surf_cnt].ReCreated = false;
        Surfaces[surf_cnt].pSurf = pBuff;
        Surfaces[surf_cnt++].Surf = *pBuff;
        
    }
    
}    

void CSurfaceDebug::ReCreateZBuffer(CSimpleSurface *pBuff)
{
    if (!bInitialized)
        Init();

    if (pBuff && (surf_cnt < CSD_MAX_SURFACES)) 
    {
        Surfaces[surf_cnt].SurfaceType = Z_Buffer;
        Surfaces[surf_cnt].Deleted = false;
        Surfaces[surf_cnt].ReCreated = true;
        Surfaces[surf_cnt].pSurf = pBuff;
        Surfaces[surf_cnt++].Surf = *pBuff;
        
    }
    
}    

void CSurfaceDebug::SetAARenderBuffer(CSimpleSurface *pBuff)
{
    if (!bInitialized)
        Init();

    if (pBuff && (surf_cnt < CSD_MAX_SURFACES)) 
    {
        Surfaces[surf_cnt].SurfaceType = AA_RenderTarget;
        Surfaces[surf_cnt].Deleted = false;
        Surfaces[surf_cnt].ReCreated = false;
        Surfaces[surf_cnt].pSurf = pBuff;
        Surfaces[surf_cnt++].Surf = *pBuff;
        
    }
}    

void CSurfaceDebug::SetAAZBuffer(CSimpleSurface *pBuff)
{
    if (!bInitialized)
        Init();

    if (pBuff && (surf_cnt < CSD_MAX_SURFACES)) 
    {
        Surfaces[surf_cnt].SurfaceType = AA_Z_Buffer;
        Surfaces[surf_cnt].Deleted = false;
        Surfaces[surf_cnt].ReCreated = false;
        Surfaces[surf_cnt].pSurf = pBuff;
        Surfaces[surf_cnt++].Surf = *pBuff;
        
    }
}    

void CSurfaceDebug::DeleteSurface(CSimpleSurface *pBuff)
{
    for (int i = 0; i < surf_cnt; i++) 
    {
        if (Surfaces[i].pSurf == pBuff)
        {
            Surfaces[i].Deleted = true;
            break;
        }
    }
}    


// ToDo: Write this function!
void CSurfaceDebug::ValidateSurfacePlacement()
{
    return;
}    

void CSurfaceDebug::DumpCSimpleSurface(CSimpleSurface *p)
{
    DWORD bytes_written;
    DWORD flags;

    wsprintf(outbuf, "    LinAddr: 0x%08x\r\n", p->getAddress());
    NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
    wsprintf(outbuf, "    Offset: 0x%08x\r\n", p->getOffset());
    NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
    wsprintf(outbuf, "    Pitch: 0x%08x\r\n", p->getPitch());
    NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
    wsprintf(outbuf, "    Height: 0x%08x\r\n", p->getHeight());
    NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
    flags = p->getFlags();
    if (flags & CSimpleSurface::FLAG_TILED) 
    {
        wsprintf(outbuf, "    Tiled\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
    }
    if (flags & CSimpleSurface::FLAG_ZBUFFER) 
    {
        wsprintf(outbuf, "    ZBuffer\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
    }
    flags = p->getAllocFlags();
    if (flags & CSimpleSurface::ALLOCATE_SIMPLE) 
    {
        wsprintf(outbuf, "    ALLOCATE_SIMPLE\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
        
    }
    if (flags & CSimpleSurface::ALLOCATE_TILED) 
    {
        wsprintf(outbuf, "    ALLOCATE_TILED\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
        
    }
#if (NVARCH >= 0x20)
    if (flags & CSimpleSurface::ALLOCATE_AS_ZBUFFER_UNC) 
    {
        wsprintf(outbuf, "    ALLOCATE_AS_ZBUFFER_UNC\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
        
    }
    if (flags & CSimpleSurface::ALLOCATE_AS_ZBUFFER_CMP) 
    {
        wsprintf(outbuf, "    ALLOCATE_AS_ZBUFFER_CMP\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
        
    }
#endif 
    if (flags & CSimpleSurface::ALLOCATE_AS_ZBUFFER) 
    {
        wsprintf(outbuf, "    ALLOCATE_AS_ZBUFFER\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
        
    }
    flags = p->getRMAllocFlags();

    switch (flags)
    {
    case TYPE_TEXTURE:
        wsprintf(outbuf, "    TYPE_TEXTURE\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
       break;
    case TYPE_IMAGE:
        wsprintf(outbuf, "    TYPE_IMAGE\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
       break;
    case TYPE_DEPTH:
        wsprintf(outbuf, "    TYPE_DEPTH\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
       break;
#if (NVARCH >= 0x20)
    case TYPE_DEPTH_COMPR16:
        wsprintf(outbuf, "    TYPE_DEPTH_COMPR16\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
       break;
    case TYPE_DEPTH_COMPR32:
        wsprintf(outbuf, "    TYPE_DEPTH_COMPR32\r\n");
        NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
       break;
#endif 
    default:
        break;
    }

}    

void CSurfaceDebug::DumpSurfaceInfo()
{
    static char *SurfName[] =
    {
        "Front Buffer",
        "Back Buffer",
        "Z Buffer",
        "AA Render Target",
        "AA Z Buffer"
    };
    DWORD bytes_written;
    static char filetemp[] = "\\Surf%03i.txt";
    wsprintf(outbuf, filetemp, FileNum++);
    CSDFile=NvCreateFile(outbuf,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (CSDFile > 0) 
        bFileOpen = true;
    else 
        bFileOpen = false;
    
    if (bFileOpen)
    {
        

        for (int i = 0 ; i < surf_cnt; i++)
        {
            wsprintf(outbuf, "%s:\r\n", SurfName[Surfaces[i].SurfaceType]);
            NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
            if (Surfaces[i].Deleted) 
            {
                wsprintf(outbuf, "*** Deleted Surface:\r\n");
                NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
            }
            if (Surfaces[i].ReCreated) 
            {
                wsprintf(outbuf, "*** Recreated Surface:\r\n");
                NvWriteFile(CSDFile, outbuf, strlen(outbuf), &bytes_written, 0);
            }
            DumpCSimpleSurface(&Surfaces[i].Surf);
            
        }

    }
    
    if (bFileOpen)
    {
        if (!NvCloseHandle (CSDFile)) 
        {
            __asm int 3;
        }
        CSDFile = 0;
        bFileOpen = false;
    }
}    

void CSurfaceDebug::REG_WR32(DWORD addr, DWORD data)
{
    pHWRegisters[addr] = data;    
}    

void CSurfaceDebug::REG_RD32(DWORD addr, DWORD &data)
{
    data = pHWRegisters[addr];    
}    

void CSurfaceDebug::RDI_REG_WR32(DWORD select, DWORD addr, DWORD data)
{
//     REG_WR32(NV_PGRAPH_RDI_INDEX,       
//              DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, select) |    
//              DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, addr));     
//     REG_WR32(NV_PGRAPH_RDI_DATA, data);                         
    
}    

void CSurfaceDebug::RDI_REG_RD32(DWORD select, DWORD addr, DWORD &data)
{
//     REG_WR32(NV_PGRAPH_RDI_INDEX,       
//              DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, select) |    
//              DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, addr));     
//     data = REG_RD32(NV_PGRAPH_RDI_DATA);                        
    
}    
#endif // DEBUG_SURFACE_PLACEMENT
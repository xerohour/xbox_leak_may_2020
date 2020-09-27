/*
 * nvPatchUtilInit.c
 *
 * Software emulation for curved surfaces.
 *
 * Copyright (c) 2000, Nvidia Corporation.  All rights reserved.
 */

#if !defined(IS_OPENGL)
#include "nvprecomp.h"
#endif

#if !defined(_WIN64)
#include "x86.h"
#endif

//FOWARD DECLARATIONS ========================================================================

PFOFSTM CompileOffsetFDMatrix2x2(NV_PATCH_INFO *info);
PFOFSTM CompileOffsetFDMatrix4x4(NV_PATCH_INFO *info);
PFOFSTM CompileOffsetFDMatrix6x6(NV_PATCH_INFO *info);
PFOFSTMED CompileOffsetFDMatrixExtractDiscard2x2(NV_PATCH_INFO *info);
PFOFSTMED CompileOffsetFDMatrixExtractDiscard4x4(NV_PATCH_INFO *info);
PFOFSTMED CompileOffsetFDMatrixExtractDiscard6x6(NV_PATCH_INFO *info);
PFOFSC CompileOffsetCurve2x2(NV_PATCH_INFO *info);
PFOFSC CompileOffsetCurve4x4(NV_PATCH_INFO *info);
PFOFSC CompileOffsetCurve6x6(NV_PATCH_INFO *info);
PFMM MatrixMult2x2(NV_PATCH_INFO *info);
PFMM MatrixMult4x4(NV_PATCH_INFO *info);
PFMM MatrixMult6x6(NV_PATCH_INFO *info);

//FD UNIT GLOBAL DATA ========================================================================
/*int bForceSoftware = 0;
void *HOSInnerLoops[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned int RetessFDtag[2][2] = {0x3f800000, 0, 0x3f800000, 0};     //[slot][0=ratio/ 1=order]
FDMatrix *RetessFDCache[2] = {NULL,NULL};                            //ptrs to cached FDMatrices
int RetessFDMRU = 0;                                                 //most recently used cache slot*/
NV_PATCH_GLOBAL_DATA g_FDGlobalData = DEFAULT_PATCH_GLOBAL_DATA;

//file scope global variables used by x86.h macros to setup inner loops
static unsigned char *ilcData;
static unsigned int ilcCount = 0x0;
static unsigned int ilcMax = 0x0;

//INIT THE FD UNIT ========================================================================
#ifdef DCR_SEMAPHORE
void InitSemaphoreData(NV_PATCH_INFO *info);
#endif
void nvPatchInitFD(NV_PATCH_INFO *info)
{
#if !defined(IS_OPENGL)
    //init pCache to NULL
    int i;
    for(i=0; i < MAX_EV_CACHE; i++){ if(info->pCache[i]) memset(info->pCache[i],0,sizeof(NV_PATCH_ALLOC_CACHE)); }

#endif

    g_FDGlobalData.cpuType = 0x0;
#if !defined(_WIN64)
    if(info->cpuType & FS_KATMAI)
    {
        g_FDGlobalData.cpuType = FS_KATMAI;
        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_ED_2x2] = (void *) CompileOffsetFDMatrixExtractDiscard2x2(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_ED_4x4] = (void *) CompileOffsetFDMatrixExtractDiscard4x4(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_ED_6x6] = (void *) CompileOffsetFDMatrixExtractDiscard6x6(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_ED_DEF] = (void *) 0x0;

        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_2x2] =    (void *) CompileOffsetFDMatrix2x2(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_4x4] =    (void *) CompileOffsetFDMatrix4x4(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_6x6] =    (void *) CompileOffsetFDMatrix6x6(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_FDMATRIX_DEF] =    (void *) 0x0;

        g_FDGlobalData.HOSInnerLoops[OFFSET_CURVE_2x2] = (void *) CompileOffsetCurve2x2(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_CURVE_4x4] = (void *) CompileOffsetCurve4x4(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_CURVE_6x6] = (void *) CompileOffsetCurve6x6(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_CURVE_DEF] =  (void *) 0x0;

        g_FDGlobalData.HOSInnerLoops[OFFSET_MATRIX_MULT_2x2] = (void *) MatrixMult2x2(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_MATRIX_MULT_4x4] = (void *) MatrixMult4x4(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_MATRIX_MULT_6x6] = (void *) MatrixMult6x6(info);
        g_FDGlobalData.HOSInnerLoops[OFFSET_MATRIX_MULT_DEF] = 0x0;
    }
    else if(info->cpuType & FS_3DNOW)
    {
        //turn off optimized assembly routines -- I haven't written any 3DNOW stuff
        g_FDGlobalData.cpuType = 0x0;
    }
    else{
        //turn off optimized assembly routines if we don't have KATMAI support
        g_FDGlobalData.cpuType = 0x0;
    }    
#endif

#ifdef DCR_SEMAPHORE
    InitSemaphoreData(info);    
#else
    g_FDGlobalData.bForceSoftware = 0;
#endif

    return;
}

#ifdef DCR_SEMAPHORE
#define PAGE_SIZE   0x00001000
void InitSemaphoreData(NV_PATCH_INFO *info){        
    unsigned long dwSize = 2*PAGE_SIZE;    
    unsigned long dwRootHandle, dwDeviceHandle;

#if defined(IS_OPENGL)
    dwRootHandle = 0;
    dwDeviceHandle = 0;
    nvAssert(0);
#else
    dwRootHandle = ((PNVD3DCONTEXT)info->context)->pDriverData->dwRootHandle;
    dwDeviceHandle = ((PNVD3DCONTEXT)info->context)->pDriverData->dwDeviceHandle;
#endif
 
    dwSize--;
    //setup up mutual exclusion area for FD      
    if(NvRmAllocMemory(dwRootHandle, dwDeviceHandle, 
                        FD_SEMAPHORE_HANDLE, 
                        NV01_MEMORY_SYSTEM,
                        (NVOS02_FLAGS_PHYSICALITY_NONCONTIGUOUS << 4) | 
                        (NVOS02_FLAGS_LOCATION_PCI << 8) | 
                        (NVOS02_FLAGS_COHERENCY_CACHED << 12),
                        (void**)&g_FDGlobalData.baseAllocation,
                        &(dwSize)
                        )){
        g_FDGlobalData.bForceSoftware = 1;
        return;
    }

    //FORCE ALIGNMENT TO A PAGE BOUNDARY FOR SEMAPHORES.
    g_FDGlobalData.alignedAllocation = (BYTE*)((uintptr_t)(g_FDGlobalData.baseAllocation + PAGE_SIZE - 1) & ~(PAGE_SIZE-1));
    g_FDGlobalData.FDSynchCounter = (long *)g_FDGlobalData.alignedAllocation;
    memset(g_FDGlobalData.alignedAllocation,0x0,PAGE_SIZE);

    // map a context dma to the page of semaphores
    if(NvRmAllocContextDma(dwRootHandle,
                        FD_SEMAPHORE_206E_CONTEXT_DMA,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                        (void*) &(g_FDGlobalData.alignedAllocation),
                        PAGE_SIZE-1)){
        g_FDGlobalData.bForceSoftware = 1;
        return;
    }
    
    if(NvRmAllocContextDma(dwRootHandle,
                        FD_SEMAPHORE_097_CONTEXT_DMA,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                        (void*) &(g_FDGlobalData.alignedAllocation),
                        PAGE_SIZE-1)){
        g_FDGlobalData.bForceSoftware = 1;
        return;
    }

    // success
    g_FDGlobalData.bForceSoftware = 0;
    return;
}
#undef PAGE_SIZE
#endif

//TEAR DOWN THE FD UNIT ========================================================================
#if !defined(IS_OPENGL)
void nvPatchDestroyFD()
{
    int i;

#ifdef DCR_SEMAPHORE
#if defined(IS_OPENGL)
    unsigned long dwRootHandle, dwDeviceHandle;
    dwRootHandle = 0;
    dwDeviceHandle = 0;
    nvAssert(0);
#else
    unsigned long dwRootHandle, dwDeviceHandle;
//    dwRootHandle = ((PNVD3DCONTEXT)info->context)->pDriverData->dwRootHandle;
//    dwDeviceHandle = ((PNVD3DCONTEXT)info->context)->pDriverData->dwDeviceHandle;
    dwRootHandle = pDriverData->dwRootHandle;
    dwDeviceHandle = pDriverData->dwDeviceHandle;
#endif
#endif

    if(g_FDGlobalData.bFirstTimeInit){ return; } //we've never initialized FD so ignore this call

    //delete HOS inner loops and Retess cache data
    for(i = 0; i < 16; i++){
        if(g_FDGlobalData.HOSInnerLoops[i]){
            delete (unsigned char *)g_FDGlobalData.HOSInnerLoops[i];
            g_FDGlobalData.HOSInnerLoops[i] = NULL;
        }
    }
    for(i=0; i < MAX_EV_CACHE; i++){
        if(MyCache[i].pBaseCache){ delete MyCache[i].pBaseCache; }
        MyCache[i].pBaseCache = NULL;
        MyCache[i].bytesAllocCache = 0;
        MyCache[i].inUse = 0;
        MyCache[i].contextCache = NULL;
        MyCache[i].countFreeCheck = 0;
    }
    g_FDGlobalData.RetessFDtag[0][0] = (unsigned int)0x3f800000;    g_FDGlobalData.RetessFDtag[0][1] = 0;
    g_FDGlobalData.RetessFDtag[1][0] = (unsigned int)0x3f800000;    g_FDGlobalData.RetessFDtag[1][1] = 0;
    if(g_FDGlobalData.RetessFDCache[0]){ delete g_FDGlobalData.RetessFDCache[0]; g_FDGlobalData.RetessFDCache[0] = NULL; }
    if(g_FDGlobalData.RetessFDCache[1]){ delete g_FDGlobalData.RetessFDCache[1]; g_FDGlobalData.RetessFDCache[1] = NULL; }

    g_FDGlobalData.RetessFDCache[0] = NULL;  //ptrs to cached FDMatrices
    g_FDGlobalData.RetessFDCache[1] = NULL;  //ptrs to cached FDMatrices

#ifdef DCR_SEMAPHORE
    NvRmFree (dwRootHandle, dwDeviceHandle, FD_SEMAPHORE_HANDLE );
    NvRmFree (dwRootHandle, dwDeviceHandle, FD_SEMAPHORE_097_CONTEXT_DMA );
    NvRmFree (dwRootHandle, dwDeviceHandle, FD_SEMAPHORE_206E_CONTEXT_DMA );
#endif

    g_FDGlobalData.bFirstTimeInit = 1;   
}
#endif

#if !defined(_WIN64)
//EXTRACT DISCARD ROUTINES ========================================================================
#define BOFFSET    0xc
#define AOFFSET    0x8
#define MOFFSET    0x4
PFOFSTMED CompileOffsetFDMatrixExtractDiscard2x2(NV_PATCH_INFO *info)
{
    return (PFOFSTMED)(0x0);
}

PFOFSTMED CompileOffsetFDMatrixExtractDiscard4x4(NV_PATCH_INFO *info)
{
    unsigned int loop, extract_v, finished, finished2, finished3, skip_rows, skip_columns;
    unsigned int lEntry;

    ilcData = (unsigned char *)__NV_MALLOC(info->context, 1024);
    if(ilcData == 0x0) return (PFOFSTMED)NULL;    
    ilcCount = 0;    ilcMax = 1024;

    while(ilcCount &31)
    {
        xINT3;
    }
    
    //IF FDMatrix format changes this MUST be changed also.
    nvAssert(sizeof(FDMatrix) == (FDMATRIX_HEADER_SIZE + 16*16*4*sizeof(float)));
    xLABEL(lEntry)

    //void (FASTCALL *PFOFSTMED)(NV_PATCH_CURVE_INFO *dst, int dir, FDMatrix *m, int a, int b);
    //for function above stack frame will look like 
    //      ECX                                         dst            
    //      EDX                                         dir             
    
    xTEST_r_rm          (rEDX, rmREG(rEDX))
    xLABEL(extract_v)
    xJNZ32(0)
    //process as though I were extracting a row
        xMOV_r_rm       (rEAX, rmSIB8) xSIB8_esp(MOFFSET)      //EAX = m
        xADD_rm_imm     (rmREG(rEAX), FDMATRIX_HEADER_SIZE)    //#row/cols data members skipped  ECX=m->data[0][0][0];
        xMOV_rm_imm     (rmIND(rECX), 0x4)                     //fill in the #coeffs in the extracted curve
        xADD_rm_imm     (rmREG(rECX), PATCH_CURVE_HEADER_SIZE) //#coeffs data members skipped  ECX=dst->data;
               
        xMOVLPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(0)     //fetch first two columns
        xMOVHPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(8)
        xMOVLPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(16)
        xMOVHPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(24)        

        xMOV_r_rm       (rEDX, rmSIB8) xSIB8_esp(BOFFSET)   //EDX = b
        xPUSH_r         (rEDX)                              //save off loop var I'll need it again.
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xLABEL(skip_columns)   
        xJZ32(0)

        xMOVLPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(256)   //fetch rest of values if I'm actually going to use them.
        xMOVHPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(264)
        xMOVLPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(272)
        xMOVHPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(280)        
        xMOVLPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(512)
        xMOVHPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(520)
        xMOVLPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(528)
        xMOVHPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(536)
        xMOVLPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(768)
        xMOVHPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(776)
        xMOVLPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(784)
        xMOVHPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(792)
        xADD_rm_imm     (rmREG(rEAX),32)                    //skip to next two columns
        
        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
            xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
            xTEST_r_rm      (rEDX,rmREG(rEDX))
            xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
            xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
        xJNZ(loop)
        
        xTARGET_b32     (skip_columns)                      //we skipped columns       
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(0)      //save off intermediate results
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(8)      
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(16)      
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(24)     

        xMOVLPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(0)     //fetch next two columns
        xMOVHPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(8)
        xMOVLPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(16)
        xMOVHPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(24)        

        xPOP_r          (rEDX)
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xLABEL(skip_columns)   
        xJZ32(0)

        xMOVLPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(256)   //fetch rest of values if I'm actually going to use them.
        xMOVHPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(264)
        xMOVLPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(272)
        xMOVHPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(280)        
        xMOVLPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(512)
        xMOVHPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(520)
        xMOVLPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(528)
        xMOVHPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(536)
        xMOVLPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(768)
        xMOVHPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(776)
        xMOVLPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(784)
        xMOVHPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(792)

        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
            xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
            xTEST_r_rm      (rEDX,  rmREG(rEDX))             //loop over first two cols
            xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
            xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
        xJNZ(loop)
        
        xTARGET_b32     (skip_columns)                       //we skipped columns       
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(32)      //save off intermediate results
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(40)      
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(48)      
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(56)     


        xMOV_r_rm       (rEDX, rmSIB8) xSIB8_esp(AOFFSET)  //load EDX<-a
        xTEST_r_rm      (rEDX, rmREG(rEDX))                //skip primary loop if we don't do any work here
        xLABEL(finished2)   
        xJZ32(0)
        
        //fetch data to process from temporary storage...   
        xMOVLPS_r_rm    (rXMM0, rmIND8(rECX)) xOFS8(0)   //only process the row we are interested in
        xMOVHPS_r_rm    (rXMM0, rmIND8(rECX)) xOFS8(8)      
        xMOVLPS_r_rm    (rXMM1, rmIND8(rECX)) xOFS8(16)      
        xMOVHPS_r_rm    (rXMM1, rmIND8(rECX)) xOFS8(24)     
        xMOVLPS_r_rm    (rXMM2, rmIND8(rECX)) xOFS8(32)  
        xMOVHPS_r_rm    (rXMM2, rmIND8(rECX)) xOFS8(40)      
        xMOVLPS_r_rm    (rXMM3, rmIND8(rECX)) xOFS8(48)      
        xMOVHPS_r_rm    (rXMM3, rmIND8(rECX)) xOFS8(56)     

        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xTEST_r_rm      (rEDX, rmREG(rEDX))        //loop over row
            xADDPS_r_rm     (rXMM3, rmREG(rXMM3))               
        xJNZ(loop)

        xMOVLPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(0)      //save final results
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(8)      
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM1) xOFS8(16)      
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM1) xOFS8(24)     
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM2) xOFS8(32)     
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM2) xOFS8(40)      
        //xMOVLPS_rm_r    (rmIND8(rEAX), rXMM3) xOFS8(48)   //XMM3 should already have been stored off from before.
        //xMOVHPS_rm_r    (rmIND8(rEAX), rXMM3) xOFS8(56)     

    xLABEL(finished)
    xJMP(0)
    xTARGET_b32(extract_v)
        //process as though I were extracting a column
        xMOV_r_rm       (rEAX, rmSIB8) xSIB8_esp(MOFFSET)      //ECX = m
        xADD_rm_imm     (rmREG(rEAX), FDMATRIX_HEADER_SIZE)    //#row/cols data members skipped  ECX=m->data[0][0][0];
        xMOV_rm_imm     (rmIND(rECX), 0x4)                     //fill in the #coeffs in the extracted curve
        xADD_rm_imm     (rmREG(rECX), PATCH_CURVE_HEADER_SIZE) //#coeffs data members skipped  ECX=dst->data;

        xMOVLPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(0)     //fetch first two rows
        xMOVHPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(8)
        xMOVLPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(256)
        xMOVHPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(264)        

        xMOV_r_rm       (rEDX, rmSIB8) xSIB8_esp(AOFFSET)   //EDX = a
        xPUSH_r         (rEDX)                              //save off loop var I'll need it again.        
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xLABEL(skip_rows)   
        xJZ32(0)

        xMOVLPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(16)   //fetch rest of values if I'm actually going to use them.
        xMOVHPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(24)
        xMOVLPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(272)
        xMOVHPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(280)        
        xMOVLPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(32)
        xMOVHPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(40)
        xMOVLPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(288)
        xMOVHPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(296)
        xMOVLPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(48)
        xMOVHPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(56)
        xMOVLPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(304)
        xMOVHPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(312)
        xADD_rm_imm     (rmREG(rEAX),512)                    //skip to next two rows
        
        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
            xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
            xTEST_r_rm      (rEDX,rmREG(rEDX))
            xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
            xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
        xJNZ(loop)
        
        xTARGET_b32     (skip_rows)                         //we skipped rows
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(0)      //save off intermediate results
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(8)      
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(16)      
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(24)     

        xMOVLPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(0)     //fetch first two rows
        xMOVHPS_r_rm    (rXMM0,rmIND32(rEAX)) xOFS32(8)
        xMOVLPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(256)
        xMOVHPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32(264)        

        xPOP_r          (rEDX)
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xLABEL(skip_rows)   
        xJZ32(0)

        xMOVLPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(16)   //fetch rest of values if I'm actually going to use them.
        xMOVHPS_r_rm    (rXMM1,rmIND32(rEAX)) xOFS32(24)
        xMOVLPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(272)
        xMOVHPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32(280)        
        xMOVLPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(32)
        xMOVHPS_r_rm    (rXMM2,rmIND32(rEAX)) xOFS32(40)
        xMOVLPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(288)
        xMOVHPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32(296)
        xMOVLPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(48)
        xMOVHPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32(56)
        xMOVLPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(304)
        xMOVHPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32(312)

        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
            xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
            xTEST_r_rm      (rEDX,  rmREG(rEDX))            //loop over first two colsdd 
            xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
            xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
        xJNZ(loop)
        
        xTARGET_b32     (skip_rows)                         //we skipped rows
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(32)     //save off intermediate results
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(40)      
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(48)      
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM4) xOFS8(56)     

        xMOV_r_rm       (rEDX, rmSIB8) xSIB8_esp(BOFFSET)
        xTEST_r_rm      (rEDX, rmREG(rEDX))                 //skip primary loop if we don't do any work here
        xLABEL(finished3)   
        xJZ32(0)
        
        //fetch data to process from temporary storage...   
        xMOVLPS_r_rm    (rXMM0, rmIND8(rECX)) xOFS8(0)      //only process the row we are interested in
        xMOVHPS_r_rm    (rXMM0, rmIND8(rECX)) xOFS8(8)      
        xMOVLPS_r_rm    (rXMM1, rmIND8(rECX)) xOFS8(16)      
        xMOVHPS_r_rm    (rXMM1, rmIND8(rECX)) xOFS8(24)     
        xMOVLPS_r_rm    (rXMM2, rmIND8(rECX)) xOFS8(32)  
        xMOVHPS_r_rm    (rXMM2, rmIND8(rECX)) xOFS8(40)      
        //xMOVLPS_r_rm    (rXMM3, rmIND8(rECX)) xOFS8(48)      
        //xMOVHPS_r_rm    (rXMM3, rmIND8(rECX)) xOFS8(56)     

        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xTEST_r_rm      (rEDX, rmREG(rEDX))             //loop over row
            xADDPS_r_rm     (rXMM3, rmREG(rXMM3))               
        xJNZ(loop)

        xMOVLPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(0)      //save final results
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM0) xOFS8(8)      
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM1) xOFS8(16)      
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM1) xOFS8(24)     
        xMOVLPS_rm_r    (rmIND8(rECX), rXMM2) xOFS8(32)     
        xMOVHPS_rm_r    (rmIND8(rECX), rXMM2) xOFS8(40)      
        //xMOVLPS_rm_r    (rmIND8(rEAX), rXMM3) xOFS8(48)   //XMM3 should already have been stored off from before.
        //xMOVHPS_rm_r    (rmIND8(rEAX), rXMM3) xOFS8(56)     

    xTARGET_jmp(finished)
    xTARGET_b32(finished2)
    xTARGET_b32(finished3)
    //save final data

    xRET_imm            (0xc)                           //pop off 10 words for the b,a,m,dir that was pushed via fast call.
    nvAssert(ilcCount < ilcMax);
    return (PFOFSTMED)(ilcData + lEntry);
}


PFOFSTMED CompileOffsetFDMatrixExtractDiscard6x6(NV_PATCH_INFO *info)
{
    return (PFOFSTMED)(0x0);
}


//OFFSET FDMATRIX ROUTINES ========================================================================
PFOFSTM CompileOffsetFDMatrix2x2(NV_PATCH_INFO *info)
{
    unsigned int loop, skip_rows, skip_cols;

    ilcData = (unsigned char *)__NV_MALLOC(info->context, 512);
    if(ilcData == 0x0) return (PFOFSTM)NULL;    
    ilcCount = 0;    ilcMax = 512;

    while(ilcCount &31)
    {
        xINT3;
    }

    //void (FASTCALL *PFOFSTM)(FDMatrix *m, int a, int b);
    //for function above stack frame will look like 
    //      ECX                                         m            
    //      EDX                                         a             
    //      xMOV_r_rm       (rECX, rmSIB8) xSIB8_esp(0x4)   //b
        
    xADD_rm_imm     (rmREG(rECX), FDMATRIX_HEADER_SIZE) //#row/cols data members skipped  ECX=m->data[0][0][0];

    xMOVLPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(0)         xMOVHPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(8)
    xMOVLPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(16)        xMOVHPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(24)
    xMOVLPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(32)        xMOVHPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(40)
    xMOVLPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(48)        xMOVHPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(56)

    xTEST_r_rm(rEDX, rmREG(rEDX))
    xLABEL(skip_rows)
    xJZ32(0)
    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)
        xADDPS_r_rm     (rXMM2, rmREG(rXMM3))    
        xTEST_r_rm      (rEDX,rmREG(rEDX))
    xJNZ(loop)
    
    xTARGET_b32(skip_rows)

    xTEST_r_rm(rEDX, rmREG(rEDX))
    xLABEL(skip_cols)
    xJZ32(0)
    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM2))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)
        xADDPS_r_rm     (rXMM1, rmREG(rXMM3))    
        xTEST_r_rm      (rEDX,rmREG(rEDX))
    xJNZ(loop)
    
    xTARGET_b32(skip_cols)

    xMOVLPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(0)         xMOVHPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(16)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(24)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(32)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(40)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(48)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(56)

    return (PFOFSTM)(0x0);
}

PFOFSTM CompileOffsetFDMatrix4x4(NV_PATCH_INFO *info)
{
    unsigned int loop, skip_rows, skip_cols;
    unsigned int lEntry;

    ilcData = (unsigned char *)__NV_MALLOC(info->context, 1280);
    if(ilcData == 0x0) return (PFOFSTM)NULL;    
    ilcCount = 0;    ilcMax = 1280;

    while(ilcCount &31)
    {
        xINT3;
    }
    
    //IF FDMatrix format changes this MUST be changed also.
    nvAssert(sizeof(FDMatrix) == (FDMATRIX_HEADER_SIZE + 16*16*4*sizeof(float)));
    xLABEL(lEntry)

    //void (FASTCALL *PFOFSTM)(FDMatrix *m, int a, int b);
    //for function above stack frame will look like 
    //      ECX                                         m            
    //      EDX                                         a             
    //      xMOV_r_rm       (rECX, rmSIB8) xSIB8_esp(0x4)   //b
        
    xADD_rm_imm     (rmREG(rECX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  ECX=m->data[0][0][0];
    xTEST_r_rm(rEDX, rmREG(rEDX))
    xLABEL(skip_rows);
    xJZ32(0)
    xPUSH_r         (rEDX)                              //save this for later.
    //fetch first two rows
    xMOVLPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(0)         xMOVHPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(8)
    xMOVLPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(16)        xMOVHPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(24)
    xMOVLPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(32)        xMOVHPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(40)
    xMOVLPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(48)        xMOVHPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(56)
    xMOVLPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(256+0)     xMOVHPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(256+8)
    xMOVLPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(256+16)    xMOVHPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(256+24)
    xMOVLPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(256+32)    xMOVHPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(256+40)
    xMOVLPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(256+48)    xMOVHPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(256+56)        
    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
        xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)
        xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
        xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
        xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
    xJNZ(loop)
    //save results 
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(0)         xMOVHPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(16)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(24)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(32)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(40)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(48)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(56)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(256+0)     xMOVHPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(256+8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(256+16)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(256+24)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(256+32)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(256+40)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(256+48)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(256+56)                    
    
    xADD_rm_imm     (rmREG(rECX), 512)                    //skip to next two rows
    xPOP_r          (rEDX)    
    
    //fetch second two rows
    xMOVLPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(0)         xMOVHPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(8)
    xMOVLPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(16)        xMOVHPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(24)
    xMOVLPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(32)        xMOVHPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(40)
    xMOVLPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(48)        xMOVHPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(56)
    xMOVLPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(256+0)     xMOVHPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(256+8)
    xMOVLPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(256+16)    xMOVHPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(256+24)
    xMOVLPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(256+32)    xMOVHPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(256+40)
    xMOVLPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(256+48)    xMOVHPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(256+56)        
    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
        xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)
        xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
        xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
        xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
    xJNZ(loop)
    //save results 
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(0)         xMOVHPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(16)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(24)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(32)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(40)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(48)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(56)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(256+0)     xMOVHPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(256+8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(256+16)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(256+24)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(256+32)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(256+40)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(256+48)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(256+56)                        
    xADD_rm_imm     (rmREG(rECX), -512) //move back to beginning
    
    xTARGET_b32(skip_rows)    

    xMOV_r_rm       (rEDX, rmSIB8) xSIB8_esp(0x4)   //EDX <-- b
    xTEST_r_rm      (rEDX,rmREG(rEDX))    
    xLABEL(skip_cols)   
    xJZ32(0)
    
    xPUSH_r         (rEDX)
    //fetch first two cols
    xMOVLPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(0)         xMOVHPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(8)
    xMOVLPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(256)       xMOVHPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(264)
    xMOVLPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(512)       xMOVHPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(520)
    xMOVLPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(768)       xMOVHPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(776)
    xMOVLPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(16+0)      xMOVHPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(16+8)
    xMOVLPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(16+256)    xMOVHPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(16+264)
    xMOVLPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(16+512)    xMOVHPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(16+520)
    xMOVLPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(16+768)    xMOVHPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(16+776)        
    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
        xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)
        xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
        xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
        xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
    xJNZ(loop)
    //save results 
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(0)         xMOVHPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(256)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(264)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(512)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(520)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(768)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(776)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(16+0)      xMOVHPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(16+8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(16+256)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(16+264)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(16+512)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(16+520)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(16+768)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(16+776)        
    
    xADD_rm_imm     (rmREG(rECX), 32)                    //skip to next two rows
    xPOP_r          (rEDX)    

    xMOVLPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(0)         xMOVHPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(8)
    xMOVLPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(256)       xMOVHPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(264)
    xMOVLPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(512)       xMOVHPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(520)
    xMOVLPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(768)       xMOVHPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(776)
    xMOVLPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(16+0)      xMOVHPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(16+8)
    xMOVLPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(16+256)    xMOVHPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(16+264)
    xMOVLPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(16+512)    xMOVHPS_r_rm    (rXMM6,rmIND32(rECX)) xOFS32(16+520)
    xMOVLPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(16+768)    xMOVHPS_r_rm    (rXMM7,rmIND32(rECX)) xOFS32(16+776)        
    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
        xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)
        xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
        xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xADDPS_r_rm     (rXMM5, rmREG(rXMM6))
        xADDPS_r_rm     (rXMM6, rmREG(rXMM7))
    xJNZ(loop)
    //save results 
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(0)         xMOVHPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(256)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(264)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(512)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(520)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(768)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(776)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(16+0)      xMOVHPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(16+8)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(16+256)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(16+264)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(16+512)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM6) xOFS32(16+520)
    xMOVLPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(16+768)    xMOVHPS_rm_r    (rmIND32(rECX),rXMM7) xOFS32(16+776)        

    xTARGET_b32(skip_cols)
    xRET_imm            (0x4)                           //pop off 4 words for the m,a,b that was pushed via fast call.
    nvAssert(ilcCount < ilcMax);
    return (PFOFSTM)(ilcData + lEntry);
}

PFOFSTM CompileOffsetFDMatrix6x6(NV_PATCH_INFO *info)
{
    unsigned int loop, outer_loop, skip_rows, skip_cols;
    unsigned int lEntry;

    ilcData = (unsigned char *)__NV_MALLOC(info->context, 1024);
    if(ilcData == 0x0) return (PFOFSTM)NULL;    
    ilcCount = 0;    ilcMax = 1024;

    while(ilcCount &31)
    {
        xINT3;
    }
    
    //IF FDMatrix format changes this MUST be changed also.
    nvAssert(sizeof(FDMatrix) == (FDMATRIX_HEADER_SIZE + 16*16*4*sizeof(float)));
    xLABEL(lEntry)

    //void (FASTCALL *PFOFSTM)(FDMatrix *m, int a, int b);
    //for function above stack frame will look like 
    //      ECX                                         m            
    //      EDX                                         a             
    //      xMOV_r_rm       (rECX, rmSIB8) xSIB8_esp(0x4)   //b
        
    xADD_rm_imm     (rmREG(rECX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  ECX=m->data[0][0][0];
    xTEST_r_rm(rEDX, rmREG(rEDX))
    xLABEL(skip_rows);
    xJZ32(0)
    xMOV_rm_imm     (rmREG(rEAX), 0x6)                  //how many rows to do.
    
    xLABEL(outer_loop)
        xSUB_rm_imm     (rmREG(rEAX), 0x1)
        xPUSH_r         (rEDX)                              //save this for later.
        //fetch a row
        xMOVLPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(0)         xMOVHPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(8)
        xMOVLPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(16)        xMOVHPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(24)
        xMOVLPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(32)        xMOVHPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(40)
        xMOVLPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(48)        xMOVHPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(56)
        xMOVLPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(64)        xMOVHPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(72)
        xMOVLPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(80)        xMOVHPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(88)
        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
            xADDPS_r_rm     (rXMM3, rmREG(rXMM4))
            xTEST_r_rm      (rEDX,rmREG(rEDX))
            xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
        xJNZ(loop)
        //save results 
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(0)         xMOVHPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(8)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(16)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(24)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(32)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(40)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(48)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(56)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(64)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(72)
        //xMOVLPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(80)        xMOVHPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(88)    
        xADD_rm_imm     (rmREG(rECX), 256)                      //skip to next row
        xPOP_r          (rEDX)        
    xTEST_r_rm      (rEAX,rmREG(rEAX))
    xJNZ32(outer_loop)
    
    xTARGET_b32(skip_rows)    

    xMOV_r_rm       (rEDX, rmSIB8) xSIB8_esp(0x4)   //EDX <-- b
    xTEST_r_rm      (rEDX,rmREG(rEDX))    
    xLABEL(skip_cols)   
    xJZ32(0)

    xMOV_rm_imm         (rmREG(rEAX), 0x6)                  //how many cols to do.    
    xLABEL(outer_loop)
        xSUB_rm_imm     (rmREG(rEAX), 0x1)    
        xPUSH_r         (rEDX)
        //fetch first two cols
        xMOVLPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(0)         xMOVHPS_r_rm    (rXMM0,rmIND32(rECX)) xOFS32(8)
        xMOVLPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(256)       xMOVHPS_r_rm    (rXMM1,rmIND32(rECX)) xOFS32(264)
        xMOVLPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(512)       xMOVHPS_r_rm    (rXMM2,rmIND32(rECX)) xOFS32(520)
        xMOVLPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(768)       xMOVHPS_r_rm    (rXMM3,rmIND32(rECX)) xOFS32(776)
        xMOVLPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(1024)      xMOVHPS_r_rm    (rXMM4,rmIND32(rECX)) xOFS32(1032)
        xMOVLPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(1280)      xMOVHPS_r_rm    (rXMM5,rmIND32(rECX)) xOFS32(1288)
        xLABEL(loop)
            xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
            xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
            xSUB_rm_imm     (rmREG(rEDX), 0x1)
            xADDPS_r_rm     (rXMM2, rmREG(rXMM3))               
            xADDPS_r_rm     (rXMM3, rmREG(rXMM4))
            xTEST_r_rm      (rEDX,rmREG(rEDX))
            xADDPS_r_rm     (rXMM4, rmREG(rXMM5))
        xJNZ(loop)
        //save results 
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(0)         xMOVHPS_rm_r    (rmIND32(rECX),rXMM0) xOFS32(8)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(256)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM1) xOFS32(264)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(512)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM2) xOFS32(520)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(768)       xMOVHPS_rm_r    (rmIND32(rECX),rXMM3) xOFS32(776)
        xMOVLPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(1024)      xMOVHPS_rm_r    (rmIND32(rECX),rXMM4) xOFS32(1032)
        //xMOVLPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(1280)      xMOVHPS_rm_r    (rmIND32(rECX),rXMM5) xOFS32(1288)    
        xADD_rm_imm     (rmREG(rECX), 16)                    //skip to next column
        xPOP_r          (rEDX)    
    xTEST_r_rm      (rEAX,rmREG(rEAX))
    xJNZ32(outer_loop)    
    xTARGET_b32(skip_cols)
    xRET_imm            (0x4)                           //pop off 4 words for the m,a,b that was pushed via fast call.
    
    nvAssert(ilcCount < ilcMax);
    return (PFOFSTM)(ilcData + lEntry);
}

//OFFSET CURVE ROUTINES ========================================================================
PFOFSC CompileOffsetCurve2x2(NV_PATCH_INFO *info)
{
    return (PFOFSC)(0x0);
}

PFOFSC CompileOffsetCurve4x4(NV_PATCH_INFO *info)
{
    unsigned int loop, finished, lEntry;

    ilcData = (unsigned char *)__NV_MALLOC(info->context, 512);
    if(ilcData == 0x0) return (PFOFSC)NULL;    
    ilcCount = 0;    ilcMax = 512;

    while(ilcCount &31)
    {
        xINT3;
    }

    nvAssert(sizeof(NV_PATCH_CURVE_INFO) == (PATCH_CURVE_HEADER_SIZE + 16*4*sizeof(float)));
    xLABEL(lEntry)

    //void (FASTCALL *PFOFSTM)(NV_PATCH_CURVE *c, int n);
    //for function above stack frame will look like 
    //      ECX                                         c            
    //      EDX                                         n             
    xTEST_r_rm  (rEDX, rmREG(rEDX))
    xLABEL(finished)
    xJZ32(0)
    xMOVLPS_r_rm    (rXMM0,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+0)     //4+ skips the order member of the        
    xMOVHPS_r_rm    (rXMM0,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+8)     //curve struct
    xMOVLPS_r_rm    (rXMM1,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+16)       
    xMOVHPS_r_rm    (rXMM1,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+24)
    xMOVLPS_r_rm    (rXMM2,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+32)       
    xMOVHPS_r_rm    (rXMM2,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+40)
    xMOVLPS_r_rm    (rXMM3,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+48)       
    xMOVHPS_r_rm    (rXMM3,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+56)       
    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)
        xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xADDPS_r_rm     (rXMM2, rmREG(rXMM3))                   
    xJNZ(loop)
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM0) xOFS8(PATCH_CURVE_HEADER_SIZE+0)     //4+ skips the order member of the        
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM0) xOFS8(PATCH_CURVE_HEADER_SIZE+8)     //curve struct
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM1) xOFS8(PATCH_CURVE_HEADER_SIZE+16)       
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM1) xOFS8(PATCH_CURVE_HEADER_SIZE+24)
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM2) xOFS8(PATCH_CURVE_HEADER_SIZE+32)       
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM2) xOFS8(PATCH_CURVE_HEADER_SIZE+40)
    //xMOVLPS_rm_r    (rmIND8(rECX),rXMM3) xOFS8(4+48)  //don't save off last one     
    //xMOVHPS_rm_r    (rmIND8(rECX),rXMM3) xOFS8(4+56)       
 
    xTARGET_b32(finished)
    xRET_imm            (0x0)                           
    nvAssert(ilcCount < ilcMax);

    return (PFOFSC)(ilcData + lEntry);
}

PFOFSC CompileOffsetCurve6x6(NV_PATCH_INFO *info)
{
   unsigned int loop, finished, lEntry;

    ilcData = (unsigned char *)__NV_MALLOC(info->context, 512);
    if(ilcData == 0x0) return (PFOFSC)NULL;    
    ilcCount = 0;    ilcMax = 512;

    while(ilcCount &31)
    {
        xINT3;
    }
    nvAssert(sizeof(NV_PATCH_CURVE_INFO) == (PATCH_CURVE_HEADER_SIZE + 16*4*sizeof(float)));
    xLABEL(lEntry);

    //void (FASTCALL *PFOFSTM)(NV_PATCH_CURVE *c, int n);
    //for function above stack frame will look like 
    //      ECX                                         c            
    //      EDX                                         n             
    xTEST_r_rm  (rEDX, rmREG(rEDX))
    xLABEL(finished)
    xJZ32(0)
    xMOVLPS_r_rm    (rXMM0,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+0)     //4+ skips the order member of the        
    xMOVHPS_r_rm    (rXMM0,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+8)     //curve struct
    xMOVLPS_r_rm    (rXMM1,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+16)       
    xMOVHPS_r_rm    (rXMM1,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+24)
    xMOVLPS_r_rm    (rXMM2,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+32)       
    xMOVHPS_r_rm    (rXMM2,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+40)
    xMOVLPS_r_rm    (rXMM3,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+48)       
    xMOVHPS_r_rm    (rXMM3,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+56)       
    xMOVLPS_r_rm    (rXMM4,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+64)       
    xMOVHPS_r_rm    (rXMM4,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+72)       
    xMOVLPS_r_rm    (rXMM5,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+80)       
    xMOVHPS_r_rm    (rXMM5,rmIND8(rECX)) xOFS8(PATCH_CURVE_HEADER_SIZE+88)       

    xLABEL(loop)
        xADDPS_r_rm     (rXMM0, rmREG(rXMM1))    
        xADDPS_r_rm     (rXMM1, rmREG(rXMM2))    
        xSUB_rm_imm     (rmREG(rEDX), 0x1)   
        xADDPS_r_rm     (rXMM2, rmREG(rXMM3))                   
        xTEST_r_rm      (rEDX,rmREG(rEDX))
        xADDPS_r_rm     (rXMM3, rmREG(rXMM4))                   
        xADDPS_r_rm     (rXMM4, rmREG(rXMM5))                   
    xJNZ(loop)
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM0) xOFS8(PATCH_CURVE_HEADER_SIZE+0)   //4+ skips the order member of the        
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM0) xOFS8(PATCH_CURVE_HEADER_SIZE+8)   //curve struct
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM1) xOFS8(PATCH_CURVE_HEADER_SIZE+16)       
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM1) xOFS8(PATCH_CURVE_HEADER_SIZE+24)
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM2) xOFS8(PATCH_CURVE_HEADER_SIZE+32)       
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM2) xOFS8(PATCH_CURVE_HEADER_SIZE+40)
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM3) xOFS8(PATCH_CURVE_HEADER_SIZE+48)  
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM3) xOFS8(PATCH_CURVE_HEADER_SIZE+56)       
    xMOVLPS_rm_r    (rmIND8(rECX),rXMM4) xOFS8(PATCH_CURVE_HEADER_SIZE+64)       
    xMOVHPS_rm_r    (rmIND8(rECX),rXMM4) xOFS8(PATCH_CURVE_HEADER_SIZE+72)   
    //xMOVLPS_rm_r    (rmIND8(rECX),rXMM5) xOFS8(4+80) //don't save off last one       
    //xMOVHPS_rm_r    (rmIND8(rECX),rXMM5) xOFS8(4+88)       
 
    xTARGET_b32(finished)
    xRET_imm            (0x0)                           
    nvAssert(ilcCount < ilcMax);

    return (PFOFSC)(ilcData + lEntry);
}

//MATRIX MULTIPLY ROUTINES ========================================================================
PFMM MatrixMult2x2(NV_PATCH_INFO *info){
/*      x[1][1] = a00*b00+a01*b10;
        x[1][2] = a00*b01+a01*b11;
        x[2][1] = a10*b00+a11*b10;
        x[2][2] = a10*b01+a11*b11;      */
       return (PFMM)(NULL);
}


PFMM MatrixMult6x6(NV_PATCH_INFO *info)
{
    unsigned int lEntry = 0;

#if !defined(IS_OPENGL)
    ilcData = (unsigned char *)__NV_MALLOC(info->context, 8192);
    if(ilcData == 0x0) return (PFMM)NULL;    
    ilcCount = 0;    ilcMax = 8192;

    while(ilcCount &31){
        xINT3;
    }
    xLABEL(lEntry);

    //typedef void (FASTCALL *PFMM)(FDMatrix *dst, FDMatrix *a, FDMatrix *b);
    //for function above stack frame will look like 
    //      ECX                                             dst            
    //      EDX                                             a             
    //      xMOV_r_rm       (rECX, rmSIB8) xSIB8_esp(0x4)   b
    
    xMOV_r_rm       (rEAX, rmSIB8) xSIB8_esp(0x4)           //EAX<-b
    xADD_rm_imm     (rmREG(rEAX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  EAX=b->data[0][0][0];
    xADD_rm_imm     (rmREG(rECX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  ECX=dst->data[0][0][0];
    xADD_rm_imm     (rmREG(rEDX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  EDX=a->data[0][0][0];    

//get first half row vector of a
#define GET_ROWa(a) {                                                               \
                    xMOVLPS_r_rm    (rXMM0,rmIND32(rEDX)) xOFS32((256*a+0));        \
                    xMOVHPS_r_rm    (rXMM0,rmIND32(rEDX)) xOFS32((256*a+8));        \
                    xMOVLPS_r_rm    (rXMM1,rmIND32(rEDX)) xOFS32((256*a+16));       \
                    xMOVHPS_r_rm    (rXMM1,rmIND32(rEDX)) xOFS32((256*a+24));       \
                    xMOVLPS_r_rm    (rXMM2,rmIND32(rEDX)) xOFS32((256*a+32));       \
                    xMOVHPS_r_rm    (rXMM2,rmIND32(rEDX)) xOFS32((256*a+40));       \
                   }

#define GET_ROWb(a) {                                                               \
                    xMOVLPS_r_rm    (rXMM0,rmIND32(rEDX)) xOFS32((256*a+48));       \
                    xMOVHPS_r_rm    (rXMM0,rmIND32(rEDX)) xOFS32((256*a+56));       \
                    xMOVLPS_r_rm    (rXMM1,rmIND32(rEDX)) xOFS32((256*a+64));       \
                    xMOVHPS_r_rm    (rXMM1,rmIND32(rEDX)) xOFS32((256*a+72));       \
                    xMOVLPS_r_rm    (rXMM2,rmIND32(rEDX)) xOFS32((256*a+80));       \
                    xMOVHPS_r_rm    (rXMM2,rmIND32(rEDX)) xOFS32((256*a+88));       \
                   }

//get first half row vector of a
#define GET_COLa(a) {                                                              \
                    xMOVLPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32((16*a+0));        \
                    xMOVHPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32((16*a+8));        \
                    xMOVLPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32((16*a+256));      \
                    xMOVHPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32((16*a+264));      \
                    xMOVLPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32((16*a+512));      \
                    xMOVHPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32((16*a+520));      \
                   }

//get first half row vector of a
#define GET_COLb(a) {                                                              \
                    xMOVLPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32((16*a+768));      \
                    xMOVHPS_r_rm    (rXMM3,rmIND32(rEAX)) xOFS32((16*a+776));      \
                    xMOVLPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32((16*a+1024));     \
                    xMOVHPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32((16*a+1032));     \
                    xMOVLPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32((16*a+1280));     \
                    xMOVHPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32((16*a+1288));     \
                   }

#define CROSSa() {                                                               \
                    xMULPS_r_rm     (rXMM3, rmREG(rXMM0));                       \
                    xMULPS_r_rm     (rXMM4, rmREG(rXMM1));                       \
                    xADDPS_r_rm     (rXMM3, rmREG(rXMM4));                       \
                    xMULPS_r_rm     (rXMM5, rmREG(rXMM2));                       \
                    xADDPS_r_rm     (rXMM3, rmREG(rXMM5));                       \
                    xMOVAPS_r_rm    (rXMM6, rmREG(rXMM3));                       \
                }

#define CROSSb() {                                                               \
                    xMULPS_r_rm     (rXMM3, rmREG(rXMM0));                       \
                    xMULPS_r_rm     (rXMM4, rmREG(rXMM1));                       \
                    xADDPS_r_rm     (rXMM3, rmREG(rXMM4));                       \
                    xMULPS_r_rm     (rXMM5, rmREG(rXMM2));                       \
                    xADDPS_r_rm     (rXMM3, rmREG(rXMM5));                       \
                    xMOVAPS_r_rm    (rXMM7, rmREG(rXMM3));                       \
                }

#define SAVE(a,b) {                                                              \
                    xADDPS_r_rm     (rXMM6, rmREG(rXMM7));                       \
                    xMOVLPS_rm_r    (rmIND32(rECX), rXMM6) xOFS32(256*a+16*b+0); \
                    xMOVHPS_rm_r    (rmIND32(rECX), rXMM6) xOFS32(256*a+16*b+8); \
                  }    
    
    // x[0][0] = a00*b00+a01*b10+a02*b20+a03*b30+a04*b40+a05*b50;
    GET_ROWa(0)    GET_COLa(0)    CROSSa()
    GET_ROWb(0)    GET_COLb(0)    CROSSb()
    SAVE(0,0)
    // x[0][1] = a00*b01+a01*b11+a02*b21+a03*b31+a04*b41+a05*b51;
    GET_ROWa(0)    GET_COLa(1)    CROSSa()
    GET_ROWb(0)    GET_COLb(1)    CROSSb()
    SAVE(0,1)
    // x[0][2] = a00*b02+a01*b12+a02*b22+a03*b32+a04*b42+a05*b52;
    GET_ROWa(0)    GET_COLa(2)    CROSSa()
    GET_ROWb(0)    GET_COLb(2)    CROSSb()
    SAVE(0,2)
    // x[0][3] = a00*b03+a01*b13+a02*b23+a03*b33+a04*b43+a05*b53;
    GET_ROWa(0)    GET_COLa(3)    CROSSa()
    GET_ROWb(0)    GET_COLb(3)    CROSSb()
    SAVE(0,3)
    // x[0][4] = a00*b04+a01*b14+a02*b24+a03*b34+a04*b44+a05*b54;
    GET_ROWa(0)    GET_COLa(4)    CROSSa()
    GET_ROWb(0)    GET_COLb(4)    CROSSb()
    SAVE(0,4)
    // x[0][5] = a00*b05+a01*b15+a02*b25+a03*b35+a04*b45+a05*b55;
    GET_ROWa(0)    GET_COLa(5)    CROSSa()
    GET_ROWb(0)    GET_COLb(5)    CROSSb()
    SAVE(0,5)
    // x[1][0] = a10*b00+a11*b10+a12*b20+a13*b30+a14*b40+a15*b50;
    GET_ROWa(1)    GET_COLa(0)    CROSSa()
    GET_ROWb(1)    GET_COLb(0)    CROSSb()
    SAVE(1,0)
    // x[1][1] = a10*b01+a11*b11+a12*b21+a13*b31+a14*b41+a15*b51;
    GET_ROWa(1)    GET_COLa(1)    CROSSa()
    GET_ROWb(1)    GET_COLb(1)    CROSSb()
    SAVE(1,1)
    // x[1][2] = a10*b02+a11*b12+a12*b22+a13*b32+a14*b42+a15*b52;
    GET_ROWa(1)    GET_COLa(2)    CROSSa()
    GET_ROWb(1)    GET_COLb(2)    CROSSb()
    SAVE(1,2)
    // x[1][3] = a10*b03+a11*b13+a12*b23+a13*b33+a14*b43+a15*b53;
    GET_ROWa(1)    GET_COLa(3)    CROSSa()
    GET_ROWb(1)    GET_COLb(3)    CROSSb()
    SAVE(1,3)
    // x[1][4] = a10*b04+a11*b14+a12*b24+a13*b34+a14*b44+a15*b54;
    GET_ROWa(1)    GET_COLa(4)    CROSSa()
    GET_ROWb(1)    GET_COLb(4)    CROSSb()
    SAVE(1,4)
    // x[1][5] = a10*b05+a11*b15+a12*b25+a13*b35+a14*b45+a15*b55;*/
    GET_ROWa(1)    GET_COLa(5)    CROSSa()
    GET_ROWb(1)    GET_COLb(5)    CROSSb()
    SAVE(1,5)

    // x[2][0] = a20*b00+a21*b10+a22*b20+a23*b30+a24*b40+a25*b50;
    GET_ROWa(2)    GET_COLa(0)    CROSSa()
    GET_ROWb(2)    GET_COLb(0)    CROSSb()
    SAVE(2,0)
    // x[2][1] = a20*b01+a21*b11+a22*b21+a23*b31+a24*b41+a25*b51;
    GET_ROWa(2)    GET_COLa(1)    CROSSa()
    GET_ROWb(2)    GET_COLb(1)    CROSSb()
    SAVE(2,1)
    // x[2][2] = a20*b02+a21*b12+a22*b22+a23*b32+a24*b42+a25*b52;
    GET_ROWa(2)    GET_COLa(2)    CROSSa()
    GET_ROWb(2)    GET_COLb(2)    CROSSb()
    SAVE(2,2)
    // x[2][3] = a20*b03+a21*b13+a22*b23+a23*b33+a24*b43+a25*b53;
    GET_ROWa(2)    GET_COLa(3)    CROSSa()
    GET_ROWb(2)    GET_COLb(3)    CROSSb()
    SAVE(2,3)
    // x[2][4] = a20*b04+a21*b14+a22*b24+a23*b34+a24*b44+a25*b54;
    GET_ROWa(2)    GET_COLa(4)    CROSSa()
    GET_ROWb(2)    GET_COLb(4)    CROSSb()
    SAVE(2,4)
    // x[2][5] = a20*b05+a21*b15+a22*b25+a23*b35+a24*b45+a25*b55;
    GET_ROWa(2)    GET_COLa(5)    CROSSa()
    GET_ROWb(2)    GET_COLb(5)    CROSSb()
    SAVE(2,5)

    // x[3][0] = a30*b00+a31*b10+a32*b20+a33*b30+a34*b40+a35*b50;
    GET_ROWa(3)    GET_COLa(0)    CROSSa()
    GET_ROWb(3)    GET_COLb(0)    CROSSb()
    SAVE(3,0)
    // x[3][1] = a30*b01+a31*b11+a32*b21+a33*b31+a34*b41+a35*b51;
    GET_ROWa(3)    GET_COLa(1)    CROSSa()
    GET_ROWb(3)    GET_COLb(1)    CROSSb()
    SAVE(3,1)
    // x[3][2] = a30*b02+a31*b12+a32*b22+a33*b32+a34*b42+a35*b52;
    GET_ROWa(3)    GET_COLa(2)    CROSSa()
    GET_ROWb(3)    GET_COLb(2)    CROSSb()
    SAVE(3,2)
    // x[3][3] = a30*b03+a31*b13+a32*b23+a33*b33+a34*b43+a35*b53;
    GET_ROWa(3)    GET_COLa(3)    CROSSa()
    GET_ROWb(3)    GET_COLb(3)    CROSSb()
    SAVE(3,3)
    // x[3][4] = a30*b04+a31*b14+a32*b24+a33*b34+a34*b44+a35*b54;
    GET_ROWa(3)    GET_COLa(4)    CROSSa()
    GET_ROWb(3)    GET_COLb(4)    CROSSb()
    SAVE(3,4)
    // x[3][5] = a30*b05+a31*b15+a32*b25+a33*b35+a34*b45+a35*b55;
    GET_ROWa(3)    GET_COLa(5)    CROSSa()
    GET_ROWb(3)    GET_COLb(5)    CROSSb()
    SAVE(3,5)

    // x[4][0] = a40*b00+a41*b10+a42*b20+a43*b30+a44*b40+a45*b50;
    GET_ROWa(4)    GET_COLa(0)    CROSSa()
    GET_ROWb(4)    GET_COLb(0)    CROSSb()
    SAVE(4,0)
    // x[4][1] = a40*b01+a41*b11+a42*b21+a43*b31+a44*b41+a45*b51;
    GET_ROWa(4)    GET_COLa(1)    CROSSa()
    GET_ROWb(4)    GET_COLb(1)    CROSSb()
    SAVE(4,1)
    // x[4][2] = a40*b02+a41*b12+a42*b22+a43*b32+a44*b42+a45*b52;
    GET_ROWa(4)    GET_COLa(2)    CROSSa()
    GET_ROWb(4)    GET_COLb(2)    CROSSb()
    SAVE(4,2)
    // x[4][3] = a40*b03+a41*b13+a42*b23+a43*b33+a44*b43+a45*b53;
    GET_ROWa(4)    GET_COLa(3)    CROSSa()
    GET_ROWb(4)    GET_COLb(3)    CROSSb()
    SAVE(4,3)
    // x[4][4] = a40*b04+a41*b14+a42*b24+a43*b34+a44*b44+a45*b54;
    GET_ROWa(4)    GET_COLa(4)    CROSSa()
    GET_ROWb(4)    GET_COLb(4)    CROSSb()
    SAVE(4,4)
    // x[4][5] = a40*b05+a41*b15+a42*b25+a43*b35+a44*b45+a45*b55;
    GET_ROWa(4)    GET_COLa(5)    CROSSa()
    GET_ROWb(4)    GET_COLb(5)    CROSSb()
    SAVE(4,5)

    // x[5][0] = a50*b00+a51*b10+a52*b20+a53*b30+a54*b40+a55*b50;
    GET_ROWa(5)    GET_COLa(0)    CROSSa()
    GET_ROWb(5)    GET_COLb(0)    CROSSb()
    SAVE(5,0)
    // x[5][1] = a50*b01+a51*b11+a52*b21+a53*b31+a54*b41+a55*b51;
    GET_ROWa(5)    GET_COLa(1)    CROSSa()
    GET_ROWb(5)    GET_COLb(1)    CROSSb()
    SAVE(5,1)
    // x[5][2] = a50*b02+a51*b12+a52*b22+a53*b32+a54*b42+a55*b52;
    GET_ROWa(5)    GET_COLa(2)    CROSSa()
    GET_ROWb(5)    GET_COLb(2)    CROSSb()
    SAVE(5,2)
    // x[5][3] = a50*b03+a51*b13+a52*b23+a53*b33+a54*b43+a55*b53;
    GET_ROWa(5)    GET_COLa(3)    CROSSa()
    GET_ROWb(5)    GET_COLb(3)    CROSSb()
    SAVE(5,3)
    // x[5][4] = a50*b04+a51*b14+a52*b24+a53*b34+a54*b44+a55*b54;
    GET_ROWa(5)    GET_COLa(4)    CROSSa()
    GET_ROWb(5)    GET_COLb(4)    CROSSb()
    SAVE(5,4)
    // x[5][5] = a50*b05+a51*b15+a52*b25+a53*b35+a54*b45+a55*b55;
    GET_ROWa(5)    GET_COLa(5)    CROSSa()
    GET_ROWb(5)    GET_COLb(5)    CROSSb()
    SAVE(5,5)
#endif

    xRET_imm            (0x4)                           
    nvAssert(ilcCount < ilcMax);

    return (PFMM)(ilcData + lEntry);

#undef GET_COLa
#undef GET_ROWa
#undef CROSSa
#undef GET_COLb
#undef GET_ROWb
#undef CROSSb
#undef SAVE

}

PFMM MatrixMult4x4(NV_PATCH_INFO *info){
    unsigned int lEntry;

    ilcData = (unsigned char *)__NV_MALLOC(info->context, 2048);
    if(ilcData == 0x0) return (PFMM)NULL;    
    ilcCount = 0;    ilcMax = 2048;

    while(ilcCount &31){
        xINT3;
    }
    xLABEL(lEntry);

    //typedef void (FASTCALL *PFMM)(FDMatrix *dst, FDMatrix *a, FDMatrix *b);
    //for function above stack frame will look like 
    //      ECX                                             dst            
    //      EDX                                             a             
    //      xMOV_r_rm       (rECX, rmSIB8) xSIB8_esp(0x4)   b
    
    xMOV_r_rm       (rEAX, rmSIB8) xSIB8_esp(0x4)           //EAX<-b
    xADD_rm_imm     (rmREG(rEAX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  EAX=b->data[0][0][0];
    xADD_rm_imm     (rmREG(rECX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  ECX=dst->data[0][0][0];
    xADD_rm_imm     (rmREG(rEDX), FDMATRIX_HEADER_SIZE)     //#row/cols data members skipped  EDX=a->data[0][0][0];    
//get first column vector of b
#define GET_COL(a) {                                                               \
                    xMOVLPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32((16*a+0));        \
                    xMOVHPS_r_rm    (rXMM4,rmIND32(rEAX)) xOFS32((16*a+8));        \
                    xMOVLPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32((16*a+256));      \
                    xMOVHPS_r_rm    (rXMM5,rmIND32(rEAX)) xOFS32((16*a+264));      \
                    xMOVLPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32((16*a+512));      \
                    xMOVHPS_r_rm    (rXMM6,rmIND32(rEAX)) xOFS32((16*a+520));      \
                    xMOVLPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32((16*a+768));      \
                    xMOVHPS_r_rm    (rXMM7,rmIND32(rEAX)) xOFS32((16*a+776));      \
                   }

//get first row vector of a
#define GET_ROW(a) {                                                                \
                    xMOVLPS_r_rm    (rXMM0,rmIND32(rEDX)) xOFS32((256*a+0));        \
                    xMOVHPS_r_rm    (rXMM0,rmIND32(rEDX)) xOFS32((256*a+8));        \
                    xMOVLPS_r_rm    (rXMM1,rmIND32(rEDX)) xOFS32((256*a+16));       \
                    xMOVHPS_r_rm    (rXMM1,rmIND32(rEDX)) xOFS32((256*a+24));       \
                    xMOVLPS_r_rm    (rXMM2,rmIND32(rEDX)) xOFS32((256*a+32));       \
                    xMOVHPS_r_rm    (rXMM2,rmIND32(rEDX)) xOFS32((256*a+40));       \
                    xMOVLPS_r_rm    (rXMM3,rmIND32(rEDX)) xOFS32((256*a+48));       \
                    xMOVHPS_r_rm    (rXMM3,rmIND32(rEDX)) xOFS32((256*a+56));       \
                   }

#define CROSS() {                                                                \
                    xMULPS_r_rm     (rXMM4, rmREG(rXMM0));                       \
                    xMULPS_r_rm     (rXMM5, rmREG(rXMM1));                       \
                    xADDPS_r_rm     (rXMM4, rmREG(rXMM5));                       \
                    xMULPS_r_rm     (rXMM6, rmREG(rXMM2));                       \
                    xADDPS_r_rm     (rXMM4, rmREG(rXMM6));                       \
                    xMULPS_r_rm     (rXMM7, rmREG(rXMM3));                       \
                    xADDPS_r_rm     (rXMM4, rmREG(rXMM7));                       \
                }

#define SAVE(a,b) {                                                              \
                    xMOVLPS_rm_r    (rmIND32(rECX), rXMM4) xOFS32(256*a+16*b+0); \
                    xMOVHPS_rm_r    (rmIND32(rECX), rXMM4) xOFS32(256*a+16*b+8); \
                  }    

    GET_ROW(0)
    //  x[0][0] = a00*b00+a01*b10+a02*b20+a03*b30;
    GET_COL(0)
    CROSS()
    SAVE(0,0)
    //  x[0][1] = a00*b01+a01*b11+a02*b21+a03*b31;    
    GET_COL(1)
    CROSS()
    SAVE(0,1)
    //  x[0][2] = a00*b02+a01*b12+a02*b22+a03*b32;
    GET_COL(2)
    CROSS()
    SAVE(0,2)
    //  x[0][3] = a00*b03+a01*b13+a02*b23+a03*b33;
    GET_COL(3)
    CROSS()
    SAVE(0,3)

    GET_ROW(1)
    //  x[1][0] = a10*b00+a11*b10+a12*b20+a13*b30;
    GET_COL(0)
    CROSS()
    SAVE(1,0)
    //  x[1][1] = a10*b01+a11*b11+a12*b21+a13*b31;
    GET_COL(1)
    CROSS()
    SAVE(1,1)
    //  x[1][2] = a10*b02+a11*b12+a12*b22+a13*b32;
    GET_COL(2)
    CROSS()
    SAVE(1,2)
    //  x[1][3] = a10*b03+a11*b13+a12*b23+a13*b33;
    GET_COL(3)
    CROSS()
    SAVE(1,3)

    GET_ROW(2)
    //  x[2][0] = a20*b00+a21*b10+a22*b20+a23*b30;
    GET_COL(0)
    CROSS()
    SAVE(2,0)
    //  x[2][1] = a20*b01+a21*b11+a22*b21+a23*b31;
    GET_COL(1)
    CROSS()
    SAVE(2,1)
    //  x[2][2] = a20*b02+a21*b12+a22*b22+a23*b32;
    GET_COL(2)
    CROSS()
    SAVE(2,2)
    //  x[2][3] = a20*b03+a21*b13+a22*b23+a23*b33;
    GET_COL(3)
    CROSS()
    SAVE(2,3)

    GET_ROW(3)
    //  x[3][0] = a30*b00+a31*b10+a32*b20+a33*b30;
    GET_COL(0)
    CROSS()
    SAVE(3,0)
    //  x[3][1] = a30*b01+a31*b11+a32*b21+a33*b31;
    GET_COL(1)
    CROSS()
    SAVE(3,1)
    //  x[3][2] = a30*b02+a31*b12+a32*b22+a33*b32;
    GET_COL(2)
    CROSS()
    SAVE(3,2)
    //  x[3][3] = a30*b03+a31*b13+a32*b23+a33*b33;
    GET_COL(3)
    CROSS()
    SAVE(3,3)
    
    xRET_imm            (0x4)                           
    assert(ilcCount < ilcMax);

    return (PFMM)(ilcData + lEntry);
#undef GET_COL
#undef GET_ROW
#undef SAVE
#undef CROSS
}
#endif
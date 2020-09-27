 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************* CPU Information Gather Routines ***********************\
*                                                                           *
* Module: CPUID.C                                                           *
*   One time initialization code to setup the Processor type                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nvrm.h>
#include <nvcm.h>
#include <os.h>

#define CPU_STD_TSC                 BIT(4)
#define CPU_STD_MMX                 BIT(23)
#define CPU_STD_XMM                 BIT(25)
#define CPU_STD_WNI                 BIT(26)
#define CPU_EXT_3DNOW               BIT(31)

/*
 * Identify chip foundry.
 *      IS_INTEL   = "GenuineIntel"
 *      IS_AMD     = "AuthenticAMD"
 *      IS_WINCHIP = "CentaurHauls"
 *      IS_CYRIX   = "CyrixInstead"
 */
#define IS_INTEL(fndry)     (((fndry).StrID[0]==0x756E6547)&&((fndry).StrID[1]==0x49656E69)&&((fndry).StrID[2]==0x6C65746E))
#define IS_AMD(fndry)       (((fndry).StrID[0]==0x68747541)&&((fndry).StrID[1]==0x69746E65)&&((fndry).StrID[2]==0x444D4163))
#define IS_WINCHIP(fndry)   (((fndry).StrID[0]==0x746E6543)&&((fndry).StrID[1]==0x48727561)&&((fndry).StrID[2]==0x736C7561))
#define IS_CYRIX(fndry)     (((fndry).StrID[0]==0x69727943)&&((fndry).StrID[1]==0x736E4978)&&((fndry).StrID[2]==0x64616574))

/*
 * ID the CPU.
 */
VOID RmInitCpuInfo
(
    PHWINFO pDev
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    int i;
    struct _CPUID
    {
        union
        {
            U008 String[12];
            U032 StrID[3];
        } Foundry;
        U032 pad;
        U032 Family;
        U032 Model;
        U032 Stepping;
        U032 StandardFeatures;
        U032 MemoryFeatures[4];
        U032 ExtendedFeatures;
        U032 ChipFlags;
        U032 MHz;
    } cpuinfo;

#ifdef __GNUC__
    U032 eax, ebx, ecx, edx;
#endif

    
    /*
     * Init structure to default.
     */
    for (i = 0; i < 12; i++)
        cpuinfo.Foundry.String[i] = '\0';
    cpuinfo.pad              = 0;
    cpuinfo.Family           = 0;
    cpuinfo.Model            = 0;
    cpuinfo.Stepping         = 0;
    cpuinfo.StandardFeatures = 0;
    cpuinfo.ExtendedFeatures = 0;
    cpuinfo.ChipFlags        = 0;
    cpuinfo.MHz              = 0;
    
    pRmInfo->Processor.Type = 0;
    pRmInfo->Processor.Clock = 0;
    
#ifdef __GNUC__
    // if nv_cpuid() returns 0, then this cpu does not support cpuid instruction
    // We just worry about this on the first call...
    if ( ! nv_cpuid(0x00000000/*cpuid op*/, &eax, &ebx, &ecx, &edx))
        goto Exit;

    cpuinfo.Foundry.StrID[0] = ebx;
    cpuinfo.Foundry.StrID[1] = edx;
    cpuinfo.Foundry.StrID[2] = ecx;
        
    nv_cpuid(0x00000001/*cpuid op*/, &eax, &ebx, &ecx, &edx);
    cpuinfo.Family = (eax >> 8) & 0xF;
    cpuinfo.Model = (eax >> 4) & 0xF;
    cpuinfo.Stepping = (eax & 0xF);
    cpuinfo.StandardFeatures = edx;

    nv_cpuid(0x00000002/*cpuid op*/, &eax, &ebx, &ecx, &edx);
    cpuinfo.MemoryFeatures[0] = eax;
    cpuinfo.MemoryFeatures[1] = ebx;
    cpuinfo.MemoryFeatures[2] = ecx;
    cpuinfo.MemoryFeatures[3] = edx;

#else        /* ! __GNUC__ */

        /* XXX this code should be converted to use less asm */

#ifndef IA64
    __asm
    {
        ; save state
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd
        pop     eax
        mov     ebx, eax
        xor     eax, 0x00200000
        push    eax
        popfd
        pushfd
        pop     eax
        cmp     eax, ebx
        je      Exit                      // cpuid inst not supported
        mov     eax, 0
//      cpuid instr
        _emit   0x0F
        _emit   0xA2
        lea     esi, cpuinfo
        mov     [esi], ebx                // cpuinfo.Foundary.StrID[0]
        mov     [esi + 4], edx            // cpuinfo.Foundary.StrID[1]
        mov     [esi + 8], ecx            // cpuinfo.Foundary.StrID[2] 
        push    eax                       // Save max function count for later


        mov     eax, 1
//      cpuid instr
        _emit   0x0F
        _emit   0xA2

        mov     ebx, eax
        shr     ebx, 8
        and     ebx, 0x0F
        mov     [esi + 16], ebx           // cpuinfo.Family

        mov     ebx, eax
        shr     ebx, 4
        and     ebx, 0x0F
        mov     [esi + 20], ebx           // cpuinfo.Model

        mov     ebx, eax
        and     ebx, 0x0F
        mov     [esi + 24], ebx           // cpuinfo.Stepping

        mov     [esi + 28], edx           // cpuinfo.StandardFeatures


        mov     eax, 2
//      cpuid instr
        _emit   0x0F
        _emit   0xA2
        mov     [esi + 32], eax           // cpuinfo.MemoryFeatures
        mov     [esi + 36], ebx
        mov     [esi + 40], ecx
        mov     [esi + 44], edx
        pop     eax                       // Restore max function, but discard
        
        ; restore state
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
#endif // IA64 cannot handle inline asm
#endif
    
    if (IS_INTEL(cpuinfo.Foundry))
    {
        if (cpuinfo.Family == 5)
        {
            if (cpuinfo.Model == 4)
                pRmInfo->Processor.Type = NV_CPU_P55;
            else 
                pRmInfo->Processor.Type = NV_CPU_P5;
        }
        if (cpuinfo.Family == 6)
        {
            if (cpuinfo.Model == 1)
                pRmInfo->Processor.Type = NV_CPU_P6;
            else if (cpuinfo.Model == 3)
                pRmInfo->Processor.Type = NV_CPU_P2;
            else if (cpuinfo.Model == 5)
                pRmInfo->Processor.Type = NV_CPU_P2XC;
            else if (cpuinfo.Model == 6)
                pRmInfo->Processor.Type = NV_CPU_CELA;
            else if (cpuinfo.Model == 7)
                pRmInfo->Processor.Type = NV_CPU_P3;
            else if (cpuinfo.Model == 8)
                pRmInfo->Processor.Type = NV_CPU_P3_INTL2;
        }
            
        if (cpuinfo.StandardFeatures & CPU_STD_MMX)
            pRmInfo->Processor.Type |= NV_CPU_FUNC_MMX;
        if (cpuinfo.StandardFeatures & CPU_STD_XMM)
        {
            U032 my_cr4;
            
            pRmInfo->Processor.Type |= NV_CPU_FUNC_SFENCE;
#ifdef __GNUC__
            my_cr4 = nv_rdcr4();
#else
#ifndef IA64
            // Before setting the NV_CPU_FUNC_SSE bit, we'll also check
            // that CR4.OSFXSR(bit 9) is set, which means the OS is prepared
            // to switch the additional KATMAI FP state for us.
            
            __asm
            {
                ; save state
                push    eax
                push    ebx
                push    ecx
                push    edx

                // mov     eax,cr4
                _emit   0x0F
                _emit   0x20
                _emit   0xE0

                mov     my_cr4, eax
                
                ; restore state
                pop     edx
                pop     ecx
                pop     ebx
                pop     eax
            }
#else
my_cr4 = 0;
#endif // IA64 cannot handle inline asm
#endif
            if (my_cr4 & 0x200)
            {
                pRmInfo->Processor.Type |= NV_CPU_FUNC_SSE;

                // supports SSE2 (Willamette NI) instructions
                if (cpuinfo.StandardFeatures & CPU_STD_WNI)
                    pRmInfo->Processor.Type |= NV_CPU_FUNC_SSE2;
            }
        }
    }
    else
    {
#ifdef __GNUC__
        nv_cpuid(0x80000001/*cpuid op*/, &eax, &ebx, &ecx, &edx);
        cpuinfo.ExtendedFeatures = edx;
#else
#ifndef IA64
        // All other non-Intel CPU types will want to get the extended features
        __asm
        {
            ; save state
            push    eax
            push    ebx
            push    ecx
            push    edx
                
            mov     eax, 0x80000001
            // cpuid instr
            _emit   0x0F
            _emit   0xA2
            mov     [esi + 48], edx           // cpuinfo.ExtendedFeatures
        
            ; restore state
            pop     edx
            pop     ecx
            pop     ebx
            pop     eax
        }
#endif // IA64 cannot handle inline asm
#endif

        if (IS_AMD(cpuinfo.Foundry))
        {
            if (cpuinfo.Family == 5)                // K5, K6, K6-2 with 3DNow, K6-3
            {
                if (cpuinfo.Model < 6)
                    pRmInfo->Processor.Type = NV_CPU_K5;
                else if (cpuinfo.Model < 8)
                    pRmInfo->Processor.Type = NV_CPU_K6;
                else if (cpuinfo.Model == 8)
                    pRmInfo->Processor.Type = NV_CPU_K62;
                else if (cpuinfo.Model == 9)
                    pRmInfo->Processor.Type = NV_CPU_K63;
             }
            if (cpuinfo.Family == 6)                // K7
            {
                pRmInfo->Processor.Type = NV_CPU_K7;
                pRmInfo->Processor.Type |= NV_CPU_FUNC_SFENCE;
            }
            if (cpuinfo.StandardFeatures & CPU_STD_MMX)
                pRmInfo->Processor.Type |= NV_CPU_FUNC_MMX;
            if (cpuinfo.ExtendedFeatures & CPU_EXT_3DNOW)
                pRmInfo->Processor.Type |= NV_CPU_FUNC_3DNOW;
        }
        else if (IS_WINCHIP(cpuinfo.Foundry))
        {
            if (cpuinfo.Family == 5)                // Winchip C6, Winchip2 w/ 3DNow
            {
                if (cpuinfo.Model == 4)
                    pRmInfo->Processor.Type = NV_CPU_C6;
                if (cpuinfo.Model == 8)        
                    pRmInfo->Processor.Type = NV_CPU_C62;
            }
                    
            if (cpuinfo.StandardFeatures & CPU_STD_MMX)
                pRmInfo->Processor.Type |= NV_CPU_FUNC_MMX;
            if (cpuinfo.ExtendedFeatures & CPU_EXT_3DNOW)
                pRmInfo->Processor.Type |= NV_CPU_FUNC_3DNOW;
        }
        else if (IS_CYRIX(cpuinfo.Foundry))
        {
            if (cpuinfo.Family == 4)                // MediaGX
                pRmInfo->Processor.Type = NV_CPU_GX;
            if (cpuinfo.Family == 5)                // Cyrix 6x86 or MediaGX w/ MMX
            {
                if (cpuinfo.Model == 2)
                    pRmInfo->Processor.Type = NV_CPU_M1;
                if (cpuinfo.Model == 4)        
                    pRmInfo->Processor.Type = NV_CPU_MGX;
            }
            if (cpuinfo.Family == 6)                // Cyrix MII
                pRmInfo->Processor.Type = NV_CPU_M2;
        
            if (cpuinfo.StandardFeatures & CPU_STD_MMX)
                pRmInfo->Processor.Type |= NV_CPU_FUNC_MMX;
            if (cpuinfo.ExtendedFeatures & CPU_EXT_3DNOW)
                pRmInfo->Processor.Type |= NV_CPU_FUNC_3DNOW;
        }
    }
    
    // Calculate the frequency
    if (cpuinfo.StandardFeatures & CPU_STD_TSC)
        pRmInfo->Processor.Clock = osGetCpuFrequency();
#ifndef IA64        
Exit:    
#endif // IA64 cannot handle inline asm
    return;
}

void RmInitBusInfo
(
    PHWINFO pDev
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    U016 vendorID, deviceID;
    U032 handle, data32;
    U032 hclk_freq, mclk_freq;

    //
    // Currently the only chipset we need to really concern ourselves with is
    // Crush, so zero out the others for now
    //
    pRmInfo->Processor.HalInfo.FrontSideBusFreq = 0;
    pRmInfo->Processor.HalInfo.SystemMemoryFreq = 0;

    //
    // Determine if this is a Crush system
    //
    handle = osPciInitHandle(0, 0, 0, &vendorID, &deviceID);
    if (!handle)
        return;

    if ((vendorID != 0x10B9) || (deviceID != 0x1631))
    {
        U016 tempvid, tempdid;
        U032 temphandle;

        //
        // Deal with ALI trickery.  On some systems they changed the chipset
        // devid to 0x1621 (their old chipset).  If this is the case, peer at
        // bus1 dev0 to see if we're there.  If so, assume this is really
        // a 1631
        //
        if ((vendorID == 0x10B9) && (deviceID == 0x1621))
        {
            temphandle = osPciInitHandle(1, 0, 0, &tempvid, &tempdid);
            if (!temphandle)
                return;

            // Is NV out there at Bus1 Dev0?
            if ((tempvid != 0x10DE) || ((tempdid & 0xFFF0) != 0x00A0))
                return;

            // Get the original handle back
            handle = osPciInitHandle(0, 0, 0, &vendorID, &deviceID);
            if (!handle)
                return;
        }
        else
            return;
    }

    data32 = osPciReadDword(handle, 0xF4);    // Clock Freq register

    hclk_freq = (data32 >> 6) & 0x03;   // hclk_freq [7:6]
    mclk_freq = (data32 >> 4) & 0x03;   // mclk_freq [5:4]

    switch (hclk_freq)
    {
        case 2:
            pRmInfo->Processor.HalInfo.FrontSideBusFreq = 133000000;
            break;

        case 1:
            pRmInfo->Processor.HalInfo.FrontSideBusFreq = 100000000;
            break;

        case 0:
        default:
            pRmInfo->Processor.HalInfo.FrontSideBusFreq = 66000000;
            break;
    }

    switch (mclk_freq)
    {
        case 2:
            pRmInfo->Processor.HalInfo.SystemMemoryFreq = 133000000;
            break;

        case 1:
            pRmInfo->Processor.HalInfo.SystemMemoryFreq = 100000000;
            break;

        case 0:
        default:
            pRmInfo->Processor.HalInfo.SystemMemoryFreq = 66000000;
            break;
    }
}

#ifdef DEBUG
static char *MTRR_Typenames[] = {
    "NVRM:      Type Uncached\n",
    "NVRM:      Type WriteCombined\n",
    "NVRM:      Type Reserved\n",
    "NVRM:      Type Reserved\n",
    "NVRM:      Type WriteThrough\n",
    "NVRM:      Type WriteProtected\n",
    "NVRM:      Type WriteBack\n",
    "NVRM:      Type Reserved\n"
};

//
// This routine is used to make sure the MTRRs are setup the way we think
// they should be. Manually edit the mtrr_done_once variable, if you want
// to see the output post-boot.
//
VOID dbgDumpMTRRs(VOID)
{
    U032 i, numRanges = 0, mtrrAddr;
    U032 BaseHi, BaseLo, MaskHi, MaskLo, Type;
    static U032 mtrr_done_once = 0;
#ifdef __GNUC__
    U032 eax, ebx, ecx, edx;
#endif

     if (mtrr_done_once)
        return;
    mtrr_done_once = 1;

#ifdef __GNUC__
    // if nv_cpuid() returns 0, then this cpu does not support cpuid instruction
    // We just worry about this on the first call...
    if ( ! nv_cpuid(0x00000001/*cpuid op*/, &eax, &ebx, &ecx, &edx))
        return;

    // Is MTRR supported?
    if (0 == (edx & 0x1000))
        return;

    if ( ! nv_rdmsr(254/*address*/, &eax, &edx))
        return;
    if (0 == (eax & 0x400))
        return;
    
    numRanges = eax & 0xFF;

#else

#ifndef IA64
#define CPUID   __asm _emit 0x0F __asm _emit 0xA2
#define RDMSR   __asm _emit 0x0F __asm _emit 0x32

    __asm
    {
        ; save state
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd
        pop     eax
        mov     ebx, eax
        xor     eax, 0x00200000
        push    eax
        popfd
        pushfd
        pop     eax
        cmp     eax, ebx
        je      Exit                // CPUID inst not supported

        mov     eax, 1
        CPUID
        and     edx, 0x1000         // check for MTRR support
        jz      Exit

        mov     ecx, 254            // read the MTRRcap MSR to determine number of ranges.
        RDMSR
        test    eax, 0x400          // check that WC as a type is supported (should be 1).
        jz      Exit

        and     eax, 0xFF           // save number of ranges
        mov     numRanges, eax
Exit:    
        ; restore state
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
#else
numRanges = 0;    
#endif // IA64 cannot handle inline asm
#endif

    // Use a high DEBUGLEVEL for these strings, since this is normally done once.
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Valid MTRRs:\n");

    // Now determine if we find a Write Combined match in the MTRRs
    for (i = 0, mtrrAddr = 512; i < numRanges; i++, mtrrAddr += 2)
    {
#ifdef __GNUC__
        nv_rdmsr(mtrrAddr, &BaseLo, &BaseHi);
        nv_rdmsr(mtrrAddr+1, &MaskLo, &MaskHi);        
#else
#ifndef IA64
        __asm {
            ; save state
            push    eax
            push    ebx
            push    ecx
            push    edx

            mov     ecx, mtrrAddr
            RDMSR

            mov     BaseHi, edx
            mov     BaseLo, eax

            mov     ecx, mtrrAddr
            add     ecx, 1
            RDMSR

            mov     MaskHi, edx
            mov     MaskLo, eax

            ; restore state
            pop     edx
            pop     ecx
            pop     ebx
            pop     eax
        }
#else
BaseHi = 0;
BaseLo = 0;
MaskHi = 0;
MaskLo = 0;
#endif // IA64 cannot handle inline asm
#endif

        if (!(MaskLo & 0x800))
            continue;               // valid bit not set

        Type = BaseLo & 0xFF;      // set the type
        if (Type >= 0x8)
            Type = 0x7;            // reserved, if it's too large

        MaskLo &= ~0xFFF;          // mask off the valid bit
        BaseLo &= ~0xFFF;          // mask off the type bits

        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Range ", i);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:      Base ", BaseLo);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:    Length ", (~(MaskLo & ~0xFFF)) + 1);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, MTRR_Typenames[Type]);
    }
}
#endif

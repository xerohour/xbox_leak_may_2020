 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1999 NVIDIA, Corp.  All rights reserved.             *|
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
|*     Copyright (c) 1999 NVIDIA, Corp.    NVIDIA  design  patents           *|
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
* Module: cpuid-ppc.c                                                       *
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

#ifdef MACOS9
#include <MacTypes.h>
#include <NameRegistry.h>

static U032 PPCAttributes_macos9(
    PHWINFO pDev
)
{
    U032 attributes = 0;
    U032 wc_supported;
    RegEntryIter            cookie;
    RegEntryID              entryID;
    OSStatus                err = noErr;
    RegEntryIterationOp     iterOp;
    Boolean                 done;
        
    // write-combining?
    wc_supported = osReadRegistryBoolean(pDev, "Devices:device-tree:pci", "write-combining");

    if (wc_supported)
          attributes |= NV_CPU_FUNC_WRITE_COMBINING;
    
    // XXX hack for P21s that don't have a rom that sets 'write-combining' (1/2001)
    // WC first supported on P21
    if ( ! wc_supported)
    {
        char model_name[32+1];
        U032 len = sizeof(model_name) - 1;
        if (RM_OK == osReadRegistryBinary(pDev, "Devices:device-tree", "model", (unsigned char *) model_name, &len))
        {
            if (0 == strncmp(model_name, "PowerMac3,4", 11))
               attributes |= NV_CPU_FUNC_WRITE_COMBINING;
        }
    }

    // altivec?
    RegistryEntryIDInit(&entryID);

    iterOp = kRegIterDescendants;
    if (noErr == RegistryEntryIterateCreate(&cookie))
    {
       do
       {
           err = RegistryEntrySearch(&cookie, iterOp,
                                     &entryID, &done, "altivec", (char *) 0, 0);
           if ( ! done && (noErr == err))
           {
               attributes |= NV_CPU_FUNC_ALTIVEC;
               RegistryEntryIDDispose(&entryID);
               done = 1;        // can also be set by RegistryEntrySearch
           }

           iterOp = kRegIterContinue;
       } while ( ! done && (err == noErr));
    }
    RegistryEntryIterateDispose(&cookie);   

    return attributes;
}
#endif

#ifdef MACOSX
static U032 PPCAttributes_macosX(
    PHWINFO pDev
)
{
    U032 attributes = 0;

    return attributes;
}
#endif

/*
 * ID the CPU.
 */
VOID RmInitCpuInfo(
    PHWINFO pDev
)
{
    PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    // XXX
    pRmInfo->Processor.Type = NV_CPU_PPC603;
#if defined(MACOS9)
    pRmInfo->Processor.Type |= PPCAttributes_macos9(pDev);
#elif defined(MACOSX)
    pRmInfo->Processor.Type |= PPCAttributes_macosX(pDev);
#endif

    // Calculate the frequency
    pRmInfo->Processor.Clock = osGetCpuFrequency();
}

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

/******************* AGP Chipset Routines **********************************\
*                                                                           *
* Module: NVAGP.C                                                           *
*   One time initialization code to update the AGP chipset and              *
*   our own AGP related values (includes workarounds and registry           *
*   overrides.                                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <stddef.h>    // for size_t
#include <nvReg.h>
#include <nvrm.h>
#include <nvhw.h>
#include <os.h>
#include <nvos.h>
#include <nvagp.h>

#ifdef LINUX 
#include <os-interface.h>
#include <asm/system.h>
#define INTERRUPT_SETUP    unsigned long flags;
#define CLI                flags = osCli(flags);
#define STI                flags = osSti(flags);
#else
#define INTERRUPT_SETUP
#ifdef IA64   /* XXX should this include linux?? */
#define CLI
#define STI
#else
#if (_WINN_WIN32 >= 0x0500)
#define CLI     osEnterCriticalCode(pDev)
#define STI      osExitCriticalCode(pDev)
#else
#define CLI      _asm cli
#define STI      _asm sti
#endif
#endif
#endif

VOID
NvUpdateAGPConfig(PHWINFO pDev)
{
}

#if 0


#define BYTES_MAPPED_PER_PAGE   ((RM_PAGE_SIZE >> 2) * RM_PAGE_SIZE)

extern char strDevNodeRM[];

// see how many "devices" are connected to us.
// This is important under Linux, since multiple X Servers can
// be run at once. We don't want the second X Server to reinitialize
// everything when it starts up, or the first X Server to exit
// teardown AGP on the remaining X Server.
static int agp_ref_count = 0;


// XXX get rid of this quick!!
#ifdef LINUX
#define RM_REGISTRY_AGP_BASE_PATH 0
#endif

/* define which chipsets should be compiled in. 
 * This allows controlled adding and testing of new chipsets without 
 * causing problems for people not expecting them.
 */
#define SUPPORT_INTEL_AGP     1
#define SUPPORT_VIA_AGP       1
#define SUPPORT_AMD_AGP       1
#define SUPPORT_RCC_AGP       1
#define SUPPORT_MICRON_AGP    1

enum {
      CS_UNKNOWN = 0
#ifdef SUPPORT_INTEL_AGP
    , CS_INTEL_440LX,
      CS_INTEL_440BX,
      CS_INTEL_440GX,
      CS_INTEL_815,       // solano
      CS_INTEL_820,       // camino
      CS_INTEL_840,       // carmel
      CS_INTEL_850,       // tehama
      CS_INTEL_860,       // colusa
      CS_INTEL_COMPAT     // compatibility
#endif
#ifdef SUPPORT_AMD_AGP
    , CS_AMD_751,         // irongate
      CS_AMD_761          // igd4
#endif
#ifdef SUPPORT_VIA_AGP
    , CS_VIA_8371,        // w/ intel cpu
      CS_VIA_82C694X,     // w/ amd slot A
      CS_VIA_KT133        // w/ amd socket A
#endif
#ifdef SUPPORT_RCC_AGP
    , CS_RCC_6585HE       // cnb20he
#endif
#ifdef SUPPORT_MICRON_AGP
    , CS_MICRON_SAMDDR    // m4369c
#endif
};

/* moved agp mapping info here from pDev.
 * this primarily allows two things:
 *   - a second X Server run on another VT can share AGP with the
 *         initial X Server by using this info.
 *   - if X crashes and leaves AGP setup, the restarting X Server
 *         can use this info, as if it was just a second X Server.
 * In the long run, the variables in pDev should be changed to point
 * to this structure here, but we're too close to a release under
 * multiple platforms to change that much code.
 * For now, we'll just have to deal with keeping info synchronized.
 */

typedef struct {
    unsigned int initialized;
    unsigned int fw_capable;
    unsigned int sba_capable;
    VOID (*enable_mappings)(PHWINFO pDev, U032 handle, U032 physaddr);
    VOID (*flush_mappings)(PHWINFO pDev, U032 handle);
    U032 (*get_aper_size)(PHWINFO pDev, U032 handle);
    U032 (*get_aper_base)(PHWINFO pDev, U032 handle);
    U032 (*get_gart_size)(PHWINFO pDev, U032 handle);
    U032 (*earlyinit)(PHWINFO pDev, U032 handle);
    VOID (*fixup_gart)(PHWINFO pDev, U032 handle, U032 physaddr);
    VOID (*apply_updates)(PHWINFO pDev, U032 handle, U016 deviceID);
    VOID (*disable_chipset)(PHWINFO pDev, U032 handle);
} agp_chipset_funcs;

/* noop error functions to avoid NULL pointer dereferences */
static VOID agp_noop1(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID agp_noop2(PHWINFO pDev, U032 handle);
static U032 agp_noop3(PHWINFO pDev, U032 handle);
static VOID agp_noop4(PHWINFO pDev, U032 handle, U016 deviceID);

/* Chipset Explanation:
 *
 * In theory, it should be easy (sw-wise) to add multiple AGP chipsets per 
 * system. chipset_funcs[] is a static array of all chipsets in a system, 
 * but is only accessed in the code via the chipset ptr. To initialize to 
 * a different chipset, call the AGP_SELECT_DEVICE() macro, to select a 
 * new bus/slot combination. currently, this uses the bus as an offset to 
 * choose which chipset to use. Although this may be a bad assumption, the 
 * only place it is made is right here, and so is a simple, isolated change.
 */

#define AGP_MAX_CHIPSETS 1
static agp_chipset_funcs chipset_funcs[AGP_MAX_CHIPSETS] = 
    { 
        { 
            0, 0, 0, 
            agp_noop1, agp_noop2, agp_noop3, agp_noop3, agp_noop3, agp_noop3, 
            agp_noop1, agp_noop4, agp_noop2
        }
    };

static agp_chipset_funcs *agp_current = NULL;

#define AGP_SELECT_DEVICE(handle, venid, devid)                      \
    agp_current = &chipset_funcs[0];                                 \
    NvSetBusDeviceFunc(pDev);                                        \
    handle = osPciInitHandle(AgpInfo(pDev).NBAddr.bus,               \
                             AgpInfo(pDev).NBAddr.device,            \
                             AgpInfo(pDev).NBAddr.func,              \
                             venid, devid);

/* for a multi-mon environment, we need to save the pDev for AllocAGPPages
 * and FreeAGPPages. Assuming the AGP card is device 0 is wrong. In the
 * long run, this change is wrong for multi-AGP devices, we'll need to
 * change the interfaces to the above functions to take a pDev..
 */ 

static PHWINFO tmp_pDev = NULL;

#define AGP_GET_PDEV()                  tmp_pDev
#define AGP_SAVE_PDEV(pDev)             tmp_pDev = pDev


static VOID 
agp_noop1(PHWINFO pDev, U032 handle, U032 physaddr)
{
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** called agp_noop1\n");
}

static VOID 
agp_noop2(PHWINFO pDev, U032 handle)
{
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** called agp_noop2\n");
}

static U032
agp_noop3(PHWINFO pDev, U032 handle)
{
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** called agp_noop3\n");
    return 0;
}

static VOID 
agp_noop4(PHWINFO pDev, U032 handle, U016 deviceID)
{
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** called agp_noop4\n");
}


#ifdef SUPPORT_AMD_AGP

BOOL 
AMD_IsChipset(PHWINFO pDev)
{
    switch (AgpInfo(pDev).AGPChipset) 
    {
        case CS_AMD_751:
        case CS_AMD_761:
            return TRUE;
    }
    return FALSE;
}

U032
AMD_GetChipset(PHWINFO pDev, U032 device)
{
    switch(device)
    {
        case 0x7006:
            return CS_AMD_751;
        case 0x700E:
            return CS_AMD_761;
    }
    return 0;
}

static VOID AMD_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID AMD_FlushMappings(PHWINFO pDev, U032 handle);
static U032 AMD_GetAperSize(PHWINFO pDev, U032 handle);
static U032 AMD_GetAperBase(PHWINFO pDev, U032 handle);
static U032 AMD_GetGartSize(PHWINFO pDev, U032 handle);
static U032 AMD_EarlyInit(PHWINFO pDev, U032 handle);
static VOID AMD_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID AMD_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID);
static VOID AMD_DisableChipset(PHWINFO pDev, U032 handle);

VOID
AMD_SetupFuncs(U032 chipset, agp_chipset_funcs *funcs)
{
    funcs->enable_mappings  = AMD_EnableMappings;
    funcs->flush_mappings   = AMD_FlushMappings;
    funcs->get_aper_size    = AMD_GetAperSize;
    funcs->get_aper_base    = AMD_GetAperBase;
    funcs->get_gart_size    = AMD_GetGartSize;
    funcs->earlyinit        = AMD_EarlyInit;
    funcs->fixup_gart       = AMD_FixupGart;
    funcs->apply_updates    = AMD_ApplyChipsetUpdates;
    funcs->disable_chipset  = AMD_DisableChipset;
    funcs->initialized = 1;
    funcs->fw_capable = 1;
    funcs->sba_capable = 0;
}


static VOID
AMD_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr)
{
    switch (AgpInfo(pDev).AGPChipset)    
    {
        case CS_AMD_751:
        case CS_AMD_761:
            // enable GART directory cache      
            osPciOrWord(handle, PCI_DEVICE_SPECIFIC + 0x72, 0x2);

            // enable GART table cache and errors
            ((unsigned char*)AgpInfo(pDev).AGPChipsetMapping)[0x2] |= 0x5;

            // base of GART
            ((unsigned int*)AgpInfo(pDev).AGPChipsetMapping)[0x1] = physaddr;
            break;
        default:
            break;
    }

}

static VOID
AMD_FlushMappings(PHWINFO pDev, U032 handle)
{
    // We could invalidate the individual entries, but for starters, just
    // invalidate the whole cache of entries. 
    ((unsigned int*)AgpInfo(pDev).AGPChipsetMapping)[0x3] |= 0x1;
}

static U032
AMD_GetAperSize(PHWINFO pDev, U032 handle)
{
    unsigned int apsize = osPciReadWord(handle, AMD_APER_SIZE); // aperture size
    switch ( (apsize & 0xE) >> 1 )   /* mask and shift bits 1-3 */
    {
        case 0x00:
            apsize = (  32 * 1024 * 1024);
            break;
        case 0x01:
            apsize = (  64 * 1024 * 1024);
            break;
        case 0x02:
            apsize = ( 128 * 1024 * 1024);
            break;
        case 0x03:
            apsize = ( 256 * 1024 * 1024);
            break;
        case 0x04:
            apsize = ( 512 * 1024 * 1024);
            break;
        case 0x05:
            apsize = (1024 * 1024 * 1024);
            break;
        case 0x06:
            apsize = (2024 * 1024 * 1024);
            break;
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Intel bad apsize!\n");
            apsize = 0;
    }

#if 1
    // I'm having problems under Linux if I go greater than 32 Megs...
    if (apsize > AGP_APERTURE_SIZE) 
    {
        apsize = AGP_APERTURE_SIZE;
        // XXX - translate new aperture size back to hw-encoding
        osPciAndWord(handle, AMD_APER_SIZE, ~(0xe) /* apsize */);
    }
#endif

    return apsize;
}

static U032
AMD_GetAperBase(PHWINFO pDev, U032 handle)
{
    return osPciReadDword(handle, PCI_BASE_ADDRESS_0);
}

static U032
AMD_GetGartSize(PHWINFO pDev, U032 handle)
{
    unsigned int gart_size;
    gart_size = ((AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE - 1) / RM_PAGE_SIZE) * 4;
    gart_size += RM_PAGE_SIZE;     // this is for the 1st-level lookup
    return gart_size;
}

static U032 
AMD_EarlyInit(PHWINFO pDev, U032 handle)
{
    if ((AgpInfo(pDev).AGPChipset == CS_AMD_751) || 
        (AgpInfo(pDev).AGPChipset == CS_AMD_761))
    {
        // AMD 751/761 has its AGP related regs memory-mapped via BAR1
        unsigned int physaddr = (osPciReadDword(handle, PCI_BASE_ADDRESS_1) & ~RM_PAGE_MASK);

        AgpInfo(pDev).AGPChipsetMapping = (VOID_PTR) 
            osMapKernelSpace(physaddr, RM_PAGE_SIZE, FALSE);
        if (AgpInfo(pDev).AGPChipsetMapping == (VOID_PTR)NULL)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, 
                    "NVRM: *** Failed to map AMD's AGP chipset registers\n");
            return 0;
        }
    }
    return 1;
}

static VOID
AMD_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr)
{
    U032 i, *baseptr, tableptr;

    //
    // For AMD 751/761, convert the first allocated GART page into the first level GART
    // lookup ... where each GART directory entry is just the subsequent 4k pages. We can
    // derive the following entries, because we know the pages are contiguous.
    //

    baseptr  = (U032 *)((U008 *)AgpInfo(pDev).AGPGartBase + 
        (AgpInfo(pDev).AGPPhysStart >> 20));
    tableptr = physaddr + RM_PAGE_SIZE;    // GART directory entries start on the 2nd page

    for (i = 0; i < ((AgpInfo(pDev).AGPLimit + 1) / BYTES_MAPPED_PER_PAGE); i++)
    {
        *baseptr = (tableptr | 0x1);

        tableptr += RM_PAGE_SIZE;   // next phys GART table address
        baseptr++;                  // next GART directory entry
    }
        
    //
    // Move the AGPGartBase past the first level GART page to where the GART
    // tables really begin (this makes nvagp_LoadGARTEntries() common).
    //
    (U008 *)AgpInfo(pDev).AGPGartBase += RM_PAGE_SIZE;
}

static VOID
AMD_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID)
{
    U032 agp_command;

    if (deviceID != 0x7006)
        return;    // workarounds below are AMD751 (Irongate) specific

    // disable SBA on Irongate with NV5 (some faster NV5's are failing in the lab).
    if (IsNV5(pDev))
    {
        // clear chipset's SBA enable bit
        agp_command  = osPciReadDword(handle, AGP_COMMAND);
        agp_command &= ~(DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _ON));
        osPciWriteDword(handle, AGP_COMMAND, agp_command);

        // clear our SBA enable bit
        FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _OFF);
    }

    // Due to an AMD power glitch bug, drop down to 1x on NV10/NV15
    if (IsNV10orBetter(pDev))
    {
        //
        // Compaq (and possibly others) have a modified NV10 which allows 2x to work.
        // This rework is not SW visable, so we detect it by a registry key.
        //
        if (IsNV10(pDev))
        {
            U032 data32;
            RM_STATUS status;

            // 
            // Compaq has a modified NV10 which allows 2x to work. This rework is not
            // SW visible, so we detect it by a registry key. If set, there's nothing
            // to do.
            //
            status = OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                            "EnableIrongate2x", &data32);
            if (status == RM_OK)
                return;

            //
            // This yields little perf difference, but Gateway has requested an option
            // to turn back on SBA when dropping to 1x mode. This requires setting the
            // strap first, since NV10 is normally not strapped for SBA.
            //
            status = OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                            "EnableIrongateSBA", &data32);
            if (status == RM_OK)
            {
                // set the overwrite bit and change our SBA strapping
                FLD_WR_DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_OVERWRITE, _ENABLED);
                FLD_WR_DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_AGP_SBA,   _ENABLED);

                // set chipset's SBA enable bit
                agp_command  = osPciReadDword(handle, AGP_COMMAND);
                agp_command |= DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _ON);
                osPciWriteDword(handle, AGP_COMMAND, agp_command);

                // set our SBA enable bit
                FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _ON);
            }
        }

        // set AGP 1x mode
        agp_command  = osPciReadDword(handle, AGP_COMMAND);
        agp_command &= ~0xF;    // clear the old AGP rate
        agp_command |= DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_DATA_RATE, _1X);
        osPciWriteDword(handle, AGP_COMMAND, agp_command);

        FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_DATA_RATE, _1X);
    }
}

static VOID
AMD_DisableChipset(PHWINFO pDev, U032 handle)
{
    // paranoia
    if (!AgpInfo(pDev).AGPChipsetMapping)
        return;

    // enable GART directory cache
    // nv_printf("disabling GART directory cache\n");
    // osPciOrWord(handle, PCI_DEVICE_SPECIFIC + 0x72, 0x2);

    // enable GART table cache and errors
    ((unsigned char*)AgpInfo(pDev).AGPChipsetMapping)[0x2] &= ~(0x5);

    // base of GART
    ((unsigned int*)AgpInfo(pDev).AGPChipsetMapping)[0x1] = 0x0;

    // unmap AMD AGP registers
    osUnmapKernelSpace((void *) AgpInfo(pDev).AGPChipsetMapping, RM_PAGE_SIZE);
}

#endif

#ifdef SUPPORT_INTEL_AGP
BOOL 
Intel_IsChipset(PHWINFO pDev)
{
    switch (AgpInfo(pDev).AGPChipset) 
    {
        case CS_INTEL_440LX:
        case CS_INTEL_440BX:
        case CS_INTEL_440GX:
        case CS_INTEL_815:
        case CS_INTEL_820:
        case CS_INTEL_840:
        case CS_INTEL_850:
        case CS_INTEL_860:
        case CS_INTEL_COMPAT:
            return TRUE;
    }
    return FALSE;
}

U032
Intel_GetChipset(PHWINFO pDev, U032 device)
{
    RM_STATUS status;
    U032 data32;

    switch(device)
    {
        case 0x7180:
            return CS_INTEL_440LX;
        case 0x7190:
            return CS_INTEL_440BX;
        case 0x71A0:
            return CS_INTEL_440GX;
        case 0x1130:
            return CS_INTEL_815;
        case 0x2500:          // dell poblano
            return CS_INTEL_820;
        case 0x2501:
            return CS_INTEL_820;
        case 0x1A20:
            return CS_INTEL_840;
        case 0x1A21:
            return CS_INTEL_840;
        case 0x1A22:
            return CS_INTEL_840;
        case 0x2530:
            return CS_INTEL_850;
        case 0x2531:
            return CS_INTEL_860;
    }

    //
    // For this undetected chipset, check if the registry key is on to run it
    // in "compatibility mode". This allows Intel customers with new chipsets
    // to see if it'll work with our existing Intel support.
    //
    status = OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH, "UseIntelCompat", &data32);
    if ((status == RM_OK) && data32)
        return CS_INTEL_COMPAT;

    return 0;
}

static VOID Intel_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID Intel_FlushMappings(PHWINFO pDev, U032 handle);
static U032 Intel_GetAperSize(PHWINFO pDev, U032 handle);
static U032 Intel_GetAperBase(PHWINFO pDev, U032 handle);
static U032 Intel_GetGartSize(PHWINFO pDev, U032 handle);
static U032 Intel_EarlyInit(PHWINFO pDev, U032 handle);
static VOID Intel_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID Intel_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID);
static VOID Intel_DisableChipset(PHWINFO pDev, U032 handle);

VOID
Intel_SetupFuncs(U032 chipset, agp_chipset_funcs *funcs)
{
    funcs->enable_mappings = Intel_EnableMappings;
    funcs->flush_mappings  = Intel_FlushMappings;
    funcs->get_aper_size   = Intel_GetAperSize;
    funcs->get_aper_base   = Intel_GetAperBase;
    funcs->get_gart_size   = Intel_GetGartSize;
    funcs->earlyinit       = Intel_EarlyInit;
    funcs->fixup_gart      = Intel_FixupGart;
    funcs->apply_updates   = Intel_ApplyChipsetUpdates;
    funcs->disable_chipset = Intel_DisableChipset;
    funcs->initialized = 1;
    funcs->sba_capable = 0;

    /* SOLANO, CAMINO, CARMEL, TEHAMA and COLUSA are fast-write capable */
    if (chipset == CS_INTEL_815
        || chipset == CS_INTEL_820
        || chipset == CS_INTEL_840
        || chipset == CS_INTEL_850
        || chipset == CS_INTEL_860)
       funcs->fw_capable = 1;
    else
       funcs->fw_capable = 0;
}

static VOID
Intel_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr)
{
    switch (AgpInfo(pDev).AGPChipset)    
    {
        case CS_INTEL_440LX:
        case CS_INTEL_440BX:
        case CS_INTEL_440GX:
            osPciWriteWord(handle, INTEL_ERRSTS, 0x07);
        /* FALLTHROUGH */
        case CS_INTEL_815:
        case CS_INTEL_820:
        case CS_INTEL_840:
        case CS_INTEL_850:
        case CS_INTEL_860:
        case CS_INTEL_COMPAT:
            osPciWriteDword(handle, INTEL_GART_BASE, physaddr); // base of GART
            osPciOrDword(handle, INTEL_GART_CONTROL, 0x80);  // enable GTLB
            osPciOrDword(handle, INTEL_CHIPSET_CONFIG, 0x200);  // enable global access
        default:
            break;
    }
}

static VOID
Intel_FlushMappings(PHWINFO pDev, U032 handle)
{
    // Invalidate the GTLB
    if (!handle) return;
    osPciAndDword(handle, INTEL_GART_CONTROL, ~(0x80));
    osPciOrDword(handle, INTEL_GART_CONTROL, 0x80);
}


static U032
Intel_GetAperSize(PHWINFO pDev, U032 handle)
{
    unsigned int apsize;
    apsize = osPciReadWord(handle, INTEL_APER_SIZE); // aperture size
    switch (apsize)
    {
        case 0x3f:
            apsize = (4 * 1024 * 1024);
            break;
        case 0x3e:
            apsize = (8 * 1024 * 1024);
            break;
        case 0x3c:
            apsize = (16 * 1024 * 1024);
            break;
        case 0x38:
            apsize = (32 * 1024 * 1024);
            break;
        case 0x30:
            apsize = (64 * 1024 * 1024);
            break;
        case 0x20:
            apsize = (128 * 1024 * 1024);
            break;
        case 0x00:
            apsize = (256 * 1024 * 1024);
            break;
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Intel bad apsize!\n");
            apsize = 0;
    }

    // I'm having problems under Linux if I go greater than 32 Megs...
    if (apsize > AGP_APERTURE_SIZE) 
    {
        apsize = AGP_APERTURE_SIZE;
        // XXX - translate new aperture size back to hw-encoding
        osPciWriteWord(handle, INTEL_APER_SIZE, 0x38 /* apsize */);
    }

    return apsize;
}

static U032
Intel_GetAperBase(PHWINFO pDev, U032 handle)
{
    return osPciReadDword(handle, PCI_BASE_ADDRESS_0);
}

static U032
Intel_GetGartSize(PHWINFO pDev, U032 handle)
{
    /* XXX - change 4 to sizeof(void *)?? */
    return ((AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE) / RM_PAGE_SIZE) * 4;
}

static U032 
Intel_EarlyInit(PHWINFO pDev, U032 handle)
{
    /* nothing to do... */
    return 1;
}

static VOID
Intel_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr)
{
    /* nothing to do... */
}

static VOID
Intel_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID)
{
    U032 agp_status, agp_command;
    U032 data;
    INTERRUPT_SETUP

    // Some special Intel chipset handling
    switch (deviceID)
    {
        case 0x2500:    // Camino
        case 0x2501:
            // expose the FW capability on Camino
            osPciOrDword(handle, INTEL_GART_CONTROL, 0x2);
            break;
        case 0x7190:    // BX/ZX

            if (IsNV15(pDev) || IsNV11(pDev))
                FLD_WR_DRF_DEF(_PBUS, _DEBUG_2, _AGP_VREF, _ENABLED);

            //
            // Temporary workaround for Toshiba's motherboard that didn't
            // connect external vref.
            //
            if (IsNV11(pDev) &&
                (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "Mobile", &data) == RM_OK)) {
                if (data == 2) {        // equate for Toshiba
                    FLD_WR_DRF_DEF(_PBUS, _DEBUG_2, _AGP_VREF, _DISABLED);
                }
            }

            break;
        default:
            break;
    }

    //
    // Check for chipset overrides to AGP Multi-Transaction Timer or the
    // Low Priority Transaction Timer for the supported Intel chipsets.
    //
    switch (deviceID)
    {
        case 0x2500:    // Camino
        case 0x2501:
        case 0x1A20:    // Carmel
        case 0x1A21:
        case 0x1A22:
        case 0x1130:    // Solano
        case 0x2530:    // Tehama
        case 0x2531:    // Colusa
        {
            U032 amtt, lptt;
            if (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                       "IntelAMTT", &amtt) == RM_OK) {
                if (amtt != ~0) {    // value of 0xFFFFFFFF disables the override
                    if (amtt & 0x7) {
                        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Intel AMTT not multiple of 8 clocks\n");
                    } else {
                        osPciWriteByte(handle, INTEL_MULTI_TRANS_TIMER, (U008)amtt);
                    }
                }
            }
            if (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                       "IntelLPTT", &lptt) == RM_OK) {
                if (lptt != ~0) {    // value of 0xFFFFFFFF disables the override
                    if (lptt & 0x7) {
                        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Intel LPTT not multiple of 8 clocks\n");
                    } else {
                        osPciWriteByte(handle, INTEL_LOW_PRI_TRANS_TIMER, (U008)lptt);
                    }
                }
            }
            break;
        }
        default:
            break;
    }

#ifdef LINUX
    {
        unsigned int support_fw;
        if ( (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH, 
                                     "EnableAGPFW", &support_fw) != RM_OK) ||
             (support_fw == 0))
        {
            return;
        }
    }
#endif

    // First, check if this chipset is FW capable
    if (!agp_current->fw_capable)
        return;

    // Also, check if we're FW capable
    if (REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_FW) == NV_PBUS_PCI_NV_18_AGP_STATUS_FW_NONE)
        return;

    //
    // Finally, check if this chipset is strapped for fastwrites.
    // Solano lies about not supporting FW's, so even though the strap isn't
    // set, we can still enable them (there's no backdoor like Camino).
    //
    agp_status = osPciReadDword(handle, AGP_STATUS);
    if (((agp_status & 0x10) == 0) &&
        (Intel_GetChipset(pDev, deviceID) != CS_INTEL_815))
        return;

    CLI

    // enable AGP fastwrites for both us and the chipset
    agp_command  = osPciReadDword(handle, AGP_COMMAND);
    agp_command |= DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE, _ON);

    FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE, _ON);
    osPciWriteDword(handle, AGP_COMMAND, agp_command);

    STI
}

static VOID
Intel_DisableChipset(PHWINFO pDev, U032 handle)
{
    unsigned int agp_command;
    INTERRUPT_SETUP

    agp_command  = osPciReadDword(handle, AGP_COMMAND);

    /* if fast-writes aren't enabled, exit */
    if ( !(agp_command & 0x10) ) return;

    agp_command &= ~(DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE, _ON));

    CLI

    // disable AGP fastwrites for both us and the chipset
    FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE, _OFF);
    osPciWriteDword(handle, AGP_COMMAND, agp_command);

    STI
}
#endif

#ifdef SUPPORT_VIA_AGP

/* XXX hack alert!! */
static unsigned int via_base = 0;

BOOL 
Via_IsChipset(PHWINFO pDev)
{
        switch (AgpInfo(pDev).AGPChipset) {
                case CS_VIA_8371:
                case CS_VIA_82C694X:
                case CS_VIA_KT133:
                        return TRUE;
        }
        return FALSE;
}

U032
Via_GetChipset(PHWINFO pDev, U032 device)
{
    // The spec for the 8371 chipset claims it has a device ID
    // of 0x691, but the version we looked at returned 0x391.
    // This also happens to be the device ID for the 82C694X
    // chipset, so we'll be OK regardless.
    switch(device)
    {
        case 0x0691:
            return CS_VIA_8371;
        case 0x0391:
            return CS_VIA_82C694X;
                case 0x0305:
            return CS_VIA_KT133;
    }
    return 0;
}

static VOID Via_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID Via_FlushMappings(PHWINFO pDev, U032 handle);
static U032 Via_GetAperSize(PHWINFO pDev, U032 handle);
static U032 Via_GetAperBase(PHWINFO pDev, U032 handle);
static U032 Via_GetGartSize(PHWINFO pDev, U032 handle);
static U032 Via_EarlyInit(PHWINFO pDev, U032 handle);
static VOID Via_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID Via_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID);
static VOID Via_DisableChipset(PHWINFO pDev, U032 handle);

VOID
Via_SetupFuncs(U032 chipset, agp_chipset_funcs *funcs)
{
    funcs->enable_mappings  = Via_EnableMappings;
    funcs->flush_mappings   = Via_FlushMappings;
    funcs->get_aper_size    = Via_GetAperSize;
    funcs->get_aper_base    = Via_GetAperBase;
    funcs->get_gart_size    = Via_GetGartSize;
    funcs->earlyinit        = Via_EarlyInit;
    funcs->fixup_gart       = Via_FixupGart;
    funcs->apply_updates    = Via_ApplyChipsetUpdates;
    funcs->disable_chipset  = Via_DisableChipset;
    funcs->initialized = 1;
    funcs->fw_capable = 0;
    funcs->sba_capable = 0;
}

static VOID
Via_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr)
{
    switch (AgpInfo(pDev).AGPChipset)    
    { 
        case CS_VIA_8371:
        case CS_VIA_82C694X:
        case CS_VIA_KT133:
            // enable global access 
            osPciOrDword(handle, VIA_GART_BASE, physaddr | 0x2);
            // enable GA address translation
            osPciOrDword(handle, VIA_GART_CONTROL, 0xf);
            break;
        default:
            break;
    }
}

static VOID
Via_FlushMappings(PHWINFO pDev, U032 handle)
{
    if (!handle) return;
    osPciOrDword(handle, VIA_GART_CONTROL, 0x80);       // enable TLB flush
    osPciAndDword(handle, VIA_GART_CONTROL, ~(0x80));   // disable TLB flush
}


/* XXX 
 * I'm unsure if there is a BIOS-default size here, the docs seem
 * to indicate it defaults to 0, so we'll see
 */
static U032
Via_GetAperSize(PHWINFO pDev, U032 handle)
{
    unsigned int apsize = 0;
    apsize = osPciReadDword(handle, VIA_APER_SIZE);

    switch (apsize)
    {
        case 0xff:
            apsize =   1 * 1024 * 1024; /*   1 Meg */
            break;
        case 0xfe:
            apsize =   2 * 1024 * 1024; /*   2 Meg */
            break;
        case 0xfc:
            apsize =   4 * 1024 * 1024; /*   4 Meg */
            break;
        case 0xf8:
            apsize =   8 * 1024 * 1024; /*   8 Meg */
            break;
        case 0xf0:
            apsize =  16 * 1024 * 1024; /*  16 Meg */
            break;
        case 0xe0:
            apsize =  32 * 1024 * 1024; /*  32 Meg */
            break;
        case 0xc0:
            apsize =  64 * 1024 * 1024; /*  64 Meg */
            break;
        case 0x80:
            apsize = 128 * 1024 * 1024; /* 128 Meg */
            break;
        case 0x00:
            apsize = 256 * 1024 * 1024; /* 256 Meg */
            break;
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** VIA bad apsize!\n");
            apsize = 0;
    }

#if 1
    // I'm having problems under Linux if I go greater than 32 Megs...
    if (apsize > AGP_APERTURE_SIZE) 
    {
        apsize = AGP_APERTURE_SIZE;
        // XXX - translate new aperture size back to hw-encoding
        osPciWriteWord(handle, VIA_APER_SIZE, 0xe0 /* apsize */);
    }
#endif

    return apsize;
}

static U032
Via_GetAperBase(PHWINFO pDev, U032 handle)
{
    if (!via_base)
        via_base = osPciReadDword(handle, PCI_BASE_ADDRESS_0);
    return via_base;
}

static U032
Via_GetGartSize(PHWINFO pDev, U032 handle)
{
    /* XXX - change 4 to sizeof(void *)?? */
    return ((AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE) / RM_PAGE_SIZE) * 4;
}

static U032 
Via_EarlyInit(PHWINFO pDev, U032 handle)
{
    /* nothing to do... */
    return 1;
}

static VOID
Via_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr)
{
    /* nothing to do... */
}

static VOID
Via_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID)
{
    U032 agp_status, agp_command, gart_control;
    INTERRUPT_SETUP

    //
    // First, determine the real ID of the chipset.
    //
    // Via, in some cases, has been using the Apollo 694x device ID with
    // the KX133 and KT133, and since we're not treating them the same
    // anymore, we need to get the real ID.
    //
    if (deviceID == 0x691)
    {
        U008 backdoor1;

        // disable the Back-Door Device ID Enable bit
        backdoor1  = osPciReadByte(handle, 0xFC);
        osPciWriteByte(handle, 0xFC, (U008)(backdoor1 & ~0x1));

        // reread the deviceID
        deviceID = (U016)(osPciReadDword(handle, 0x0) >> 16);

        // restore the Back-Door control byte
        osPciWriteByte(handle, 0xFC, backdoor1);
    }

    // return, if it's not a 694x, KX133 or KT133
    if ((deviceID != 0x691) && (deviceID != 0x391) && (deviceID != 0x305))
        return;

    //
    // Workaround for a Via chipset driver in WinME that forgot to reenable
    // this bit on the way back from hibernation. This is fixed in the latest
    // Via chipset driver, but just to be safe, leave it in for now.
    //
    gart_control = osPciReadDword(handle, 0x80);
    gart_control |= 0x2;
    osPciWriteDword(handle, 0x80, gart_control);

    // 
    // If AGP FW's are already enabled (as part of normal chipset driver
    // configuration) avoid trying to change the AGP rate, since this can
    // cause a hang.
    //
    if (REG_RD_DRF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE) ==
        NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE_ON)
        return;

    //
    // Only the Apollo Pro133 (Via694x) should be dropped down from 4x to 2x
    //
    if (deviceID == 0x691)
    {
        // check if we're currently in AGP 4x mode
        agp_command = osPciReadDword(handle, AGP_COMMAND);
        if (DRF_VAL(_PBUS, _PCI_NV_19, _AGP_COMMAND_DATA_RATE, agp_command) == NV_PBUS_PCI_NV_19_AGP_COMMAND_DATA_RATE_4X)
        {
            U032 data32;
            RM_STATUS status;

            // check if there's a registry override to stay at AGP 4x
            status = OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH, "EnableVia4x", &data32);
            if ( (status != RM_OK) || (data32 == 0) )
            {
                U008 agp_misc;

                // no override, so we'll go to AGP 2x mode
                agp_command &= ~0xF;    // clear the old AGP rate
                agp_command |= DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_DATA_RATE, _2X);
                osPciWriteDword(handle, AGP_COMMAND, agp_command);

                FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_DATA_RATE, _2X);

                // 
                // In addition to changing AGPCMD, also change AGPSTATUS to disable 4x
                // in case there's another module that tries to put us back in that mode.
                //
                agp_misc  = osPciReadByte(handle, 0xAE);
                agp_misc &= 0xFB;    // clear 4x enable
                osPciWriteByte(handle, 0xAE, agp_misc);

                // clear our 4x enable
                FLD_WR_DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_OVERWRITE, _ENABLED);
                FLD_WR_DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_AGP_4X,    _DISABLED);
            }
        }
    }

    if (IsNV10orBetter(pDev))
    {
        //
        // Set Via's AGP Read Synchronization bit which fixes a coherency
        // problem on their chipset (applies to NV10 and later chips).
        //
        osPciOrByte(handle, VIA_AGP_CONTROL /* 0xAC */, 0x40);
    }

#ifdef LINUX
    {
        unsigned int support_fw;
        if ( (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH, 
                                     "EnableAGPFW", &support_fw) != RM_OK) ||
             (support_fw == 0))
        {
            return;
        }
    }
#endif

    // check if we're FW capable
    if (REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_FW) == NV_PBUS_PCI_NV_18_AGP_STATUS_FW_NONE)
        return;

    // check if Via is strapped for FW
    agp_status = osPciReadDword(handle, AGP_STATUS /* 0xA4 */);
    if (DRF_VAL(_PBUS, _PCI_NV_18, _AGP_STATUS_FW, agp_status) == NV_PBUS_PCI_NV_18_AGP_STATUS_FW_NONE)
        return;
    
    CLI

    // enable AGP fastwrites for both us and the chipset
    agp_command  = osPciReadDword(handle, AGP_COMMAND);
    agp_command |= DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE, _ON);

    FLD_WR_DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE, _ON);
    osPciWriteDword(handle, AGP_COMMAND, agp_command);
    
    STI
}

static VOID
Via_DisableChipset(PHWINFO pDev, U032 handle)
{
    switch (AgpInfo(pDev).AGPChipset)    
    { 
        case CS_VIA_8371:
        case CS_VIA_82C694X:
        case CS_VIA_KT133:
            // enable global access 
            osPciWriteDword(handle, VIA_GART_BASE, 0x0);
            // enable GA address translation
            // osPciAndDword(handle, VIA_GART_CONTROL, ~(0xf));
            // restore aperture size
            // XXX hack, need to translate this mapping from stored value!!
            osPciWriteWord(handle, VIA_APER_SIZE, 0xe0 /* apsize */);
            break;
        default:
            break;
    }
}

#endif

#ifdef SUPPORT_RCC_AGP

BOOL 
RCC_IsChipset(PHWINFO pDev)
{
    switch (AgpInfo(pDev).AGPChipset) 
    {
        case CS_RCC_6585HE:
            return TRUE;
    }
    return FALSE;
}

U032
RCC_GetChipset(PHWINFO pDev, U032 device)
{
    switch(device)
    {
        case 0x0009:
            return CS_RCC_6585HE;
    }
    return 0;
}

static VOID RCC_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID RCC_FlushMappings(PHWINFO pDev, U032 handle);
static U032 RCC_GetAperSize(PHWINFO pDev, U032 handle);
static U032 RCC_GetAperBase(PHWINFO pDev, U032 handle);
static U032 RCC_GetGartSize(PHWINFO pDev, U032 handle);
static U032 RCC_EarlyInit(PHWINFO pDev, U032 handle);
static VOID RCC_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID RCC_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID);
static VOID RCC_DisableChipset(PHWINFO pDev, U032 handle);
static VOID RCC_FlushPostedWriteBuffer(PHWINFO pDev);

VOID
RCC_SetupFuncs(U032 chipset, agp_chipset_funcs *funcs)
{
    funcs->enable_mappings  = RCC_EnableMappings;
    funcs->flush_mappings   = RCC_FlushMappings;
    funcs->get_aper_size    = RCC_GetAperSize;
    funcs->get_aper_base    = RCC_GetAperBase;
    funcs->get_gart_size    = RCC_GetGartSize;
    funcs->earlyinit        = RCC_EarlyInit;
    funcs->fixup_gart       = RCC_FixupGart;
    funcs->apply_updates    = RCC_ApplyChipsetUpdates;
    funcs->disable_chipset  = RCC_DisableChipset;
    funcs->initialized = 1;
    funcs->fw_capable = 0;
    funcs->sba_capable = 0;
}

static VOID
RCC_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr)
{
    volatile U032 *ccr;    // cache control register

    switch (AgpInfo(pDev).AGPChipset)    
    {
        case CS_RCC_6585HE:

            // enable GART cache and error reporting 
            ((unsigned char*)AgpInfo(pDev).AGPChipsetMapping)[0x2] |= 0x9;
            
            // invalidate the caches before we start (and poll until it's done)
            ccr = &((U032 *)AgpInfo(pDev).AGPChipsetMapping)[0x3];
            *ccr |= 0x1;
            while ((*ccr & 0x1) != 0)
                ;

            // set the base of GART
            ((unsigned int*)AgpInfo(pDev).AGPChipsetMapping)[0x1] = physaddr;

            // set the global AGP enable bit
            osPciOrByte(handle, RCC_APER_SIZE, 0x1);
            break;

        default:
            break;
    }
}

static VOID
RCC_FlushMappings(PHWINFO pDev, U032 handle)
{
    volatile U032 *ccr;    // cache control register

    //
    // We could invalidate individual entries, but for starters, just
    // invalidate the whole cache (and poll until it's done).
    //
    ccr = &((U032 *)AgpInfo(pDev).AGPChipsetMapping)[0x3];
    *ccr |= 0x1;
    while ((*ccr & 0x1) != 0)
        ;
}

static U032
RCC_GetAperSize(PHWINFO pDev, U032 handle)
{
    unsigned int apsize = osPciReadWord(handle, RCC_APER_SIZE); // aperture size
    switch ( (apsize & 0xE) >> 1 )   /* mask and shift bits 1-3 */
    {
        case 0x00:
            apsize = (  32 * 1024 * 1024);
            break;
        case 0x01:
            apsize = (  64 * 1024 * 1024);
            break;
        case 0x02:
            apsize = ( 128 * 1024 * 1024);
            break;
        case 0x03:
            apsize = ( 256 * 1024 * 1024);
            break;
        case 0x04:
            apsize = ( 512 * 1024 * 1024);
            break;
        case 0x05:
            apsize = (1024 * 1024 * 1024);
            break;
        case 0x06:
            apsize = (2024 * 1024 * 1024);
            break;
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Intel bad apsize!\n");
            apsize = 0;
    }

#if 1
    // I'm having problems under Linux if I go greater than 32 Megs...
    if (apsize > AGP_APERTURE_SIZE) 
    {
        apsize = AGP_APERTURE_SIZE;
        // XXX - translate new aperture size back to hw-encoding
        osPciAndWord(handle, RCC_APER_SIZE, ~(0xe) /* apsize */);
    }
#endif

    return apsize;
}

static U032
RCC_GetAperBase(PHWINFO pDev, U032 handle)
{
    U032 handle_func0;
    U016 vendorID, deviceID;

    //
    // Note, for RCC the handle is for func 1, but BAR0/BAR1 are in func 0
    // space, so we have to temporarily generate a func 0 handle.
    //
    handle_func0 = osPciInitHandle(AgpInfo(pDev).NBAddr.bus,
                                   AgpInfo(pDev).NBAddr.device,
                                   0x0,            // func 0
                                   &vendorID, &deviceID);

    return osPciReadDword(handle_func0, PCI_BASE_ADDRESS_0);
}

static U032
RCC_GetGartSize(PHWINFO pDev, U032 handle)
{
    unsigned int gart_size;
    gart_size = ((AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE - 1) / RM_PAGE_SIZE) * 4;
    gart_size += RM_PAGE_SIZE;     // this is for the 1st-level lookup
    return gart_size;
}

static U032 
RCC_EarlyInit(PHWINFO pDev, U032 handle)
{
    U032 handle_func0, physaddr;
    U016 vendorID, deviceID;

    if (AgpInfo(pDev).AGPChipset != CS_RCC_6585HE)
        return 1;

    //
    // Note, for RCC the handle is for func 1, but BAR0/BAR1 are in func 0
    // space, so we have to temporarily generate a func 0 handle.
    //
    handle_func0 = osPciInitHandle(AgpInfo(pDev).NBAddr.bus,
                                   AgpInfo(pDev).NBAddr.device,
                                   0x0,            // func 0
                                   &vendorID, &deviceID);

    // The RCC chipset registers, like AMD are mapped at BAR1.
    physaddr = (osPciReadDword(handle_func0, PCI_BASE_ADDRESS_1) & ~RM_PAGE_MASK);
    AgpInfo(pDev).AGPChipsetMapping = (VOID_PTR)osMapKernelSpace(physaddr,
                                                                 RM_PAGE_SIZE,
                                                                 FALSE);
    if (AgpInfo(pDev).AGPChipsetMapping == (VOID_PTR)NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, 
        "NVRM: *** Failed to map RCC 6585HE's AGP chipset registers\n");
        return 0;
    }
    return 1;
}

static VOID
RCC_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr)
{
    U032 i, *baseptr, tableptr;

    //
    // Convert the first allocated GART page into the first level GART lookup,
    // where each GART directory entry is just the subsequent 4k page.
    // We can derive the following entries, because we know the pages are
    // contiguous.
    //
    baseptr  = (U032 *)AgpInfo(pDev).AGPGartBase;
    tableptr = physaddr + RM_PAGE_SIZE;    // GART directory entries start on the 2nd page

    for (i = 0; i < ((AgpInfo(pDev).AGPLimit + 1) / BYTES_MAPPED_PER_PAGE); i++)
    {
        *baseptr = tableptr;

        tableptr += RM_PAGE_SIZE;   // next phys GART table address
        baseptr++;                  // next GART directory entry
    }
        
    //
    // Move the AGPGartBase past the first level GART page to where the GART
    // tables really begin (this makes nvagp_LoadGARTEntries() common).
    //
    (U008 *)AgpInfo(pDev).AGPGartBase += RM_PAGE_SIZE;
}

static VOID
RCC_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID)
{
    /* nothing to do... */
    return;
}

static VOID
RCC_DisableChipset(PHWINFO pDev, U032 handle)
{
    // paranoia
    if (!AgpInfo(pDev).AGPChipsetMapping)
        return;

    // enable GART directory cache
    // nv_printf("disabling GART directory cache\n");
    // osPciOrWord(handle, PCI_DEVICE_SPECIFIC + 0x72, 0x2);

    // enable GART table cache and errors
    ((unsigned char*)AgpInfo(pDev).AGPChipsetMapping)[0x2] &= ~(0x9);

    // base of GART
    ((unsigned int*)AgpInfo(pDev).AGPChipsetMapping)[0x1] = 0x0;

    // unmap RCC AGP registers
    osUnmapKernelSpace((void *) AgpInfo(pDev).AGPChipsetMapping, RM_PAGE_SIZE);
}

static VOID
RCC_FlushPostedWriteBuffer(PHWINFO pDev)
{
    volatile U008 *pwbcr;    // posted writebuffer control register

    // flush the write buffer and wait until it's completed
    pwbcr = &((U008 *)AgpInfo(pDev).AGPChipsetMapping)[0x14];
    *pwbcr |= 0x1;
    while ((*pwbcr & 0x1) != 0)
        ;
}
#endif

#ifdef SUPPORT_MICRON_AGP
BOOL 
Micron_IsChipset(PHWINFO pDev)
{
    switch (AgpInfo(pDev).AGPChipset) 
    {
        case CS_MICRON_SAMDDR:
            return TRUE;
    }
    return FALSE;
}

U032
Micron_GetChipset(PHWINFO pDev, U032 device)
{
    switch(device)
    {
        case 0x3120:
            return CS_MICRON_SAMDDR;
    }

    return 0;
}

static VOID Micron_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID Micron_FlushMappings(PHWINFO pDev, U032 handle);
static U032 Micron_GetAperSize(PHWINFO pDev, U032 handle);
static U032 Micron_GetAperBase(PHWINFO pDev, U032 handle);
static U032 Micron_GetGartSize(PHWINFO pDev, U032 handle);
static U032 Micron_EarlyInit(PHWINFO pDev, U032 handle);
static VOID Micron_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr);
static VOID Micron_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID);
static VOID Micron_DisableChipset(PHWINFO pDev, U032 handle);

VOID
Micron_SetupFuncs(U032 chipset, agp_chipset_funcs *funcs)
{
    funcs->enable_mappings = Micron_EnableMappings;
    funcs->flush_mappings  = Micron_FlushMappings;
    funcs->get_aper_size   = Micron_GetAperSize;
    funcs->get_aper_base   = Micron_GetAperBase;
    funcs->get_gart_size   = Micron_GetGartSize;
    funcs->earlyinit       = Micron_EarlyInit;
    funcs->fixup_gart      = Micron_FixupGart;
    funcs->apply_updates   = Micron_ApplyChipsetUpdates;
    funcs->disable_chipset = Micron_DisableChipset;
    funcs->initialized = 1;
    funcs->sba_capable = 0;
}

static VOID
Micron_EnableMappings(PHWINFO pDev, U032 handle, U032 physaddr)
{
    switch (AgpInfo(pDev).AGPChipset)    
    {
        case CS_MICRON_SAMDDR:
            osPciWriteDword(handle, INTEL_GART_BASE, physaddr); // base of GART
            osPciOrDword(handle,    INTEL_GART_CONTROL, 0x80);  // enable GTLB
            osPciOrDword(handle,    INTEL_CHIPSET_CONFIG, 0x08);  // enable global access
        default:
            break;
    }
}

static VOID
Micron_FlushMappings(PHWINFO pDev, U032 handle)
{
    // Invalidate the GTLB
    if (!handle)
        return;
    osPciAndDword(handle, INTEL_GART_CONTROL, ~(0x80));
    osPciOrDword(handle,  INTEL_GART_CONTROL, 0x80);
}


static U032
Micron_GetAperSize(PHWINFO pDev, U032 handle)
{
    unsigned int apsize;
    apsize = osPciReadWord(handle, INTEL_APER_SIZE); // aperture size
    switch (apsize)
    {
        case 0x3f:
            apsize = (4 * 1024 * 1024);
            break;
        case 0x3e:
            apsize = (8 * 1024 * 1024);
            break;
        case 0x3c:
            apsize = (16 * 1024 * 1024);
            break;
        case 0x38:
            apsize = (32 * 1024 * 1024);
            break;
        case 0x30:
            apsize = (64 * 1024 * 1024);
            break;
        case 0x20:
            apsize = (128 * 1024 * 1024);
            break;
        case 0x00:
            apsize = (256 * 1024 * 1024);
            break;
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Intel bad apsize!\n");
            apsize = 0;
    }

    // I'm having problems under Linux if I go greater than 32 Megs...
    if (apsize > AGP_APERTURE_SIZE) 
    {
        apsize = AGP_APERTURE_SIZE;
        // XXX - translate new aperture size back to hw-encoding
        osPciWriteWord(handle, INTEL_APER_SIZE, 0x38 /* apsize */);
    }
    return apsize;
}

static U032
Micron_GetAperBase(PHWINFO pDev, U032 handle)
{
    return osPciReadDword(handle, PCI_BASE_ADDRESS_0);
}

static U032
Micron_GetGartSize(PHWINFO pDev, U032 handle)
{
    /* XXX - change 4 to sizeof(void *)?? */
    return ((AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE) / RM_PAGE_SIZE) * 4;
}

static U032 
Micron_EarlyInit(PHWINFO pDev, U032 handle)
{
    /* nothing to do... */
    return 1;
}

static VOID
Micron_FixupGart(PHWINFO pDev, U032 handle, U032 physaddr)
{
    /* nothing to do... */
}

static VOID
Micron_ApplyChipsetUpdates(PHWINFO pDev, U032 handle, U016 deviceID)
{
    /* nothing to do... */
}

static VOID
Micron_DisableChipset(PHWINFO pDev, U032 handle)
{
    /* nothing to do... */
}
#endif


// END OF CHIPSET-SPECIFIC CODE
// the rest of this file is generic AGP support

//
// Determine which chipset we're using (from available options)
// and initialize chipset-specific functions
//
RM_STATUS
NvInitChipset(PHWINFO pDev, U032 *handle, U032 *chipset)
{
    U016 vendor_id, device_id;

    AGP_SELECT_DEVICE(*handle, &vendor_id, &device_id);
    if (!*handle) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Unable to get PCI handle\n");
        return RM_ERR_OPERATING_SYSTEM;
    }

#ifdef SUPPORT_INTEL_AGP
    // Check Intel ...
    if (vendor_id == PCI_VENDOR_ID_INTEL)
    {
        *chipset = Intel_GetChipset(pDev, device_id);
        Intel_SetupFuncs(*chipset, agp_current);
    }
#endif

#ifdef SUPPORT_AMD_AGP
    // Check AMD ...
    if (vendor_id == PCI_VENDOR_ID_AMD)
    {
        *chipset = AMD_GetChipset(pDev, device_id);
        AMD_SetupFuncs(*chipset, agp_current);
    }
#endif

#ifdef SUPPORT_VIA_AGP
    // Check Via ...
    if (vendor_id == PCI_VENDOR_ID_VIA)
    {
        *chipset = Via_GetChipset(pDev, device_id);
        Via_SetupFuncs(*chipset, agp_current);
    }
#endif

#ifdef SUPPORT_RCC_AGP
    // Check RCC/ServerWorks ...
    if (vendor_id == PCI_VENDOR_ID_RCC)
    {
        *chipset = RCC_GetChipset(pDev, device_id);
        RCC_SetupFuncs(*chipset, agp_current);
    }
#endif
#ifdef SUPPORT_MICRON_AGP
    // Check Micron ...
    if (vendor_id == PCI_VENDOR_ID_MICRON)
    {
        *chipset = Micron_GetChipset(pDev, device_id);
        Micron_SetupFuncs(*chipset, agp_current);
    }
#endif

    //
    // Set the AGP Cap offset.
    // By offsetting to get to the chipset's AGP_STATUS/AGP_COMMAND, more common
    // code is used, since accessing these registers is now chipset independent.
    // 
    NvSetCapIDOffset(pDev, *handle, &AgpInfo(pDev).AGPCapPtr);
    return RM_OK;
}


//
// If the AGP chipset is 2.0 compliant, we'll set the AGP rate in a
// chipset independent way.
// 
// This is called at boot time from NvUpdateAGPConfig based on a
// registry key, but could also be called in response to a ConfigSet
// called after boot up.
//
RM_STATUS
NvSetAGPRate(PHWINFO pDev, U032 handle, U032 *agprate)
{
    U032 agp_status, agp_command; 

    if (handle == 0)
        return RM_ERROR;    // handle hasn't been initialized

    // return, if the capability offset for the chipset is NULL
    if (AgpInfo(pDev).AGPCapPtr == 0x0)
        return RM_ERROR;

    // 
    // If AGP FW's are already enabled, avoid trying to change the
    // AGP rate, since this can cause a hang.
    //
    if (REG_RD_DRF(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE) ==
        NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE_ON)
        return RM_ERROR;

    //
    // Compare the rate with what the chipset and NV can do
    //
    agp_status = osPciReadDword(handle, AGP_STATUS);
    *agprate &= agp_status;
    *agprate &= REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_RATE);

    /* we now have a mask of acceptable agp rates, but this may 
     * be more than just one rate. Choose the highest rate of
     * the available, and set the chipsets to that rate
     */
    if (*agprate & 0x4) *agprate = 0x4;
    else 
    if (*agprate & 0x2) *agprate = 0x2;
    else 
    if (*agprate & 0x1) *agprate = 0x1;

    if (*agprate)
    {
       // Looks to be a valid rate for the chipset and NV, set it.
        agp_command = osPciReadDword(handle, AGP_COMMAND);
        agp_command &= ~0xF;    // clear the current rate
        agp_command |= *agprate;
        osPciWriteDword(handle, AGP_COMMAND, agp_command);

        FLD_WR_DRF_NUM(_PBUS, _PCI_NV_19, _AGP_COMMAND_DATA_RATE, *agprate);
    }

    return RM_OK;
}

RM_STATUS
NvSetAGPFastwrites(PHWINFO pDev, U032 handle, U032 agpfw)
{
    U032 agp_status, agp_command; 
    INTERRUPT_SETUP

    if (handle == 0)
        return RM_ERROR;    // handle hasn't been initialized

    // return, if the capability offset for the chipset is NULL
    if (AgpInfo(pDev).AGPCapPtr == 0x0)
        return RM_ERROR;

    //
    // Compare the fastwrite bit with what the chipset and NV can do
    //
    agp_status = osPciReadDword(handle, AGP_STATUS);
    agpfw &= (agp_status >> 0x4);
    agpfw &= REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_FW);

    agp_command = osPciReadDword(handle, AGP_COMMAND);
    agp_command &= ~0x10;    // clear the current fw bit
    agp_command |= (agpfw << 0x4);

    CLI

    osPciWriteDword(handle, AGP_COMMAND, agp_command);
    FLD_WR_DRF_NUM(_PBUS, _PCI_NV_19, _AGP_COMMAND_FW_ENABLE, agpfw);

    STI

    return RM_OK;
}

RM_STATUS
NvSetAGPSideBand(PHWINFO pDev, U032 handle, U032 agpsba)
{
    U032 agp_status, agp_command; 

    if (handle == 0)
        return RM_ERROR;    // handle hasn't been initialized

    // return, if the capability offset for the chipset is NULL
    if (AgpInfo(pDev).AGPCapPtr == 0x0)
        return RM_ERROR;

    agp_status = osPciReadDword(handle, AGP_STATUS);
    agpsba &= (agp_status >> 0x9);

    //
    // If we're turning on SBA (and the chipset supports it), make sure it's
    // also in our AGP_STATUS before we enable ourselves.
    //
    if (agpsba &&
        (REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_SBA) == NV_PBUS_PCI_NV_18_AGP_STATUS_SBA_NONE)) {
        U032 strapbits;

        // Write the STRAP_AGP_SBA enable bit
        strapbits  = REG_RD32(NV_PEXTDEV_BOOT_0);
        strapbits &= ~(DRF_MASK(NV_PEXTDEV_BOOT_0_STRAP_AGP_SBA) <<
                       DRF_SHIFT(NV_PEXTDEV_BOOT_0_STRAP_AGP_SBA));
        strapbits |= DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_AGP_SBA, _ENABLED);
        REG_WR32(NV_PEXTDEV_BOOT_0, (strapbits |
                                     DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_OVERWRITE, _ENABLED)));

        // Read it back to make sure the write's completed
        strapbits = (volatile)REG_RD32(NV_PEXTDEV_BOOT_0);

        // If AGP_STATUS still indicates SBA isn't supported, then it's not
        if (REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_SBA) == NV_PBUS_PCI_NV_18_AGP_STATUS_SBA_NONE) {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NvSetAGPSideBand: SBA isn't supported\n");
            return RM_ERROR;
        }
    }

    // mask and set the chipsets SBA bit
    agp_command  = osPciReadDword(handle, AGP_COMMAND);
    agp_command &= ~(DRF_MASK(NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE) <<
                     DRF_SHIFT(NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE));
    agp_command |= DRF_NUM(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, agpsba);
    osPciWriteDword(handle, AGP_COMMAND, agp_command);

    // set our SBA enable bit
    FLD_WR_DRF_NUM(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, agpsba);

    return RM_OK;
}

//
// Determine if any updates are needed to the AGP rate or state of AGP fastwrites.
//
VOID
NvUpdateAGPConfig(PHWINFO pDev)
{
    U032 handle;
    U032 agprate, agpfw, agpfwstrap, agpsba;
    U016 vendorID, deviceID;

    // return if AGP is not enabled
    if (REG_RD_DRF(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE) == NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE_OFF)
        return;

    AGP_SELECT_DEVICE(handle, &vendorID, &deviceID);
    if (!handle)
        return;

    /* this may be our first entry point under 9x/w2k, so
     * if we're not initialized, call NvInitChipset(), which
     * will only detect the chipset and set up our data structures.
     */
    if (!agp_current->initialized) 
    {
        U032 dummy;
        NvInitChipset(pDev, &handle, &dummy);
    }

    //
    // Before applying the chipset updates, look for a registry key that changes
    // the strap bits. This impacts what apply_updates will do and any registry
    // overrides (but, doesn't affect normal boot time AGP configuration).
    // 
    if (IsNV10orBetter(pDev) &&
        (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                           "EnableAGPFWStrap", &agpfwstrap) == RM_OK)) {
        U032 strapbits;
        strapbits  = REG_RD32(NV_PEXTDEV_BOOT_0);
        strapbits &= ~(DRF_MASK(NV_PEXTDEV_BOOT_0_STRAP_AGP_FASTWR) <<
                       DRF_SHIFT(NV_PEXTDEV_BOOT_0_STRAP_AGP_FASTWR));
        if (agpfwstrap)
            strapbits |= DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_AGP_FASTWR, _ENABLED);
        else
            strapbits |= DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_AGP_FASTWR, _DISABLED);

        // OR-in the _STRAP_OVERWRITE_ENABLED bit before writing
        REG_WR32(NV_PEXTDEV_BOOT_0, (strapbits |
                                     DRF_DEF(_PEXTDEV, _BOOT_0, _STRAP_OVERWRITE, _ENABLED)));
    }

    agp_current->apply_updates(pDev, handle, deviceID);

    // 
    // After applying any chipset updates and/or workarounds, see if there's an
    // AGP command overrides in the registry. We've seen where the Via chipset
    // driver later resets the AGP rate, so this key may not work there.
    //

    // set the AGP transfer rate
    if (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                               "ReqAGPRate",  &agprate) == RM_OK) {
        NvSetAGPRate(pDev, handle, &agprate);
    }

    // set/clear the AGP sideband addressing bit
    if (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                "EnableAGPSBA", &agpsba) == RM_OK) {
        NvSetAGPSideBand(pDev, handle, agpsba);
    }

    if (IsNV11orBetter(pDev)) {
        U032 agpfw_adis;

        // allows switching between fullspeed FW's and NV10-style compat mode
        if (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                   "DisableAGPFWDisconnect", &agpfw_adis) == RM_OK)
            FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _AGPFW_ADIS, agpfw_adis);
    }

    // set/clear the AGP fastwrite bit
    if (OS_READ_REGISTRY_DWORD(pDev, RM_REGISTRY_AGP_BASE_PATH,
                                "EnableAGPFW", &agpfw) == RM_OK) {
        NvSetAGPFastwrites(pDev, handle, agpfw);
    }
}


//
// Return the AGP base/limit from the chipset. This function assumes
// the aperture base and size can be determined from PCI BAR0 in config
// space.
//
RM_STATUS
NvGetAGPBaseLimit(PHWINFO pDev, U032 *base, U032 *limit)
{
    U032 aper0, apoffset, mask;
    U032 size;
    U032 handle;

#ifdef _WIN64
    //
    // XXX This code is specific to the 460GX chipset that's used with
    // Itanium with WIN64.
    //
    // It's possible NvSetBusDeviceFunc() may now be able to detect the
    // location of the Northbridge, rather than hardcode the bus/device/func
    // (talking to one of the Intel BIOS guys, the device/func are probably
    // correct, but it can be on a different bus.
    //
    U016 vendorID, deviceID;
    U032 aper1, apsize;
    U008 bus, device, func;

    //
    // Temporary hack to get AGP working on the 460GX
    // (hardcode the NB location at Bus 4, Device 14, Func 1)
    //
    // At some point, we'll want to see if this works (when
    // we're feeling daring with a little time to spare).
    //
    //     NvSetBusDeviceFunc(pDev, &bus, &device, &func);
    //
    bus    = 0x4;
    device = 0x14;
    func   = 0x1;
    handle = osPciInitHandle(bus, device, func, &vendorID, &deviceID);

    if ((vendorID != 0x8086) || ((deviceID != 0x84EA) && (deviceID != 0x84E2))) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: didn't find expected 460GX vendor/device ID's\n");
        return RM_ERROR;
    }

    //
    // read APSIZE holds the size (limit + 1) and bit 3 determine which
    // PCI offset holds the Aperture base.
    //
    apsize = osPciReadByte(handle, 0xA2);
    switch (apsize & 0x7) {
    case 0x1:   // 256 Mb
    case 0x2:   // 1 Gb
        break;
    case 0x4:   // 32GB
    default :
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: too big an Aperture size\n");
        break;
    }
    
    if (apsize & 0x8)
        apoffset = 0x98; // use BAPBASE, which are bits 0x98->0x9F
    else
        apoffset = 0x10; // use APBASE, which are bits 0x10->0x17

    aper0 = osPciReadDword(handle, apoffset);
    if ((aper0 & 0x6) == 0x4)
        aper1 = 0x0;
    else
        aper1 = osPciReadDword(handle, apoffset + 0x4);

    // 
    // If the upper 32bit's of the aperture are set, we're in trouble.
    // Because we're a 32bit device, this better be 0's, since we won't
    // be able to generate the proper address.
    //
    if (aper1) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Aperture base above 4GB\n");
        return RM_ERROR;
    }

#else
    U016 dummy;

    AGP_SELECT_DEVICE(handle, &dummy, &dummy);
    if (!handle)
        return RM_ERROR;

    apoffset = PCI_BASE_ADDRESS_0;

    // read BAR0 to get the base

    aper0 = osPciReadDword(handle, apoffset);
#endif

    *base = aper0 & 0xFFFFF000;

    // write -1 and then read BAR0 to get the size mask
    osPciWriteDword(handle, apoffset, 0xFFFFFFFF);
    mask = osPciReadDword(handle, apoffset);
    *limit = (mask & 0xFFFFF000) ^ 0xFFFFFFFF;

    // restore original aperture value
    osPciWriteDword(handle, apoffset, aper0);

    // limit should be a power of 2 (i.e. size has one bit set)
    size = *limit + 1;    
    if ((size & (size - 1)) != 0)
        return RM_ERROR;

    return RM_OK;
}


VOID
NvEnableAGP(PHWINFO pDev)
{
    U032 handle;
    unsigned short dummy;

    AGP_SELECT_DEVICE(handle, &dummy, &dummy);
    if (!handle) return;

    /* set AGP_ENABLE bit */
    osPciOrWord(handle, AGP_COMMAND, 0x100);
}

VOID
NvDisableAGP(PHWINFO pDev)
{
    U032 handle;
    unsigned short dummy;
                
    AGP_SELECT_DEVICE(handle, &dummy, &dummy);
    if (!handle) return;

    /* clear AGP_ENABLE bit */
    osPciAndWord(handle, AGP_COMMAND, ~0x100);
}

/*
 * Basic AGP config/turn-on. 
 * Let chipset-specific code deal with workarounds in update.
 * Fast-write "stuff" is there.
 */
VOID
NvSetupAGPConfig(PHWINFO pDev)
{
    unsigned int do_sba, rq_depth, chipset_rq_depth;
    unsigned short dummy;
    U032 agprate;
    U032 handle;
    INTERRUPT_SETUP
                
    AGP_SELECT_DEVICE(handle, &dummy, &dummy);
    if (!handle) return;

    //
    // At init time, an AGP rate has not been established, so max it out.
    // If this needs to be downgraded, it'll be done in the apply_updates
    // routine.
    //
    agprate = 0x7;
    if (RM_OK != NvSetAGPRate(pDev, handle, &agprate))
        return;

    /* setup AGP state */
    // Can we do side-band addressing (we assume the chipset can do SBA)??
    do_sba = REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_SBA);
    do_sba &= agp_current->sba_capable;
    if (do_sba) {
        osPciOrWord(handle,  AGP_COMMAND, 0x0200);
    } else {
        osPciAndWord(handle, AGP_COMMAND, ~0x0200);    // make sure it's off
    }

    //
    // Determine the maximum AGP request depth, our value must be less
    // than or equal to the maximum the chipset can handle.
    //
    rq_depth         = REG_RD_DRF(_PBUS, _PCI_NV_18, _AGP_STATUS_RQ);
    chipset_rq_depth = osPciReadDword(handle, AGP_STATUS) >> 24;
    if (chipset_rq_depth < rq_depth)
        rq_depth = chipset_rq_depth;

    CLI
    REG_WR32(NV_PBUS_PCI_NV_19, DRF_NUM(_PBUS, _PCI_NV_19, _AGP_COMMAND_RQ_DEPTH, rq_depth) |
                                DRF_NUM(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE, 0x1)    |
                                DRF_NUM(_PBUS, _PCI_NV_19, _AGP_COMMAND_DATA_RATE, agprate) |
                                ((do_sba) ? DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _ON) : 0) |
                                0);
    STI
    flush_cache();
}

static RM_STATUS 
nvagp_AllocAGPBitmap
(
    PHWINFO pDev,
    U032   Size,    // in pages
    U032  *Offset
)
{
    U008 *GARTBitmap = (U008 *)AgpInfo(pDev).AGPGartBitmap;
    U032 agp_pages = (AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE) / RM_PAGE_SIZE;
    U032 i, j;

    for (i = 0; i < agp_pages; i++)
    {
        if (!GARTBitmap[i >> 3])
        {
            i += 7;
        }
        else if (GARTBitmap[i >> 3] & BIT(i & 7))
        {
            //
            // Search for string of 1's the length of the block.
            //
            j = 0;
            do
            {
                if ((++i >= agp_pages) || (++j >= Size))
                    break;
            }
            while (GARTBitmap[i >> 3] & BIT(i & 7));
            if (j == Size)
            {
                //
                // Score, a block was found.
                //
                *Offset = i - j;
                while (j--)
                {
                    //
                    // Mark AGP memory as allocated.
                    //
                    i--;
                    GARTBitmap[i >> 3] &= ~BIT(i & 7);
                }
                return (RM_OK);
            }
        }
    }
    return (RM_ERR_NO_FREE_MEM);
}


static VOID 
nvagp_FreeAGPBitmap
(
    PHWINFO pDev,
    U032 Offset,
    U032 PageCount
)
{
    U008 *GARTBitmap = (U008 *)AgpInfo(pDev).AGPGartBitmap;

    while (PageCount--)
    {
        //
        // Mark AGP memory as free.
        //
        GARTBitmap[Offset >> 3] |= BIT(Offset & 7);
        Offset++;
    }
}

static RM_STATUS 
nvagp_LoadGARTEntries
(
    PHWINFO pDev,
    VOID *pAddress,
    U032 PageCount,
    U032 Index
)
{
    U032 i;
    VOID *ucptr;    // uncached ptr
    U032 pagePhysicalAddr;
    U032 *GARTEntry = ((U032 *)AgpInfo(pDev).AGPGartBase) + Index;
    RM_STATUS status;

    for (i = 0; i < PageCount; i++)
    {
        status = osGetAGPPhysAddr( pDev, (VOID *) ((U008 *)pAddress + (i << RM_PAGE_SHIFT)), &pagePhysicalAddr);
        if (status != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NULL physical address for DMA page\n");
            return RM_ERR_NO_FREE_MEM;
        }

        // As part of loading the GART entry, create a temporary uncached mapping
        // to clean each page (must do this page by page, since the memory won't
        // be contiguous) and then free the mapping.
        //
        // It seems we can't be certain, the cache is clean w.r.t. this page, so
        // by zeroing it uncached, we hopefully force everything out preventing
        // any later writebacks to corrupt our data.

        ucptr = osMapKernelSpace(pagePhysicalAddr, RM_PAGE_SIZE, FALSE);
        if (ucptr == NULL)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unable to create uncached mapping\n");
            return RM_ERR_NO_FREE_MEM;
        }

        // clean the page
        osMemSet(ucptr, 0, RM_PAGE_SIZE);
        *(U032 *)ucptr = 0x0;

        // free the mapping
        osUnmapKernelSpace(ucptr, RM_PAGE_SIZE);

#ifdef SUPPORT_MICRON_AGP
        if (Micron_IsChipset(pDev))
            // load the GART
            GARTEntry[i] = (pagePhysicalAddr >> RM_PAGE_SHIFT) | 0x80000000;
        else 
#endif
            // load the GART
            GARTEntry[i] = (pagePhysicalAddr & ~RM_PAGE_MASK) | 1;
    }

#ifdef SUPPORT_RCC_AGP
    // Make sure to flush the posted write buffer
    if (RCC_IsChipset(pDev))
        RCC_FlushPostedWriteBuffer(pDev);
#endif
    flush_cache();
    return RM_OK;
}

static RM_STATUS 
nvagp_MapAGPAperture
(
    PHWINFO pDev,
    U032 clientClass,
    U032 index,
    U032 pagecount,
    VOID **pMdl,
    VOID **pAddress
)
{
    VOID *kernAddr;
    RM_STATUS rmStatus = RM_OK;

    // Here's the kernel mapping to the AGP Aperture
    kernAddr = (VOID *)((U008 *)AgpInfo(pDev).AGPLinearStart +
                        (index << RM_PAGE_SHIFT) + ((size_t)*pAddress & RM_PAGE_MASK));

    switch (clientClass)
    {
        case NV01_ROOT:
            *pAddress = kernAddr;
            pMdl = NULL;           // no MDL needed for this mapping
            break;

        case NV01_ROOT_USER:
            // create the user mapping to these pages
            *pMdl = *pAddress;  // linux will use this, but NT will ignore it
            *pAddress = osMapUserSpace(kernAddr, pMdl, (pagecount << RM_PAGE_SHIFT), MAP_WRITECOMBINE);
            if (*pAddress == (VOID *)NULL)
            {
                return RM_ERROR;
            }
            break;

        default:
            rmStatus = RM_ERR_OPERATING_SYSTEM;
    }

    // let's also scrub from the kernel view, to make sure
    // everything is consistent.
    osMemSet(kernAddr, 0, pagecount << RM_PAGE_SHIFT);
    *(U032 *)kernAddr = 0x0;

    return rmStatus;
}

static VOID 
nvagp_UnmapAGPAperture
(
    PHWINFO pDev,
    VOID *pAddress,
    U032 Index,
    PAGP_MEM_INFO pMemInfo
)
{
    U032 *GARTEntry = (U032 *)AgpInfo(pDev).AGPGartBase;
    U016 dummy;
    U032 handle;

    AGP_SELECT_DEVICE(handle, &dummy, &dummy);
    if (!handle)
        return;

    while (pMemInfo->PageCount--)
    {
        // invalidate the GART entries
        GARTEntry[Index++] = 0x0;
    }

#ifdef SUPPORT_RCC_AGP
    // Make sure to flush the posted write buffer
    if (RCC_IsChipset(pDev))
        RCC_FlushPostedWriteBuffer(pDev);
#endif
    flush_cache();

    //
    // If we're passed a non-NULL MDL, this means a user mapping was
    // created and needs to be cleaned up
    //
    if (pMemInfo->pMdl)
    {
        // free the user mapping
        osUnmapUserSpace(pAddress, pMemInfo->pMdl);
        pMemInfo->pMdl = NULL;
    }

    agp_current->flush_mappings(pDev, handle);

}

RM_STATUS NvAllocAGPPages
(
    PHWINFO pDev,
    VOID **pAddress,
    U032   PageCount,
    U032   ClientClass,
    VOID **pMemData
)
{
    U032          Index;
    PAGP_MEM_INFO pMemInfo;
    U032          rmStatus = RM_ERROR;

    if (!pDev) {
        return RM_ERR_NO_FREE_MEM;  // AGP not initialized, PCI card?
    }

    if (!AgpInfo(pDev).AGPLinearStart) {
        return RM_ERR_NO_FREE_MEM;  // true for PCI cards
    }

    // Since there will be some cleanup needed on the free side
    // create a struct that hooks into VOID * in CLI_MEMORY_INFO

    rmStatus = osAllocMem((VOID **)&pMemInfo, sizeof(AGP_MEM_INFO));
    if (rmStatus != RM_OK)
    {
        return rmStatus;
    }

    // Alloc the pages for the caller.
    //
    // Don't use osAllocMem, since it'll write a DEBUG pattern to the buffer
    // via a cached mapping. Just use the underlying NT routine ... using the
    // non-paged pool guarantees nothing needs to be done in osLockUserMem.
 
    rmStatus = osAllocPool(pAddress, PageCount << RM_PAGE_SHIFT);
    if (rmStatus != RM_OK)
        {
        osFreeMem(pMemInfo);
        return RM_ERR_NO_FREE_MEM;
    }

    pMemInfo->SysAddr   = *pAddress;      // keep the original system addr
    pMemInfo->PageCount = PageCount;

    // Alloc from the GART bitmap for the number of pages we'll need.
    rmStatus = nvagp_AllocAGPBitmap(pDev, PageCount, &Index);
    if (rmStatus != RM_OK)
    {
        osFreePool(pMemInfo->SysAddr);
        osFreeMem(pMemInfo);
        return rmStatus;
    }

    pMemInfo->Offset = Index;

    // Load up the GART entries with the physical addresses
    rmStatus = nvagp_LoadGARTEntries(pDev, *pAddress, PageCount, Index);
    if (rmStatus != RM_OK)
    {
        nvagp_FreeAGPBitmap(pDev, Index, PageCount);
        osFreePool(pMemInfo->SysAddr);
        osFreeMem(pMemInfo);
        return rmStatus;
    }

    // Finally, setup a user/kernel mapping within the Aperture
    rmStatus = nvagp_MapAGPAperture(pDev,
                                ClientClass,
                                Index,
                                PageCount,
                                &pMemInfo->pMdl,
                                pAddress);
   if (rmStatus != RM_OK)
   {
        nvagp_FreeAGPBitmap(pDev, Index, PageCount);
        osFreePool(pMemInfo->SysAddr);
        osFreeMem(pMemInfo);
        return rmStatus;
    }

#ifdef _WIN64
   DBG_PRINT_STRING_VAL64(DEBUGLEVEL_TRACEINFO, "NVRM: alloc user addr    ", *(U064*)pAddress);
#else
   DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: alloc user addr    ", *(U032*)pAddress);
#endif
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: mapped to agp addr ", (U032)AgpInfo(pDev).AGPPhysStart + (Index << RM_PAGE_SHIFT));

    *pMemData = (VOID *)pMemInfo;

    return RM_OK;
}

//
// osFreeAGPPages - Free a number of aligned memory pages
//
//  **pAddress - handle to the linear address
//  MemoryType - memory pool
//  *pMemData  - pointer to private per-alloc data
//
RM_STATUS NvFreeAGPPages
(
    PHWINFO pDev,
    VOID **pAddress,
    VOID  *pMemData
)
{
    PAGP_MEM_INFO pMemInfo = (PAGP_MEM_INFO)pMemData;

    if (!pDev)
        return RM_ERROR;        // this would be a misguided free

    if (!AgpInfo(pDev).AGPLinearStart)
        return RM_ERROR;        // this would be a misguided free

    nvagp_FreeAGPBitmap(pDev, pMemInfo->Offset, pMemInfo->PageCount);

    // reset the GART entries, free the mapping and flush the GTLB
    nvagp_UnmapAGPAperture(pDev, *pAddress, pMemInfo->Offset, pMemInfo);

    // free the original system pages
    osFreePool(pMemInfo->SysAddr);

    // free our data struct that kept track of this mapping
    osFreeMem(pMemInfo);

    return RM_OK;
}


//
// Here's our attempt to configure AGP on our own for NT4.
//
// First, we'll attempt to recognize the chipset to determine if we know how to
// program it. Next, we'll create a bitmap that's used to alloc/free slots from
// the GART.
//
BOOL RmInitAGP
(
    PHWINFO pDev
)
{
    VOID *tmpATT;           // temporary holder of the GART
    U032 i, status;
    U032 physaddr, apsize, apbase;
    U032 handle;
    U032 chipset = CS_UNKNOWN;
    U008 *pBitmap;
    U032 gart_size, gart_bitmap_size;

    // verify we're an AGP graphics card
    if (REG_RD_DRF(_PBUS, _PCI_NV_24, _NEXT_PTR) != NV_PBUS_PCI_NV_24_NEXT_PTR_AGP)
    {
        AgpInfo(pDev).AGPLinearStart = 0;
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    // don't initialize AGP if it's already setup
    // moved after the AGP card check so we know we should be
    // safe for the agp_restore_mappings() if needed.
    if (agp_ref_count) 
    {
        agp_ref_count++;
        return 0;
    }

    NvInitChipset(pDev, &handle, &chipset);

    if (chipset == CS_UNKNOWN)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Unrecongnized AGP chipset\n");
        AgpInfo(pDev).AGPLinearStart = 0;
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    /* if AGP is already turned on, turn it off while we configure it */
    NvDisableAGP(pDev);

    AgpInfo(pDev).AGPChipset = chipset;

    if (!agp_current->earlyinit(pDev, handle))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** AGP early init failed\n");
        AgpInfo(pDev).AGPLinearStart = 0;
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    // get aperture info
    apsize = agp_current->get_aper_size(pDev, handle);
    apbase = agp_current->get_aper_base(pDev, handle);

    if (!apsize || !apbase)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot find Aperture Base or Size\n");
        AgpInfo(pDev).AGPLinearStart = 0;
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    AgpInfo(pDev).AGPPhysStart = apbase & ~RM_PAGE_MASK;
        
    // Obtain a mapping (already marked write-combined) to the AGP Aperture
    // This represents the base of our allocations.
    AgpInfo(pDev).AGPLinearStart = 
                (VOID_PTR) osMapKernelSpace(AgpInfo(pDev).AGPPhysStart, apsize, MAP_WRITECOMBINE);
    if (AgpInfo(pDev).AGPLinearStart == (VOID_PTR)NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot map the AGP Aperture WriteCombined\n");
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    AgpInfo(pDev).AGPLimit = apsize - 1;

    //
    // Allocate space for the GART.
    //
    // This memory needs to be contiguous, 4K (RM_PAGE_SIZE) aligned and locked
    // down for the page tables. My best guess at what to use is __dma_get_pages()
    // for this. If on AMD, we'll add another page for their first level table.
    //
    gart_size = agp_current->get_gart_size(pDev, handle);

    /* this is num_pages / 8 bits for bitmasks */
    gart_bitmap_size = (AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE) / (8 * RM_PAGE_SIZE); /* don't want the lookup here */

    tmpATT = osAllocContigPages(gart_size);
    if (tmpATT == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Unable to allocate GART\n");
        AgpInfo(pDev).AGPLinearStart = 0;
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    // Get the physical addr to be loaded into the chipset
    physaddr = osGetKernPhysAddr(tmpATT); 
    physaddr = ((physaddr + (RM_PAGE_SIZE - 1)) & ~RM_PAGE_MASK); // if it's not page aligned

    // Mark these pages uncached, so we can freely update the GART
    AgpInfo(pDev).AGPGartBase = (VOID_PTR) osMapKernelSpace(physaddr, gart_size, FALSE);

    if (AgpInfo(pDev).AGPGartBase == (VOID_PTR)NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Unable to create uncached mapping for the GART\n");
        osFreeContigPages(tmpATT);
        AgpInfo(pDev).AGPLinearStart = 0;
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    osMemSet((void *)AgpInfo(pDev).AGPGartBase, 0, gart_size); /* Clear the ram out, no junk to the user */
    flush_cache();

    // allow chipset-specific modifications to gart structuring (amd...)
    agp_current->fixup_gart(pDev, handle, physaddr);

    //
    // Allocate the GART bitmap, which we'll use to manage AGP allocations.
    // Each bit in the bitmap represents a 4K mapping.
    //
    status = osAllocMem((VOID **)&AgpInfo(pDev).AGPGartBitmap, gart_bitmap_size);

    if (status != RM_OK) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Unable to allocate GART bitmap\n");
        /* osUnmapIoSpace((VOID *)AgpInfo(pDev).AGPGartBase, gart_size); */
        osFreeContigPages(tmpATT);
        AgpInfo(pDev).AGPLinearStart = 0;
        AgpInfo(pDev).AGPLimit = 0;
        return FALSE;
    }

    pBitmap = (char *)AgpInfo(pDev).AGPGartBitmap;
    for (i = 0; i < gart_bitmap_size; i++)
        pBitmap[i] = 0xFF;


    //
    // Program up the rest of the chipset.
    //
    agp_current->enable_mappings(pDev, handle, physaddr);

    /* setup chipset-specific features */
    NvSetupAGPConfig(pDev);

    /* enable chipset-specific overrides */
    NvUpdateAGPConfig(pDev);

    /* now that everything's configured, turn it on */
    NvEnableAGP(pDev);

    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: AGP Phys Base:      ", 
        (VOID *)AgpInfo(pDev).AGPPhysStart);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: AGP Linear Base:    ", 
        (VOID *)AgpInfo(pDev).AGPLinearStart);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: AGP GART Base:      ", 
        (VOID *)AgpInfo(pDev).AGPGartBase);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: AGP GART Bitmap:    ", 
        (VOID *)AgpInfo(pDev).AGPGartBitmap);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: # of GART Mappings: ", 
        (AgpInfo(pDev).AGPLimit + RM_PAGE_SIZE) / RM_PAGE_SIZE);

    // looks all good, count this as a "connected device"
    agp_ref_count++;
    AGP_SAVE_PDEV(pDev);

    return 0;

} // end of RmInitAGP()



BOOL RmTeardownAGP
(
    PHWINFO pDev
)
{
    unsigned short dummy;
    U032 handle;

    AGP_SELECT_DEVICE(handle, &dummy, &dummy);
    if (!handle)
        return 0;

    agp_ref_count--;

    // Don't tear down AGP if someone is still using it
    // (like a second X Server)
    if (agp_ref_count != 0) 
        return 0;

    agp_current->disable_chipset(pDev, handle);
    NvDisableAGP(pDev);

    if (AgpInfo(pDev).AGPLinearStart) 
        osUnmapKernelSpace( (void *) AgpInfo(pDev).AGPLinearStart, 
            AgpInfo(pDev).AGPLimit + 1);
    AgpInfo(pDev).AGPLinearStart = 0;

    if (AgpInfo(pDev).AGPGartBase) 
        osFreeContigPages( (void *) AgpInfo(pDev).AGPGartBase);
    AgpInfo(pDev).AGPGartBase = 0;

    return 0;
}


//
// Return the Bus, Device, Func numbers of the Northbridge.
//
// We determine this by looking for the another AGP device that's not us
// and is a bridge device (so, we hopefully avoid an integrated graphics
// device). We should check for a host bridge device where the subclass
// is PCI_SUBCLASS_BR_HOST, but this doesn't work on the RCC chipset.
//
BOOL
NvSetBusDeviceFunc
(
    PHWINFO pDev
)
{
        PRMINFO pRmInfo = (PRMINFO)pDev->pRmInfo;
    U008 bus, device, func;
    U032 handle, cap_offset;
    U016 vendorID, deviceID;
    RM_STATUS status;

    // return it, if we've got it already
    if (pRmInfo->AGP.NBAddr.valid)
        return TRUE;

    // we're checking all the device/funcs for the first 5 buses
    for (bus = 0; bus < 5; bus++)
    {
        for (device = 0; device < PCI_MAX_DEVICES; device++)
        {
            for (func = 0; func < PCI_MAX_FUNCTION; func++)
            {
                // read at bus, device, func
                handle = osPciInitHandle(bus, device, func, &vendorID, &deviceID);
                if (!handle)
                    break;

                if (vendorID == PCI_INVALID_VENDORID)
                    break;           // skip to the next device

                if (REG_RD32(NV_PBUS_PCI_NV_0) == (U032)(deviceID << 16 | vendorID))
                    break;           // skip over ourselves

                if (osPciReadByte(handle, 0xB) != PCI_CLASS_BRIDGE_DEV)
                    break;           // not a bridge device

                // look for the AGP Capability ID
                status = NvSetCapIDOffset(pDev, handle, &cap_offset);
                if (status == RM_OK)
                {
                    // Found it
                    AgpInfo(pDev).NBAddr.bus    = bus;
                    AgpInfo(pDev).NBAddr.device = device;
                    AgpInfo(pDev).NBAddr.func   = func;
                    AgpInfo(pDev).NBAddr.valid  = 0x1;
                    return TRUE;
                }

                if ((osPciReadDword(handle, 0xE) & PCI_MULTIFUNCTION) == 0)
                    break;        // no need to cycle through functions
            }
        }
    }

    // This is bad, we didn't find the NB device (assume bus0/device0/func0)
    AgpInfo(pDev).NBAddr.bus    = 0x0;
    AgpInfo(pDev).NBAddr.device = 0x0;
    AgpInfo(pDev).NBAddr.func   = 0x0;
    AgpInfo(pDev).NBAddr.valid  = 0x1;
    DBG_BREAKPOINT();

    return FALSE;
}

RM_STATUS
NvSetCapIDOffset
(
    PHWINFO pDev, 
    U032 handle,
    U032 *cap_offset
)
{
    U008 cap_next; 
    U032 agp_caps; 

    if (handle == 0)
        return RM_ERROR;    // handle hasn't been initialized

    if ((osPciReadDword(handle, 0x4) & 0x00100000) == 0)
        return RM_ERROR;    // chipset doesn't support capability ptrs

    // find the PCI offset for the AGP Cap ID
    cap_next = osPciReadByte(handle, PCI_CAPABILITY_LIST);
    while (cap_next)
    {
        agp_caps = osPciReadDword(handle, cap_next);
        if ((agp_caps & 0xFF) == 0x2)
            break;      // found the AGP Cap ID
        cap_next = (U008)((agp_caps >> 8) & 0xFF);            
    }

    if (cap_next == 0)
        return RM_ERROR;    // didn't find the AGP capid

    *cap_offset = cap_next;
    return RM_OK;
}
#endif

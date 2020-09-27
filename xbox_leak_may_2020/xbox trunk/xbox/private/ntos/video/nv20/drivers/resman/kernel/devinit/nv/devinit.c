 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/**************************************************************************************************************
*
*   Module: devinit.c
*
*   Description:
*       Device POST code.
*
*   Revision History:
*       Original -- 2/99 Jeff Westerinen
*
**************************************************************************************************************/


#ifdef NV_PORTABLE

/* "portable" form for use outside resman and by external source customers */
/* look in devinit/portable/ for more details */
#include <nv_portable.h>
#include <nv_ref.h>
#include <nv4_ref.h>
#include <devinit.h>

#else   /* not NV_PORTABLE --> Nvidia internal */

#include <nvrm.h>
#include <nvhw.h>
#include <nv4_ref.h>
#include <os.h>
#include <devinit.h>

#endif

// TO DO: this is a hack to avoid osallocmem() from an isr (the TMDS bip3 interpret is called during modeset)
#define USE_STATIC_BIP3_TABLES

#ifdef USE_STATIC_BIP3_TABLES
static BIOS_BIP3_TABLES bip3_static_tables;
#endif


// ***** MAIN ENTRY POINT FOR DEVICE INITIALIZATIONS *****

RM_STATUS DevinitInitializeDevice
(
    PHWINFO pDev,
    PHWREG  externalBiosImage,
    BOOL   *pBiosUsedToInit
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 offset;
    U008 lock;
    PHWREG tempBiosAddr;
    BMP_Control_Block bmpCtrlBlk;
    U032 Head = 0;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: initializing device...\n");

    // enable VGA subsystem
    FLD_WR_DRF_DEF(_PBUS,_PCI_NV_21,_VGA,_ENABLED);

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);

    // determine current strap crystal frequency (in Hz), if it has not already been set
    if (pDev->Chip.HalInfo.CrystalFreq == 0)
    {
        if (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_CRYSTAL) == NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_13500K)
            pDev->Chip.HalInfo.CrystalFreq = 13500000;
        else if (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_CRYSTAL) == NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180)
            pDev->Chip.HalInfo.CrystalFreq = 14318180;
    }

    // be pessimistic
    *pBiosUsedToInit = FALSE;

    // use the external bios image, if present
    tempBiosAddr = biosAddr;
    if (externalBiosImage)
    {
        biosAddr = externalBiosImage;
    }

    // pull in the BMP control block
    rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);

    // process the initialization code
    if (rmStatus == RM_OK)
    {
        // use the BIOS image to init
        if ((bmpCtrlBlk.CTL_Version >= BMP_INIT_TABLE_VER_5) &&
            (bmpCtrlBlk.CTL_Flags >= BMP_INIT_TABLE_SUBVER_0x10))
        {
            // initialize the device using BIP/3 code
            rmStatus = DevinitProcessBip3(pDev);
            if (rmStatus == RM_OK)
                *pBiosUsedToInit = TRUE;
        }
        else
        {
            // initialize the device using BIP/2 code
            rmStatus = DevinitProcessBip2(pDev, offset, bmpCtrlBlk, pBiosUsedToInit);
        }
    }
    else
    {
        // use the static tables to init
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: cannot find BMP Control Block, using static tables...\n");
        rmStatus = DevinitProcessStaticTables(pDev);
    }

    // restore original biosAddr
    biosAddr = tempBiosAddr;

    RestoreLock(pDev, Head, lock);

    // enable shadowing of BIOS
    FLD_WR_DRF_DEF(_PBUS,_PCI_NV_20,_ROM_SHADOW,_ENABLED);

    if (rmStatus == RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: ...device initialization succeeded\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ...device initialization *** FAILED ***\n");
    }

    return rmStatus;

} // end of DevinitInitializeDevice()

RM_STATUS DevinitGetBMPControlBlock
(
    PHWINFO             pDev,
    BMP_Control_Block  *pControlBlock,
    U032               *offset
)
{
    RM_STATUS rmStatus = RM_ERROR;
    int i;

    // attempt to find the init info in the BIOS
    for (i = 0; i < MAX_BIOS_SCAN-3; i++)
        if (BIOS_RD32(i) == CTRL_BLK_ID)
            break;

    if (i < MAX_BIOS_SCAN-3)
    {
        int so;

        // going to recalc offset, so make sure it is zero now since the
        //   following read must happen at exactly 'i'
        pDev->biosOffset = 0;

        rmStatus = BiosReadStructure(pDev,
                                     (U008*) pControlBlock,
                                     i,
                                     (U032 *) 0,
                                     BMP_CONTROL_BLOCK_FMT);
        if (rmStatus != RM_OK)
            goto done;

        // The BMP tables have offsets with them relative to the beginning
        //   of the rom.  If the rom has been relocated, these offsets will
        //   be bogus.
        //
        //   This happens on Mac roms when the BMP tables are relocated
        //   from their normal location in the pc bios image to the end of the
        //   openforth image.
        //
        //   Calculate an 'extra offset' to compensate and save it in the pDev
        //

        for (so = i-2; so >= 0; so--)
            if ((BIOS_RD08(so) == 0x55) && (BIOS_RD08(so+1) == 0xAA))
                break;
        if (so >= 0)
            pDev->biosOffset = so;

        // where did we get the table from?
        *offset = i - pDev->biosOffset;
    }

 done:
    return rmStatus;
}


// ***** CORE 2 SCRIPT PROCESSING *****

RM_STATUS DevinitProcessBip2
(
    PHWINFO pDev,
    U032 offset,
    BMP_Control_Block bmpCtrlBlk,
    BOOL* pBiosUsedToInit
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 genInitTableOffset, extraInitTableOffset, sizeInitTable, memInitTableOffset, SDRSeqTableOffset, DDRSeqTableOffset;
    devinit_tables_t *dt = 0;

    rmStatus = DevinitGetInitTableInfo(pDev,
                                       offset,
                                       &bmpCtrlBlk,
                                       &genInitTableOffset,
                                       &extraInitTableOffset,
                                       &sizeInitTable,
                                       &memInitTableOffset,
                                       &SDRSeqTableOffset,
                                       &DDRSeqTableOffset);

    if (rmStatus != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: invalid BIOS init info version\n");
        goto fallback;
    }

    if (sizeInitTable > BMP_INIT_CODE_SIZE_MAX)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: genInitData too large\n");
        goto fallback;
    }

    // allocate space for init tables
    rmStatus = osAllocMem((VOID **) &dt, sizeof(*dt));
    if (rmStatus != RM_OK)
        goto done;
    dt->genInitData   = (U008 *) 0;      // allocate later based on actual size
    dt->genInitCode   = (U032 *) 0;      // allocate later based on actual size
    dt->extraInitData = (U008 *) 0;      // allocate later based on actual size
    dt->extraInitCode = (U032 *) 0;      // allocate later based on actual size

    // allocate room for the genInitData
    rmStatus = osAllocMem((VOID **) &dt->genInitData, sizeInitTable);
    if (rmStatus != RM_OK)
        goto done;
    rmStatus = osAllocMem((VOID **) &dt->genInitCode, sizeInitTable * sizeof(U032));
    if (rmStatus != RM_OK)
        goto done;
    if (extraInitTableOffset)
    {
        // assume any extra init table no bigger than 'sizeInitTable'
        rmStatus = osAllocMem((VOID **) &dt->extraInitData, sizeInitTable);
        if (rmStatus != RM_OK)
            goto done;
        rmStatus = osAllocMem((VOID **) &dt->extraInitCode, sizeInitTable * sizeof(U032));
        if (rmStatus != RM_OK)
            goto done;
    }

    // extract the memory init code from the BIOS into a buffer
    if (memInitTableOffset)
    {
        int   padded_size_each;
        char *fmt;

        // figure out if using NV10 or NV15 format
        DevinitMeminitFormat(pDev, &fmt, &padded_size_each);

        // extract the array of memory formats
        // trick is that this array has to look right to the NV10 or NV15
        // routines which are expecting their own slice typedef
        BiosReadArray(pDev,
                      (VOID *) dt->memInitTable,
                      memInitTableOffset,
                      MEM_INIT_STRAP_MAX + 1,
                      (U032 *) 0,
                      fmt);
        if (rmStatus != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS,
                             "NVRM: could not unpack meminit table data\n");
            goto fallback;
        }
    }

    // extract the SDR init sequence from the BIOS into a buffer
    if (SDRSeqTableOffset)
    {
        rmStatus = BiosReadStructure(pDev,
                                     (U008*) dt->SDRSeqTable,
                                     SDRSeqTableOffset,
                                     (U032 *) 0,
                                     MEM_SEQ_FMT);
        if (rmStatus != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: could not extract SDR data\n");
            goto fallback;
        }
    }

    // extract the DDR init sequence from the BIOS into a buffer
    if (DDRSeqTableOffset)
    {
        rmStatus = BiosReadStructure(pDev,
                                     (U008*)dt->DDRSeqTable,
                                     DDRSeqTableOffset,
                                     (U032 *) 0,
                                     MEM_SEQ_FMT);
        if (rmStatus != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: could not extract DDR data\n");
            goto fallback;
        }
    }

    // Make sure we don't overflow the init code buffer
    if (sizeInitTable > BMP_INIT_CODE_SIZE_MAX)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Init table too large\n");
        rmStatus = RM_ERROR;
        goto fallback;
    }

    rmStatus = BiosReadBytes(pDev, dt->genInitData, genInitTableOffset, sizeInitTable);
    if (rmStatus != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: invalid or incomplete general init data found in BIOS\n");
        goto fallback;
    }

    // convert byte-wise init data from the BIOS to dword-wise init code
    rmStatus = DevinitInitDataToCode(dt->genInitCode, dt->genInitData);
    if (rmStatus != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: old or unusable BIOS init data\n");
        goto fallback;
    }

    // initialize with the buffer of previously extracted code
    rmStatus = InitNV(pDev, dt->genInitCode, (VOID *) dt->memInitTable, dt->SDRSeqTable, dt->DDRSeqTable);
    if (rmStatus != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: cannot initialize device using BIOS\n");
        goto fallback;
    }

    if (extraInitTableOffset)
    {
        rmStatus = BiosReadBytes(pDev, dt->extraInitData, extraInitTableOffset, sizeInitTable);
        if (rmStatus != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: invalid or incomplete extra init data found in BIOS\n");
            goto fallback;
        }

        // TO DO: change the code to byte-wise structs to remove the need for this
        // convert byte-wise init data from the BIOS to dword-wise init code
        rmStatus = DevinitInitDataToCode(dt->extraInitCode, dt->extraInitData);
        if (rmStatus != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: old or unusable BIOS extra init data\n");
            goto fallback;
        }

        // initialize with the buffer of previously extracted code
        rmStatus = InitNV(pDev, dt->extraInitCode, (VOID *) 0, (VOID *) 0, (VOID *) 0);
        if (rmStatus != RM_OK)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: cannot perform 'extra init' using BIOS\n");
            goto fallback;
        }

    }

    // Whew!
    *pBiosUsedToInit = TRUE;


 fallback:
    // could not rely on the init code in the BIOS, so use a static init table
    if ( ! *pBiosUsedToInit)
    {
        rmStatus = DevinitProcessStaticTables(pDev);
    }

 done:
    if (dt)
    {
        if (dt->genInitData)
            osFreeMem(dt->genInitData);
        if (dt->genInitCode)
            osFreeMem(dt->genInitCode);
        if (dt->extraInitData)
            osFreeMem(dt->extraInitData);
        if (dt->extraInitCode)
            osFreeMem(dt->extraInitCode);
        osFreeMem(dt);
    }

    return rmStatus;

} // end of DevinitProcessBip2()

RM_STATUS InitNV
(
    PHWINFO pDev,
    U032 genInitCode[],
    VOID* memInitTable,
    U032 SDRSeqTable[],
    U032 DDRSeqTable[]
)
{
    RM_STATUS rmStatus = RM_OK;
    U008 opcode;
    char opcodeStr[] = "NVRM: \' \' BMP operation...\n";
    U032 ip = 0;
    BOOL conditionFlag = TRUE;

    // execute the microcode referenced by the instruction pointer
    while ((opcode = DevinitFetchByte(genInitCode, &ip)) != INIT_DONE)
    {
        // extract the operands from the table and perform the operation
        opcodeStr[7] = opcode;
#if !defined(UNIX) 
        // this debug printf to the VGA console messes up the init sequence
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, opcodeStr);
#endif
        switch (opcode)
        {
            case INIT_NV_REG:
            {
                U032 addr = DevinitFetchDword(genInitCode, &ip);
                U032 mask = DevinitFetchDword(genInitCode, &ip);
                U032 data = DevinitFetchDword(genInitCode, &ip);
                rmStatus = init_NV(pDev, addr, mask, data, conditionFlag);
                break;
            }

            case INIT_ZM_REG:
            {
                U032 addr = DevinitFetchDword(genInitCode, &ip);
                U032 data = DevinitFetchDword(genInitCode, &ip);
                rmStatus = init_NV(pDev, addr, 0, data, conditionFlag);
                break;
            }

            case INIT_ZM_WREG:
            {
                U032 addr = DevinitFetchDword(genInitCode, &ip);
                U016 data = DevinitFetchWord(genInitCode, &ip);
                rmStatus = init_NV(pDev, addr, 0, (U032)(data) & 0x0000ffff, conditionFlag);
                break;
            }

            case INIT_PLL:
            {
                // fetch the address
                U032 addr = DevinitFetchDword(genInitCode, &ip);
                U016 data = DevinitFetchWord(genInitCode, &ip);
                rmStatus = init_PLL_Values(pDev, addr, data, conditionFlag);
                break;
            }

            case INIT_TIME:
            {
                U032 ticks = DevinitFetchWord(genInitCode, &ip);
                rmStatus = init_TIME_Delay(pDev, ticks);
                break;
            }

            case INIT_INDEX_IO:
            {
                U016 addr = DevinitFetchWord(genInitCode, &ip);
                U008 index = DevinitFetchByte(genInitCode, &ip);
                U008 mask = DevinitFetchByte(genInitCode, &ip);
                U008 data = DevinitFetchByte(genInitCode, &ip);
                rmStatus = init_INDEX_IO_Write(pDev, addr, index, mask, data, conditionFlag);
                break;
            }

            case INIT_MEM_RESTRICT:
            {
                U032 mask = DevinitFetchByte(genInitCode, &ip);
                U032 value = DevinitFetchByte(genInitCode, &ip);
                rmStatus = init_MEM_RESTRICT_SCRIPT(pDev, mask, value, &conditionFlag);
                break;
            }

            case INIT_STRAP_RESTRICT:
            {
                U032 mask = DevinitFetchDword(genInitCode, &ip);
                U032 value = DevinitFetchDword(genInitCode, &ip);
                rmStatus = init_STRAP_RESTRICT_SCRIPT(pDev, mask, value, &conditionFlag);
                break;
            }

            case INIT_RESUME:
            {
                conditionFlag = TRUE;
                break;
            }

            case INIT_CONFIGURE_CLK:
            {
                rmStatus = init_CONFIGURE_Clocks(pDev, memInitTable);
                break;
            }

            case INIT_CONFIGURE_MEM:
            {
                rmStatus = init_CONFIGURE_Memory(pDev, memInitTable, SDRSeqTable, DDRSeqTable);
                break;
            }

            case INIT_CONFIGURE_PREINIT:
            {
                rmStatus = init_CONFIGURE_PreInit(pDev);
                break;
            }

            case INIT_COMPUTE_MEM:
            {
                rmStatus = init_COMPUTE_Memory(pDev);
                break;
            }

            case INIT_RESET:
            {
                U032 addr = DevinitFetchDword(genInitCode, &ip);
                U032 data1 = DevinitFetchDword(genInitCode, &ip);
                U032 data2 = DevinitFetchDword(genInitCode, &ip);
                rmStatus = init_Reset_Chip(pDev, addr, data1, data2);
                break;
            }

            default:
                rmStatus = RM_ERROR;
                break;
        }
        if (rmStatus != RM_OK)
        {
            break;
        }
    }

    return rmStatus;

} // end of InitNV()

// parse the little-endian init data to convert all opcodes and operands
// to big-endian 32-bit values
RM_STATUS DevinitInitDataToCode
(
    U032 *code,
    U008 *data
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 codeIndex = 0, dataIndex = 0;
    U032 fields;
    U032 unpacked_bytes;

    // parse all instructions
    while (data[dataIndex] != INIT_DONE) // Byte quantity, no swap needed
    {
        // convert instruction based on the opcode
        // copy byte/word/dword of data into a dword of code
        switch (data[dataIndex])     // Byte quantity, no swap needed
        {
            case INIT_NV_REG:
            case INIT_RESET:
            {
                rmStatus = BiosUnpackLittleEndianStructure(&data[dataIndex],
                                                           &code[codeIndex],
                                                           INIT_FMT1_FMT,
                                                           &fields,
                                                           &unpacked_bytes);
                break;
            }
            case INIT_ZM_REG:
            case INIT_STRAP_RESTRICT:
            {
                rmStatus = BiosUnpackLittleEndianStructure(&data[dataIndex],
                                                           &code[codeIndex],
                                                           INIT_FMT2_FMT,
                                                           &fields,
                                                           &unpacked_bytes);
                break;
            }

            case INIT_ZM_WREG:
            case INIT_PLL:
            {
                rmStatus = BiosUnpackLittleEndianStructure(&data[dataIndex],
                                                           &code[codeIndex],
                                                           INIT_FMT3_FMT,
                                                           &fields,
                                                           &unpacked_bytes);
                break;
            }

            case INIT_TIME:
            {
                rmStatus = BiosUnpackLittleEndianStructure(&data[dataIndex],
                                                           &code[codeIndex],
                                                           INIT_FMT4_FMT,
                                                           &fields,
                                                           &unpacked_bytes);
                break;
            }

            case INIT_INDEX_IO:
            {
                rmStatus = BiosUnpackLittleEndianStructure(&data[dataIndex],
                                                           &code[codeIndex],
                                                           INIT_FMT5_FMT,
                                                           &fields,
                                                           &unpacked_bytes);
                break;
            }

            case INIT_MEM_RESTRICT:
            {
                rmStatus = BiosUnpackLittleEndianStructure(&data[dataIndex],
                                                           &code[codeIndex],
                                                           INIT_FMT6_FMT,
                                                           &fields,
                                                           &unpacked_bytes);
                break;
            }

            case INIT_RESUME:
            case INIT_CONFIGURE_MEM:
            case INIT_CONFIGURE_CLK:
            case INIT_CONFIGURE_PREINIT:
            case INIT_COMPUTE_MEM:
            {
                rmStatus = BiosUnpackLittleEndianStructure(&data[dataIndex],
                                                           &code[codeIndex],
                                                           INIT_FMT7_FMT,
                                                           &fields,
                                                           &unpacked_bytes);
                break;
            }

            default:
                rmStatus = RM_ERROR;
                break;
        }

        if (rmStatus != RM_OK)
            break;

        dataIndex += unpacked_bytes;
        codeIndex += fields;
    }

    // append init code delimiter
    if (rmStatus == RM_OK)
        code[codeIndex] = (U032)INIT_DONE;

    return rmStatus;

} // end of DevinitInitDataToCode()


RM_STATUS DevinitProcessStaticTables
(
    PHWINFO pDev
)
{
    RM_STATUS rmStatus = RM_OK;

#ifndef MACOS // no fallback tables for MAC
    DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: using static tables for device init\n");

    // initialize with the proper device-specific initialization code table
    switch (REG_RD_DRF(_PBUS,_PCI_NV_0,_DEVICE_ID_CHIP))
    {
        case NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV4:
            rmStatus = InitNV(pDev, Nv04_bmp_GenInitTbl, NULL, NULL, NULL);
            break;

        case NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV5:
            switch (REG_RD_DRF(_PBUS,_PCI_NV_11,_SUBSYSTEM_ID))
            {
                case NV_PBUS_PCI_NV_11_SUBSYSTEM_ID_TNT2PRO:
                    rmStatus = InitNV(pDev, Nv05Pro_bmp_GenInitTbl, Nv05_bmp_MemInitTbl, NULL, NULL);
                    break;

                default:
                    rmStatus = InitNV(pDev, Nv05_bmp_GenInitTbl, Nv05_bmp_MemInitTbl, NULL, NULL);
                    break;
            }
            break;

        case NV_PBUS_PCI_NV_0_DEVICE_ID_CHIP_NV10:
            rmStatus =
                InitNV
                (
                    pDev,
                    Nv10_bmp_GenInitTbl,
                    Nv10_bmp_MemInitTbl,
                    Nv10_SDR_Sequence_Table,
                    Nv10_DDR_Sequence_Table
                );
            break;

        default:
            rmStatus = RM_ERROR;
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: no static tables for device\n");
            break;
    }
#endif // MACOS

    return rmStatus;

} // end of DevinitProcessStaticTables()


// extract the relavant values from any arbitrary version of BMP structure
RM_STATUS DevinitGetInitTableInfo
(
    PHWINFO pDev,
    U032    offset,
    PBMP_Control_Block pBmpCtrlBlk,
    U032*   pGenInitTable,
    U032*   pExtraInitTable,
    U032*   pSizeInitTable,
    U032*   pMemInitTable,
    U032*   pSDRSequenceTable,
    U032*   pDDRSequenceTable
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (pBmpCtrlBlk->CTL_Version)
    {
        // BMP init table version 0 is for Nv3
        case BMP_INIT_TABLE_VER_0:
            rmStatus = RM_ERROR;
            break;

        // BMP init table version 1 and 2 use the version 1 base-level functionality
        case BMP_INIT_TABLE_VER_1:
        {
            BMP_Struc_Rev1 BMP_InitTableInfo;
            rmStatus = BiosReadStructure(pDev,
                                         (U008*) &BMP_InitTableInfo,
                                         offset,
                                         (U032 *) 0,
                                         BMP_INIT1_FMT);
            if (rmStatus != RM_OK)
                break;

            *pGenInitTable = BMP_InitTableInfo.BMP_GenInitTable;
            *pExtraInitTable = BMP_InitTableInfo.BMP_ExtraInitTable;
            *pSizeInitTable = BMP_InitTableInfo.BMP_SizeInitTable;
            *pMemInitTable = 0;
            *pSDRSequenceTable = 0;
            *pDDRSequenceTable = 0;
            break;
        }

        // BMP init table version 1 and 2 use the version 1 base-level functionality
        case BMP_INIT_TABLE_VER_2:
        {
            BMP_Struc_Rev2 BMP_InitTableInfo;
            rmStatus = BiosReadStructure(pDev,
                                         (U008*) &BMP_InitTableInfo,
                                         offset,
                                         (U032 *) 0,
                                         BMP_INIT2_FMT);
            if (rmStatus != RM_OK)
                break;

            *pGenInitTable = BMP_InitTableInfo.BMP_GenInitTable;
            *pExtraInitTable = BMP_InitTableInfo.BMP_ExtraInitTable;
            *pSizeInitTable = BMP_InitTableInfo.BMP_SizeInitTable;
            *pMemInitTable = 0;
            *pSDRSequenceTable = 0;
            *pDDRSequenceTable = 0;
            break;
        }

        // default is BMP init table version 3 base-level functionality
        default:
        {
            BMP_Struc_Rev3 BMP_InitTableInfo;
            BiosReadStructure(pDev,
                              (U008*) &BMP_InitTableInfo,
                              offset,
                              (U032 *) 0,
                              BMP_INIT3_FMT);

            *pGenInitTable = BMP_InitTableInfo.BMP_GenInitTable;
            *pExtraInitTable = BMP_InitTableInfo.BMP_ExtraInitTable;
            *pSizeInitTable = BMP_InitTableInfo.BMP_SizeInitTable;
            *pMemInitTable = BMP_InitTableInfo.BMP_MemInitTable;
            *pSDRSequenceTable = BMP_InitTableInfo.BMP_SDRSequenceTable;
            *pDDRSequenceTable = BMP_InitTableInfo.BMP_DDRSequenceTable;
            break;
        }
    }

    return rmStatus;

} // end of DevinitGetInitTableInfo()

U008 DevinitFetchByte(U032 code[], U032* pIp)
{
    return (U008)code[(*pIp)++];
}

U016 DevinitFetchWord(U032 code[], U032* pIp)
{
    return (U016)code[(*pIp)++];
}

U032 DevinitFetchDword(U032 code[], U032* pIp)
{
    return (U032)code[(*pIp)++];
}


// ***** CORE 3 SCRIPT PROCESSING *****

RM_STATUS DevinitProcessBip3
(
    PHWINFO pDev
)
{
    RM_STATUS rmStatus;
    EXECUTION_CONTEXT context;
    U032 scriptOffset;
    U032 scriptIndex = 0;

    // initialize execution context
    context.conditionFlag = TRUE;
    context.loopStack.sp = 0;
    context.tables = (VOID *) 0;


    // get all table offsets
    rmStatus = DevinitGetBIP3TableOffsets(pDev, &context);
    if (rmStatus != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: invalid BIOS init info version\n");
        return rmStatus;
    }

    // extract all tables from the BIOS image
    rmStatus = DevinitReadBIP3Tables(pDev, &context);
    if (rmStatus == RM_OK)
    {
        // interpret all scripts in the script table in order
        do
        {
            context.doJump = FALSE;
            scriptOffset = BiosRead16(pDev, context.scriptTableOffset + (scriptIndex * 2));
            if (scriptOffset != SCRIPT_TERMINATE)
            {
                // interpret the script at the script's offset into the BIP code
                DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: interpreting script...\n");
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   script table index: ", scriptIndex);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   script offset:      ", scriptOffset);
                rmStatus = DevinitInterpretBIP3Script(pDev, &context, scriptOffset);
                if (rmStatus == RM_OK)
                {
                    scriptIndex = (context.doJump) ? context.jumpIndex : scriptIndex + 1;
                }
                else
                {
                    DBG_PRINT_STRING      (DEBUGLEVEL_ERRORS, "NVRM: could not interpret script...\n");
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:   script table index: ", scriptIndex);
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:   script offset:      ", scriptOffset);
                    break;
                }
            }
        }
        while (scriptOffset != SCRIPT_TERMINATE);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: number of scripts interpreted = ", scriptIndex);
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: could not read BIP3 tables\n");
    }

    // release all local tables
    DevinitCleanupBIP3Tables(pDev, &context);

    return rmStatus;

} // end of DevinitProcessBip3()

RM_STATUS DevinitInterpretBIP3Script
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT pContext,
    U032               ip
)
{
    RM_STATUS rmStatus = RM_OK;
    U008 opcode;
    U008 opcodeStr[] = "NVRM: \' \' BMP operation...\n";
    BOOL done = FALSE;
    U032 packed_size, extra_packed_size;
    U008 instruction_buffer[BMP_MAX_PADDED_SIZE];
    VOID *instruction = instruction_buffer;

    // interpret the current script -- the instruction pointer, ip, always points to the next instruction
    do
    {
        // extract the operands from the current instruction and perform the operation
        opcode = 
            DevinitFetchInstruction
            (
                pDev,
                ip,
                instruction,
                &packed_size,
                &extra_packed_size
            );

        // go ahead and advance past the opcode proper.
        // After this we'll be pointing at the next instruction *or*
        //   any extra data as in BMP formats 12, 13, 14
        // At bottom we skip over any extra data associated with the opcode.
        ip += packed_size;

        if (opcode < ' ' || opcode > '~')
        {
            opcodeStr[7] = '?';
        }
        else
        {
            opcodeStr[7] = opcode;
        }

#if !defined(UNIX) 
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, opcodeStr);
#endif

#if 0
        GLOBAL_PRINTF(DEBUGLEVEL_TRACEINFO,
                      "NVRM: op: %c, 0x%x, from 0x%x, packed: %d, extra: %d\n",
                       opcode, opcode,
                       ip - packed_size,     // already added in
                       packed_size,
                       extra_packed_size);
#endif

                        
        switch (opcode)
        {
            case INIT_COMPUTE_MEM:
            {
                rmStatus = init_COMPUTE_Memory(pDev);
                break;
            }

            case INIT_RESET:
            {
                BMP_FORMAT_1 *bmp1 = instruction;
                init_Reset_Chip
                (
                    pDev, 
                    bmp1->operand1, 
                    bmp1->operand2, 
                    bmp1->operand3
                );
                break;
            }

            case INIT_IO:
            {
                BMP_FORMAT_9 *bmp9 = instruction;
                init_IO
                (
                    pDev,
                    bmp9->operand1,
                    bmp9->operand2,
                    bmp9->operand3,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_MEM_RESTRICT:
            {
                BMP_FORMAT_6 *bmp6 = instruction;
                init_MEM_RESTRICT_SCRIPT
                (
                    pDev,
                    bmp6->operand1,
                    bmp6->operand2,
                    &pContext->conditionFlag
                );
                break;
            }

            case INIT_NV_REG:
            {
                BMP_FORMAT_1 *bmp1 = instruction;
                init_NV
                (
                    pDev,
                    bmp1->operand1,
                    bmp1->operand2,
                    bmp1->operand3,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_RESUME:
            {
                pContext->conditionFlag = TRUE;
                break;
            }

            case INIT_TIME:
            {
                BMP_FORMAT_4 *bmp4 = instruction;
                init_TIME_Delay
                (
                    pDev, 
                    bmp4->operand1
                );
                break;
            }

            case INIT_INDEX_IO:
            {
                BMP_FORMAT_5 *bmp5 = instruction;
                init_INDEX_IO_Write
                (
                    pDev,
                    bmp5->operand1,
                    bmp5->operand2,
                    bmp5->operand3,
                    bmp5->operand4,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_PLL:
            {
                BMP_FORMAT_3 *bmp3 = instruction;
                init_PLL_Values
                (
                    pDev,
                    bmp3->operand1,
                    bmp3->operand2,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_ZM_REG:
            {
                BMP_FORMAT_2 *bmp2 = instruction;
                init_NV
                (
                    pDev,
                    bmp2->operand1,
                    0,
                    bmp2->operand2,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_SUB:
            {
                BMP_FORMAT_8* bmp8 = instruction;
                init_Sub
                (
                    pDev,
                    pContext,
                    bmp8->operand1,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_ZM_INDEX_IO:
            {
                BMP_FORMAT_9 *bmp9 = instruction;
                init_INDEX_IO_Write
                (
                    pDev,
                    bmp9->operand1,
                    bmp9->operand2,
                    0,
                    bmp9->operand3,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_ZM_IO:
            {
                BMP_FORMAT_10 *bmp10 = instruction;
                init_ZM_IO
                (
                    pDev,
                    bmp10->operand1,
                    bmp10->operand2,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_MACRO:
            {
                BMP_FORMAT_8 *bmp8 = instruction;
                init_Macro
                (
                    pDev,
                    pContext,
                    bmp8->operand1,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_CONDITION:
            {
                BMP_FORMAT_8 *bmp8 = instruction;
                init_Condition
                (
                    pDev,
                    pContext,
                    bmp8->operand1,
                    &pContext->conditionFlag
                );
                break;
            }

            case INIT_IO_CONDITION:
            {
                BMP_FORMAT_8 *bmp8 = instruction;
                init_IoCondition
                (
                    pDev,
                    pContext,
                    bmp8->operand1,
                    &pContext->conditionFlag
                );
                break;
            }

            case INIT_IO_FLAG_CONDITION:
            {
                BMP_FORMAT_8 *bmp8 = instruction;
                init_IoFlagCondition
                (
                    pDev,
                    pContext,
                    bmp8->operand1,
                    &pContext->conditionFlag
                );
                break;
            }

            case INIT_NOT:
            {
                pContext->conditionFlag = !pContext->conditionFlag;
                break;
            }

            case INIT_COPY:
            {
                BMP_FORMAT_11 *bmp11 = instruction;
                init_Copy
                (
                    pDev,
                    pContext,
                    bmp11->operand1,
                    (S008) bmp11->operand2,
                    bmp11->operand3,
                    bmp11->operand4,
                    bmp11->operand5,
                    bmp11->operand6,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_RESTRICT_PROG:
            {
                BMP_FORMAT_12 *bmp12 = instruction;
                init_RestrictProg
                (
                    pDev,
                    pContext,
                    bmp12->operand1,
                    bmp12->operand2,
                    bmp12->operand3,
                    bmp12->operand4,
                    bmp12->operand5,
                    ip,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_IO_RESTRICT_PROG:
            {
                BMP_FORMAT_13 *bmp13 = instruction;
                init_IoRestrictProg
                (
                    pDev,
                    pContext,
                    bmp13->operand1,
                    bmp13->operand2,
                    bmp13->operand3,
                    bmp13->operand4,
                    bmp13->operand5,
                    bmp13->operand6,
                    ip,
                    pContext->conditionFlag
                );
                break;
            }

            case INIT_IO_RESTRICT_PLL:
            {
                BMP_FORMAT_14 *bmp14 = instruction;
                init_IoRestrictPll
                (
                    pDev,
                    pContext,
                    bmp14->operand1,
                    bmp14->operand2,
                    bmp14->operand3,
                    bmp14->operand4,
                    bmp14->operand5,
                    bmp14->operand6,
                    bmp14->operand7,
                    ip,
                    pContext->conditionFlag
                );
                break;
            }
                
            case INIT_INDEX_ADDRESS_LATCHED:
            {
                BMP_FORMAT_15 *bmp15 = instruction;
                init_IndexAddressLatched
                (
                    pDev,
                    bmp15->operand1,
                    bmp15->operand2,
                    bmp15->operand3,
                    bmp15->operand4,
                    bmp15->operand5,
                    ip,
                    pContext->conditionFlag
                );
                break;
            }
                
            case INIT_REPEAT:
            {
                BMP_FORMAT_8 *bmp8 = instruction;
                init_Repeat
                (
                    pDev,
                    pContext,
                    ip,
                    bmp8->operand1
                );
                break;
            }

            case INIT_END_REPEAT:
            {
                init_EndRepeat
                (
                    pDev,
                    pContext,
                    &ip
                );
                break;
            }

            case INIT_FUNCTION:
            {
                BMP_FORMAT_8 *bmp8 = instruction;
                init_Function
                (
                    pDev,
                    pContext,
                    bmp8->operand1
                );
                break;
            }

            // all of these stop the processing of the current script
            case INIT_JUMP:
            {
                if (pContext->conditionFlag)
                {
                    // extract the destination script index and fall thru to end execution of the current script
                    BMP_FORMAT_8 *bmp8 = instruction;
                    pContext->doJump = TRUE;
                    pContext->jumpIndex = bmp8->operand1;
                    done = TRUE;
                }
                break;
            }

            case INIT_DONE:
            case INIT_EOS:
            case INIT_CONFIGURE_CLK:
            case INIT_CONFIGURE_MEM:
            case INIT_CONFIGURE_PREINIT:
                done = TRUE;
                break;

            default:
                if (opcode == 0x00)
                {
                    // screen out benign NULL opcodes
                    DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Benign opcode found in BIP3 table, skipping...\n");
                }
                else
                {    
                    // fail on undefined opcodes
                    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Bad opcode encountered\n");
                    rmStatus = RM_ERROR;
                }
                break;
        }
        if (rmStatus != RM_OK)
        {
            break;
        }

        // adjust the instruction pointer for those instruction with variable-sized operands
        ip += extra_packed_size;

    } while (!done);

    return rmStatus;

} // end of DevinitInterpretBIP3Script()

RM_STATUS DevinitProcessBip3InternalTMDS
(
    PHWINFO pDev,
    U032    fpPixClk,
    U032    interfaceType
)
{
    RM_STATUS rmStatus = RM_OK;
    EXECUTION_CONTEXT context;
    U032 TableOffset;
    
    // initialize execution context
    context.conditionFlag = TRUE;
    context.loopStack.sp = 0;

    // get all table offsets
    rmStatus = DevinitGetBIP3TableOffsets(pDev, &context);

    if (rmStatus != RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: invalid BIOS init info version\n");
        return rmStatus;
    }
    
    switch ( interfaceType )
    {
        case TMDSIT_TMDS_SINGLE_A:
        {
            TableOffset = context.TMDSSingleAOffset;
            break;
        }
        case TMDSIT_TMDS_SINGLE_B:
        {
            TableOffset = context.TMDSSingleBOffset;
            break;
        }
        case TMDSIT_TMDS_DUAL:
        {
            TableOffset = context.TMDSDualOffset;
            break;
        }
        case TMDSIT_LVDS_SINGLE_A:
        {
            TableOffset = context.LVDSSingleAOffset;
            break;
        }
        case TMDSIT_LVDS_SINGLE_B:
        {
            TableOffset = context.LVDSSingleBOffset;
            break;
        }
        case TMDSIT_LVDS_DUAL:
        {
            TableOffset = context.LVDSDualOffset;
            break;
        }
        case TMDSIT_OFF_SINGLE_A:
        {
            TableOffset = context.OffSingleAOffset;
            break;
        }
        case TMDSIT_OFF_SINGLE_B:
        {
            TableOffset = context.OffSingleBOffset;
            break;
        }
        default:
        {
            return RM_ERROR;
        }
    }

    // Is this table present in the BIOS?
    if ( TableOffset == 0 )
    {
        return RM_ERROR;
    }

    // Find out which script index to use and process it
    rmStatus = DevinitProcessBip3InternalTMDSTable
        (
            pDev,
            &context,
            fpPixClk,
            TableOffset
        );

    return rmStatus;

} // end of DevinitProcessBip3InternalTMDS()


// Process an internal TMDS programming table
RM_STATUS DevinitProcessBip3InternalTMDSTable
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT context,
    U032               fpPixClk,
    U032               TableOffset
)
{
    RM_STATUS rmStatus = RM_OK;

    U032 scriptIndex = 0;

    U032 LastFreq;
    U032 Entries;
    BOOL Found = FALSE;

    U032 currentScriptOffset;
    TMDS_MODE_ENTRY tmdsModeEntry;
    U032            packed_size;

    LastFreq = 0;
    Entries = 0;

    // Scan for frequency
    do
    {
        // Because we don't know the size, extract one entry at a time
        rmStatus = BiosReadStructure(pDev,
                                     &tmdsModeEntry,
                                     TableOffset,
                                     &packed_size,
                                     TMDS_MODE_FORMAT);
        if (rmStatus != RM_OK)
        {
            return rmStatus;
        }
        // Is the pixel clock greater than or equal to this entries frequency
        if ( fpPixClk >= tmdsModeEntry.tmdsmFreq )
        {
            scriptIndex = tmdsModeEntry.tmdsmScript;
            Found = TRUE;
        }
        else if ( ( Entries > 0 ) && ( LastFreq <= tmdsModeEntry.tmdsmFreq ) )
        {
            // We didn't find a match yet, we saw at least two values,
            // and the frequencies are not decreasing
            return RM_ERROR;
        }
        else
        {
            LastFreq = tmdsModeEntry.tmdsmFreq;
            Entries++;
            TableOffset += packed_size;
        }
    } while ( ! Found);

    // interpret the script in the script table at the given index
    currentScriptOffset = BiosRead16(pDev, context->scriptTableOffset + (scriptIndex*2));
    return DevinitInterpretBIP3Script(pDev, context, currentScriptOffset);
}


// extract the relavant values from any arbitrary version of BIP/3 structure
RM_STATUS DevinitGetBIP3TableOffsets
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp
)
{
    RM_STATUS rmStatus = RM_OK;
    BMP_Control_Block bmpCtrlBlk;
    U032 offset;

    cp->TMDSSingleAOffset = 0;
    cp->TMDSSingleBOffset = 0;
    cp->TMDSDualOffset = 0;
    cp->LVDSSingleAOffset = 0;
    cp->LVDSSingleBOffset = 0;
    cp->LVDSDualOffset = 0;
    cp->OffSingleAOffset = 0;  
    cp->OffSingleBOffset = 0; 

    // get the offset of the BMP init table
    rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);
    if (rmStatus != RM_OK)
    {
        return rmStatus;
    }

    // get the BMP init table
    if (bmpCtrlBlk.CTL_Version == BMP_INIT_TABLE_VER_5)
    {
        // Core 3 type devinit tables should only be used with BMP 0x05.0x10 or greater.
        if ( bmpCtrlBlk.CTL_Flags < 0x10 )
        {
            return RM_ERROR;
        }
        switch (bmpCtrlBlk.CTL_Flags)
        {
            // Version 0x10 did not have TMDS tables
            case 0x10:
            {
                BMP_Struc_Rev5_0x10 bmpInitTable;
                BiosReadStructure(pDev,
                                  &bmpInitTable,
                                  offset,
                                  (U032 *) 0,
                                  BMP_INIT5_0x10_FMT);
                cp->scriptTableOffset          = bmpInitTable.BMP_InitScriptTablePtr;
                cp->macroIndexTableOffset      = bmpInitTable.BMP_MacroIndexTablePtr;
                cp->macroTableOffset           = bmpInitTable.BMP_MacroTablePtr;
                cp->conditionTableOffset       = bmpInitTable.BMP_ConditionTablePtr;
                cp->ioConditionTableOffset     = bmpInitTable.BMP_IOConditionTablePtr;
                cp->ioFlagConditionTableOffset = bmpInitTable.BMP_IOFlagConditionTablePtr;
                cp->functionTableOffset        = bmpInitTable.BMP_InitFunctionTablePtr;

                rmStatus = RM_OK;

                break;
            }


            // default is BIP/3 BMP init table version 5, subversion 0x11 base-level functionality or above
            default:
            {
                BMP_Struc_Rev5_0x11 bmpInitTable;
                BiosReadStructure(pDev,
                                  &bmpInitTable,
                                  offset,
                                  (U032 *) 0,
                                  BMP_INIT5_0x11_FMT);
                cp->scriptTableOffset          = bmpInitTable.BMP_InitScriptTablePtr;
                cp->macroIndexTableOffset      = bmpInitTable.BMP_MacroIndexTablePtr;
                cp->macroTableOffset           = bmpInitTable.BMP_MacroTablePtr;
                cp->conditionTableOffset       = bmpInitTable.BMP_ConditionTablePtr;
                cp->ioConditionTableOffset     = bmpInitTable.BMP_IOConditionTablePtr;
                cp->ioFlagConditionTableOffset = bmpInitTable.BMP_IOFlagConditionTablePtr;
                cp->functionTableOffset        = bmpInitTable.BMP_InitFunctionTablePtr;
                
                cp->TMDSSingleAOffset          = bmpInitTable.BMP_TMDSSingleAPtr;
                cp->TMDSSingleBOffset          = bmpInitTable.BMP_TMDSSingleBPtr;
                cp->TMDSDualOffset             = bmpInitTable.BMP_TMDSDualPtr;   
                cp->LVDSSingleAOffset          = bmpInitTable.BMP_LVDSSingleAPtr;
                cp->LVDSSingleBOffset          = bmpInitTable.BMP_LVDSSingleBPtr;
                cp->LVDSDualOffset             = bmpInitTable.BMP_LVDSDualPtr;   
                cp->OffSingleAOffset           = bmpInitTable.BMP_OffSingleAPtr; 
                cp->OffSingleBOffset           = bmpInitTable.BMP_OffSingleBPtr; 

                rmStatus = RM_OK;
                break;
            }
        }
    }
    else
    {
        rmStatus = RM_ERROR;
    }

    return rmStatus;

} // end of DevinitGetBIP3TableOffsets()

RM_STATUS DevinitReadBIP3Tables
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp    
)
{
    RM_STATUS rmStatus;

    // allocate space for the tables
    cp->tables = (VOID *) 0;

#ifdef USE_STATIC_BIP3_TABLES
    cp->tables = &bip3_static_tables;
#else
    rmStatus = osAllocMem((VOID *) &cp->tables, sizeof(*cp->tables));
    if (rmStatus != RM_OK)
        goto done;
#endif
        
    // go ahead and read entire macro index table into memory
    rmStatus = BiosReadArray(pDev,
                             cp->tables->macroIndexTable,
                             cp->macroIndexTableOffset,
                             256,
                             (U032 *) 0,        // packed size
                             MACRO_INDEX_ENTRY_FMT);
    if (rmStatus != RM_OK)
        goto done;

    // read macro table itself into memory
    rmStatus = BiosReadArray(pDev,
                             cp->tables->macroTable,
                             cp->macroTableOffset,
                             256,
                             (U032 *) 0,        // packed size
                             MACRO_ENTRY_FMT);
    if (rmStatus != RM_OK)
        goto done;
    
    // read condition table into memory
    rmStatus = BiosReadArray(pDev,
                             cp->tables->conditionTable,
                             cp->conditionTableOffset,
                             256,
                             (U032 *) 0,        // packed size
                             CONDITION_ENTRY_FMT);
    if (rmStatus != RM_OK)
        goto done;
    
    // read io condition table    
    rmStatus = BiosReadArray(pDev,
                             cp->tables->ioConditionTable,
                             cp->ioConditionTableOffset,
                             256,
                             (U032 *) 0,        // packed size
                             IO_CONDITION_ENTRY_FMT);
    if (rmStatus != RM_OK)
        goto done;

    // read io flag condition table    
    rmStatus = BiosReadArray(pDev,
                             cp->tables->ioFlagConditionTable,
                             cp->ioFlagConditionTableOffset,
                             256,
                             (U032 *) 0,        // packed size
                             IO_FLAG_CONDITION_ENTRY_FMT);
    if (rmStatus != RM_OK)
        goto done;

    // read function table
    rmStatus = BiosReadArray(pDev,
                             cp->tables->functionTable,
                             cp->functionTableOffset,
                             256,
                             (U032 *) 0,        // packed size
                             FUNCTION_ENTRY_FMT);
    if (rmStatus != RM_OK)
        goto done;

 done:
    return rmStatus;
    
} // end of DevinitReadBIP3Tables()

VOID DevinitCleanupBIP3Tables
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp    
)
{
#ifndef USE_STATIC_BIP3_TABLES
    if (cp->tables)
    {
        osFreeMem(cp->tables);
    }
#endif
    cp->tables = (VOID *) 0;
    
} // end of DevinitCleanupBIP3Tables()

U008 DevinitFetchInstruction
(
    PHWINFO pDev,
    U032    ip,
    VOID   *pInstruction,
    U032   *pPackedSize,
    U032   *pExtraPackedSize
)
{
    U032 padded_size;
    U008 opcode;
    U032 extra_packed_size = 0;
    U032 count;
    U032 elementSize;
    char *format;
    RM_STATUS rmStatus;

    // set current instruction
    opcode = BiosRead8(pDev, ip);

    // figure out what we are looking at here...
    switch (opcode)
    {
        case INIT_RESET:
        case INIT_NV_REG:
            padded_size = sizeof(BMP_FORMAT_1);
            format = INIT_FMT1_FMT;
            break;

        case INIT_ZM_REG:
            padded_size = sizeof(BMP_FORMAT_2);
            format = INIT_FMT2_FMT;
            break;

        case INIT_PLL:
            padded_size = sizeof(BMP_FORMAT_3);
            format = INIT_FMT3_FMT;
            break;

        case INIT_TIME:
            padded_size = sizeof(BMP_FORMAT_4);
            format = INIT_FMT4_FMT;
            break;

        case INIT_INDEX_IO:
            padded_size = sizeof(BMP_FORMAT_5);
            format = INIT_FMT5_FMT;
            break;

        case INIT_MEM_RESTRICT:
            padded_size = sizeof(BMP_FORMAT_6);
            format = INIT_FMT6_FMT;
            break;

        case INIT_COMPUTE_MEM:
        case INIT_CONFIGURE_MEM:
        case INIT_CONFIGURE_CLK:
        case INIT_CONFIGURE_PREINIT:
        case INIT_DONE:
        case INIT_RESUME:
        case INIT_EOS:
        case INIT_END_REPEAT:
        case INIT_NOT:
            padded_size = sizeof(BMP_FORMAT_7);
            format = INIT_FMT7_FMT;
            break;

        case INIT_SUB:
        case INIT_JUMP:
        case INIT_MACRO:
        case INIT_CONDITION:
        case INIT_IO_CONDITION:
        case INIT_IO_FLAG_CONDITION:
        case INIT_REPEAT:
        case INIT_FUNCTION:
            padded_size = sizeof(BMP_FORMAT_8);
            format = INIT_FMT8_FMT;
            break;

        case INIT_IO:
        case INIT_ZM_INDEX_IO:
            padded_size = sizeof(BMP_FORMAT_9);
            format = INIT_FMT9_FMT;
            break;

        case INIT_ZM_IO:
            padded_size = sizeof(BMP_FORMAT_10);
            format = INIT_FMT10_FMT;
            break;

        case INIT_COPY:
            padded_size = sizeof(BMP_FORMAT_11);
            format = INIT_FMT11_FMT;
            break;

        case INIT_RESTRICT_PROG:
            padded_size = sizeof(BMP_FORMAT_12);
            format = INIT_FMT12_FMT;
            break;

        case INIT_IO_RESTRICT_PROG:
            padded_size = sizeof(BMP_FORMAT_13);
            format = INIT_FMT13_FMT;
            break;

        case INIT_IO_RESTRICT_PLL:
            padded_size = sizeof(BMP_FORMAT_14);
            format = INIT_FMT14_FMT;
            break;

        case INIT_INDEX_ADDRESS_LATCHED:
            padded_size = sizeof(BMP_FORMAT_15);
            format = INIT_FMT15_FMT;
            break;
        
        default:
            // assume unknown opcodes are 1-byte instructions
            padded_size = sizeof(bios_U008);
            format = "b";
            break;
    }

    // now extract the instruction and convert it.
    rmStatus = BiosReadStructure(pDev,
                                 pInstruction,
                                 ip,
                                 pPackedSize,
                                 format);
    if (rmStatus != RM_OK)
        goto failed;

    // now find size of any 'extra' stuff that is part of this instruction...
    // we are not reading it in, just calculating its size

    switch (opcode)
    {
        case INIT_RESTRICT_PROG:
        {
            BMP_FORMAT_12 *bmp12 = pInstruction;
            count = bmp12->operand4;
            elementSize = 4;
            extra_packed_size = count * elementSize;
            break;
        }

        case INIT_IO_RESTRICT_PROG:
        {
            BMP_FORMAT_13 *bmp13 = pInstruction;
            count = bmp13->operand5;
            elementSize = 4;
            extra_packed_size = count * elementSize;
            break;
        }

        case INIT_IO_RESTRICT_PLL:
        {
            BMP_FORMAT_14 *bmp14 = pInstruction;
            count = bmp14->operand6;
            elementSize = 2;
            extra_packed_size = count * elementSize;
            break;
        }

        case INIT_INDEX_ADDRESS_LATCHED:
        {
            BMP_FORMAT_15 *bmp15 = pInstruction;
            count = bmp15->operand5;
            elementSize = 2;
            extra_packed_size = count * elementSize;
            break;
        }

        default:
            extra_packed_size = 0;
            break;
    }

    *pExtraPackedSize = extra_packed_size;

    // return actual opcode
    if (rmStatus == RM_OK)
        return opcode;

 failed:
    // Error path
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: error in DevinitFetchInstruction\n");
    return 0;             // bad opcode

} // end of DevinitFetchInstruction()


// ***** SCRIPT INSTRUCTION IMPLEMENTATION *****

// NV register initialization
RM_STATUS init_NV
(
    PHWINFO pDev,
    U032 reg,
    U032 mask,
    U032 data,
    BOOL conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 value;

    // read, modify, and rewrite the value
    if (conditionFlag)
    {
        value = REG_RD32(reg);
        value = (value & mask) | data;
        REG_WR32(reg, value);
    }

    return rmStatus;

} // end of init_NV()

// NV PLL initialization
RM_STATUS init_PLL_Values
(
    PHWINFO pDev,
    U032 reg,
    U032 MhzX100,
    BOOL conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;

    if (conditionFlag)
    {
        // program the proper clock using the RM kernel
        switch (reg)
        {
            case NV_PRAMDAC_NVPLL_COEFF:
                pDev->Dac.HalInfo.NVClk = MhzX100 * 10000;
                rmStatus = dacProgramNVClk(pDev);
                break;
            
            case NV_PRAMDAC_MPLL_COEFF:
                pDev->Dac.HalInfo.MClk = MhzX100 * 10000;
                rmStatus = dacProgramMClk(pDev, 0);
                break;
            
            case NV_PRAMDAC_VPLL_COEFF:
                rmStatus = dacProgramPClk(pDev, 0, MhzX100);
                break;
            
            default:
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Cannot program unknown clock\n");
                rmStatus = RM_ERROR;
                break;
        }
    }    

    return rmStatus;

} // end of init_PLL_Values()

// initialization time delay
RM_STATUS init_TIME_Delay
(
    PHWINFO pDev,
    U032 uSec
)
{
    return osDelayUs(uSec);

} // end of init_TIME_Delay()

// indexed I/O Register initialization
RM_STATUS init_INDEX_IO_Write
(
    PHWINFO pDev,
    U032 port,
    U032 index,
    U032 mask,
    U032 data,
    BOOL conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U008 value;

    if (conditionFlag)
    {
        // read, modify, and write the value
        rmStatus = DevinitReadPort(pDev, port, index, &value);
        if (rmStatus == RM_OK)
        {
            value = (value & (U008) mask) | (U008) data;
            rmStatus = DevinitWritePort(pDev, port, index, value);
        }
    }

    return rmStatus;

} // end of init_INDEX_IO_Write()

//  inhibit processing based on memory size
RM_STATUS init_MEM_RESTRICT_SCRIPT
(
    PHWINFO pDev,
    U032 mask,
    U032 value,
    BOOL* pConditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;

    if ((REG_RD32(NV_PFB_BOOT_0) & mask) != value)
    {
        *pConditionFlag = FALSE;
    }

    return rmStatus;

} // end of init_MEM_RESTRICT_SCRIPT()

//  inhibit processing based on strapping
RM_STATUS init_STRAP_RESTRICT_SCRIPT
(
    PHWINFO pDev,
    U032 mask,
    U032 value,
    BOOL* pConditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;

    if ((REG_RD32(NV_PEXTDEV_BOOT_0) & mask) != value)
    {
        *pConditionFlag = FALSE;
    }

    return rmStatus;

} // end of init_STRAP_RESTRICT_SCRIPT()

RM_STATUS init_CONFIGURE_Clocks
(
    PHWINFO pDev,
    VOID* memInitTable
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (REG_RD32(NV_PBUS_PCI_NV_0) >> 16 & 0x0000ffff)
    {
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID0:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID1:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID2:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID3:
            rmStatus = Nv10MemConfigureClocks(pDev, (MEM_INIT_VALS_NV10*)memInitTable);
            break;

        default:
            rmStatus = Nv15MemConfigureClocks(pDev, (MEM_INIT_VALS_NV15*)memInitTable);
            break;
    }

    return rmStatus;

} // end of init_CONFIGURE_Clocks()

// configure the device memory
RM_STATUS init_CONFIGURE_Memory
(
    PHWINFO pDev,
    VOID* memInitTable,
    U032 SDRSeqTable[],
    U032 DDRSeqTable[]
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (REG_RD32(NV_PBUS_PCI_NV_0) >> 16 & 0x0000ffff)
    {
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID0:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID1:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID2:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID3:
            rmStatus = Nv10MemConfigureMemory(pDev, (MEM_INIT_VALS_NV10*)memInitTable, SDRSeqTable, DDRSeqTable);
            break;

        default:
            rmStatus = Nv15MemConfigureMemory(pDev, (MEM_INIT_VALS_NV15*)memInitTable, SDRSeqTable, DDRSeqTable);
            break;
    }

    return rmStatus;

} // end of init_CONFIGURE_Memory()

// configure RAM type and specifiers
RM_STATUS init_CONFIGURE_PreInit
(
    PHWINFO pDev
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (REG_RD32(NV_PBUS_PCI_NV_0) >> 16 & 0x0000ffff)
    {
        default:
            rmStatus = Nv10MemChipPreInit(pDev);
            break;
    }

    return rmStatus;

} // end of init_CONFIGURE_PreInit()

// compute the memory size of the device
RM_STATUS init_COMPUTE_Memory
(
    PHWINFO pDev
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (REG_RD32(NV_PBUS_PCI_NV_0) >> 16 & 0x0000ffff)
    {
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV04:
            rmStatus = Nv04MemResizeMemory(pDev);
            break;

        // TO DO: modify NV5 memory resizing to use data from the BIOS instead of static data
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID0:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID1:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID2:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID3:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID0:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID1:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID2:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID3:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID0:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID1:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID2:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID3:
            rmStatus = Nv05MemResizeMemory(pDev);
            break;

        default:
            rmStatus = Nv10MemComputeMemory(pDev);
            break;
    }

    return rmStatus;

} // end of init_COMPUTE_Memory()

// reset the device
RM_STATUS init_Reset_Chip
(
    PHWINFO pDev,
    U032 reg,
    U032 enginesOff,
    U032 enginesOn
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 saveValue;

    // save and toggle the AGP command register (don't disturb the FW enable)
    saveValue = REG_RD32(NV_PBUS_PCI_NV_19);
    REG_WR32(NV_PBUS_PCI_NV_19,
             (saveValue & ~(DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_SBA_ENABLE, _ON) |
                            DRF_DEF(_PBUS, _PCI_NV_19, _AGP_COMMAND_AGP_ENABLE, _ON))));

    // write out engines to turn off and on
    REG_WR32(reg, enginesOff);
    REG_WR32(reg, enginesOn);

    // restore AGP command register
    REG_WR32(NV_PBUS_PCI_NV_19, saveValue);

    // set access to the ROM thru instance mem
    FLD_WR_DRF_DEF(_PBUS,_PCI_NV_20,_ROM_SHADOW,_DISABLED);

    return rmStatus;

} // end of init_Reset_Chip()

RM_STATUS init_IO
(
    PHWINFO pDev,
    U032 port,
    U032 mask,
    U032 data,
    BOOL conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 reg, value;

    if (conditionFlag)
    {
        // convert the CRTC port into a priv register
        rmStatus = DevinitPortToPrivReg(port, &reg);
        if (rmStatus == RM_OK)
        {
            // read, modify, and rewrite the value
            value = REG_RD32(reg);
            value = (value & mask) | data;
            REG_WR32(reg, value);
        }
    }

    return rmStatus;

} // end of init_IO()

// set the condition flag based on the results of the given condition
RM_STATUS init_Condition
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               conditionId,
    BOOL              *pConditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 reg;
    U032 mask;
    U032 comparison;
    U032 value;
    CONDITION_ENTRY *pCondEntry;

    if (conditionId != NO_CONDITION)
    {
        // grab out our fields
        pCondEntry = &cp->tables->conditionTable[conditionId];
        reg         = pCondEntry->condAddress;
        mask        = pCondEntry->condMask;
        comparison  = pCondEntry->condCompare;

        // read, modify, reset condition flag on miscompare
        value = REG_RD32(reg);
        value = value & mask;
        if (value != comparison)
        {
            *pConditionFlag = FALSE;
        }
    }    

    return rmStatus;

} // end of init_Condition()

RM_STATUS init_IoCondition
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               conditionId,
    BOOL              *pConditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 port;
    U032 index, mask, comparison;
    U008 value;
    IO_CONDITION_ENTRY* pCondition;

    // modify the flag only if there is a valid condition
    if (conditionId != NO_CONDITION)
    {
        // grab out our fields
        pCondition = &cp->tables->ioConditionTable[conditionId];
        port        = pCondition->iocondPort;
        index       = pCondition->iocondIndex;
        mask        = pCondition->iocondMask;
        comparison  = pCondition->iocondCompare;

        // read, modify, reset condition flag on miscompare
        rmStatus = DevinitReadPort(pDev, port, index, &value);
        if (rmStatus == RM_OK)
        {
            value = value & (U008) mask;
            if (value != (U008) comparison)
            {
                *pConditionFlag = FALSE;
            }
        }
    }    

    return rmStatus;

} // end of init_IoCondition()

// set a flag based on the results of the given I/O flag condition
RM_STATUS init_IoFlagCondition
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               conditionId,
    BOOL              *pFlag
)
{
    U032 port;
    U032 index;
    U032 mask;
    U032 shift;
    U032 flagArrayOffset;
    U032 flag;
    U032 flagMask;
    U032 comparison;
    U008 flagIndex;
    IO_FLAG_CONDITION_ENTRY *pIoFlagCondEntry;
    RM_STATUS rmStatus = RM_OK;

    // set the flag if there is a valid condition
    if (conditionId != NO_CONDITION)
    {
        // init to TRUE
        *pFlag = TRUE;

        // grab our fields..
        pIoFlagCondEntry = &cp->tables->ioFlagConditionTable[conditionId];
        port            = pIoFlagCondEntry->iofcondPort;
        index           = pIoFlagCondEntry->iofcondIndex;
        mask            = pIoFlagCondEntry->iofcondMask;
        shift           = pIoFlagCondEntry->iofcondShift;
        flagArrayOffset = pIoFlagCondEntry->iofcondFlagArray;
        flagMask        = pIoFlagCondEntry->iofcondFlagMask;
        comparison      = pIoFlagCondEntry->iofcondFlagCompare;

        // read, modify, reset condition flag on miscompare
        rmStatus = DevinitReadPort(pDev, port, index, &flagIndex);
        if (rmStatus == RM_OK)
        {
            // set the flag based on the contents the flag array contents
            flagIndex = (flagIndex & (U008) mask) >> shift;
            flag = BiosRead8(pDev, flagArrayOffset + flagIndex);
            flag = flag & flagMask;
            if (flag != comparison)
            {
                *pFlag = FALSE;
            }
        }
    }

    return rmStatus;

} // end of init_IoFlagCondition()

RM_STATUS init_RestrictProg
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               indexReg,
    U032               mask,
    U032               shift,
    U032               dataTableSize,
    U032               reg,
    U032               dataTableOffset,
    BOOL               conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 dataIndex, data;

    // only perform the instruction if the condition flag is TRUE and the register is defined, i.e. not 0
    if (conditionFlag && reg)
    {
        // get the index of the data table
        dataIndex = REG_RD32(indexReg);

        // write the register from the data table
        dataIndex = (dataIndex & mask) >> shift;
        if (dataIndex >= dataTableSize)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: range check failure in init_IoRestrictProg\n");
            rmStatus = RM_ERROR;
        }
        else
        {
            data = BiosRead32(pDev, dataTableOffset + (dataIndex * sizeof(U032)));
            REG_WR32(reg, data);
        }
    }

    return rmStatus;

} // end of init_RestrictProg()

RM_STATUS init_IoRestrictProg
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               port,
    U032               index,
    U032               mask,
    U032               shift,
    U032               dataTableSize,
    U032               reg,
    U032               dataTableOffset,
    BOOL               conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U008 dataIndex;

    // only perform the instruction if the condition flag is TRUE and
    //   the register is defined, i.e. not 0
    if (conditionFlag && reg)
    {
        // get the index of the data table
        rmStatus = DevinitReadPort(pDev, port, index, &dataIndex);

        if (rmStatus == RM_OK)
        {
            U032 word32;

            // write the register from the data table
            dataIndex = (dataIndex & (U008) mask) >> shift;
            if (dataIndex >= dataTableSize)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: range check failure in init_IoRestrictProg\n");
                rmStatus = RM_ERROR;
            }
            else
            {
                word32 = BiosRead32(pDev, dataTableOffset + (dataIndex * sizeof(U032)));
                REG_WR32(reg, word32);
            }
        }
    }

    return rmStatus;

} // end of init_IoRestrictProg()

RM_STATUS init_IoRestrictPll
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               port,
    U032               index,
    U032               mask,
    U032               shift,
    U032               doubleFlagCondition,
    U032               freqTableSize,
    U032               pllReg,
    U032               freqTableOffset,
    BOOL               conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U008 freqIndex;
    BOOL doubleFlag = FALSE;
    U032 freq;

    // only perform the instruction if the condition flag is TRUE and the
    //   register is defined, i.e. not 0

    if (conditionFlag && pllReg)
    {
        // get the index of the frequency table
        rmStatus = DevinitReadPort(pDev, port, index, &freqIndex);
        if (rmStatus == RM_OK)
        {
            // program the PLL register from the frequency table
            freqIndex = (freqIndex & (U008) mask) >> shift;
            init_IoFlagCondition(pDev, cp, doubleFlagCondition, &doubleFlag);
            if (freqIndex >= freqTableSize)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: range check failure in init_IoRestrictPll\n");
                rmStatus = RM_ERROR;
            }
            else
            {
                freq = BiosRead16(pDev, freqTableOffset + (freqIndex * 2));
                freq = (doubleFlag) ? freq * 2 : freq;
                rmStatus = init_PLL_Values(pDev, pllReg, freq, conditionFlag);
            }
        }
    }

    return rmStatus;

} // end of init_IoRestrictPll()

RM_STATUS init_IndexAddressLatched
(
    PHWINFO pDev,
    U032    controlReg,
    U032    dataReg,
    U032    mask,
    U032    writeOr,
    U032    dataTableSize,
    U032    dataTableOffset,
    BOOL    conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;

    // only perform the instruction if the condition flag is TRUE and
    //  the register is defined, i.e. not 0
    if (conditionFlag && (dataReg != 0))
    {
        U032 i;

        for ( i = 0; i < dataTableSize; i++ )
        {
            U032 index;
            U032 dataValue;
            U032 controlValue;

            index = BiosRead8(pDev, dataTableOffset + (i * 2));
            dataValue = BiosRead8(pDev, dataTableOffset + (i * 2) + 1);

            REG_WR32( dataReg, dataValue );

            controlValue = REG_RD32( controlReg );
            controlValue &= mask;
            controlValue |= writeOr;
            controlValue |= index;

            REG_WR32( controlReg, controlValue );
        }
    }    
    
    return rmStatus;

} // end of init_IndexAddressLatched()


RM_STATUS init_Sub
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               scriptIndex,
    BOOL               conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 scriptOffset;
    
    if (conditionFlag)
    {
        scriptOffset = BiosRead16(pDev, cp->scriptTableOffset + (scriptIndex * 2));
        if (scriptOffset != SCRIPT_TERMINATE)
        {
            // interpret the script at the script's offset into the BIP code
            DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: interpreting sub script...\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   script table index: ", scriptIndex);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   script offset:      ", scriptOffset);
            rmStatus = DevinitInterpretBIP3Script(pDev, cp, scriptOffset);
            if (rmStatus != RM_OK)
            {
                DBG_PRINT_STRING      (DEBUGLEVEL_ERRORS, "NVRM: could not interpret sub script...\n");
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:   script table index: ", scriptIndex);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:   script offset:      ", scriptOffset);
            }
        }
    }    
    
    return rmStatus;

} // end of init_Sub()

RM_STATUS init_Macro
(
    PHWINFO            pDev,
    EXECUTION_CONTEXT *cp,
    U032               macroId,
    BOOL               conditionFlag
)
{
    RM_STATUS           rmStatus = RM_OK;
    MACRO_ENTRY        *pMacro;
    U032                macroCount;
    U032                i, reg, data;
    MACRO_INDEX_ENTRY  *pMacroIndexEntry;

    if (conditionFlag)
    {
        pMacroIndexEntry = &cp->tables->macroIndexTable[macroId];

        macroCount = pMacroIndexEntry->macroCount;
        pMacro = &cp->tables->macroTable[pMacroIndexEntry->macroIndex];

        // perform the set of register writes defined in the macro
        for (i = 0; i < macroCount; i++)
        {
            reg = pMacro[i].macroAddress;
            data = pMacro[i].macroValue;
            REG_WR32(reg, data);
        }
    }

    return rmStatus;

} // end of init_Macro()

RM_STATUS init_ZM_IO
(
    PHWINFO pDev,
    U032 port,
    U032 value,
    BOOL conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 reg;

    if (conditionFlag)
    {
        // convert the CRTC port into a priv register
        rmStatus = DevinitPortToPrivReg(port, &reg);
        if (rmStatus == RM_OK)
        {
            // write the value
            REG_WR32(reg, value);
        }
    }

    return rmStatus;

} // end of init_ZM_IO()

RM_STATUS init_Copy
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               srcReg,
    S008               srcShift,
    U032               srcMask,
    U032               port,
    U032               index,
    U032               destMask,
    BOOL               conditionFlag
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 srcValue;
    U032 value;
    U008 destValue;

    if (conditionFlag)
    {
        srcValue = REG_RD32(srcReg);
        if (srcShift >= 0)
        {
            srcValue >>= srcShift;
        }
        else
        {
            srcValue <<= -srcShift;
        }
        value = srcValue & srcMask;
        rmStatus = DevinitReadPort(pDev, port, index, &destValue);
        if (rmStatus == RM_OK)
        {
            destValue = (destValue & (U008) destMask) | (U008) value;
            rmStatus = DevinitWritePort(pDev, port, index, destValue);
        }
    }

    return rmStatus;

} // end of init_Copy()

RM_STATUS init_Repeat
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               ip,
    U032               loopCount
)
{
    RM_STATUS rmStatus = RM_OK;

    // setup the loop stack to contain the loop offset and the current loop count
    DevinitStackPush(&cp->loopStack, ip);
    DevinitStackPush(&cp->loopStack, loopCount);
    
    return rmStatus;

} // end of init_Repeat()

RM_STATUS init_EndRepeat
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032*              pip
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 count;

    // pop the current loop count from the loop stack and decrement it
    count = DevinitStackPop(&cp->loopStack);
    if (--count > 0)
    {
        // continue looping -- adjust offset to the top of the loop and push the counter back on
        *pip = DevinitStackTop(&cp->loopStack);
        DevinitStackPush(&cp->loopStack, count);
    }
    else
    {
        // terminate looping -- clear the rest of loop stack and go on to the next instruction
        (VOID)DevinitStackPop(&cp->loopStack);
    }
    
    return rmStatus;

} // end of init_EndRepeat()

RM_STATUS init_Function
(
    PHWINFO            pDev,
    PEXECUTION_CONTEXT cp,
    U032               functionIndex
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 functionId;

    // call the HAL to execute this "escape" function
    functionId = cp->tables->functionTable[functionIndex];
    // HalExecuteFunction(functionId);

    return rmStatus;

} // end of init_Function()

VOID DevinitMeminitFormat
(
    PHWINFO   pDev,
    char    **fmt,
    int      *padded_size
)
{
    U032 value;

    value = REG_RD32(NV_PBUS_PCI_NV_0);
    value >>= 16;
    value &= 0xffff;

    switch (value)
    {
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID0:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID1:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID2:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID3:
            *padded_size = sizeof(MEM_INIT_VALS_NV10);
            *fmt = MEM_INIT_VALS_NV10_FMT;
            break;

        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID0:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID1:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID2:
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID3:

        default:

            *padded_size = sizeof(MEM_INIT_VALS_NV15);
            *fmt = MEM_INIT_VALS_NV15_FMT;
            break;
    }
}

RM_STATUS DevinitReadPort
(
    PHWINFO pDev,
    U032    port,
    U032    index,
    U008   *pValue
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 privReg;
    U008 prevIndex;

    // convert the CRTC port into a priv register
    rmStatus = DevinitPortToPrivReg(port, &privReg);
    if (rmStatus == RM_OK)
    {
        // save the old index
        prevIndex = REG_RD08(privReg);

        // read the value
        REG_WR08(privReg, index);
        *pValue = REG_RD08(privReg + 1);

        // restore the old index
        REG_WR08(privReg, prevIndex);
    }

    return rmStatus;

} // end of DevinitReadPort()

RM_STATUS DevinitWritePort
(
    PHWINFO pDev,
    U032 port,
    U032 index,
    U032 value
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 privReg;
    U008 prevIndex;

    // convert the CRTC port into a priv register
    rmStatus = DevinitPortToPrivReg(port, &privReg);
    if (rmStatus == RM_OK)
    {
        // save the old index
        prevIndex = REG_RD08(privReg);

        // write the value
        REG_WR08(privReg, index);
        REG_WR08(privReg + 1, value);

        // restore the old index
        REG_WR08(privReg, prevIndex);
    }

    return rmStatus;

} // end of DevinitWritePort()

RM_STATUS DevinitPortToPrivReg
(
    U032 crtcReg, 
    U032* pPrivReg
)
{
    RM_STATUS rmStatus = RM_OK;

    switch (crtcReg)
    {
        case CRTC_INDEX_MONO:
        case CRTC_INDEX_COLOR:
        case ATC_INDEX:
        //...
            *pPrivReg = DEVICE_BASE(NV_PRMCIO) + crtcReg;
            break;

        case SEQ_INDEX:
        case GDC_INDEX:
        //...
            *pPrivReg = DEVICE_BASE(NV_PRMVIO) + crtcReg;
            break;

        default:
            rmStatus = RM_ERROR;
            break;
    }

    return rmStatus;

} // end of DevinitPortToPrivReg()

VOID DevinitStackPush
(
    PDEVINIT_STACK pStack,
    U032 value
)
{
    if (pStack->sp < DEVINIT_STACK_MAX)
    {
        (*pStack).body[++pStack->sp] = value;
#if defined(UNIX) && defined(IA64)
        __asm__ volatile("mf.a;;");
#endif
    }

} // end of DevinitStackPush()

U032 DevinitStackPop
(
    PDEVINIT_STACK pStack
)
{
    if (pStack->sp > 0)
    {
        return (*pStack).body[pStack->sp--];
    }

    // on stack underflow, return 0
    return 0;

} // end of DevinitStackPop()

U032 DevinitStackTop
(
    PDEVINIT_STACK pStack
)
{
    return (*pStack).body[pStack->sp];

} // end of DevinitStackTop()

RM_STATUS DevinitShadowBios
(
    PHWINFO pDev,
    PHWREG  externalBiosImage
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWREG tempBiosAddr;
    U032 i;

    if (externalBiosImage)
    {
        // setup for ROM reads
        tempBiosAddr = biosAddr;
        biosAddr = externalBiosImage;

        // copy the BIOS image into upper instance memory
        for (i = 0; i < BIOS_ROM_CODE_SIZE; i++)
        {
            REG_WR08(pDev->Pram.HalInfo.PraminOffset + i, BIOS_RD08(i));
        }

        // restore original biosAddr
        biosAddr = tempBiosAddr;
    }
    else
    {
        rmStatus = RM_ERROR;
    }

    return rmStatus;

} // end of DevinitShadowBios()

// extract the FminVCO/FmaxVCO values from the BIOS, if available
RM_STATUS DevinitGetMinMaxVCOValues
(
    PHWINFO    pDev,
    U032       *FminVCO,
    U032       *FmaxVCO
)
{
    RM_STATUS rmStatus = RM_OK;
    BMP_Control_Block bmpCtrlBlk;
    U032 offset;

    *FminVCO = *FmaxVCO = 0x0;

    // determine if we've got the correct BIOS rev
    rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);
    if (rmStatus != RM_OK)
        return rmStatus;

    if (bmpCtrlBlk.CTL_Version == BMP_INIT_TABLE_VER_5)
    {
        // FminVCO/FmaxVCO are only available in BMP 0x05.0x10 or greater
        if (bmpCtrlBlk.CTL_Flags < 0x10)
            return RM_ERROR;

        switch (bmpCtrlBlk.CTL_Flags)
        {
            // Version 0x10 did not have TMDS tables
            case 0x10:
            {
                BMP_Struc_Rev5_0x10 bmpInitTable;
                BiosReadStructure(pDev,
                                  &bmpInitTable,
                                  offset,
                                  (U032 *) 0,
                                  BMP_INIT5_0x10_FMT);

                *FminVCO = bmpInitTable.BMP_FminVco;
                *FmaxVCO = bmpInitTable.BMP_FmaxVco;
                rmStatus = RM_OK;
                break;
            }

            // default is BIP/3 BMP init table version 5, subversion 0x11 base-level functionality or above
            default:
            {
                BMP_Struc_Rev5_0x11 bmpInitTable;
                BiosReadStructure(pDev,
                                  &bmpInitTable,
                                  offset,
                                  (U032 *) 0,
                                  BMP_INIT5_0x11_FMT);

                *FminVCO = bmpInitTable.BMP_FminVco;
                *FmaxVCO = bmpInitTable.BMP_FmaxVco;
                rmStatus = RM_OK;
                break;
            }
        }
    }
    else
        rmStatus = RM_ERROR;

    return rmStatus;

} // end of DevinitGetGetMinMaxVCOValues()

// extract the flat panel flags byte for a given strap value
RM_STATUS DevinitGetFlatPanelFlags
(
    PHWINFO pDev,
    U032 panelStrap,
    U008* panelFlags
)
{
    RM_STATUS rmStatus = RM_OK;
    BMP_Control_Block bmpCtrlBlk;
    U032 offset;

    *panelFlags = 0;

    // Strap needs to be between 0 and 15
    if ( panelStrap >= MAX_FP_STRAPS )
    {
        return RM_ERROR;
    }

    // determine if we've got the correct BIOS rev
    rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);
    if (rmStatus != RM_OK)
        return rmStatus;

    if ( ( bmpCtrlBlk.CTL_Version == BMP_INIT_TABLE_VER_5 ) && ( bmpCtrlBlk.CTL_Flags >= 0x12 ) )
    {
        // default is BIP/3 BMP init table version 5, subversion 0x12 base-level functionality or above
        BMP_Struc_Rev5_0x12 bmpInitTable;
        rmStatus = BiosReadStructure
            (
                pDev,
                &bmpInitTable,
                offset,
                (U032 *) 0,
                BMP_INIT5_0x12_FMT
            );

        if ( rmStatus == RM_OK )
        {
            U008 panelFlagArray[ SIZE_FP_XLATE_FLAG_TABLE ];

            rmStatus = BiosReadBytes
            (
                pDev,
                panelFlagArray,
                bmpInitTable.BMP_FPXlateTablePtr + SIZE_FP_XLATE_TABLE,  // 16 is size of 
                sizeof( panelFlagArray ) // Size of FP_Xlate_Table and FP_Xlate_Flag_Table
            );

            if ( rmStatus == RM_OK )
            {
                *panelFlags = panelFlagArray[ panelStrap ];
            }
        }

    }
    else
    {
        rmStatus = RM_ERROR;
    }

    return rmStatus;

} // end of DevinitGetFlatPanelFlags()


// extract the flat panel flags byte for a given strap value
RM_STATUS DevinitGetManufacturerInfo
(
    PHWINFO pDev
)
{
  RM_STATUS rmStatus = RM_OK;
  BMP_Control_Block bmpCtrlBlk;
  U032 offset;
  U032 panelStrap = pDev->Dac.PanelStrap; // initialized in dacInitPanelStrap() 
                                          // called by initDac() or edidConstructMobileInfo()

  // If we've already tested this, and it doesn't work,
  // there's no need to test it again.
  if(pDev->Dac.ManufacturerProperties.Version == MANUFACTURER_NOT_CAPABLE)
    return RM_ERROR;

  // If we've already found the indexes, then return
  if(pDev->Dac.ManufacturerProperties.Version != MANUFACTURER_NOT_TESTED)
    return RM_OK;

  // Strap needs to be between 0 and 15
  if ( panelStrap >= MAX_FP_STRAPS )
  {
      return RM_ERROR;
  }

  // determine if we've got the correct BIOS rev
  rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);
  if (rmStatus != RM_OK)
      return rmStatus;

  // Manufacturer BIOSes only work with Control Blocks higher than or equal to 5.14
  if ( ( bmpCtrlBlk.CTL_Version == BMP_INIT_TABLE_VER_5 ) && ( bmpCtrlBlk.CTL_Flags >= 0x14 ) )
  {
      // default is BIP/3 BMP init table version 5, subversion 0x14 base-level functionality or above
      // Read the init table to get the pointers
      BMP_Struc_Rev5_0x14 bmpInitTable;
      rmStatus = BiosReadStructure(pDev, &bmpInitTable, offset, (U032 *) 0,BMP_INIT5_0x14_FMT);

      if ( rmStatus == RM_OK )
      {
          U008 panelManufacturerXlateArray[ SIZE_FP_XLATE_MANUFACTURER_TABLE ];

          // Read the Manufacturer Translation table - translates bootstrap to manufacturer entry index in table
          rmStatus = BiosReadBytes(pDev, panelManufacturerXlateArray, 
                                   bmpInitTable.BMP_FPXlateManufacturerTablePtr,
                                   sizeof( panelManufacturerXlateArray ) );

          if ( rmStatus == RM_OK )
          {
              PANEL_MANUFACTURER_TABLE_HEADER panelMTHeader;
              U032 panelMTHeaderPackedSize = 0;
              
              // We have a valid index into the Manufacturer's table
              // First let's get the header to determine the size of each entry.
              rmStatus = BiosReadStructure(pDev, &panelMTHeader, 
                                           bmpInitTable.BMP_LVDSManufacturerPtr,
                                           (U032 *) &panelMTHeaderPackedSize,
                                           INIT_MANUFACTURER_TABLE_HEADER_FORMAT);

              if( rmStatus == RM_OK)
              {
                  // Finally read the Manufacturer's Entry
                  switch(panelMTHeader.Version)
                  {
                  case 10:
                    {
                      PANEL_MANUFACTURER_TABLE_ENTRY_VER10 panelMTEntry;
    
                      rmStatus = BiosReadStructure(pDev, &panelMTEntry,
                                                   bmpInitTable.BMP_LVDSManufacturerPtr + 
                                                   panelMTHeaderPackedSize +
                                                   panelMTHeader.EntrySize * 
                                                   panelManufacturerXlateArray[ panelStrap ],
                                                   (U032 *) 0, 
                                                   INIT_MANUFACTURER_TABLE_ENTRY_VER10_FORMAT);
    
                      if(rmStatus == RM_OK)
                      {
                        // Save the information that we want.
                        pDev->Dac.ManufacturerProperties.Version            = panelMTHeader.Version;
                        pDev->Dac.ManufacturerProperties.Size               = panelMTHeader.EntrySize;
                        pDev->Dac.ManufacturerProperties.Flags              = panelMTEntry.Flags;
                        pDev->Dac.ManufacturerProperties.LVDSInitScript     = panelMTEntry.LVDSInitScript;
                        pDev->Dac.ManufacturerProperties.LVDSResetScript    = panelMTEntry.LVDSResetScript;
                        pDev->Dac.ManufacturerProperties.BackLightOnScript  = panelMTEntry.BackLightOnScript;
                        pDev->Dac.ManufacturerProperties.BackLightOffScript = panelMTEntry.BackLightOffScript;
                        pDev->Dac.ManufacturerProperties.PanelOnScript      = panelMTEntry.PanelOnScript;
                        pDev->Dac.ManufacturerProperties.PanelOffScript     = panelMTEntry.PanelOffScript;
                        pDev->Dac.ManufacturerProperties.PanelOffOnDelay    = panelMTEntry.PanelOffOnDelay;
    
                      }
                      break;
                    }
                  default:
                      // Unknown entry version!
                      pDev->Dac.ManufacturerProperties.Version = MANUFACTURER_NOT_CAPABLE;
                      return RM_ERROR;
                  }
              }
          }
      }

  }
  else
  {
      rmStatus = RM_ERROR;
  }

  if(rmStatus != RM_OK)
  {
      pDev->Dac.ManufacturerProperties.Version = MANUFACTURER_NOT_CAPABLE;
  }

  return rmStatus;

}


// extract the flat panel flags byte for a given strap value
RM_STATUS DevinitRunManufacturerScript
(
    PHWINFO pDev,
    U032 MScript
)
{
  RM_STATUS rmStatus = RM_OK;
  BMP_Control_Block bmpCtrlBlk;
  U032 offset;
  BMP_Struc_Rev5_0x14 bmpInitTable;
  U032 scriptIndex;
  U032 currentScriptOffset;
  EXECUTION_CONTEXT context;

  // If this BIOS is not capable of running these scripts, error out
  if(pDev->Dac.ManufacturerProperties.Version == MANUFACTURER_NOT_CAPABLE)
    return RM_ERROR;

  // If we've already found the indexes, then return error
  // You must call DevinitGetManufacturerInfo() before this function.
  if(pDev->Dac.ManufacturerProperties.Version == MANUFACTURER_NOT_TESTED)
  {
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Call DevinitGetManufacturerInfo() before DevinitRunManufacturerScript()\n");
    return RM_ERROR;
  }

  // Get the header block
  rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);
  if (rmStatus != RM_OK)
      return rmStatus;

  // default is BIP/3 BMP init table version 5, subversion 0x14 base-level functionality or above
  rmStatus = BiosReadStructure(pDev, &bmpInitTable, offset, (U032 *) 0, BMP_INIT5_0x14_FMT);
  if (rmStatus != RM_OK)
      return rmStatus;

  // Figure out which script we're running
  switch(MScript)
  {
  case MSCRIPT_LVDS_INIT_SCRIPT:
    scriptIndex = pDev->Dac.ManufacturerProperties.LVDSInitScript;
    break;
  case MSCRIPT_LVDS_RESET_SCRIPT:
    scriptIndex = pDev->Dac.ManufacturerProperties.LVDSResetScript;
    break;
  case MSCRIPT_BACKLIGHT_ON:
    scriptIndex = pDev->Dac.ManufacturerProperties.BackLightOnScript;
    break;
  case MSCRIPT_BACKLIGHT_OFF:
    scriptIndex = pDev->Dac.ManufacturerProperties.BackLightOffScript;
    break;
  case MSCRIPT_PANEL_ON:
    scriptIndex = pDev->Dac.ManufacturerProperties.PanelOnScript;
    break;
  case MSCRIPT_PANEL_OFF:
    scriptIndex = pDev->Dac.ManufacturerProperties.PanelOffScript;
    break;
  default:
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: DevinitRunManufacturerScript passed invalid MScript\n");
    return RM_ERROR;
    break;
  }

  // initialize execution context
  context.conditionFlag = TRUE;
  context.loopStack.sp = 0;

  // get all table offsets
  // rmStatus = DevinitGetBIP3TableOffsets(pDev, &context);
  context.scriptTableOffset          = bmpInitTable.BMP_InitScriptTablePtr;
  context.macroIndexTableOffset      = bmpInitTable.BMP_MacroIndexTablePtr;
  context.macroTableOffset           = bmpInitTable.BMP_MacroTablePtr;
  context.conditionTableOffset       = bmpInitTable.BMP_ConditionTablePtr;
  context.ioConditionTableOffset     = bmpInitTable.BMP_IOConditionTablePtr;
  context.ioFlagConditionTableOffset = bmpInitTable.BMP_IOFlagConditionTablePtr;
  context.functionTableOffset        = bmpInitTable.BMP_InitFunctionTablePtr;

  context.TMDSSingleAOffset          = bmpInitTable.BMP_TMDSSingleAPtr;
  context.TMDSSingleBOffset          = bmpInitTable.BMP_TMDSSingleBPtr;
  context.TMDSDualOffset             = bmpInitTable.BMP_TMDSDualPtr;   
  context.LVDSSingleAOffset          = bmpInitTable.BMP_LVDSSingleAPtr;
  context.LVDSSingleBOffset          = bmpInitTable.BMP_LVDSSingleBPtr;
  context.LVDSDualOffset             = bmpInitTable.BMP_LVDSDualPtr;   
  context.OffSingleAOffset           = bmpInitTable.BMP_OffSingleAPtr; 
  context.OffSingleBOffset           = bmpInitTable.BMP_OffSingleBPtr; 

  // interpret the script in the script table at the given index
  currentScriptOffset = BiosRead16(pDev, context.scriptTableOffset + (scriptIndex*2));

  // Don't run NULL pointers
  if(currentScriptOffset)
    rmStatus = DevinitInterpretBIP3Script(pDev, &context, currentScriptOffset);
  
  return rmStatus;
}



// end of devinit.c

//
// modehw.c -  OS specific NV register access routines for mode set code.
//
// These Win NT specific routines access the NV3 through a series of NV
// engine pointers contained in the global HwDeviceExtension and frame
// buffer pointer, fbAddr.
//
// Copyright (c) 1997 - NVidia Corporation. All Rights Reserved.
//
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"

#include "nv.h"
#include "nv3a_ref.h"

#include "modes.h"

PHW_DEVICE_EXTENSION HwDeviceExtension;
PHWREG fbAddr;

//******************************************************************************
// External function declarations
//******************************************************************************

// write a dword value to a fb address
VOID WriteFb32(U032 datum, U032 addr)
{
    MEM_WR32(addr, datum);
}


// read a dword value from a fb address
U032 ReadFb32(U032 addr)
{
    return MEM_RD32(addr);
}


// write a dword value to an NV register
// assume extended CRTC registers are unlocked
VOID WritePriv32(U032 datum, U032 addr)
{
    if (addr >= DEVICE_BASE(NV_PRMVIO) && addr < DEVICE_BASE(NV_PRMVIO) + DEVICE_EXTENT(NV_PRMVIO))
    {
        PRMVIO_REG_WR32(PRMVIO_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PRMCIO) && addr < DEVICE_BASE(NV_PRMCIO) + DEVICE_EXTENT(NV_PRMCIO))
    {
        PRMCIO_REG_WR32(PRMCIO_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PRAMDAC) && addr < DEVICE_BASE(NV_PRAMDAC) + DEVICE_EXTENT(NV_PRAMDAC))
    {
        PRAMDAC_REG_WR32(PRAMDAC_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PFB) && addr < DEVICE_BASE(NV_PFB) + DEVICE_EXTENT(NV_PFB))
    {
        PFB_REG_WR32(PFB_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PMC) && addr < DEVICE_BASE(NV_PMC) + DEVICE_EXTENT(NV_PMC))
    {
        PMC_REG_WR32(PMC_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PEXTDEV) && addr < DEVICE_BASE(NV_PEXTDEV) + DEVICE_EXTENT(NV_PEXTDEV))
    {
        PEXTDEV_REG_WR32(PEXTDEV_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PBUS) && addr < DEVICE_BASE(NV_PBUS) + DEVICE_EXTENT(NV_PBUS))
    {
        PBUS_REG_WR32(PBUS_Base, addr, datum);
        return;
    }

    // catch bad ranges
    //DebugBreakPoint();
}


// read a dword value from an NV register
U032 ReadPriv32(U032 addr)
{
    if (addr >= DEVICE_BASE(NV_PRMVIO) && addr < DEVICE_BASE(NV_PRMVIO) + DEVICE_EXTENT(NV_PRMVIO))
    {
        return PRMVIO_REG_RD32(PRMVIO_Base, addr);
    }
    if (addr >= DEVICE_BASE(NV_PRMCIO) && addr < DEVICE_BASE(NV_PRMCIO) + DEVICE_EXTENT(NV_PRMCIO))
    {
        return PRMCIO_REG_RD32(PRMCIO_Base, addr);
    }
    if (addr >= DEVICE_BASE(NV_PRAMDAC) && addr < DEVICE_BASE(NV_PRAMDAC) + DEVICE_EXTENT(NV_PRAMDAC))
    {
        return PRAMDAC_REG_RD32(PRAMDAC_Base, addr);
    }
    if (addr >= DEVICE_BASE(NV_PFB) && addr < DEVICE_BASE(NV_PFB) + DEVICE_EXTENT(NV_PFB))
    {
        return PFB_REG_RD32(PFB_Base, addr);
    }
    if (addr >= DEVICE_BASE(NV_PMC) && addr < DEVICE_BASE(NV_PMC) + DEVICE_EXTENT(NV_PMC))
    {
        return PMC_REG_RD32(PMC_Base, addr);
    }
    if (addr >= DEVICE_BASE(NV_PEXTDEV) && addr < DEVICE_BASE(NV_PEXTDEV) + DEVICE_EXTENT(NV_PEXTDEV))
    {
        return PEXTDEV_REG_RD32(PEXTDEV_Base, addr);
    }
    if (addr >= DEVICE_BASE(NV_PBUS) && addr < DEVICE_BASE(NV_PBUS) + DEVICE_EXTENT(NV_PBUS))
    {
        return PBUS_REG_RD32(PBUS_Base, addr);
    }

    // catch bad ranges
    //DbgBreakPoint();

	return(0);
}

// write a data word to Priveledged IO
VOID WriteIndexed(U032 addr, U016 datum)
{
    //
    // Keep it 8bit to be safe; index in lower byte, data in upper
    //
    if (addr >= DEVICE_BASE(NV_PRMVIO) && addr < DEVICE_BASE(NV_PRMVIO) + DEVICE_EXTENT(NV_PRMVIO))
    {
        PRMVIO_REG_WR08(PRMVIO_Base, addr, datum & 0xFF);
        PRMVIO_REG_WR08(PRMVIO_Base, addr+1, (datum >> 8) & 0xFF);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PRMCIO) && addr < DEVICE_BASE(NV_PRMCIO) + DEVICE_EXTENT(NV_PRMCIO))
    {
        PRMCIO_REG_WR08(PRMCIO_Base, addr, datum & 0xFF);
        PRMCIO_REG_WR08(PRMCIO_Base, addr+1, (datum >> 8) & 0xFF);
        return;
    }

    // catch bad ranges
    //DbgBreakPoint();

}


// write a data byte to Priveledged IO
VOID WritePriv08(U032 addr, U008 datum)
{
    if (addr >= DEVICE_BASE(NV_PRMVIO) && addr < DEVICE_BASE(NV_PRMVIO) + DEVICE_EXTENT(NV_PRMVIO))
    {
        PRMVIO_REG_WR08(PRMVIO_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_PRMCIO) && addr < DEVICE_BASE(NV_PRMCIO) + DEVICE_EXTENT(NV_PRMCIO))
    {
        PRMCIO_REG_WR08(PRMCIO_Base, addr, datum);
        return;
    }
    if (addr >= DEVICE_BASE(NV_USER_DAC) && addr < DEVICE_BASE(NV_USER_DAC) + DEVICE_EXTENT(NV_USER_DAC))
    {
        USERDAC_REG_WR08(USERDAC_Base, addr, datum);
        return;
    }

    // catch bad ranges
    //DbgBreakPoint();

}

// read a data word from Priveledged IO
U016 ReadIndexed(U032 addr, U008 reg)
{
    U016   datum;

    //
    // Return the data in the upper byte, index in the lower
    //
    if (addr >= DEVICE_BASE(NV_PRMVIO) && addr < DEVICE_BASE(NV_PRMVIO) + DEVICE_EXTENT(NV_PRMVIO))
    {
        PRMVIO_REG_WR08(PRMVIO_Base, addr, reg);
        datum = (PRMVIO_REG_RD08(PRMVIO_Base, addr+1) << 8) & 0xFF00;
        datum |= reg;
        return datum;
    }
    if (addr >= DEVICE_BASE(NV_PRMCIO) && addr < DEVICE_BASE(NV_PRMCIO) + DEVICE_EXTENT(NV_PRMCIO))
    {
        PRMCIO_REG_WR08(PRMCIO_Base, addr, reg);
        datum = (PRMCIO_REG_RD08(PRMCIO_Base, addr+1) << 8) & 0xFF00;
        datum |= reg;
        return datum;
    }

    // catch bad ranges
    //DbgBreakPoint();

	return(0);
}


// end of modehw.c



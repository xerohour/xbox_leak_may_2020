//
// modehw.c -  OS specific NV register access routines for mode set code.
//
// These routines utilize IO to access the NV3 controller. To use
// the modeset code in a non-IO based environment, the following
// routines must be modified to access the NV3 through the PCI
// addressing.
//
// Copyright (c) 1997 - NVidia Corporation. All Rights Reserved.
//
#include <nv_ref.h>
#include <nvrm.h>
#include "nvhw.h"
#include "modes.h"


//
// Enable Register Extensions
//
VOID OEMEnableExtensions(VOID)
{
    WriteIndexed(NV_PRMCIO_CRX__COLOR, 0x571F);
}                            

//
// Disable Register Extensions
// 
VOID OEMDisableExtensions(VOID)
{       
    WriteIndexed(NV_PRMCIO_CRX__COLOR, 0x991F);
}                                 

//       WriteFb32 - Write a dword value to a fb address
//
//       Entry:  EAX = Value to write
//               ESI = fb offset
//       Exit:   None
//
VOID WriteFb32(U032 datum, U032 addr)
{
    fbAddr->Reg032[addr] = datum;       // MEM_WR32/PDFB macros resolve to this
}                          


//       ReadFb32 - Read a dword value from a fb address
//
//       Entry:  ESI = fb offset
//       Exit:   EAX = Value read
//
U032 ReadFb32(U032 addr)
{               
    return fbAddr->Reg032[addr];        // MEM_RD32/PDFB macros resolve to this
}                          


//       WritePriv32 - Write a dword value to an NV register
//
//       Entry:  EAX = Value to write
//               ESI = NV register address
//               Extended CRTC registers unlocked
//       Exit:   None
//
VOID WritePriv32(U032 datum, U032 addr)
{
    REG_WR32(addr, datum);
}                          


//       ReadPriv32 - Read a dword value from an NV register
//
//       Entry:  ESI = NV register address
//               Extended CRTC registers unlocked
//       Exit:   EAX = Value read
//
U032 ReadPriv32(U032 addr)
{               
    return(REG_RD32(addr));
}                          

// Write a data word to Priveledged IO
//
// Entry: U032 addr                // Address of Controller Register
//        U016 datum              // Data to write to SR/GR/CR
//
// Exit:  none
//
VOID WriteIndexed(U032 addr, U016 datum)
{                                    
    //
    // Keep it 8bit to be safe; index in lower byte, data in upper
    //
    REG_WR08(addr, datum & 0xFF);
    REG_WR08(addr+1, (datum >> 8) & 0xFF);
}                          


// Write a data byte to Priveledged IO
//
// Entry: U032 addr                // Address of Controller Register
//        U008 datum               // Data to write to SR/GR/CR
//
// Exit:  none
//
VOID WritePriv08(U032 addr, U008 datum)
{                                    
    REG_WR08(addr, datum);
}                          

//
// Read a data word from Priveledged IO
//
// Entry: U032 addr                // Address of controller register
//        U008 reg                 // CR/GR/SR register to read
//
// Exit:  index/data pair
//
U016 ReadIndexed(U032 addr, U008 reg)
{      
    U016 datum;
              
    //
    // Return the data in the upper byte, index in the lower
    //
    REG_WR08(addr, reg);
    datum = (REG_RD08(addr+1) << 8) & 0xFF00;
    datum |= reg;
    
    return(datum);
}                          


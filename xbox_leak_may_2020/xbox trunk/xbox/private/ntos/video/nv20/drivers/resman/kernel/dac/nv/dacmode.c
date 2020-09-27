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

/******************************* DAC Mode Control ***************************\
*                                                                           *
* Module: DACMODE.C                                                         *
*   The DAC mode control management is updated here.                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
#include "nvhw.h"
#include <vga.h>
#include <nvcm.h>
#include <bios.h>   //For hotkey defs.  Can remove once those are put in nv_ref.h

extern char strDevNodeRM[];
extern char strLastExtDevice[];
extern char strCurWinDevices[];

//------------------------------------------------------------------------
// First 16 colors for mode 13h (compatible R'G'B'RGB)
//
U008 CompatColors[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
                        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};
                         
//------------------------------------------------------------------------
// Gray values for next 16 (6 bits of intensity)
//
U008 GrayColors[] = {0x00, 0x05, 0x08, 0x0B, 0x0E, 0x11, 0x14, 0x18,
                      0x1C, 0x20, 0x24, 0x28, 0x2D, 0x32, 0x38, 0x3F};
                                                                              
//------------------------------------------------------------------------
// Fixup values for DAC color registers after summing to gray shades
//
U008 FixupColors[] = {0x00, 0x05, 0x11, 0x1C, 0x08, 0x0B, 0x14, 0x28,
                       0x0E, 0x18, 0x2D, 0x32, 0x20, 0x24, 0x38, 0x3F};
                                                                       
//------------------------------------------------------------------------
// 9 groups of five intensities are used to generate the next 216 colors.
// Each group is used to generate 24 colors by walking around a "color
// triangle" with eight colors per side.
//
U008 IntenseTable[] =
{
    0x00, 0x10, 0x1F, 0x2F, 0x3F,
    0x1F, 0x27, 0x2F, 0x37, 0x3F,
    0x2D, 0x31, 0x36, 0x3A, 0x3F,
                                
    0x00, 0x07, 0x0E, 0x15, 0x1C,
    0x0E, 0x11, 0x15, 0x18, 0x1C,
    0x14, 0x16, 0x18, 0x1A, 0x1C,
                                
    0x00, 0x04, 0x08, 0x0C, 0x10,
    0x08, 0x0A, 0x0C, 0x0E, 0x10,
    0x0B, 0x0C, 0x0D, 0x0F, 0x10
};                              

//
// KJK I suspect this could be removed (or at least shortened) since we only used
//      one of the mode settings
//

//------------------------------------------------------------------------
//  Basic VGA mode default settings. This setup is used initially in all 
//  mode sets.
//
VGATBL   VGATable =
{
    // U008   columns;
    // U008   rows;
    // U008   char_height;
    // U016   RegenLenght;
    // SEQ    seq_regs;              // SR1-4
    // U008   PT_Misc;               // Misc Register
    // CRTC   crtc_regs;             // CR0-18
    // U008   PT_ATC[20];            // Attribute Controller
    // GR     gr_regs;               // GR0-8

    // Basic CRTC Table for Packed Pixel Graphics Modes.
     0x50, 0x1D, 0x10,
     0xFFFF,
    {0x01, 0x0F, 0x00, 0x0E},               // Seq
     0xEB,
    {0x5F, 0x4F, 0x4F, 0x83, 0x53, 0x9F,    // CR0-18
     0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0xEA, 0x0C,
     0xDF, 0x50, 0x00, 0xDF, 0x0C, 0xE3,
     0xFF},
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,    // Attr
     0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
     0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00,
     0x0F, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x40,    // GR
     0x05, 0x0F, 0xFF}
};    

/*************************** Helper functions *******************************/
//
//       dacWriteColor - Writes one color value (three bytes) to the video DAC
//               R(analog) = R * 2Ah + R' * 15h
//               G(analog) = G * 2Ah + G' * 15h
//               B(analog) = B * 2Ah + B' * 15h
//
static VOID dacWriteColor(PHWINFO pDev, U032 head, U008 colorval)
{
    U008   component;          // color component 4=red/2=green/1=blue
    U008   color;              // Color component to write
    
    for (component = 0x04; component; component >>= 1)
    {
        // Color?
        if (colorval & component)
            color = 0x2A;               // Add color
        else
            color = 0;                  // No color on this component
        
        // Intensity?
        if (colorval & (component << 3))
            color += 0x15;
    
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, color, head);
        ////WritePriv08(NV_USER_DAC_PALETTE_DATA, color);
    }
}        
        
//
//       dacWriteColor2 - Writes one color value (three bytes) to the video DAC
//               R(analog) = R * 2Ah + R' * 15h
//               G(analog) = G * 2Ah + G' * 15h
//               B(analog) = B * 2Ah + B' * 15h
//
static VOID dacWriteColor2(PHWINFO pDev, U032 head, U016 red, U016 green, U016 blue, U016 tblidx)
{
    DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+red], head);
    DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+green], head);
    DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+blue], head);

    ////WritePriv08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+red]);
    ////WritePriv08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+green]);
    ////WritePriv08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+blue]);
}

//
//       dacLoadColorDACTable - Load the DAC for the 16-color modes
//
static VOID dacLoadColorDACTable(PHWINFO pDev, U032 head)
{
    U008  i;
    
    // Select the DAC address
    DAC_REG_WR08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00, head);
    ////WritePriv08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00);
    
    // Do the first 64 locations
    for (i = 0; i < 64; i++)
        dacWriteColor(pDev, head, i);
        
}
    
//
//       dacLoadColor256DAC - Load the DAC for the 256-color modes
//
static VOID dacLoadColor256DAC(PHWINFO pDev, U032 head)
{
    U016  i;
    U016  red;
    U016  green;
    U016  blue;
    
    // Select the DAC address
    DAC_REG_WR08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00, head);
    ////WritePriv08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00);
    
    //--------------------------------------------------------------------
    // The first 16 colors are the compatible colors (i.e. 0,1,2,3,4,5,14,
    // 7,38,39,3A,3B,3C,3D,3E,3F).
    //
    for (i = 0; i < 16; i++)
        dacWriteColor(pDev, head, CompatColors[i]);
    
    //--------------------------------------------------------------------
    // The next 16 give various intensities of gray (0,5,8,B,E,11,14,18,
    // 1C,20,24,28,2D,32,38,3F).
    //
    for (i = 0; i < 16; i++)
    {
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i], head);
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i], head);
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i], head);
        
        ////WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i]);
        ////WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i]);
        ////WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i]);
    }                                                
        
    //--------------------------------------------------------------------
    // The next 216 registers give a variety of colors.
    //
    
    red = 0;
    green = 0;
    blue = 4;

    for (i = 0; i < 9; i++)
    {                       
    
        // I know these could probably be a function, but for now...
        // (this is getting tedious).
        
        // Traverse(blue, red);
        do
        {
            // To loop
            dacWriteColor2(pDev, head, red, green, blue, (U016)(i * 5));
            red++;                                          
        } while (red < blue);
        
        do
        {
            // From loop
            dacWriteColor2(pDev, head, red, green, blue, (U016)(i * 5));
            blue--;
        } while (blue);
        
        // Traverse(red, green);
        do
        {
            // To loop
            dacWriteColor2(pDev, head, red, green, blue, (U016)(i * 5));
            green++;
        } while (green < red);
                         
        do
        {
            // From loop
            dacWriteColor2(pDev, head, red, green, blue, (U016)(i * 5));
            red--;
        } while (red);
                 
            
        // Traverse(green, blue);
        do
        {
            // To loop
            dacWriteColor2(pDev, head, red, green, blue, (U016)(i * 5));
            blue++;
        } while (blue < green);
                         
        do
        {
            // From loop
            dacWriteColor2(pDev, head, red, green, blue, (U016)(i * 5));
            green--;
        } while (green);
    }                
             
    //-----------------------------------------------------------------------
    // The last 8 colors are black.
    //
    //               +---+-- 8 blanks * 3 locations (r/g/b)
    for (i = 0; i < (8 * 3); i++)
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, 0x00, head);
        ////WritePriv08(NV_USER_DAC_PALETTE_DATA, 0x00);
}

//
//       dacLoadIdentityDAC - Load the DAC with a gamma ramp
//
static VOID dacLoadIdentityDAC(PHWINFO pDev, U032 head)
{
    U016  i;
    
    // Select the DAC address
    DAC_REG_WR08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00, head);
                                                  
    for (i = 0; i < 256; i++)
    {           
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)i, head);
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)i, head);
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)i, head);
        
    }                                     
}

//
// Program the default video palette
//
static VOID dacLoadPalette
(
    PHWINFO pDev,
    U032    Head,
    U032    PixelDepth
)
{
    U008 i;
    U008 *pal = (U008 *)&(VGATable.PT_ATC); // default packed pixel palette
    volatile U008 scratch8;
    
    scratch8 = REG_RD08(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    ////ReadPriv32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    
    for (i = 0; i < 20; i++)
    {
        ATR_WR(i, *pal++, Head);               
        
        ////WritePriv08(NV_PRMCIO_ARX, i);      // Select AR
        ////WritePriv08(NV_PRMCIO_ARX, *pal++); // Write color data
    }
                                                
    scratch8 = REG_RD08(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    ////ReadPriv32(NV_PRMCIO_INP0__COLOR);        // Reset ATC FlipFlop
    
//    DAC_WR08(NV_PRMCIO_ARX, 0x14);   // Select AR14
//    DAC_WR08(NV_PRMCIO_ARX, 0x00);   // Zero it!
    ATR_WR(0x14, 0, Head);                 // Write 0 to AR14
    DAC_REG_WR08(NV_USER_DAC_PIXEL_MASK, 0xFF, Head);      // Write DAC Mask

    ////WritePriv08(NV_PRMCIO_ARX, 0x14);   // Select AR14
    ////WritePriv08(NV_PRMCIO_ARX, 0x00);   // Zero it!
    ////WritePriv08(NV_USER_DAC_PIXEL_MASK, 0xFF);      // Write DAC Mask

    switch(PixelDepth)
    {
        case    4:          // 4bpp - Planar -- CAN'T HAPPEN IN WINDOWS DRIVER
            dacLoadColorDACTable(pDev, Head);
            break;

        case    8:          // 8bpp - Packed
            dacLoadColor256DAC(pDev, Head);
            break;

        case    16:         // 16bpp - Packed
        case    15:         // 16bpp - Packed
        case    24:
        case    32:
        default:
            dacLoadIdentityDAC(pDev, Head);
            break;
    }
}

static VOID dacLoadIndexRange(PHWINFO pDev, U032 addr, U008 st_idx, U008 count, U008 *regs)
{
    U016  i;
    U016  wv;
    
    // Program <addr> register from <st_idx> to <st_idx + count>, using data from <regs>
    for (i = 0; i < count; i++)
    {               
        wv = *regs++;
        wv <<= 8;               // Put data in high half
        wv |= st_idx++;         // Get index and increment it
        WriteIndexed(addr, wv);
    }               
}                                  



//
// Program the default sequencer/graphics controller parameters
// PRMVIO for head 2 is accessed by setting CR44.
//
static VOID dacProgramSeq
(
    PHWINFO pDev,
    U032    Head
)
{
    U016  wv;
    
    // Load Sequencer and Misc. Output
    WriteIndexed(NV_PRMVIO_SRX, 0x0100);         // Sync Reset
    
    REG_WR08(NV_PRMVIO_MISC__WRITE, VGATable.PT_Misc);
                                                   
    // Load Sequencers, beginning at SR1, load 4 SR's (SR1-4), from the mode table
    dacLoadIndexRange(pDev, NV_PRMVIO_SRX, 1, sizeof(SEQ), (U008 *)&(VGATable.seq_regs));
                                            
    WriteIndexed(NV_PRMVIO_SRX, 0x0300);         // End Sync Reset
                                  
    // Load GDC's
    // Load GR, beginning at GR0, for all GR's (GR0-8), from the mode table
    dacLoadIndexRange(pDev, NV_PRMVIO_GRX, 0, sizeof(GR), (U008 *)&(VGATable.gr_regs));

    //
    // Common area of OEMSetRegs()
    //
    
    // User select MNP for VPLL, enable CPU access to FB, enable CGA I/O space.
    REG_WR08(NV_PRMVIO_MISC__WRITE, 0x2B);
                                                                  
    wv = 0x01;
    wv <<= 8;
    wv |= 0x01;
    WriteIndexed(NV_PRMVIO_SRX, wv); // Write SR01
    
}

//
// Program the CRTC timing (plus PRAMDAC_GENERAL_CONTROL and MISC OUTPUT sync polarities)
//
VOID dacProgramCRTC
(
    PHWINFO pDev,
    U032    Head,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac,
    U032    HorizontalVisible,        // in pixels
    U032    HorizontalBlankStart,     // in pixels
    U032    HorizontalRetraceStart,   // in pixels
    U032    HorizontalRetraceEnd,     // in pixels
    U032    HorizontalBlankEnd,       // in pixels
    U032    HorizontalTotal,          // in pixels
    U032    VerticalVisible,          // in lines
    U032    VerticalBlankStart,       // in lines
    U032    VerticalRetraceStart,     // in lines
    U032    VerticalRetraceEnd,       // in lines
    U032    VerticalBlankEnd,         // in lines
    U032    VerticalTotal,            // in lines
    U032    PixelDepth,
    U032    PixelClock,
    U032    HorizontalSyncPolarity,
    U032    VerticalSyncPolarity,
    U032    DoubleScannedMode
)
{
    U032 logicalwidth;
    U032 ramDacControl;
    U008 val08;
    U016 LC;

    // Here is how these timing values map to nv registers.
    // 
    // HorizontalTotal[8]           -> cr2D[0]  (# of characters - 5)
    // HorizontalTotal[7:0]         -> cr0[7:0] (# of characters - 5)
    // HorizontalVisible[8]         -> cr2D[1]  (in characters - 1)
    // HorizontalVisible[7:0]       -> cr1[7:0] (in characters - 1)
    // HorizontalBlankStart[8]      -> cr2D[2]  (in characters - 1)
    // HorizontalBlankStart[7:0]    -> cr2[7:0] (in characters - 1)
    // HorizontalBlankEnd[6]        -> cr25[4]
    // HorizontalBlankEnd[5]        -> cr5[7]
    // HorizontalBlankEnd[4:0]      -> cr3[4:0]
    // HorizontalRetraceStart[8]    -> cr2D[3]  (in characters + 1)
    // HorizontalRetraceStart[7:0]  -> cr4[7:0] (in characters + 1)
    // HorizontalRetraceEnd         -> cr5[4:0] (RetraceStart + pulseWidth = value programmed in this register)
    // VerticalVisible[10]          -> cr25[1]
    // VerticalVisible[9]           -> cr7[6]
    // VerticalVisible[8]           -> cr7[1]
    // VerticalVisible[7:0]         -> cr12[7:0]
    // VerticalBlankStart[10]       -> cr25[3]
    // VerticalBlankStart[9]        -> cr9[5]
    // VerticalBlankStart[8]        -> cr7[3]
    // VerticalBlankStart[7:0]      -> cr15[7:0]
    // VerticalRetraceStart[10]     -> cr25[2]
    // VerticalRetraceStart[9]      -> cr7[7]
    // VerticalRetraceStart[8]      -> cr7[2]
    // VerticalRetraceStart[7:0]    -> cr10[7:0]
    // VerticalRetraceEnd           -> cr11[3:0] (RetraceStart + pulseWidth = value programmed in this register)
    // VerticalBlankEnd             -> cr16
    // VerticalTotal[10]            -> cr25[0]
    // VerticalTotal[9]             -> cr7[5]
    // VerticalTotal[8]             -> cr7[0]
    // VerticalTotal[7:0]           -> cr6[7:0] (#of scanlines - 2)
    // PixelDepth                   -> cr13 (logical display width)
    //                              -> NV_PRAMDAC_GENERAL_CONTROL
    //                              -> cr28[1:0]
    // PixelClock                   -> NV_PRAMDAC_VPLL_COEFF
    // HorizontalSyncPolarity       -> NV_PRMVIO_MISC__WRITE[6]
    // VerticalSyncPolarity         -> NV_PRMVIO_MISC__WRITE[7]
    // DoubleScannedMode            -> cr9[7]
    //

    // Program up the basic registers first
    //
    
    //
    // Although the VGA spec is not clear on this, there is a single implicit line delay in the VRS and VRE
    // signals, so preadjust these values.
    //
    VerticalRetraceStart--;
    VerticalRetraceEnd--;
    
    // Line compare register is used to split the screen in half. We don't support this feature.
    // So the Line Compare register value is always 0x3ff.
    LC = 0x3ff;

    if (DoubleScannedMode)
    {
        // The verticalVisible and verticalBlankStart need to be doubled. 
        // The rest of the vertical timing modes are already doubled in the table.
        VerticalVisible *= 2;
        VerticalBlankStart *= 2;
    }

    //
    // CR0 (Horizontal Total) 
    //  Total number of horizontal characters in a scan line - 5
    //  Maximum horizontal resolution is 260 * 8 * (0.8), where (0.8) is visible %
    //
    CRTC_WR(NV_CIO_CR_HDT_INDEX, (U008)(HorizontalTotal >> 3) - 5, Head);
    
    //
    // CR1 (Horizontal Display End)
    //  Total number of displayed characters in a scan line - 1
    //
    CRTC_WR(NV_CIO_CR_HDE_INDEX, (U008)(HorizontalVisible >> 3) - 1, Head);
                   
    //
    // CR2 (Horizontal Blanking Start)
    //  Location where the horizontal blanking will start (in characters)
    //
    CRTC_WR(NV_CIO_CR_HBS_INDEX, (U008)((HorizontalBlankStart >> 3) - 1), Head);                   
        
    //
    // CR3 (Horizontal Blanking End)
    //  [7:7] CR (must be 1)
    //  [6:5] Display skew control (should be 0)
    //  [4:0] least significant 5 bits of Horizontal blanking end value (in characters).
    //
    CRTC_WR(NV_CIO_CR_HBE_INDEX, (U008)(((HorizontalBlankEnd >> 3) - 1) & 0x1F) | BIT(7), Head);
                   
    //
    // CR4 (Horizontal Retrace Start)
    //  Character position where the horizontal Retrace becomes active
    //
    CRTC_WR(NV_CIO_CR_HRS_INDEX, (U008)(HorizontalRetraceStart >> 3) + 1, Head);
                   
    //
    // CR5 (Horizontal Retrace End)
    //  [7:7] Bit6 of HBE (CR3)
    //  [6:5] Display skew control (should be 0)
    //  [4:0] Location of the end of the horizontal retrace period (in characters)
    //
    CRTC_WR(NV_CIO_CR_HRE_INDEX, ((U008)(((HorizontalRetraceEnd >> 3) + 1) & 0x1f))
                               | ((((HorizontalBlankEnd >> 3) - 1) & BIT(5)) ? BIT(7) : 0), Head);

    //
    // CR6 (Vertical Total)
    //  Total number of vertical scan lines on the monitor - 2
    //
    CRTC_WR(NV_CIO_CR_VDT_INDEX, (U008)((VerticalTotal - 2) & 0xFF), Head);
                   
    //
    // CR7 (Overflow Register)
    //  [7:7] Bit9 of VRS (CR10)
    //  [6:6] Bit9 of VDE (CR12)
    //  [5:5] Bit9 of VTR (CR6)
    //  [4:4] Bit8 of LC (CR18) 
    //  [3:3] Bit8 of VBS (CR15)
    //  [2:2] Bit8 of VRS (CR10)
    //  [1:1] Bit8 of VDE (CR12)
    //  [0:0] Bit8 of VTR (CR6)
    //
    CRTC_WR(NV_CIO_CR_OVL_INDEX, ((VerticalRetraceStart & BIT(9))     ? BIT(7) : 0x00)
                               | (((VerticalVisible - 1) & BIT(9))    ? BIT(6) : 0x00) 
                               | (((VerticalTotal - 2) & BIT(9))      ? BIT(5) : 0x00)
                               | ((LC & BIT(8))                       ? BIT(4) : 0x00)
                               | (((VerticalBlankStart - 1) & BIT(8)) ? BIT(3) : 0x00)
                               | ((VerticalRetraceStart & BIT(8))     ? BIT(2) : 0x00)
                               | (((VerticalVisible - 1) & BIT(8))    ? BIT(1) : 0x00)
                               | (((VerticalTotal - 2) & BIT(8))      ? BIT(0) : 0x00), Head);
                   
    //
    // CR8 (Screen A preset row scan)
    //  Should be set to 0 for us.
    CRTC_WR(NV_CIO_CR_RSAL_INDEX, 0, Head);

    //
    // CR9 (Character Cell Height register)
    //  [7:7] Scan double mode
    //  [6:6] Bit9 of LC (CR18) 
    //  [5:5] Bit9 of VBS (CR15)
    //  [4:0] Scan lines per character row - 1
    //
    CRTC_WR(NV_CIO_CR_CELL_HT_INDEX, ((DoubleScannedMode)                 ? BIT(7) : 0x00)
                                   | ((LC & BIT(9))                       ? BIT(6) : 0x00)
                                   | (((VerticalBlankStart - 1) & BIT(9)) ? BIT(5) : 0x00), Head);
                 
    if (pDev->pStereoParams &&
       (pDev->pStereoParams->Flags & STEREOCFG_STEREOACTIVATED))
    {
        //The video start address is about to change. We want to turn off stereo as soon as possible
        pDev->pStereoParams->Flags &= ~STEREOCFG_STEREOACTIVATED;
    }            

    //
    // CR0C/CR0D (Start Address)
    //
    /*
    //this should be extracted from the HalInfo structure, not assumed to be zero
    CRTC_WR(NV_CIO_CR_SA_HI_INDEX, 0, Head);                 
    CRTC_WR(NV_CIO_CR_SA_LO_INDEX, 0, Head);
    */
    //program start address through HAL layer function
    dacProgramVideoStart(pDev,Head,pDev->Framebuffer.HalInfo.Start[Head],pVidLutCurDac->Image[0].Pitch);                
                 
    //
    // CR10 (Vertical Retrace Start)
    //  Line position of the vertical Retrace start
    //
    CRTC_WR(NV_CIO_CR_VRS_INDEX, (U008)(VerticalRetraceStart & 0xFF), Head);
                   
    //
    // CR11 (Vertical Retrace End)
    //  [7:7] WriteProtect for CR0-7
    //  [6:6] Reserved (Bandwidth)
    //  [5:5] Vertical Interrupts (0)
    //  [4:4] Clear Vertical Interrupt (0)
    //  [3:0] Vertical Retrace End Width
    //
    CRTC_WR(NV_CIO_CR_VRE_INDEX, (U008)(VerticalRetraceEnd & 0x0f), Head); 
                               
    //
    // CR12 (Vertical Display End)
    //  Total number of displayed lines - 1
    //
    CRTC_WR(NV_CIO_CR_VDE_INDEX, (U008)((VerticalVisible - 1) & 0xFF), Head);
    
    //
    // CR13 (Logical Display Width)
    //  Display Pitch
    //  The value written to CR13/CR19 is the number 8-byte chunks
    //
    logicalwidth = pVidLutCurDac->Image[0].Pitch >> 3;
    CRTC_WR(NV_CIO_CR_OFFSET_INDEX, logicalwidth & 0xFF, Head);
    
    //
    // CR14 (Underline Location)
    //  Double Word mode
    //  Bit 6 is set in some VGA modes to change CRTC addressing
    //
    CRTC_WR(NV_CIO_CR_ULINE_INDEX, 0, Head);
    
    //
    // CR14 (Underline Location)
    //  Double Word mode
    //  Bit 6 is set in some VGA modes to change CRTC addressing into framebuffer
    //
    CRTC_WR(NV_CIO_CR_ULINE_INDEX, 0, Head);
    
    //
    // CR15 (Vertical Blank Start)
    //  Scan line value where the vertical blanking begins
    //
    CRTC_WR(NV_CIO_CR_VBS_INDEX, (U008)((VerticalBlankStart - 1) & 0xFF), Head);
    
    //
    // CR16 (Vertical Blank End)
    //  Scan line value where the vertical blanking Ends
    //
    CRTC_WR(NV_CIO_CR_VBE_INDEX, (U008)((VerticalBlankEnd - 1) & 0xFF), Head);

    //
    // CR17 (Mode register)
    //  bit 7 - retrace enable, bits 1,0 - memory addressing
    //  bit 6 - byte/word addressing -- non-VGA modes always use byte addressing
    CRTC_RD(NV_CIO_CR_MODE_INDEX, val08, Head);
    CRTC_WR(NV_CIO_CR_MODE_INDEX, val08 | BIT(7) | BIT(6) | BIT(1) | BIT(0), Head);

    //
    // CR18 (Line Compare Register)
    //  Bits 7:0 of the line compare value
    //  This is useful only when we want to split the screen. We just program this to 0x3ff
    //
    CRTC_WR(NV_CIO_CR_LCOMP_INDEX, (U008)(LC & 0xFF), Head);

    //
    // CR19 (Repaint control register 0)
    //  [7:5] Bits 10:8 of CRTC Offset register (CR13)
    //
    CRTC_RD(NV_CIO_CRE_RPC0_INDEX, val08, Head);
    val08 = val08 & 0x1F;
    CRTC_WR(NV_CIO_CRE_RPC0_INDEX, (U008)(((logicalwidth & 0x700) >> 8) << 5) | val08, Head);

    //
    // CR1a (Repaint control register 1)
    //  [2] large screen bit. 
    //  NOTE: The manual says bit 2 should be set to 1 for modes < 1280x1024 and 
    //        0 for modes >= 1280x1024. However, that is not correct according to Peter Tong.
    //        It should be set to 0 for all non-vga modes (CR28).
    //          Safer to follow the original meaning of this bit.
    //  The rest of the bits are constant for all modes.
    //
    CRTC_RD(NV_CIO_CRE_RPC1_INDEX, val08, Head);        // grab the current sync status
    val08 = val08 & 0xC0;
    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, ((VerticalVisible < 1024) && (HorizontalVisible < 1280)) ? (0x3c | val08) : (0x38 | val08), Head);

    //
    // CR1c (Enhancement Register)
    //  This is a constant 0x1c for us.
    if (IsNV11(pDev) || IsNV1A(pDev)) {
        CRTC_WR(NV_CIO_CRE_ENH_INDEX, 0x3c, Head);
    } else {
        CRTC_WR(NV_CIO_CRE_ENH_INDEX, 0x1c, Head);
    }

    // Cr25 (Overflow register)
    //  [5:5] Bit 11 of CRTC offset register (this is 0)
    //  [4:4] Bit 6 of Horizontal Blanking End 
    //  [3:3] Bit 10 of Vertical Blank Start
    //  [2:2] Bit 10 of Vertical Retrace Start
    //  [1:1] Bit 10 of Vertical Display End
    //  [0:0] Bit 10 of Vertical Total
    CRTC_WR(NV_CIO_CRE_LSR_INDEX, ((((HorizontalBlankEnd >> 3) - 1) & BIT(6)) ? BIT(4) : 0x00)
                                | (((VerticalBlankStart - 1) & BIT(10))       ? BIT(3) : 0x00)
                                | ((VerticalRetraceStart & BIT(10))           ? BIT(2) : 0x00)
                                | (((VerticalVisible - 1) & BIT(10))          ? BIT(1) : 0x00) 
                                | (((VerticalTotal - 2) & BIT(10))            ? BIT(0) : 0x00), Head);

    // 
    // CR28 (Pixel Format Register)
    //  [1:0] Pixel depth encoded as:
    //        0 - VGA
    //        1 - 8bpp
    //        2 - 16bpp
    //        3 - 32bpp

    // Also set PRAMDAC_GENERAL_CONTROL register. ALT_MODE field depends on the pixel depth. The rest
    // of the fields are the same for all pixel depths.
    
    ramDacControl = DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIXMIX,      _ON) |
                    DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _VGA_STATE,   _SEL) |
                    DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BLK_PEDSTL,  _OFF) |
                    DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _TERMINATION, _37OHM) |
                    DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BPC,         _8BITS) |
                    DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _DAC_SLEEP,   _DIS) |
                    DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PALETTE_CLK, _EN);
        
    switch (PixelDepth)
    {
        case 8:
            ramDacControl |= DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _ALT_MODE, _NOTSEL);
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, NV_CIO_CRE_PIXEL_FORMAT_8BPP, Head);    
            break;                
        
        case 15:
            ramDacControl |= DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _ALT_MODE, _15);
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, NV_CIO_CRE_PIXEL_FORMAT_16BPP, Head);    
            break;                
        
        case 16:
            ramDacControl |= DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _ALT_MODE, _16);
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, NV_CIO_CRE_PIXEL_FORMAT_16BPP, Head);    
            break;                
        
        case 24:
            ramDacControl |= DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _ALT_MODE, _24);
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, NV_CIO_CRE_PIXEL_FORMAT_32BPP, Head);    
            break;                
        
        case 32:        
            ramDacControl |= DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _ALT_MODE, _30);
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, NV_CIO_CRE_PIXEL_FORMAT_32BPP, Head);    
            break;                
    } // switch on pixel depth       

    DAC_REG_WR32(NV_PRAMDAC_GENERAL_CONTROL, ramDacControl, Head);

    // 
    // cr2D (Horizontal Extra Bits Register)
    //  [5] Bit 23 of start address field (This is 0 for us)
    //  [4] Bit 8 of interlace half field start (This is 0 for us)
    //  [3] Bit 8 of Horizontal Retrace Start
    //  [2] Bit 8 of Horizontal Blank Start
    //  [1] Bit 8 of Horizontal Display End
    //  [0] Bit 8 of Horizontal Total
    CRTC_WR(NV_CIO_CRE_HEB__INDEX, ((((HorizontalRetraceStart >> 3) + 1) & BIT(8)) ? BIT(3) : 0x00)
                                 | ((((HorizontalBlankStart >> 3) - 1) & BIT(8))   ? BIT(2) : 0x00)
                                 | ((((HorizontalVisible >> 3) - 1) & BIT(8)) ? BIT(1) : 0x00) 
                                 | ((((HorizontalTotal >> 3) - 5) & BIT(8))  ? BIT(0) : 0x00), Head);

    // 0x3c2 (Misc output register)
    // Program the sync polarities.
    // [7] Vertical Sync Polarity:   0 - active High, 1 - active low 
    // [6] Horizontal Sync Polarity: 0 - active High, 1 - active low 
    val08 = (REG_RD08(NV_PRMVIO_MISC__READ) & 0x3f); // just take bits 5:0
    val08 |= (HorizontalSyncPolarity) ? BIT(6) : 0;
    val08 |= (VerticalSyncPolarity)   ? BIT(7) : 0;
    REG_WR08(NV_PRMVIO_MISC__WRITE, val08);

} // end of dacProgramCustomCRTC


// This is the entry point for non-multi-head-aware modeset methods.
RM_STATUS dacSetMode
(
    PHWINFO pDev,
    U032 head
)
{
    RM_STATUS rmStatus = RM_ERROR;

    if (pDev->Dac.CrtcInfo[head].pVidLutCurDac)
    {
        rmStatus = dacSetModeMulti(pDev, (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[head].pVidLutCurDac);
    }

    return rmStatus;
}

/***************************** exported functions **********************************/

// Dac HSYNC & VSYNC control
//
// The Mac needs to be able to individually control these for its power management
//

// XXX couldn't find a #define for these 2 bits...
#define HSYNCBIT   BIT(7)
#define VSYNCBIT   BIT(6)

RM_STATUS dacGetSyncState(
    PHWINFO pDev,
    U032    Head,
    U032   *pHsync,
    U032   *pVsync
)
{
    U008 data08;
    U008 lock;
    
    lock = UnlockCRTC(pDev, Head);
    
    if(GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL)
    {
        if(dacIsFlatPanelOn(pDev,Head))
        {
            //if there was a bit 4 present, it implies *enabled*
            *pHsync=1;
            *pVsync=1;
        }
        else
        {
            *pHsync=0;
            *pVsync=0;
        }
    }
    else
    {
        CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
        *pHsync = ! (data08 & HSYNCBIT);
        *pVsync = ! (data08 & VSYNCBIT);
    }
    
    RestoreLock(pDev, Head, lock);
        
    return RM_OK;
}

RM_STATUS dacSetSyncState(
    PHWINFO pDev,
    U032    Head,
    U032    Hsync,
    U032    Vsync
)
{
    U008 data08;
    U008 lock;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    int  isFp = 0;
    
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: SetSyncState on Head", Head);
    
    if ( !Hsync || !Vsync)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Shutoff of sync requested.");
    }
    
    lock = UnlockCRTC(pDev, Head);
    
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    if (pVidLutCurDac && (pVidLutCurDac->DisplayType == DISPLAY_TYPE_FLAT_PANEL))
        isFp = 1;

#ifdef MACOS
    // Dac.CrtcInfo[Head].pVidLutCurDac is not a safe lookup on the mac, because
    // no modeset has been done to place a valid pVidLutCurDac into the pDev structure.
    // Info in HAL is correct at this point.
    if ( ! pVidLutCurDac && (GETDISPLAYTYPE(pDev, Head)==DISPLAY_TYPE_FLAT_PANEL))
        isFp = 1;
#endif

    if (isFp)
    {
        if((!Hsync)||(!Vsync))
        {
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: Specifically the flatpanel\n");
            
            //power off flat panel
            dacPowerOffMobilePanel(pDev,Head);
            
            //kill PWM
            dacZeroPWMController(pDev);
        }
        else
        {            
            //power on the flat panel
            dacPowerOnMobilePanel(pDev,Head);
            
            //restore PWM
            dacSetPWMController(pDev);
        }
    }
    else
    {
        // read current value
        CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
        
        // clear both
        data08 &= ~(HSYNCBIT | VSYNCBIT);

        // set to 1 if no sync
        if ( ! Hsync)
            data08 |= HSYNCBIT;
        if ( ! Vsync)
            data08 |= VSYNCBIT;
        
        CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08, Head);
    }
    
    RestoreLock(pDev, Head, lock);
    
    return RM_OK;
}
#undef HSYNCBIT
#undef VSYNCBIT


RM_STATUS dacSetSpanning(
    PHWINFO pDev,
    U032    data
)
{
    U008 lock, crtcdata;
    //Update a local flag
    pDev->Dac.DeviceProperties.Spanning = data;

    //Also update a BIOS scratch bit
    lock = UnlockCRTC(pDev, 0);

    CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, crtcdata, 0);
    crtcdata &= ~(NV_SPANNING_MODE);
    if (data)
        crtcdata |= NV_SPANNING_MODE;
    CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, crtcdata, 0);

    RestoreLock(pDev, 0, lock);

    return RM_OK;
}

//
// Perform any necessary system synchronization that is required at the start
// of a display change.
//
RM_STATUS dacSetModeStart(
    PHWINFO pDev,
    U032    data
)
{
#define TOSHIBA_FP          (0x1 << 0)
#define TOSHIBA_CRT         (0x1 << 1)
#define TOSHIBA_TV          (0x1 << 2)
    U032    DevToBIOSDev[] = {TOSHIBA_CRT, TOSHIBA_TV, 0, TOSHIBA_FP};
    U032    NewDevices;
    U032    OldDevices;

    U008 lock, crtcdata;
    U032 mscounter = 0;

    // check if this brackets a modeset (in the case of win9x, some don't).
    if ((data & NV_CFGEX_DISPLAY_CHANGE_BRACKETS_MODESET) == 0)
        return RM_OK;

    // If there is a change in process (someone called start a second time)
    // get out of here.  This is currently expected behavior -- more than
    // one start, but only one end.
    //
    if (pDev->Dac.DisplayChangeInProgress)
        return (RM_OK);

    //If we get here in a low power state jump out
    if (pDev->Power.State != 0)
    {
        return RM_OK;
    }

    // clear the DISPLAY_CHANGE_BRACKETS_MODESET flag
    data &= (DAC_DD_MASK_IN_MAP | DAC_TV_MASK_IN_MAP | DAC_CRT_MASK_IN_MAP);

    //
    // Determine the device(s) we're switching away from
    //
    OldDevices = 0x0;
    if (pDev->Dac.CrtcInfo[0].pVidLutCurDac && (GETDISPLAYTYPE(pDev, 0) != DISPLAY_TYPE_NONE))
        OldDevices |= DevToBIOSDev[GETDISPLAYTYPE(pDev, 0)];
    if (pDev->Dac.CrtcInfo[1].pVidLutCurDac && (GETDISPLAYTYPE(pDev, 1) != DISPLAY_TYPE_NONE))
        OldDevices |= DevToBIOSDev[GETDISPLAYTYPE(pDev, 1)];
    //
    // If we couldn't determine the device being switched away from, use the tracked value.
    // Win2K has this nasty habit of destroying all the Dac objects before calling the dispaly change start
    // so we never get the current device right so, to safeguard against this, if the OldDevices
    // is still set to none at this point, we'll just use the last device state that was tracked.
    //
    if(!OldDevices)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: dacSetModeStart - Determining OldDevices using fallback method.\n");
        if (pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType != DISPLAY_TYPE_NONE)
            OldDevices |= DevToBIOSDev[pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType];
        if (pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType != DISPLAY_TYPE_NONE)
            OldDevices |= DevToBIOSDev[pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType];
    }

    NewDevices = 0x0;
    if (data & DAC_CRT_MASK_IN_MAP)
        NewDevices |= TOSHIBA_CRT;
    if (data & DAC_TV_MASK_IN_MAP)
        NewDevices |= TOSHIBA_TV;
    if (data & DAC_DD_MASK_IN_MAP)
        NewDevices |= TOSHIBA_FP;

    //
    // ONLY DO THIS ON HEAD0
    //

    lock = ReadCRTCLock(pDev, 0);           //Back up lock state
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);    //Unlock CRTC extended regs

    // Notify BIOS we're starting a display driver change.
    CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, crtcdata, 0);
    CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, crtcdata | NV_MOBILE_FLAG_DISPLAY_CHANGING, 0);

    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, 0); // Restore lock state

    //
    // Save change data for use on the final notification
    //
    // This flag also acts as notification that we are in process of a display change
    //
//    pDev->Dac.DisplayChangeInProgress = data;
    pDev->Dac.DisplayChangeInProgress = (NewDevices << 8) | OldDevices;

    // 
    // Notify the system if required
    //
    dacMobileDisplayNotifyBegin(pDev, OldDevices, NewDevices);

    // This delay is here because the SBIOS does not seem to be setting the flag (checked below).
    // We exit the do/while in the first try. We expect that a fix in the SBIOS will make this delay unnecessary.
    osDelay(100); // wait 100 ms
    
    //Wait for SBIOS to finish.
    // Something in the system seems to relock them on us from time to time, so unlock every loop
    lock = UnlockCRTC(pDev, 0);
    do {
        UnlockCRTC(pDev, 0);
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, crtcdata, 0);
        tmrDelay(pDev, 1000000);    //delay 1ms
        mscounter++;
    } while ((crtcdata & NV_MOBILE_FLAG_SBIOS_CRIT_SEC) && (mscounter < 1000));
    RestoreLock(pDev, 0, lock);

    if (mscounter >= 1000) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: timeout while waiting for SBIOS (CR4A[1]).\r\n");
    }

    return RM_OK;
}

//
// Perform any necessary system synchronization that is required at the end
// of a display change.
//
RM_STATUS dacSetModeEnd(
    PHWINFO pDev,
    U032    data
)
{
    U008 lock, crtcdata;
    U032 mscounter = 0;

    // check if this brackets a modeset (in the case of win9x, some don't).
    if ((data & NV_CFGEX_DISPLAY_CHANGE_BRACKETS_MODESET) == 0)
        return RM_OK;    // if this doesn't bracked the modeset, return

    //
    // If there is not a change in process (someone called end without calling start)
    // get out of here.
    //
    if (!pDev->Dac.DisplayChangeInProgress)
        return (RM_OK);

    //If we get here in a low power state jump out
    if (pDev->Power.State != 0)
    {
        return RM_OK;
    }

    // clear the DISPLAY_CHANGE_BRACKETS_MODESET flag
    data &= (DAC_DD_MASK_IN_MAP | DAC_TV_MASK_IN_MAP | DAC_CRT_MASK_IN_MAP);

    // 
    // Notify the system if required
    //
    dacMobileDisplayNotifyEnd(pDev, pDev->Dac.DisplayChangeInProgress & 0xFF, (pDev->Dac.DisplayChangeInProgress >> 8) & 0xFF);

    //
    // ONLY DO THIS ON HEAD0
    //

    //Wait for SBIOS to finish.
    // Something in the system seems to relock them on us from time to time, so unlock every loop
    // Save current lock state for restore later
    lock = UnlockCRTC(pDev, 0);
    do {
        UnlockCRTC(pDev, 0);
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, crtcdata, 0);
        tmrDelay(pDev, 1000000);    //delay 1ms
        mscounter++;
    } while ((crtcdata & NV_MOBILE_FLAG_SBIOS_CRIT_SEC) && (mscounter < 1000));

    if (mscounter >= 1000) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: timeout while waiting for SBIOS (CR4A[1]).\r\n");
    }

    // Make sure the CRTC registers are still unlocked
    UnlockCRTC(pDev, 0);

    // Notify BIOS we're done with the display driver change.
    CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, crtcdata, 0);
    crtcdata &= ~(NV_MOBILE_FLAG_DISPLAY_CHANGING);
    CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, crtcdata, 0);

    // Restore the original locking state that was saved off earlier.
    RestoreLock(pDev, 0, lock);

    //Mobile specific: keep around knowledge of that the secondary (non-LCD) display
    // was set to.  Used when returning to Windows to enable TwinView modes.
    // The logic may seem unnecessarily careful, but we've seen problems with
    // the CR registers before due to BIOS problems, so I'm trying to handle
    // possible cases like the same display being set as enabled on more than
    // one head, etc., gracefully.
    if (pDev->Power.MobileOperation) {
        U032 extDevice = DISPLAY_TYPE_NONE;
#if 1
        //We know that if there's more than one device, one is the LCD, and there
        // are no other flat panels.  But I'll code the general case on general principle.
        if (data & (data - 1)) {  //if more than one display is enabled
            if (data & DAC_DD_MASK_IN_MAP) extDevice = DISPLAY_TYPE_FLAT_PANEL; //should always be true in mobile -- lowest priority
            if (data & DAC_TV_MASK_IN_MAP) extDevice = DISPLAY_TYPE_TV;         //higher priority
            if (data & DAC_CRT_MASK_IN_MAP) extDevice = DISPLAY_TYPE_MONITOR;   //highest priority
        } else {
            U008 lock, data08;
        //In NT, we can't count on destruction of the LUT objects as an indicator
        // that displays have been turned off -- both LUT objects are always around.
        // So instead, we clear the CR3B registers here.
        //This would be an acceptable solution on 9x as well, except that at the
        // moment the call to this function occurs before the actual modesets...
        // 6/8/01 (LPL) : have now enabled this for 9x as well.  This call occurs
        // after the modesets now, and we need it in order to straighten out the
        // scratch registers at startup if the display driver opts for a display
        // other than what the VBIOS had picked (in this case we wouldn't otherwise
        // clear CR3B for the other display, since there is no LUT object to destroy).
        // Fixes bug 34209.
            if (data & DAC_DD_MASK_IN_MAP) {
                lock = UnlockCRTC(pDev, 0);
                CRTC_RD(0x3B, data08, 0);
                CRTC_WR(0x3B, (data08 & 0x0F), 0);
                RestoreLock(pDev, 0, lock);
            } else {
                lock = UnlockCRTC(pDev, 1);
                CRTC_RD(0x3B, data08, 1);
                CRTC_WR(0x3B, (data08 & 0x0F), 1);
                RestoreLock(pDev, 1, lock);
            }
            extDevice = DISPLAY_TYPE_NONE;         //don't set any secondary devices
        }
#else   //#if 1
        //Read directly from the CR registers.  Problem: displaychangeend occurs _before_ the modesets.
        extDevice = DISPLAY_TYPE_NONE;  //default, if only one display is enabled
        if ((cr3BhA & 0xF0) && (cr3BhB & 0xF0)) {   //If two displays are enabled.
            //Check for displays other than the internal LCD
            if (cr3BhB & 0xE0) extDevice = cr3BhB;
            if (cr3BhA & 0xE0) extDevice = cr3BhA;  //head A trumps B
        }

        //Now translate into a display type
        if (extDevice & 0x20) {
            extDevice = DISPLAY_TYPE_MONITOR;
        } else if (extDevice & 0x40) {
            extDevice = DISPLAY_TYPE_TV;
        } else if (extDevice & 0x80) {
            extDevice = DISPLAY_TYPE_FLAT_PANEL; //external DFP
        } else {
            extDevice = DISPLAY_TYPE_NONE;
        }
#endif  //#if 1... else...

        osWriteRegistryDword(pDev, strDevNodeRM, strLastExtDevice, extDevice);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: external dev:", extDevice);

        osWriteRegistryDword(pDev, strDevNodeRM, strCurWinDevices, data);
    }

    //
    // Complete the display change -- clear the flag
    //
    pDev->Dac.DisplayChangeInProgress = 0;

    return RM_OK;
}


RM_STATUS dacSetModeStartEx(
    PHWINFO pDev,
    U032    devices,
    U032    properties
)
{
    RM_STATUS status;
    //Note: even if we add other operations here, call dacSetModeStart _first_ --
    // it sets a flag that signals a display change in progress.
    status = dacSetModeStart(pDev, devices);

#ifdef NTRM
    //Currently, the WinNT implementation supports the 'properties' field,
    // and 9x does not.  Get display driver people to unite on a single approach
    // soon!  The 9x display driver doesn't easily have access to this info
    // at display change time, so there is now a separate ConfigSet (NV_CFG_DISPLAY_CHANGE_CONFIG)
    // which can be used.
    status = dacSetSpanning(pDev, (properties & NV_CFGEX_DISPLAY_CHANGE_SPANNING)?1:0);
#endif

    return status;
}

RM_STATUS dacSetModeEndEx(
    PHWINFO pDev,
    U032    devices,
    U032    properties
)
{
    //Note: even if we add other operations here, call dacSetModeEnd _last_ --
    // it clears a flag that signals a display change in progress.
    return dacSetModeEnd(pDev, devices);
}

//
// Program the dac video parameters
//
//  Note that the video parameters have already been deposited in pDev.  This allows us to go
//  through multiple load/unload pairs (like DOS boxes) while still maintaining the original
//  video mode parameters.  It also makes it easier for an applet (like the control panel)
//  to tweak one value, then force an unload/load to make it affective.
//
RM_STATUS dacSetModeMulti(
    PHWINFO pDev,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac
)
{
    RM_STATUS status = RM_OK;
    U016      data,save_sr1 = 0;
    U032      data32;
    U008      shadow,data08, i, cr3Bmod, savedCR3D;
    volatile U032 scratch;
    
    // Extract parameters from the VIDEO_LUT_CURSOR_DAC object.
    U032      HorizontalVisible         = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    U032      HorizontalBlankStart      = pVidLutCurDac->HalObject.Dac[0].HorizontalBlankStart;
    U032      HorizontalRetraceStart    = pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart;
    U032      HorizontalRetraceEnd      = (pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart +
                                           pVidLutCurDac->HalObject.Dac[0].HorizontalSyncWidth);
    U032      HorizontalBlankEnd        = (pVidLutCurDac->HalObject.Dac[0].HorizontalBlankStart +
                                           pVidLutCurDac->HalObject.Dac[0].HorizontalBlankWidth);
    U032      HorizontalTotal           = pVidLutCurDac->HalObject.Dac[0].TotalWidth;
    U032      VerticalVisible           = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;
    U032      VerticalBlankStart        = pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart;
    U032      VerticalRetraceStart      = pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart;
    U032      VerticalRetraceEnd        = (pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart +
                                           pVidLutCurDac->HalObject.Dac[0].VerticalSyncHeight);
    U032      VerticalBlankEnd          = (pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart +
                                           pVidLutCurDac->HalObject.Dac[0].VerticalBlankHeight);
    U032      VerticalTotal             = pVidLutCurDac->HalObject.Dac[0].TotalHeight;
    U032      PixelDepth                = pVidLutCurDac->HalObject.Dac[0].PixelDepth;
    U032      PixelClock                = pVidLutCurDac->HalObject.Dac[0].PixelClock;
    U032      HorizontalSyncPolarity    = pVidLutCurDac->HalObject.Dac[0].Format.HorizontalSyncPolarity;
    U032      VerticalSyncPolarity      = pVidLutCurDac->HalObject.Dac[0].Format.VerticalSyncPolarity;
    U032      DoubleScannedMode         = pVidLutCurDac->HalObject.Dac[0].Format.DoubleScanMode;
    U032      Head                      = pVidLutCurDac->HalObject.Head;
    
    //If we get here in a low power state jump out
    if (pDev->Power.State != 0)
    {
        return RM_OK;
    }
    //
    // Validate this mode is valid for the amount of framebuffer present
    // in this specific device.
    // Watch out for pixel depth of 15; add 1 to ensure it divides by 8 and give correct #bytes
    if ((HorizontalVisible * VerticalVisible * ((PixelDepth+1) >> 3)) > 
        pDev->Framebuffer.HalInfo.RamSize)
            return RM_ERROR;
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: dacSetModeMulti,Enter\r\n");

    //
    // There is a bug in NV hardware where the hardware cursor will not clip correctly
    // if a border is defined in the display timings (blank start != visible end).  
    // Borders are only defined in two specific DMT modes.
    //
    // To fix this, set the blank starts to match the visible totals and remove the 
    // implicit borders.
    //
    // Remove this code when the hardware gets fixed.  Since this bug has been in the 
    // hardware since 1994, don't expect a fix soon.
    //
    HorizontalBlankStart = HorizontalVisible;
    HorizontalBlankEnd   = HorizontalTotal;
    VerticalBlankStart   = VerticalVisible;
    VerticalBlankEnd     = VerticalTotal;

    //
    // Now set monitor type.
    //
    switch (pVidLutCurDac->DisplayType)
    {
        case DISPLAY_TYPE_MONITOR:
            SETMONITORTYPE(pDev, Head, MONITOR_TYPE_VGA);
            break;
        case DISPLAY_TYPE_TV:
            switch (pVidLutCurDac->TVStandard)
            {
                case NTSC_M:
                case NTSC_J:
                    SETMONITORTYPE(pDev, Head, MONITOR_TYPE_NTSC);
                    break;
                case PAL_A:
                case PAL_M:
                case PAL_N:
                case PAL_NC:
                    SETMONITORTYPE(pDev, Head, MONITOR_TYPE_PAL);
                    break;
            }
            //
            // Set TV standard.
            //
            pDev->Dac.TVStandard = pVidLutCurDac->TVStandard;

            // Also set it in the cached copy that's used to refresh Macrovision settings.
            pDev->Dac.TVLastMacrovision.tvStandard = pVidLutCurDac->TVStandard;

            break;
        case DISPLAY_TYPE_DUALSURFACE:
            break;
        case DISPLAY_TYPE_FLAT_PANEL:        
            SETMONITORTYPE(pDev, Head, MONITOR_TYPE_FLAT_PANEL);

            if ( IsNV11(pDev) && pDev->Power.MobileOperation && (pDev->Power.MobileOperation != 2) )
            {
                if (dacGetFlatPanelConfig(pDev, Head) == NV_CFGEX_GET_FLATPANEL_INFO_SCALED)
                {
                    if ((HorizontalVisible <= 800) && (DAC_REG_RD32(NV_PRAMDAC_FP_HCRTC, Head) >= 1400))
                    {
                        // PCLK will be lowered
                        // If the PCLK isn't lowered now, we need to turn off the panel to prevent blooming
                        // If we ever change the PCLK, we need to power down the panel and run the reset script.
                        // This fixes bug 32763.
                        if(!(pDev->Dac.DevicesEnabled & DAC_LOW_PCLK_ENABLED))
                            dacPowerOffMobilePanel(pDev, Head);
                    }
                    else
                    {
                        // PCLK will be normal.
                        // If the PCLK is currently lowered, the we need to turn off the panel to prevent blooming
                        // If we ever change the PCLK, we need to power down the panel and run the reset script.
                        // This fixes bug 32763.
                        if(pDev->Dac.DevicesEnabled & DAC_LOW_PCLK_ENABLED)
                            dacPowerOffMobilePanel(pDev, Head);
                    }
                }
            }

            break;
        default:
            break;
    }
    // Work-around for a NV11 HW bug:
    // Find out what the current display is (do this before osPreModeSet sets new display type).
    // If it is TV, and this is NV11, call dacDisableTV to be sure PCLK doesn't get stuck when we change the
    // clock source. dacDisableTV checks for a stalled PCLK and kick-starts it.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)    
    {
        if (IsNV11(pDev)) dacDisableTV(pDev, Head);
    }
    //
    // We are about to set the new mode. Do any os specific actions necessary at this point.
    // This includes any registry overrides that may affect the new mode.
    // Specifically, determine what type of display device (CRT, FP, or TV), and if TV,
    // what format, and if FP, parse EDID to get timings.
    // NOTE: at this point, the new DisplayType is in the VidLutCurDac object, the old DisplayType is in HalInfo.CrtcInfo
    // (which GETDISPLAYTYPE and SETDISPLAYTYPE access).
    osPreModeSet(pDev, Head);

    //
    // Set display type.  We can't rely on the caller to actually
    // have specified an allowable valid type.  As one last check,
    // consult the allowed mask.  Don't reset the DisplayType field,
    // but continue on with the modeset just to be safe...
    //
    if ((GETCRTCHALINFO(pDev, Head, DisplayTypesAllowed) & DISPLAYTYPEBIT(pVidLutCurDac->DisplayType)) != 0)
        SETDISPLAYTYPE(pDev, Head, pVidLutCurDac->DisplayType);

    // 
    // Update the other Head's PrimaryDevice state, so it's consistent with this
    // Head's PrimaryDevice state (opposite head = opposite PrimaryDevice value).
    // 
    pDev->Dac.CrtcInfo[Head^1].PrimaryDevice = pDev->Dac.CrtcInfo[Head].PrimaryDevice^1;
    
    DBG_PRINT_STRING (DEBUGLEVEL_TRACEINFO, "Setting mode \n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "    H Res:", pDev->Framebuffer.HalInfo.HorizDisplayWidth);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "    V Res:", pDev->Framebuffer.HalInfo.VertDisplayWidth);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "Bit Depth:", PixelDepth);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "  Refresh:", pDev->Framebuffer.HalInfo.RefreshRate);

    // Set the requested refresh rate
    pVidLutCurDac->RefreshRate = (PixelClock * 10000) /
                                 (HorizontalTotal * VerticalTotal);

    //
    // Update tiling configuration.
    //
    nvHalFbControl(pDev, FB_CONTROL_TILING_CONFIGURE);

    //
    // Check for a change to nvclk/mclk values.
    //
    // Since we don't go through a STATE_LOAD as part of a mode switch, this is
    // where we'll program these clocks. The values are only interesting to the
    // arbitration code below, so anytime before that should be good.
    //
    // Since we also haven't done a STATE_UNLOAD, we're assuming this SW method
    // in the host FIFO has allowed enough of the chip to idle, so we can change
    // the clocks. In limited testing, this seems to work.
    //
    if (pDev->Dac.UpdateFlags & UPDATE_HWINFO_DAC_NVPLL)
    {
        // Program NVCLK
        status = dacProgramNVClk(pDev);
        pDev->Dac.UpdateFlags &= ~UPDATE_HWINFO_DAC_NVPLL;
    }

    // 
    // Transfer any update flags (i.e. mclk programming request) to pDev.
    //
    if (pDev->halHwInfo.pDacHalInfo->UpdateFlags & NV_HAL_DAC_UPDATE_MPLL)
    {
        pDev->Dac.UpdateFlags |= UPDATE_HWINFO_DAC_MPLL;
        pDev->halHwInfo.pDacHalInfo->UpdateFlags &= ~NV_HAL_DAC_UPDATE_MPLL;
    }

    if (pDev->Dac.UpdateFlags & UPDATE_HWINFO_DAC_MPLL)
    {
        // Program MCLK
        status = dacProgramMClk(pDev, 0);
        pDev->Dac.UpdateFlags &= ~UPDATE_HWINFO_DAC_MPLL;
    }



    //
    // Enable full CRTC access
    //           
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, 0x57, Head);

    //
    // If this is for CRTC2, set CRTC access bit and PRAMDAC address
    // This access bit controls accesses to Sequencer and Graphics Controller (C03xx).
    // CRTC and Attribute Controller must be accessed at different addresses for each head
    // (6103xx/6033xx), since there is only one bus.
    //
    if (IsNV11(pDev))
    {
        //
        // If there hasn't been a modeset on the other head, turn off the sequencer
        // there allowing more FB bandwidth for this head. When a modeset does occur,
        // dacProgramSeq will reset SR1 back. 
        //
        if (pDev->Dac.CrtcInfo[Head^1].pVidLutCurDac == NULL)
        {
            EnableHead(pDev, Head^1);
            REG_WR08(NV_PRMVIO_VSE2, 1);                    // enable VGA subsystem
            data08 = REG_RD08(NV_PRMVIO_MISC__READ);
            REG_WR08(NV_PRMVIO_MISC__WRITE, data08 | 1);    // enable CGA I/O space
        }

        // point CR44 at this head, for the rest of the modeset
        EnableHead(pDev, Head);
        if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
        {
            AssocTV(pDev, Head);
            AssocDDC(pDev, Head);
        }

        REG_WR08(NV_PRMVIO_VSE2, 1);                    // enable VGA subsystem
        data08 = REG_RD08(NV_PRMVIO_MISC__READ);
        REG_WR08(NV_PRMVIO_MISC__WRITE, data08 | 1);    // enable CGA I/O space
    }
        
    // Switching from FP or TV to CRT (CRTC slave mode) can stall the scanline counter. 
    dacDisableCRTCSlave(pDev, Head);

    // Disable flat panel syncs
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _DISABLE, Head); // 
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _DISABLE, Head); // 

    CRTC_RD(NV_CIO_CR_VRE_INDEX, data08, Head);
    CRTC_WR(NV_CIO_CR_VRE_INDEX, data08 & ~0x80, Head);    // unlock CR0-7
    
    // Before unlocking / reading CR21 we should write CR3D[0] to 1 to enable reading of
    // the shadow registers.  This will allow us to read the real CR register values from
    // the locked registers.
    CRTC_RD(0x3D, savedCR3D, Head);     // read CR3D
    savedCR3D |= 0x1;                   // set CR3D[0] to 1
    CRTC_WR(0x3D, savedCR3D, Head);     // write back CR3D[0] = 1

    CRTC_RD(0x21, shadow, Head);        // read shadow lock
    shadow |= 0xfa;                     // enable HDE, VDE, and 8/9 dot chars too, for NV5.

    // Before programming the shadow registers you must set CR3D[0] to 0
    savedCR3D &= ~0x1;                  // set CR3D[0] to 0
    CRTC_WR(0x3D, savedCR3D, Head);     // write back CR3D[0] = 0

    CRTC_WR(0x21, shadow, Head);        // write back shadow

    // Set H and V retrace start to a large value to prevent possible lock-up.
    // This is because if the CRTC is slaved to an external device (the TV encoder),
    // it will stall waiting for retrace start to sync up with the external clock. 
    // When we switch from TV to CRT, we may get stuck in this stall. Setting retrace 
    // start to a large value guarantees this won't happen.
    CRTC_WR(NV_CIO_CR_HRS_INDEX, 0xff, Head);
    CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xff, Head);

    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL)
    {
        U008 i;

        // bug workaround--disable scanline doubling and then shadow it. This allows writes to CR09 to affect the CRTC,
        //   but not the flat panel controller (else the flat panel controller may quadruple the scanline count).
        CRTC_RD(NV_CIO_CR_CELL_HT_INDEX, data, Head);         // read scanline double
        CRTC_WR(NV_CIO_CR_CELL_HT_INDEX, (data &= ~0x80), Head);   // clear scanline double bit

        // Wait one vsync before shadowing the scanline double bit
        // Use a time-out in case the bit is stuck.
        for (i=0; i<17; i++)    // max time it can take is 16.7 ms (actually less)
        {
            if (DAC_REG_RD32(NV_PCRTC_RASTER, Head) & DRF_DEF(_PCRTC, _RASTER, _VERT_BLANK, _ACTIVE))
                break;      
            tmrDelay(pDev, 1000000);    // wait 1 ms
        }
        for (i=0; i<17; i++)    // max time it can take is 16.7 ms
        {
            if (!(DAC_REG_RD32(NV_PCRTC_RASTER, Head) & DRF_DEF(_PCRTC, _RASTER, _VERT_BLANK, _ACTIVE)))
                break;
            tmrDelay(pDev, 1000000);    // wait 1 ms
        }
    
        // shadow = CR21
        shadow &= ~0x40;                // shadow the scanline double bit
        CRTC_WR(0x21, shadow, Head);    // allow horiz & vert write-thru

        // de-couple vertical sync from flat panel while setting mode
        data32 = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_0, Head); 
        data32 |= (NV_PRAMDAC_FP_DEBUG_0_TEST_NEWPIX << 16);   // set VCLK bits
        DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, data32, Head);

        // power off the panel until the mode is all setup
        //if (pDev->Power.MobileOperation)
        //{
        //    dacPowerOffMobilePanel(pDev, Head);
        //}
        // There's no need to turn off the panel.  We disable the visible screen so the user
        // not see anything.  This gets rid of at least 0.8 seconds in mode transitions.
    }
    else
    {
        //
        // We aren't driving DFP on this head, so we need to make sure to flush the overlay
        // clocking in the chip so it's not accidently left pointing at the DFP clocks.
        //
        // If we're not careful, the fp_newline signal inside the chip may still be driven
        // from fpclk, even though we aren't actively using fpclk.  This will result in
        // the overlay video engine not sending output data
        //
        // Here's the HW description of the bug
        //
        // This is a hardware bug that exists in NV10 and subsequent designs: 
        //  *  the video scaler relies on an a signal from the flat panel timing logic, fp_newline
        //  *  when the CRTC is not slaved to the FP, fp_newline should remain 1, however, this is 
        //     not always true
        //  *  if the FP timing has been previously programmed, and the CRTC is un-slaved from the 
        //     FP timing, fp_newline will continue to toggle as if the CRTC were still slaved to the FP.
        //  * also,  if the FPCLK is powered down at a moment when fp_newline is 0, fp_newline will 
        //    remain 0 until FPCLK is powered back up
        //  * fp_newline can be made to remain 1 by doing these steps before unslaving the CRTC from 
        //    the FP timing
        //    1.) disable vertical FP scaling
        //    2.) wait until the next frame
        //    3.) FPCLK and FP syncs must be enabled during this time (steps 1 and 2), i.e., 
        //        NV_PRAMDAC_FP_TG_CONTROL bits 1 and 5 must both be 0.
        //    4.) un-slave CRTC to FP timing
        //    5.) FPCLK and FP H- and V- syncs may now be disabled if desired
        //
        // This is supposed to be fixed on NV17 and later...
        // 
        
        if (IsNV10(pDev) || IsNV11(pDev) || IsNV1A(pDev) || IsNV15(pDev) || IsNV20(pDev))
        {
            U032 savedPowerState;

            U032 regvalue;
            U032 savemode;
            U032 savecenter;
            
            //Get current state of register
            regvalue   = DAC_REG_RD32(NV_PRAMDAC_FP_TG_CONTROL, Head); 
            savemode   = DRF_VAL(_PRAMDAC, _FP_TG_CONTROL, _MODE,   regvalue);
            savecenter = DRF_VAL(_PRAMDAC, _FP_TG_CONTROL, _CENTER, regvalue);

            // Temporarily turn on the FPCLK
            savedPowerState = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_0, Head);
            dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);

            // Set the fp to centered with valid sync's 
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _NEG, Head);
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _NEG, Head);
            
            DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_START, 1, Head);
            DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_END,   0, Head);
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _CENTER, Head);
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _CENTER, _VERT, Head);

            // need to wait for fp_vsync to be high twice.
            // this could take 4 vsync times if we start at 0.
            // Figure a vsync time is ~20ms.
            // Adding another vsync (now 5) to wait to fix bug 31102
            tmrDelay(pDev, 5 * 20 * (1000 * 1000));

            // Put it back to previous mode
            DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _MODE,   savemode,   Head);
            DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _CENTER, savecenter, Head);

            // Turn FPCLK back off again
            DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, savedPowerState, Head);
            tmrDelay(pDev, 5 * 1000 * 1000);
        }
    }

    // Program the TV encoder if any.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
    {
        status = dacSetupTVEncoder(pDev, Head); 
        if (status)
        {
            switch (status)
            {
                case ERR_I2C_MODE:
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Can't program the TV encoder for this mode.\n");
                    break;
                case ERR_I2C_COMM:
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: I2C error programming mode\n");
                    break;
                case ERR_I2C_UNKNOWN:
                    DBG_PRINT_STRING (DEBUGLEVEL_ERRORS, "NVRM: No TV Encoder detected\n");
                    break;
            }                
            if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)      // if flat panel, leave it that way.
                SETDISPLAYTYPE(pDev, Head, DISPLAY_TYPE_MONITOR);  // No encoder set up for monitor
        }
    }

    //
    // Disable the visible screen
    //
    scratch = REG_RD32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    ////ReadPriv32(NV_PRMCIO_INP0__COLOR);       // Reset ATC FlipFlop
    REG_WR08(NV_PRMCIO_ARX, 0x00);               // Turn off screen at AR
    ////WritePriv08(NV_PRMCIO_ARX, 0x00);        // Turn off screen at AR
        
    //
    // FullCPUOn()
    //
    data = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
    data |= 0x2000;                             // Full Bandwidth bit ON
    WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit

    //
    // Set the sequencer & graphics controller values that are common for all packed-pixel
    // graphics modes.
    //            
    dacProgramSeq(pDev, Head);
    //
    // Program the CRTC & SR values for this particular mode.
    //
    dacProgramCRTC(pDev, Head, pVidLutCurDac,
                         HorizontalVisible,
                         HorizontalBlankStart,
                         HorizontalRetraceStart,
                         HorizontalRetraceEnd,
                         HorizontalBlankEnd,
                         HorizontalTotal,
                         VerticalVisible,
                         VerticalBlankStart,
                         VerticalRetraceStart,
                         VerticalRetraceEnd,
                         VerticalBlankEnd,
                         VerticalTotal,
                         PixelDepth,
                         PixelClock,
                         HorizontalSyncPolarity,
                         VerticalSyncPolarity,
                         DoubleScannedMode);

    // Set the flat panel timing registers (PRAMDAC_FP).
    // We must program the DAC before we load the palette, because the DAC is driving the 
    // VGA clock.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL)
    {
        U032 fpMode, fpMaxX, fpMaxY;
        BOOL fpConnect;
        BOOL fpNative;
   
// TO DO: do not use the registry for NT
#ifndef NTRM
        status = dacGetFlatPanelInfo(pDev, Head, &fpMode, &fpMaxX, &fpMaxY, &fpConnect, &fpNative,
            ((pDev->Power.MobileOperation)?FALSE:TRUE) /* from registry */);

        //XXX: Special workaround for Toshiba. There are times when doing a modeset on 
        //a flat panel where the CR registers can become locked after we generate an
        //INT10 to our video bios, which in turn generates an INT15 to the system
        //bios. The real solution is to work with either the SBIOS or VBIOS people
        //to ensure that our lock state is being saved and restored correctly. For
        //now re-unlock the extended registers and enable vga subsystem after the
        //ReadEdid call.
        if (pDev->Power.MobileOperation == 2)
        {
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, 0x57, Head);
            EnableHead(pDev, Head);
            REG_WR08(NV_PRMVIO_VSE2, 1);                    // enable VGA subsystem
            data08 = REG_RD08(NV_PRMVIO_MISC__READ);
            REG_WR08(NV_PRMVIO_MISC__WRITE, data08 | 1);    // enable CGA I/O space
        }
#else
        status = dacGetFlatPanelInfo
        (
            pDev, 
            Head, 
            &fpMode, 
            &fpMaxX, 
            &fpMaxY, 
            &fpConnect, 
            &fpNative,
            FALSE           /* NOT from registry for NT */
        );
#endif

        // if Native mode requested, make sure this monitor supports it
        if ((fpMode == NV_CFGEX_GET_FLATPANEL_INFO_NATIVE) && (!fpNative))
            fpMode = NV_CFGEX_GET_FLATPANEL_INFO_SCALED;    // no, then do scaled
        status = dacSetFlatPanelMode(pDev, Head, pVidLutCurDac, fpMode, FALSE /* don't commit changes */);
    }
    else
    {
        //
        // If we aren't driving the panel through this head, or
        // (or the other head) turn it off.
        // 
        if (pDev->Power.MobileOperation)
        {
            if ((pDev->Dac.CrtcInfo[Head^1].pVidLutCurDac == NULL) ||
                (((PVIDEO_LUT_CURSOR_DAC_OBJECT)(pDev->Dac.CrtcInfo[Head^1].pVidLutCurDac))->DisplayType != DISPLAY_TYPE_FLAT_PANEL))
            {
                dacPowerOffMobilePanel(pDev, Head);
            }
        }
    }

    // If using an analog CRT with the external DAC, program the PRAMDAC_FP register to generate timing
    if ((GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_MONITOR) && (Head == 1))
    {
        U032 fpMode, fpMaxX, fpMaxY;
        BOOL fpConnect;
        BOOL fpNative;
   
        status = dacGetFlatPanelInfo(pDev, Head, &fpMode, &fpMaxX, &fpMaxY, &fpConnect, &fpNative, FALSE ); /* NOT from registry */
        status = dacSetFlatPanelMode(pDev, Head, pVidLutCurDac, NV_CFGEX_GET_FLATPANEL_INFO_NATIVE, FALSE);
        dacAdjustCRTCForFlatPanel(pDev, Head, pVidLutCurDac);
    }
    
    // This code is necessary to fix a bug found where the memory controller would hang if a delay
    // was NOT inserted before turning on the display after programming the pixel clock PLL's.
    // Only required for SMA mode.
    if(IsNV0A(pDev)){
        if(pDev->Framebuffer.HalInfo.RamType == BUFFER_CRUSH_UMA){
            save_sr1 = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
            data = save_sr1;
            data |= 0x2000;                             // Full Bandwidth bit ON
            WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit
        }
    }

    //
    // Program the new VPLL clock rate
    // If this is a flat panel, or flat panel engine is driving the analog monitor, we should not change the clocks
    if ((GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_FLAT_PANEL) && (Head != 1))
    {
        dacProgramPClk(pDev, Head, PixelClock);
    }

    //
    // Native modes require a divide-by-2 clock ratio
    //                                    
    if (Head == 0) 
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK_RATIO, _DB2);
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK_TV, _NONE);   // assume no TV for now
    }
    else
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK2_RATIO, _DB2);
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK2_TV, _NONE);  // assume no TV for now
    }
    pDev->Framebuffer.CursorAdjust = 0;   // default to no adjustment of cursor position
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
    {
        dacAdjustCRTCForTV(pDev, Head);              // CRTC tweaks for TV
    }
    else    
    {
        // Not TV, if flat panel, we may need to adjust CRTC's
        if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL)
            dacAdjustCRTCForFlatPanel(pDev, Head, pVidLutCurDac);
        if (Head == 0) 
        {
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V1SOURCE, _PROG);
        }
        else
        {
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V2SOURCE, _PROG);
        }
    }        
    
    //
    // Load default palette values. Do this after clock is running, else we hang.
    //
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL)
    {
        // Set NEWPIX before programming palette to prevent hang on some systems (i.e. VIA).
        // This prevents the PCLK from being held off. Stopping PCLK is done to allow the pixel
        // stream to sync with the flat panel controller, but is also causes the palette to stop 
        // responding, causing bus retries. The VIA chipset doesn't like a lot of bus retries, 
        // and may eventually hang the bus.
        data32 = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_0, Head);           
        data32 |= (NV_PRAMDAC_FP_DEBUG_0_TEST_NEWPIX << 16);  // set NEWPIX
        DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, data32, Head);
    }

#ifdef MACOS
    // We don't init the palette on either Mac os; its already been set
    //   to their own style of LUT and these defaults just cause flashing
#else
    dacLoadPalette(pDev, Head, PixelDepth);
#endif

    // Restore normal clocking (flat panel can hold off)
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL)
    {
        data32 = DAC_REG_RD32(NV_PRAMDAC_FP_DEBUG_0, Head);           
        data32 &= ~(NV_PRAMDAC_FP_DEBUG_0_TEST_BOTH << 16); // set to normal mode
        DAC_REG_WR32(NV_PRAMDAC_FP_DEBUG_0, data32, Head);
    }

    if(!IsNV0A(pDev)){
        //
        // FullCpuOff()
        //
        data = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
        data &= 0xDFFF;                             // Full Bandwidth bit OFF
        WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit            
    }else{
        if(pDev->Framebuffer.HalInfo.RamType != BUFFER_CRUSH_UMA){
            data = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
            data &= 0xDFFF;                             // Full Bandwidth bit OFF
            WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit            
        }
    }
    
    //
    // ATCOn()
    //
    scratch = REG_RD32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    ////ReadPriv32(NV_PRMCIO_INP0__COLOR);       // Reset ATC FlipFlop
    REG_WR08(NV_PRMCIO_ARX, 0x20);               // Turn off screen at AR
    ////WritePriv08(NV_PRMCIO_ARX, 0x20);        // Turn off screen at AR
    scratch = REG_RD32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    ////ReadPriv32(NV_PRMCIO_INP0__COLOR);       // Reset ATC FlipFlop
      
    // make sure test modes are off, else screen may be blanked
    DAC_REG_WR32(NV_PRAMDAC_TEST_CONTROL, 0, Head); // make sure test modes are off!

    // Enable active display, blank all non-active displays
    //  (TV is blanked in encoder-specific function).
    // + in mobile case, update BIOS's record of enabled displays
    cr3Bmod = 0;
    switch (GETDISPLAYTYPE(pDev, Head))
    {
        case DISPLAY_TYPE_MONITOR:
            if (Head == 0) // if normal VGA (internal DAC) 
            {
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON, Head);  // CRT on
                //
                // We need to be careful about powering down the TMDS
                // transmitter on NV11 because we can lose a flat
                // panel connected to the second crtc (head 1).
                //
                // XXX doing this on the P11 kills the FP on head 1.
                // commenting it out doesn't seem to break anything on the Mac....
                
#ifndef MACOS
                if (IsNV11(pDev))
                    dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_FPCLK);     // FPCLK off only;leave TMDS going for secondary crtc
                else
                    dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_BOTH);      // FPCLK/TMDS both off
#endif       
                CRTC_RD(0x33, data08, Head);
                CRTC_WR(0x33, (data08 & ~0x02), Head);  // clear CR33[1] (BIOS flag)

                // enable H&V sync's
                CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
                CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08 & 0x3F, Head);
                
                // make sure that head 0 is NOT driving FP output
                CRTC_RD(0x33, data08, Head);
                CRTC_WR(0x33, data08 & ~0x10, Head);    // Bit 4 = 0
                
                dacSetCursorBlink(pDev, Head, 0); // set cursor blink rate for DOS
            }
            else // PRAMDAC_FP is driving analog
            {
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head); // CRT off
                CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
                CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08 & ~0x40, Head);
                
                // make head 0 drive FP output
                CRTC_RD(0x33, data08, Head);
                CRTC_WR(0x33, data08 | 0x10, Head);    // Bit 4 = FP 2
                
                dacSetCursorBlink(pDev, Head, 1); // set cursor blink rate for DOS
            }

            // If no head is using TV, blank image.
            for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                if (GETDISPLAYTYPE(pDev, i) == DISPLAY_TYPE_TV)
                    break;
            if (i == pDev->Dac.HalInfo.NumCrtcs)
                dacDisableTV(pDev, Head);
            //Update BIOS's record of enabled displays

            cr3Bmod = 0x20;
            break;                              
        case DISPLAY_TYPE_TV:
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head); // CRT offs
            //
            // We need to be careful about powering down the TMDS
            // transmitter on NV11 because we can lose a flat
            // panel connected to the second crtc (head 1).  This wouldn't
            // be an issue if we could 
            //
            if (IsNV11(pDev))
                dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_FPCLK);     // FPCLK off only;leave TMDS going for secondary crtc
            else
                dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_BOTH);      // FPCLK/TMDS both off
            // Turn Hsync off and Vsync on, or leave both on, depending on what we found in registry
            CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
            data08 &= ~0xC0;
            if (pDev->Dac.HsyncOff) 
            {
                CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08 |= 0x80, Head); // disable horiz sync to put CRT in suspend
            }
            else
            {
                CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08, Head);    // leave both syncs on
            }
            dacSetCursorBlink(pDev, Head, 1); // set cursor blink rate for DOS

            //Update BIOS's record of enabled displays
            cr3Bmod = 0x40;
            break;                              
        case DISPLAY_TYPE_FLAT_PANEL:
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head); // CRT off
            // Turn Hsync off and Vsync on, or leave both on, depending on what we found in registry
            CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
            data08 &= ~0xC0;
            if (pDev->Dac.HsyncOff) 
            {
                CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08 |= 0x80, Head); // disable horiz sync to put CRT in suspend
            }
            else
            {
                CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08, Head);    // leave both syncs on
            }
            CRTC_RD(0x33, data08, Head);
            CRTC_WR(0x33, data08 | 0x10, Head);    // Bit 4 = FP 2
            // If no head is using TV, blank image.
            for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                if (GETDISPLAYTYPE(pDev, i) == DISPLAY_TYPE_TV)
                    break;
            if (i == pDev->Dac.HalInfo.NumCrtcs)
                dacDisableTV(pDev, Head);
            // Need to actually power on the panel as well
            if (pDev->Power.MobileOperation)
                dacPowerOnMobilePanel(pDev, Head);
            dacSetCursorBlink(pDev, Head, 1); // set cursor blink rate for DOS

            //Update BIOS's record of enabled displays
            if (Head == 1) {
                cr3Bmod = 0x10;    //internal LCD
            } else {
                cr3Bmod = 0x80;    //external DFP
            }
            break;                              
    }

    //Update BIOS's record of enabled displays
    if (pDev->Power.MobileOperation  && !pDev->Vga.Enabled) {
        U008 lock;

        lock = UnlockCRTC(pDev, Head);

        CRTC_RD(0x3B, data08, Head);
        CRTC_WR(0x3B, (data08 & 0x0F) | cr3Bmod, Head);

        RestoreLock(pDev, Head, lock);
    }

    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
    {
        shadow &= ~0x80;          // shadow Horiz only
        CRTC_WR(0x21, shadow, Head);    // restore the shadow lock

        // restore macrovision mode if appropriate
        if (pDev->Dac.TVLastMacrovision.mode & 0x3)
            SetMacrovisionMode(pDev, Head, (RM_MACROVISION_CONTROL *)&(pDev->Dac.TVLastMacrovision));

    }

    //Fix for Crush. TV and Flat Panel share some registers. We must
    //set the mux to pick the correct display.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL) 
    {
        if (IsNV0A(pDev)) 
        {
            data32 = 0x00000005;
            DAC_REG_WR32(NV_PRAMDAC_TV_SETUP, data32, Head); 
        }
    }    
    //
    // For NV4, change the default flip functionality to end of vsync, not start
    //
    // BUG BUG BUG:  There is a hardware bug in NV4/NV5 relating to this particular
    // register.  You must access another register in this general area before
    // accessing PCRTC_CONFIG
    //
    data32 = DAC_REG_RD32(NV_PCRTC_START, Head);
    DAC_FLD_WR_DRF_DEF(_PCRTC, _CONFIG, _START_ADDRESS, _NON_VGA, Head);
    
    //
    // Update arb settings for each mode set
    // test: if this is the modeset class' allocation, do arbitration setup. Else skip
    if(pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac==(VOID_PTR)pVidLutCurDac)
    {   
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"updating arb settings for modeset");
        nvHalDacUpdateArbSettings(pDev,(VOID*)&pVidLutCurDac->HalObject);
    }

    // Save the retrace start/end values (display position default)
    dacSaveMonitorDefaultPosition(pDev, Head);

    // We have successfully set a new mode. Do any os specific actions.
    // This includes any registry settings that should take effect at a mode set.
    osPostModeSet(pDev, Head);

    //
    // Determine if it's possible to support a video overlay in this resolution
    //

    //LPL: will later either add a new multichip function for multi-display,
    // or expand the parameter list on the existing fn.  For now, this hack
    // minimizes the changes to other files.
    nvHalDacValidateArbSettings(pDev, 
                                PixelDepth, 
                                1, 0, 
                                (PixelClock * 10000), Head,
                                &(pDev->Video.HalInfo.OverlayAllowed));


    //
    // Set ReadOnly CRTC access
    //           
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, 0x99, Head);

    if(IsNV0A(pDev)){
        if(pDev->Framebuffer.HalInfo.RamType == BUFFER_CRUSH_UMA){
            tmrDelay(pDev,/*U032 nsec*/100000000);
            save_sr1 &= 0xDFFF;                         // Full Bandwidth bit OFF
            WriteIndexed(NV_PRMVIO_SRX, save_sr1);      // Set bit            
        }
    }

    //
    // If we're on an NV11, restore VGA accesses to the primary device.
    //
    if (IsNV11(pDev))
    {
        if ((pDev->Chip.BiosRevision & 0xFF00FFFF) < 0x03000020)
        {
            // Not a multi-head aware BIOS, back to head 0
            EnableHead(pDev, 0);
        }
        else if (pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].PrimaryDevice == FALSE)
        {
            // Point VGA accesses back to the other (primary) head
            EnableHead(pDev, Head^1);
        }
    }
    
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NV_PCRTC_START : ",DAC_REG_RD32(NV_PCRTC_START,Head));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NV_PCRTC_CONFIG: ",DAC_REG_RD32(NV_PCRTC_CONFIG,Head));

    //
    // Clear out panning state.
    //
    pDev->Dac.CrtcInfo[Head].CurrentPanOffset = 0;
    pDev->Video.HalInfo.PanOffsetDeltaX[Head] = 0;
    pDev->Video.HalInfo.PanOffsetDeltaY[Head] = 0;

    //
    // Finally, make sure VBlank will be serviced
    DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, DACGETHEAD(pVidLutCurDac));
    DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_EN_0, _VBLANK, _ENABLED, DACGETHEAD(pVidLutCurDac));
    pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].StateFlags |= DAC_CRTCINFO_VBLANK_ENABLED;
    {
        volatile U032 rasterPos;
        rasterPos = REG_RD32(NV_PCRTC_RASTER + CurDacAdr(Head));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"dacSetModeMulti,Exit, raster : ",rasterPos);

    }
    return RM_OK;
} // end of dacSetModeMulti


//
// Program the dac video parameters, and JUST the dac video parameters
//
// This routine is called primarily during VGA connect testing for
// minimal clocking
//
RM_STATUS dacSetModeMini(
    PHWINFO pDev,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac
)
{
    U016      data,save_sr1 = 0;
    U008      shadow,data08,savedCR3D;
    volatile U032 scratch;
    
    // Extract parameters from the VIDEO_LUT_CURSOR_DAC object.
    U032      HorizontalVisible         = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    U032      HorizontalBlankStart      = pVidLutCurDac->HalObject.Dac[0].HorizontalBlankStart;
    U032      HorizontalRetraceStart    = pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart;
    U032      HorizontalRetraceEnd      = (pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart +
                                           pVidLutCurDac->HalObject.Dac[0].HorizontalSyncWidth);
    U032      HorizontalBlankEnd        = (pVidLutCurDac->HalObject.Dac[0].HorizontalBlankStart +
                                           pVidLutCurDac->HalObject.Dac[0].HorizontalBlankWidth);
    U032      HorizontalTotal           = pVidLutCurDac->HalObject.Dac[0].TotalWidth;
    U032      VerticalVisible           = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;
    U032      VerticalBlankStart        = pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart;
    U032      VerticalRetraceStart      = pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart;
    U032      VerticalRetraceEnd        = (pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart +
                                           pVidLutCurDac->HalObject.Dac[0].VerticalSyncHeight);
    U032      VerticalBlankEnd          = (pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart +
                                           pVidLutCurDac->HalObject.Dac[0].VerticalBlankHeight);
    U032      VerticalTotal             = pVidLutCurDac->HalObject.Dac[0].TotalHeight;
    U032      PixelDepth                = pVidLutCurDac->HalObject.Dac[0].PixelDepth;
    U032      PixelClock                = pVidLutCurDac->HalObject.Dac[0].PixelClock;
    U032      HorizontalSyncPolarity    = pVidLutCurDac->HalObject.Dac[0].Format.HorizontalSyncPolarity;
    U032      VerticalSyncPolarity      = pVidLutCurDac->HalObject.Dac[0].Format.VerticalSyncPolarity;
    U032      DoubleScannedMode         = pVidLutCurDac->HalObject.Dac[0].Format.DoubleScanMode;
    U032      Head                      = pVidLutCurDac->HalObject.Head;
    

    //
    // There is a bug in NV hardware where the hardware cursor will not clip correctly
    // if a border is defined in the display timings (blank start != visible end).  
    // Borders are only defined in two specific DMT modes.
    //
    // To fix this, set the blank starts to match the visible totals and remove the 
    // implicit borders.
    //
    // Remove this code when the hardware gets fixed.  Since this bug has been in the 
    // hardware since 1994, don't expect a fix soon.
    //
    HorizontalBlankStart = HorizontalVisible;
    HorizontalBlankEnd   = HorizontalTotal;
    VerticalBlankStart   = VerticalVisible;
    VerticalBlankEnd     = VerticalTotal;

    //
    // Enable full CRTC access
    //           
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, 0x57, Head);

    //
    // If this is for CRTC2, set CRTC access bit and PRAMDAC address
    // This access bit controls accesses to Sequencer and Graphics Controller (C03xx).
    // CRTC and Attribute Controller must be accessed at different addresses for each head
    // (6103xx/6033xx), since there is only one bus.
    //
    if (IsNV11(pDev))
    {

        // point CR44 at this head, for the rest of the modeset
        EnableHead(pDev, Head);

        REG_WR08(NV_PRMVIO_VSE2, 1);                    // enable VGA subsystem
        data08 = REG_RD08(NV_PRMVIO_MISC__READ);
        REG_WR08(NV_PRMVIO_MISC__WRITE, data08 | 1);    // enable CGA I/O space
    }


    CRTC_RD(NV_CIO_CR_VRE_INDEX, data08, Head);
    CRTC_WR(NV_CIO_CR_VRE_INDEX, data08 & ~0x80, Head);    // unlock CR0-7

    // Before unlocking / reading CR21 we should write CR3D[0] to 1 to enable reading of
    // the shadow registers.  This will allow us to read the real CR register values from
    // the locked registers.
    CRTC_RD(0x3D, savedCR3D, Head);     // read CR3D
    savedCR3D |= 0x1;                   // set CR3D[0] to 1
    CRTC_WR(0x3D, savedCR3D, Head);     // write back CR3D[0] = 1

    CRTC_RD(0x21, shadow, Head);        // read shadow lock
    shadow |= 0xfa;                     // enable HDE, VDE, and 8/9 dot chars too, for NV5.

    // Before programming the shadow registers you must set CR3D[0] to 0
    savedCR3D &= ~0x1;                  // set CR3D[0] to 0
    CRTC_WR(0x3D, savedCR3D, Head);     // write back CR3D[0] = 0

    CRTC_WR(0x21, shadow, Head);        // write back shadow
      
    // Set H and V retrace start to a large value to prevent possible lock-up.
    // This is because if the CRTC is slaved to an external device (the TV encoder),
    // it will stall waiting for retrace start to sync up with the external clock. 
    // When we switch from TV to CRT, we may get stuck in this stall. Setting retrace 
    // start to a large value guarantees this won't happen.
    CRTC_WR(NV_CIO_CR_HRS_INDEX, 0xff, Head);
    CRTC_WR(NV_CIO_CR_VRS_INDEX, 0xff, Head);

    //
    // Disable the visible screen
    //
    scratch = REG_RD32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    REG_WR08(NV_PRMCIO_ARX, 0x00);               // Turn off screen at AR
        
    //
    // FullCPUOn()
    //
    data = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
    data |= 0x2000;                             // Full Bandwidth bit ON
    WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit

    //
    // Set the sequencer & graphics controller values that are common for all packed-pixel
    // graphics modes.
    //            
    dacProgramSeq(pDev, Head);
    //
    // Program the CRTC & SR values for this particular mode.
    //
    dacProgramCRTC(pDev, Head, pVidLutCurDac,
                         HorizontalVisible,
                         HorizontalBlankStart,
                         HorizontalRetraceStart,
                         HorizontalRetraceEnd,
                         HorizontalBlankEnd,
                         HorizontalTotal,
                         VerticalVisible,
                         VerticalBlankStart,
                         VerticalRetraceStart,
                         VerticalRetraceEnd,
                         VerticalBlankEnd,
                         VerticalTotal,
                         PixelDepth,
                         PixelClock,
                         HorizontalSyncPolarity,
                         VerticalSyncPolarity,
                         DoubleScannedMode);


    // This code is necessary to fix a bug found where the memory controller would hang if a delay
    // was NOT inserted before turning on the display after programming the pixel clock PLL's.
    // Only required for SMA mode.
    if(IsNV0A(pDev)){
        if(pDev->Framebuffer.HalInfo.RamType == BUFFER_CRUSH_UMA){
            save_sr1 = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
            data = save_sr1;
            data |= 0x2000;                             // Full Bandwidth bit ON
            WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit
        }
    }

    //
    // Program the new VPLL clock rate
    //
    dacProgramPClk(pDev, Head, PixelClock);

    //
    // Native modes require a divide-by-2 clock ratio
    //                                    
    if (Head == 0) 
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V1SOURCE, _PROG);
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK_RATIO, _DB2);
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK_TV, _NONE);   // assume no TV for now
    }
    else
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V2SOURCE, _PROG);
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VCLK2_RATIO, _DB2);
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK2_TV, _NONE);  // assume no TV for now
    }
        
    if(!IsNV0A(pDev)){
        //
        // FullCpuOff()
        //
        data = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
        data &= 0xDFFF;                             // Full Bandwidth bit OFF
        WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit            
    }else{
        if(pDev->Framebuffer.HalInfo.RamType != BUFFER_CRUSH_UMA){
            data = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
            data &= 0xDFFF;                             // Full Bandwidth bit OFF
            WriteIndexed(NV_PRMVIO_SRX, data);          // Set bit            
        }
    }
    
    //
    // ATCOn()
    //
    scratch = REG_RD32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    REG_WR08(NV_PRMCIO_ARX, 0x20);               // Turn off screen at AR
    scratch = REG_RD32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
      
    //
    // Set ReadOnly CRTC access
    //           
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, 0x99, Head);

    if(IsNV0A(pDev)){
        if(pDev->Framebuffer.HalInfo.RamType == BUFFER_CRUSH_UMA){
            tmrDelay(pDev,/*U032 nsec*/100000000);
            save_sr1 &= 0xDFFF;                         // Full Bandwidth bit OFF
            WriteIndexed(NV_PRMVIO_SRX, save_sr1);      // Set bit            
        }
    }

    return RM_OK;

} // end of dacSetModeMini

//
// Mobile systems may require further notifications be made to the BIOS
// that we are changing display devices.
//
RM_STATUS dacMobileDisplayNotifyBegin
(
    PHWINFO pDev,
    U032    OldDevices,
    U032    NewDevices
)
{
#if !defined(MACOS) 
    U032    EAX, EBX, ECX, EDX;
    RM_STATUS status;
    U008    lock, crtcdata;
    U032    timeHI, timeLO;
#endif

// until osCallVideoBIOS() is added to the other os's
#if !defined(MACOS) 
    //
    // If we're not running mobile, don't do this
    //
    // NOTE: We can probably stop limiting this to just Toshiba
    //
    if (!pDev->Power.MobileOperation)
        return RM_OK;

    //Removed for non-Toshiba mobiles since backlight manufacture script implementation
    //    if (pDev->Power.MobileOperation != 2)
    //            return RM_OK;

    DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: dacMobileDisplayNotifyBegin\n");

    // If we aren't switching any devices, just ignore this notification
    //
    //Keeping this in will leave one screen blank during resume from hibernation
    // when in clone or extended mode.  Doh.  Other than that, it's a good idea...
//    if (OldDevices == NewDevices)
//        return RM_OK;

    // Incurr smart delay for stalling from power on to power off
    // This fixes 32252 and 31237 - Going from Clone to Extended mode
    // we get called 3 times to change the mode.  This will slow those modechanges down.
    if (pDev->Power.MobileOperation == 2)
    {
        tmrGetCurrentTime(pDev, &timeHI, &timeLO);
        if((pDev->Dac.PanelOffTimestampHI > timeHI) ||
           ((pDev->Dac.PanelOffTimestampHI == timeHI) &&
            (pDev->Dac.PanelOffTimestampLO > timeLO)) )
        {
            // Delay the difference of the times.  Unsigned math shouldn't matter.
            // Somehow this is sending delays over 500 ms. Since in 
            // MobileDisplayEnd we set the delay to 500, this is wrong.
            // So, now, I should just delay if it's under 500 ms.
            if((pDev->Dac.PanelOffTimestampLO - timeLO) < 500000000)
              tmrDelay(pDev, (pDev->Dac.PanelOffTimestampLO - timeLO));
        }
    }

    // 
    // If the FP is involved, unconditionally turn off the backlight
    // (with more work, we probably could make this test smarter).
    //
    if ((OldDevices & TOSHIBA_FP) && (NewDevices & TOSHIBA_FP) && !(pDev->Power.Flags & DAC_POWER_PENDING))
    {
        dacBacklightOffMobilePanel(pDev);

        // 40ms delay between power and bl enable
        // I don't understand it, but it fixes bug 31339.
        // Before removing this, make sure to test 31339.
        // Well now, I'm upping the ante. We're setting this to 100 ms to fix
        // bug 31847.  So before removing this, check bug 31847 as well.
        // Now I'm making it 120 ms to fix 32383.  110 doesn't work.
        tmrDelay(pDev, 120000000);
    }

        //This is Toshiba bios specific code
    if (pDev->Power.MobileOperation == 2)
    {
        //
        // Generate a DeviceControlBegin message to the video BIOS
        // so the system knows we're about to change displays
        //
        EAX = 0x4F14;
        EBX = 0x028F;
        ECX = 0;
    
        //To know the arg order for DX, we have to check a scratch bit that tells
        // us whether this SBIOS got it right or wrong...
        lock = UnlockCRTC(pDev, 0);
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, crtcdata, 0);
        RestoreLock(pDev, 0, lock);
    
        if (crtcdata & NV_CORRECT_ARG_ORDER) {
            EDX = (NewDevices << 8) | OldDevices;
        } else {
            EDX = (OldDevices << 8) | NewDevices;
        }
    
        status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
    
        // Toshiba turns off the panel if it's not used.
        if(!(NewDevices & TOSHIBA_FP))
            pDev->Dac.DevicesEnabled &= ~(DAC_PANEL_ENABLED);
    
        if (status)
            return status;
    }
#endif

    return RM_OK;
}

RM_STATUS dacMobileDisplayNotifyEnd
(
    PHWINFO pDev,
    U032    OldDevices,
    U032    NewDevices
)
{
#if !defined(MACOS)
    U032    i;
    U032    EAX, EBX, ECX, EDX;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    RM_STATUS status;
#endif

// until osCallVideoBIOS() is added to the other os's
#if !defined(MACOS)
    //
    // If we're not running mobile, don't do this
    //
    // NOTE: We can probably stop limiting this to just Toshiba
    //
    if (!pDev->Power.MobileOperation)
        return RM_OK;


    //Removed for non-Toshiba mobiles since backlight manufacture script implementation
    //    if (pDev->Power.MobileOperation != 2)
    //            return RM_OK;

    // If we aren't switching any devices, just ignore this notification
    //
//    if (OldDevices == NewDevices)
//        return RM_OK;

    DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: dacMobileDisplayNotifyEnd\n");

    if (pDev->Power.MobileOperation == 2)
    {
        //
        // Track what has been set as the active devices
        // Since this routine can exit early from about 50,000,000,000 different places, I've
        // moved this code to the begining rather than the end, since it should always be executed.
        //
        // For now just set current display type to DISPLAY_TYPE_NONE to effectively disable the 
        // code in dacMobileDisplayNotifyBegin because executing it seems to cause other problems
        // even though it shouldn't.  Need to get Toshiba's help in debugging what's going on in the
        // sbios black box.  I suspect there is a bug in there somewhere.
        //
//        if (pDev->Dac.CrtcInfo[0].pVidLutCurDac)
//            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = ((PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[0].pVidLutCurDac)->DisplayType;
//        else
            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = DISPLAY_TYPE_NONE;
//        if (pDev->Dac.CrtcInfo[1].pVidLutCurDac)
//            pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType = ((PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[1].pVidLutCurDac)->DisplayType;
//        else
            pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType = DISPLAY_TYPE_NONE;
        //
        // Generate a DeviceControlEnd message to the video BIOS
        // so the system knows we're done changing displays
        //
        EAX = 0x4F14;
        EBX = 0x038F;
        ECX = 0;
        EDX = NewDevices;
        status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
    
        if (status)
            return status;
    }
    else
    {
        //
        // Track what has been set as the active devices
        // Since this routine can exit early from about 50,000,000,000 different places, I've
        // moved this code to the begining rather than the end, since it should always be executed.
        //
        // For non-Toshiba, this still needs to be tracked.
        // And hopefully we can actually do the right thing on non-toshiba and actually track the
        // devices correctly (unlike on Toshiba for some reason).
        //
        if (pDev->Dac.CrtcInfo[0].pVidLutCurDac)
            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = ((PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[0].pVidLutCurDac)->DisplayType;
        else
            pDev->Dac.HalInfo.CrtcInfo[0].CurrentDisplayType = DISPLAY_TYPE_NONE;
        if (pDev->Dac.CrtcInfo[1].pVidLutCurDac)
            pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType = ((PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[1].pVidLutCurDac)->DisplayType;
        else
            pDev->Dac.HalInfo.CrtcInfo[1].CurrentDisplayType = DISPLAY_TYPE_NONE;
    }

    // APS -- added for Bug 33993 -- if we are switching to LCD and the BackLights are not on then turn them on
    if (((OldDevices & TOSHIBA_FP) && (NewDevices & TOSHIBA_FP)) || ((NewDevices & TOSHIBA_FP) && (!(pDev->Dac.DevicesEnabled & DAC_PANEL_ENABLED))))
    {
        if(!(pDev->Power.Flags & DAC_POWER_PENDING))
        {
            // For Toshiba only delay a bit to make sure that last bios call has finished up.
            // don't need for non-toshiba, so don't penalize them.
            if (pDev->Power.MobileOperation == 2)
                tmrDelay(pDev, 1000000*75); // 75ms
            // Turn on the backlight
            dacBacklightOnMobilePanel(pDev);
            pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;
        }

        //
        // Note: in the case of a hotkey-based DFP+TV modeset, turning on the
        // backlight displays the DFP, but for some reason turns off the TV.
        //
        // I imagine we've confused Toshiba with our manipulations of the
        // backlight to clean up the modesets. To workaround this, we'll follow
        // up with dacEnableImage calls for any heads that have modes set.
        //
        for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
        {
            pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[i].pVidLutCurDac;
            if (pVidLutCurDac && (pVidLutCurDac->DisplayType != DISPLAY_TYPE_NONE))
                dacEnableImage(pDev, i);
        }
    }

    //We've done for all vendors except Toshiba
    if(pDev->Power.Flags & DAC_POWER_PENDING)
        return RM_OK;
    
    if (pDev->Power.MobileOperation == 2)
    {
        // Start the 500 ms smart delay for stalling from power on to power off
        // This fixes 32252 and 31237 - Going from Clone to Extended mode
        // we get called 3 times to change the mode.  This will slow those modechanges down.
        tmrGetCurrentTime(pDev, 
            &(pDev->Dac.PanelOffTimestampHI), &(pDev->Dac.PanelOffTimestampLO));
            
        // Go ahead and add the delay to the timestamp
        // Check for carry condition
        if((pDev->Dac.PanelOffTimestampLO + 500000000) < pDev->Dac.PanelOffTimestampLO)
        {
            pDev->Dac.PanelOffTimestampHI++;
        }
        pDev->Dac.PanelOffTimestampLO += 500000000;
    }
#endif

    return RM_OK;
}

// Disable slaving the CRTC to an external PCLK
// We must do this in such a way that the scanline counter doesn't stall (it can stall if unslaving during blanking)
VOID dacDisableCRTCSlave
(
    PHWINFO pDev, 
    U032    Head
)
{
        BOOL          bCrtcSlaved;
        volatile U032 retryCount, rasterPos0, rasterPos1, rasterPosBase;
        U008          data08, crHRS;
        // Set HRS to > HT, so we never get into blanking
        // This will prevent the counter from getting stalled
        CRTC_RD(NV_CIO_CR_HDT_INDEX, data08, Head);
        CRTC_RD(NV_CIO_CR_HRS_INDEX, crHRS, Head);
        CRTC_WR(NV_CIO_CR_HRS_INDEX, data08 + 6, Head);
        tmrDelay(pDev, 1000000);
        CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data08, Head);
        bCrtcSlaved = ((data08 & 0x80) != 0);
        data08 &= ~0x84;
        CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data08, Head);
        //
        // Work around a hardware bug that's present on all chips NV20 and earlier.
        // Should be fixed in NV17/NV25.
        //
        // Only do the workaround if the crtc was slaved when 
        //
        if (bCrtcSlaved)
        {
            //
            // After clearing CR28[7] make sure the raster position counter has not gotten stuck.
            // If it has, then re-enable CR28[7] to kick start the raster position counter and try
            // again.
            // It's not elegant but none of the suggested work arounds worked. Fortunately the problem
            // is intermittent enough that a stuck raster position counter should only have to be
            // kick started once.
            //

            U032 usec     = 2000000;
            U032 retryMax = 25;
            retryCount = 0;
            rasterPosBase = REG_RD32(NV_PCRTC_RASTER + CurDacAdr(Head));
            while (bCrtcSlaved && (retryCount < retryMax))
            {
                rasterPos0 = REG_RD32(NV_PCRTC_RASTER + CurDacAdr(Head));
                tmrDelay(pDev, usec);
                rasterPos1 = REG_RD32(NV_PCRTC_RASTER + CurDacAdr(Head));
                if (rasterPos0 == rasterPos1)
                {
                    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, (data08 | 0x80), Head);
                    tmrDelay(pDev, 1000000);
                    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data08, Head);
                    retryCount++;
                }
                else
                    bCrtcSlaved = FALSE;
            }
            if (bCrtcSlaved)
            {
                //
                // failed to kick start the raster position count.
                // unslave the crtc one last time and continue on.  Hopefully nothing bad is about to happen!
                //
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data08, Head);
                DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: dacDisableCRTCSlave - Exceeded maximum retry count.  Hope nothing bad is about to happen!!\r\n");
            }
            else
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: dacDisableCRTCSlave-Unslaved,Retry count \r\n",retryCount);

            }
        }
        else
        {
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: dacDisableCRTCSlave, not slaved \r\n");

        }
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: dacDisableCRTCSlave,   rasterPosBase : ",rasterPosBase);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: dacDisableCRTCSlave,      rasterPos0 : ",rasterPos0);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: dacDisableCRTCSlave exit, rasterPos1 : ",rasterPos1);
        //
        // Restore the original value of CR_HRS.
        // During power management (monitor off) there isn't any modeset to restore this value,
        // so if it is not restored now, it will result in an incorrect tv display when the tv is
        // turned back on.
        // ***Hopefully restoring this after we've made sure that the raster counter did not get
        // ***stuck will not have any nasty side effects.
        //
        CRTC_WR(NV_CIO_CR_HRS_INDEX, crHRS, Head);
}

RM_STATUS ExtractLastModeset(PHWINFO pDev, U032 Head, PVIDEO_LUT_CURSOR_DAC_HAL_OBJECT pHalVidLutCurDac)
{
    U032 TemporaryValue;
    U032 TMVal,TNVal,TPVal;
    U032 TvA;
    U008 * pGeneric;
    U032 count;
    RM_STATUS OpStatus;
    U008 lock;
    
    lock=UnlockCRTC(pDev,Head);

    if(pHalVidLutCurDac)
    {    
        pGeneric=(U008 *)pHalVidLutCurDac;
        
        //clear the structure to all zero's
        for(count=0;count<sizeof(VIDEO_LUT_CURSOR_DAC_HAL_OBJECT);count++)
        {
            pGeneric[count]=(U008)0;
        }
        
        if(pHalVidLutCurDac)
        {
            pHalVidLutCurDac->Head=Head;
            
            pHalVidLutCurDac->Dac[0].TotalWidth = dacGetCRTC(pDev, Head, H_TOTAL);    
            pHalVidLutCurDac->Dac[0].VisibleImageWidth = dacGetCRTC(pDev, Head, H_DISPLAY_END);    
            pHalVidLutCurDac->Dac[0].HorizontalBlankStart = dacGetCRTC(pDev, Head, H_BLANK_START);    
            pHalVidLutCurDac->Dac[0].HorizontalBlankWidth = dacGetCRTC(pDev, Head, H_BLANK_END)-dacGetCRTC(pDev, Head, H_BLANK_START);    
            pHalVidLutCurDac->Dac[0].HorizontalSyncStart = dacGetCRTC(pDev, Head, H_RETRACE_START);    
            pHalVidLutCurDac->Dac[0].HorizontalSyncWidth = dacGetCRTC(pDev, Head, H_RETRACE_END)-dacGetCRTC(pDev, Head, H_RETRACE_START);    
            pHalVidLutCurDac->Dac[0].TotalHeight = dacGetCRTC(pDev, Head, V_TOTAL);    
            pHalVidLutCurDac->Dac[0].VisibleImageHeight = dacGetCRTC(pDev, Head, V_DISPLAY_END);    
            pHalVidLutCurDac->Dac[0].VerticalSyncStart = dacGetCRTC(pDev, Head, V_RETRACE_START);    
            pHalVidLutCurDac->Dac[0].VerticalSyncHeight = dacGetCRTC(pDev, Head, V_RETRACE_END)-dacGetCRTC(pDev, Head, V_RETRACE_START);    
            pHalVidLutCurDac->Dac[0].VerticalBlankStart = dacGetCRTC(pDev, Head, V_BLANK_START);    
            pHalVidLutCurDac->Dac[0].VerticalBlankHeight = dacGetCRTC(pDev, Head, V_BLANK_END)-dacGetCRTC(pDev, Head, V_BLANK_START); 
                    
            //Read raw pixel clock on NV11
            if (Head == 0) 
            {
                TemporaryValue = REG_RD32( NV_PRAMDAC_VPLL_COEFF );
                
                TMVal=DRF_VAL(_PRAMDAC,_VPLL_COEFF,_MDIV,TemporaryValue);
                TNVal=DRF_VAL(_PRAMDAC,_VPLL_COEFF,_NDIV,TemporaryValue);
                TPVal=DRF_VAL(_PRAMDAC,_VPLL_COEFF,_PDIV,TemporaryValue);
            }
            else
            {
                TemporaryValue = REG_RD32( NV_PRAMDAC_VPLL2_COEFF );
                    
                TMVal=DRF_VAL(_PRAMDAC,_VPLL2_COEFF,_MDIV,TemporaryValue);
                TNVal=DRF_VAL(_PRAMDAC,_VPLL2_COEFF,_NDIV,TemporaryValue);
                TPVal=DRF_VAL(_PRAMDAC,_VPLL2_COEFF,_PDIV,TemporaryValue);
            }
            
            pHalVidLutCurDac->Dac[0].PixelDepth=(U032)((TemporaryValue==1)?8:(TemporaryValue==2)?16:(TemporaryValue==3)?32:8);
            
            CRTC_RD(0x28,TvA,Head);
            TemporaryValue=((U032)TvA)&0x3;
            pHalVidLutCurDac->Dac[0].PixelClock=(14318180*(TNVal/TMVal))/(1<<TPVal); 
            
            //educated guess at the color format
            pHalVidLutCurDac->Dac[0].ColorFormat=(U032)((pHalVidLutCurDac->Dac[0].PixelDepth==16)?NVFF6_SET_COLOR_FORMAT_LE_X16A1R5G5B5:(pHalVidLutCurDac->Dac[0].PixelDepth==32)?NVFF6_SET_COLOR_FORMAT_LE_X24Y8:(pHalVidLutCurDac->Dac[0].PixelDepth==8)?NVFF6_SET_COLOR_FORMAT_LE_X8R8G8B8:0); 
            
            CRTC_RD(0x09,TvA,Head);
            pHalVidLutCurDac->Dac[0].Format.DoubleScanMode=(BIT(7)&((U032)TvA)?1:0);
            
            CRTC_RD(0x39,TvA,Head);
            pHalVidLutCurDac->Dac[0].Format.InterlacedMode=(((U032)TvA)==0xff?0:1);
            
            TvA=REG_RD08(NV_PRMVIO_MISC__READ);
            pHalVidLutCurDac->Dac[0].Format.HorizontalSyncPolarity=(BIT(6)&((U032)TvA)?1:0);
            pHalVidLutCurDac->Dac[0].Format.VerticalSyncPolarity=(BIT(7)&((U032)TvA)?1:0);
            
        }
        
        //this debug is always active on Mac, though I can disable it if it hurts anyone.
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Start HAL Dac Struct print out");
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: --------------------------------------------------------------------------");
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Visible Image Width     : ",pHalVidLutCurDac->Dac[0].VisibleImageWidth);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Visible Image Height    : ",pHalVidLutCurDac->Dac[0].VisibleImageHeight);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Horizontal Blank Start  : ",pHalVidLutCurDac->Dac[0].HorizontalBlankStart);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Horizontal Blank Width  : ",pHalVidLutCurDac->Dac[0].HorizontalBlankWidth);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Horizontal Sync Start   : ",pHalVidLutCurDac->Dac[0].HorizontalSyncStart);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Horizontal Sync Width   : ",pHalVidLutCurDac->Dac[0].HorizontalSyncWidth);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Vertical Blank Start    : ",pHalVidLutCurDac->Dac[0].VerticalBlankStart);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Vertical Blank Height   : ",pHalVidLutCurDac->Dac[0].VerticalBlankHeight);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Vertical Sync Start     : ",pHalVidLutCurDac->Dac[0].VerticalSyncStart);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Vertical Sync Height    : ",pHalVidLutCurDac->Dac[0].VerticalSyncHeight);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Total Width             : ",pHalVidLutCurDac->Dac[0].TotalWidth);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Total Height            : ",pHalVidLutCurDac->Dac[0].TotalHeight);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Pixel Clock             : ",pHalVidLutCurDac->Dac[0].PixelClock);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Color Format            : ",pHalVidLutCurDac->Dac[0].ColorFormat);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Pixel Depth             : ",pHalVidLutCurDac->Dac[0].PixelDepth);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Double Scan Mode        : ",pHalVidLutCurDac->Dac[0].Format.DoubleScanMode);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Interlaced Mode         : ",pHalVidLutCurDac->Dac[0].Format.DoubleScanMode);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Horizontal Sync Polarity: ",pHalVidLutCurDac->Dac[0].Format.DoubleScanMode);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Vertical Sync Polarity  : ",pHalVidLutCurDac->Dac[0].Format.DoubleScanMode);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: --------------------------------------------------------------------------");
        
        OpStatus=RM_OK;
    }
    else
    {
        OpStatus=RM_ERROR;
    }
    
    RestoreLock(pDev,Head,lock);
    
    
    return OpStatus;
}

RM_STATUS initPVidLutCurDacStructs(PHWINFO pDev)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDacObj;
    U032 count;
    RM_STATUS OpStatus=RM_OK;
    
    PHALHWINFO pHalHwInfo;
    PDACHALINFO pDacHalInfo;
    
    pHalHwInfo=&(pDev->halHwInfo);
    pDacHalInfo=(pHalHwInfo->pDacHalInfo);
    
    //all cards have head zero, so scan all heads avail.
    for(count=0;(count<pDev->Dac.HalInfo.NumCrtcs)&&(OpStatus==RM_OK);count++)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"NVRM: init vidlutcurdacs - counting");

        if(GETDISPLAYTYPE(pDev,count)!=DISPLAY_TYPE_NONE)
        {
            OpStatus = osAllocMem((VOID **) &pVidLutCurDacObj, sizeof(VIDEO_LUT_CURSOR_DAC_OBJECT));
            initRmPVidLutCurDacStructs(pDev, count, pVidLutCurDacObj);
            
            pDev->Dac.CrtcInfo[count].pVidLutCurDac=(VOID_PTR)pVidLutCurDacObj;
            
            //set the hal ptrs for this
            pDacHalInfo->CrtcInfo[count].pVidLutCurDac=&(pVidLutCurDacObj->HalObject);
            
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Good VidLutCur created on Head: ",count);
            
            /*
            //this routine assumes there can only be one flat panel on this device
            if(GETDISPLAYTYPE(pDev,count)==DISPLAY_TYPE_FLAT_PANEL)
            {
                //load in data from the registers to the pDev
                pDev->Dac.fpHActive=DAC_REG_RD32(NV_PRAMDAC_FP_HDISPLAY_END,count)+1;
                pDev->Dac.fpHDispEnd=DAC_REG_RD32(NV_PRAMDAC_FP_HDISPLAY_END,count);
                pDev->Dac.fpHValidEnd=DAC_REG_RD32(NV_PRAMDAC_FP_HVALID_END,count);
                pDev->Dac.fpHSyncStart=DAC_REG_RD32(NV_PRAMDAC_FP_HSYNC_START,count);
                pDev->Dac.fpHSyncEnd=DAC_REG_RD32(NV_PRAMDAC_FP_HSYNC_END,count);
                pDev->Dac.fpHValidStart=DAC_REG_RD32(NV_PRAMDAC_FP_HVALID_START,count);
                pDev->Dac.fpHTotal=DAC_REG_RD32(NV_PRAMDAC_FP_HTOTAL,count);
                pDev->Dac.fpVActive=DAC_REG_RD32(NV_PRAMDAC_FP_VDISPLAY_END,count)+1;
                pDev->Dac.fpVDispEnd=DAC_REG_RD32(NV_PRAMDAC_FP_VDISPLAY_END,count);
                pDev->Dac.fpVValidEnd=DAC_REG_RD32(NV_PRAMDAC_FP_VVALID_END,count);
                pDev->Dac.fpVSyncStart=DAC_REG_RD32(NV_PRAMDAC_FP_VSYNC_START,count);
                pDev->Dac.fpVSyncEnd=DAC_REG_RD32(NV_PRAMDAC_FP_VSYNC_END,count);
                pDev->Dac.fpVValidStart=DAC_REG_RD32(NV_PRAMDAC_FP_VVALID_START,count);
                pDev->Dac.fpVTotal=DAC_REG_RD32(NV_PRAMDAC_FP_VTOTAL,count);
            }
            */
        }
        else
        {
            //NULL everything to be consistent with docs in class7c
            //kill it off now while we have the chance!!!
            pDev->Dac.CrtcInfo[count].pVidLutCurDac=NULL;
            
            //set the hal ptrs for this
            pDacHalInfo->CrtcInfo[count].pVidLutCurDac=NULL;
            
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: No display on Head: ",count);
        }
    }
    
    if(OpStatus!=RM_OK)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"NVRM: Error Detected: Cleaning up.\n");
        
        for(count=0;count < pDev->Dac.HalInfo.NumCrtcs; count++)
        {
            if(pDev->Dac.CrtcInfo[count].pVidLutCurDac)
            {
                //null the hal pointer
                pDacHalInfo->CrtcInfo[count].pVidLutCurDac=NULL;
                
                //free the memory
                osFreeMem((void *)pDev->Dac.CrtcInfo[count].pVidLutCurDac);
                
                //null the second pointer
                pDev->Dac.CrtcInfo[count].pVidLutCurDac=NULL;
            }
        }
    }
            
    return OpStatus;
}

RM_STATUS initRmPVidLutCurDacStructs(PHWINFO pDev, U032 Head, PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac)
{
    RM_STATUS OpStatus=RM_ERROR;  //we might be just passin' thru if we have a NULL pointer
    U032 Height,Width,PixelClock;
    U008 lock;
    
    lock=UnlockCRTC(pDev,Head);
    
    if(pVidLutCurDac)
    {
        OpStatus=RM_OK;
    
        //blank the structure
        osMemSet((void*)pVidLutCurDac,0,sizeof(VIDEO_LUT_CURSOR_DAC_OBJECT));
        
        //setup the Hal object
        ExtractLastModeset(pDev, Head, &(pVidLutCurDac->HalObject));
        
        //code stolen from dacGetDisplayType() (its statically declared)
        //get something
        pVidLutCurDac->DisplayType=GETDISPLAYTYPE(pDev,Head);
        
        //TV guys, feel free to elaborate here if you need the current function
        pVidLutCurDac->TVStandard=0;                                 // variations of NTSC and PAL
        
        //calculate RefreshRate (Pixel clock/)
        Height=pVidLutCurDac->HalObject.Dac[0].TotalHeight;
        Width=pVidLutCurDac->HalObject.Dac[0].TotalWidth;
        PixelClock=pVidLutCurDac->HalObject.Dac[0].PixelClock;
        
        pVidLutCurDac->RefreshRate=PixelClock/(Height*Width);
        
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Display Type        : ",pVidLutCurDac->DisplayType);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Display Pixel Clock : ",pVidLutCurDac->RefreshRate);
    }
    
    RestoreLock(pDev,Head, lock);
   
    return OpStatus;
}


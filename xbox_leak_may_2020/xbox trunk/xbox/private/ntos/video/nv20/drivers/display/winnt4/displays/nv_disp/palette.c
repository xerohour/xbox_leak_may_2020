//******************************Module*Header***********************************
//
// Module Name: palette.c
//
// Palette support.
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "nv32.h"
#include "driver.h"

// Global Table defining the 20 Window default colours.  For 256 colour
// palettes the first 10 must be put at the beginning of the palette
// and the last 10 at the end of the palette.
PALETTEENTRY gapalBase[20] =
{
    { 0,   0,   0,   0 },       // 0
    { 0x80,0,   0,   0 },       // 1
    { 0,   0x80,0,   0 },       // 2
    { 0x80,0x80,0,   0 },       // 3
    { 0,   0,   0x80,0 },       // 4
    { 0x80,0,   0x80,0 },       // 5
    { 0,   0x80,0x80,0 },       // 6
    { 0xC0,0xC0,0xC0,0 },       // 7
    { 192, 220, 192, 0 },       // 8
    { 166, 202, 240, 0 },       // 9
    { 255, 251, 240, 0 },       // 10
    { 160, 160, 164, 0 },       // 11
    { 0x80,0x80,0x80,0 },       // 12
    { 0xFF,0,   0   ,0 },       // 13
    { 0,   0xFF,0   ,0 },       // 14
    { 0xFF,0xFF,0   ,0 },       // 15
    { 0   ,0,   0xFF,0 },       // 16
    { 0xFF,0,   0xFF,0 },       // 17
    { 0,   0xFF,0xFF,0 },       // 18
    { 0xFF,0xFF,0xFF,0 },       // 19
};

//******************************************************************************
//
//  Function:   bInitializePalette
//
//  Routine Description:
//
//      Initializes default palette for PDEV.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL bInitializePalette(
PDEV*    ppdev,
DEVINFO* pdi)

    {
    PALETTEENTRY*   ppal;
    PALETTEENTRY*   ppalTmp;
    ULONG           ulLoop;
    BYTE            jRed;
    BYTE            jGre;
    BYTE            jBlu;
    HPALETTE        hpal;

    //**********************************************************************
    // Allocate our palette. We create a default 256 color palette for
    // all the video modes. The reason for this is that we need to store
    // an 8BPP palette in the PDEV since DrvSetCursorShape can be called
    // with an 8BPP bitmap and an XLATEOBJ with no RGB values. In this case
    // we use the default palette RGB values to convert it to 16BPP, which
    // is what our color cursor hw requires.
    //**********************************************************************

    ppal = (PALETTEENTRY *) EngAllocMem(FL_ZERO_MEMORY, sizeof(PALETTEENTRY) * 256, ALLOC_TAG);
    if (ppal == NULL)
        goto ReturnFalse;

    ppdev->pPal = ppal;

    //**********************************************************************
    // Generate 256 (8*4*4) RGB combinations to fill the palette
    //**********************************************************************

    jRed = 0;
    jGre = 0;
    jBlu = 0;

    ppalTmp = ppal;

    for (ulLoop = 256; ulLoop != 0; ulLoop--)
        {
        ppalTmp->peRed   = jRed;
        ppalTmp->peGreen = jGre;
        ppalTmp->peBlue  = jBlu;
        ppalTmp->peFlags = 0;

        ppalTmp++;

        if (!(jRed += 32))
            if (!(jGre += 32))
                jBlu += 64;
        }

    //**********************************************************************
    // Fill in Windows reserved colours from the WIN 3.0 DDK
    // The Window Manager reserved the first and last 10 colours for
    // painting windows borders and for non-palette managed applications.
    //**********************************************************************

    for (ulLoop = 0; ulLoop < 10; ulLoop++)
        {
        //******************************************************************
        // First 10
        //******************************************************************

        ppal[ulLoop]       = gapalBase[ulLoop];

        //******************************************************************
        // Last 10
        //******************************************************************

        ppal[246 + ulLoop] = gapalBase[ulLoop+10];
        }

    //**********************************************************************
    // Get handle for palette from GRE.
    //**********************************************************************

    if (ppdev->iBitmapFormat == BMF_8BPP)
        {
        hpal = EngCreatePalette(PAL_INDEXED, 256, (ULONG*) ppal, 0, 0, 0);
        }
    else

        {
        ASSERTDD((ppdev->iBitmapFormat == BMF_16BPP) ||
         (ppdev->iBitmapFormat == BMF_24BPP) ||
         (ppdev->iBitmapFormat == BMF_32BPP),
         "This case handles only 16, 24 or 32bpp");

        hpal = EngCreatePalette(PAL_BITFIELDS, 0, NULL,
                                ppdev->flRed, ppdev->flGreen, ppdev->flBlue);
        }

    ppdev->hpalDefault = hpal;
    pdi->hpalDefault   = hpal;

    if (hpal == 0)
        goto ReturnFalse;

    return(TRUE);

ReturnFalse:

    DISPDBG((2, "Failed bInitializePalette"));
    return(FALSE);
    }

/******************************Public*Routine******************************\
* VOID vUninitializePalette
*
* Frees resources allocated by bInitializePalette.
*
* Note: In an error case, this may be called before bInitializePalette.
*
\**************************************************************************/

VOID vUninitializePalette(PDEV* ppdev)
{
    // Delete the default palette if we created one:

    if (ppdev->hpalDefault != 0)
        EngDeletePalette(ppdev->hpalDefault);

    if (ppdev->pPal != (PALETTEENTRY*) NULL)
        EngFreeMem(ppdev->pPal);
}

//******************************************************************************
//
//  Function:   bEnablePalette
//
//      Initialize the hardware's 8bpp palette registers.
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL bEnablePalette(PDEV* ppdev)

    {
    ULONG i, j;
    ULONG ulHead, ulOffset;



    if (ppdev->iBitmapFormat == BMF_8BPP)
    {
        //**********************************************************************
        // Set palette using hardware specific routine
        // (Specify start and length)
        //**********************************************************************

		for(i = 0; i < ppdev->ulNumberDacsActive; i++)
    	{
        	ulHead = ppdev->ulDeviceDisplay[i];
	        ulOffset = ulHead*256;
   
    	    for(j = 0 ; j < 256; j++)
        	{
	            ppdev->ajClutData[j + ulOffset].Red =    ppdev->pPal[j].peRed >>
                                          ppdev->cPaletteShift;
    	        ppdev->ajClutData[j + ulOffset].Green =  ppdev->pPal[j].peGreen >>
                                          ppdev->cPaletteShift;
        	    ppdev->ajClutData[j + ulOffset].Blue =   ppdev->pPal[j].peBlue >>
                                          ppdev->cPaletteShift;
	            ppdev->ajClutData[j + ulOffset].Unused = 0;
    	    }
	    }
        ppdev->pfnSetPalette(ppdev, 0, 256 * sizeof(VIDEO_CLUTDATA));
    }


    DISPDBG((5, "Passed bEnablePalette"));

    return(TRUE);
    }

/******************************Public*Routine******************************\
* VOID vDisablePalette
*
* Undoes anything done in bEnablePalette.
*
\**************************************************************************/

VOID vDisablePalette(
PDEV*   ppdev)
{
    // Nothin' to do
}

/******************************Public*Routine******************************\
* VOID vAssertModePalette
*
* Sets/resets the palette in preparation for full-screen/graphics mode.
*
\**************************************************************************/

VOID vAssertModePalette(
PDEV*   ppdev,
BOOL    bEnable)
{
    // USER immediately calls DrvSetPalette after switching out of
    // full-screen, so we don't have to worry about resetting the
    // palette here.
}

/******************************Public*Routine******************************\
* BOOL DrvSetPalette
*
* DDI entry point for manipulating the palette.
*
\**************************************************************************/

BOOL DrvSetPalette(
DHPDEV  dhpdev,
PALOBJ* ppalo,
FLONG   fl,
ULONG   iStart,
ULONG   cColors)
{
    BYTE            ajClutSpace[MAX_CLUT_SIZE];
    PVIDEO_CLUT     pScreenClut;
    PDEV*           ppdev = (PDEV *) dhpdev;
    UCHAR   cTemp;
    ULONG i;
    ULONG ulHead, ulCount;
    ULONG ulOffset;

    UNREFERENCED_PARAMETER(fl);

    ACQUIRE_CRTC_CRITICAL_SECTION(ppdev);
    
    // Fill in pScreenClut header info:

    pScreenClut             = (PVIDEO_CLUT) ajClutSpace;
    pScreenClut->NumEntries = (USHORT) cColors;
    pScreenClut->FirstEntry = (USHORT) iStart;

    // Set the high reserved byte in each palette entry to 0.
    // Do the appropriate palette shifting to fit in the DAC.
   	for(i = 0; i < ppdev->ulNumberDacsActive; i++)
    {
        ulCount = cColors;
        ulOffset = ppdev->ulDeviceDisplay[i] * 256;

        if (cColors != PALOBJ_cGetColors(ppalo, iStart, cColors,
                                         (ULONG*) (ppdev->ajClutData+ulOffset)))
        {
            DISPDBG((2, "DrvSetPalette failed PALOBJ_cGetColors"));
            RELEASE_CRTC_CRITICAL_SECTION(ppdev);
            return (FALSE);
        }

        if (ppdev->cPaletteShift)
        {
            while(ulCount--)
            {
                cTemp = ppdev->ajClutData[ulCount+ulOffset].Red >> ppdev->cPaletteShift;
                ppdev->ajClutData[ulCount+ulOffset].Red = ppdev->ajClutData[ulCount+ulOffset].Blue >> ppdev->cPaletteShift;
                ppdev->ajClutData[ulCount+ulOffset].Blue = cTemp;
                ppdev->ajClutData[ulCount+ulOffset].Green >>= ppdev->cPaletteShift;
                ppdev->ajClutData[ulCount+ulOffset].Unused = 0;
            }
        }
        else
        {
            while(ulCount--)
            {
                cTemp = ppdev->ajClutData[ulCount+ulOffset].Red;
                ppdev->ajClutData[ulCount+ulOffset].Red = ppdev->ajClutData[ulCount+ulOffset].Blue;
                ppdev->ajClutData[ulCount+ulOffset].Blue = cTemp;
                ppdev->ajClutData[ulCount+ulOffset].Unused = 0;
            }
        }
    }

    //**************************************************************************
    // Setup the palette using hardware specific routine
    // (Specify start and length)
    //**************************************************************************

    if(ppdev->bEnabled) // Incomplete fix for the av in V4DmaPushSetPalette if acceleration is disabled.
        ppdev->pfnSetPalette(ppdev, 0, 256 * sizeof(VIDEO_CLUTDATA));
    else
    {
        PVIDEO_CLUTDATA pScreenClutData;

        ulCount = cColors;
        pScreenClutData = (PVIDEO_CLUTDATA)(&(pScreenClut->LookupTable[0]));

        if (cColors != PALOBJ_cGetColors(ppalo, iStart, cColors,
                                         (ULONG*)pScreenClutData))
        {
            DISPDBG((2, "DrvSetPalette failed PALOBJ_cGetColors"));
            RELEASE_CRTC_CRITICAL_SECTION(ppdev);
            return (FALSE);
        }

        if(ppdev->cPaletteShift)
        {
            while(ulCount--)
            {
                pScreenClutData[ulCount].Red >>= ppdev->cPaletteShift;
                pScreenClutData[ulCount].Green >>= ppdev->cPaletteShift;
                pScreenClutData[ulCount].Blue >>= ppdev->cPaletteShift;
                pScreenClutData[ulCount].Unused = 0;
            }
        }
        else
        {
            while(ulCount--)
            {
                pScreenClutData[ulCount].Unused = 0;
            }
        }

        if(EngDeviceIoControl(ppdev->hDriver,
                              IOCTL_VIDEO_SET_COLOR_REGISTERS,
                              pScreenClut,
                              MAX_CLUT_SIZE,
                              NULL,
                              0,
                              &ulCount))
        {
            RELEASE_CRTC_CRITICAL_SECTION(ppdev);
            return(FALSE);
        }
    }

    RELEASE_CRTC_CRITICAL_SECTION(ppdev);
    return(TRUE);

}

/******************************Public*Routine******************************\
* ULONG nvSetGammaRamp
*
* DDI entry point for setting gamma ramp.
*
\**************************************************************************/
ULONG
nvSetGammaRamp(PDEV *ppdev, PUSHORT fpRampVals)
{
    ULONG nNumEntries = 256;
    ULONG nStartIndex = 0;
    ULONG i, ulHead, ulDevice, ulOffset;

    if (!ppdev) return(FALSE);

    if(ppdev->iBitmapFormat != BMF_8BPP)
    {
        for(ulDevice = 0; ulDevice < ppdev->ulNumberDacsActive; ulDevice++)
        {
            ulHead = ppdev->ulDeviceDisplay[ulDevice];
            ulOffset = ulHead * 256;

            for (i = 0; i < nNumEntries; i++) 
            {
                ppdev->ajClutData[i + ulOffset].Blue  = (UCHAR) (fpRampVals[512 + i] >> 8); /* blue value */
                ppdev->ajClutData[i + ulOffset].Green = (UCHAR) (fpRampVals[256 + i] >> 8); /* green value */
                ppdev->ajClutData[i + ulOffset].Red   = (UCHAR) (fpRampVals[i] >> 8); /* red value */
            }
        }

        ppdev->pfnSetPalette(ppdev, (nStartIndex << 2) , (nNumEntries << 2) );
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/******************************Public*Routine******************************\
* ULONG nvGetGammaRamp
*
* DDI entry point for getting gamma ramp.
*
\**************************************************************************/
ULONG
nvGetGammaRamp(PDEV *ppdev, PUSHORT fpRampVals)
{

    unsigned short i;
    unsigned long nNumEntries = 256;

    if (!ppdev) return(FALSE);

    if(ppdev->iBitmapFormat != BMF_8BPP)
    {
        for (i = 0; i < nNumEntries; i++) 
        {
            fpRampVals[512 + i] = ((USHORT)ppdev->ajClutData[i].Blue << 8); // blue value
            fpRampVals[256 + i] = ((USHORT)ppdev->ajClutData[i].Green << 8);// green value
            fpRampVals[i]       = ((USHORT)ppdev->ajClutData[i].Red << 8);  // red value
        }
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}


#if _WIN32_WINNT >= 0x0500
/*****************************************************************************\
*   BOOL    DrvIcmSetDeviceGammaRamp
*
*   DDI entry point for setting gamma ramp.
*
*   iFormat == IGRF_RGB_256WORDS (always)
*   lpRamp == pointer to GAMMARAMP structure.
*   where       typedef struct _GAMMARAMP {
*                   WORD Red[256];
*                   WORD Green[256];
*                   WORD Blue[256];
*               } GAMMARAMP, *PGAMMARAMP;
*
\*****************************************************************************/

BOOL
DrvIcmSetDeviceGammaRamp(
    DHPDEV  dhpdev,
    ULONG   iFormat,
    LPVOID  lpRamp
)
{
    PPDEV           ppdev = (PPDEV)dhpdev;
    WORD *          pwGamma;
    ULONG i, ulHead, ulDevice, ulOffset;

    //
    //  Sanity check
    //

    if(iFormat == IGRF_RGB_256WORDS)
    {
        pwGamma = lpRamp;

        //
        //  Enable PIXMIX
        //
//        dwRet = EngDeviceIoControl(
//            ppdev->hDriver,
//            IOCTL_VIDEO_ENABLE_PIXMIX_INDEX,
//            NULL, 0,
//            NULL, 0,
//            &dwReturnedDataLength
//            );

        //
        //  Copy gamma ramp
        //
        for(ulDevice = 0; ulDevice < ppdev->ulNumberDacsActive; ulDevice++)
        {
            ulHead = ppdev->ulDeviceDisplay[ulDevice];
            ulOffset = ulHead * 256;

            for(i = 0; i < 256; i++)
            {
                ppdev->ajClutData[i + ulOffset].Red = (UCHAR)(pwGamma[i] >> 8);
                ppdev->ajClutData[i + ulOffset].Green = (UCHAR)(pwGamma[256 + i] >> 8);
                ppdev->ajClutData[i + ulOffset].Blue = (UCHAR)(pwGamma[(2*256) + i] >> 8);
                ppdev->ajClutData[i + ulOffset].Unused = 0;
            }
        }

        ppdev->pfnSetPalette(ppdev, 0, 256 * sizeof(VIDEO_CLUTDATA));

        return(TRUE);
    }
    
    return(FALSE);
}

#endif

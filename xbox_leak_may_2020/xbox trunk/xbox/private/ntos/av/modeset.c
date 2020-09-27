/*++

Copyright (c) Microsoft Corporation

Module Name:

    modeset.c

Abstract:

    Programs the CRTC to stream the frame buffer to the tv encoder.

--*/

#include "avp.h"
#include "xpcicfg.h"

#if DBG
ULONG AvpDump;
#endif

DECLSPEC_STICKY ULONG AvpCurrentMode;
ULONG AvpMacrovisionMode;
ULONG AvpCGMS;
ULONG AvpCapabilities;

//
// Address for the frame saved across reboots and mode changes
//
DECLSPEC_STICKY PVOID AvpSavedDataAddress;

// 
// Supported frame buffer modes.
//

#define D3DFMT_LIN_A1R5G5B5   0x00000010
#define D3DFMT_LIN_X1R5G5B5   0x0000001C
#define D3DFMT_LIN_R5G6B5     0x00000011
#define D3DFMT_LIN_A8R8G8B8   0x00000012
#define D3DFMT_LIN_X8R8G8B8   0x0000001E

ULONG
AvpCalcWSSCRC(
    IN  ULONG Value
    )
/*++

Routine Description:

    Sets various encoder options.

Arguments:

    Value - the 20 bit number to calculate CRC for

Return Value:

    The value plus the proper CRC

--*/
{
    ULONG CRC;    
    ULONG i, lsb, msb;

    CRC = 0x3F;    

    // This can ultimately be done more efficiently, but why?

    for (i = 0; i < 14; i++)
    {
        lsb = (CRC & 0x01) ^ ((Value >> i) & 0x01);
        msb = ((CRC >> 5) & 0x01) ^ lsb;

        CRC  = (CRC >> 1) & 0x0F;
        CRC |= msb << 4;
        CRC |= lsb << 5;
    }

    return Value | (CRC << 14);
}

VOID
AvpSetWSSBits(
    IN  PVOID RegisterBase
    )
/*++

Routine Description:

    Uses the current mode and macrovision state to correctly
    set the WSS and CGMS bits.

Arguments:

    RegisterBase - base address of the gpu hw registers

Return Value:

    None

--*/
{
    ULONG OutputMode;
    ULONG WSS;
    
    OutputMode = AvpCurrentMode & AV_MODE_OUT_MASK;

    WSS = 0;

    if (OutputMode == AV_MODE_OUT_480SDTV)
    {
        if (AvpCurrentMode & AV_MODE_FLAGS_WSS)
        {
            WSS |= 0x00000001;
        }

        WSS |= AvpCGMS << 6;
        WSS |= (AvpMacrovisionMode & 0x01) << 9;
        WSS |= (AvpMacrovisionMode & 0x02) << 7;

        WSS = AvpCalcWSSCRC(WSS);
    }

    // 525 should be 576
    else if (OutputMode == AV_MODE_OUT_525SDTV)
    {
        if (AvpCurrentMode & AV_MODE_FLAGS_WSS)
        {
            WSS |= 0x00000007;
        }
        else
        {
            WSS |= 0x00000008;
        }

        WSS |= AvpCGMS << 12;
    }

    // Do nothing.
    else
    {
        return;
    }

#ifdef FOCUS

    // Set the registers.
    SMB_WR(RegisterBase, 0x85, (UCHAR)((WSS & 0x0F)));
    SMB_WR(RegisterBase, 0x84, (UCHAR)((WSS >> 4) & 0xFF));
    SMB_WR(RegisterBase, 0x83, (UCHAR)((WSS >> 12) & 0xFF));

    SMB_WR(RegisterBase, 0x88, (UCHAR)((WSS & 0x0F)));
    SMB_WR(RegisterBase, 0x87, (UCHAR)((WSS >> 4) & 0xFF));
    SMB_WR(RegisterBase, 0x86, (UCHAR)((WSS >> 12) & 0xFF));

#else !FOCUS

    // Set the registers.
    SMB_WR(RegisterBase, 0x60, (UCHAR)(0xC0 | (WSS & 0x0F)));
    SMB_WR(RegisterBase, 0x62, (UCHAR)((WSS >> 4) & 0xFF));
    SMB_WR(RegisterBase, 0x64, (UCHAR)((WSS >> 12) & 0xFF));

#endif !FOCUS
}

VOID
AvpEnableSCART(
    IN  PVOID RegisterBase,
    IN  ULONG iTV
    )
/*++

Routine Description:

    Do all of the work to enable SCART, assuming that 
    it has already been set up in the mode-set code.

Arguments:

    RegisterBase - base address of the gpu hw registers

    iTV - tv encoder table index.

Return Value:

    None

--*/
{
#ifdef FOCUS

    const UCHAR *pByte, *pByteMax;
    ULONG i;
    UCHAR Data;    
    
    pByte    = AvpFocusRGBEnable[2];
    pByteMax = pByte + sizeof(AvpFocusRGBEnable[0]);

    for (i = 0; pByte < pByteMax; pByte++, i++)
    {
        Data = SMB_RD(RegisterBase, AvpFocusRGBEnable[0][i]);

        Data &= ~AvpFocusRGBEnable[1][i];

        SMB_WR(RegisterBase, AvpFocusRGBEnable[0][i], Data | *pByte);
    }

#else

    const UCHAR *pByte, *pByteMax;
    ULONG i;

    pByte    = AvpSCARTEnable[iTV];
    pByteMax = pByte + sizeof(AvpSCARTEnable[0]);

    for (i = 0; pByte < pByteMax; pByte++, i++)
    {
        SMB_WR(RegisterBase, AvpSCARTEnable[0][i], *pByte);
    }

#endif
}

//****************************************************************************
//
// Mode set code.
//
//****************************************************************************

ULONG
AvSetDisplayMode(
    IN  PVOID RegisterBase,
    IN  ULONG Step,
    IN  ULONG Mode,                   // mode from the AV_MODE list (avmode.h)
    IN  ULONG Format,                 // D3DFORMAT
    IN  ULONG Pitch,
    IN  ULONG FrameBuffer
    )
/*++

Routine Description:

    Initializes the CRTC to stream the framebuffer to the 
    tv encoder.

Arguments:

    RegisterBase - base address of the gpu hw registers

    Step - what step we're currently doing in the mode-set process

    Mode - the AV_PIPELINE mode to initialize

    Format - format of the frame buffer

    Pitch - the pitch of the frame buffer

    FrameBuffer - physical address of the frame buffer

Return Value:

    None

--*/
{
    ULONG GeneralControl;
    ULONG Value;
    ULONG i;
    ULONG LgsValue;

    UCHAR Data;
    UCHAR Register;

    const UCHAR *pByte;
    const UCHAR *pByteMax;

    const ULONG *pLong;
    const ULONG *pLongMax;

    UCHAR CR28Depth;
    UCHAR CR13Value;

    ULONG OutputMode;
    ULONG iRegister;
    ULONG iCRTC;
    ULONG iTV;

    UCHAR DACs;

    USHORT FocusValue;
    NTSTATUS Status;

    // 
    // Decode the mode.
    //

    // If the mode is zero then we just set this to NTSC and turn
    // the DACs off.
    //
    if (Mode == AV_MODE_OFF)
    {
        Mode = AV_MODE_640x480_TO_NTSC_M_YC
                | AV_MODE_FLAGS_DACA_DISABLE
                | AV_MODE_FLAGS_DACB_DISABLE
                | AV_MODE_FLAGS_DACC_DISABLE
                | AV_MODE_FLAGS_DACD_DISABLE;
    }

    OutputMode = Mode & AV_MODE_OUT_MASK;

    iRegister = (Mode & 0x00FF0000) >> 16;
    iCRTC     = (Mode & 0x0000FF00) >> 8;
    iTV       = (Mode & 0x0000007F);

    DACs      = (UCHAR)((Mode & 0x0F000000) >> 24);

    //
    // Get the format-specific parameters.
    //

    switch(Format)
    {
    case D3DFMT_LIN_A1R5G5B5:
    case D3DFMT_LIN_X1R5G5B5:
        GeneralControl = 0x00100030;
        CR28Depth = 2;
        break;

    case D3DFMT_LIN_R5G6B5:
        GeneralControl = 0x00101030;
        CR28Depth = 2;
        break;

    case D3DFMT_LIN_A8R8G8B8:
    case D3DFMT_LIN_X8R8G8B8:
        GeneralControl = 0x00100030;
        CR28Depth = 3;
        break;
    }

    // Pitch is always measured in bytes.
    Pitch /= 8;

    //
    // Handle format-only changes, very little has to be munged
    // for this case.
    //

    if (AvpCurrentMode == Mode)
    {
        ASSERT(Step == 0);

        // Reset the general control register.
        REG_WR32(RegisterBase, NV_PRAMDAC_GENERAL_CONTROL, GeneralControl);

        // Unlock the CRTC
        CRTC_WR(RegisterBase, NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE);

        // Set the pitch.
        CRTC_WR(RegisterBase, 0x13, (UCHAR)(Pitch & 0xFF));
        CRTC_WR(RegisterBase, 0x19, (UCHAR)((Pitch & 0x700) >> 3));

        // Slave the CRTC to the encoder with the proper color depth.
        CRTC_WR(RegisterBase, 0x28, 0x80 | CR28Depth);

        // Set the frame buffer pointer.
        REG_WR32(RegisterBase, NV_PCRTC_START, FrameBuffer);

        // Make sure the flicker filter and luma filter gets reset to their
        // default values.
        //
        AvSendTVEncoderOption(RegisterBase,
                              AV_OPTION_FLICKER_FILTER,
                              5,
                              NULL);

        AvSendTVEncoderOption(RegisterBase,
                              AV_OPTION_ENABLE_LUMA_FILTER,
                              FALSE,
                              NULL);

        AvpCurrentMode = Mode;
        return 0;
    }

    // 
    // Do a full mode change
    //

    else
    {
        ASSERT(Step == 0);

        // Use the hardware to disable the DACs.
        _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xD3, 0x5);

        //
        // Wait for any previous lightgun changes have been processed by the
        // hardware before making any other changes.
        //

        do
        {
            LgsValue = REG_RD32(RegisterBase, NV_PRAMDAC_LGS_TRANSFER);
        }
        while (!(LgsValue & 0x01) != !(LgsValue & 0x10));

        //
        // Unlock the CRTC and the tv encoder
        //

        CRTC_WR(RegisterBase, 0x1F, 0x57);
        CRTC_WR(RegisterBase, 0x21, 0xFF);

        // Unslave the CRTC from the tv encoder clock for now.
        CRTC_WR(RegisterBase, 0x28, 0x00);

        // Disable pclk stalling.
        REG_WR32(RegisterBase, NV_PRAMDAC_FP_DEBUG_0, 0x21121111);

#ifndef FOCUS

        // Do a reset and turn off dacs.
        SMB_WR(RegisterBase, 0xBA, 0x80);
        SMB_WR(RegisterBase, 0xBA, 0x3F);

        // Enable tv encoder register munging.
        SMB_WR(RegisterBase, 0x6C, 0x46);

#endif

        // Wait 1 microsecond
        KeStallExecutionProcessor(1);

#ifndef FOCUS

        // Reset the timing.
        SMB_WR(RegisterBase, 0x6C, 0xC6);

#endif

        // 
        // Program the TV encoder
        //

        // !!! PERF we can write runs of registers more efficiently.    
        if (OutputMode == AV_MODE_OUT_HDTV)
        {

            pByte    = AvpHDTVRegisters[iTV];
            pByteMax = pByte + sizeof(AvpHDTVRegisters[0]);

            for (i = 0; pByte < pByteMax; pByte++, i++)
            {
                SMB_WR(RegisterBase, AvpHDTVRegisters[0][i], *pByte);
            }
        }
        else
        {

#ifndef FOCUS

            pByte    = AvpCommonTVRegisters[1];
            pByteMax = pByte + sizeof(AvpCommonTVRegisters[0]);

            for (i = 0; pByte < pByteMax; pByte++, i++)
            {
                SMB_WR(RegisterBase, AvpCommonTVRegisters[0][i], *pByte);
            }

#endif !FOCUS

            pByte    = AvpTVRegisters[iTV];
            pByteMax = pByte + sizeof(AvpTVRegisters[0]);

            for (i = 0; pByte < pByteMax; pByte++, i++)
            {
                SMB_WR(RegisterBase, AvpTVRegisters[0][i], *pByte);
            }

            // Hack for NTSC-M and NTSC-J YPrPb outputs.
            if (XboxGameRegion == XC_GAME_REGION_JAPAN && (iTV == 0xD || iTV == 0xE || iTV == 0x12))
            {
#ifdef FOCUS

                SMB_WRW(RegisterBase, 0x4E, 0x013C);

#else !FOCUS

                SMB_WR(RegisterBase, 0xA2, 0x08);
                SMB_WR(RegisterBase, 0xA4, 0xF0);
                SMB_WR(RegisterBase, 0xAC, 0x9A);

#endif !FOCUS

            }
        }

        // Enable SCART
        if (Mode & AV_MODE_FLAGS_SCART)
        {
            // Twiddle the other registers.
            AvpEnableSCART(RegisterBase, iTV);
        }

        // Always set the SCART aspect ratio signal because it may be
        // handy to have for future AV packs.
        //
        _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xD6, 
              (Mode & AV_MODE_FLAGS_WSS) ? 0x5 : 0x4);

        // We are now allowed to always have RGB output on...so just
        // set this pin and leave it.
        //
        _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xD8, 0x4);

        //
        // Set up the RAMDAC to stream from the frame buffer.
        //

        // Set up the clocks to always use the tv-encoder as the 
        // source of the pclk and to divice pclk by 2 for the 
        // vclk.
        //
        REG_OR32(RegisterBase, NV_PRAMDAC_PLL_COEFF_SELECT, 0x10020000);

        // Unlock the VGA registers.
        REG_WR08(RegisterBase, NV_PRMVIO_VSE2, 0x01);

        // Set random output bits.
        REG_WR08(RegisterBase, NV_PRMVIO_MISC__WRITE, 0xE3);

        // Set up the general control register.  This varies depending on
        // the frame buffer format.
        //
        // The interesting bits:
        //
        //     5: 4 - 3 = PIXMIX bit is ignored
        //    12:12 - Selects between 5-5-5 (off) and 5-6-5 (on)
        //    16:16 - BLK_PEDSTL ON (??)
        //
        REG_WR32(RegisterBase, NV_PRAMDAC_GENERAL_CONTROL, GeneralControl);

        pLong    = AvpRegisters[iRegister];    
        pLongMax = pLong + sizeof(AvpRegisters[0]) / sizeof(ULONG);
    
        // Set the rest.
        for (i = 0; pLong < pLongMax; pLong++, i++)
        {
            REG_WR32(RegisterBase, AvpRegisters[0][i], *pLong);
        }

        // SCART shovels its output in RGB mode.
        if (Mode & AV_MODE_FLAGS_SCART)
        {
            REG_WR32(RegisterBase, 0x680630, 0);  
            REG_WR32(RegisterBase, 0x6808C4, 0);  
            REG_WR32(RegisterBase, 0x68084C, 0);  
        }

        //
        // Set the initial VGA registers.
        //

        // Write the SRX registers.
        pByte    = AvpSRXRegisters;
        pByteMax = pByte + sizeof(AvpSRXRegisters);

        for (i = 0; pByte < pByteMax; pByte++, i++)
        {
            SRX_WR(RegisterBase, (UCHAR)i, *pByte);
        }

        // Write the GRX registers
        pByte    = AvpGRXRegisters;
        pByteMax = pByte + sizeof(AvpGRXRegisters);

        for (i = 0; pByte < pByteMax; pByte++, i++)
        {
            GRX_WR(RegisterBase, (UCHAR)i, *pByte);
        }

        // Write the ARX registers.  This will cause the screen to blank
        // until the registers have all been written.
        //
        REG_RD08(RegisterBase, NV_PRMCIO_INP0__COLOR);

        pByte    = AvpARXRegisters;
        pByteMax = pByte + sizeof(AvpARXRegisters);

        for (i = 0; pByte < pByteMax; pByte++, i++)
        {
            ARX_WR(RegisterBase, (UCHAR)i, *pByte);
        }

        // Unblank the screen.
        REG_WR08(RegisterBase, NV_PRMCIO_ARX, 0x20); 

        //
        // Program the CRTC
        //

        // Unlock the rest of the CRTC.
        CRTC_WR(RegisterBase, 0x11, 0x00);

        pByte    = AvpCRTCRegisters[iCRTC];
        pByteMax = pByte + sizeof(AvpCRTCRegisters[0]);

        for (i = 0; pByte < pByteMax; pByte++, i++)
        {
            Register = AvpCRTCRegisters[0][i];
            Data = *pByte;

            if (Register == 0x13)
            {
                Data = (UCHAR)(Pitch & 0xFF);
            }
            else if (Register == 0x19)
            {
                Data |= (UCHAR)((Pitch & 0x700) >> 3);
            }
            else if (Register == 0x25)
            {
                Data |= (UCHAR)((Pitch & 0x800) >> 6);
            }

            CRTC_WR(RegisterBase, AvpCRTCRegisters[0][i], Data);
        }

        //
        // Commit the light gun changes.
        //

        REG_WR32(RegisterBase, NV_PRAMDAC_LGS_TRANSFER, !(LgsValue & 0x01) & 0x01);

        //
        // Sync up the CRTC with the tv encoder.
        //

        // Slave the CRTC to the encoder.
        CRTC_WR(RegisterBase, 0x28, 0x80 | CR28Depth);

        // Wait a few lines until everything gets in sync.
        for (i = 0; i < 3; i++)
        {
            while ((REG_RD32(RegisterBase, NV_PRMCIO_INP0__COLOR) & 0x08) == 0x08)
                ;

            while ((REG_RD32(RegisterBase, NV_PRMCIO_INP0__COLOR) & 0x08) == 0x00)
                ;
        }

        // Set FP_DEBUG_0 which will also enable pclk stalling.
        REG_WR32(RegisterBase, NV_PRAMDAC_FP_DEBUG_0, AvpFpDebug0[iRegister]);

        //
        // Afterglow
        //

        // Wait 1 microsecond
        KeStallExecutionProcessor(1);

#ifndef FOCUS

        // Reset the timing.
        Data = SMB_RD(RegisterBase, 0x6C);   
        SMB_WR(RegisterBase, 0x6C, Data | 0x80);

        // Enable the DACs.
        SMB_WR(RegisterBase, 0xBA, 0x20 | DACs);

#endif

        // Set the frame buffer pointer.
        REG_WR32(RegisterBase, NV_PCRTC_START, FrameBuffer);

        AvpCurrentMode = Mode;

        AvpSetWSSBits(RegisterBase);
                  
        return 0;
    }
}

VOID 
AvpSetMacrovisionMode(
    IN PVOID RegisterBase,
    IN ULONG MacrovisionMode
    )
/*++

Routine Description:

    Sets up macrovision.

Arguments:

    RegisterBase - base address of the gpu hw registers

    MacrovisionMode -

Return Value:

    None

--*/
{
    const UCHAR *Registers = NULL;
    UCHAR Tv;

    ULONG i,c ;
    ULONG OutputMode;
    UCHAR Data;
         
#ifdef FOCUS

    UCHAR CCEnable;

#endif

    Tv = (UCHAR)(AvpCurrentMode & 0xFF);
    OutputMode = AvpCurrentMode & AV_MODE_OUT_MASK;

    // 525 should be 576
    if (OutputMode != AV_MODE_OUT_480SDTV && OutputMode != AV_MODE_OUT_525SDTV)
    {
        // No macrovision.
        return;
    }

    if (OutputMode == AV_MODE_OUT_480SDTV)
    {
        switch(MacrovisionMode)
        {
        case AV_MV_OFF:
            Registers = AvpMacrovision[AVP_MV_NTSC_MODE_0];
            break;

        case AV_MV_AGC_ONLY:
            Registers = AvpMacrovision[AVP_MV_NTSC_MODE_1];
            break;

        case AV_MV_TWO_STRIPES_PLUS_AGC:
            Registers = AvpMacrovision[AVP_MV_NTSC_MODE_2];
            break;

        case AV_MV_FOUR_STRIPES_PLUS_AGC:
        default:
            Registers = AvpMacrovision[AVP_MV_NTSC_MODE_3];
            break;
        }

        // Sploich the registers.
        c = sizeof(AvpMacrovision[0]);

#ifdef FOCUS

        CCEnable = (SMB_RDW(RegisterBase, 0xB2) >> 8 ) & 0xC0;

#endif

        for (i = 0; i < c; i++)
        {

#ifdef FOCUS

            if (AvpMacrovision[0][i] == 0xB3)
            {
                SMB_WR(RegisterBase, AvpMacrovision[0][i], Registers[i] | CCEnable);
            }
            else

#endif

            {
                SMB_WR(RegisterBase, AvpMacrovision[0][i], Registers[i]);
            }
        }
    }

    else
    {
        switch(MacrovisionMode)
        {
        case AV_MV_OFF:
            Registers = AvpMacrovisionPAL[AVP_MV_PAL_MODE_0];
            break;

        case AV_MV_AGC_ONLY:
        default:
            Registers = AvpMacrovisionPAL[AVP_MV_PAL_MODE_1];
            break;
        }

        // Sploich the registers.
        c = sizeof(AvpMacrovisionPAL[0]);

        for (i = 0; i < c; i++)
        {
            SMB_WR(RegisterBase, AvpMacrovisionPAL[0][i], Registers[i]);
        }
    }
}

VOID 
AvpCommitMacrovisionMode(
    IN PVOID RegisterBase,
    IN ULONG MacrovisionMode
    )
/*++

Routine Description:

    Commits the macrovision changes.

Arguments:

    RegisterBase - base address of the gpu hw registers

    MacrovisionMode -

Return Value:

    None

--*/
{
    const UCHAR *Registers = NULL;
    UCHAR Tv;

    ULONG i,c ;
    ULONG OutputMode;
    USHORT Data;
        
    Tv = (UCHAR)(AvpCurrentMode & 0xFF);
    OutputMode = AvpCurrentMode & AV_MODE_OUT_MASK;

#ifdef FOCUS

    if (OutputMode == AV_MODE_OUT_480SDTV 
        || OutputMode == AV_MODE_OUT_525SDTV
        || (AvpCurrentMode & 0xFF0000FF) == 0x88000001)
    {
        Data = SMB_RDW(RegisterBase, 0x0E);

        if (MacrovisionMode == AV_MV_OFF)
        {
            SMB_WRW(RegisterBase, 0x0E, Data & ~0x8000);
        }
        else
        {
            SMB_WRW(RegisterBase, 0x0E, Data | 0x8000);
        }
    }

#else !FOCUS

    if (OutputMode == AV_MODE_OUT_480SDTV)
    {

        switch(MacrovisionMode)
        {
        case AV_MV_OFF:
            Registers = AvpMacrovisionEnable[AVP_MV_NTSC_MODE_0];
            break;

        case AV_MV_AGC_ONLY:
            Registers = AvpMacrovisionEnable[AVP_MV_NTSC_MODE_1];
            break;

        case AV_MV_TWO_STRIPES_PLUS_AGC:
            Registers = AvpMacrovisionEnable[AVP_MV_NTSC_MODE_2];
            break;

        case AV_MV_FOUR_STRIPES_PLUS_AGC:
        default:
            Registers = AvpMacrovisionEnable[AVP_MV_NTSC_MODE_3];
            break;
        }

        // Sploich the registers.
        c = sizeof(AvpMacrovisionEnable[0]);

        for (i = 0; i < c; i++)
        {
            SMB_WR(RegisterBase, AvpMacrovisionEnable[0][i], Registers[i]);
        }
    }

#endif !FOCUS

}

ULONG
AvSMCVideoModeToAVPack(
    ULONG VideoMode
    )
/*++

Routine Description:

    Converts an SMC video mode to an AV pack constant.

Arguments:

    VideoMode - Video mode as returned by SMC_COMMAND_VIDEO_MODE.

Return Value:

    Returns the AV pack constant.

--*/
{
    ULONG Result;

    switch(VideoMode)
    {
    case 0x0:

        // DVT-1 boards will always select this mode.  That should
        // be ok because only the composite output will be enabled
        // on an NTSC-M system.
        //
        Result = AV_PACK_SCART;
        break;

    case 0x1:
        Result = AV_PACK_HDTV;
        break;

    case 0x2:
        Result = AV_PACK_VGA;
        break;

    case 0x3:
        Result = AV_PACK_RFU;
        break;

    case 0x4:
        Result = AV_PACK_SVIDEO;
        break;

    case 0x6:
        Result = AV_PACK_STANDARD;
        break;

    default:
    case 0x7:
        Result = AV_PACK_NONE;
        break;
    }

    return Result;
}

ULONG
AvpQueryAvCapabilities(
    )
/*++

Routine Description:

    Reads the AV settings from the AV pack and EEPROM.

Arguments:

    None

Return Value:

    The AV caps flags.

--*/
{
    ULONG VideoMode;
    ULONG Type;
    ULONG ResultLength;

    NTSTATUS status;

    ULONG Result;

    Result = AvSMCVideoModeToAVPack(HalBootSMCVideoMode);

    // Get the region from the EEPROM.
    status = ExQueryNonVolatileSetting(XC_FACTORY_AV_REGION,
                                       &Type,
                                       &VideoMode,
                                       sizeof(VideoMode),
                                       &ResultLength);

    // Didn't work?  Default to NTSC.
    if (status != STATUS_SUCCESS || ResultLength != sizeof(VideoMode))
    {
        VideoMode = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
    }

    Result |= VideoMode & (AV_STANDARD_MASK | AV_REFRESH_MASK);

    // Get the user settings.
    status = ExQueryNonVolatileSetting(XC_VIDEO_FLAGS,
                                       &Type,
                                       &VideoMode,
                                       sizeof(VideoMode),
                                       &ResultLength);

    // Didn't work?  Default to no options.
    if (status != STATUS_SUCCESS || ResultLength != sizeof(VideoMode))
    {
        VideoMode = 0;
    }

    Result |= VideoMode & ~(AV_STANDARD_MASK | AV_PACK_MASK);

    return Result;
}

void
AvSendTVEncoderOption(
    IN  PVOID RegisterBase,
    IN  ULONG Option,
    IN  ULONG Param,
    OUT PULONG Result
    )
/*++

Routine Description:

    Sets various encoder options.

Arguments:

    RegisterBase - base address of the gpu hw registers

Return Value:

    None

--*/
{
    ULONG OutputMode;
    UCHAR Data, OldValue;
    ULONG iTV, i;
    USHORT WValue;

    const UCHAR *pByte;
    const UCHAR *pByteMax;

    OutputMode = AvpCurrentMode & AV_MODE_OUT_MASK;
    iTV       = (AvpCurrentMode & 0x0000007F);

    if (!RegisterBase)
    {
        RegisterBase = (void *)XPCICFG_GPU_MEMORY_REGISTER_BASE_0;
    }

    switch (Option) {

        case AV_OPTION_BLANK_SCREEN:

            REG_WR08(RegisterBase, NV_PRMVIO_SRX, 0x01);

            if (Param)
            {
                REG_WR08(RegisterBase, NV_PRMVIO_SR_RESET, 0x21);
            }
            else
            {
                REG_WR08(RegisterBase, NV_PRMVIO_SR_RESET, 0x01);

                // Renable the DACs.
                _outp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xD3, 0x4);
            }
            break;

        case AV_OPTION_MACROVISION_MODE:

            AvpSetMacrovisionMode(RegisterBase, Param);
            AvpMacrovisionMode = Param;
            break;

        case AV_OPTION_MACROVISION_COMMIT:
                
            AvpCommitMacrovisionMode(RegisterBase, Param);

            AvpSetWSSBits(RegisterBase);

            break;
        
        case AV_OPTION_ENABLE_CC:

            if (OutputMode == AV_MODE_OUT_480SDTV)
            {

#ifdef FOCUS

                WValue = SMB_RDW(RegisterBase, 0xB2);
                SMB_WRW(RegisterBase, 0xB2, WValue | 0xC000);

#else !FOCUS
                                                                                                                                                                                                
                Data = SMB_RD(RegisterBase, 0xC4);
                SMB_WR(RegisterBase, 0xC4, Data | 0x38);

                pByte    = AvpCCRegisters[iTV];
                pByteMax = pByte + sizeof(AvpCCRegisters[0]);

                for (i = 0; pByte < pByteMax; pByte++, i++)
                {
                    SMB_WR(RegisterBase, AvpCCRegisters[0][i], *pByte);
                }

#endif !FOCUS

            }
            break;

        case AV_OPTION_DISABLE_CC:

            if (OutputMode == AV_MODE_OUT_480SDTV)
            {

#ifdef FOCUS
                
                WValue = SMB_RDW(RegisterBase, 0xB2);
                SMB_WRW(RegisterBase, 0xB2, WValue & 0x3FFF);

#else !FOCUS
                UCHAR Data = SMB_RD(RegisterBase, 0xC4);

                // Clear ECC enable bits.
                SMB_WR(RegisterBase, 0xC4, Data & ~0x30);

#endif !FOCUS

            }
            break;

        case AV_OPTION_SEND_CC_DATA:

            if (OutputMode == AV_MODE_OUT_480SDTV)
            {

#ifdef FOCUS

                USHORT Value;

                Value  = (USHORT)(*Result & 0xFF);
                Value |= (USHORT)((*Result >> 16) & 0xFF) << 8;

                if (Param & 1)
                {
                    SMB_WRW(RegisterBase, 0xAE, Value);
                }
                else
                {
                    SMB_WRW(RegisterBase, 0xB0, Value);
                }

#else !FOCUS

                if (Param & 1)
                {
                    SMB_WR(RegisterBase, 0xC0, (UCHAR)(*Result >> 16));
                    SMB_WR(RegisterBase, 0xC2, (UCHAR)*Result);
                }
                else
                {
                    SMB_WR(RegisterBase, 0xBC, (UCHAR)(*Result >> 16));
                    SMB_WR(RegisterBase, 0xBE, (UCHAR)*Result);
                }

#endif !FOCUS

            }
            break;

        case AV_QUERY_CC_STATUS:

            if (OutputMode == AV_MODE_OUT_480SDTV)
            {
#ifdef FOCUS

                *Result = 0;

                if (SMB_RDW(RegisterBase, 0xAE))
                {
                    *Result |= 0x1;
                }

                if (SMB_RDW(RegisterBase, 0xB0))
                {
                    *Result |= 0x2;
                }

#else !FOCUS

                *Result = (SMB_RD(RegisterBase, 0x02) >> 2) & 3;

#endif !FOCUS

            }
            else
            {
                *Result = 0;
            }

            break;

        case AV_QUERY_AV_CAPABILITIES:

            AvpCapabilities = AvpQueryAvCapabilities();
            *Result = AvpCapabilities;
            break;

        case AV_OPTION_FLICKER_FILTER:

#ifdef FOCUS

            SMB_WR(RegisterBase, 0x26, (UCHAR)(Param * 16 / 5));
            SMB_WR(RegisterBase, 0x27, 0);

#else !FOCUS

            if (Param == 0)
            {
                // Turn the flicker filter off.
                Data = SMB_RD(RegisterBase, 0xC8);

                SMB_WR(RegisterBase, 0xC8, Data | 0x40);
            }
            else
            {
                switch(Param)
                {
                case 1:
                    Data = 0x01;
                    break;

                case 2:
                    Data = 0x02;
                    break;

                case 3:
                    Data = 0x03;
                    break;

                case 4:
                default:
                    Data = 0x00;
                    break;
                }

                // Turn the flicker filter on.
                OldValue = SMB_RD(RegisterBase, 0xC8);

                // Clear out the old values and clear the disable bit.
                OldValue &= 0x80;

                // Set the new values.
                OldValue |= Data | (Data << 3);

                SMB_WR(RegisterBase, 0xC8, OldValue);

                // Munge the adapate FF.
                SMB_WR(RegisterBase, 0x34, Param == 5 ? 0x80 : 0x00);
            }

#endif !FOCUS

            break;

        case AV_OPTION_ZERO_MODE:

            AvpCurrentMode = 0;
            break;

        case AV_OPTION_QUERY_MODE:

            *Result = AvpCurrentMode;
            break;

        case AV_OPTION_ENABLE_LUMA_FILTER:

#ifndef FOCUS

            Data = SMB_RD(RegisterBase, 0x96) & 0x0F;

            if (Param)
            {
                Data |= 0x10;
            }

            SMB_WR(RegisterBase, 0x96, Data);

#endif

            break;

        case AV_OPTION_GUESS_FIELD:

#ifndef FOCUS

            *Result = SMB_RD(RegisterBase, 0x06) & 0x01;
                                
#endif

            break;

        case AV_QUERY_ENCODER_TYPE:

#ifdef FOCUS
            *Result = AV_ENCODER_FOCUS;
#else
            *Result = AV_ENCODER_CONEXANT_871;
#endif

            break;

        case AV_QUERY_MODE_TABLE_VERSION:

            *Result = AV_MODE_TABLE_VERSION;
            break;

        case AV_OPTION_CGMS:
            
            AvpCGMS = Param;

            AvpSetWSSBits(RegisterBase);

            break;

        case AV_OPTION_WIDESCREEN:

            AvpCurrentMode = (AvpCurrentMode & ~AV_MODE_FLAGS_WSS);

            if (Param)
            {
                AvpCurrentMode |= AV_MODE_FLAGS_WSS;
            }

            AvpSetWSSBits(RegisterBase);

            break;

        default:
            ;
    }
}


PVOID
AvGetSavedDataAddress(
    VOID
    )
/*++

Routine Description:

    Returns sticky data address saved acress quick reboots.

Arguments:

    None.

Return Value:

    Saved global address.

--*/
{
    return AvpSavedDataAddress;
}

VOID
AvSetSavedDataAddress(
    PVOID Address
    )
/*++

Routine Description:

    Sets sticky data address saved acress quick reboots.

Arguments:

    The address to save.

Return Value:

    None.

--*/
{
    AvpSavedDataAddress = Address;
}

VOID
AvRelocateSavedDataAddress(
    IN PVOID NewAddress,
    IN SIZE_T NumberOfBytes
    )
/*++

Routine Description:

    Sets sticky data address saved acress quick reboots.

Arguments:

    The address to save.

Return Value:

    None.

--*/
{
    D3DSurface *OldSurface;
    D3DSurface *NewSurface;

    ASSERT(AvpSavedDataAddress != NULL);

    OldSurface = (D3DSurface*)AvpSavedDataAddress;
    NewSurface = (D3DSurface*)NewAddress;

    //
    // Copy the bits from the the old surface to the new surface, including all
    // of the headers.
    //

    RtlCopyMemory(NewSurface, OldSurface, NumberOfBytes);

    //
    // Update the physical address of the surface data stored in the surface
    // header.
    //

    NewSurface->Data = MmGetPhysicalAddress(NewSurface) +
        (OldSurface->Data - MmGetPhysicalAddress(OldSurface));

    //
    // Wait for the write combine buffers to flush.
    //

    __asm sfence;

    //
    // Program the NV2A to scan from the new surface.
    //

    REG_WR32((PVOID)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PCRTC_START,
        NewSurface->Data);

    //
    // Reset the VBLANK pending flag.
    //

    REG_WR32((PVOID)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PCRTC_INTR_0,
        NV_PCRTC_INTR_0_VBLANK_RESET);

    //
    // Persist the new surface and update the saved data address to point at
    // this surface.
    //

    MmPersistContiguousMemory(NewSurface, NumberOfBytes, TRUE);
    AvpSavedDataAddress = NewSurface;

    //
    // Before freeing the old surface, loop until a VBLANK has occurred.  The
    // NV2A begins using the new surface at the next VBLANK.
    //

    while ((REG_RD32((PVOID)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PCRTC_INTR_0) &
        NV_PCRTC_INTR_0_VBLANK_PENDING) == 0);

    //
    // Free the memory used for the old surface.
    //

    MmFreeContiguousMemory(OldSurface);
}

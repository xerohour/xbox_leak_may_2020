//
// modeset.c -  Program to set a VESA mode. Assumes the controller is
//              already initialized.
//
// Copyright (c) 1997-2000 - NVidia Corporation. All Rights Reserved.
//
#ifdef DOS_COMPILE
#include    <stdio.h>
#include    <stdlib.h>
#else
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <os.h>
#include <vga.h>
#include <i2c.h>
#endif // DOS_COMPILE
                
#include    "modes.h"
#include    "modedata.c"
#include	"modehw.h"
#include "nvhw.h"
#include "dac.h"
#include "nvcm.h"

#define ERR_I2C_MODE    1
#define ERR_I2C_COMM    2
#define ERR_I2C_UNKNOWN 3


U016 setI2C(MODEDESC *);
VOID ProgramTV(	U008 tvmode );
U016 programBrooktree(U016 TVmode);
U016 programChrontel(U016 TVmode);
VOID SetDMTOverrideValues(PDMT_OVERRIDES pDMTOverrides);
VOID output(U016, U008);
U008 input(U016);
VOID DDCout(U008);
U008 DDCin();
VOID FlatPanelCrtc();
U008 getDisplayType();
// U032 GetDefaultPosition();
// VOID SaveDefaultPosition();

//JJV-New Registry Process extern char strDevNodeDisplay[];
//JJV-New Registry Process extern char strDisplayType[];
//JJV-New Registry Process extern char strTVtype[];

//U008        DisplayType;
#ifndef PAL // just in case you want to default to PAL
U008        TVformat = NTSC;    
#else
U008        TVformat = PAL;
#endif

//
// VBE Function 02h - Vesa SetMode routine.
//
// Used to set a VESA mode. This routine will not accept standard
// VGA Mode numbers, nor will it load/support fonts. This means that
// TTY output is DISABLED.
//
// Entry:   U016 mode             // VESA mode to be set
//
//          +------------------ Don't clear screen (1=Save Screen)
//          |+----------------- Linear Frame Buffer (1=Enable)
//          ||++--------------- Reserved for VBE/AF 
//          ||||+-------------- User Refresh (1=User Specified)
//          |||||++------------ Reserved
//          |||||||+-++++++++-- Mode Number
//          VVVVVVVV VVVVVVVV
//          00000000 00000000
//
//          111111           
//          54321098 76543210
//
//          CRTCInfoBlk *cib        // Pointer to CRTCInfoBlock Structure
//
// Exit:    U016 rc               // VESA Error Status
//                                                  

U016 VBESetMode(U016 mode, PGTF_TIMINGS timings, PDMT_OVERRIDES pDMTOverrides, U016 UseDmtFlag)
{
    MODEDESC    *minfo;             // Mode Information
    U016        wv;
    U016        rc = 0x014F;        // Default to FAIL
    U016        status;
    U016        shadow, data;
    U032        data32;
    U008        data8;

    // initDac has set the pDev variable to the correct display type based on what the bios says.
    // If it is TV, setup the format.
    if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_TV)
    {
        mode &= ~0x800;     // do not use custom refresh rates
        if(pDev->Framebuffer.MonitorType == MONITOR_TYPE_PAL)
        {
            TVformat = PAL;
        }
        else       
        {
            TVformat = NTSC;
        }
    }        

    // check for registry override for TV type (PAL/NTSC)
    if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strTVtype, &data32) == RM_OK)
    {
        if (data32 == 0)
            TVformat = NTSC;
        if (data32 == 1)
        {
            TVformat = PAL;
            pDev->Framebuffer.MonitorType = MONITOR_TYPE_PAL;   // tell display driver what type of display
        }
    }
    // check for registry override for display type (TV/MONITOR/BOTH)
    if (osReadRegistryDword(pDev->Registry.DBstrDevNodeDisplay, strDisplayType, &data32) == RM_OK)
    {
        switch (data32)
        {
            case DISPLAY_TYPE_MONITOR:
                pDev->Framebuffer.DisplayType = DISPLAY_TYPE_MONITOR;
                // we're overriding, so change to monitor timing. The following code assumes this is set correctly.
                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data8);
                data8 &= 0x3f;
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data8);
                pDev->Framebuffer.MonitorType = MONITOR_TYPE_VGA;   // tell display driver what type of display
                break;
            case DISPLAY_TYPE_TV:
                mode &= ~0x800;     // do not use custom refresh rates
                pDev->Framebuffer.DisplayType = DISPLAY_TYPE_TV;
                // we're overriding, so change to TV timing. The following code assumes this is set correctly.
                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data8);
                if (TVformat == NTSC)
                {
                    data8 |= 0x80; 
                    pDev->Framebuffer.MonitorType = MONITOR_TYPE_NTSC;   // tell display driver what type of display
                }
                else
                {
//                    data8 |= 0xc0; // PAL                   
                    data8 |= 0x80; // not in NV4, bit 6 is overscan color
                    pDev->Framebuffer.MonitorType = MONITOR_TYPE_PAL;   // tell display driver what type of display
                }
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data8);
                break;
            case DISPLAY_TYPE_DUALSURFACE:
                pDev->Framebuffer.DisplayType = DISPLAY_TYPE_DUALSURFACE;
                break;
            case DISPLAY_TYPE_FLAT_PANEL:        
                pDev->Framebuffer.DisplayType = DISPLAY_TYPE_FLAT_PANEL;
                pDev->Framebuffer.MonitorType = MONITOR_TYPE_FLAT_PANEL;   // tell display driver what type of display
                break;
            default:
                pDev->Framebuffer.DisplayType = DISPLAY_TYPE_MONITOR;
        }
    }
    // check whether monitor is a flat panel
    if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_FLAT_PANEL)
    {
             // de-couple vertical sync from flat panel while setting mode
            data32 = REG_RD32(NV_PRAMDAC_FP_DEBUG_0); // usual macro won't help us here
            data32 |= (NV_PRAMDAC_FP_DEBUG_0_TEST_NEWPIX << 16);   // set VCLK bits
            REG_WR32(NV_PRAMDAC_FP_DEBUG_0, data32);
    }
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "Setting mode ",mode);

    OEMEnableExtensions();          // Unlock the registers
    shadow = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x21);        // read shadow lock (is there really no define for this reg?)

    data = shadow | 0xC800;         // Enable loading of CRTC's: bit 7 Horiz shadow, bit 3 Vert shadow
                                    // bit 6 = scanline doubling shadow
    WriteIndexed(NV_PRMCIO_CRX__COLOR, data);

    if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_FLAT_PANEL)
    {
        // bug workaround--disable scanline doubling and then shadow it. This allows writes to CR09 to affect the CRTC,
        //   but not the flat panel controller (else the flat panel controller may quadruple the scanline count).
        data = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x09);         // read scanline double
        WriteIndexed(NV_PRMCIO_CRX__COLOR, (data &= ~0x8000));   // clear scanline double bit

        // Wait one vsync before shadowing the scanline double bit
        while (REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x08);     // wait for active display
        while (!(REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x08));  // wait for retrace start
    
        // shadow = CR21
        shadow &= ~0x4000;  // shadow the scanline double bit
        WriteIndexed(NV_PRMCIO_CRX__COLOR, (shadow |= 0x8800)); // allow horiz & vert write-thru
    }    
    if ((minfo = ValidateMode((U016)(mode & 0x1FF))) != NULL)
    {                                
        // If we are requested to do DualSurface, check that the bit depth is 16.
        if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_DUALSURFACE)
            if (minfo->mdBPP != 16)
                pDev->Framebuffer.DisplayType = DISPLAY_TYPE_MONITOR;  // if not 16 bpp, we can't do it, so use monitor only
        // Program the TV encoder thru the I2C port.
        status = setI2C(minfo);  // Set up encoder. 
        if (status)
        {
            switch (status)
            {
                case ERR_I2C_MODE:
                    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "Can't program the TV encoder for mode ",mode);
                    break;
                case ERR_I2C_COMM:
                    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "I2C error programming mode ",mode);
                    break;
                case ERR_I2C_UNKNOWN:
                    DBG_PRINT_STRING (DEBUGLEVEL_WARNINGS, "NVRM: I2C error. Unknown encoder\n");
                    break;
            }                
            if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_TV)      // if flat panel, leave it that way.
                pDev->Framebuffer.DisplayType = DISPLAY_TYPE_MONITOR;  // I2C failed, set up for monitor
        }
        // Mode is maintained here, for the linear/regen flags
        // minfo now points to everything we need to program the mode.
        
        // Set the mode
        ATCOff();                   // Screen off
        OEMPreSetRegs();            // Set up DAC for programming palette.
        
        SetRegs(minfo);             // Set the VGA Registers
                                    // Always set the same packed pixel mode 
                                    // (the CRTC's will be overwritten for the particular mode later)
        FullCpuOn();
        
        OEMSetRegs(minfo);          // Set the OEM Registers
                                    //   if TV, use TV table to adjust timings).
        LoadPalette(minfo);
        
        DualSurfaceDesktop((U016)pDev->Framebuffer.DisplayType);    // Setup the DAC scan out and possibly video scalar.

        // Mode now successfully set
        // Use DMT override values instead of GTF timings
        if (UseDmtFlag && (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_MONITOR))
            {
            // Mode now successfully set .. Set DMT override values
            SetDMTOverrideValues(pDMTOverrides);            
            }
            
        else
            {
            
            // Mode now successfully set .. Set GTF timings
            if ((mode & 0x0800) && timings && (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_MONITOR))  // See if bit 11 is set for VBE Refresh
              VBESetRefresh(timings);        // Set the VBE 3.0 Refresh
            }
              
        FullCpuOff();
        ATCOn();

        // Save the retrace start/end values (display position default)
        dacSaveMonitorDefaultPosition(pDev);

        // See if there are any registry overrides for the desktop positioning.
        if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_MONITOR)
        {
            U032 cbLen;
            NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_PARAMS position;

            cbLen = 4 * sizeof(U032);
            status = dacReadDesktopPositionFromRegistry(pDev, 
                                                        TRUE,  // isMonitor
                                                        (U008 *) &(position.HRetraceStart),
                                                        &cbLen);
            if (status == RM_OK)
            {
                // Program the new values.
                dacSetMonitorPosition(pDev, 
                                      position.HRetraceStart, position.HRetraceEnd,
                                      position.VRetraceStart, position.VRetraceEnd);
            }
        }
        // See if there are any registry overrides for the TV desktop positioning.
        // We currently only have adjustments for Brooktree.
        if ((pDev->Framebuffer.DisplayType == DISPLAY_TYPE_TV) 
        && (TV_ENCODER_FAMILY(pDev->Framebuffer.EncoderType) == TV_ENCODER_BROOKTREE))
        {
            U032 cbLen;
            NV_CFGEX_DESKTOP_POSITION_TV_PARAMS position;

            cbLen = 4 * sizeof(NV_CFGEX_DESKTOP_POSITION_TV_PARAMS);
            status = dacReadDesktopPositionFromRegistry(pDev, 
                                                        FALSE,  // isMonitor
                                                        (U008 *) &(position.HRetraceStart),
                                                        &cbLen);
            if (status == RM_OK)
            {
                // Program the new values.
                dacSetTVPosition(pDev, position.HRetraceStart, position.HRetraceEnd, position.VRetraceStart, 
                        position.VRetraceEnd, position.reg0x80, position.reg0x82, position.reg0x92, position.reg0x98, 
                        position.reg0x9A, position.reg0xC8, position.reg0xCA, position.reg0xCC);
            }
        }
        // Set the default mode if the display type is flat panel.
        if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_FLAT_PANEL)
        {
            U032 fpMode, fpMaxX, fpMaxY;
            status = dacGetFlatPanelInfo(pDev, &fpMode, &fpMaxX, &fpMaxY, 
                                         TRUE /* from registry */);
            if (status == RM_OK)
            {
                dacSetFlatPanelMode(pDev, fpMode, FALSE /* don't commit changes */);
            }
        }
                
        // Clear the DPMS State
        wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
        wv &= 0x3FFF;                   // Strip to standard DAC settings
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);

        // blank monitor if TV
        if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_TV)
            FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF);
        else
            FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON);

        rc &= 0x00FF;               // Clear error status
    }                                   
    if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_TV)
    {
        shadow &= ~0x8000;          // shadow Horiz only
        WriteIndexed(NV_PRMCIO_CRX__COLOR, shadow);     // restore the shadow lock
    }
    OEMDisableExtensions();         // Lock the registers
    
    return(rc);                     // Mode Not Supported
}          

//
// Program the CRTC timing values given by the GTF parameters
//
// WARNING: This is not going to be pretty!!
// TODO: Clean the code to be more straightforward.
//
U016 VBESetRefresh(PGTF_TIMINGS timings)
{

    U032 val32;
    U016 val16, val16b, val16c;
    U008 val08;
    U016 pixperclk;
    U016 border;
    U008 ov07, ov25, ov2D;
    U016 horiz_total;
    U016 hbe;


    //
    // Program the new clocks
    //
    OEMSetClock((U016)(timings->dot_clock / 10000));

    /*
        ; Program new horizontal sync polarity
        mov     bl, [edi].CRTCInfoBlock.HorizontalSyncPolarity
        cmp     bl, '-'
        jne     @f
        or      al, 040h
@@:
        ; Program new vertical sync polarity
        mov     bl, [edi].CRTCInfoBlock.VerticalSyncPolarity
        cmp     bl, '-'
        jne     @f
        or      al, 080h
@@:
        mov     dx, MISC_OUTPUT
        LOG_GO  dx, al
    */
    //
    // Program the sync polarities
    //
    val32 = ReadPriv32(NV_PRMVIO_MISC__READ) & 0x3F;
    if (timings->flags & GTF_FLAGS_HSYNC_NEGATIVE)
        val32 |= 0x40;
    if (timings->flags & GTF_FLAGS_VSYNC_NEGATIVE)
        val32 |= 0x80;
    WritePriv08(NV_PRMVIO_MISC__WRITE, (U008)val32);

    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: PRMVIO_MISC=",(U008)val32);

    /*
        ;---------------------------------------------------------
        ; Compute CRTC settings
        call    PixPerCharClk
        mov     si, ax
    */
    // 
    // Get the pixels per char settings
    //
    if (ReadIndexed(NV_PRMVIO_SRX, NV_PRMVIO_SR_CLOCK_INDEX) & 0x0100)
        pixperclk = 8;
    else
        pixperclk = 9;    

    /*
        ; Another way to compute vertical border would be to
        ; subtract current CR12 (VDE[0:7]) from CR15 (VBS[0:7])
        call    GetCRTCAddr
        mov     al, 012h
        call    GetIndexRegister        ; VDE[7:0]
        mov     bh, ah
        mov     al, 015h
        call    GetIndexRegister        ; VBS[7:0]
        sub     ah, bh
        ;add    ah, 1-1                 ; VGA adjustments cancel
        xor     al, al
        or      ah, ah
        jz      @f
        inc     al                      ; 1 character horizontal
@@:
        mov     bx, [edi].CRTCInfoBlock.RefreshRate
        cmp     bx, 7200
        jle     @f
        xor     ax,ax                   ; No boarder for refresh above 72Hz
@@:
        mov     bp, ax                  ; Save boarders in bp
    */
    //
    // Get the border setting
    //
    border = (U016)ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x15) & 0xFF00;
    border -= (U016)ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x12) & 0xFF00;
    border &= 0xFF00;
    if (border & 0xFF00)
        border++;
    if (timings->refresh > 7200)
        border = 0;
    
    /*        
        ;---------------------------------------------------------
        ; Store CRTC settings
        ; Get overflow values to accumulate
        mov     al, 007h
        call    GetIndexRegister
        mov     ch, ah                  ; Save off overflow register
        and     ch, 01010010b           ; Mask bits to save
        mov     al, 025h
        call    GetIndexRegister
        mov     cl, ah                  ; Save off extra bits screen register
        and     cl, 11100010b           ; Mask bits to save
IFDEF NVIDIA_SPECIFIC
        ; Load original 02Dh
        mov     al,02Dh
        call    GetIndexRegister
        and     ah,0E2h         ; Save unused bits and HDE
        rol     ecx,16          ; switch to upper part of ECX
        mov     cl,ah
        ror     ecx,16          ; switch back to lower part of ECX
ENDIF ; NVIDIA_SPECIFIC
    */
    //
    // Get the overflow bits
    //
    ov07 = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x07) >> 8;
    ov07 &= 0x52;
    ov25 = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x25) >> 8;
    ov25 &= 0xE2;
    ov2D = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x2D) >> 8;
    ov2D &= 0xE2;
    
    /*
        ; cl = CRTC[25] overflow
        ; ch = CRTC[07] overflow
        ; upper cl = CRTC[2D] overflow
        ; dx = CRTC I/O address
        ; si = pixels per character
        ; es:di = pointer to CRTCInfoBlock
        ; ds = seg0
        ; bp = boarder
        mov     ax, [edi].CRTCInfoBlock.HorizontalTotal
        push    dx
        xor     dx, dx
        div     si
        pop     dx
        mov     bx, ax                  ; Save original
        sub     ax, 5                   ; VGA HT adjustment
IFDEF NVIDIA_SPECIFIC
        ; Do bit 8 of HT
        and     ah,1            ; mask out bit 8
        ;shl     ah,0-0          ; already in position 0
        rol     ecx,16          ; switch to upper part of ECX
        or      cl,ah
        ror     ecx,16          ; switch back to lower part of ECX
ENDIF ; NVIDIA_SPECIFIC
        ;
        ; CRTC[00]
        mov     ah, al
        mov     al, 000h
        LOG_GO  dx, ax
    */
    //
    // Calculate and program horizontal total (CR00)
    //
    horiz_total = timings->horiz_total / pixperclk;
    val16 = horiz_total - 5;
    val16 &= 0x01FF;
    ov2D |= (U008)(val16 >> 8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)((val16 << 8) & 0xFF00));
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR00=",(U008)val16);
    
    /*            
IFDEF NVIDIA_SPECIFIC
        ; CRTC[39]
        ; Note, we are assuming that the 9th bit of interlace half field start is always 0
        mov     ax, bx
        shr     ax, 1           ; Compute horizontal total divided by 2
        mov     ah, [edi].CRTCInfoBlock.Flags
        and     ah,00000010b    ; ?Interlaced mode
        jnz     @f              ; Y: Leave ah at 1/2 HT
        mov     al, 0FFh        ; N: Disable interlace
@@:
        mov     ah, al
        mov     al, 039h
        SIM_NO  dx, ax
ENDIF ; NVIDIA_SPECIFIC
    */
    //
    // Program interlace half-field value (CR39)
    //
    if (timings->flags & GTF_FLAGS_INTERLACED)
        val16 = (((horiz_total >> 1) << 8) & 0xFF00) | 0x39;
    else
        val16 = 0xFF39;   // disabled
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR39=",(U008)(val16>>8));
    
    /*            
        mov     ax, bp
        xor     ah, ah                  ; Make into word
        sub     bx, ax                  ; Subtract boarder from HT
        ; CRTC[03]
        mov     al, 003h
        call    GetIndexRegister
        and     ah, 11100000b
        dec     bl                      ; VGA HBE Adjustment
        mov     bh, bl                  ; save HBE
        and     bl, 00011111b           ; HBE[4:0]
        or      ah, bl
        LOG_GO  dx, ax
    */
    //
    // Program horizontal blanking end (CR03)
    //
    val16 = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x03);
    val16 &= 0xE0FF;
    hbe = horiz_total - (border & 0xFF) - 1;
    val16 |= ((hbe & 0x1F) << 8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR03=",(U008)(val16>>8));
    
    /*        
        ; CRTC[05]
        mov     ax, [edi].CRTCInfoBlock.HorizontalSyncEnd
        push    dx
        xor     dx, dx
        div     si
        pop     dx
        mov     bl, al
        mov     al, 028h                ; See if we are in native mode or vga mode
IFDEF NVIDIA_SPECIFIC
        call    GetIndexRegister
        and     ah, 003h
ENDIF ; NVIDIA_SPECIFIC
        .if (zero?)
                inc     bl              ; VGA HSE adjustment is +2
        .endif
        inc     bl                      ; Native HSE adjustment is +1
        mov     al, 005h
        call    GetIndexRegister
        and     ah, 01100000b
        and     bl, 00011111b           ; HSE[4:0]
        or      ah, bl
        mov     bl, bh                  ; restore HBE
        and     bl, 00100000b           ; HBE[5]
        shl     bl, 7-5                 ; Move from bit 5 to bit 7
        or      ah, bl
        LOG_GO  dx, ax
    */
    //
    // Program horizontal retrace end (CR05)
    //
    val16 = timings->horiz_end / pixperclk;
    val16++;    // always native mode
    val16 &= 0x1F;
    val16b = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x05);
    val16b &= 0x60FF;
    val16b |= (val16 << 8);
    val16 = (hbe & 0x20) << 2;
    val16b |= (val16 << 8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16b);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR05=",(U008)(val16b>>8));
    
    /*        
IFDEF NVIDIA_SPECIFIC
        and     bh, 01000000b
        shr     bh, 6-4
        ; MSK - Assume HBE[6] can always be set, but only takes effect if LC ignore flag is set
        or      cl, bh                  ; HBE[6]
ENDIF ; NVIDIA_SPECIFIC
    */
    //
    // Modify overflow25
    //
    ov25 |= ((hbe & 0x40) >> 2);
    
    /*
        ; CRTC[02]
        mov     al, 001h
        call    GetIndexRegister        ; HDE[0:7]
        mov     al, ah
IFDEF NVIDIA_SPECIFIC
        ; Get bit 8 of HDE
        rol     ecx,16          ; switch to upper part of ECX
        mov     ah, cl
        ror     ecx,16          ; switch back to lower part of ECX
        and     ah, 2           ; mask out bit 1
        shr     ah, 1-0         ; shift into position 0
ELSE ; NVIDIA_SPECIFIC
        xor     ah, ah
ENDIF ; NVIDIA_SPECIFIC
        ;
        inc     ax                      ; VGA HDE adjustment
        mov     bx, bp
        xor     bh, bh
        add     ax, bx
        dec     ax                      ; VGA HBS Adjustment
IFDEF NVIDIA_SPECIFIC
        ; Store bit 8 of HBS
        and     ah,1            ; mask out bit 8
        shl     ah,2-0          ; shift into position 2
        rol     ecx,16          ; switch to upper part of ECX
        or      cl,ah
        ror     ecx,16          ; switch back to lower part of ECX
ENDIF ; NVIDIA_SPECIFIC
        ;
        mov     ah, al
        mov     al, 002h
        LOG_GO  dx, ax
    */
    //
    // Program horizontal blanking start (CR02)
    //
    val16 = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x01) >> 8) & 0x00FF;
    val16 |= ((U016)(ov2D & 2) << 7) & 0xFF00;
    val16++;
    val16 += (border & 0xFF);
    val16--;
    val08 = ((val16 >> 8) & 1) << 2;
    ov2D |= (U016)val08;
    val16 = ((val16 << 8) & 0xFF00) | 0x02;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR02=",(U008)(val16>>8));
    
    /*
        ; CRTC[04]
        mov     ax, [edi].CRTCInfoBlock.HorizontalSyncStart
        push    dx
        xor     dx, dx
        div     si
        pop     dx
        mov     bx, ax
        mov     al, 028h                ; See if we are in native mode or vga mode
IFDEF NVIDIA_SPECIFIC
        call    GetIndexRegister
        and     ah, 003h
ENDIF ; NVIDIA_SPECIFIC
        .if (zero?)
                inc     bx              ; VGA HSS adjustment is +2
        .endif
        inc     bx                      ; Native HSS adjustment is +1
IFDEF NVIDIA_SPECIFIC
        ; do bit 8 of HRS
        and     bh,1            ; mask out bit 8
        shl     bh,3-0          ; shift into position 3
        rol     ecx,16          ; switch to upper part of ECX
        or      cl,bh
        ror     ecx,16          ; switch back to lower part of ECX
ENDIF ; NVIDIA_SPECIFIC
        ;
        mov     ah, bl
        mov     al, 004h
        LOG_GO  dx, ax
    */
    //
    // Program horizontal retrace start (CR04)
    //
    val16 = timings->horiz_start / pixperclk;
    val16++;    // always native mode
    val08 = ((val16 >> 8) & 1) << 3;
    ov2D |= val08;
    val16 = ((val16 << 8) & 0xFF00) | 0x04;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR04=",(U008)(val16>>8));
    
    /*    
IFDEF NVIDIA_SPECIFIC
        ; CRTC[2D]
        rol     ecx,16          ; switch to upper part of ECX
        mov     ah,cl
        ror     ecx,16          ; switch back to lower part of ECX
        mov     al,02Dh
        SIM_NO  dx,ax
ENDIF ; NVIDIA_SPECIFIC
    */
    //
    // Program CR2D
    //
    val16 = ((((U016)ov2D) << 8) & 0xFF00) | 0x2D;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR2D=",(U008)(val16>>8));
    
    /* 
        ; CRTC[16]
        mov     bx, [edi].CRTCInfoBlock.VerticalTotal
        mov     ax, bp
        mov     al, ah
        xor     ah, ah
        sub     ax, bx
        neg     ax
        mov     ah, al
        mov     al, 016h
        dec     ah                      ; VGA VBE Adjustment
        LOG_GO  dx, ax                  ; VBE[0:7]
    */
    //
    // Program vertical blanking end (CR16)
    //
    val16 = (border >> 8) - timings->vertical_total;
    val16 = -val16;
    val16--;
    val16 = ((val16 << 8) & 0xFF00) | 0x16;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR16=",(U008)(val16>>8));
    
    /*            
        ; CRTC[06]
        mov     al, 006h
        sub     bx, 2                   ; VGA VT Adjustement
        mov     ah, bl
        LOG_GO  dx, ax                  ; VT[0:7]
    */
    //
    // Program the vertical total (CR06)
    //
    val16 = timings->vertical_total - 2;        
    val16 = ((val16 << 8) & 0xFF00) | 0x06;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR06=",(U008)(val16>>8));
    
    /*
        mov     bl, bh
        and     bl, 00000001b
        ;shl    0-(8-8)
        or      ch, bl                  ; VT[8]
        mov     bl, bh
        and     bl, 00000010b
        shl     bl, 5-(9-8)
        or      ch, bl                  ; VT[9]
        and     bh, 00000100b
        shr     bh, (10-8)-0
        or      cl, bh                  ; VT[10]
        ; CRTC[15]
        mov     ah, cl                  ; CRTC[25]
        and     ah, 00000010b
        shl     ah, (10-8)-1
        mov     bh, ah                  ; VDE[10]
        mov     al, ch                  ; CRTC[7]
        and     al, 01000000b
        shr     al, 6-(9-8)
        or      bh, al                  ; VDE[9]
        mov     ah, ch                  ; CRTC[7]
        and     ah, 00000010b
        shr     ah, 1-(8-8)
        or      bh, ah                  ; VDE[8]
        mov     al, 012h
        call    GetIndexRegister        ; VDE[7:0]
        mov     bl, ah
        inc     bx                      ; VGA VDE adjustment
        mov     ax, bp                  ; Adjust boarder
        mov     al, ah
        xor     ah, ah
        add     bx, ax
        mov     al, 015h
        dec     bx                      ; VGA VBS adjustment
        mov     ah, bl
        LOG_GO  dx, ax                  ; VBS[0:7]
    */
    //
    // Program vertical blanking start (CR15)
    //
    val16 = timings->vertical_total - 2;
    val08 = (val16 >> 8) & 0x01;
    ov07 |= (U016)val08;
    val08 = (val16 >> 8) & 0x02;
    val08 <<= 4;
    ov07 |= (U016)val08;
    val08 = (val16 >> 8) & 0x04;
    val08 >>= 2;
    ov25 |= (U016)val08;
    
    val08 = (ov25 & 0x02) << 1;
    val16 = (U016)val08 << 8;
    val08 = (ov07 & 0x40) >> 5;
    val16 |= (U016)val08 << 8;
    val08 = (ov07 & 0x02) >> 1;
    val16 |= (U016)val08 << 8;
    
    val16 &= 0xFF00;
    val16 |= (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x12) >> 8) & 0x00FF;
    val16++;
    val16 += (border >> 8);
    val16--;
    val16c = val16;                     // save for later
    val16b = ((val16 << 8) & 0xFF00) | 0x15;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16b);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR15=",(U008)(val16b>>8));
    
    /*
        mov     bl, bh
        and     bl, 00000001b
        shl     bl, 3-(8-8)
        or      ch, bl                  ; VBS[8]
    */
    val08 = ((val16 >> 8) & 1) << 3;
    ov07 |= val08;
    
    /*        
        ; CRTC[09]
        ; MSK - 5/15/97
        ; To allow a double scan mode to be non double scanned, we need to preserve the
        ; double scan bit so we can check it later
        ; We need to adjust VDE based on the old and new double scan bits
        ; We also need the original VDE
        ; NOTE: alternate way would be to get the value from the mode tables

        mov     al, 009h
        call    GetIndexRegister
        mov     bl, [edi].CRTCInfoBlock.Flags
        and     ah,10000000b    ; Old double scan mode
        shr     ah,7-0
        and     bl,00000001b    ; ?Double scan mode
        cmp     bl, ah
        .if     (!zero?)        ; Mismatch
    */
    //
    // Is the current doublescan status different from the requested flag?
    //
    val16 = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x09) >> 8) & 0x00FF;
    val16 >>= 7;
    // this takes advantage that the flag is 0x01
    if ((timings->flags & GTF_FLAGS_DOUBLE_SCANNED) != (U008)val16)
    {
     
        /*               
                ; New mode had different double scan setting from old mode
                ; We have to multiply or divide VDE by 2

                mov     bl, ch          ; CR[7].1
                and     bl, 00000010b
                shr     bl, 1-0         ; move to bit 0

                mov     ah, ch          ; CR[7].6
                and     ah, 01000000b
                shr     ah, 6-1         ; move to bit 1
                or      bl, ah

                mov     ah, cl          ; CR[25].1
                and     ah, 00000010b
                shl     ah, 2-1         ; Move to bit 2
                or      bl, ah
        */
        //
        // Adjust VDE
        //
        val08 = (ov07 & 0x02) >> 1;
        val08 |= (ov07 & 0x40) >> 5;
        val08 |= (ov25 & 0x02) << 1;
        
        /*
                mov     al, 012h
                call    GetIndexRegister
                mov     al, [edi].CRTCInfoBlock.Flags
                and     al, 00000001b   ; Is new mode a double scan mode?
                .if (zero?)
                        ; was double scan, now is not, so divide by 2
                        shr     bl, 1           ; Divide by 2
                        rcr     ah, 1
                .else
                        ; was not double scan, now is, so multiply by 2
                        shl     ah, 1           ; Multiply by 2
        BUG? =>         rcr     bl, 1
                        inc     ah              ; Make lsb set

                .endif
                mov     al, 012h

                LOG_GO  dx, ax          ; VDE[0:7]
        */
        
        val16 = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x12) >> 8) & 0x00FF;
        val16 |= (U016)val08 << 8;
        if (!(timings->flags & GTF_FLAGS_DOUBLE_SCANNED))
        {
            val16 >>= 1;
        } 
        else 
        {
            val16 <<= 1;
            val16++;
        }            
        val16b = ((val16 << 8) & 0xFF00) | 0x12;
        WriteIndexed(NV_PRMCIO_CRX__COLOR, val16b);
                        
        DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR12=",(U008)(val16b>>8));
                        
        /*
                and     ch, NOT 01000010b       ; Strip old VDE
                mov     al, bl
                and     al, 00000001b
                shl     al, 1-0         ; move to bit 1
                or      ch, al          ; CR[7].1
                mov     al, bl
                and     al, 00000010b
                shl     al, 6-1         ; move to bit 6
                or      ch, al          ; CR[7].1

                and     cl, NOT 00000010b       ; Strip old VDE
                mov     al, bl
                and     al, 00000100b
                shr     al, 2-1         ; move to bit 1
                or      cl, al          ; CR[25].1

        .endif
        ; End MSK - 5/15/97
        */
        //
        // Adjust the overflows based on the new VDE
        //
        ov07 &= 0xBD;   
        val08 = (U008)(val16 >> 8);
        val08 = (val08 & 0x01) << 1;
        ov07 |= val08;
        val08 = (U008)(val16 >> 8);
        val08 = (val08 & 0x02) << 5;
        ov07 |= val08;
        ov25 &= 0xFD;
        val08 = (U008)(val16 >> 8);
        val08 = (val08 & 0x04) >> 1;
        ov25 |= val08;
    }        

    /*
        mov     al, 009h
        call    GetIndexRegister
        and     ah, NOT 10100000b
        mov     bl, [edi].CRTCInfoBlock.Flags
        and     bl,00000001b    ; ?Double scan mode
        jz      @f              ; N: Leave clear
        or      ah, 10000000b   ; Y: Set double scan
@@:
        mov     bl, bh
        and     bl, 00000010b
        shl     bl, 5-(9-8)
        or      ah, bl                  ; VBS[9]
        LOG_GO  dx, ax
    */        
    //
    //
    val16b = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x09) >> 8) & 0x00FF;
    val16b &= 0x5F;
    if (timings->flags & GTF_FLAGS_DOUBLE_SCANNED)
        val16b |= 0x80;
    val08 = (U008)(val16c >> 8);
    val08 = (val08 & 0x02) << 4;
    val16b |= (U016)val08;    
    val16 = ((val16b << 8) & 0xFF00) | 0x09;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR09=",(U008)(val16>>8));
    
    /*
        and     bh, 00000100b
        shl     bh, 3-(10-8)
        or      cl, bh                  ; VBS[10]
        ; CRTC[10]
        mov     al, 010h
        mov     bx, [edi].CRTCInfoBlock.VerticalSyncStart
        mov     ah, bl
        LOG_GO  dx, ax                  ; VSS[0:7]
    */
    //
    // 
    val08 = (U008)(val16c >> 8);
    val08 = (val08 & 0x04) << 1;
    ov25 |= val08;
    val16 = timings->vertical_start;
    val16 = ((val16 << 8) & 0xFF00) | 0x10;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR10=",(U008)(val16>>8));
    
    /*        
        mov     bl, bh
        and     bl, 00000001b
        shl     bl, 2-(8-8)
        or      ch, bl                  ; VSS[8]
        mov     bl, bh
        and     bl, 00000010b
        shl     bl, 7-(9-8)
        or      ch, bl                  ; VSS[9]
        and     bh, 00000100b
        ;shl     bh, 2-(10-8)
        or      cl, bh                  ; VSS[10]
        ; CRTC[11]
        mov     al, 011h
        call    GetIndexRegister
        and     ah, 11110000b
        mov     bl, byte ptr [edi].CRTCInfoBlock.VerticalSyncEnd
        and     bl, 00001111b
        or      ah, bl
        LOG_GO  dx, ax                  ; VSE[0:3]
    */
    //
    // 
    val08 = (U008)(timings->vertical_start >> 8);
    val08 = (val08 & 0x01) << 2;
    ov07 |= val08;    
    val08 = (U008)(timings->vertical_start >> 8);
    val08 = (val08 & 0x02) << 6;
    ov07 |= val08;    
    val08 = (U008)(timings->vertical_start >> 8);
    val08 = val08 & 0x04;
    ov25 |= val08;    
    val16b = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x11);
    val16b &= 0xF0FF;
    val16b |= (timings->vertical_end & 0x000F) << 8;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16b);
    
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR11=",(U008)(val16b>>8));
    
    /*        
        ; Store overflow registers
        ; CRTC[7]
        mov     al, 007h
        mov     ah, ch
        LOG_GO  dx, ax
    */
    val16 = ((((U016)ov07) << 8) & 0xFF00) | 0x07;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
            
    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR07=",(U008)(val16>>8));
            
    /*        
        ; CRTC[25]
IFDEF NVIDIA_SPECIFIC
        mov     al, 025h
        mov     ah, cl
        SIM_NO  dx, ax
ENDIF ; NVIDIA_SPECIFIC
    */
    val16 = ((((U016)ov25) << 8) & 0xFF00) | 0x25;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "SetMode: CR25=",(U008)(val16>>8));

    return(0);
}

//*****************************************************************************
//
// Function: SetDMTOverrideValues()
//
// Description:
//    
//      Do VESA 1.0 rev 0.7 DMT timings for the following modes:
//
//      DMT timing calculations were produced with the VESA DMT 1.0 rev 0.7 
//      Monitor Timing Specifications and D.Reed's NV3 CRTC spreadsheet.
//
//           1600 x 1200 @ 60,70,75,85
//           1280 x 1024 @ 60,75,85
//           1152 x  864 @ 75
//           1024 x  768 @ 60,70,75,85
//            800 x  600 @ 60,72,75,85
//            640 x  480 @ 60,72,75,85
//
//      DMT_Overrides_structure
//
//          U016	DotClock;
//          U016    Polarity;
//          U016    CR0;
//          U016    CR2;
//          U016    CR3;
//          U016    CR4;
//          U016    CR5;
//          U016    CR6;
//          U016    CR7;
//          U016    CR10;
//          U016    CR11;
//          U016    CR15;
//          U016    CR16;
//  
//*****************************************************************************


VOID SetDMTOverrideValues(PDMT_OVERRIDES pDMTOverrides)            

{

    U032 val32;
    U016 val16;
    U016  wv;                     

    //
    // Program the new clocks
    //

    OEMSetClock((U016)(((U032)(pDMTOverrides->DotClock))));

    //
    // Program the sync polarity
    //

    val32   = ReadPriv32(NV_PRMVIO_MISC__READ) & 0x3F;
    val32  &= 0xf3;
    val32  |= 0x8;
    val32  |= pDMTOverrides->Polarity;
    WritePriv08(NV_PRMVIO_MISC__WRITE, (U008)val32);

    //
    // Clear the crtc protection bit.
    //
    
    val16 = 0x0011;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);


    //
    // Update CR0
    //

    val16 = pDMTOverrides->CR0;
    val16 = ((val16 << 8) & 0xFF00) | 0x00;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);


    //
    // Update CR2
    //

    val16 = pDMTOverrides->CR2;
    val16 = ((val16 << 8) & 0xFF00) | 0x02;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR3
    //

    val16 = pDMTOverrides->CR3;
    val16 = ((val16 << 8) & 0xFF00) | 0x03;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR4
    //

    val16 = pDMTOverrides->CR4;
    val16 = ((val16 << 8) & 0xFF00) | 0x04;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR5
    //

    val16 = pDMTOverrides->CR5;
    val16 = ((val16 << 8) & 0xFF00) | 0x05;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR6
    //

    val16 = pDMTOverrides->CR6;
    val16 = ((val16 << 8) & 0xFF00) | 0x06;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR7
    //

    val16 = pDMTOverrides->CR7;
    val16 = ((val16 << 8) & 0xFF00) | 0x07;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR10
    //

    val16 = pDMTOverrides->CR10;
    val16 = ((val16 << 8) & 0xFF00) | 0x10;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);
    //
    // Update CR11
    //

    val16 = pDMTOverrides->CR11;
    val16 = ((val16 << 8) & 0xFF00) | 0x11;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR15
    //

    val16 = pDMTOverrides->CR15;
    val16 = ((val16 << 8) & 0xFF00) | 0x15;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

    //
    // Update CR16
    //

    val16 = pDMTOverrides->CR16;
    val16 = ((val16 << 8) & 0xFF00) | 0x16;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, val16);

}


//       OEMSetRegs - Set extended registers (standard VGA has been set)
//
//       Entry:  ES:DI = Standard VGA parameter pointer
//               DS = Seg0
//       Exit:   None
//
VOID OEMSetRegs(MODEDESC *mi)
{
    U016  wv;                     // Write value    
    U016  i, c;
    U032   lwv;
    U032   lrv;
    U008   cv;

    if (mi->mdCMode != 0xFF)        // Do we have a CRTC Override Table?
    {
        // Yep - program it
        WritePriv08(NV_PRMVIO_MISC__WRITE, crt_override[mi->mdCMode].CO_Misc);
                                                                      
        wv = crt_override[mi->mdCMode].CO_ClockMode;
        wv <<= 8;
        wv |= 0x01;
        WriteIndexed(NV_PRMVIO_SRX, wv); // Write SR01
        
        wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x11);    // Read CR11
        wv &= 0x7FFF;               // Unlock CR0-7
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR11
        
        // Do CR0-CR8 sequentially
        LoadIndexRange(NV_PRMCIO_CRX__COLOR, 0, 8, (U008 *)&(crt_override[mi->mdCMode].HTotal));
        
        wv = crt_override[mi->mdCMode].CO_CellHeight;
        wv <<= 8;
        wv |= 0x09;
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR09
        
        // Do CR10-CR12 sequentially
        LoadIndexRange(NV_PRMCIO_CRX__COLOR, 0x10, 0x03, (U008 *)&(crt_override[mi->mdCMode].CO_VSyncStart));
        
        // Do CR15-CR16 sequentially
        LoadIndexRange(NV_PRMCIO_CRX__COLOR, 0x15, 0x02, (U008 *)&(crt_override[mi->mdCMode].CO_VBlankStart));
    }
    
    // Program Extended Registers
    
    wv = eregs[mi->mdEMode].xrOffset;
    wv <<= 13;              
    wv &= 0xE000;                   // Clear start address bits
    wv |= 0x19;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR19
                                                       
    wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
    wv &= 0xC0FF;                   // Preserve sync disables
    wv |= 0x3800;                   // Set reserved bits and text mode
                    
    c = (eregs[mi->mdEMode].xrFlags);
         
    if (c & 0x01)
        wv |= 0x0100;               // Set address wrap
        
    i = (eregs[mi->mdEMode].xrVOver);
    i &= 0x20;
    i >>= 0x03;
    wv |= (i << 8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1A
    
    // Default Fifo
    wv = 0x011B;                    // Default CR1B value for VGA modes
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1B
                                                        
    wv = c;
    wv &= 0x06; 
    wv <<= 8;
    wv |= 0x1C;                     // CR1C
    
    i = (eregs[mi->mdEMode].xrPixFmt);
    i &= 0x30;                      // Mask off chain-4 optimize bits
    i >>= 1;            // (shr 4-3?)
    
    wv |= (i << 8);                 // Set chain 4 optimize bits
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1A
    
    
    wv = 0x001D;                    // Clear CR1D & 1E
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1D
    
    wv = 0x001E;                    // Clear CR1D & 1E
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1E
    
    wv = 0x1020;                    // Fifo Setting for Standar VGA modes
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set low water mark at CR20
    
    c = (eregs[mi->mdEMode].xrVOver); // Vertical Overflow
    c &= 0x1F;                      // Mask our bits                                               
    
    i = (eregs[mi->mdEMode].xrOffset);
    i &= 0x08;                      // Bit 3
    i <<= 2;                        // Move to bit 5
    c |= i;
    wv =  (c << 8) | 0x25;          // Write data to CR25
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set CR25
                                               
    
    wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28);
    wv &= 0xF8FF;                   // Preserve sync disables
             
    c = eregs[mi->mdEMode].xrPixFmt;
    c &= 0x07;                      // Set bits [2:0] - clear rest
    wv |= (c << 8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set CR28
                                                   
    c = eregs[mi->mdEMode].xrPixFmt;
    c &= 0xC0;
    c >>= 6;
    
    i = eregs[mi->mdEMode].xrVOver;
    i &= 0xC0;
    i >>= 4;
    c |= i;
    wv = ((c << 8) | 0x2D);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set CR2D
                                                      
    wv = 0x29;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  
    wv = 0x2A;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  
    
    // Do CR29-33
    /*
    for (i = 0x30; i <= 0x33; i++)
    {                      
        wv = (U016)i;
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  
    } 
    */                                          
    
    wv = (((eregs[mi->mdEMode].xrIntlace) << 8) | 0x39);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  
    
    // Set dot clock
    //OEMSetClock(eregs[mi->mdEMode].xrClkSel);
    if (pDev->Framebuffer.MonitorType != MONITOR_TYPE_FLAT_PANEL)
        OEMSetClock(tblClockFreq[eregs[mi->mdEMode].xrClkSel]);

    wv = (((eregs[mi->mdEMode].xrVOffset) << 8) | 0x13);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  
                                                     
    pDev->Framebuffer.CursorAdjust = 0;   // default to no adjustment of cursor position
    if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_TV)
    {
        ProgramTV( mi->mdTVMode );              // CRTC tweaks for TV
        // plus some additional tweaks for pixel/line doubled modes
        if (mi->mdTVMode <= TV_MODE_400x300)    // if clock doubling
        {
            WritePriv32(0x01030700, NV_PRAMDAC_PLL_COEFF_SELECT);   // clock doubled mode
        }
        else
            WritePriv32(0x10030700, NV_PRAMDAC_PLL_COEFF_SELECT);
    }
    else    
    {
        // Not TV, if flat panel, we may need to adjust CRTC's
        if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_FLAT_PANEL)
            FlatPanelCrtc(mi->mdXRes);
        else
            WritePriv32(0x10000700, NV_PRAMDAC_PLL_COEFF_SELECT);
    }        

    // if flat panel, leave DAC alone
    if (pDev->Framebuffer.DisplayType != DISPLAY_TYPE_FLAT_PANEL)
    {
        lrv = ReadPriv32(NV_PRAMDAC_GENERAL_CONTROL);
    
        lrv &= (U032)(((1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE)) |
             (1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE)) |
             (1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_BPC))) ^ -1L);
        
        lrv |= (U032)(1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE));
        
        wv = (eregs[mi->mdEMode].xrOffset);
        lwv = (U032)(wv & 0x80);       // Set 565 Mode
    
        lwv <<= (DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE) - 7L);
        
        lrv |= lwv; 
    
        cv = (eregs[mi->mdEMode].xrFlags);
        cv ^= (U008)-1;
        cv &= 0x08;                 // Isolate DAC width
    
        lwv = (U032)(cv);          // Dac Width
        lwv <<= (U032)(DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_BPC) - 3L);
                                       
        lrv |= lwv; 
        
        cv = (eregs[mi->mdEMode].xrPixFmt);
        cv ^= (U008)-1;                   // Invert
        cv &= 0x02;                 // Isolate index/gamma bit
                             
        lwv = (U032)(cv);
        
        //KJK lwv <<= (U032)(DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE) - 1L);
        lrv |= lwv; 
    
        WritePriv32(lrv, NV_PRAMDAC_GENERAL_CONTROL);
    }
        
    WritePriv08(NV_PRMCIO_CRX__COLOR, 0x18);    // Select CR18 (why?)
}

//  
//  The H/V timings may have to be adjusted for TV.
//  Also, the fifo low water mark should be set for TV.
//
VOID ProgramTV
(
	U008	tvmode
)
{
 
	U008	data;
    U016    data16 = 0;
    U008    hbeAdjust;
        
    CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, 0x20);     // if TV, we can fix low water mark at 20.

    //  fix up CRTC timing for NV4
    if (TV_ENCODER_FAMILY(pDev->Framebuffer.EncoderType) == TV_ENCODER_CHRONTEL)
    {
        if (TVformat == NTSC)
        {
            CRTC_WR(NV_CIO_CR_HDT_INDEX, CRTC_CH_NTSC[ tvmode ].CR00);
            CRTC_WR(NV_CIO_CR_HRS_INDEX, CRTC_CH_NTSC[ tvmode ].CR04);
            CRTC_WR(NV_CIO_CR_VDT_INDEX, CRTC_CH_NTSC[ tvmode ].CR06);
            CRTC_WR(NV_CIO_CR_OVL_INDEX, CRTC_CH_NTSC[ tvmode ].CR07);
            CRTC_WR(NV_CIO_CR_RSAL_INDEX, CRTC_CH_PAL[ tvmode ].CR08);
            CRTC_WR(NV_CIO_CR_VRS_INDEX, CRTC_CH_NTSC[ tvmode ].CR10);
            CRTC_WR(NV_CIO_CR_VDE_INDEX, CRTC_CH_NTSC[ tvmode ].CR12);
            CRTC_WR(NV_CIO_CRE_LSR_INDEX, CRTC_CH_NTSC[ tvmode ].CR25);
            CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data);      // read pixel format
            data |= CRTC_CH_NTSC[ tvmode ].CR28;        // OR with horizontal pixel adjust and slave TV to encoder
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data);

            // write Seq 01
            WriteIndexed(NV_PRMVIO_SRX, (U016) ((CRTC_CH_NTSC[ tvmode ].SR01 << 8) | NV_PRMVIO_SR_CLOCK_INDEX));         
        }
        else // its PAL
        {
            CRTC_WR(NV_CIO_CR_HDT_INDEX, CRTC_CH_PAL[ tvmode ].CR00);
            CRTC_WR(NV_CIO_CR_HRS_INDEX, CRTC_CH_PAL[ tvmode ].CR04);
            CRTC_WR(NV_CIO_CR_VDT_INDEX, CRTC_CH_PAL[ tvmode ].CR06);
            CRTC_WR(NV_CIO_CR_OVL_INDEX, CRTC_CH_PAL[ tvmode ].CR07);
            CRTC_WR(NV_CIO_CR_RSAL_INDEX, CRTC_CH_PAL[ tvmode ].CR08);
            CRTC_WR(NV_CIO_CR_VRS_INDEX, CRTC_CH_PAL[ tvmode ].CR10);
            CRTC_WR(NV_CIO_CR_VDE_INDEX, CRTC_CH_PAL[ tvmode ].CR12);
            CRTC_WR(NV_CIO_CRE_LSR_INDEX, CRTC_CH_PAL[ tvmode ].CR25);
            CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data);      // read pixel format
            data |= CRTC_CH_PAL[ tvmode ].CR28;           // OR with horizontal pixel adjust and slave TV to encoder
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data);

            // write Seq 01
            WriteIndexed(NV_PRMVIO_SRX, (U016) ((CRTC_CH_PAL[ tvmode ].SR01 << 8) | NV_PRMVIO_SR_CLOCK_INDEX));         
        }
    }
    else if (TV_ENCODER_FAMILY(pDev->Framebuffer.EncoderType) == TV_ENCODER_BROOKTREE)
    {
        if (TVformat == NTSC)
        {
            // write Seq 01
            if (CRTC_BT[ tvmode ].SR01 != 0xff) // if table entry exists, do these adjustments
            {
                WriteIndexed(NV_PRMVIO_SRX, (U016) ((CRTC_BT[ tvmode ].SR01 << 8) | NV_PRMVIO_SR_CLOCK_INDEX));         
        
                // Adjust Horiz and Vert totals
                CRTC_WR(NV_CIO_CR_HDT_INDEX, CRTC_BT[ tvmode ].CR00);
                CRTC_WR(NV_CIO_CR_HRS_INDEX, CRTC_BT[ tvmode ].CR04);
                CRTC_WR(NV_CIO_CR_VDT_INDEX, CRTC_BT[ tvmode ].CR06);
                CRTC_WR(NV_CIO_CR_OVL_INDEX, CRTC_BT[ tvmode ].CR07);
                CRTC_WR(NV_CIO_CR_RSAL_INDEX, CRTC_BT[ tvmode ].CR08);
                CRTC_WR(NV_CIO_CR_VRS_INDEX, CRTC_BT[ tvmode ].CR10);
                CRTC_WR(NV_CIO_CR_VDE_INDEX, CRTC_BT[ tvmode ].CR12);
                if (tvmode == TV_MODE_320x200)  // 320 modes require additional adjustment for HDE
                {
                    CRTC_RD(NV_CIO_CR_HRS_INDEX, data);       // read CR04
                    CRTC_WR(NV_CIO_CR_HDE_INDEX, data-2);                
                    CRTC_WR(NV_CIO_CR_HBS_INDEX, data-2);                
                }
                if (tvmode == TV_MODE_320x240)  // 320 modes require additional adjustment for HDE
                {
                    CRTC_RD(NV_CIO_CR_HRS_INDEX, data);       // read CR04
                    CRTC_WR(NV_CIO_CR_HDE_INDEX, data-3);                
                    CRTC_WR(NV_CIO_CR_HBS_INDEX, data-3);                
                }
            }
        }
        else    // Brooktree PAL
        {
                
            // write Seq 01
            if (CRTC_BT_PAL[ tvmode ].SR01 != 0xff) // if table entry exists, do these adjustments
            {
                WriteIndexed(NV_PRMVIO_SRX, (U016) ((CRTC_BT[ tvmode ].SR01 << 8) | NV_PRMVIO_SR_CLOCK_INDEX));         
        
                // Adjust Horiz and Vert totals
                CRTC_WR(NV_CIO_CR_HDT_INDEX, CRTC_BT_PAL[ tvmode ].CR00);
                CRTC_WR(NV_CIO_CR_HRS_INDEX, CRTC_BT_PAL[ tvmode ].CR04);
                CRTC_WR(NV_CIO_CR_VDT_INDEX, CRTC_BT_PAL[ tvmode ].CR06);
                CRTC_WR(NV_CIO_CR_OVL_INDEX, CRTC_BT_PAL[ tvmode ].CR07);
                CRTC_WR(NV_CIO_CR_RSAL_INDEX, CRTC_BT_PAL[ tvmode ].CR08);
                CRTC_WR(NV_CIO_CR_VRS_INDEX, CRTC_BT_PAL[ tvmode ].CR10);
                CRTC_WR(NV_CIO_CR_VDE_INDEX, CRTC_BT_PAL[ tvmode ].CR12);
                if (tvmode <= TV_MODE_320x400)  // 320 modes require additional adjustment for HDE
                {
                    CRTC_RD(NV_CIO_CR_HRS_INDEX, data);       // read CR04
                    CRTC_WR(NV_CIO_CR_HDE_INDEX, data-5);                
                    CRTC_WR(NV_CIO_CR_HBS_INDEX, data-5);                
                }
            }
        }        
    } // encoder specific

    CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data);              // get CR28
    data &= ~0x38;
    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data);              // clear the pixel shift 

    if (tvmode <= TV_MODE_320x400)
    {
        // adjust HBE = HT. This lets the cursor move across the whole screen.
        CRTC_RD(NV_CIO_CR_HDT_INDEX, data16);           // get HT (CR0)
        CRTC_RD(NV_CIO_CR_HBE_INDEX, data);             // CR03
        data &= ~0x1f;                                  // lose the HBE bits
        data |= (U008) data16 & 0x1F;
        CRTC_WR(NV_CIO_CR_HBE_INDEX, data);             // HBE 4:0
        pDev->Framebuffer.CursorAdjust = 8;
    }
    else
    {
        if (tvmode == TV_MODE_400x300)
        {
            CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data);          // get CR28
            data &= ~0x38;
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data | (5<<3)); // bits[5:3] shift 5 pixels right        
            pDev->Framebuffer.CursorAdjust = 0;
            if (TVformat == NTSC)   // PAL 400x300 requires special case for HBE for cursor position
                hbeAdjust = 4;
            else                
                hbeAdjust = 2;
        }
        else
        {
            pDev->Framebuffer.CursorAdjust = 5;
            hbeAdjust = 4;
        }
        // adjust HBE = HT + 4. This lets the cursor move across the whole screen.
        CRTC_RD(NV_CIO_CR_HDT_INDEX, data16);           // get HT (CR0)
        CRTC_RD(NV_CIO_CRE_HEB__INDEX, data);           // MS bit HT (CR2D)            
        if (data & 1)
            data16 |= 0x100;
        data16 += hbeAdjust;                            // HT effective

        CRTC_RD(NV_CIO_CR_HBE_INDEX, data);             // CR03
        data &= ~0x1f;                                  // lose the HBE bits
        data |= (U008) data16 & 0x1F;
        CRTC_WR(NV_CIO_CR_HBE_INDEX, data);             // HBE 4:0

        CRTC_RD(NV_CIO_CR_HRE_INDEX, data);             // CR05
        data &= ~0x80;                                  // lose the HBE bit
        if (data16 & 0x20)
            data |= 0x80;
        CRTC_WR(NV_CIO_CR_HRE_INDEX, data);             // HBE 5
                            
        CRTC_RD(NV_CIO_CRE_LSR_INDEX, data);            // CR25
        data & ~0x10;
        if (data16 & 0x40)
            data |= 0x10;
        CRTC_WR(NV_CIO_CRE_LSR_INDEX, data);            // HBE 6
    }
    // adjust VBE = VT + 1. This lets the cursor move vertically through the whole screen.
    CRTC_RD(NV_CIO_CR_VDT_INDEX, data);             // CR6
    data += 1;
    CRTC_WR(NV_CIO_CR_VBE_INDEX, data);             // CR16

    // adjust normally scanline doubled modes which are not doubled for TV
    if ((tvmode == TV_MODE_512x384) | (tvmode == TV_MODE_480x360))
        CRTC_WR(NV_CIO_CR_CELL_HT_INDEX, 0x40);
        
}

//
//       LoadPalette - Load the system palette and the user defined palette if there is one
//
//       Entry:  DS = Seg0
//               ES:DI = Pointer to parameter entry
//       Exit:   None
VOID LoadPalette(MODEDESC *mi)
{
    U016  flags;
    
#ifdef DOS_COMPILE    
    _asm    pushf;                  // Save Interrupt state
    _asm    pop ax;
    _asm    mov flags,ax
    _asm    cli;                    // No interrupts during this!
#endif 
    
    //
    // Load default internal palette
    //
    LoadDefaultATCAndSavePtr((vga_tbl[mi->mdIMode].PT_ATC));
                             
    ReadPriv32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    
    WritePriv08(NV_PRMCIO_ARX, 0x14);   // Select AR14
    WritePriv08(NV_PRMCIO_ARX, 0x00);   // Zero it!
                                           
    //
    // Load default external palette
    //
    LoadDefaultRAMDAC(mi);
 
#ifdef DOS_COMPILE    
    _asm    mov ax,flags
    _asm    push ax
    _asm    popf;                   // Restore interrupt status
#endif
        
}

//
//       LoadDefaultATCAndSavePtr - Load a the default ATC registers and store them in the saveptr area if necessary
//
//       Entry:  ES:SI = Pointer to palette data
//               DX = I/O Address of Input Status Register
//               DS = Seg0
//       Exit:   None
//
//       AX, CX, SI are trashed
//       DX is preserved
//
VOID LoadDefaultATCAndSavePtr(U008 *pal)
{
    U008  i;
    U016  flags;
    
#ifdef DOS_COMPILE    
    _asm    pushf;                  // Save Interrupt state
    _asm    pop ax;
    _asm    mov flags,ax
    _asm    cli;                    // No interrupts during this!
#endif    
     
    ReadPriv32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    
    for (i = 0; i < 20; i++)
    {
        WritePriv08(NV_PRMCIO_ARX, i);      // Select AR
        WritePriv08(NV_PRMCIO_ARX, *pal++); // Write color data
    }
                                                
    ReadPriv32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
                                    // So DOS Edit doesn't barf
#ifdef DOS_COMPILE    
    _asm    mov ax,flags
    _asm    push ax
    _asm    popf;                   // Restore interrupt status
#endif    
}

//
//       LoadDefaultRAMDAC - Load the default palette
//
//       Entry:  DS = Seg0
//       Exit:   None
//
//       AX, BX, CX, DX, SI are trashed
//

// #define DAC_MASK 0x3C6

VOID LoadDefaultRAMDAC(MODEDESC *mi)
{
    WritePriv08(NV_USER_DAC_PIXEL_MASK, 0xFF);      // Write DAC Mask
    // outp(DAC_MASK, 0xFFh);
    
    switch(mi->mdBPP)
    {
    case    4:          // 4bpp - Planar
        LoadColorDACTable();
        break;
        
    case    8:          // 8bpp - Packed
        LoadColor256DAC();
        break;
        
    case    16:         // 16bpp - Packed
    case    32:         // 16bpp - Packed
    default:
        LoadIdentityDAC();
        break;
    }       
}                        

//
//       LoadColorDACTable - Load the DAC for the 16-color modes
//
//       Entry:  DS = Seg0
//       Exit:   None
//
//       WARNING! This routine must follow "LoadColorTextDACTable"!
//
//       AX, BX, CX, DX, SI are trashed.
//
VOID LoadColorDACTable(VOID)
{
    U008  i;
    U016  flags;
    
#ifdef DOS_COMPILE    
    _asm    pushf;                  // Save Interrupt state
    _asm    pop ax;
    _asm    mov flags,ax
    _asm    cli;                    // No interrupts during this!
#endif    
    
    // Select the DAC address
    WritePriv08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00);
    
    // Do the first 64 locations
    for (i = 0; i < 64; i++)
        WriteColor(i);
        
#ifdef DOS_COMPILE    
    _asm    mov ax,flags
    _asm    push ax
    _asm    popf;                   // Restore interrupt status
#endif    
}
    
//
//       LoadColor256DAC - Load the DAC for the 256-color modes
//
//       Entry:  DS = Seg0
//       Exit:   None
//
//       AX, BX, CX, DX, SI are trashed.
//
//------------------------------------------------------------------------
// First 16 colors for mode 13h (compatible R'G'B'RGB)
//
extern U008 CompatColors[];
                         
//------------------------------------------------------------------------
// Gray values for next 16 (6 bits of intensity)
//
extern U008 GrayColors[];
                                                                              
//------------------------------------------------------------------------
// Fixup values for DAC color registers after summing to gray shades
//
extern U008 FixupColors[];
                                                                       
//------------------------------------------------------------------------
// 9 groups of five intensities are used to generate the next 216 colors.
// Each group is used to generate 24 colors by walking around a "color
// triangle" with eight colors per side.
//
extern U008 IntenseTable[];                              
                                                                   
VOID LoadColor256DAC(VOID)
{
    U016  i;
    U016  red;
    U016  green;
    U016  blue;
    U016  flags;
    
#ifdef DOS_COMPILE    
    _asm    pushf;                  // Save Interrupt state
    _asm    pop ax;
    _asm    mov flags,ax
    _asm    cli;                    // No interrupts during this!
#endif    
    
    // Select the DAC address
    WritePriv08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00);
    
    //--------------------------------------------------------------------
    // The first 16 colors are the compatible colors (i.e. 0,1,2,3,4,5,14,
    // 7,38,39,3A,3B,3C,3D,3E,3F).
    //
    for (i = 0; i < 16; i++)
        WriteColor(CompatColors[i]);
    
    //--------------------------------------------------------------------
    // The next 16 give various intensities of gray (0,5,8,B,E,11,14,18,
    // 1C,20,24,28,2D,32,38,3F).
    //
    for (i = 0; i < 16; i++)
    {
        WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i]);
        WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i]);
        WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)GrayColors[i]);
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
            WriteColor2(red, green, blue, (U016)(i * 5));
            red++;                                          
        } while (red < blue);
        
        do
        {
            // From loop
            WriteColor2(red, green, blue, (U016)(i * 5));
            blue--;
        } while (blue);
        
        // Traverse(red, green);
        do
        {
            // To loop
            WriteColor2(red, green, blue, (U016)(i * 5));
            green++;
        } while (green < red);
                         
        do
        {
            // From loop
            WriteColor2(red, green, blue, (U016)(i * 5));
            red--;
        } while (red);
                 
            
        // Traverse(green, blue);
        do
        {
            // To loop
            WriteColor2(red, green, blue, (U016)(i * 5));
            blue++;
        } while (blue < green);
                         
        do
        {
            // From loop
            WriteColor2(red, green, blue, (U016)(i * 5));
            green--;
        } while (green);
    }                
             
    //-----------------------------------------------------------------------
    // The last 8 colors are black.
    //
    //               +---+-- 8 blanks * 3 locations (r/g/b)
    for (i = 0; i < (8 * 3); i++)
        WritePriv08(NV_USER_DAC_PALETTE_DATA, 0x00);
    
#ifdef DOS_COMPILE    
    _asm    mov ax,flags
    _asm    push ax
    _asm    popf;                   // Restore interrupt status
#endif    
    
}

//
//       WriteColor - Writes one color value (three bytes) to the video DAC
//               R(analog) = R * 2Ah + R' * 15h
//               G(analog) = G * 2Ah + G' * 15h
//               B(analog) = B * 2Ah + B' * 15h
//
//       Entry:  DAC address already written
//               DX = DAC data address
//               BL = 6 bit color (R'G'B'RGB)
//       Exit:   None
//
//       AX is trashed
//
VOID WriteColor(U008 colorval)
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
    
        WritePriv08(NV_USER_DAC_PALETTE_DATA, color);
    }
}        
        
//
//       WriteColor2 - Writes one color value (three bytes) to the video DAC
//               R(analog) = R * 2Ah + R' * 15h
//               G(analog) = G * 2Ah + G' * 15h
//               B(analog) = B * 2Ah + B' * 15h
//
//       Entry:  DAC address already written
//               DX = DAC data address
//               CS:BX points to intensity table
//               SI = Red index (0 to 4)
//               DI = Green index (0 to 4)
//               CX = Blue index (0 to 4)
//
//       Exit:   None
//
//       AL is trashed
//
VOID WriteColor2(U016 red, U016 green, U016 blue, U016 tblidx)
{
    WritePriv08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+red]);
    WritePriv08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+green]);
    WritePriv08(NV_USER_DAC_PALETTE_DATA, IntenseTable[tblidx+blue]);
}

//
//       LoadIdentityDAC - Load the DAC with a gamma ramp
//
//       Entry:  DS = Seg0
//               ES = _TEXT
//       Exit:   None
//
//       AX, BX, CX, DX, SI are trashed.
//
VOID LoadIdentityDAC(VOID)
{
    U016  i;
    U016  flags;
    
#ifdef DOS_COMPILE    
    _asm    pushf;                  // Save Interrupt state
    _asm    pop ax;
    _asm    mov flags,ax
    _asm    cli;                    // No interrupts during this!
#endif    
    
    // Select the DAC address
    WritePriv08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0x00);
                                                  
    for (i = 0; i < 256; i++)
    {           
        WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)i);
        WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)i);
        WritePriv08(NV_USER_DAC_PALETTE_DATA, (U008)i);
    }                                     
    
#ifdef DOS_COMPILE    
    _asm    mov ax,flags
    _asm    push ax
    _asm    popf;                   // Restore interrupt status
#endif    
}
 
//       OEMSetClock - Set the clock chip to the requested frequency
//
//       Entry:  AL = Clock select index
//       Exit:   None
//
//       All registers are preserved.
VOID OEMSetClock(U016 clock)
{
    U032   mnp;
    
    //clock = tblClockFreq[csel];         // Get selected clock value 
    FixLowWaterMark(clock);             // Set low water mark.
    mnp = CalcMNP(&clock);              // Get the MNP of frequency
    
    // mnp = xxxxxxxx PPPPPPPP NNNNNNNN MMMMMMMM
    
//  AL = M parameter
//  AH = N parameter
//  DL = P parameter

    SetDACClock(mnp);
}

//       SetDACClock - Set the VPLL parameters in the NVidia DAC
//
//  Entry:  mnp = xxxxxxxx PPPPPPPP NNNNNNNN MMMMMMMM
//
//       Entry:  AL = M parameter
//               AH = N parameter
//               DL = P parameter
//       Exit:   None
//
VOID SetDACClock(U032 mnp)
{
    // NOTE: if we need to support divide by two, use msb of p parameter
    // Set the MNP VPLL parameters in the DAC
    WritePriv32(mnp, NV_PRAMDAC_VPLL_COEFF);
}

//
//       FixLowWaterMark
//
//       Entry:  AX = Dot clock frequency in kHz*100
//       Exit:   None
//
VOID FixLowWaterMark(U016 clock)
{
    U016  wv;
    U016  i;
    U032  clockMHz;
    
    wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28);
    wv >>= 8;
    wv &= 0x03;       // mask pixel format bits
    
    // Check dac format
    if (wv)
    {
        wv--;
        clockMHz = clock;
        clockMHz <<= (U008)wv;
        clockMHz /= 100;
        
        for (i = 0; clockMHz > DACFifoTable[i].Freq ; i++)
            ;   // Find the proper FIFO value
        
        if (pDev->Dac.InputWidth == 128)
        {
            // 128 bit
            // Do WV first, as I get's blown away
            wv = ((DACFifoTable[i].LowWaterMark128) << 8) | 0x20;
            i  = ((DACFifoTable[i].BurstSize128) << 8) | 0x1B; 
            
        }                                                   
        else
        {
            // 64 bit   
            wv = ((DACFifoTable[i].LowWaterMark64) << 8) | 0x20;
            i  = ((DACFifoTable[i].BurstSize64) << 8) | 0x1B;
        }                                                  
        
        WriteIndexed(NV_PRMCIO_CRX__COLOR, i);
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);         // otherwise, we have an optimum value
    }
}   

//       CalcMNP - Set the clock chip to the requested frequency
//
//       Entry:  AX = Clock frequency in 100 kHz
//       Exit:   AL = M parameter
//               AH = N parameter
//               DL = P parameter
//               BX = Closest clock frequency in 100 kHz
//
//      Returns: long lwv = xxxxxxxx PPPPPPPP NNNNNNNN MMMMMMMM
//
U032 CalcMNP(U016 *clock)
{
    U016  crystalFreq;
    U032   vclk;
    U016  vclkClosest;
    U016  deltaOld;
    U016  bestM;
    U016  bestP;
    U016  bestN;
    U016  lowM;
    U016  highM;
    
    U016  powerP;
    U016  m;
    U016  n;
    U016  i, j;
    U032   lwv;
    
    U032   edx;
    U016  dx, cx, bx;
    U016  lwvs, vclks;
    U016  t;
    
    vclk = (U032)(*clock * 10);     // Make KHz
    
    deltaOld = 0xFFFF;
                                    
    lwv = ReadPriv32(NV_PEXTDEV_BOOT_0) & 0x00000040;
    
    if (lwv)
    {
        // 14.3Khz
        dx = CRYSTAL_FREQ143KHZ;
        cx = 1;
        bx = 14;
    }
    else
    {
        // 13.5Khz
        dx = CRYSTAL_FREQ135KHZ;
        cx = 1;
        bx = 13;
    }                      
    
    lowM = cx;
    highM = bx;
    crystalFreq = dx;
                                              
    // Register usage below
    // eax = scratch
    // ebx = scratch
    // edx = scratch
    // si = m
    // di = n
    
    // P Loop
    for (powerP = 0; powerP <= 4; powerP++) // Init Power of P
    {                             
        // M Loop
        for (m = lowM; m <= highM; m++)
        {                          
            lwv = vclk << powerP;   // vclk * P
            
            // Compare with internal range of DAC
            if ((lwv >= 128000) && (lwv <= 350000))
            //if (1)
            {
                lwv *= (U032)m;    // vclk * P * M
                
                // Add crystal Frequency /2 - will round better
                lwv += (crystalFreq >> 1);
                
                lwv /= crystalFreq; // N = vclk*P*M/crystal
                n = (U016)(lwv & 0xFFFF);
                
                lwv *= crystalFreq; // N * crystal
                   
                i = 1 << powerP;    // for rounding
                i >>= 1;            // /2
                
                lwv += i;           // Add 1/2 of P
                lwv >>= powerP;     // N*cyrstal/P
                
                i = m;
                j = m;
                
                j >>= 1;            // M/2
                lwv += j;           // For rounding
                lwv /= m;           // Freq = N*crystal/P/M
                                       
                
//                   ; upper part of ebx should still be 0
//                   mov     bx, m
//                   ; Note: edx should be zero from multiply above
//                   mov     dx, bx
//                   shr     dx, 1           ; m / 2
//                   add     eax, edx        ; This will round better
//                   xor     dx, dx
//                   div     ebx             ; Freq=N*crystal/P/M
//                   
//                   mov     edx, eax        ; Save a copy        

                edx = lwv;          // Save a copy

                // Because the only divide in computing N is by the
                // crystal frequency, the remainder will always be
                // less than the crystal frequency which fits into
                // a word, so we can do the delta math on words
                //
                // Compute delta
                
                lwvs = (U016)(lwv & 0xFFFF);
                vclks = (U016)(vclk & 0xFFFF);
                
                if (lwvs < vclks)
                {               
                    // Exchange the values
                    t = lwvs;
                    lwvs = vclks;
                    vclks = t;
                }            
                    
                lwvs -= vclks;      // ABS(Freq-Vclk);
                
                // lwvs is ax
                // vclks is bx
                if (((vclk / lwvs) > 500) && ((*clock) < 25000))
                {
                    bestP = powerP;
                    bestM = m;
                    bestN = n;
                    edx /= 10;
                    vclkClosest = (U016)(edx & 0xFFFF);
                    lwv = bestP;
                    lwv <<= 16;
                    lwv |= ((bestN << 8) | (bestM));
                    
                   
                    // mov     bx, vclkClosest
                    *clock = vclkClosest;
                    
                    return(lwv);
                
                }                
                else  if (lwvs < deltaOld)
                {
                    // Closer match
                    deltaOld = lwvs;
                    
                    bestP = powerP;
                    bestM = m;
                    bestN = n;
                            
                    //         mov     deltaOld, ax
                    //         mov     bestP, powerP
                    //         mov     ax, m  
                    //         mov     bestM, al
                    //         mov     ax, n
                    //         mov     bestN, al
                    
                    edx /= 10;
                    
                    //         mov     eax,edx
                    //         xor     edx,edx
                    //         mov     ebx,10
                    //         div     ebx             ; Convert
                    //         mov     vclkClosest,ax
                    vclkClosest = (U016)(edx & 0xFFFF);
                }
            }
        }
    }                             
    
  //  lwv = ((bestP << 16) | (bestN << 8) | (bestM));
    lwv = bestP;
    lwv <<= 16;
    lwv |= ((bestN << 8) | (bestM));
    
   
    // mov     bx, vclkClosest
    *clock = vclkClosest;
    
    return(lwv);
} 

//       FullCpuOn - Give the CPU full bandwidth to video memory
//
//       Entry:  None
//       Exit:   None
//
VOID FullCpuOn(VOID)
{
    U016  datum;
    
    datum = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
    datum |= 0x2000;                            // Full Bandwidth bit ON
    WriteIndexed(NV_PRMVIO_SRX, datum);          // Set bit
} 
                                                   
//       FullCpuOff - Share the CPU bandwidth with the CRTC
//
//       Entry:  None
//       Exit:   None
//
VOID FullCpuOff(VOID)
{            
    U016  datum;
    
    datum = ReadIndexed(NV_PRMVIO_SRX, 0x01);    // Read SR1
    datum &= 0xDFFF;                            // Full Bandwidth bit OFF
    WriteIndexed(NV_PRMVIO_SRX, datum);          // Set bit            
} 

//
//       SetRegs - Set VGA registers
//
//       Entry:  ES:DI = Pointer to standard parameter table entry
//               DS = Seg0
//       Exit:   None
VOID SetRegs(MODEDESC *mi)
{                    
    // Load Sequencer and Misc. Output
    WriteIndexed(NV_PRMVIO_SRX, 0x0100);         // Sync Reset
    
    WritePriv08(NV_PRMVIO_MISC__WRITE, vga_tbl[mi->mdIMode].PT_Misc);
                                                   
    // Load Sequencers, beginning at SR1, load 4 SR's (SR1-4), from the mode table
    LoadIndexRange(NV_PRMVIO_SRX, 1, sizeof(SEQ), (U008 *)&(vga_tbl[mi->mdIMode].seq_regs));
                                            
    WriteIndexed(NV_PRMVIO_SRX, 0x0300);         // End Sync Reset
                                  
    // Load CRTC's
    WriteIndexed(NV_PRMCIO_CRX__COLOR, 0x2011);  // Unlock CR0-7
                                            
    // Load CRTC, beginning at CR0, for all CR's (CR0-18), from the mode table
    LoadIndexRange(NV_PRMCIO_CRX__COLOR, 0, sizeof(CRTC), (U008 *)&(vga_tbl[mi->mdIMode].crtc_regs));
                                                                                                   
    // Load GDC's
    // Load GR, beginning at GR0, for all GR's (GR0-8), from the mode table
    LoadIndexRange(NV_PRMVIO_GRX, 0, sizeof(GR), (U008 *)&(vga_tbl[mi->mdIMode].gr_regs));
}                                           

VOID LoadIndexRange(U032 addr, U008 st_idx, U008 count, U008 *regs)
{
    U016  i;
    U016  wv;
    
    // Program SR1-4
    for (i = 0; i < count; i++)
    {               
        wv = *regs++;
        wv <<= 8;               // Put data in high half
        wv |= st_idx++;         // Get index and increment it
        WriteIndexed(addr, wv);
    }               
}                                  

//       OEMPreSetRegs - Make special preparations to load registers
//
//       Entry:  ES:DI = Pointer to standard parameter table entry
//               DS = Seg0
//       Exit:   None
VOID OEMPreSetRegs(VOID)
{                      
    U016  rc;
    
    rc = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28);
    rc &= 0xF8FF;                   // Strip to standard DAC settings
    WriteIndexed(NV_PRMCIO_CRX__COLOR, rc);
    
    rc = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
    rc |= 0x0200;                   // Use 6-bit DAC
    WriteIndexed(NV_PRMCIO_CRX__COLOR, rc);
}                               

//
//       ATCOff - Turn off Attribute Controller
//
//       Entry:  None
//       Exit:   None
VOID ATCOff(VOID)
{
    U016  flags = 0;
    
#ifdef DOS_COMPILE    
    _asm    pushf;                  // Save Interrupt state
    _asm    pop ax;
    _asm    mov flags,ax            // Save flags 
    _asm    cli;                    // No interrupts during this!
#endif    
    
    ReadPriv32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    WritePriv08(NV_PRMCIO_ARX, 0x00);   // Turn off screen at AR
    
#ifdef DOS_COMPILE    
    _asm    mov ax,flags
    _asm    push ax
    _asm    popf
#endif    
}

//
//       ATCOn - Turn on Attribute Controller
//
//       Entry:  None
//       Exit:   None
VOID ATCOn(VOID)    
{
    U016  flags;
    
#ifdef DOS_COMPILE    
    _asm
    {
        pushf
        pop ax
        mov flags,ax
        cli
    }
#endif    
    
    ReadPriv32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
    WritePriv08(NV_PRMCIO_ARX, 0x20);   // Turn off screen at AR
    ReadPriv32(NV_PRMCIO_INP0__COLOR);   // Reset ATC FlipFlop
                                    // So DOS Edit doesn't barf
#ifdef DOS_COMPILE    
    _asm
    {
        mov ax,flags
        push ax
        popf
    }
#endif        
}

//
// ValidateMode - Validate the passed mode is valid, and if it is,
//                return the pointer to the mode information block.
//
//  Entry:  U016  mode            // VESA Mode Number to validate
//
//  Exit:   MODEDESC *mi            // Mode Information Block Pointer
//                                  // NULL if error (invalid mode)
//  NOTES:  This function must also verify the operational mode,
//          and return failure status if not valid on TV
//

MODEDESC *ValidateMode(U016 mode)
{
    MODEDESC    *mi;
    
    for (mi = &mib[0]; mi->mdVMode != 0xFFFF; mi++)
    {
        if (mi->mdVMode == mode)
        {
            if (OEMValidateMode(mi))
                return(mi);            
            break;      // Out of the for - report error
        }   
    }   
    return(NULL);
}

// From OEMIsModeAvailable - validate the mode from the OEM perspective.
//
U008 OEMValidateMode(MODEDESC *m)
{
    U032   memory;
            
    // Are we running tv mode? If so, we need an additional filter
    if (pDev->Framebuffer.DisplayType == DISPLAY_TYPE_TV)
    {
        if (m->mdTVMode == 0xFF)
            return((U008)0);
    }
    
    // Validate we have enough memory
    // (XRes * YRes * Bpp)/8
    //           +--- XRes / 8
    //           V
    memory = (((m->mdXRes >> 3) * m->mdBPP) * m->mdYRes);
    memory += 0xFFFF;               // Take to next 64K bank size
    memory >>= 16;                  // Divide by 64K for number of banks
    
    // memory now has the 64K blocks necessary for this mode.
    
    if (((U016)memory) > OEMGetMemSize())
        return((U008)0);
    return((U008)1);
}

// some CRTC bit definitions
// CR07
#define VT08    1
#define VT09    0x20
#define VRS08   0x04
#define VRS09   0x80
#define EXT_VRS 0x84
#define VDE08   0x02
#define VDE09   0x40
#define VBS08   0x08
// CR09
#define VBS09   0x20
// CR25
#define VT10    0x01
#define VDE10   0x02
#define VRS10   0x04
#define VBS10   0x08

VOID FlatPanelCrtc( XRes )
{
U032    DacHcrtc, DacHt, Ht, Hrs, Hre, DacVcrtc, DacVt, Vt, Vrs, Vre, Vbs, data32;
U016    Attr, Seq;
U008    Cr07, Cr09, Cr11, Cr25;

    // check for pixel doubled modes
    if (XRes < 640)
    {
        Seq = ReadIndexed(NV_PRMVIO_SRX, 1);
        Seq |= 0x800;   // divide pixel clock to double size
        WriteIndexed(NV_PRMVIO_SRX, Seq);
    }        
    // Make sure attr is set correctly
    Cr07 = REG_RD08(NV_PRMCIO_INP0__COLOR); // read 3DA to toggle (data = don't care)
    REG_WR08(NV_PRMCIO_ARX, 0x30);          // select reg 0x10, bit 5 set to access reg (not palette)
    REG_WR08(NV_PRMCIO_ARX, 0x01);          // clear bit 5

    // New rules
    // VRS = VT - 3
    // VRE = VT - 2
    // HRS = HT - 3
    // HRE = HT - 2
    Ht = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x00)>>8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)((Ht-3)<<8 | 0x04));   // HRS
    Hre = ((ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x05)>>8) & 0xe0);    // read CR05 and mask 4:0
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(((Ht-2) | Hre)<<8 | 0x05));   // HRE = Ht - 2, CR04[4:0]
    
    // gather VT bits
    Vt = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x06)>>8);
    Cr07 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x07)>>8);
    Cr25 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x25)>>8);
    if (Cr07 & VT08)
        Vt |= 0x0100;
    if (Cr07 & VT09)
        Vt |= 0x0200;
    if (Cr25 & VT10)
        Vt |= 0x0400;

    // VRS: CR10,7,25
    Vrs = Vt - 3;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Vrs<<8 | 0x10));
    Cr07 &= ~EXT_VRS;
    if (Vrs & 0x100) Cr07 |= VRS08;
    if (Vrs & 0x200) Cr07 |= VRS09;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr07<<8 | 0x07));
    if (Vrs & 0x400) Cr25 |= VRS10;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr25<<8 | 0x25));
    
    // VRE: CR11[3:0]
    Vre = (Vt - 2) & 0x0f;
    Cr11 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x11)>>8);
    Vre |= (Cr11 & 0xf0);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Vre<<8 | 0x11));
    
    // VBS = VRS
    Cr09 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x09)>>8);
    Vbs = Vrs;
                
    // write VBS            
    Cr07 &= ~VBS08;
    if (Vbs & 0x100) Cr07 |= VBS08;
    Cr09 &= ~VBS09;
    if (Vbs & 0x200) Cr09 |= VBS09;
    Cr25 &= ~VBS10;
    if (Vbs & 0x400) Cr25 |= VBS10;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Vbs<<8 | 0x15));
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr07<<8 | 0x07));
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr09<<8 | 0x09));
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr25<<8 | 0x25));
            
    // wait for vsync
    while (REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x08);     // wait for active display
    while (!(REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x08));  // wait for retrace start
    
    data32 = REG_RD32(NV_PRAMDAC_FP_DEBUG_0);           // usual macro won't help us here
    data32 &= ~(NV_PRAMDAC_FP_DEBUG_0_TEST_BOTH << 16);    // clear VCNTR bits
    REG_WR32(NV_PRAMDAC_FP_DEBUG_0, data32);
}

U008 getDisplayType(VOID)
{
    U016   rc;
               
    OEMEnableExtensions();          // Unlock the registers
    rc = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28);
    if (rc & 0x8000)    // is VGA slaved to something?
    {
        rc = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x33);
        if (rc & 0x0100)
            return DISPLAY_TYPE_FLAT_PANEL;   
        else
            return DISPLAY_TYPE_TV;      
    }
    else
        return DISPLAY_TYPE_MONITOR;    
}

U008 isTV(VOID)
{
    U016   rc;
               
    OEMEnableExtensions();          // Unlock the registers
    rc = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28);
    return((U008)((rc >> 8) & 0x80));
}

U008 isPAL(VOID)
{
    U016   rc;
               
    rc = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x2C);   // BIOS sets 2c bit 5 for PAL, bit 4 for NTSC
    return((U008)((rc >> 8) & 0x20));
}

//
//       OEMGetMemSize - Return the pre-calculated memory size
//
//       Entry:  None
//       Exit:   AX = Number of 64K blocks
//
U016 OEMGetMemSize(VOID)
{

#ifdef DEBUG
    // This should never happen, since RamSize was set during initMapping
    if (!pDev->Framebuffer.RamSize) {
        DBG_BREAKPOINT();
    }
#endif

    return (pDev->Framebuffer.RamSize >> 16);
}          


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
    
// And the following routines will become "throwaway" code, as they
// deal with the saving and restoring of the IO state during the
// accesses to the PRIV_IO.

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
//       SaveNVAccessState - Save the state of the NV real mode access registers
//
//       Entry:  ds = seg0
//               Extended CRTC registers unlocked
//       Exit:   cx  = CRTC address
//               eax = 32-bit NV address
//               bh  = real mode access control bits
//               bl  = real mode access register (NVRM_ACCESS)
//               dx  = LSW of 32-bit data register
//               Real mode access registers set to address
VOID SaveNVAccessState(ACCESS_STATE *as)
{                      
    U008   rc;
    
    U016  wval;
    U032   dval;
           
/*            
    as->crtc_addr = 0x3D4;           // Set 3D4 by default
    as->access_reg = 0x38;           // Set CR38
                                            
    // rc = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x38);
    dval = NV_PRMCIO_CRX__COLOR;
                           
    // Output the MSW of the address    
    outpw(0x3D2, (U016)((dval >> 16) & 0xFFFF));
                           
    // and the LSW of the address   
    outpw(0x3D0, (U016)(dval & 0xFFFF));
                                
    // Select the data port
    outpw(as->crtc_addr, (((NVRM_ACCESS_MASK | NVRM_DATA) << 8) | (as->access_reg)));
                                                                     
    outp(0x3D0, 0x38);              // Select the register
    wval = inpw(0x3D0);             // This data is Byte/Word accessable
    
    as->access_reg = (U008)(wval & 0xFF);
    as->access_bits = (U008)((wval >> 8) & 0xFF);
                               
    // Select the data port
    outpw(as->crtc_addr, (((NVRM_ACCESS_MASK | NVRM_DATA) << 8) | (as->access_reg)));
                                                                     
    as->data_low = inpw(0x3D0);      // Save lsw of 32-bit data
                               
    // Get the address next
    outpw(as->crtc_addr, (((NVRM_ACCESS_MASK | NVRM_ADDRESS) << 8) | (as->access_reg)));
                                                                        
    as->nv_address = inpw(0x3D2);    // Read MSW of address
    as->nv_address <<= 16;           // Put in high word.
    as->nv_address |= inpw(0x3D0);   // Read LSW of address
*/
    
}


//       RestoreNVAccessState - Restore the state of the NV real mode access registers
//
//       Entry:  ds = seg0
//               Extended CRTC registers unlocked
//               cx  = CRTC address
//               eax = 32-bit NV address
//               bh  = real mode access control bits
//               bl  = real mode access register (NVRM_ACCESS)
//               dx  = LSW of 32-bit data register
//       Exit:   none
VOID RestoreNVAccessState(ACCESS_STATE *as)
{
/*
    // Get the address next
    outpw(as->crtc_addr, (((NVRM_ACCESS_MASK | NVRM_ADDRESS) << 8) | (as->access_reg)));
    outpw(0x3D2, (U016)(as->nv_address >> 16));       // MSW of address
    outpw(0x3D0, (U016)(as->nv_address & 0xFFFF));    // LSW of address

    // Select the buffered data port
    outpw(as->crtc_addr, (((NVRM_ACCESS_MASK | NVRM_DATA32) << 8) | (as->access_reg)));
                                                                       
    outpw(0x3D0, as->data_low);     // LSW of address
    
    outpw(as->crtc_addr, (((as->access_bits) << 8) | (as->access_reg)));
*/    
}

/**********************************************************************/
/**********************************************************************/

//
// POST the chip, just like the BIOS normally does
//
#ifdef KJK
//
// Needs completely rewritten for NV4
//
VOID NvPost()
{
    U032 memconfig;
    U032 val32;

    //////////////////////////////////////////////
    //
    // Taken from BIOS POST sequence
    //
    
    //
    // Enable requisite engines
    //
    WritePriv32(0x11111111, NV_PMC_ENABLE);
    
    //
    // Program 100MHz MPLL clocking
    //
    if (ReadPriv32(NV_PEXTDEV_BOOT_0) & 0x00000040)
        WritePriv32(0x0001C40E, NV_PRAMDAC_MPLL_COEFF);    
    else
        WritePriv32(0x0001A30B, NV_PRAMDAC_MPLL_COEFF);    

    //
    // Program PFB Config to defaults
    //
    WritePriv32( DRF_DEF(_PFB,_CONFIG_0,_RESOLUTION,_DEFAULT)
               | DRF_DEF(_PFB,_CONFIG_0,_PIXEL_DEPTH,_DEFAULT)
               | DRF_DEF(_PFB,_CONFIG_0,_TILING,_DISABLED)
               | DRF_DEF(_PFB,_CONFIG_0,_TILING_DEBUG,_DISABLED), NV_PFB_CONFIG_0);
    
    //
    // Program VPLL clocking
    //
    if (ReadPriv32(NV_PEXTDEV_BOOT_0) & 0x00000040)
        WritePriv32(0x00028D0E, NV_PRAMDAC_MPLL_COEFF);    
    else
        WritePriv32(0x0001400C, NV_PRAMDAC_MPLL_COEFF);    
    
    //
    // Program PFB Power control
    //
    WritePriv32(DRF_DEF(_PFB,_GREEN_0,_LEVEL,_VIDEO_ENABLED), NV_PFB_GREEN_0);    

    //
    // Program memory timings based on framebuffer size
    //
    //
    
    //
    // Determine how wide/deep is the framebuffer
    //
    /*  
     Memory layout:

           4mb 128-bit     33221100 77665544 bbaa9988 ffeeddcc
           2mb 128-bit     33221100 77665544 xxxxxxxx xxxxxxxx

           128-bit covers address 0-3fffff in both 2mb and 4mb configs,
           but the second 2 dwords are garbage on 2mb boards.
              
           4mb 64-bit      33221100 77665544 bbaa9988 ffeeddcc
           2mb 64-bit      33221100 77665544 bbaa9988 ffeeddcc

           64-bit covers address 0-3fffff in 4mb configs, while
           64-bit covers address 0-1fffff in 2mb configs. All bytes are
           addressed.
    */
    
    memconfig = ReadPriv32(NV_PFB_BOOT_0);
    memconfig &= 0xFFFFFFFC;
    memconfig |= DRF_DEF(_PFB, _BOOT_0, _RAM_AMOUNT, _DEFAULT); // mask to 4meg, keep bus width
  
    //
    // Poke the framebuffer based on bus width
    //    
    if (memconfig & DRF_DEF(_PFB, _BOOT_0, _RAM_WIDTH_128, _ON))
    {
        //
        // 128bit test
        //
        WriteFb32('NV3A', 0x00200008);  // 2meg+8
    
        //
        // 4meg?
        //
        if (ReadFb32(0x00200008) != 'NV3A')
            //
            // Assume 2meg
            //
            memconfig = (memconfig & 0xFFFFFFFC) | DRF_DEF(_PFB, _BOOT_0, _RAM_AMOUNT, _2MB);
    
    } else {
    
        //
        // 64bit test
        //
        WriteFb32('NV3A', 0x00100000);  // 1meg
        WriteFb32('NV3B', 0x00300000);  // 3meg
        
        //
        // 4meg?
        //
        if (ReadFb32(0x00300000) != 'NV3B')
            //
            // Assume 2meg
            //
            memconfig = (memconfig & 0xFFFFFFFC) | DRF_DEF(_PFB, _BOOT_0, _RAM_AMOUNT, _2MB);
        
            //
            // 2meg?
            //
            if (ReadFb32(0x00100000) != 'NV3A')
                //
                // Assume 1meg
                //
                ;//memconfig = (memconfig & 0xFFFFFFFC) | DRF_DEF(_PFB, _BOOT_0, _RAM_AMOUNT, _1MB);
        
    }
    
    //
    // Write the memory config
    //
    WritePriv32(memconfig, NV_PFB_BOOT_0);
    
    // 
    // Now program the appropriate memory timings
    //
    if ((memconfig & 3) == DRF_DEF(_PFB, _BOOT_0, _RAM_AMOUNT, _2MB))
    {
        //
        // 2meg timings
        //
        WritePriv32(DRF_DEF(_PFB,_DELAY_1,_WRITE_ENABLE_RISE,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_WRITE_ENABLE_FALL,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_CAS_ENABLE_RISE,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_CAS_ENABLE_FALL,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_OUTPUT_DATA,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_RAS_ENABLE,_0), NV_PFB_DELAY_1);
                  
        WritePriv32(DRF_DEF(_PFB,_DEBUG_0,_CKE_ALWAYSON,_ON)
                  | DRF_DEF(_PFB,_DEBUG_0,_CKE_INVERT,_OFF)
                  | DRF_DEF(_PFB,_DEBUG_0,_CASOE,_ENABLED)
                  | DRF_DEF(_PFB,_DEBUG_0,_REFRESH_COUNTX64,_DEFAULT)
                  | DRF_DEF(_PFB,_DEBUG_0,_REFRESH,_ENABLED)
                  | DRF_DEF(_PFB,_DEBUG_0,_PAGE_MODE,_ENABLED), NV_PFB_DEBUG_0);
                  
        WritePriv32(DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_3)
                  | DRF_DEF(_PFB,_CONFIG_1,_SGRAM100,_ENABLED)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_RAS,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_PCHG,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_LOW,_DEFAULT) 
                  | DRF_DEF(_PFB,_CONFIG_1,_MRS_TO_RAS,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_WRITE_TO_READ,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_TO_CAS_M1,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_READ_TO_WRITE,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_READ_TO_PCHG,_ON), NV_PFB_CONFIG_1);
                  
        WritePriv32(DRF_DEF(_PFB,_RTL,_S,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_V,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_M,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_H,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_A,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_G,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_ARB_GR_HI_PRIOR,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_ARB_MEDIA_HI_PRIOR,_DEFAULT), NV_PFB_RTL);
                  
        //
        // Program 100MHz MPLL clocking
        //
        if (ReadPriv32(NV_PEXTDEV_BOOT_0) & 0x00000040)
            WritePriv32(0x0001C40E, NV_PRAMDAC_MPLL_COEFF);    
        else
            WritePriv32(0x0001A30B, NV_PRAMDAC_MPLL_COEFF);    
                  
    } else {
        //
        // 4meg
        //
        WritePriv32(DRF_DEF(_PFB,_DELAY_1,_WRITE_ENABLE_RISE,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_WRITE_ENABLE_FALL,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_CAS_ENABLE_RISE,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_CAS_ENABLE_FALL,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_OUTPUT_DATA,_0)
                  | DRF_DEF(_PFB,_DELAY_1,_RAS_ENABLE,_0), NV_PFB_DELAY_1);
                  
        WritePriv32(DRF_DEF(_PFB,_DEBUG_0,_CKE_ALWAYSON,_ON)
                  | DRF_DEF(_PFB,_DEBUG_0,_CKE_INVERT,_OFF)
                  | DRF_DEF(_PFB,_DEBUG_0,_CASOE,_ENABLED)
                  | DRF_DEF(_PFB,_DEBUG_0,_REFRESH_COUNTX64,_DEFAULT)
                  | DRF_DEF(_PFB,_DEBUG_0,_REFRESH,_ENABLED)
                  | DRF_DEF(_PFB,_DEBUG_0,_PAGE_MODE,_ENABLED), NV_PFB_DEBUG_0);
    
        WritePriv32(DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_3)
                  | DRF_DEF(_PFB,_CONFIG_1,_SGRAM100,_ENABLED)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_RAS,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_PCHG,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_LOW,_DEFAULT) 
                  | DRF_DEF(_PFB,_CONFIG_1,_MRS_TO_RAS,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_WRITE_TO_READ,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_RAS_TO_CAS_M1,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_READ_TO_WRITE,_DEFAULT)
                  | DRF_DEF(_PFB,_CONFIG_1,_READ_TO_PCHG,_ON), NV_PFB_CONFIG_1);
    
        WritePriv32(DRF_DEF(_PFB,_RTL,_S,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_V,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_M,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_H,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_A,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_G,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_ARB_GR_HI_PRIOR,_DEFAULT)
                  | DRF_DEF(_PFB,_RTL,_ARB_MEDIA_HI_PRIOR,_DEFAULT), NV_PFB_RTL);
    
        //
        // Program 100MHz MPLL clocking
        //
        if (ReadPriv32(NV_PEXTDEV_BOOT_0) & 0x00000040)
            WritePriv32(0x0001C40E, NV_PRAMDAC_MPLL_COEFF);    
        else
            WritePriv32(0x0001A30B, NV_PRAMDAC_MPLL_COEFF);    
    
    }
    
    //
    // Enable clocking
    //
    WritePriv32(DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_DLL_BYPASS,_FALSE)
              | DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_MPLL_SOURCE,_PROG)
              | DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_MPLL_BYPASS,_FALSE)
              | DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VPLL_SOURCE,_DEFAULT)
              | DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VPLL_BYPASS,_FALSE) 
              | DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_PCLK_SOURCE,_VPLL)
              | DRF_DEF(_PRAMDAC,_PLL_COEFF_SELECT,_VCLK_RATIO,_DB1), NV_PRAMDAC_PLL_COEFF_SELECT);
            
    //
    // Setup optimal PCI interface
    //
    WritePriv32(DRF_DEF(_PBUS,_DEBUG_1,_PCIM_THROTTLE,_DISABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIM_CMD,_SIZE_BASED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIM_AGP,_IS_AGP)
              | DRF_DEF(_PBUS,_DEBUG_1,_AGPM_CMD,_LP_ONLY)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_WRITE,_0_CYCLE)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_2_1,_ENABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RETRY,_ENABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RD_BURST,_ENABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_WR_BURST,_ENABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_EARLY_RTY,_ENABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_RMAIO,_DISABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_PCIS_CPUQ,_ENABLED)
              | DRF_DEF(_PBUS,_DEBUG_1,_SPARE1,_ZERO)
              | DRF_DEF(_PBUS,_DEBUG_1,_SPARE2,_ZERO), NV_PBUS_DEBUG_1);                  


    //
    // Make sure hsync and vsync are on (we only set them in DPMS calls)
    //
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A) & 0x3FFF) | 0x0000);
    
    //
    // Boot with TV off, and set for NTSC if it was on.
    // Also set horizontal pixel adjustment
    // Sets pixel format to VGA
    //
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28) & 0x00FF) | 0x0000);
    
    //
    // Float DDC pins high (not touched during a mode set or save/restore state)
    //
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x3F) & 0x00FF) | 0x3300);

    //
    // Disable DDC
    //
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x3F) & 0x00FF) | 0x3200);

    //
    // Program NTSC TV settings
    //
    WritePriv32(0x00000006, NV_PRAMDAC_VSERR_WIDTH); 
    WritePriv32(0x0000000C, NV_PRAMDAC_VEQU_END);
    WritePriv32(0x0000000C, NV_PRAMDAC_VBBLANK_END);
    WritePriv32(0x00000024, NV_PRAMDAC_VBLANK_END);
    WritePriv32(0x0000020C, NV_PRAMDAC_VBLANK_START);
    WritePriv32(0x00000206, NV_PRAMDAC_VBBLANK_START);
    WritePriv32(0x00000207, NV_PRAMDAC_VEQU_START);
    WritePriv32(0x0000020D, NV_PRAMDAC_VTOTAL);
    WritePriv32(0x0000003F, NV_PRAMDAC_HSYNC_WIDTH);
    WritePriv32(0x00000048, NV_PRAMDAC_HBURST_START);
    WritePriv32(0x0000006A, NV_PRAMDAC_HBURST_END);
    WritePriv32(0x0000034A, NV_PRAMDAC_HBLANK_START);
    WritePriv32(0x00000088, NV_PRAMDAC_HBLANK_END);
    WritePriv32(0x0000035A, NV_PRAMDAC_HTOTAL);
    WritePriv32(0x0000001F, NV_PRAMDAC_HEQU_WIDTH);
    WritePriv32(0x0000016E, NV_PRAMDAC_HSERR_WIDTH);

    //
    // If strapped to PAL, program PAL TV settings
    //
    if ((ReadPriv32(NV_PEXTDEV_BOOT_0) & 0x00000180) == 0x00000100)
    {
    
        WritePriv32(0x00000005, NV_PRAMDAC_VSERR_WIDTH);
        WritePriv32(0x0000000A, NV_PRAMDAC_VEQU_END);
        WritePriv32(0x0000000A, NV_PRAMDAC_VBBLANK_END);
        WritePriv32(0x0000002C, NV_PRAMDAC_VBLANK_END);
        WritePriv32(0x0000026C, NV_PRAMDAC_VBLANK_START);        
        WritePriv32(0x00000269, NV_PRAMDAC_VBBLANK_START);
        WritePriv32(0x0000026C, NV_PRAMDAC_VEQU_START);
        WritePriv32(0x00000271, NV_PRAMDAC_VTOTAL);
        WritePriv32(0x0000003F, NV_PRAMDAC_HSYNC_WIDTH);
        WritePriv32(0x00000048, NV_PRAMDAC_HBURST_START);
        WritePriv32(0x0000006A, NV_PRAMDAC_HBURST_END);
        WritePriv32(0x00000350, NV_PRAMDAC_HBLANK_START);
        WritePriv32(0x00000092, NV_PRAMDAC_HBLANK_END);
        WritePriv32(0x00000360, NV_PRAMDAC_HTOTAL);
        WritePriv32(0x0000001F, NV_PRAMDAC_HEQU_WIDTH);
        WritePriv32(0x0000016E, NV_PRAMDAC_HSERR_WIDTH);
    }
    
}
#endif // KJK

/**********************************************************************/
/**********************************************************************/


//
// The following MAIN() routine is only used for standalone DOS compiles
//                                   
#ifdef DOS_COMPILE                                                 
VOID main(U016 ac, U008 **av)
{
    U016  mode;                   // Mode to be set
    U016  rc;                     // Return code
    
    printf("NVIDIA MODESET V1.00\n\n");
    
    if (ac < 2)
    {
        printf("Usage: modeset vmode\n");
        printf("       vmode - Vesa Mode Number\n");
        exit(1);
    }           
    
    if ((mode = h2i(*(++av))) == 0xFFFF)
    {
        printf("Error! Bad Vesa Mode Number\n");
        exit(2);
    }        
    
    printf("Setting Mode 0x%02X\n", mode);
    getchar();
    
    rc = VBESetMode(mode);
    
    if (((rc & 0xFF) != 0x4F) || ((rc & 0xFF00) >> 8))
    {
        printf("Error! Could Not Set Mode 0x%02X [rc=%04X]\n", mode, rc);
        exit(3);
    }        
                                                      
    exit(0);
}

//
// h2i - convert Hex string to an Unsigned Integer
U016 h2i(U008 *str)                 
{
    U016  sus = 0;                // Starting unsigned short
    U016  i;
    U016  val;                    // Conversion value
    
    // Since we only do U016's, we have a max of 0xFFFF-1
    if ((i = strlen(str)) > 4)
        return(0xFFFF);
    
    while (i--)
    {    
        val = toupper(*str) - 0x30;  // Convert Hex 0-9
        if (val > 9)
        {
            val -= 7;               // Take 7 more to convert A-F
            if (val < 0x0A || val > 0x0F)
                return(0xFFFF);     // Error!
        }                                  
        sus *= 16;                  // Shift by 4
        sus += val;             
        str++;
    }                                                 
    return(sus);
}
#endif // DOS_COMPILE
 


//****************************************************************************************
//
//  Program the I2C device for a display mode
//  returns:    status word
U016 setI2C(MODEDESC *minfo)
{
U016     status;

#ifdef IKOS
    return ERR_I2C_COMM;
#endif
    // Identify which part: Chrontel or Brooktree?
    // ID was done at fbinit.
    i2cInit(pDev->Dac.TVOutPortID);
    switch (pDev->Framebuffer.EncoderID)
    {
        case 0x88:
        case 0x8A:
            status = programBrooktree(minfo->mdTVMode);
            break;
        case 0xEA:
        case 0xEC:
            status = programChrontel(minfo->mdTVMode);
            break;                        
    }
    return (status);
}

#define	HSYNCWIDTH	0x3f	// bits we must preserve	
//****************************************************************************************
//
//  Program the Brooktree device for a display mode
//  args:       internal TV mode
//  returns:    status word
U016 programBrooktree(U016 TVmode)
{
U016    i, status;
U008    *strptr;
U008    Data, Adr;

    // to send a string to the I2C: send the start sequence, send the string, send the stop sequence

    if (pDev->Framebuffer.DisplayType != DISPLAY_TYPE_TV)    // can be MONITOR, TV, or FLAT PANEL
    {
        TVmode = TV_MODE_640x480;   // program the encoder anyway, to get synched display on TV
    }        

    // send an init string
    i2cStart(pDev->Dac.TVOutPortID);                       // start state for I2C
    strptr = (U008 *)&BT_Init;          // send init sequence
    status = i2cSendByte(pDev->Dac.TVOutPortID, (U008)pDev->Framebuffer.EncoderID);   // send ID
    strptr++;                                  // string includes ID, skip it
    for (i=0; *strptr != 0xff; i++)
    {
        status = i2cSendByte(pDev->Dac.TVOutPortID, *strptr++);  
        if (status)
            break;
    }     
    i2cStop(pDev->Dac.TVOutPortID);                         // end state for I2C

    // get a pointer to the string to write to the I2C controller
    if (TVformat == NTSC)
        strptr = (U008 *)BT_NTSC[TVmode];
    else
        strptr = (U008 *)BT_PAL[TVmode];

    // send a timing string
    i2cStart(pDev->Dac.TVOutPortID);                       // start state for I2C
    status = i2cSendByte(pDev->Dac.TVOutPortID, (U008)pDev->Framebuffer.EncoderID);   // send ID
    strptr++;                                  // string includes ID, skip it
    for (i=0; *strptr != 0xff; i++)
    {
        status = i2cSendByte(pDev->Dac.TVOutPortID, *strptr++);
        if (status)
            break;
    }
    i2cStop(pDev->Dac.TVOutPortID);                           // end state for I2C

    // If not TV, blank TV image
    if (pDev->Framebuffer.DisplayType != DISPLAY_TYPE_TV)    // can be MONITOR, TV, or FLAT PANEL
    {
        Data = 0x07;                // set luminance to 0 gain
        Adr = 0xca;
        i2cWrite(pDev->Dac.TVOutPortID, (U008)(pDev->Framebuffer.EncoderID >> 1), (U016) 1, &Adr, 1, &Data);
        Data = 0x07;                // set chroma to 0 gain
        Adr = 0xcc;
        i2cWrite(pDev->Dac.TVOutPortID, (U008)(pDev->Framebuffer.EncoderID >> 1), (U016) 1, &Adr, 1, &Data);
    }
    return (status);
}

//****************************************************************************************
//
//  Program the Chrontel device for a display mode
//  args:       internal TV mode
//  returns:    status word
U016 programChrontel(U016 TVmode)
{    
U016    i, status;
U008    *strptr;
U008    *initptr;

    if (pDev->Framebuffer.EncoderType != NV_ENCODER_CHRONTEL_7003) 
    {
        // The only Chrontel encoder we know how to program is 7003.
        return ERR_I2C_UNKNOWN;
    }

    // to send a string to the I2C: send the start sequence, send the string, send the stop sequence

    if ((pDev->Framebuffer.DisplayType != DISPLAY_TYPE_TV) | (TVmode == 0xff))    // can be MONITOR, NTSC, or PAL
    {
        TVmode = TV_MODE_640x480;   // program the encoder anyway, to get synched display on TV
    }        
    
    // send an init string
    i2cStart(pDev->Dac.TVOutPortID);                   // start state for I2C
    strptr = (U008 *)&I2C_Init;     // send init sequence
    status = i2cSendByte(pDev->Dac.TVOutPortID, (U008)pDev->Framebuffer.EncoderID);   // send ID
    strptr++;                                  // string includes ID, skip it
    for (i=0; *strptr != 0xff; i++)
    {
//      DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, " ", *initptr);
        status = i2cSendByte(pDev->Dac.TVOutPortID, *strptr++);  
        if (status)
            break;
    }     
    i2cStop(pDev->Dac.TVOutPortID);                     // end state for I2C

    // get a pointer to the string to write to the I2C controller
    if (TVformat == NTSC)
        strptr = (U008 *)i2c_NTSC[TVmode];
    else
        strptr = (U008 *)i2c_PAL[TVmode];
            
    // send a timing string
    i2cStart(pDev->Dac.TVOutPortID);                   // start state for I2C
    i2cSendByte(pDev->Dac.TVOutPortID, (U008)pDev->Framebuffer.EncoderID);   // send ID
    strptr++;                                  // string includes ID, skip it
    for (i=0; *strptr != 0xff; i++)
    {
//        DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, " ", *strptr);
        status = i2cSendByte(pDev->Dac.TVOutPortID, *strptr++);
        if (status)
            break;
    }
    i2cStop(pDev->Dac.TVOutPortID);                     // end state for I2C
    return (status);
}


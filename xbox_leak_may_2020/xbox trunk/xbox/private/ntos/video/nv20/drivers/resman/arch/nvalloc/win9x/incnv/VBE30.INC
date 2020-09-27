;       Copyright (C) 1996-1997 NVidia Corporation
;
;       VBE30.INC - VESA VBE 3.0 constants, structure and data definitions
;
;       Written by:     Mark Stephen Krueger
;       Date:           10/10/96
;
;       $Revision: 1 $
;       $Date: 5/13/97 10:43a $
;       $Logfile: /resman.040/win96/vdd/VBE30.INC $
;       $Log: /resman.040/win96/vdd/VBE30.INC $
; 
; 1     5/13/97 10:43a Jeffwe
; 
; 1     5/12/97 6:49p Jeffwe
; 
; 3     1/23/97 11:02a Markk
; Removed tabs.
; 
; 2     11/19/96 2:03p Markk
; Fixed bug with size of ModeInfoBlock.
; 
; 1     10/16/96 9:16a Markk
; VESA VBE 3.0 definitions for use by the Mini-VDD.
; 
;
VESA_SET_MODE           equ     04F02h
;
VESA_SUCCESS            equ     004Fh        
VESA_NOSUCCESS          equ     014fh        ; Supported function unsuccessful.
;
; VBE_InfoBlock data structure (as defined by VBE 2.0 revision 0.3)
;
VBE_InfoBlock   struc
        VESASignature           db      'VESA'          ; VBE Signature
        VESAVersion             dw      200h            ; VBE Version
        OEMStringPtr            dd      ?               ; Pointer to OEM string
        VBE_Capabilities        db      4 dup (?)       ; Capabilities of graphics
        VBE_ModePtr             dd      ?               ; Pointer to VBE mode list
        VBE_TotalMemory         dw      ?               ; Number of 64K memory blocks
; VBE 2.0 stuff
        VBE_SoftwareRev         dw      ?               ; VBE BIOS Software revision
        VBE_VendorNamePtr       dd      ?               ; Pointer to Vendor Name string
        VBE_ProductNamePtr      dd      ?               ; Pointer to Product Name string
        VBE_ProductRevPtr       dd      ?               ; Pointer to Product Revision string
IFDEF VESA30_REFRESH
        VBE_DotClockPtr         dd      ?               ; Pointer to DotClockList
ENDIF ; VESA30_REFRESH
        Reserved                db      222 dup (?)     ; Reserved
        VBE_OEM_Data            db      256 dup (?)     ; Data Scratch for OEM strings
VBE_InfoBlock   ends
;
VBE_InfoBlock_102       struc   ; Old version of info block structure
        VESASignature_102       db      'VESA'          ; VBE Signature
        VESAVersion_102         dw      102h            ; VBE Version
        OEMStringPtrOff_102     dw      ?               ; Pointer to OEM string (offset)
        OEMStringPtrSeg_102     dw      ?               ; Pointer to OEM string (segment)
        Capabilities_102        dd      ?               ; Capabilities of graphics
        VideoModePtrOff_102     dw      ?               ; Pointer to VBE mode list (offset)
        VideoModePtrSeg_102     dw      ?               ; Pointer to VBE mode list (segment)
        TotalMemory_102         dw      ?               ; Number of 64K memory blocks
VBE_InfoBlock_102       ends
;
; VBE_ModeInfoBlock data structure (as defined by VBE 2.0 revision 0.3)
;
VBE_ModeInfoBlock       struc
; Mandatory (all revisions)
        ModeAttributes          dw      ?       ; Mode attributes
        WinAAttributes          db      ?       ; Window A attributes
        WinBAttributes          db      ?       ; Window B attributes
        WinGranularity          dw      ?       ; Window granularity
        WinSize                 dw      ?       ; Window size
        WinASegment             dw      ?       ; Window A start segment
        WinBSegment             dw      ?       ; Window B start segment
        WinFuncPtr              dd      ?       ; Pointer to window function
        BytesPerScanLine        dw      ?       ; Bytes per scan line
; Mandatory (VBE 1.20 and above)
        XResolution             dw      ?       ; Horizontal resolution
        YResolution             dw      ?       ; Vertical resolution
        XCharSize               db      ?       ; Character cell width
        YCharSize               db      ?       ; Character cell height
        NumberOfPlanes          db      ?       ; Number of memory planes
        BitsPerPixel            db      ?       ; Bits per pixel
        NumberOfBanks           db      ?       ; Number of banks
        MemoryModel             db      ?       ; Memory model type
        BankSize                db      ?       ; Bank size in KB
        NumberOfImagePages      db      ?       ; Number of images
        Reserved1               db      1       ; Reserved for page function
; Direct color fields (required for direct/6 and YUV/7 models)
        RedMaskSize             db      ?       ; Size of direct color red mask in bits
        RedFieldPosition        db      ?       ; Bit position of LSB of red mask
        GreenMaskSize           db      ?       ; Size of direct color green mask in bits
        GreenFieldPosition      db      ?       ; Bit position of LSB of green mask
        BlueMaskSize            db      ?       ; Size of direct color blue mask in bits
        BlueFieldPosition       db      ?       ; Bit position of LSB of blue mask
        ReservedMaskSize        db      ?       ; Size of direct color reserved mask in bits
        ReservedFieldPosition   db      ?       ; Bit position of LSB of reserved mask
        DirectColorModeInfo     db      ?       ; Direct color mode attributes
; Mandatory information (VBE 2.00 and above)
        PhysBasePtr             dd      ?       ; Physical address for flat frame buffer
        OffScreenMemPtr         dd      ?       ; Pointer to start of off-screen memory
        OffScreenMemPtrSize     dw      ?       ; Amount of off-screen memory in 1K units
IFDEF VESA30
; Optional information for VBE 3.0 and above (filled with zeroes if same in linear modes)
        LinBytesPerScanLine     dw      ?       ; Bytes per scan line for linear modes
        BnkNumberOfImagePages   db      ?       ; Number of images for banked modes
        LinNumberOfImagePages   db      ?       ; Number of images for linear modes
        LinRedMaskSize          db      ?       ; Size of direct color red mask (linear modes)
        LinRedFieldPosition     db      ?       ; Bit position of lsb of red mask (linear modes)
        LinGreenMaskSize        db      ?       ; Size of direct color green mask  (linear modes)
        LinGreenFieldPosition   db      ?       ; Bit position of lsb of green mask (linear modes)
        LinBlueMaskSize         db      ?       ; Size of direct color blue mask  (linear modes)
        LinBlueFieldPosition    db      ?       ; Bit position of lsb of blue mask (linear modes)
        LinRsvdMaskSize         db      ?       ; Size of direct color reserved mask (linear modes)
        LinRsvdFieldPosition    db      ?       ; Bit position of lsb of reserved mask (linear modes)
IFDEF VESA30_REFRESH
; Mandatory information (VBE 3.0 and above)
        MaxDotClock             dw      ?       ; Maximum dot clock for graphics mode
        DotClockScaleFactor     dw      ?       ; Dot clock scaling factor for mode
        IntDotClockScaleFactor  dw      ?       ; Interlaced dot clock scaling factor for mode
        Reserved2               db      188 dup (?)
ELSE ; VESA30_REFRESH
        Reserved2               db      194 dup (?)
ENDIF ; VESA30_REFRESH
ELSE ; VESA30
        Reserved2               db      206 dup (?)
ENDIF ; VESA30
VBE_ModeInfoBlock   ends
;
IFDEF VESA30_REFRESH
CRTCInfoBlock           struc
HorizontalTotal                 dw      ?       ; Normalized horizontal total in pixels
HorizontalSyncStart             dw      ?       ; Normalized horizontal sync start in pixels
HorizontalSyncEnd               dw      ?       ; Normalized horizontal sync end in pixels
VerticalTotal                   dw      ?       ; Normalized vertical total in lines
VerticalSyncStart               dw      ?       ; Normalized vertical sync start in lines
VerticalSyncEnd                 dw      ?       ; Normalized vertical sync end in lines
HorizontalSyncPolarity          db      ?       ; Horizontal sync polarity '+' or '-'
VerticalSyncPolarity            db      ?       ; Vertical sync polarity '+' or '-'
Flags                           db      ?       ; 0 for non-interlaced, 1 for interlaced
PhysDotClock                    dw      ?       ; Physical dot clock in KHz * 100 
RefreshRate                     dw      ?       ; Refresh rate in Hz * 100
Reserved                        db      40 dup (?)  ; remainder of ModeInfoBlock
CRTCInfoBlock           ends
ENDIF ; VESA30_REFRESH
;
VBEStateSave    struc
        SeqIndex        db      ?               ; SEQ index
        Miscreg         db      ?               ; Extended misc reg (3C5[11])
        GraIndex        db      ?               ; GDC index
        CRTIndex        db      ?               ; CRTC index
        ATCToggle       db      ?               ; ATC toggle state
        ATCIndex        db      ?               ; ATC index register
        OvrColH         db      ?               ; Overscan Color High (3C0[15])
        BankSelReg      db      ?               ; Bank Select
        LowSeqRegs      db      2 dup (?)       ; SEQ[5..6]
        HighSeqRegs     db      5 dup (?)       ; SEQ[12..16]
        CRTRegs         db      5 dup (?)       ; CRTC[19..1D]
        GDCRegs         db      8 dup (?)       ; GDC[9..10]
        DACMode         db      ?               ; DAC Mode control
        FreqIndex       db      ?               ; Frequency synthesizer index
        FreqRegs        dw      8 dup (?)       ; Frequency synthesizer clock words
        DACAuxCtrl      db      ?               ; DAC auxiliary and power down control
VBEStateSave    ends
;
; Number of 64-byte blocks needed for save/restore state information
;
if ((size VBEStateSave) MOD 64) GT 0
  SVGABLOCKS    equ     (size VBEStateSave)/64 + 1
else
  SVGABLOCKS    equ     (size VBEStateSave)/64
endif


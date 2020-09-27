
;******************************* Module Header *********************************
;
; Module Name: NV4TEXT.ASM
;
; Contains the x86 'Asm' versions of some inner-loop routines for the
; partially hardware accelerated text blt.
;
; Copyright (c) 1994-1995 Microsoft Corporation
;
;*******************************************************************************

;*******************************************************************************
;*                                                                             *
;*   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
;*                                                                             *
;*******************************************************************************

        .386

        .model  small,c

        assume cs:FLAT,ds:FLAT,es:FLAT,ss:FLAT
        assume fs:nothing,gs:nothing

        .xlist
        include stdcall.inc             ;calling convention cmacros
        include i386\strucs.inc
        .list

FALSE                       EQU 0
TRUE                        EQU 1
LLNULL                      EQU 0ff00ff00h
FONT_REJECTED_TAG           EQU 0ffffffffh
FONTFIXEDPITCHSHIFT         EQU 7
FONTFIXEDPITCH              EQU 1 SHL FONTFIXEDPITCHSHIFT
MAXFONTCACHESIZE            EQU FONTFIXEDPITCH * MAXGLYPHS
MAXGLYPHSIZE                EQU (FONTFIXEDPITCH-4)
MAXGLYPHS                   EQU 256
MAXGLYPHINDEX               EQU 255
MAXFONTS                    EQU 16

FontCacheInfoStruc struc

    FontCacheId         dd  LLNULL
    FontPitchAndOffset  dd  LLNULL
    pFontInfoPrev       dd  LLNULL
    pFontInfoNext       dd  LLNULL
    FontCacheGlyphInfo  dd  MAXGLYPHS dup (0)

FontCacheInfoStruc ends

FONTCACHEINFOHEADERSIZE EQU 16      ; size of first 4 DWORDS in FontCachInfoStruc
                                    ; before glyph data

        .data

LRUFontInfoNode         dd  LLNULL
MRUFontInfoNode         dd  LLNULL


; Remove structure since font caching is turned off. This buffer should be
; allocated at initialization instead of here anyway.
;
;FontCacheInfo           db (MAXFONTS * sizeof(FontCacheInfoStruc)) dup (0)
FontCacheInfo           dd ?

;
;  stack based params and local variables
;


NV4_DrawFastGlyphs_STRUC          struc

; Feel free to add any local variables here:

gWidth                  dd      ?
localNvDmaCount         dd      ?
localNvDmaFifoPtr       dd      ?
missingGlyphFlag        dd      ?
numBytesPerScan         dd      ?
numDwordsPerGlyph       dd      ?
originalPgpParm         dd      ?
ptlOriginx              dd      ?   ;PointL
ptlOriginy              dd      ?
clipRight               dd      ?
clipBottom              dd      ?
saveESI                 dd      ?
saveEDX                 dd      ?
saveGlyphCount          dd      ?
saveNvDmaCount          dd      ?
savePgp                 dd      ?
savePpdev               dd      ?
ulCharIncLocal          dd      ?
fontInfoBuffer          db      (size FONTINFO) dup (?)

; Don't add any fields below here without modifying XXX_STRUC_SIZE!

DFG_ebp                 dd      ?
DFG_esi                 dd      ?
DFG_edi                 dd      ?
DFG_ebx                 dd      ?
DFG_RetAddr             dd      ?
pso                     dd      ?
pstro                   dd      ?
pfo                     dd      ?
prclClip                dd      ?
pgpParm                 dd      ?
glyphCount              dd      ?

NV4_DrawFastGlyphs_STRUC          ends

NV4_DrawFastGlyphs_PARM_SIZE     equ     11 * 4


        .code

extrn       FONTOBJ_vGetInfo@12:NEAR
extrn       NV_DmaPush_Wrap@8:NEAR
extrn       NV_DmaPush_CheckWrapped@8:NEAR


;******************************************************************************
;
;   CheckDMAFreeCount - Get current free count in DMA buffer. If not enough
;       space, then calls Vidal's routines to make sure there is room in
;       the DMA buffer. Then gets new DMA buffer free count. This routine is
;       similar to the NV_DMAPUSH_CHECKFREE C macro.
;
;   NOTE: requires ppdev.nvDmaCount to equal local nvDmaCount
;
;   INPUT:  EBP - stack pointer
;           ECX - number of DWORDS needed in DMA buffer
;   OUTPUT: EAX = free count
;           updates localNvDmaCount when ppdev->nvDmaCount is updated
;
;******************************************************************************

align 4

CheckDMAFreeCount   proc

        mov     ebx, [ebp].savePpdev        ; ppdev
        mov     edx, [ebx].nvDmaCount       ; ppdev->nvDmaCount
        cmp     [ebx].nvDmaWrapFlag, FALSE
        jne     short CheckDmaWrapped

    ;*************************************************************************
    ; WRAP_FLAG = FALSE. Check if we need to wrap around.
    ;
    ; EBX - ppdev
    ; EBP - stack pointer
    ; ECX - number of DWORDS needed in DMA buffer
    ; EDX - nvDmaCount
    ;
    ;*************************************************************************

        mov     eax, [ebx].DmaPushBufCanUseSize ;
        sub     eax, edx                    ; less current nvDmaCount
        sub     eax, ecx                    ; less number of DWORDS needed
        jl      GoWrapDmaBuffer
        
        mov     [ebx].nvDmaCachedFreeCount, eax
        ret                                 ; enough room - return new freecount

    ;*************************************************************************
    ; Need to wrap.
    ;
    ; EBX - ppdev
    ; EBP - stack pointer
    ; ECX - number of DWORDS needed in DMA buffer
    ;
    ;*************************************************************************

GoWrapDmaBuffer:
        push    ecx                         ; save dwords needed
        push    ecx                         ; DWORDS needed - parameter
        push    ebx                         ; ppdev - parameter
        call    NV_DmaPush_Wrap@8           ; (ppdev, DWORDS_NEEDED)
        pop     ecx                         ; restore dwords needed
        mov     ebx, [ebp].savePpdev
        mov     edx, [ebx].nvDmaCount
        mov     [ebp].localNvDmaCount, edx  ; update local nvDmaCount

CheckDMADone:
        mov     [ebx].nvDmaCachedFreeCount, 0
        ret

    ;*************************************************************************
    ; WRAP_FLAG = TRUE. We are currently wrapped around (DMA count < Get Ptr)
    ;
    ; EBX - ppdev
    ; EBP - stack pointer
    ; ECX - number of DWORDS needed in DMA buffer
    ;
    ;*************************************************************************

CheckDmaWrapped:

        push    ecx                         ; save dwords needed
        push    ecx                         ; DWORDS needed - parameter
        push    ebx                         ; ppdev - parameter
        call    NV_DmaPush_CheckWrapped@8   ; (ppdev, DWORDS_NEEDED)
        pop     ecx                         ; restore dwords needed
        mov     ebx, [ebp].savePpdev
        mov     edx, [ebx].nvDmaCount
        mov     [ebp].localNvDmaCount, edx  ; update local nvDmaCount
        mov     [ebx].nvDmaCachedFreeCount, 0
        ret

CheckDMAFreeCount   endp


;******************************************************************************
;
;   NV4_DrawFastGlyphs
;
;   SURFOBJ*    pso
;   STROBJ*     pstro
;   FONTOBJ*    pfo
;   RECTL*      prclClip
;   GLYPHPOS*   pgpParm
;   ULONG       glyphCount
;
;******************************************************************************

align 4
        public NV4_DrawFastGlyphs@24

NV4_DrawFastGlyphs@24 proc near

        push    ebx
        push    edi
        push    esi
        push    ebp

        sub     esp,(size NV4_DrawFastGlyphs_STRUC) - NV4_DrawFastGlyphs_PARM_SIZE
        cld

        mov     ebx, [esp].pso
        mov     ecx, 6                      ; overhead for caching string
        mov     ebx, [ebx].dhpdev           ; pso->dhpdev points to pdev
        mov     esi, [ebx].nvDmaCount       ; ppdev->nvDmaCount
        mov     eax, [ebx].nvDmaFifo
        mov     [esp].savePpdev, ebx
        mov     [esp].localNvDmaCount, esi   ; save local copy
        mov     [esp].localNvDmaFifoPtr, eax ; save local copy

    ;*************************************************************************
    ; Get current free count in DMA buffer.
    ;
    ; NOTE: requires ppdev.nvDmaCount to equal local nvDmaCount
    ;
    ; INPUT:  EBP - stack pointer
    ;         ECX - number of DWORDS needed in DMA buffer
    ; OUTPUT: EAX = free count
    ;         updates localNvDmaCount when ppdev->nvDmaCount is updated
    ;
    ;*************************************************************************

        add     ecx, [esp].glyphCount       ; number of glyphs + overhead for push buffer check
        mov     ebp, esp
        call    CheckDMAFreeCount           ; gets free count
        mov     ebx, [esp].savePpdev
        mov     eax, [esp].pstro
        mov     edi, [eax].ulCharInc        ; space between glyphs if non-zero
        mov     [esp].ulCharIncLocal, edi   ; save glyph increment

    ;*************************************************************************
    ;
    ; Clip rectangle
    ;
    ; CLIP_POINT0_C = prclClip->top << 16 | prclClip->left
    ; CLIP_POINT1_C = prclClip->bottom <<16 | prclClip->right
    ;
    ; For bottom right, use MIN(prclClip->bottom, MAX_CX_CY_MEMORY_VALUE) << 16 ||
    ;                       MIN(prclClip->right,  ppdev->cxScreen)
    ;
    ; This is necessary because the bottom and right values in the clipping
    ; rectangle are sometimes too large for the current frame buffer and these
    ; values screw up the hardware.
    ;       
    ; EBX - ppdev
    ;
    ;*************************************************************************

        mov     ecx, [esp].localNvDmaCount
        shl     ecx, 2                      ; offset into DMA buffer
        mov     esi, [esp].localNvDmaFifoPtr
        mov     eax, [esp].prclClip

        mov     [esi+ecx], RECT_AND_TEXT_SUBCHANNEL OR NV04A_CLIP_POINT0_C OR (2 SHL 18)

        mov     edx, [eax].ytop             ; prclClip->top
        mov     edi, [eax].xleft            ; prclClip->left
        shl     edx, 16                     ; prclClip->top << 16
        and     edi, 0ffffh
        or      edx, edi

        mov     [esi+ecx+4], edx            ; NV04A_CLIP_POINT0_C

    ;*************************************************************************
    ;
    ; Clip right, bottom
    ;
    ; EAX - prclClip
    ; EBX - ppdev
    ; ECX - localNvDmaCount*4
    ; ESI - nvDmaFifo
    ;
    ;*************************************************************************

        mov     edi, [eax].xright           ; prclClip->right
        mov     edx, [eax].ybottom          ; prclClip->bottom
        cmp     edi, [ebx].cxScreen
        jl      @f
        
        mov     edi, [ebx].cxScreen
@@:
        mov     [esp].clipRight, edi        ; save right clip for later checking
        
        cmp     edx, MAX_CX_CY_MEMORY_VALUE
        jl      @f
        
        mov     edx, MAX_CX_CY_MEMORY_VALUE
@@:
        mov     [esp].clipBottom, edx       ; save bottom clip for later checking
        shl     edx, 16
        and     edi, 0ffffh
        or      edx, edi
        mov     [esi+ecx+8], edx            ; NV04A_CLIP_POINT1_C

        add     ecx, 12                     ; number of bytes written to DMA buffer
        shr     ecx, 2                      ; convert to dwords
        mov     [esp].localNvDmaCount, ecx  ; update local nvDmaCount
        mov     [esp].saveNvDmaCount, ecx   ; save buffer pointer here in case
                                            ; need to back up if invalid text data
        mov     eax, [esp].pgpParm
        mov     [esp].originalPgpParm, eax  ; save starting pgp

        cmp     [ebx].fontCacheEnabled, FALSE ; if caching turned off
        je      DrawGlyphsWithoutCache      ; then draw glyphs without caching

    ;*************************************************************************
    ; Find the font in the FontCacheInfo table if it exists.
    ;*************************************************************************

        mov     [esp].missingGlyphFlag, FALSE
        mov     edx, [esp].pfo               ; Font object
        mov     ebx, [edx].pvConsumer        ; check if font has been cached
        cmp     ebx, FONT_REJECTED_TAG       ; can't cache this font
        je      DrawGlyphsWithoutCache       ; draw glyphs the old way

        or      ebx, ebx                     ; if zero then not cached
        je      CacheNewFont                 ; so cache font

    ;*************************************************************************
    ; EBX = offset to FontCacheInfo entry
    ;*************************************************************************

        mov     eax, [ebx].FontCacheId       ; font handle
        cmp     eax, [edx].iUniq             ; pfo.iUniq = font handle
        jne     CacheFontAgain               ; font was bumped - cache again

    ;*************************************************************************
    ; Found the font in cache.  Update the MRU and LRU font info linkedlist
    ;
    ; EBX - pointer to FontCacheInfo entry for this font
    ;
    ;*************************************************************************

CachedFontFound:

        cmp     ebx, LRUFontInfoNode
        jne     MakeFontMRU                 ; if we are not in LRU then don't worry

        mov     edi, [ebx].pFontInfoPrev    ; CurFontInfo at LRU.  Remove it
        test    edi, edi
        js      OutputCachedFont            ; current font is only font in linked list

        mov     LRUFontInfoNode, edi
        mov     [edi].pFontInfoNext, LLNULL ; null for linked list

MakeFontMRU:
        cmp     ebx, MRUFontInfoNode
        je      OutputCachedFont            ; done if MRU font ptr is correct

        mov     esi, [ebx].pFontInfoPrev    ; esi  = CurFontInfo->Prev
        mov     edi, [ebx].pFontInfoNext    ; edi  = CurFontInfo->Next
        mov     [esi].pFontInfoNext, edi    ; CurFontInfo->Prev->Next
                                            ;     = CurFontInfo->Next
        test    edi, edi
        js      short @f
        mov     [edi].pFontInfoPrev, esi    ; CurFontInfo->Next->Prev
                                            ;     = CurFontInfo->Prev
@@:
        mov     esi, MRUFontInfoNode        ; Update MRU list
        mov     MRUFontInfoNode, ebx
        mov     [ebx].pFontInfoNext, esi    ; CurFontInfo->Next = OldMRUFontInfo
        mov     [ebx].pFontInfoPrev, LLNULL ; CurFontInfo->Prev = NULL
        mov     [esi].pFontInfoPrev, ebx    ; points to new MRU

    ;*************************************************************************
    ; EBX - ptr to FontCacheInfo entry for current font
    ;*************************************************************************

OutputCachedFont:

        mov     esi, [esp].localNvDmaFifoPtr
        mov     edi, [esp].localNvDmaCount
        shl     edi, 2
        add     esi, edi                    ; nvDmaFifo[nvDmaCount]
        mov     eax, [ebx].FontPitchAndOffset
        mov     dword ptr [esi], RECT_AND_TEXT_SUBCHANNEL OR NV04A_FONTF OR (1 SHL 18)
        mov     [esi+4], eax                ; set font info

        mov     eax, [esp].glyphCount       ; number of glyphs to draw
        shl     eax, 18                     ; bits 31:18 had glyph count
        or      eax, RECT_AND_TEXT_SUBCHANNEL OR NV04A_CHARACTER_COLOR1_F
        mov     [esi+8], eax                ; number of dwords to send
        add     esi, 12                     ; update nvDmaCount

    ;*************************************************************************
    ;
    ; EBX - ptr to FontCacheInfo entry for current font
    ;*************************************************************************

        mov     eax, [esp].pgpParm
        mov     ecx, [esp].pstro
        mov     edi, [eax].gp_x             ; pgp->ptl.x
        mov     edx, [eax].gp_y             ; pgp->ptl.y

        shl     edi, 8
        shl     edx, 20
        and     edi, 0fff00h                ; shouldn't be necessary, but just in case
        or      edx, edi                    ; y bits 31:20, x bits 19:8

        add     ebx, FONTCACHEINFOHEADERSIZE

;*****************************************************************************
;
; Loop through all the unclipped glyphs for this string with caching
;
; EAX - pgp
; EBX - ptr to FontCacheInfo.FontCacheGlyphInfo for current font
; EDX - destination x and y in hardware format: Y bits 31:20, X bits 19:8
; ESI - ptr to nvDmaFifo[nvDmaCount]
;
;*****************************************************************************

CachedGlyphWhichLoop:
        mov     edi, [esp].glyphCount       ; number of glyphs in string
        cmp     [esp].ulCharIncLocal, 0
        jnz     CachedGlyphFixedLoop        ; ulCharInc is used for glyph positions

    ;*************************************************************************
    ;
    ; Loop through all glyphs. Glyph positions are not fixed, that is, ulCharInc
    ; is zero and the pgp contains the glyph spacing.
    ;
    ;*************************************************************************

CachedGlyphLoop:

    ;*************************************************************************
    ; Get the size (in pixels) of the monochrome bitmap and
    ; get pointer to BYTE aligned glyph bitmap
    ;*************************************************************************

        mov     ecx, [eax].gp_hg            ; glyph index in pgp
        cmp     ecx, MAXGLYPHINDEX
        jg      NotValidGlyph

        mov     ecx, [ebx+ecx*4]            ; saved XY and glyph index
        cmp     ecx, LLNULL                 ; if null
        je      LoadMissingGlyphsToCache    ; then need to load glyph

        add     ecx, edx                    ; add the glyph based XY org
        add     eax, (size GLYPHPOS)        ; pgp++
        mov     [esi], ecx                  ; output glyph from cache
        add     esi, 4                      ; nvDmaFifoPtr++

    ;*************************************************************************
    ; Check for more glyphs in string
    ;*************************************************************************

        dec     edi                         ; more glyphs to draw?
        jle     DrawCachedGlyphsDone        ; no - exit

    ;*************************************************************************
    ; Update to next glyph and update to next x and y value
    ;
    ; EAX - pgp
    ; EBX - ptr to FontCacheInfo.FontCacheGlyphInfo for current font
    ; EDX - destination x and y in hardware format: Y bits 31:20, X bits 19:8
    ; EDI - number of glyphs in string
    ; ESI - ptr to nvDmaFifo[nvDmaCount]
    ;
    ;*************************************************************************

        mov     ecx, [eax].gp_x             ; pgp->ptl.x
        mov     edx, [eax].gp_y             ; pgp->ptl.y
        shl     ecx, 8
        shl     edx, 20
        and     ecx, 0fff00h                ; shouldn't be necessary, but just in case
        or      edx, ecx
        jmp     CachedGlyphLoop             ; yes - continue with next glyph

    ;*************************************************************************
    ; Update DMA buffer
    ;
    ; ESI - ptr to nvDmaFifo[nvDmaCount]
    ;
    ;*************************************************************************

DrawCachedGlyphsDone:
        sub     esi, [esp].localNvDmaFifoPtr
        shr     esi, 2
        mov     [esp].localNvDmaCount, esi  ; update nvDmaCount
        jmp     DrawGlyphsDone

;*****************************************************************************
;
; Loop through all the unclipped glyphs for this string with caching. Use
; ulCharInc to position each glyph in string
;
; EAX - pgp
; EBX - ptr to FontCacheInfo.FontCacheGlyphInfo for current font
; EDX - destination x and y in hardware format: Y bits 31:20, X bits 19:8
; EDI - number of glyphs in string
; ESI - ptr to nvDmaFifo[nvDmaCount]
;
;*****************************************************************************

CachedGlyphFixedLoop:

    ;*************************************************************************
    ; Get the size (in pixels) of the monochrome bitmap and
    ; get pointer to BYTE aligned glyph bitmap
    ;*************************************************************************

        mov     ecx, [eax].gp_hg            ; glyph index in pgp
        cmp     ecx, MAXGLYPHINDEX
        jg      NotValidGlyph

        mov     ecx, [ebx+ecx*4]            ; saved XY and glyph index
        cmp     ecx, LLNULL                 ; if null
        je      LoadMissingGlyphsToCache    ; then need to load glyph

        add     ecx, edx                    ; add the glyph based XY org
        mov     [esi], ecx                  ; output glyph from cache
        add     esi, 4                      ; nvDmaFifoPtr++

    ;*************************************************************************
    ; Check for more glyphs in string
    ;*************************************************************************

        dec     edi                         ; more glyphs to draw?
        jle     DrawCachedGlyphsDone        ; no - exit

    ;*************************************************************************
    ; Update to next glyph and update to next x and y value
    ;
    ; EAX - pgp
    ; EBX - ptr to FontCacheInfo.FontCacheGlyphInfo for current font
    ; EDX - destination x and y in hardware format: Y bits 31:20, X bits 19:8
    ; EDI - number of glyphs in string
    ; ESI - ptr to nvDmaFifo[nvDmaCount]
    ;
    ;*************************************************************************

        add     eax, (size GLYPHPOS)
        mov     ecx, [esp].ulCharIncLocal
        shl     ecx, 8
        add     edx, ecx                    ; if ulCharInc is non-zero
                                            ; then ptlOrigin.x += ulCharInc
        jmp     CachedGlyphFixedLoop        ; continue with next glyph

;*****************************************************************************
;
; Glyph has not been cached, so update current DMA pointers in case caching is
; aborted and try to cache rest of glyphs now.
;
; EAX - pgp
; EBX - ptr to FontCacheInfo.FontCacheGlyphInfo for current font
; EDX - destination x and y in hardware format: Y bits 31:20, X bits 19:8
; EDI - number of glyphs left to draw
; ESI - ptr to nvDmaFifo[nvDmaCount]
;
;*****************************************************************************

LoadMissingGlyphsToCache:

        sub     ebx, FONTCACHEINFOHEADERSIZE
        mov     [esp].pgpParm, eax          ; save current pgp
        mov     [esp].glyphCount, edi       ; update number of glyphs left
        mov     [esp].saveESI, esi          ; DMA pointer
        mov     [esp].saveEDX, edx          ; destination x and y position

        mov     edi, [ebx].FontPitchAndOffset
        mov     [esp].missingGlyphFlag, TRUE
        and     edi, 0fffffffh              ; clear pitch
        jmp     LoadGlyphsToCache

;******************************************************************************
;
; Font is not cached because it is new. Check if we can fit the font into
; the offscreen memory cache.
;
; EDX - pfo - pointer to font object
;
;******************************************************************************

align 4

CacheNewFont:

        lea     ebx, [esp].fontInfoBuffer
        mov     ecx, size FONTINFO
        push    ebx                         ; &fontInfo
        push    ecx                         ; size (FONTINFO)
        push    edx                         ; pfo
        call    FONTOBJ_vGetInfo@12         ; FONTOBJ_vGetInfo (pfo, cjSize, pfontInfo)

        lea     eax, [esp].fontInfoBuffer
        mov     edx, [esp].pfo
        mov     ecx, [eax].cjMaxGlyph1
        cmp     ecx, MAXGLYPHSIZE
        jle     LoadFontToCache

        mov     [edx].pvConsumer, FONT_REJECTED_TAG
        jmp     DrawGlyphsWithoutCache

    ;**************************************************************************
    ;
    ; Font is not cached, either because it is new or because it was bumped from
    ; the cache. So if we can cache the font, load it to offscreen memory.
    ;
    ; First, an entry in our FontCacheInfo table. If the FontCacheInfo table
    ; is full, the LRU font is freed to make room for the new font.
    ;
    ;  EDX - pfo - pointer to font object
    ;
    ;**************************************************************************

align 4

CacheFontAgain:
LoadFontToCache:

        mov     ebx, offset FontCacheInfo
        xor     ecx, ecx
        mov     eax, LLNULL                 ; NULL

FindFreeEntry:
        cmp     eax, [ebx].FontCacheID      ; if not null
        jne     ContinueLookingForEntry     ; then keep looking

    ;*************************************************************************
    ; Found empty entry in table
    ;
    ;  EBX = ptr of FontCacheInfo entry
    ;  ECX = index in FontCacheInfo table
    ;
    ;*************************************************************************

GotFontCacheInfo:

        mov     esi, [esp].savePpdev
        mov     eax, MAXFONTCACHESIZE
        imul    eax, ecx
        mov     edi, [esi].fontCacheOffset
        add     edi, eax
        mov     eax, LLNULL
        jmp     GotFontCacheOffset

ContinueLookingForEntry:
        inc     ecx
        add     ebx, (size FontCacheInfoStruc)  ; each entry is fixed (256 DWORDS)
        cmp     ecx, MAXFONTS
        jl      short FindFreeEntry

    ;*************************************************************************
    ; Free LRU font and initialize font info entry to empty
    ;
    ;  EAX = LLNULL - null for font info structure and linked list 0ff00ff00h
    ;
    ;*************************************************************************

        mov     ebx, LRUFontInfoNode
        mov     edi, ebx                            ; init the font entry to empty
        mov     esi, [ebx].pFontInfoPrev
        mov     [esi].pFontInfoNext, eax            ; set to LLNULL
        mov     LRUFontInfoNode, esi
        mov     ebp, [ebx].FontPitchAndOffset       ; save font offset for reuse

        mov     ecx, ((size FontCacheInfoStruc)/4)  ; note: LLNULL == 0ff00ff00h
        rep     stosd
        mov     edi, ebp                            ; font pitch and offset
        and     edi, 0fffffffh                      ; clear pitch info

    ;*************************************************************************
    ; Initialize FontCacheInfo Header
    ;
    ;  EAX = LLNULL
    ;  EBX = ptr of FontCacheInfo entry
    ;  EDX = pfo
    ;  EDI = font cache offset
    ;
    ;*************************************************************************

GotFontCacheOffset:

        mov     ecx, FONTFIXEDPITCHSHIFT SHL 28 ; pitch in 31:28
        or      ecx, edi                        ; load offset
        mov     [ebx].FontPitchAndOffset, ecx   ; save pitch/offset to font cache
        mov     esi, [edx].iUniq                ; pfo.iUniq - font handle
        mov     [ebx].FontCacheID, esi          ; save FontCacheID (handle)
        mov     [edx].pvConsumer, ebx           ; save ptr to FontCacheInfo entry
        cmp     eax, LRUFontInfoNode            ; if LRUFont == LLNULL then
        jne     short @f                        ; LRUFont = CurFontInfo
        mov     LRUFontInfoNode, ebx
@@:
        mov     esi, MRUFontInfoNode
        mov     [ebx].pFontInfoNext, esi        ; CurFontInfo->Next = SecondMRU
        mov     [ebx].pFontInfoPrev, eax        ; CurFontInfo->Prev = LLNULL
        mov     MRUFontInfoNode, ebx            ; MRUFont = CurFontInfo
        cmp     esi, eax                        ; if OldMRU != LLNULL then
        je      short LoadGlyphsToCache
        mov     [esi].pFontInfoPrev, ebx        ; CurFontInfo->Prev = OldMRU

;*****************************************************************************
;
; Load all glyphs in font to offscreen cache and save info in FontCacheInfo
;
; EBX - ptr to FontCacheInfo entry
; EDI - offset into frame buffer where font cache resides
;
;*****************************************************************************

LoadGlyphsToCache:

        mov     eax, [esp].savePpdev
        add     ebx, FONTCACHEINFOHEADERSIZE    ; points to glyph entries
        add     edi, [eax].pjFrameBufbase             ; frame buffer offset
        mov     eax, [esp].pgpParm
        mov     ecx, [esp].glyphCount
        mov     [esp].saveGlyphCount, ecx

    ;*************************************************************************
    ; Load all glyphs
    ;
    ; saveGlyphCount - temporary copy of glyph count
    ; EAX - pgp
    ; EBX - ptr to FontCacheInfo.FontCacheGlyphInfo for current font
    ; EDI - ptr to font cache in frame buffer
    ;
    ;*************************************************************************

LoadGlyphLoop:

        mov     edx, [eax].gp_hg            ; glyph index
        cmp     edx, MAXGLYPHINDEX          ; 255 - max value for hardware design
        jg      NextGlyph                   ; can't cache glyph

        cmp     [ebx+edx*4], LLNULL         ; check if glyph is cached
        jne     NextGlyph                   ; glyph already cached

        mov     [esp].savePgp, eax
        mov     ecx, [eax].gp_pgdf
        mov     ecx, [ecx].gdf_pgb
        mov     eax, [ecx].gb_y
        shl     eax, 16
        or      ax, word ptr [ecx].gb_x
        jns     short @f

        sub     eax, 10000h                 ; need to adjust for later addition
                                            ; with x and y positions

@@:
        shl     ax, 4                       ; x in 19:8
        shl     eax, 4                      ; y in 31:20
        or      eax, edx                    ; glyph index in 7:0 NOTE: edx <= 255
        mov     [ebx+edx*4], eax            ; save glyph data in FontCacheInfo table
        shl     edx, FONTFIXEDPITCHSHIFT

        lea     esi, [ecx].gb_aj            ; offset to glyph
        mov     eax, [ecx].gb_cy            ; height
        mov     ebp, [ecx].gb_cx            ; width
        add     ebp, 7                      ; byte packed - round to next byte
        and     ebp, 0fff8h                 ; number of bits - byte packed
        mov     ecx, ebp                    ; save width bits
        imul    ecx, eax                    ; bits in glyph
        add     ecx, 31
        shr     ecx, 5                      ; number of dwords in glyph

        shl     eax, 16
        or      eax, ebp                    ; high word is height, low word is width
        mov     [edi+edx], eax              ; save glyph width and height first

    ;*************************************************************************
    ; load and save glyph data to offscreen memory
    ;
    ; EBX - ptr to FontCacheInfo.FontCacheGlyphInfo for current font
    ; ECX - number of dword in glyph bitmap
    ; EDX - glyph index
    ; EDI - offset to font cache in frame buffer
    ;
    ;*************************************************************************

        mov     ebp, edi                    ; save offset to start of font cache
        lea     edi, [edi+edx+4]            ; offset to glyph in cache
        rep     movsd                       ; save glyph bitmap
        mov     edi, ebp                    ; restore offset to font cache
        mov     eax,[esp].savePgp           ; pgp for current glyph

NextGlyph:
        add     eax, (size GLYPHPOS)        ; next glyph
        dec     [esp].saveGlyphCount        ; check if more glyphs to cache
        jnz     LoadGlyphLoop               ; cache next glyph

        sub     ebx, FONTCACHEINFOHEADERSIZE  ; point to FontCacheInfo entry
        cmp     [esp].missingGlyphFlag, FALSE ; if interrupted drawing to cache glyph
        je      CachedFontFound               ; then continue drawing glyphs

        add     ebx, FONTCACHEINFOHEADERSIZE  ; FontCacheInfo.FontCacheGlyphInfo
        mov     edx, [esp].saveEDX            ; destination x and y position
        mov     esi, [esp].saveESI            ; DMA pointer
        mov     eax, [esp].pgpParm            ; restore current pgp
        mov     [esp].missingGlyphFlag, FALSE ; reset flag
        jmp     CachedGlyphWhichLoop


    ;*************************************************************************
    ; Glyph can't be cached because it's index exceeds the hardware's capacity.
    ; This rarely happens, so just start from the beginning and draw the string
    ; the old way.
    ;*************************************************************************

NotValidGlyph:
        mov     eax, [esp].originalPgpParm  ; original pgp
        mov     [esp].pgpParm, eax          ; restore pgp
        mov     eax, [esp].saveNvDmaCount   ; original DMA count
        mov     [esp].localNvDmaCount, eax

;*****************************************************************************
;*****************************************************************************
;*****************************************************************************
;
; Draw glyph loop with checking for clipping without caching.
;
;*****************************************************************************

DrawGlyphsWithoutCache:

        mov     eax, [esp].pgpParm          ; ptr to glyphPos
        mov     ebx, [eax].gp_pgdf          ; ptr to glyph definition
        mov     ebx, [ebx].gdf_pgb          ; EBX = pgb (ptr to glyph bits)

    ;*************************************************************************
    ; ptlOrigin = Origin of glyph in bitmap + position on screen
    ;
    ; EAX - pgp
    ; EBX - pgb
    ;
    ;*************************************************************************

        mov     esi, [esp].pstro
        mov     edi, [ebx].gb_x             ; pgb->ptlOrigin.x
        mov     edx, [ebx].gb_y             ; pgb->ptlOrigin.y
        add     edi, [eax].gp_x             ; pgp->ptl.x
        add     edx, [eax].gp_y             ; pgp->ptl.y

        mov     [esp].ptlOriginx, edi
        mov     eax, [esp].prclClip
        mov     [esp].ptlOriginy, edx

    ;*************************************************************************
    ; Check if bounding box for string is within clipping rectangle
    ;
    ; EAX - prclClip
    ; EBX - pgb
    ; ESI - pstro
    ;
    ;*************************************************************************

        mov     ecx, [esp].clipRight
        mov     edi, [eax].xleft            ; prclClip->xleft
        mov     edx, [eax].ytop
        mov     eax, [esp].clipBottom

    ;*************************************************************************
    ; Trivial rejection
    ;*************************************************************************

        cmp     ecx, [esi].rclBGxleft       ; right clip < left of rect?
        jl      DrawGlyphsDone              ; yes - done

        cmp     eax, [esi].rclBGytop        ; bottom clip < top of rect
        jl      DrawGlyphsDone              ; yes - done

        cmp     edi, [esi].rclBGxright      ; left clip > right of rect?
        jg      DrawGlyphsDone              ; yes - done

        cmp     edx, [esi].rclBGybottom     ; top clip > bottom of rect?
        jg      DrawGlyphsDone              ; yes - done

    ;*************************************************************************
    ; Eliminate beginning glyphs that are clipped on the left
    ;
    ; EBX - pgb  pointer to glyphbits
    ; ECX - clipRight
    ; EDI - rclClip.xleft
    ;
    ;*************************************************************************

        mov     eax, [esp].pgpParm          ; pgp

PreClipString:
        mov     edx, [ebx].gb_cx            ; pgb->sizlBitmap.cx
        add     edx, [esp].ptlOriginx
        cmp     edx, edi                    ; if glyph not clipped
        jg      DrawGlyphSetup              ; then continue

        dec     [esp].glyphCount            ; one less glyph
        jle     DrawGlyphsDone              ; nothing to draw

        add     eax, (size GLYPHPOS)        ; next pgp
        mov     [esp].pgpParm, eax
        mov     ebx, [eax].gp_pgdf          ; pgp->pgdf

        mov     edx, [esp].ulCharIncLocal
        mov     ebx, [ebx].gdf_pgb          ; pgb
        add     [esp].ptlOriginx, edx       ; if ulCharInc is non-zero
                                            ; then ptlOrigin.x += ulCharInc
        test    edx, edx
        jnz     PreClipString               ; continue with next glyph

        mov     ecx, [ebx].gb_x             ; pgb->ptlOrigin.x
        add     ecx, [eax].gp_x             ; pgp->ptl.x
        mov     [esp].ptlOriginx, ecx

        mov     edx, [ebx].gb_y             ; pgb->ptlOrigin.y
        add     edx, [eax].gp_y             ; pgp->ptl.y
        mov     [esp].ptlOriginy, edx
        jmp     PreClipString               ; continue with next glyph

;*****************************************************************************
;
; Loop through all the clipped glyphs for this string
;
; EBX - pgb
;
;*****************************************************************************

DrawGlyphSetup:

        mov     edi, [esp].localNvDmaCount
        shl     edi, 2
        add     edi, [esp].localNvDmaFifoPtr

    ;*************************************************************************
    ; Get the size (in pixels) of the monochrome bitmap and
    ; get pointer to BYTE aligned glyph bitmap
    ;
    ; EBX - pgb
    ; EDI - nvDmaFifo+nvDmaCount
    ;
    ;*************************************************************************

ClipGlyphLoop:
        mov     edx, [ebx].gb_cx            ; pgb->sizlBitmap.cx
        mov     ecx, [ebx].gb_cy            ; pgb->sizlBitmap.cy
        test    ecx, ecx
        jz      CheckMoreGlyphs             ; nothing to output

        and     edx, 0ffffh                 ; must be word
        mov     [esp].gWidth, edx

    ;*************************************************************************
    ;
    ; Check if glyph is clipped and skip if so. Note that we can't just exit now
    ; and assume every glyph will be clipped since this is not the case for italic
    ; or other glyphs with backups. It will also fail DCT test.
    ;
    ;*************************************************************************
        mov     eax, [esp].ptlOriginy
        mov     esi, [esp].ptlOriginx
        cmp     eax, [esp].clipBottom
        jg      CheckMoreGlyphs             ; char is clipped
        
        cmp     esi, [esp].clipRight
        jg      CheckMoreGlyphs             ; char is clipped
        
    ;*************************************************************************
    ; As far as I can tell, the beginning of the glyph (according to the
    ; GLYPHBITS structure) appears to always start on a DWORD boundary.
    ; In addition, the DDK specifies that the bits will always be DWORD padded
    ; at the end. So let's just deal with DWORDS for performance.
    ;*************************************************************************
    ;*************************************************************************
    ; Specify SizeInWidth as multiple of 8 since the incoming glyph data is
    ; always row ordered and BYTE packed. We will always output up to a
    ; BYTE boundary. The excess pixels will get clipped according to the width
    ; we specify in Size. 'SizeIn' corresponds to the amount of data that the
    ; NV Engine expects to receive, and it specifies how the data will be
    ; layed out on the screen.
    ;
    ; EBX - pgb
    ; ECX - glyph height
    ; EDX - glyph width
    ; EDI - nvDmaFifo+nvDmaCount
    ;
    ;*************************************************************************

        add     edx, 7                      ; width + 7
        and     edx, 0fff8h                 ; byte boundary for byte packed
        mov     eax, ecx                    ; save height
        shl     ecx, 16                     ; height in high word
        or      ecx, edx                    ; height <<16 | sizeInWidth
        shr     edx, 3                      ; divide by 8 for number of bytes
        mov     [esp].numBytesPerScan, edx
        mul     edx                         ; #bytes in glyph

        add     eax, 3
        shr     eax, 2                      ; number of dwords for the glyph
        mov     [esp].numDwordsPerGlyph, eax

    ;*************************************************************************
    ;
    ; Check freecount
    ;
    ; EAX - num Dwords in glyph bitmap
    ; EBX - pgb
    ; ECX - SizeIn = height <<16 | sizeInWidth
    ; EDI - nvDmaFifo+nvDmaCount
    ;
    ;*************************************************************************

        mov     edx, [esp].savePpdev
        mov     esi, [esp].localNvDmaFifoPtr
        add     eax, 5                      ; overhead for each glyph
        sub     [edx].nvDmaCachedFreeCount, eax   ; overhead per glyph
        jg      SetParmsForGlyph            ; have enough room in DMA buffer

        sub     edi, esi
        shr     edi, 2
        mov     [edx].nvDmaCount, edi       ; ppdev->nvDmaCount = localNvDmaCount
        mov     [esp].localNvDmaCount, edi

        mov     ebp, esp                    ; save stack pointer
        push    ebx                         ; save pgb
        push    ecx                         ; save SizeIn
        mov     ecx, eax                    ; number of dwords needed
        call    CheckDmaFreeCount
        pop     ecx                         ; restore SizeIn
        pop     ebx                         ; restore pgb
        mov     edi, [esp].localNvDmaCount
        shl     edi, 2
        add     edi, [esp].localNvDmaFifoPtr

    ;*************************************************************************
    ; Set parameters for glyph
    ;
    ; EBX - pgb
    ; ECX - SizeIn = height <<16 | sizeInWidth
    ; EDI - nvDmaFifo+nvDmaCount
    ;
    ;*************************************************************************

SetParmsForGlyph:

        mov     eax, [esp].ptlOriginy
        mov     [edi], RECT_AND_TEXT_SUBCHANNEL OR NV04A_SIZE_IN_E OR (3 SHL 18)
        mov     [edi+4], ecx                ; SIZE_IN_E = height <<16 | sizeInWidth
        and     ecx, 0ffff0000h             ; clear sizeInWidth
        or      ecx, [esp].gWidth
        mov     [edi+8], ecx                ; SIZE_OUT_E = height <<16 | width
        shl     eax, 16                     ; ptlOrigin.y < 16
        mov     ecx, [esp].ptlOriginx
        and     ecx, 0ffffh
        or      eax, ecx
        mov     [edi+12], eax               ; ptlOrigin.y < 16 |    ptlOrigin.x
        add     edi, 16                     ; update nvDmaFifo to point to nvDmaCount

    ;*************************************************************************
    ; Blast out the data as fast as possible
    ;
    ; EBX - pgb
    ; EDI - nvDmaFifo+nvDmaCount
    ;
    ;*************************************************************************

        lea     esi, DWORD PTR [ebx].gb_aj  ; pgb->aj ptr to monochrome bitmap

        mov     eax, [esp].NumDwordsPerGlyph
        mov     ecx, eax
        cmp     eax, MAXMONODWORDS
        jg      BigLoop
        
        shl     eax, 18                     ; number of dwords in bits 31:18
        or      eax, RECT_AND_TEXT_SUBCHANNEL OR NV04A_MONOCHROME_COLOR1_C
        mov     [edi], eax                  ; setup for glyph bitmap that follows
        add     edi, 4                      ; update nvDmaFifo to point to nvDmaCount
        rep     movsd

    ;*************************************************************************
    ; Check for more glyphs in string
    ;
    ; EDI - NvDmaPtr[NvDmaCount]
    ;
    ;*************************************************************************

CheckMoreGlyphs:
        dec     [esp].glyphCount            ; more glyphs to draw?
        jle     UpdateDMACountAndExit       ; no - exit

    ;*************************************************************************
    ; Update to next glyph
    ;*************************************************************************

        mov     eax, [esp].pgpParm
        add     eax, (size GLYPHPOS)
        mov     [esp].pgpParm, eax
        mov     ebx, [eax].gp_pgdf
        mov     ebx, [ebx].gdf_pgb          ; ptr to glyph bits

    ;*************************************************************************
    ; Update to next x and y value
    ;
    ; EAX - pgp
    ; EBX - pgb
    ; EDI - nvDmaFifo+nvDmaCount
    ;
    ;*************************************************************************

        mov     edx, [esp].ulCharIncLocal
        mov     ecx, edx
        add     ecx, [esp].ptlOriginx       ; if ulCharInc is non-zero
                                            ; then ptlOrigin.x += ulCharInc
        test    edx, edx                    ; ulCharInc is valid
        jnz     short @f                    ; check if next glyph is clipped

        mov     ecx, [ebx].gb_x             ; pgb->ptlOrigin.x
        mov     edx, [ebx].gb_y             ; pgb->ptlOrigin.y
        add     ecx, [eax].gp_x             ; pgp->ptl.x
        add     edx, [eax].gp_y             ; pgp->ptl.y
        mov     [esp].ptlOriginy, edx

@@:
        mov     [esp].ptlOriginx, ecx
        jmp     ClipGlyphLoop               ; Continue with next glyph

    ;*************************************************************************
    ; Update local DMA buffer values
    ;
    ; EDI - nvDmaFifo+nvDmaCount
    ;
    ;*************************************************************************

UpdateDMACountAndExit:

        mov     esi, [esp].localNvDmaFifoPtr
        sub     edi, esi
        shr     edi, 2
        mov     [esp].localNvDmaCount, edi  ; ppdev->nvDmaCount = localNvDmaCount

    ;*************************************************************************
    ; Update DMA buffer values
    ;
    ; localNvDmaCount - has current count of entries in DMA buffer
    ;
    ;*************************************************************************

DrawGlyphsDone:
        mov     ebx, [esp].savePpdev
        mov     eax, [esp].localNvDmaCount
        mov     [ebx].nvDmaCount, eax       ; ppdev->nvDmaCount = localNvDmaCount
        add     esp,(size NV4_DrawFastGlyphs_STRUC) - NV4_DrawFastGlyphs_PARM_SIZE
        pop     ebp
        pop     esi
        pop     edi
        pop     ebx

        ret     24

    ;*************************************************************************
    ; More than MAXMONODWORDS in glyph. Need to break it up and send
    ; MAXMONODWORDS at a time. This requires some extra overhead that needs to
    ; be subtracted from the free count.
    ;
    ; ECX - Number of DWORDS
    ; EDI - NvDmaPtr[NvDmaCount]
    ; ESI - ptr to glyph bitmap
    ;
    ;*************************************************************************

BigLoop:
        mov     edx, [esp].savePpdev
        mov     eax, ecx                    ; number of DWORDS in glyph - use to
                                            ; calculate additional overhead
        add     eax, MAXMONODWORDS-1
        shr     eax, MAXMONOSHIFT           ; number of extra DMA entries for setup
        sub     [edx].nvDmaCachedFreeCount, eax   ; overhead per glyph
        jg      SetupBigLoop                ; have enough room in DMA buffer

        add     eax, ecx                    ; add number of DWORDS in glyph for
                                            ; total DWORDS needed from DMA buffer
        sub     edi, [esp].localNvDmaFifoPtr
        shr     edi, 2
        mov     [edx].nvDmaCount, edi       ; ppdev->nvDmaCount = localNvDmaCount
        mov     [esp].localNvDmaCount, edi
        
        mov     ebp, esp                    ; save stack pointer
        push    esi                         ; save ptr to glyph bitmap
        push    ecx                         ; save number of dwords in glyph
        mov     ecx, eax                    ; number of extra dwords needed in buffer
        call    CheckDmaFreeCount
        pop     ecx                         ; restore number of dwords in glyph
        pop     esi                         ; restore ptr to glyph bitmap
        mov     edi, [esp].localNvDmaCount
        shl     edi, 2
        add     edi, [esp].localNvDmaFifoPtr

    ;*************************************************************************
    ; ECX - Number of DWORDS
    ; EDI - NvDmaPtr[NvDmaCount]
    ; ESI - ptr to glyph bitmap
    ;*************************************************************************

SetupBigLoop:
        mov     edx, ecx                    ; number of dwords in glyph

SendMaxDwords:
        mov     [edi], (MAXMONODWORDS SHL 18) OR RECT_AND_TEXT_SUBCHANNEL OR NV04A_MONOCHROME_COLOR1_C
        add     edi, 4

        mov     ecx, MAXMONODWORDS
        sub     edx, MAXMONODWORDS
        rep     movsd

        cmp     edx, MAXMONODWORDS
        jge     SendMaxDwords

    ;*************************************************************************
    ; Send remaining dwords
    ;
    ; ECX = 0 after REP MOVSD
    ; EDX = number of dwords in bitmap remaining to be written to DMA buffer
    ; EDI - NvDmaPtr[NvDmaCount]
    ; ESI - ptr to glyph bitmap
    ;
    ;*************************************************************************

        or      ecx, edx                    ; rest of dwords
        jz      CheckMoreGlyphs             ; zero - continue

        mov     eax, edx                    ; rest of dwords
        shl     eax, 18                     ; put in bits 31:18
        or      eax, RECT_AND_TEXT_SUBCHANNEL OR NV04A_MONOCHROME_COLOR1_C
        mov     [edi], eax                  ; set up for bitmap data
        add     edi, 4

        rep     movsd
        jmp     CheckMoreGlyphs

NV4_DrawFastGlyphs@24 endp

end


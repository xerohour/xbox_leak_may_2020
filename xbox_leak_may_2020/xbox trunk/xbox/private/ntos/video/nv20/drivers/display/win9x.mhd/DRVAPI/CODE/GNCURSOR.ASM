;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gncursor.asm
;
; Purpose:      This file holds the SetCursor, MoveCursor, and CheckCursor
;               display driver entry point.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc


OPENSEG  _DATA, USE16, DATA
; This structure is too big to put on the stack because sometimes
; GDI calls SetCursor with a 4K (!) stack and this is over 4K!
; We use this area to convert 8bpp cursors to 32bpp cursors to
; avoid problems where the palette changes while a cursor is
; still in the cache and then the pattern gets reloaded into the
; hardware without getting retranslated through the new palette.
SIZE_32BPP_CURSOR_PLUS_HEADER   TEXTEQU <SIZE CURSORDESCEX + AND_MASK_SIZE + 20H * 20H * 4>
TempCursor              DB      (SIZE_32BPP_CURSOR_PLUS_HEADER) DUP (0)
wTempCursorInUse        DW      0
CLOSESEG _DATA


OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GN_CreateCursorCache
;
; Purpose:      This function destroys a cursor cache. Memory is
;               freed and ptrs are zeroed.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset
;
; Returns:      eax             0       then Failure
;                               non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CreateCursorCache, PASCAL, FRAME, FAR32
PARMD   pGNCrtcCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pGNCrtcCXOffset

        ; Now allocate a cursor cache
        mov     eax, SIZE CURSORCACHE
        add     eax,(SIZE CURSORDESCEX * MAX_CACHEABLE_CURSORS)
        push    eax
        call    HW_AllocFlatMemory
        mov     ds:[esi].GNCRTCCX.pCursorCache,eax

        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_DestroyCursorCache
;
; Purpose:      This function destroys a cursor cache. Memory is
;               freed and ptrs are zeroed.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DestroyCursorCache, PASCAL, FRAME, FAR32
PARMD   pGNCrtcCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pGNCrtcCXOffset

        ; Now allocate a cursor cache
        mov     eax,ds:[esi].GNCRTCCX.pCursorCache
        or      eax,eax
        je      @F

        push    esi
        call    GN_ClearCursorCache

        push    ds:[esi].GNCRTCCX.pCursorCache
        call    HW_FreeFlatMemory
        mov     ds:[esi].GNCRTCCX.pCursorCache,0
@@:
        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_ClearCursorCache
;
; Purpose:      This function clears the cursor cache which will
;               cause all cache entries to be marked empty. However
;               it will not flush current patterns from the cache,
;               thus making sure the selected pattern remains valid
;               until the next pattern is set.
;
; Arguments:
;               ds              Context Selector
;               pGNCrtcCXOffset
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ClearCursorCache, PASCAL, FRAME, FAR32
PARMD   pGNCrtcCXOffset
OPENPROC
        PUSHR   esi
        mov     esi,pGNCrtcCXOffset

        ; Mark all cache spots so that there will never be a match
        mov     ebx,ds:[esi].GNCRTCCX.pCursorCache
        mov     ecx,ds:[esi].GNCRTCCX.dwMaxCacheableCursors
        lea     ebx,[ebx + SIZE CURSORCACHE]
        mov     eax,-1
@@:     mov     ds:[ebx].CURSORDESCEX.wWidthBytes,ax
        mov     ds:[ebx].CURSORDESCEX.ExtraData.dwCursorHash,eax
        add     ebx,SIZE CURSORDESCEX
        dec     ecx
        jne     @B

        ; Set flags so that the cursor will be moved/selected correctly
        mov     ebx,ds:[esi].GNCRTCCX.pCursorCache
        or      ds:[ebx].CURSORCACHE.dwCursorFlags,CURSOR_MOVE OR CURSOR_SELECT OR CURSOR_IS_NULL
        mov     ds:[ebx].CURSORCACHE.dwNextCacheSlot,0

        POPR    esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_ClearAllCursorCaches
;
; Purpose:      This routine runs through all the GNLOGDEVCXs on the
;               board and clears the cursor cache for each of them.
;
; Arguments:
;               ds                      Context Selector
;               pGNBoardCXOffset        GNBOARDCX ptr
;
; Returns:
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ClearAllCursorCaches, PASCAL, FRAME, FAR32
PARMD   pGNBoardCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     eax,pGNBoardCXOffset

        ; Set up a loop to enumerate logical devices
        sub     ebx,ebx

LoopLogdev:
        mov     ecx,ds:[eax].GNBOARDCX.pGNLogdevCX[ebx * 4]
        or      ecx,ecx
        je      NextLogdev

        ; Start with the first CRTCCX
        mov     edx,0

LoopCrtc:
        mov     esi,ds:[ecx].GNLOGDEVCX.pGNCrtcCX[edx * 4]
        or      esi,esi
        je      NextCrtc

        ; We found a CRTCCX. Clear the cursor cache.
        push    eax                     ; save GNBOARDCX
        push    ebx                     ; save index into GNLOGDEVCXs
        push    ecx                     ; save GNLOGDEVCX
        push    edx                     ; save index into GNCRTCCXs
        push    esi
        call    GN_ClearCursorCache
        pop     edx                     ; restore index into GNCRTCCXs
        pop     ecx                     ; restore GNLOGDEVCX
        pop     ebx                     ; restore index into GNLOGDEVCXs
        pop     eax                     ; restore GNBOARDCX

NextCrtc:
        ; We could potentially have as many physical devices on a
        ; logical device as there are CRTCs on the board.
        inc     edx
        cmp     edx,ds:[ecx].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCrtc

NextLogdev:
        ; We could potentially have as many logical devices
        ; as CRTCs on the board
        inc     ebx
        cmp     ebx,ds:[eax].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopLogdev

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_Convert8BppCursorTo32BppCursor
;
; Purpose:      This function converts an 8bpp cursor to a 32bpp
;               cursor.
;
; Arguments:
;               ds                      Context Selector
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               lp8BppCursorShape       16:16 "src" 8bpp cursor
;               lp32BppCursorShape      16:16 ptr to place to store
;                                       converted cursor
;
; Returns:      eax = 0 - OK; NE 0 - Fail
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_Convert8BppCursorTo32BppCursor, PASCAL, FRAME, NEAR
PARMD   pGNCrtcCXOffset
PARMD   lp8BppCursorShape
PARMD   lp32BppCursorShape
OPENPROC
        PUSHR   esi,edi

        ; Convert the 8bpp cursor to a 32bpp cursor
        sub     esi,esi
        lfs     si,lp8BppCursorShape
        sub     edi,edi
        les     di,lp32BppCursorShape

        ; First copy the header
        movzx   eax,fs:[esi].CURSORDESC.wWidth
        imul    ax,fs:[esi].CURSORDESC.wHeight
        cmp     eax,SIZE_32BPP_CURSOR_PLUS_HEADER
        jg      WrongHeader
        mov     ecx,eax
        shr     ecx,5
        add     ecx,(SIZE CURSORHEADER) SHR 2
        rep     movs dword ptr es:[edi], fs:[esi]

        ; Fix up the bits per pixel field to say 32bpp
        movzx   ecx,word ptr lp32BppCursorShape
        mov     es:[ecx].CURSORDESC.bBitsPerPixel,32

        ; Now we are at the XOR mask for the cursor.
        ; Translate it through the palette, pixel by pixel
        mov     ebx,pGNCrtcCXOffset
        lea     ebx,[ebx].GNCRTCCX.dwPaletteTable
        mov     ecx,eax

Cont_Xlate:
        movzx   eax,byte ptr fs:[esi]
        inc     esi
        mov     eax,ds:[ebx][eax * 4]
        and     eax,00FFFFFFh
        mov     es:[edi],eax
        add     edi,4
        dec     ecx
        jne     Cont_Xlate
        sub     eax,eax

WrongHeader:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_SetCursor
;
; Purpose:      This function is the SetCursor display driver entry point.
;               This routine may be interrupt time -- be sure to preserve
;               the registers appropriately.
;
; Arguments:
;               lpCursorShape   ptr to cursorShape structure
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_SetCursor, PASCAL, FRAME, FAR16, LOGOFF
PARMD   lpCursorShape
LOCALD  lpTempCursor
LOCALD  dwUsingTempCursorInDataSeg
LOCALD  dwTotalCursorDataSize
LOCALD  dwCursorsLeftToSearch
LOCALD  pThisCursorCache
LOCALD  dwMaxCursorCacheEntries
LOCALD  dwCursorPixelDepth
LOCALD  dwHadToAlloc
LOCALD  dwCRTCCount
LOCALD  ppGNCrtcCXOffset
LOCALD  pGNCrtcCXOffset
OPENPROC
        PUSHR   alld,ds,es,fs,gs
        pushf

        ; Prepare for the routine
        mov     dwHadToAlloc,0
        cld

        ; Load up our context
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]

        ; Prevent H/W access when in DOS FS or PowerDown mode
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
        jne     DoneIt

        mov     dwCRTCCount,0
        lea     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     ppGNCrtcCXOffset,eax

        ; If this CRTC is not a NULL context ...
LoopCRTC:
        mov     eax,ppGNCrtcCXOffset
        mov     esi,ds:[eax]
        or      esi,esi
        je      NextCrtc

        ; Get a few things relating to this cursor cache
        mov     eax,ds:[esi].GNCRTCCX.dwMaxCacheableCursors
        mov     dwMaxCursorCacheEntries,eax
        mov     edi,ds:[esi].GNCRTCCX.pCursorCache
        or      edi,edi
        je      NextCrtc

        test    ds:[edi].CURSORCACHE.dwCursorFlags,CURSOR_HAS_TRAILS OR CURSOR_IS_SOFTWARE
        je      HW_SetCursor

        ; Check for NULL cursor ...
        or      ds:[edi].CURSORCACHE.dwCursorFlags,CURSOR_IS_NULL
        cmp     lpCursorShape,0
        je      @F
        and     ds:[edi].CURSORCACHE.dwCursorFlags,NOT CURSOR_IS_NULL
@@:
        ; We have to move the cursor first time after switching.
        test    ds:[edi].CURSORCACHE.dwCursorFlags,CURSOR_MOVE
        je      @F
        and     ds:[edi].CURSORCACHE.dwCursorFlags,NOT CURSOR_MOVE
        mov     eax,dword ptr ds:[edi].CURSORCACHE.wCursorX
        ror     eax,10H
        push    eax
        push    cs:[CSlp16LogDevice]
        call    DIB_MoveCursorExt

@@:
        push    lpCursorShape
        push    cs:[CSlp16LogDevice]
        call    DIB_SetCursorExt
        jmp     NextCrtc

HW_SetCursor:
        mov     pThisCursorCache,edi

        ; Check for NULL cursor ...
        cmp     lpCursorShape,0
        jne     NonNullCursor

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; A NULL lpCursorShape means to turn the cursor off.
NullCursor:
        ; disable the cursor on it
        or      ds:[edi].CURSORCACHE.dwCursorFlags,CURSOR_IS_NULL
        push    esi
        pushd   0
        pushd   0
        call    GN_TurnCursorOffOrOn
        jmp     NextCrtc
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
NonNullCursor:
        ; See if we need to convert the 8bpp cursor to a 32bpp
        ; cursor through the palette. This way, palette changes
        ; won't affect the cursor color in the future. Do we have
        ; a temporary area to do the cursor translation? If not
        ; we have to allocate. The Alloc should not happen
        ; unless SetCursor reenters itself.
        mov     eax,lpCursorShape
        mov     lpTempCursor,eax
        sub     ebx,ebx
        sub     eax,eax
        les     bx,lpCursorShape

        ; Check for the valid selector first
        mov     ax,es
        lar     edx,eax
        jnz     DoneIt
        and     dh,NOT 7
        cmp     dh,0F0H
        jne     DoneIt

        ; Check for the valid bpp
        mov     al,es:[ebx].CURSORDESC.bBitsPerPixel
        cmp     al,1
        je      CheckCache
        cmp     al,4
        je      CheckCache
        cmp     al,15
        je      CheckCache
        cmp     al,16
        je      CheckCache
        cmp     al,24
        je      CheckCache
        cmp     al,32
        je      CheckCache
        cmp     al,8
        jne     DoneIt

        mov     ax,SEG TempCursor
        shl     eax,10H
        lea     ax,TempCursor
        mov     lpTempCursor,eax

        mov     ax,_DATA
        mov     fs,ax
        mov     ecx,1
        xchg    cx,fs:[wTempCursorInUse]
        or      ecx,ecx
        je      @F

        mov     eax,SIZE_32BPP_CURSOR_PLUS_HEADER
        call    GN_MemoryAlloc
        or      eax,eax
        je      SetCursorDone
        shl     edx,10H
        mov     lpTempCursor,edx
        mov     dwHadToAlloc,1
@@:
        push    esi
        push    lpCursorShape
        push    lpTempCursor
        call    GN_Convert8BppCursorTo32BppCursor
        or      eax,eax
        jnz     SetCursorDone

        ; Reload this
        sub     ebx,ebx
        les     bx,lpTempCursor
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CheckCache:
        ; es:ebx should point to the CURSORDESCEX

        ; See if this cursor shape is currently in the cache
        ; First we calculate the size of the cursor in EBX
        movzx   ecx,es:[ebx].CURSORDESC.bBitsPerPixel
        mov     dwCursorPixelDepth,ecx

        ; Convert 15 to 16
        cmp     ecx,15
        sete    al
        movzx   eax,al
        add     ecx,eax

        ; Anything over 32bpp gets set to 32bpp
        ; For Debug Only:
        ; 33Bpp = ARGB format
        ; 34Bpp = ARGB premultiplied format
        cmp     ecx,32
        jle     @F
        mov     ecx,32
@@:
        ; Figure out the total size of the CURSORDESC
        ; plus the AND_MASK and the cursor data
        inc     ecx
        movzx   eax,es:[ebx].CURSORDESC.wWidth
        imul    eax,ecx
        movzx   ecx,es:[ebx].CURSORDESC.wHeight
        imul    eax,ecx
        shr     eax,3
        add     eax,(SIZE CURSORHEADER)
        shr     eax,2
        cmp     eax,SIZE_32BPP_CURSOR_PLUS_HEADER
        jg      SetCursorDone
        mov     dwTotalCursorDataSize,eax
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; It's a little quicker to find by hashing first
        mov     eax,dwMaxCursorCacheEntries
        mov     dwCursorsLeftToSearch,eax
        mov     eax,pThisCursorCache
        add     eax,SIZE CURSORCACHE
        mov     esi,lpCursorShape
HashLoop:
        cmp     esi,ds:[eax].CURSORDESCEX.ExtraData.dwCursorHash
        je      HashHit
HashFail:
        add     eax,SIZE CURSORDESCEX
        dec     dwCursorsLeftToSearch
        jne     HashLoop
        jmp     CheckLong
HashHit:
        movzx   edi,word ptr lpTempCursor
        lea     esi,[eax].CURSORDESCEX.wHotX
        mov     ecx,dwTotalCursorDataSize
        repe    cmps dword ptr ds:[esi], es:[edi]
        je      CursorMatch
        mov     esi,lpCursorShape
        jmp     HashFail
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Didn't find a hash match. Check everything the long way.
CheckLong:
        mov     edi,pThisCursorCache
        lea     eax,[edi + SIZE CURSORCACHE].CURSORDESCEX.wHotX
        mov     ecx,dwMaxCursorCacheEntries
        mov     dwCursorsLeftToSearch,ecx

LongLoop:
        movzx   edi,word ptr lpTempCursor
        mov     esi,eax
        mov     ecx,dwTotalCursorDataSize
        repe    cmps dword ptr ds:[esi], es:[edi]
        je      CursorMatch
        add     eax,SIZE CURSORDESCEX
        dec     dwCursorsLeftToSearch
        jne     LongLoop
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
NoMatch:
        ; Didn't find a match, so use the next cache slot
        mov     edx,pThisCursorCache
        mov     eax,ds:[edx].CURSORCACHE.dwNextCacheSlot
        mov     ds:[edx].CURSORCACHE.dwSelectedCacheSlot,eax
        imul    edi,eax,SIZE CURSORDESCEX
        lea     edi,[edx + SIZE CURSORCACHE][edi]
        mov     ecx,lpCursorShape
        mov     ds:[edi].CURSORDESCEX.ExtraData.dwCursorHash,ecx
        sub     esi,esi
        lfs     si,lpTempCursor
        mov     cx,ds
        mov     es,cx
        add     edi,SIZE CURSOREXTRA
        mov     ecx,dwTotalCursorDataSize
        rep     movs dword ptr es:[edi], fs:[esi]

        ; Indicate which cursor slot needs updating by the hardware and
        ; also that some lazy activity is required.
        lea     ecx,[eax + NUM_CURSOR_FLAGS]
        mov     ebx,1
        shl     ebx,cl
        or      ebx,CURSOR_MOVE OR CURSOR_SELECT
        or      ebx,ds:[edx].CURSORCACHE.dwCursorFlags
        and     ebx,NOT CURSOR_IS_NULL
        mov     ds:[edx].CURSORCACHE.dwCursorFlags,ebx

        ; Update the next cache slot.  The first three cursors to
        ; go in are usually the really common ones, like the arrow,
        ; the hourglass, and the arrow/hourglass combo. So never
        ; kick the first three out once they go in.
        inc     eax
        cmp     eax,dwMaxCursorCacheEntries
        jc      @F
        mov     eax,3
@@:     mov     ds:[edx].CURSORCACHE.dwNextCacheSlot,eax
        mov     ebx,edx
        jmp     SetCursorWhere

CursorMatch:
        ; edx is the cursor cache number that matched
        mov     ebx,pThisCursorCache
        mov     edx,dwMaxCursorCacheEntries
        sub     edx,dwCursorsLeftToSearch
        mov     ds:[ebx].CURSORCACHE.dwSelectedCacheSlot,edx

        ; Don't put off the lazy activity for cursors that are already
        ; in the cache or the cursor seems unresponsive.
        mov     eax,ds:[ebx].CURSORCACHE.dwCursorFlags
        and     eax,NOT CURSOR_IS_NULL
        or      eax,CURSOR_MOVE OR CURSOR_SELECT
        mov     ds:[ebx].CURSORCACHE.dwCursorFlags,eax

SetCursorWhere:
        mov     eax,ppGNCrtcCXOffset
        push    dword ptr ds:[eax]
        call    GN_CursorAction
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SetCursorDone:
        cmp     dwHadToAlloc,0
        je      @F
        movzx   eax,word ptr lpTempCursor+2
        call    GN_MemoryFree
        jmp     Skip5
@@:
        ; We didn't have to alloc, so wemust have been using
        ; the TempCursor
        mov     ax,_DATA
        mov     fs,ax
        mov     fs:[wTempCursorInUse],0
Skip5:

NextCrtc:
        ; Go to the next CRTC
        add     ppGNCrtcCXOffset,4
        inc     dwCRTCCount
        mov     esi,cs:[CSpGNLogicalDevice]
        mov     eax,dwCRTCCount
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCRTC
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DoneIt:
        ; We are Done!
        popf
        POPR    alld,ds,es,fs,gs
CLOSEPROC


;==============================================================================
;
; Function:     GN_CursorAction
;
; Purpose:      This function should be called when it is desired to
;               set, move, select, or enable the cursor. The appropriate
;               sequencing is performed to do all the actions in the
;               correct order.
;
; Arguments:
;               ds      Context Selector
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_CursorAction, PASCAL, FRAME, NEAR, LOGOFF
PARMD   pGNCrtcCXOffset
LOCALD  pHWCrtcCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     edi,pGNCrtcCXOffset
        mov     esi,ds:[edi].GNCRTCCX.pCursorCache
        mov     eax,ds:[edi].GNCRTCCX.pHWCrtcCX
        mov     pHWCrtcCXOffset,eax

        ; Check the semaphore so we don't step on ourselves
        bts     ds:[esi].CURSORCACHE.dwCursorFlags,CURSOR_SEMAPHORE_BIT
        jc      BlockedSemaphore

        ; Handle cursor setting first. There may be many cursors to set
        ; because SetCursor may have been called multiple times since
        ; the last lazy call to this routine. However, only bother
        ; placing the cursor we are going to select into the HW.

        ; If the currently selected cursor needs setting, then do so
        mov     ecx,ds:[esi].CURSORCACHE.dwSelectedCacheSlot
        add     ecx,NUM_CURSOR_FLAGS
        mov     eax,1
        shl     eax,cl
        test    ds:[esi].CURSORCACHE.dwCursorFlags,eax
        jz      Set1

        ; Clear out the SETTING bit for the particular cache slot
        not     eax
        and     ds:[esi].CURSORCACHE.dwCursorFlags, eax
        mov     eax,ds:[esi].CURSORCACHE.dwSelectedCacheSlot

        ; Call the hw specific code to set the cursor pattern into vidmem
        imul    ecx,eax,SIZE CURSORDESCEX
        lea     edi,[esi + SIZE CURSORCACHE][ecx]
        lea     eax,[edi].CURSORDESCEX.wHotX

        push    pHWCrtcCXOffset
        push    ds:[esi].CURSORCACHE.dwSelectedCacheSlot
        push    eax
        call    HW_CursorSet
        mov     ds:[edi].CURSORDESCEX.ExtraData.dwCursorType,eax

Set1:
        ; Handle cursor selection second
        test    ds:[esi].CURSORCACHE.dwCursorFlags,CURSOR_SELECT
        jz      Select1

        mov     eax,ds:[esi].CURSORCACHE.dwSelectedCacheSlot
        imul    ecx,eax,SIZE CURSORDESCEX
        lea     edi,[esi + SIZE CURSORCACHE][ecx]
        lea     edx,[edi].CURSORDESCEX.wHotX

        push    pHWCrtcCXOffset
        push    eax
        push    ds:[edi].CURSORDESCEX.ExtraData.dwCursorType
        push    edx
        call    HW_CursorSelect
        and     ds:[esi].CURSORCACHE.dwCursorFlags,NOT CURSOR_SELECT

Select1:
        ; Handle cursor moves last
        test    ds:[esi].CURSORCACHE.dwCursorFlags,CURSOR_MOVE
        jz      Move1

        mov     eax,ds:[esi].CURSORCACHE.dwSelectedCacheSlot
        imul    ecx,eax,SIZE CURSORDESCEX
        lea     edi,[esi + SIZE CURSORCACHE][ecx]
        push    pGNCrtcCXOffset
        push    word ptr ds:[esi].CURSORCACHE.wCursorX
        push    word ptr ds:[esi].CURSORCACHE.wCursorY
        call    GN_CheckPanAndMoveCursor

        and     ds:[esi].CURSORCACHE.dwCursorFlags,NOT CURSOR_MOVE

Move1:
        push    pGNCrtcCXOffset
        pushd   1
        pushd   0
        call    GN_TurnCursorOffOrOn

        ; turn off the blocking semaphore
        and     ds:[esi].CURSORCACHE.dwCursorFlags,NOT CURSOR_SEMAPHORE

BlockedSemaphore:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_MoveCursor
;
; Purpose:      This function is the MoveCursor display driver entry point.
;               This routine may be interrupt time -- be sure to preserve
;               the registers appropriately.
;
; Arguments:    wX, wY          coords of new cursor location
;
;==============================================================================
DECPROC GENERIC_MoveCursor, PASCAL, FRAME, FAR16, LOGOFF
PARMW   wX
PARMW   wY
LOCALD  ppGNCrtcCXOffset
LOCALD  dwCRTCCount
LOCALD  PhysicalXRes
LOCALD  PhysicalYRes
LOCALD  NewScanStart
OPENPROC
        PUSHR   alld,ds,es,fs,gs
        pushf

        ; Load up our context
        mov     ds,cs:[wFlatDataSel]
        mov     esi,cs:[CSpGNLogicalDevice]

        mov     dwCRTCCount,0
        lea     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     ppGNCrtcCXOffset,eax

        ; If this CRTC is not a NULL context ...
LoopCRTC:
        mov     eax,ppGNCrtcCXOffset
        mov     edi,ds:[eax]
        or      edi,edi
        je      NextCrtc

        push    edi
        push    word ptr wX
        push    word ptr wY
        call    GN_CheckPanAndMoveCursor

NextCrtc:
        ; Go to the next CRTC
        add     ppGNCrtcCXOffset,4
        inc     dwCRTCCount
        mov     esi,cs:[CSpGNLogicalDevice]
        mov     eax,dwCRTCCount
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCRTC

done:
        popf
        POPR    alld,ds,es,fs,gs
CLOSEPROC


;==============================================================================
;
; Function:     GN_CheckPanAndMoveCursor
;
; Purpose:      This function checks if the PhysicalViewport needs to pan
;               and updates its coordinates in the GNCRTCCX if so. This
;               function then figures out the physical postion of the
;               cursor relative to the virtual desktop.
;
; Arguments:    pGNCrtcCXOffset GNCRTCCX ptr
;               wX, wY          coords of new cursor location
;
;==============================================================================
DECPROC GN_CheckPanAndMoveCursor, PASCAL, FRAME, NEAR, LOGOFF
PARMD   pGNCrtcCXOffset
PARMW   wX
PARMW   wY
LOCALD  dwCRTCCount
LOCALD  PhysicalXRes
LOCALD  PhysicalYRes
LOCALD  NewScanStart
OPENPROC
        PUSHR   esi,edi

        ; Call the DIBENG first
        push    dword ptr wY
        push    cs:[CSlp16LogDevice]
        call    DIB_MoveCursorExt

        mov     edi,pGNCrtcCXOffset
        mov     esi,cs:[CSpGNLogicalDevice]

        ; If the cursor has not been enabled, then we are done
        mov     ebx,ds:[edi].GNCRTCCX.pCursorCache
        or      ebx,ebx
        je      done

        cmp     ds:[edi].GNCRTCCX.dwPanAndScanEnabled,0
        je      skip_pan

        mov     NewScanStart,0
        sub     edx,edx

        ; Check to make sure the cusor coordinates are inside the
        ; virtual desktop window.
        mov     ax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        cmp     ax,wX
        jge     @F
        mov     dx,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        cmp     ax,wY
        jl      skip_pan

@@:
        ; Calculate the physical X and Y of the ViewPort.
        mov     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwRight
        sub     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft
        mov     PhysicalXRes,eax
        mov     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwBottom
        sub     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop
        mov     PhysicalYRes,eax

        mov     dx,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        mov     eax,PhysicalXRes
        cmp     edx,eax
        jne     check_XLeft
        
        mov     dx,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        mov     eax,PhysicalYRes
        cmp     edx,eax
        jne     check_YTop
        jmp     skip_pan

check_XLeft:
        movzx   eax,wX
        cmp     ax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        jle     @F
        mov     ax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
@@:
        cmp     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft
        jge     check_XRight
        
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft,eax
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwRight,eax
        mov     edx,PhysicalXRes
        add     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwRight,edx
        mov     NewScanStart,1
        jmp     check_YTop

check_XRight:
        cmp     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwRight
        jle     check_YTop

        inc     eax
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft,eax
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwRight,eax
        mov     edx,PhysicalXRes
        sub     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft,edx
        mov     NewScanStart,1

check_YTop:
        movzx   eax,wY
        cmp     ax,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        jle     @F
        mov     ax,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
@@:
        cmp     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop
        jge     check_YBottom
        
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop,eax
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwBottom,eax
        mov     edx,PhysicalYRes
        add     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwBottom,edx
        mov     NewScanStart,1
        jmp     set_scanline_start

check_YBottom:
        cmp     eax,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwBottom
        jle     set_scanline_start

        inc     eax
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop,eax
        mov     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwBottom,eax
        mov     edx,PhysicalYRes
        sub     ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop,edx
        mov     NewScanStart,1

set_scanline_start:
        cmp     NewScanStart,1
        jne     skip_pan

        ; Prevent H/W access when in DOS FS or PowerDown mode
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
        jne     skip_pan

        push    edi
        call    GN_SetPanRectangle
        
skip_pan:

        ; Save the coords for later
        mov     ebx,ds:[edi].GNCRTCCX.pCursorCache
        mov     eax,dword ptr wY
        ror     eax,10H
        mov     dword ptr ds:[ebx].CURSORCACHE.wCursorX,eax

        ; Is this a hardware or software cursor
        ; For software cursors, we don't have to enumerate the
        ; CRTCs for Clone Mode since a software cursor is embedded
        ; in the frame buffer. Writing it once will have it show
        ; up on all cloned monitors.
        mov     ebx,ds:[edi].GNCRTCCX.pCursorCache
        test    ds:[ebx].CURSORCACHE.dwCursorFlags,CURSOR_HAS_TRAILS OR CURSOR_IS_SOFTWARE
        jnz     done

        ; Since this is a hardware cursor, we need to subtract the position
        ; of the topleft corner from the cursor position of the virtual
        ; desktop in order to get the cursor position relative to the
        ; physical desktop.
        mov     edx,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwTop
        shl     edx,10h
        sub     eax,edx
        mov     edx,ds:[edi].GNCRTCCX.rcPhysicalViewport.dwLeft
        sub     ax,dx
        
        ; We need the actual CURSORDESC for the selected cursor
        ; since we need to know its hot spot
        mov     ecx,ds:[ebx].CURSORCACHE.dwSelectedCacheSlot
        imul    ecx,SIZE CURSORDESCEX
        lea     edx,[ebx + SIZE CURSORCACHE][ecx]

        ; Prevent H/W access when in DOS FS or PowerDown mode
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
        jne     done

        push    ds:[edi].GNCRTCCX.pHWCrtcCX
        push    eax
        push    dword ptr ds:[edx].CURSORDESCEX.wHotX
        call    HW_CursorMove

done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_CheckCursor
;
; Purpose:      This function is the CheckCursor display driver entry point.
;               This routine may be interrupt time -- be sure to preserve
;               the registers appropriately.
;
;==============================================================================
DECPROC GENERIC_CheckCursor, PASCAL, FRAME, FAR16, LOGOFF
LOCALD  ppGNCrtcCXOffset
LOCALD  dwCRTCCount
OPENPROC
        PUSHR   alld,ds,es,fs,gs
        pushf

        ; Just in case the driver is deinitted but we still get called
        cmp     cs:[CSpGNLogicalDevice],0
        je      Done

        ;!!!!!! FIGURE OUT WHERE IS THE CRITICAL INTERVAL !!!!!!
        sub     eax,eax
        mov     ax,cs:[wFlatDataSel]
        lar     edx,eax
        jnz     Done
        and     dh,NOT 7
        cmp     dh,0F0H
        jne     Done
        mov     ds,ax
        mov     esi,cs:[CSpGNLogicalDevice]

        ; Always call the DIBENGINE CheckCursor just in case
        push    cs:[CSlp16LogDevice]
        call    DIB_CheckCursorExt

        ; Prevent H/W access when in DOS FS or PowerDown mode
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
        jne     Done

        ; This is used as a time base for a few things in the driver
        inc     ds:[esi].GNLOGDEVCX.dwCheckCursorCount
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        inc     ds:[eax].GNBOARDCX.dwCheckCursorTicksSinceSecondDriverLoaded

        ; Deal with Windows bug doing repaint request processing
        call    GN_CheckUserRepaint

        ; Set up to enumerate all CRTC attached to this logical device
        mov     dwCRTCCount,0
        lea     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     ppGNCrtcCXOffset,eax

        ; If this CRTC is not a NULL context ...
LoopCRTC:
        mov     eax,ppGNCrtcCXOffset
        mov     edi,ds:[eax]
        or      edi,edi
        je      NextCrtc

        ; If the cursor has not been enabled, then we are done
        mov     esi,ds:[edi].GNCRTCCX.pCursorCache
        or      esi,esi
        je      NextCrtc

        ; If the cursor is NULL, then we're done
        test    ds:[esi].CURSORCACHE.dwCursorFlags,CURSOR_IS_NULL
        jnz     NextCrtc

        ; If the cursor is software, then we're done
        test    ds:[esi].CURSORCACHE.dwCursorFlags,CURSOR_HAS_TRAILS OR CURSOR_IS_SOFTWARE
        jnz     NextCrtc

        ; If the cursor has no action to take then we are done
        test    ds:[esi].CURSORCACHE.dwCursorFlags,(((1 SHL MAX_CACHEABLE_CURSORS) - 1) SHL NUM_CURSOR_FLAGS) OR CURSOR_MOVE OR CURSOR_SELECT
        jz      NextCrtc

        push    edi
        call    GN_CursorAction

NextCrtc:
        ; Go to the next CRTC
        add     ppGNCrtcCXOffset,4
        inc     dwCRTCCount
        mov     esi,cs:[CSpGNLogicalDevice]
        mov     eax,dwCRTCCount
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCRTC

Done:
        popf
        POPR    alld,ds,es,fs,gs
CLOSEPROC


;==============================================================================
;
; Function:     GN_TurnCursorOffOrOn
;
; Purpose:      This function turns off the cursor. If the cursor is
;               software, this routine do nothing.
;
; Arguments:
;               ds                      CX Sel
;               pGNCrtcCXOffset         GNCRTCCX ptr
;               dwZeroForOffOneForOn    Just what it says
;               dwMoveCursor            if this parameter s non-0, then the
;                                       cursor position is updated as well
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_TurnCursorOffOrOn, PASCAL, FRAME, FAR32, LOGOFF
PARMD   pGNCrtcCXOffset
PARMD   dwZeroForOffOneForOn
PARMD   dwMoveCursor
OPENPROC
        PUSHR   esi,edi

        mov     esi,pGNCrtcCXOffset
        mov     edi,ds:[esi].GNCRTCCX.pCursorCache
        or      edi,edi
        je      Done

        ; Is mouse trails active?
        test    ds:[edi].CURSORCACHE.dwCursorFlags,CURSOR_HAS_TRAILS OR CURSOR_IS_SOFTWARE
        jnz     Done

        cmp     dwZeroForOffOneForOn,0
        je      HwOff

        ; Are we supposed to move the cursor?
        cmp     dwMoveCursor,0
        je      @F
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        push    dword ptr ds:[edi].CURSORCACHE.wCursorX
        pushd   0
        call    HW_CursorMove

@@:
        ; Don't turn the cursor on if it is NULL ...
        test    ds:[edi].CURSORCACHE.dwCursorFlags,CURSOR_IS_NULL
        jnz     Done

        ; Turn the cursor back on
        push    esi
        call    GN_CursorAction

HwOff:
        ; Turn off/on the hardware cursor
        push    ds:[esi].GNCRTCCX.pHWCrtcCX
        pushd   dwZeroForOffOneForOn
        call    HW_CursorEnable

Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_ResetSelectedCursor
;
; Purpose:      This function takes the currently selected cursor
;               and recalls SetCursor with the pattern. We need
;               this for times when there is a modeset or whatever
;               and the cursor gets turned off and marked as NULL,
;               but the system does not call SetCursor to turn it
;               back on.
;
; Arguments:
;               ds                      CX Sel
;               pGNCrtcCXOffset         GNCRTCCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_ResetSelectedCursor, PASCAL, FRAME, FAR32, LOGOFF
PARMD   pGNCrtcCXOffset
LOCALD  lpResetCursorShape
OPENPROC
        PUSHR   esi,edi
        mov     edi,pGNCrtcCXOffset
        mov     esi,ds:[edi].GNCRTCCX.pCursorCache
        mov     eax,ds:[esi].CURSORCACHE.dwSelectedCacheSlot

        ; Call the hw specific code to set the cursor pattern into vidmem
        imul    ecx,eax,SIZE CURSORDESCEX
        lea     edi,[esi + SIZE CURSORCACHE][ecx]
        lea     ebx,[edi].CURSORDESCEX.wHotX

        mov     eax,1
        mov     ecx,SIZE CURSORDESC + (CURSOR_WIDTH * CURSOR_HEIGHT * 4)
        call    GN_AllocSelectorAndMapIt
        shl     eax,10H
        mov     lpResetCursorShape,eax

        push    lpResetCursorShape
        CALLFAR16 GENERIC_SetCursor

        mov     eax,lpResetCursorShape
        shr     eax,10H
        call    GN_UnmapAndFreeSelector

        POPR    esi,edi
CLOSEPROC

CLOSESEG _NVTEXT32

END


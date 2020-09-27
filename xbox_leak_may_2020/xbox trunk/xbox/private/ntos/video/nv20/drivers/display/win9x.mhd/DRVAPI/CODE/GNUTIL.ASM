;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnutil.asm
;
; Purpose:      This file holds services that the rest of the driver
;               commonly needs.
;==============================================================================
.586
incLogical  = 1
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
include modeext.inc
include escape.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GN_MemoryAlloc
;
; Purpose:      This function is called to purchase system memory.
;
; Arguments:    eax     amount of memory to purchase
;
; Returns:      eax     0       then failed
;                       non-0   then success
;                               In this case edx = selector to memory block
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_MemoryAlloc, PASCAL, NOFRAME, FAR16
OPENPROC
        ; 2000H = GMEM_SHARE and 0040H = GMEM_ZEROINIT
        mov     edx,2040H
        push    dx
        push    eax
        call    GlobalAlloc
        sub     edx,edx
        or      ax,ax
        je      done

        ; Allocation was successful. Now lock the memory
        push    ax
        push    ax
        call    GlobalLock
        or      dx,dx
        jne     @F

        ; Lock failed so free the memory
        call    GlobalFree
        sub     edx,edx
        jmp     done
@@:
        ; Return edx = selector and ax nonzero to indicate success
        movzx   edx,dx
        pop     ax
done:
CLOSEPROC


;==============================================================================
;
; Function:     GN_MemoryFree
;
; Purpose:      This function is called to free system memory.
;
; Arguments:    eax     selector of memory block to free
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_MemoryFree, PASCAL, NOFRAME, FAR16
OPENPROC
        ; Get the handle corresponding to this selector
        push    ax
        call    GlobalHandle
        or      ax,ax
        je      done

        ; Unlock the memory block and free it
        push    ax
        push    ax
        call    GlobalUnlock
        call    GlobalFree
done:
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetSelectorBase
;
; Purpose:      This function is called to get the base of
;               a selector.
;
; Arguments:
;               dwSelector              Selctor to get base of
;
; Returns:
;               eax                     Linear Base of selector
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetSelectorBase, PASCAL, FRAME, FAR16
PARMD   dwSelector
OPENPROC
        push    word ptr dwSelector
        call    GetSelectorBase
        shl     edx,10H
        mov     dx,ax
        mov     eax,edx
CLOSEPROC


;==============================================================================
;
; Function:     GN_AllocSelectorAndNullIt
;
; Purpose:      This function allocates a selector and sets its base
;               and limit to zero.
;
; Arguments:    None
;
; Returns:      eax     selector if successful
;                       0        if failure
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_AllocSelectorAndNullIt, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   edi

        ; Alloc a selector
        mov     ax,cs
        push    ax
        call    AllocCSToDSAlias
        movzx   edi,ax
        or      eax,eax
        je      Done

        ; Set selector to be small, data, and read/write
        mov     eax,00010012H
        push    di
        push    eax
        call    SelectorAccessRights

        ; Set selector base to NULL
        sub     eax,eax
        push    di
        push    eax
        call    SetSelectorBase

        ; Set selector limit to NULL
        sub     eax,eax
        push    di
        push    eax
        call    SetSelectorLimit

        ; Return the selector
        mov     eax,edi
Done:
        POPR    edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_AllocSelectorAndMapIt
;
; Purpose:      This function allocates a selector and sets its base
;               and limit to the values passed in. The selector is
;               always forced to be large, data, read/write.
;
; Arguments:    eax     0       then address in ebx is a physical address
;                       non-0   then address in ebx is a linear address
;               ebx     base address -- physical or linear based on eax
;               ecx     limit of selector
;
; Returns:      eax     selector if successful
;                       0        if failure
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_AllocSelectorAndMapIt, PASCAL, FRAME, FAR16
LOCALD  dwBase
LOCALD  dwLimit
LOCALW  wSelector
OPENPROC
        PUSHR   esi,edi

        mov     dwBase,ebx
        mov     dwLimit,ecx

        or      eax,eax
        jne     @F

        ; Convert the physical address to a linear address
        mov     cx,word ptr dwBase+0
        mov     bx,word ptr dwBase+2
        mov     di,word ptr dwLimit+0
        mov     si,word ptr dwLimit+2
        mov     eax,800H
        int     31H
        mov     eax,0
        jc      Done

        ; Call succeeded -- linear address is in bx:cx
        mov     word ptr dwBase+0,cx
        mov     word ptr dwBase+2,bx
@@:
        ; Get a selector
        call    GN_AllocSelectorAndNullIt
        or      eax,eax
        je      Done
        mov     wSelector,ax

        ; Set the selector to be large, data, read/write
        push    wSelector
        pushd   00008012H
        call    SelectorAccessRights

        ; Set the selector base
        push    wSelector
        push    dwBase
        call    SetSelectorBase

        ; Set the selector limit
        dec     dwLimit
        push    wSelector
        push    dwLimit
        call    SetSelectorLimit
        movzx   eax,wSelector
Done:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_UnmapAndFreeSelector
;
; Purpose:      This function sets the mapping for a selector to be
;               small, data. read/write with a NULL base and limit and
;               then frees the selector.
;
; Arguments:    eax     selector to free
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_UnmapAndFreeSelector, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   edi

        ; Safety check
        or      eax,eax
        jz      Exit

        mov     edi,eax

        ; Set selector to be small, data, and read/write
        mov     eax,00010012H
        push    di
        push    eax
        call    SelectorAccessRights

        ; Set selector base to NULL
        sub     eax,eax
        push    di
        push    eax
        call    SetSelectorBase

        ; Set selector limit to NULL
        sub     eax,eax
        push    di
        push    eax
        call    SetSelectorLimit

        ; Free selector
        push    di
        call    FreeSelector

Exit:
        POPR    edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetDeviceLocalBasePath
;
; Purpose:      This function just gets the local base path for
;               the specified devnode.
;
; Arguments:
;               dwDevnodeHandle         Device Node handle
;               dwPathSel               sel:offset of place to store
;               dwPathOffset               the monitor registry path
;               dwDeviceNameSel         sel:offset of the device type
;               dwDeviceNameOffset      should be either "Display"
;                                       or "Monitor"
;
;
; Returns:
;               If this routine is successful then the first dword
;               pointed to by dwPathSel:dwPathOffset gets stuffed
;               with a constant HKEY like HKEY_LOCAL_MACHINE. All
;               bytes after that are a registry path inside the key
;               given in the first dword that are that local base
;               registry path for the given device.
;
;               ecx     SPECIFIC_DEVICE
;                       UNKNOWN_DEVICE
;                       PLUG_AND_PLAY_DEVICE
;
; Preserve:     ds,es,fs,esi,edi
;==============================================================================
szToDevice      DB      "System\CurrentControlSet\Services\Class\",0

USE_EBP TEXTEQU <1>

DECPROC GN_GetDeviceLocalBasePath, PASCAL, FRAME, FAR16
PARMD   dwDevnodeHandle
PARMD   dwPathSel
PARMD   dwPathOffset
PARMD   dwDeviceNameSel
PARMD   dwDeviceNameOffset
LOCALS  szPath,MAX_KEY_LEN
LOCALS  szDevice,MAX_KEY_LEN
LOCALS  szDeviceName,MAX_KEY_LEN
LOCALS  szDriver,16
LOCALS  szDeviceDesc,16
LOCALD  dwStackOffsetToDeviceName
LOCALD  hLocalKey
LOCALD  dwSize
LOCALD  dwUnknownDevice
OPENPROC
        PUSHR   alld,ds,es,fs
        mov     dwUnknownDevice,SPECIFIC_DEVICE

        ; Place "Enum\" into szPath
        mov     dword ptr szPath[0],'munE'
        mov     byte  ptr szPath[4],'\'

        cmp     dwDevnodeHandle,0
        je      SetDefaultPath

        ; Create a flat selector to all of memory
        mov     eax,1
        sub     ebx,ebx
        mov     ecx,0FFFFFFFEH
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      SetDefaultPath
        mov     fs,ax

        ; The second DWORD in the devnode structure is a ptr to
        ; a registry path string name.
        mov     eax,dwDevnodeHandle
        mov     edi,fs:[eax + 4]
        cmp     edi,0AAAAAAAAH
        je      SkipCopy

        ; Copy the string to local storage
        mov     ecx,5
@@:     mov     al,fs:[edi]
        inc     edi
        mov     szPath[ecx],al
        inc     ecx
        or      al,al
        jne     @B

SkipCopy:
        mov     ax,fs
        call    GN_UnmapAndFreeSelector
        cmp     edi,0AAAAAAAAH
        je      SetDefaultPath

        ; Open the key
        pushd   HKEY_LOCAL_MACHINE      ; hKey
        mov     ax,ss
        lea     ebx,szPath
        push    ax
        push    bx                      ; lpSubKey
        lea     ebx,hLocalKey
        push    ax
        push    bx                      ; phkResult
        call    RegCreateKey
        shl     edx,10H
        mov     dx,ax
        cmp     edx,ERROR_SUCCESS
        jne     SetDefaultPath

        ; Read the "Driver" string in this key. It should be equal
        ; to something like: "Display\0003"  or "Monitor\0001"
        ; Monitor or Display may be upper or lower case
        mov     dword ptr szDriver[0],'virD'
        mov     dword ptr szDriver[4],'re'
        sub     ecx,ecx
        mov     szDevice[0],cl
        mov     dwSize,128
        mov     ax,ss
        shl     eax,10H
        push    hLocalKey
        lea     ax,szDriver
        push    eax
        push    ecx
        push    ecx
        lea     ax,szDevice
        push    eax
        lea     ax,dwSize
        push    eax
        call    RegQueryValueEx

        ; Read the "DeviceDesc" string in this key. It should be equal
        ; to something like: "(Unknown Monitor)" or "Default Monitor".
        ; If it is, then we should flag it as an unknown device.
        mov     dword ptr szDeviceDesc[0],'iveD'
        mov     dword ptr szDeviceDesc[4],'eDec'
        mov     dword ptr szDeviceDesc[8],'cs'
        sub     ecx,ecx
        mov     szDeviceName[0],cl
        mov     dwSize,128
        mov     ax,ss
        shl     eax,10H
        push    hLocalKey
        lea     ax,szDeviceDesc
        push    eax
        push    ecx
        push    ecx
        lea     ax,szDeviceName
        push    eax
        lea     ax,dwSize
        push    eax
        call    RegQueryValueEx

        push    hLocalKey
        call    RegCloseKey

        cmp     byte ptr szDevice[0],0
        je      SetDefaultPath

MakePath:
        ; Let's build the final string. First,
        ; System\CurrentControlSet\Services\Class\
        mov     ds,dwPathSel
        mov     ebx,dwPathOffset
        mov     dword ptr ds:[ebx],HKEY_LOCAL_MACHINE
        add     ebx,4
        sub     ecx,ecx
@@:     mov     al,cs:szToDevice[ecx]
        inc     ecx
        mov     ds:[ebx],al
        inc     ebx
        or      al,al
        jne     @B

        ; Attach the device and number for something like
        ; System\CurrentControlSet\Services\Class\Display\0005
        dec     ebx
        sub     ecx,ecx
@@:     mov     al,szDevice[ecx]
        inc     ecx
        mov     ds:[ebx],al
        inc     ebx
        or      al,al
        jne     @B

        ; If DeviceDesc="(Unknown Monitor)" , then flag it.
        ; Windows 98 uses this string to indicate unknown monitors.
        sub     eax,eax
        cmp     dword ptr szDeviceName[0],'knU('
        jne     Check2
        cmp     dword ptr szDeviceName[4],'nwon'
        jne     Check2
        cmp     dword ptr szDeviceName[8],'noM '
        jne     Check2
        cmp     dword ptr szDeviceName[12],'roti'
        jne     Check2
        cmp     byte ptr szDeviceName[16],')'
        je      MarkAsUnknown

        ; If DeviceDesc="Default Monitor" , then flag it as unknown.
        ; Windows 98 SE2 uses this string to indicate unknown monitors.
Check2: cmp     dword ptr szDeviceName[0],'afeD'
        jne     Check3
        cmp     dword ptr szDeviceName[4],' tlu'
        jne     Check3
        cmp     dword ptr szDeviceName[8],'inoM'
        jne     Check3
        cmp     dword ptr szDeviceName[12],'rot'
        jne     Check3
        je      MarkAsUnknown

        ; If DeviceDesc="Plug and Play Monitor" , then flag it as
        ; an unknown monitor since no INF file is really installed.
Check3: sub     eax,eax
        cmp     dword ptr szDeviceName[0],'gulP'
        jne     Check4
        cmp     dword ptr szDeviceName[4],'dna '
        jne     Check4
        cmp     dword ptr szDeviceName[8],'alP '
        jne     Check4
        cmp     dword ptr szDeviceName[12],'oM y'
        jne     Check4
        cmp     dword ptr szDeviceName[16],'otin'
        jne     Check4
        cmp     byte ptr szDeviceName[20],'r'
        mov     dwUnknownDevice,PLUG_AND_PLAY_DEVICE

Check4: jmp     Done

MarkAsUnknown:
        mov     dwUnknownDevice,UNKNOWN_DEVICE
        jmp     Done

SetDefaultPath:
        mov     ds,dwDeviceNameSel
        mov     ecx,dwDeviceNameOffset
        sub     ebx,ebx
@@:     mov     al,ds:[ecx][ebx]
        mov     szDevice[ebx],al
        inc     ebx
        or      al,al
        jne     @B
        mov     dword ptr szDevice[ebx - 1],'000\'
        mov     dword ptr szDevice[ebx + 3],'0'
        mov     dwUnknownDevice,UNKNOWN_DEVICE
        jmp     MakePath

Done:
        POPR    alld,ds,es,fs
        mov     ecx,dwUnknownDevice
CLOSEPROC

;==============================================================================
;
; Function:     GN_CheckInterlacedStereoConnection
;
; Purpose:      This function determines if stereo is enabled and 
;               stereo connection type is interlaced (aka ASUS).
;
; Arguments:
;               None
;
;
; Returns:
;               If stereo is disabled or stereo connection type 
;               is not interlaced the function returns 0 otherwise
;               guess what? Absolutely right. Not zero.
;               bytes after that are a registry path inside the key
;
; Preserve:     ds,es,fs,esi,edi
;==============================================================================
szToStereo      DB      "Software\NVIDIA Corporation\Global\Stereo3D",0

DECPROC GN_CheckInterlacedStereoConnection, PASCAL, FRAME, FAR16
LOCALS  szSubKey,MAX_KEY_LEN
LOCALD  hLocalKey
LOCALD  dwSize
LOCALD  dwInterlacedStereo
LOCALD  dwValue
OPENPROC
        PUSHR   alld,ds,es,fs
        mov     dwInterlacedStereo,0

        ; Open the key
        pushd   HKEY_LOCAL_MACHINE      ; hKey
        mov     ax,cs
        lea     ebx,szToStereo
        push    ax
        push    bx                      ; lpSubKey
        lea     ebx,hLocalKey
        mov     ax,ss
        push    ax
        push    bx                      ; phkResult
        call    RegOpenKey
        shl     edx,10H
        mov     dx,ax
        cmp     edx,ERROR_SUCCESS
        jne     Done

        ; Read the "StereoEnable" string. If it is not 0 then stereo is enable
        ; Place "StereoEnable" into szSubKey
        mov     dword ptr szSubKey[ 0],'retS'
        mov     dword ptr szSubKey[ 4],'nEoe'
        mov     dword ptr szSubKey[ 8],'elba'
        mov     dword ptr szSubKey[12],0

        sub     ecx,ecx
        mov     dwValue, ecx
        mov     dwSize,4
        mov     ax,ss
        shl     eax,10H
        push    hLocalKey
        lea     ax,szSubKey
        push    eax
        push    ecx
        push    ecx
        lea     ax,dwValue
        push    eax
        lea     ax,dwSize
        push    eax
        call    RegQueryValueEx
        shl     edx,10H
        mov     dx,ax
        cmp     edx,ERROR_SUCCESS
        jne     Done_And_Close

        cmp     dwValue,0
        je      Done_And_Close

        ; Read the "StereoViewerType" string. If it is STEREOCFG_INTERLACED then stereo is interlaced
        ; Place "StereoViewerType" into szSubKey
        mov     dword ptr szSubKey[ 0],'retS'
        mov     dword ptr szSubKey[ 4],'iVoe'
        mov     dword ptr szSubKey[ 8],'rewe'
        mov     dword ptr szSubKey[12],'epyT'
        mov     dword ptr szSubKey[16],0

        sub     ecx,ecx
        mov     dwValue, ecx
        mov     dwSize,4
        mov     ax,ss
        shl     eax,10H
        push    hLocalKey
        lea     ax,szSubKey
        push    eax
        push    ecx
        push    ecx
        lea     ax,dwValue
        push    eax
        lea     ax,dwSize
        push    eax
        call    RegQueryValueEx
        shl     edx,10H
        mov     dx,ax
        cmp     edx,ERROR_SUCCESS
        jne     Done_And_Close

        cmp     dwValue,STEREOCFG_INTERLACED
        jne      Done_And_Close
        mov     dwInterlacedStereo,1

Done_And_Close:
        push    hLocalKey
        call    RegCloseKey

Done:
        POPR    alld,ds,es,fs
        mov     eax,dwInterlacedStereo
CLOSEPROC

CLOSESEG _NVTEXT16

END


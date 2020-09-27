;
;  (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
;
;********* Operating System Interface for NVidia Resource Manager **********;
;                                                                           ;
; Module: VXDSTUB.ASM                                                       ;
;       These routines provide an operating system independent layer        ;
;       between the Resource Manager and the underlying OS APIs.            ;
;                                                                           ;
;---------------------------------------------------------------------------;
;                                                                           ;
; History:                                                                  ;
;        David Schmenk (dschmenk)     11/17/93 - wrote it                   ;
;        David Schmenk (dschmenk)     06/15/94 - rewrote for real RM        ;
;        JohnH                        10/3/97 - rewrote for MS tools--      ;
;                                     include DDB and service table here    ;
;*********************** Copyright 1993 NVidia, Inc. ***********************;

.386p

INCLUDE BASEDEF.INC
INCLUDE VMM.INC
INCLUDE VWIN32.INC
INCLUDE VPICD.INC
INCLUDE V86MMGR.INC
INCLUDE VNVRMD.INC
INCLUDE DEBUG.INC
include         ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc


VxD_LOCKED_DATA_SEG
PUBLIC  dwSrcLinearPageNum, dwDstLinearPageNum, dwLinearRegionSizeInPages, dwPageDirectoryBaseRegLinear
dwSrcLinearPageNum              DD      0
dwDstLinearPageNum              DD      0
dwLinearRegionSizeInPages       DD      0
dwPageDirectoryBaseRegLinear    DD      0

PUBLIC  dwPageTableEntry
dwPageTableEntry                DD      0
PUBLIC	dwGotNulPTE, dwNulPTE
dwGotNulPTE			DD	0
dwNulPTE			DD	0
VxD_LOCKED_DATA_ENDS


VxD_LOCKED_CODE_SEG

;   The Device Declartation Block

;Declare_Virtual_Device MACRO Name, Major_Ver, Minor_Ver, Ctrl_Proc, Device_Num, Init_Order, V86_Proc, PM_Proc, Reference_Data
;    LOCAL   V86_API_Offset, PM_API_Offset, Serv_Tab_Offset, Serv_Tab_Len, Ref_Data_Offset

IFDEF NV4_HW
Declare_Virtual_Device  NVCORE, NVRM_Major_Version, NVRM_Minor_Version, rmControlEntry, UNDEFINED_DEVICE_ID, UNDEFINED_INIT_ORDER, \
                        ,                    \
                        rmUsrAPIEntry        \
                        ,
ENDIF

;---------------------------------------------------------------------------
;
;  These bogus routines are needed for the C compiler's optimizer function
;    Called as:
;               EAX = buffer offset
;               ECX = count
;               EDX = value
;
;---------------------------------------------------------------------------
BeginProc __STOSB
        jcxz    exitb
fillitb:
        dec     ecx
        mov     BYTE PTR [eax][ecx], dl
        jnz     fillitb
exitb:
        ret
EndProc __STOSB
BeginProc __STOSW
        jcxz    exitw
fillitw:
        dec     ecx
        mov     WORD PTR [eax][ecx*2], dx
        jnz     fillitw
exitw:
        ret
EndProc __STOSW
BeginProc __STOSD
        jcxz    exitd
fillitd:
        dec     ecx
        mov     DWORD PTR [eax][ecx*4], edx
        jnz     fillitd
exitd:
        ret
EndProc __STOSD
BeginProc _inpd
        push    edx
        mov     dx,  ax
        in      eax, dx
        pop     edx
        ret
EndProc _inpd
BeginProc _outpd
        xchg    eax, edx
        out     dx, eax
        ret
EndProc _outpd
BeginProc _inp
        push    edx
        mov     dx,  ax
        in      al, dx
        pop     edx
        ret
EndProc _inp
BeginProc _outp
        xchg    eax, edx
        out     dx, al
        ret
EndProc _outp
;---------------------------------------------------------------------------
;
;  VxD Control entrypoints.
;
;---------------------------------------------------------------------------
EXTRN   _rmControl : NEAR
public  rmControlEntry
BeginProc rmControlEntry
;       ebp points to the Client_Regiser_Structure
;       rmControl expects two args: msg, VMh
        push    ebx             ; VMh
        push    eax             ; msg
        call    _rmControl
        add     esp,2*4
        or      eax, eax
        stc
        jnz     @F
        clc
@@:     ret
EndProc rmControlEntry
;---------------------------------------------------------------------------
;
;  VxD User API entrypoints.
;
;---------------------------------------------------------------------------
EXTRN   @rmUsrAPI@8 : NEAR
public  rmUsrAPIEntry
BeginProc rmUsrAPIEntry
;       fix up arguments for the __fastcall protocol -- C function will use ecx & edx for first two args
        mov     ecx, ebx        ; VM handle
        mov     edx, ebp        ; user parameters (register structure)
        jmp     @rmUsrAPI@8     ; no arguments on stack, so no additional stack maintenance
EndProc rmUsrAPIEntry


;---------------------------------------------------------------------------
;
;  Moved from the RegisterDisplayDriover routine in the minivdd.
;
;---------------------------------------------------------------------------

; 200H bytes should be enough.
align   4
PUBLIC  LogdevSharedArea
LogdevSharedArea:
DD      200H    dup (0)

public  _RegisterDisplayDriver
BeginProc _RegisterDisplayDriver

        push    ebp
        mov     ebp,esp
        pushad
        mov     ebp,ss:[ebp + 8]

        ; The new architecture display driver needs some functions
        ; which can only be performed at the VxD level. I made a
        ; simple interface for these services out of the minivdd
        ; function RegisterDisplayDriver. The Client_EAX register
        ; is used up because it must hold the special code
        ; VDD_REGISTER_DISPLAY_DRIVER_INFO in order to get here,
        ; and Client_EBX is used up holding the system VM handle.
        ; The other registers are all free, so:
        ; CLIENT_ecx    - sub-function
        ; All other registers are sub-function specific.
        ;
        ; I placed the sub-function constants in nvrmarch at
        ; the time I wrote this code.
        mov     eax,[ebp].Client_Reg_Struc.Client_ECX
        cmp     eax,MVDD_ALLOC_LINEAR_ADDRESS_SPACE
        je      AllocLinearAddrSpace
        cmp     eax,MVDD_FREE_LINEAR_ADDRESS_SPACE
        je      FreeLinearAddrSpace
        cmp     eax,MVDD_COPY_PTES_FOR_LINEAR_RANGE
        je      CopyPtesForLinearRange
        cmp     eax,MVDD_ALLOC_MEMORY_BLOCK
        je      AllocSystemAreaMemory
        cmp     eax,MVDD_FREE_MEMORY_BLOCK
        je      FreeSystemAreaMemory
        cmp     eax,MVDD_GET_LOGDEV_SHARED_MEMORY
        je      GetLogdevSharedPtr
        jmp     MRDDExit

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_ALLOC_LINEAR_ADDRESS_SPACE -- this service allocates
        ; linear address space with no physical memory backing it.
        ;
        ; Parameters:
        ; Client_ECX = MVDD_ALLOC_LINEAR_ADDRESS_SPACE
        ; Client_EDX = amount of linear address space to alloc in bytes
        ;
        ; Returns:
        ; Client_EAX    Linear Base Address of linear space if successful
        ;               NULL if there was an error
AllocLinearAddrSpace:

        mov     eax,[ebp].Client_Reg_Struc.Client_EDX
        add     eax,0FFFH
        shr     eax,0CH
        VMMCall _PageReserve,<PR_SYSTEM,eax,PR_FIXED>
        mov     [ebp].Client_Reg_Struc.Client_EAX,eax
        cmp     eax,-1
        jne     MRDDExit
        mov     [ebp].Client_Reg_Struc.Client_EAX,0
        jmp     MRDDExit
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_FREE_LINEAR_ADDRESS_SPACE -- this service frees
        ; linear address space allocated via AllocLinearAddressSpace.
        ;
        ; Parameters:
        ; Client_ECX = MVDD_FREE_LINEAR_ADDRESS_SPACE
        ; Client_EDX = base of linear address space to free
        ; Client_EBX = length in bytes of the range to free
        ;
        ; Returns:      None
FreeLinearAddrSpace:
        mov     eax, [ebp].Client_Reg_Struc.Client_EDX
        shr     eax, 0CH
        mov     [dwDstLinearPageNum], eax
        mov     eax, [ebp].Client_Reg_Struc.Client_EBX
        add     eax, 0FFFH
        shr     eax, 0CH
        mov     [dwLinearRegionSizeInPages], eax

        ; First get a linear address for CR3 (The Page Directory Base Register)
        mov     eax, cr3
        and     eax, NOT 0FFFh
        VMMCall _MapPhysToLinear,<eax,1000H,0>
        mov     [dwPageDirectoryBaseRegLinear], eax
@@:
        ; We need to remap all the pages behind this linear address
        ; space to the nul PTE so that when we PageFree, Windows
        ; does not try and decommit anything.
        mov     eax, [dwPageDirectoryBaseRegLinear]
        mov     ebx, [dwDstLinearPageNum]
        shr     ebx, 10
        and     ebx, 3FFH
        lea     ebx, [eax][ebx * 4]

        mov     eax, [ebx]
        and     eax, 0FFFFF000H
        VMMCall _MapPhysToLinear,<eax,1000H,0>

        mov     ebx, [dwDstLinearPageNum]
        and     ebx, 3FFH
        lea     ebx, [eax][ebx * 4]

        ; We now have a ptr in ebx to the place to put the new PTE
        mov	eax,dwNulPTE
        mov     [ebx], eax

        ; Go to the next PTEs
        inc     [dwDstLinearPageNum]

        dec     [dwLinearRegionSizeInPages]
        jne     @B

	; Now that we have mapped all PTEs to NULL, we can free the
	; linear range and Windows won't try to decommit anything.
        mov     eax,[ebp].Client_Reg_Struc.Client_EDX
        VMMcall _PageFree,<eax,0>
        jmp     MRDDExit
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_COPY_PTES_FOR_LINEAR_RANGE -- this service copies page
        ; table entries for the pages behind one linear range to the
        ; PTEs for the pages behind another linear range. This makes
        ; the two linear ranges address the same physical memory.
        ;
        ; Parameters:
        ; Client_ECX = MVDD_COPY_PTES_FOR_LINEAR_RANGE
        ; Client_EDX = base of linear address space to copy PTEs from
        ; Client_EBX = base of linear address space to copy PTEs to
        ; Client_EDI = size of linear region in bytes to copy PTEs for
        ;
        ; Returns:      None
CopyPtesForLinearRange:
        mov     eax, [ebp].Client_Reg_Struc.Client_EDX
        shr     eax, 0CH
        mov     [dwSrcLinearPageNum], eax
        mov     eax, [ebp].Client_Reg_Struc.Client_EBX
        shr     eax, 0CH
        mov     [dwDstLinearPageNum], eax
        mov     eax, [ebp].Client_Reg_Struc.Client_EDI
        add     eax, 0FFFH
        shr     eax, 0CH
        mov     [dwLinearRegionSizeInPages], eax

IF 0
        ; CopyPageTable does NOT work when you are running SoftICE
        ; if the dst page table entry you are copying over was
        ; originally part of an AllocLinearAddrSpace. SoftICE marks
        ; PTES behind an AllocLinearAddrSpace call with 000B3000
        ; (if SoftICE isn't running, non-physically-backed pages
        ; on AllocLinearAddrSpace are marked differently).
        ; CopyPageTable seems to fail if a current dst PTE is
        ; set to 000B3000 (or maybe SoftICE hooks CopyPageTable as
        ; well and blocks if the current dst PTE is 000B3000).
@@:
        ; Get the first src PTE
        mov     eax, OFFSET [dwPageTableEntry]
        VMMcall _CopyPageTable,<dwSrcLinearPageNum,1,eax,0>
        inc     [dwSrcLinearPageNum]

        ; Write the PTE for the dst region
        mov     eax, [dwPageTableEntry]
        shr     eax, 0CH
        VMMcall _PageCommitPhys,<dwDstLinearPageNum,1,eax,PC_USER + PC_WRITEABLE>
        inc     [dwDstLinearPageNum]

        ; Do the whole range
        dec     [dwLinearRegionSizeInPages]
        jne     @B
ELSE
        ; Because of SoftICE, I must bang the PTEs myself!
        ; NOTE!  This WILL NOT WORK with Physical Address Extension - i.e. P6
        ; architecture extension to 36 address bits (currently available only
        ; in Intel Xeon processors).  This can be checked with the PAE flag
        ; (bit 5 of CR4).  If we are in 36 bit (PAE) mode, CR3 becomes a
        ; 27 bit page directory pointer table base address (PDPTR).  The top
        ; two bits of the linear address will select one of four entries in the
        ; page directory pointer table.  Also, PDEs and PTEs become 64 bits to
        ; accomodate 36 bit base addresses (i.e. there are 512 per 4K page).

        ; First get a linear address for CR3 (The Page Directory Base Register)
        mov     eax, cr3
        ; Mask off PCD, PWT flags and other detritus - fixes bug 1990807-124351
        and     eax, NOT 0FFFh
        VMMCall _MapPhysToLinear,<eax,1000H,0>
        mov     [dwPageDirectoryBaseRegLinear], eax

@@:
        mov     eax, [dwPageDirectoryBaseRegLinear]
        ; add in 4 * (top 10 bits of the address) to get the PDE
        mov     ebx, [dwSrcLinearPageNum]
        shr     ebx, 10
        and     ebx, 3FFH
        lea     ebx, [eax][ebx * 4]

        ; Now get an address with which we can reference the Page Directory
        mov     eax, [ebx]
        and     eax, 0FFFFF000H
        VMMCall _MapPhysToLinear,<eax,1000H,0>

        ; add in 4 * (bits 21:12 of the address) to get the PTE
        mov     ebx, [dwSrcLinearPageNum]
        and     ebx, 3FFH
        lea     ebx, [eax][ebx * 4]
        mov     eax, [ebx]
        push    eax

        ; Now go through a similar process with the dst page
        mov     eax, [dwPageDirectoryBaseRegLinear]
        mov     ebx, [dwDstLinearPageNum]
        shr     ebx, 10
        and     ebx, 3FFH
        lea     ebx, [eax][ebx * 4]

        mov     eax, [ebx]
        and     eax, 0FFFFF000H
        VMMCall _MapPhysToLinear,<eax,1000H,0>

        mov     ebx, [dwDstLinearPageNum]
        and     ebx, 3FFH
        lea     ebx, [eax][ebx * 4]

	; remember whatever is in the first dst PTE that we find.
	; This PTE will be what we want to restore them to when we
	; free the linear range.
	cmp	dwGotNulPTE,0
	jne	GotNulPTE
	mov	ecx,[ebx]
	mov	dwNulPTE,ecx
	inc dwGotNulPTE
GotNulPTE:
        ; We now have a ptr in ebx to the place to put the new PTE
        pop     eax
        mov     [ebx], eax

        ; Go to the next PTEs
        inc     [dwSrcLinearPageNum]
        inc     [dwDstLinearPageNum]

        dec     [dwLinearRegionSizeInPages]
        jne     @B
ENDIF
        mov     eax, cr3        ; Flush TLBs
        mov     cr3, eax
        jmp     MRDDExit
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


AllocSystemAreaMemory:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_ALLOC_MEMORY_BLOCK -- this service allocates memory in
        ; the system area (3Ggg-4Gig) and returns a flat ptr to it.
        ; Client_ECX = MVDD_ALLOC_MEMORY_BLOCK
        ; Client_EDX = amount of memory to buy in bytes
        ;
        ; Returns:      CLIENT_EAX      flat ptr to memory block
        ;                               0 if there was an error

        mov     eax,[ebp].Client_Reg_Struc.Client_EDX
        add     eax,0FFFH
        shr     eax,12
        VMMcall _PageAllocate, <eax,PG_SYS,ebx,0,0,0,0,PAGEZEROINIT>
        mov     [ebp].Client_Reg_Struc.Client_EAX,eax

	; clear out the memory block
	mov	edi,eax
        mov     ecx,[ebp].Client_Reg_Struc.Client_EDX
        sub	eax,eax
        rep	stos byte ptr es:[edi]
        jmp     MRDDExit

FreeSystemAreaMemory:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_FREE_MEMORY_BLOCK -- this service frees memory in
        ; the system area (3Gig-4Gig)
        ; Client_ECX = MVDD_ALLOC_MEMORY_BLOCK
        ; Client_EDX = ptr to memory to free
        ;
        ; Returns:      None

        mov     eax,[ebp].Client_Reg_Struc.Client_EDX
        VMMcall _PageFree, <eax,0>
        jmp     MRDDExit

GetLogdevSharedPtr:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;  -- this service allocates memory in
        ; the system area (3Ggg-4Gig) and returns a flat ptr to it.
        ; Client_ECX = MVDD_GET_LOGDEV_SHARED_MEMORY
        ;
        ; Returns:      None

        mov     eax,OFFSET LogdevSharedArea
        mov     [ebp].Client_Reg_Struc.Client_EAX,eax
        jmp     MRDDExit

MRDDExit:
        popad
        pop     ebp
        ret
EndProc _RegisterDisplayDriver


;---------------------------------------------------------------------------
;
;  TIMER BIOS interface
;
;---------------------------------------------------------------------------
;
; BOOL TIMER(struct Client_Regs *);
;
BeginProc _TIMER
        push    ebp
        mov     ebp, esp
;
; Save all registers.
;
        pushad
;
; Load register structure for TIMER call.
;
        mov     esi, [ebp + 8]
        mov     eax, [esi + Client_EAX]
        mov     ebx, [esi + Client_EBX]
        mov     ecx, [esi + Client_ECX]
        mov     edx, [esi + Client_EDX]
        mov     edi, [esi + Client_EDI]
        mov     esi, [esi + Client_ESI]
;
; Call TIMER BIOS.
;
        push    ebp
        push    DWORD PTR 01ah
        VMMcall Exec_VxD_Int
        pop     ebp
;
; Fill register structure from TIMER call.
;
        push    esi
        mov     esi, [ebp + 8]
        mov     [esi + Client_EAX], eax
        mov     [esi + Client_EBX], ebx
        mov     [esi + Client_ECX], ecx
        mov     [esi + Client_EDX], edx
        mov     [esi + Client_EDI], edi
        pushfd
        pop     eax
        mov     [esi + Client_EFlags], eax
        pop     eax
        mov     [esi + Client_ESI], eax
;
; Restore registers and return.
;
        popad
        pop     ebp
        ret
EndProc _TIMER

;---------------------------------------------------------------------------
;
;  PCI BIOS interface
;
;---------------------------------------------------------------------------
;
; BOOL PCI(struct Client_Regs *);
;
BeginProc _PCI
        push    ebp
        mov     ebp, esp
;
; Save all registers.
;
        pushad
;
; Load register structure for PCI call.
;
        mov     esi, [ebp + 8]
        mov     eax, [esi + Client_EAX]
        mov     ebx, [esi + Client_EBX]
        mov     ecx, [esi + Client_ECX]
        mov     edx, [esi + Client_EDX]
        mov     edi, [esi + Client_EDI]
        mov     esi, [esi + Client_ESI]
;
; Call PCI BIOS.
;
        push    ebp
        push    DWORD PTR 01ah
        VMMcall Exec_VxD_Int
        pop     ebp
;
; Fill register structure from PCI call.
;
        push    esi
        mov     esi, [ebp + 8]
        mov     [esi + Client_EAX], eax
        mov     [esi + Client_EBX], ebx
        mov     [esi + Client_ECX], ecx
        mov     [esi + Client_EDX], edx
        mov     [esi + Client_EDI], edi
        pushfd
        pop     eax
        mov     [esi + Client_EFlags], eax
        pop     eax
        mov     [esi + Client_ESI], eax
;
; Restore registers and return.
;
        popad
        pop     ebp
        ret
EndProc _PCI

;---------------------------------------------------------------------------
;
;  Video BIOS interface
;
;---------------------------------------------------------------------------
;
; BOOL VBE(struct Client_Regs *);
;
BeginProc _VBE
        push    ebp
        mov     ebp, esp
;
; Save all registers.
;
        pushad
;
; Load register structure for PCI call.
;
        mov     esi, [ebp + 8]
        mov     eax, [esi + Client_EAX]
        mov     ebx, [esi + Client_EBX]
        mov     ecx, [esi + Client_ECX]
        mov     edx, [esi + Client_EDX]
        mov     edi, [esi + Client_EDI]
        mov     esi, [esi + Client_ESI]
;
; Call VBE BIOS.
;
        push    ebp
        push    DWORD PTR 010h
        VMMcall Exec_VxD_Int
        pop     ebp
;
; Fill register structure from PCI call.
;
        push    esi
        mov     esi, [ebp + 8]
        mov     [esi + Client_EAX], eax
        mov     [esi + Client_EBX], ebx
        mov     [esi + Client_ECX], ecx
        mov     [esi + Client_EDX], edx
        mov     [esi + Client_EDI], edi
        pop     eax
        mov     [esi + Client_ESI], eax
;
; Restore registers and return.
;
        popad
        pop     ebp
        ret
EndProc _VBE


;
; Use an int10h to our VGA BIOS to read the EDID from the Toshiba
; system BIOS (used for mobile systems).  Pass a buffer in crs.CRS.Client_EDI
; to copy the result to (128 bytes).  Based on the VBE30 function in this file.
; Called by the Win9x implementation of osGenerateInt10h.
;
; BOOL readToshibaEDID(struct Client_Regs *);
;
;
BeginProc _readToshibaEDID
        push    ebp
        mov     ebp, esp
;
; Save all registers.
;
        pushad
;
; Allocate 128 bytes in the local VM translation buffer, then
; use the returned v86 segment:offset as an absolute offset within
; the VM's real-mode space.
;
        mov     esi, [ebp + 8]
        mov     esi, [esi + Client_EDI] ; get flat offset of the table into esi
        push    esi                     ; store it for the copy back later

        VMMcall Get_Cur_VM_Handle       ; get the current vm handle into ebx

        push    ebp
        mov     ebp, [ebp + 8]          ; get a ptr to the client registers
        mov     ecx, 128                ;  buffer size
        stc                             ; set carry flag to copy, clear otherwise
;        clc
        push    ss                      ; pointer must be in fs:esi
        pop     fs
;fs:esi = source, ebp = destination, ebx = handle of current VM
        VxDcall V86MMGR_Allocate_Buffer
        pop     ebp                     ; v86 address in edi 
                                        ; (16 bits of seg, 16 bits of off)
                                        
                                        ; Should probably check cx to see if all
                                        ; data was allocated.  But 60 bytes should
                                        ; always fit in the default 4k translation

        mov     eax, edi                ; Break the v86 address into ax:di
        and     edi, 0FFFFh
        shr     eax, 16

        mov     esi, [ebp + 8]          ; Load it back into our local crs
        mov     [esi + Client_ES], ax
        mov     [esi + Client_EDI], edi

;
; Load register structure for PCI call.  Move the local crs values into the
; real registers.  Don't know if this is still required with the use of the 
; Push_Client_State macros, but it won't hurt for now.
;
        mov     esi, [ebp + 8]
        mov     eax, [esi + Client_EAX]
        mov     ebx, [esi + Client_EBX]
        mov     ecx, [esi + Client_ECX]
        mov     edx, [esi + Client_EDX]
        mov     edi, [esi + Client_EDI]
        mov     esi, [esi + Client_ESI]

;
; Call VBE BIOS.
;
;        push    ebp
;        push    DWORD PTR 010h
;        VMMcall Exec_VxD_Int
;        pop     ebp

        push    ebp                     ; save away ebp so we can get back 
                                        ; to our local crs
        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; 
        ; Move the register values from our local crs structure into
        ; the global vm crs that Exec_Int uses
        ;
        mov     esi, [ebp + 8]          ; esi points to local crs

                                        ; get the global vm crs from the
        VMMcall Get_Cur_VM_Handle       ; current vm handle
        mov     ebp, [ebx + CB_Client_Pointer]
                                        ; now ebp points to global crs

        push    ebx                     ; save the vm handle
        mov     ebx, [esi + Client_EBX] ; Copy over the regs
        mov     [ebp + Client_EBX], ebx
        mov     ecx, [esi + Client_ECX]
        mov     [ebp + Client_ECX], ecx
        mov     edx, [esi + Client_EDX]
        mov     [ebp + Client_EDX], edx
        mov     edi, [esi + Client_EDI]
        mov     [ebp + Client_EDI], edi
        mov     ax, [esi + Client_ES]
        mov     [ebp + Client_ES], ax
        mov     eax, [esi + Client_EAX]
        mov     [ebp + Client_EAX], eax
IF 1
        mov     esi, [esi + Client_ESI]
        mov     [ebp + Client_ESI], esi
        pop     ebx                     ; restore the handle

        push ebp    ;store global crs handle

        mov     eax, 010h
        VMMcall Exec_Int

        pop     ebp                     ;retrieve global crs handle
        mov     eax, [ebp + CLIENT_EAX] ;get success/failure code
ELSE
        mov     esi, [esi + Client_ESI]
        mov     [ebp + Client_ESI], esi
        pop     ebx                     ; restore the handle

        mov     eax, 010h
        VMMcall Exec_Int
ENDIF

        VMMcall End_Nest_Exec
        Pop_Client_State


        pop     ebp
;
; Fill register structure from PCI call.
;

        mov     esi, [ebp + 8]          ; now we need to copy data back to our
                                        ;   local crs
        mov     [esi + Client_EAX], eax ; really only eax is needed

;        push    esi
;        mov     esi, [ebp + 8]
;        mov     [esi + Client_EAX], eax
;        mov     [esi + Client_EBX], ebx
;        mov     [esi + Client_ECX], ecx
;        mov     [esi + Client_EDX], edx
;        mov     [esi + Client_EDI], edi
;        pop     eax
;        mov     [esi + Client_ESI], eax

;
; Now we must free the translation buffer we've allocated.
;

        VMMcall Get_Cur_VM_Handle       ; make sure we've got the right vm handle

        pop     esi                     ; address of extended memory buffer, stored at start of routine
        mov     ebp, [ebp + 8]          ; point to our local crs
        mov     ecx, 128                ; free buffer size
;        clc                             ; no copy back required
        push    ss                      ; pointer must be in fs:esi
        pop     fs
        stc                             ; copy back
        VxDcall V86MMGR_Free_Buffer
;
; Restore registers and return.
;
        popad
        pop     ebp
        ret
EndProc _readToshibaEDID




;
; BOOL VBE30(struct Client_Regs *);
;
;   VBE3.0 SetMode with custom refresh table
;
BeginProc _VBE30
        push    ebp
        mov     ebp, esp
;
; Save all registers.
;
        pushad
;
; Copy over the mode table into V86 space.  We need to allocate 60 bytes
; in the local VM translation buffer, copy the table over, and then
; use the returned v86 segment:offset as an absolute offset within
; the VM's real-mode space.
;
        mov     esi, [ebp + 8]
        mov     esi, [esi + Client_EDI] ; get flat offset of the table into esi

        VMMcall Get_Cur_VM_Handle       ; get the current vm handle into ebx

        push    ebp
        mov     ebp, [ebp + 8]          ; get a ptr to the client registers
        mov     ecx, 60                 ; need about 60 bytes
        stc                             ; set carry flag to copy, clear otherwise
        push    ss                      ; pointer must be in fs:esi
        pop     fs
        VxDcall V86MMGR_Allocate_Buffer
        pop     ebp                     ; v86 address in edi 
                                        ; (16 bits of seg, 16 bits of off)
                                        
                                        ; Should probably check cx to see if all
                                        ; data was allocated.  But 60 bytes should
                                        ; always fit in the default 4k translation

        mov     eax, edi                ; Break the v86 address into ax:di
        and     edi, 0FFFFh
        shr     eax, 16

        mov     esi, [ebp + 8]          ; Load it back into our local crs
        mov     [esi + Client_ES], ax
        mov     [esi + Client_EDI], edi

;
; Load register structure for PCI call.  Move the local crs values into the
; real registers.  Don't know if this is still required with the use of the 
; Push_Client_State macros, but it won't hurt for now.
;
        mov     esi, [ebp + 8]
        mov     eax, [esi + Client_EAX]
        mov     ebx, [esi + Client_EBX]
        mov     ecx, [esi + Client_ECX]
        mov     edx, [esi + Client_EDX]
        mov     edi, [esi + Client_EDI]
        mov     esi, [esi + Client_ESI]

;
; Call VBE BIOS.
;
;        push    ebp
;        push    DWORD PTR 010h
;        VMMcall Exec_VxD_Int
;        pop     ebp

        push    ebp                     ; save away ebp so we can get back 
                                        ; to our local crs
        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; 
        ; Move the register values from our local crs structure into
        ; the global vm crs that Exec_Int uses
        ;
        mov     esi, [ebp + 8]          ; esi points to local crs

                                        ; get the global vm crs from the
        VMMcall Get_Cur_VM_Handle       ; current vm handle
        mov     ebp, [ebx + CB_Client_Pointer]
                                        ; now ebp points to global crs

        push    ebx                     ; save the vm handle
        mov     ebx, [esi + Client_EBX] ; Copy over the regs
        mov     [ebp + Client_EBX], ebx
        mov     ecx, [esi + Client_ECX]
        mov     [ebp + Client_ECX], ecx
        mov     edx, [esi + Client_EDX]
        mov     [ebp + Client_EDX], edx
        mov     edi, [esi + Client_EDI]
        mov     [ebp + Client_EDI], edi
        mov     ax, [esi + Client_ES]
        mov     [ebp + Client_ES], ax
        mov     eax, [esi + Client_EAX]
        mov     [ebp + Client_EAX], eax
        mov     esi, [esi + Client_ESI]
        mov     [ebp + Client_ESI], esi
        pop     ebx                     ; restore the handle

        mov     eax, 010h
        VMMcall Exec_Int

        VMMcall End_Nest_Exec
        Pop_Client_State

        pop     ebp
;
; Fill register structure from PCI call.
;

        mov     esi, [ebp + 8]          ; now we need to copy data back to our
                                        ;   local crs
        mov     [esi + Client_EAX], eax ; really only eax is needed

;        push    esi
;        mov     esi, [ebp + 8]
;        mov     [esi + Client_EAX], eax
;        mov     [esi + Client_EBX], ebx
;        mov     [esi + Client_ECX], ecx
;        mov     [esi + Client_EDX], edx
;        mov     [esi + Client_EDI], edi
;        pop     eax
;        mov     [esi + Client_ESI], eax

;
; Now we must free the translation buffer we've allocated.
;

        VMMcall Get_Cur_VM_Handle       ; make sure we've got the right vm handle

        mov     ebp, [ebp + 8]          ; point to our local crs
        mov     ecx, 60                 ; free 60 bytes
        clc                             ; no copy back required
        VxDcall V86MMGR_Free_Buffer

;
; Restore registers and return.
;
        popad
        pop     ebp
        ret
EndProc _VBE30

;---------------------------------------------------------------------------
;
;  Read an IO port.
;
;---------------------------------------------------------------------------
BeginProc inp_
        push    edx
        mov     dx,  ax
        xor     eax, eax
        in      al, dx
        pop     edx
        ret
EndProc inp_
;---------------------------------------------------------------------------
;
;  Write an IO port.
;
;---------------------------------------------------------------------------
BeginProc outp_
        xchg    eax, edx
        out     dx, al
        ret
EndProc outp_

;IFDEF VPE_EXTRAS

; John Tobin : This was put here because it would not work compile
; in a 'C' environment...this was easier


public _vwin32CloseVxdHandle
public _vwin32SetWin32Event

BeginProc _vwin32SetWin32Event, CCALL, PUBLIC

ArgVar hEvent, DWORD

        EnterProc

        pushfd
        push    ecx
        push    edx
        mov     eax, [hEvent]

        VxDCall _VWIN32_SetWin32Event

        pop     edx
        pop     ecx
        popfd

        LeaveProc
        return 

EndProc _vwin32SetWin32Event

BeginProc _vwin32CloseVxdHandle, CCALL, PUBLIC

ArgVar hEvent, DWORD

        EnterProc

        push    ecx
        push    edx
        mov     eax, [hEvent]

        VxDCall _VWIN32_CloseVxDHandle

        pop     edx
        pop     ecx

        LeaveProc
        return

EndProc _vwin32CloseVxdHandle


;ENDIF

public  _DotN_help
_DotN_help db '.T - NVIDIA TnT Resource Manager Soft-Ice/W Extension handler.',0ah,0dh,00

EXTRN   _DotN_cmd_c:NEAR
public  _DotN_cmd_asm
BeginProc _DotN_cmd_asm
 
        push    ebp
        mov     ebp, esp
;
; Save all registers.
;
        pushad
        
        call _DotN_cmd_c
        
        popad
        pop     ebp
        retf
        
EndProc _DotN_cmd_asm

VxD_LOCKED_CODE_ENDS

VxD_REAL_INIT_SEG
;---------------------------------------------------------------------------;
;                                                                           ;
;       NVRM_Real_Init_Proc                                                 ;
;                                                                           ;
;   DESCRIPTION:                                                            ;
;       This code is called when the system is still in real mode, and      ;
;       the VxDs are being loaded.                                          ;
;                                                                           ;
;   PARAMETERS:                                                             ;
;       AX = VMM Version                                                    ;
;       BX = Flags                                                          ;
;               Bit 0: duplicate device ID already loaded                   ;
;               Bit 1: duplicate ID was from the INT 2F device list         ;
;               Bit 2: this device is from the INT 2F device list           ;
;       EDX = Reference data from INT 2F response, or 0                     ;
;       SI = Environment segment, passed from MS-DOS                        ;
;                                                                           ;
;   RETURN VALUES:                                                          ;
;       BX = ptr to list of pages to exclude (0, if none)                   ;
;       SI = ptr to list of instance data items (0, if none)                ;
;       EDX = DWORD of reference data to be passed to protect mode init     ;
;                                                                           ;
;---------------------------------------------------------------------------;
BeginProc NVRM_Real_Init_Proc
        test    bx, Duplicate_Device_ID ; check for already loaded NVRM
        jnz     short @F                ; jump if so
        xor     bx, bx                  ; no exclusion table
        xor     si, si                  ; no instance data table
        xor     edx, edx                ; no reference data
        mov     ax, Device_Load_Ok
        ret
@@:     mov     ax, Abort_Device_Load + No_Fail_Message
        ret
EndProc NVRM_Real_Init_Proc

VxD_REAL_INIT_ENDS

        END

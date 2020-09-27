	TITLE   copyport.ASM


; copyport.asm - Copy DWORD data from an input port and to an output port.


?PLM=1          ; PASCAL Calling convention is DEFAULT
?WIN=0          ; Windows calling convention
?386regs=1 

.xlist
include cmacros.new
.list


;---------------------------------------;
;       Segment Declarations            ;
;---------------------------------------;

ifndef  SEGNAME
	SEGNAME equ <_TEXT32>
endif

createSeg %SEGNAME, CodeSeg, dword, public, CODE

sBegin  CodeSeg
	assumes cs,CodeSeg
	assumes ds,Data
	assumes es,nothing


	ALIGN 2

.486





;-----------------------------------------------------------------------;
; void FAR PASCAL CopyFromInPort (WORD pollport, WORD dataport,
;                                 LPSTR p, WORD dsize);

cProc   CopyFromInPort,<FAR, PASCAL, PUBLIC>,< edi >
	parmW  pollport
	parmW  dataport
	parmD  p          ; destination
	parmW  dsize      ; number of DWORDs to copy

cBegin
		; Load destination pointer.
		cld
		xor     edi, edi
		les     di, p

		; Calculate number of 16 byte blocks.
		mov     bx, dsize
		shr     bx, 2
		jz      noRead4   ; if less than 16 bytes

		; Copy data in portions of 4 DWORDs.
		xor     ecx, ecx
loopRead4:
		mov     dx, pollport
		mov     cx, 4
pollRead4:
		in      al, dx
		and     al, 16
		jz      pollRead4
		mov     dx, dataport
		rep insd es:[edi], dx
		dec     bx
		jnz     loopRead4

		; How many DWORDs remain ?
noRead4:
		mov     bx, dsize
		and     bx, 3
		jz      endRead4   ; if no remaining DWORDs

		; Copy DWORD data.
loopRead:
		mov     dx, pollport
pollRead:
		in      al, dx
		and     al, 32
		jnz     pollRead
		mov     dx, dataport
		insd    es:[edi], dx
		dec     bx
		jnz     loopRead

endRead4:

		cEnd CopyFromInPort





;-----------------------------------------------------------------------;
; void FAR PASCAL CopyToOutPort (WORD pollport, WORD dataport,
;                                LPSTR p, WORD dsize, WORD rest);

cProc   CopyToOutPort,<FAR, PASCAL, PUBLIC>,< esi >
	parmW  pollport
	parmW  dataport
	parmD  p          ; source
	parmW  dsize      ; number of DWORDs to copy
	parmW  rest

cBegin
		push    ds

		; Load source pointer.
		cld
		xor     esi, esi
		lds     si, p

		; Calculate number of 16 byte blocks.
		mov     bx, dsize
		shr     bx, 2
		jz      noWrite4   ; if less than 16 bytes

		; Copy data in portions of 4 DWORDs.
		xor     ecx, ecx
loopWrite4:
		mov     dx, pollport
		mov     cx, 4
pollWrite4:
		in      al, dx
		and     al, 2
		jz      pollWrite4
		mov     dx, dataport
		rep outsd dx, ds:[esi]
		dec     bx
		jnz     loopWrite4

		; How many DWORDs remain ?
noWrite4:
		mov     bx, dsize
		and     bx, 3
		jz      endWrite4   ; if no remaining DWORDs

		; Copy DWORD data.
loopWrite:
		mov     dx, pollport
pollWrite:
		in      al, dx
		and     al, 2
		jz      pollWrite
		mov     dx, dataport
		outsd   dx, ds:[esi]
		dec     bx
		jnz     loopWrite

endWrite4:
		mov     bx, rest
		or      bx, bx
		jz      endWrite5
loopWrite5:
		lodsb   ds:[esi]
		mov     cl, 8
		ror     eax, cl
		dec     bx
		jnz     loopWrite5

		mov     bx, 4
		sub     bx, rest
		add     bx, bx
		add     bx, bx
		add     bx, bx
		mov     cl, bl
		ror     eax, cl

		mov     dx, pollport
		push    eax
pollWrite5:
		in      al, dx
		and     al, 2
		jz      pollWrite5
		pop     eax
		mov     dx, dataport
		out     dx, eax

endWrite5:
		pop     ds

		cEnd CopyToOutPort





;-----------------------------------------------------------------------;
; void FAR PASCAL CopyBytesFromInPort (WORD dataport, LPSTR p, DWORD bytesize);

cProc   CopyBytesFromInPort,<FAR, PASCAL, PUBLIC>,< edi >
	parmW  dataport
	parmD  p          ; destination
	parmD  bytesize   ; number of bytes to copy

cBegin
		push    es

		; Load source pointer.
		cld
		xor     edi, edi
		les     di, p
		mov     dx, dataport

		; Copy whole words.
		mov     ecx, bytesize
		shr     ecx, 1
		jz      noRead2   ; if less than 2 bytes
		rep  insw es:[edi], dx

		; How many bytes remain ?
noRead2:
		mov     ecx, bytesize
		and     cx, 1
		jz      endRead2   ; if no remaining bytes

		; Copy one additional byte.
		in      ax, dx
		stosb   es:[edi]   ; store al

endRead2:
		pop     es

		cEnd CopyBytesFromInPort





;-----------------------------------------------------------------------;
; void FAR PASCAL CopyBytesToOutPort (WORD dataport, LPSTR p, DWORD bytesize);

cProc   CopyBytesToOutPort,<FAR, PASCAL, PUBLIC>,< esi >
	parmW  dataport
	parmD  p          ; source
	parmD  bytesize   ; number of bytes to copy

cBegin
		push    ds

		; Load source pointer.
		cld
		xor     esi, esi
		lds     si, p
		mov     dx, dataport

		; Copy whole words.
		mov     ecx, bytesize
		shr     ecx, 1
		jz      noWrite2   ; if less than 2 bytes
		rep  outsw dx, ds:[esi]

		; How many bytes remain ?
noWrite2:
		mov     ecx, bytesize
		and     cx, 1
		jz      endWrite2   ; if no remaining bytes

		; Copy one additional byte.
		lodsb   ds:[esi]   ; loaded to al
		xor     ah, ah
		out     dx, ax

endWrite2:
		pop     ds

		cEnd CopyBytesToOutPort





sEnd CodeSeg

end

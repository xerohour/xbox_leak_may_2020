	P586
	;.model large
	public  _get_tsc
	public	_get_tick
	public	_time_gold_system
	public	_clocks_per_rtc_update
test	SEGMENT USE16
	ASSUME  CS:test


rdtsc	macro
	db	0Fh
	db	31h
	endm


wait_till_uip macro
		local	W1, W2
		MOV 	AL,8AH
		OUT 	70H,AL
		DAA
		DAA
W1:		IN		AL,71H			; Wait for the uip to be low
		AND		AL,80H
		JNZ		W1
		IN		AL,71H			; Wait for the uip to be low
		AND		AL,80H
		JNZ		W1

		MOV 	AL,8AH
		OUT 	70H,AL
		DAA
		DAA
W2:    	IN		AL,71H			; Wait for the uip to be high
		AND		AL,80H
		JZ		W2
    	IN		AL,71H			; Wait for the uip to be high
		AND		AL,80H
		JZ		W2
        endm


wait_till_change macro
		local	wc
		;mov		dx,379H
		mov		dx,3FEH		;new !!!
		in		al,dx
		mov		ah,al
wc:     in		al,dx
		xor		al,ah
		and		al,10h		;new !!!
		jz		wc			;new !!!
		;jns		wc
		endm


wait_rtc_second macro
		local	W1
		MOV 	AL,0H
		OUT 	70H,AL
		DAA
		DAA
		IN		AL,71H
		MOV		AH,AL
W1:    	IN		AL,71H
		CMP		AL,AH
		JZ		W1
		endm



_get_tick	proc far
	push ds
	mov bx,0
	mov ds,bx
	mov bx,46CH
	mov ax,[bx]
	pop ds
	ret
	endp


_get_tsc	proc far
	rdtsc
	mov dx,ax
	ror eax,16
	xchg dx,ax
	ret
	endp


_clocks_per_rtc_update	proc C far
	arg howmany:word
	cli
	mov cx,howmany
	wait_till_uip
	rdtsc
over:
	mov	edi,eax
	mov	esi,edx
	wait_till_uip
	loop over
	rdtsc
	SUB     EAX, EDI
	SBB		EDX, ESI
	STI                     ; Start Interrupts
	MOV     ECX, EAX        ; Preserve the low DWORD of CPU counts in ECX
	MOV     AX, CX          ; Load the lowest 16 bits of the CPU clock time into AX
	SHR     ECX, 16
	MOV     DX, CX          ; Load the next 16 bits of the CPU clock time into DX
    ret
	endp



_time_gold_system	proc far
        PUSH    EBX
        PUSH    ECX
        PUSH    EDI
        PUSH    ESI
        PUSH    ES
        CLI                     ;  stop interrupts
		wait_till_change
		rdtsc                   ; Read the time-stamp counter
        MOV     EDI, EAX        ; Preserve the Least Significant 32 bits
        MOV     ESI, EDX        ; Preserve the Most Significant 32 bits
		wait_till_change
		rdtsc                   ; Read the time-stamp counter
		SUB     EAX, EDI
		SBB		EDX, ESI
		STI                     ; Start Interrupts
		MOV     ECX, EAX        ; Preserve the low DWORD of CPU counts in ECX
		MOV     AX, CX          ; Load the lowest 16 bits of the CPU clock time into AX
		SHR     ECX, 16
		MOV     DX, CX          ; Load the next 16 bits of the CPU clock time into DX
		POP     ES
        POP     ESI
        POP     EDI
        POP     ECX
        POP     EBX
		RET                     ; Return with result in AX
        endp

test	ends
	end




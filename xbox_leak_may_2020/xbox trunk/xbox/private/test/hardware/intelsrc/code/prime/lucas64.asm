
	TITLE   setup

	.386

_TEXT32 SEGMENT PARA USE32 PUBLIC 'DATA'

IFDEF WIN31
_MAXERR		DQ	0.0		; Maximum error
_SUMINP		DQ	0.0		; Sum of inputs
_SUMOUT		DQ	0.0		; Sum of outputs
_ERRCHK		DD	0		; Is error checking enabled?
_PARG		DD	0		; Prime exponent being tested

ELSE
EXTRN	_MAXERR:QWORD
EXTRN	_SUMINP:QWORD
EXTRN	_SUMOUT:QWORD
EXTRN	_ERRCHK:DWORD
EXTRN	_PARG:DWORD
ENDIF

;
; Global variables needed in squaring
;

        align   8
HALF		DD	0.5		; One-half
MINUS2		DD	-2.0		; Used to subtract 2.
BIGVAL		DD	0.0		; Used to round to an integer
scaled_numbig	DD	0		; numbig * (2^32 / n)
scaled_numlit	DD	0		; numlit * (2^32 / n)
		; These values only used during setup
p		DD	0		; Mersenne prime being tested
numlit		DD	0		; number of small words in the fft
numbig		DD	0		; number of big words in the fft
_TEXT32	ENDS


	ASSUME  CS: _TEXT32, DS: _TEXT32, SS: _TEXT32, ES: _TEXT32

n	EQU	65536		; size of the FFT

INCLUDE common.mac

;
; FFT.CPP should fully describe the memory layout.
; In pass1:
;	The distance between each data element is 8 bytes.
;	The distance between every fourth element is 256 bytes.
;	The distance between every 64th element is 4K bytes + gap.
; In pass2:
;	The distance between each data element is 32 bytes.
;	The distance between every eighth element is 32K bytes + gaps.
;	The distance between every 128th element is 4K bytes + gap.
;

	assign_addrs 0, 3072, 1536, 0, 179520

pass1_distances MACRO
	dist1 = 8
	dist4 =	256			*INTERLEAVE
	dist64 = (4096+32)		*INTERLEAVE
	dist512 = 32			*INTERLEAVE
	dist4096 = (32768+4096+8*32)	*INTERLEAVE
	ENDM
pass2_distances MACRO
	dist1 = 32			*INTERLEAVE
	dist8 = (32768+4096+8*32)	*INTERLEAVE
	dist128 = 8
	dist512 = 256			*INTERLEAVE
	dist8192 = (4096+32)		*INTERLEAVE
	ENDM

_TEXT32	SEGMENT 

; Setup routine

	PUBLIC	_setup64
	IFDEF WIN31
	ORG	0400h
_setup64 PROC FAR
	push	es			; USE32 routines must preserve es,bp
	push	ebp
	push	ds			; copy ds to es
	pop	es
	mov	_PARG, ecx
	ELSE
_setup64 PROC NEAR
	push	ebp
	push	ebx
	push	edi
	push	esi
	ENDIF

; Initialize

	finit				; Init FPU

; Save p

	mov	eax, _PARG		; p
	mov	p, eax

; Compute two-to-phi and two-to-minus-phi multipliers

	normalize_setup

; Compute the array of sine and cosine/sine values

	sine_cosine_1_setup
	sine_cosine_2_setup
	pre_multiplier_setup pre_mults2, n, 128

; Return

	IFDEF WIN31
	pop	ebp
	pop	es
	ELSE
	pop	esi
	pop	edi
	pop	ebx
	pop	ebp
	ENDIF
	ret
_setup64 ENDP

;
; Compute next number in lucas sequence.  i.e.
; square the input number mod 2**p-1, subtract 2
;

	PUBLIC	_lucas64
	IFDEF WIN31
	ORG	1000h
_lucas64 PROC FAR
	push	es			; USE32 routines must preserve es,bp
	push	ebp
	push	ds			; copy ds to es
	pop	es
	mov	_ERRCHK, ecx		; ERRCHK is passed in ecx
	ELSE
_lucas64 PROC NEAR
	push	ebp
	push	ebx
	push	edi
	push	esi
	ENDIF

; Init the FPU every iteration just to be safe

	finit

; Start the fft process, do pass1 fft
; 128 sections of 512 values
; Perform nine levels of the FFT on 512 real values.

	pass1_distances
	mov	esi, input		; U - Starting address
	mov	edx, 16*256+8		; V - 16 iterations of 8 iterations
b0a:	nine_levels_fft
	dec	dl			; V - Test inner loop counter
	JNZ_X	b0a			;*V - Iterate if necessary
	lea	esi, [esi-8*dist512+dist4096]; U - Next source pointer
	sub	edx, 256		; V - Test outer loop counter
	mov	dl, 8			; U - Reload inner loop counter
	JNZ_X	b0a			; V - Iterate if necessary

; Do the first section of pass 2

	pass2_distances
	seven_levels_real_fft_sq_unfft

; Do the other 255 sections of pass 2 

	mov	esi, input+2*dist128	; U - Restore source pointer
	mov	ebp, pre_mults2		; V - Addr of group pre-multipliers
	mov	edx, 8*256+16+80000000h	; U - 8 iters of 16 iters of 1 or 2
b1b:	seven_levels_fft_sq_unfft
	xor	edx, 80000000h		; U - Test inner loop counter
	JS_X	b1b			; V - Iterate if necessary
	lea	esi, [esi-4*dist128+dist512]; U - Next source pointer
	dec	dl			; V - Test outer loop counter
	JNZ_X	b1b			;*V - Iterate if necessary
	lea	esi, [esi-16*dist512+dist8192]; U - Next source pointer
	sub	edx, 256		; V - Test outer loop counter
	mov	dl, 16			; U - Reload inner loop counter
	JNZ_X	b1b			; V - Iterate if necessary

; Finish the inverse fft process, do pass1 inverse fft

	pass1_distances
	mov	esi, input		; U - Source pointer
	mov	edx, 16*256+8		; V - 16 iterations of 8 iterations
b2a:	nine_levels_unfft
	dec	dl			; V - Test inner loop counter
	JNZ_X	b2a			;*V - Iterate if necessary
	lea	esi, [esi-8*dist512+dist4096]; U - Next source pointer
	sub	edx, 256		; V - Test outer loop counter
	mov	dl, 8			; U - Reload inner loop counter
	JNZ_X	b2a			; V - Iterate if necessary

; Error check - find largest round off error

	cmp	_ERRCHK, 0
	JZ_X	noechk

; Code to check for how close we are to incurring a round-off error

	pass2_distances
	fldz				; Maximum error
	mov	edx, norm_grp_mults	; Address of the group multipliers
	mov	esi, input		; Address of squared number
	mov	edi, scaled_numlit	; Used to compute big vs little words
	mov	ebp, scaled_numbig
	mov	eax, -1			; First word is a big word (-1)
	mov	ebx, 8*256+16		; 8 iterations of 16 iterations of
elp4:	mov	save_counter, ebx
	mov	ecx, 4*256		; 4 iterations of
elp3:	mov	cl, 16			; 128 / 8 iterations
	mov	ebx, norm_col_mults	; Restart the column multipliers
	fld	QWORD PTR [edx+norm_gap]; Two-to-minus-phi group multiplier
	fld	QWORD PTR [edx+norm_gap]; Two-to-minus-phi
elp2:	error_check			; Error check eight values
	lea	esi, [esi+dist8]	; Next source
	lea	ebx, [ebx+4*norm_gap]	; Next set of 8 column multipliers
	dec	cl
	JNZ_X	elp2			; Loop til done
	lea	esi, [esi-16*dist8+dist128]; Next source
	lea	edx, [edx+8]		; Next group multiplier
	fcompp				; Pop group multipliers
	sub	ecx, 256
	JNZ_X	elp3			; Loop til done
	lea	esi, [esi-4*dist128+dist512]; Next source
	lea	edx, [edx-4*8+big_norm_gap]; Next group multiplier
	mov	ebx, save_counter
	dec	bl
	JNZ_X	elp4			; Loop til done
	lea	esi, [esi-16*dist512+dist8192]; Next source
	sub	ebx, 256
	mov	bl, 16
	JNZ_X	elp4			; Loop til done
	fstp	_MAXERR			; Store maximum error

; Normalize the results

noechk:	pass2_distances
	fldz				; Init SUMOUT
	fld	MINUS2			; Start normalization process
	fadd	BIGVAL			; with a BIGVAL-2.0 carry!
	mov	edx, norm_grp_mults	; Address of the group multipliers
	mov	esi, input		; Address of squared number
	mov	edi, scaled_numlit	; Used to compute big vs little words
	mov	ebp, scaled_numbig
	mov	eax, -1			; First word is a big word (-1)
	mov	ebx, 8*256+16		; 8 iterations of 16 iterations of
nlp4:	mov	save_counter, ebx
	mov	ecx, 4*256		; 4 iterations of
nlp3:	mov	cl, 16			; 128 / 8 iterations
	mov	ebx, norm_col_mults	; Restart the column multipliers
	fld	QWORD PTR [edx]		; Two-to-phi
	fld	QWORD PTR [edx+norm_gap]; Two-to-minus-phi group multiplier
	fld	QWORD PTR [edx]		; Two-to-phi group multiplier
	fld	QWORD PTR [edx+norm_gap]; Two-to-minus-phi
nlp2:	normalize			; Normalize 8 values
	lea	esi, [esi+dist8]	; Next source
	lea	ebx, [ebx+4*norm_gap]	; Next set of 8 column multipliers
	dec	cl
	JNZ_X	nlp2			; Loop til done
	lea	esi, [esi-16*dist8+dist128]; Next source
	lea	edx, [edx+8]		; Next group multiplier
	fcompp				; Pop group multipliers
	fcompp				; Pop group multipliers
	sub	ecx, 256
	JNZ_X	nlp3			; Loop til done
	lea	esi, [esi-4*dist128+dist512]; Next source
	lea	edx, [edx-4*8+big_norm_gap]; Next group multiplier
	mov	ebx, save_counter
	dec	bl
	JNZ_X	nlp4			; Loop til done
	lea	esi, [esi-16*dist512+dist8192]; Next source
	sub	ebx, 256
	mov	bl, 16
	JNZ_X	nlp4			; Loop til done

					; Add any carry to first values
	mov	esi, input		; Address of squared number
	mov	edi, scaled_numlit	; To compute big vs little words
	mov	ebx, norm_col_mults	; Restart the column multipliers
	normalize_012

; Normalize SUMOUT value by multiplying by 1 / (n/2).
; This is the value of the first two-to-minus-phi group multiplier

	fmul	QWORD PTR ds:(norm_grp_mults+norm_gap)
	fstp	_SUMOUT

; Return

exit:	IFDEF WIN31
	fld	_SUMINP			;; Return values in floating
	fld	_SUMOUT			;; point registers
	fld	_MAXERR
	pop	ebp
	pop	es
	ELSE
	pop	esi
	pop	edi
	pop	ebx
	pop	ebp
	ENDIF
	ret

_lucas64 ENDP

_TEXT32	ENDS
END

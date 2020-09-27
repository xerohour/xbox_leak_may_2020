		.686
		.xmm
		.model flat,c

		.data

		.code
xmmtest		PROC C PUBLIC 
		xorps xmm0, xmm0
		ret
xmmtest		ENDP

xmm128INT	PROC C PUBLIC USES EAX EDI, datai:DWORD, iresultp:DWORD
		mov	eax,datai
		movups	xmm0,[eax]
		movaps	xmm1, xmm0
		movaps	xmm2, xmm1
		movaps	xmm3, xmm2
		movaps	xmm4, xmm3
		movaps	xmm5, xmm4
		movaps	xmm6, xmm5
		movaps	xmm7, xmm6
		shufps	xmm7, xmm7, 1bh	;shuffle a3,a2,a1,a0 to a0,a1,a2,a3
		shufps	xmm7, xmm7, 1bh	;shuffle a3,a2,a1,a0 to a0,a1,a2,a3
		mov	edi,iresultp
		movups	[edi],xmm7
		ret
xmm128INT	ENDP

xmm128FLT	PROC C PUBLIC USES EAX EDI, dataf1:DWORD, dataf2:DWORD, fresultp:DWORD
		mov	eax, dataf1		;load 2 registers with 128 bit floats
		movups	xmm0, [eax]
		mov	eax, dataf2
		movups	xmm1, [eax]		
		addps	xmm0, xmm1		;add xmm0 + xmm1 store in xmm0
		sqrtps	xmm2, xmm0		;sqrt xmm0 store in xmm2
		minps	xmm1, xmm2		;take the min of xmm1 and xmm2, store in xmm1
		mov	edi,  fresultp		;point to result structure
		movups	[edi],xmm1		;xfr result to structure
		ret
xmm128FLT	ENDP


xmm128CMP	PROC C PUBLIC USES EAX EDI, dataf1:DWORD, dataf2:DWORD, fresultp:DWORD
		mov	eax, dataf1
		movups	xmm0, [eax]
		mov	eax, dataf2
		movups	xmm1, [eax]
		maxps	xmm1, xmm0	;find max and store in xmm1
		movups	xmm2, [eax]
		cmpeqps	xmm1, xmm2	;which value changed?  should be second value
		andps	xmm0, xmm1	;lose the second value in xmm0
		subps	xmm0, xmm2	;
		mov	edi,  fresultp	;point to result structure
		movups	[edi],xmm0	;xfr result to structure				
		ret
xmm128CMP	ENDP

xmm128SHF	PROC C PUBLIC USES EAX EDI, fresultp:DWORD
		mov		eax, 5
		cvtsi2ss	xmm0, eax		;load 5 into lsd
		shufps		xmm0, xmm0, 93h		;rotate by 32 bits 5 in 2 of 4
		mov		eax, 6
		cvtsi2ss	xmm0, eax		;load 6 into lsd
		shufps		xmm0, xmm0, 93h		;rotate by 32 bits 5 in 3 of 4
		mov		eax, 7
		cvtsi2ss	xmm0, eax		;load 7	
		shufps		xmm0, xmm0, 93h		;rotate by 32 bits 5 in 4 of 4
		mov		eax, 8
		cvtsi2ss	xmm0, eax		;load 8   xmm0 = 5678
		movaps		xmm1, xmm0
		shufps		xmm1, xmm1, 1bh		;rotate	  xmm1 = 8765
		mulps		xmm1, xmm0		;ans  40,42,42,40
		mov		eax, 2
		cvtsi2ss	xmm0, eax		;load 2
		divss		xmm1, xmm0		;divide by 2 scalar

		mov		edi,  fresultp		;point to result structure
		movups		[edi],xmm1		;xfr result to structure
		ret
xmm128SHF	ENDP

		END

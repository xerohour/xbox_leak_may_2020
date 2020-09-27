	P586

test	SEGMENT USE16
	ASSUME  CS:test
	
	public	_rdmsr
	public	_wrmsr
	public	_shift_left_qword
	public	_shift_right_qword
	public	_get_psp_command_tail


_rdmsr	proc	C far
	arg	address:dword, where:dword
	push	ds
	pusha
	mov	ecx,address
	rdmsr
	lds	bx,where
	mov	[bx+4],edx
	mov	[bx],eax
	popa
	pop	ds
	ret
	endp


_wrmsr	proc	C far
	arg	address:dword, mask:dword, data:dword
	push	ds
	pusha
	mov	ecx,address
	cli
	wbinvd			;must write back dirty lines in case wrmsr alters any cache controls!!!
	jmp	next
next:	nop
	rdmsr			;do wrmsr as a read-modify-write operation
	lds	bx,mask
	and	edx,[bx+4]
	and	eax,[bx]
	lds	bx,data
	or	edx,[bx+4]
	or	eax,[bx]
	mov	[bx+4],edx	;return what we wrote in data arg
	mov	[bx],eax
	wrmsr
	popa
	pop	ds
	ret
	endp
	

_shift_left_qword proc	C far
	arg	address:dword, bit:word
	push	ds
	pusha
	lds	bx,address
	ror	byte ptr[bit],1
	rcl	dword ptr[bx],1
	rcl	dword ptr[bx+4],1
	popa
	pop	ds
	ret
	endp

	
_shift_right_qword proc	C far
	arg	address:dword, bit:word
	push	ds
	pusha
	lds	bx,address
	ror	byte ptr[bit],1
	rcr	dword ptr[bx+4],1
	rcr	dword ptr[bx],1
	popa
	pop	ds
	ret
	endp
	
	
_get_psp_command_tail proc C far
	push	bx
	push	ds
	mov	ah,51h
	int	21h
	mov	ds,bx
	mov	bx,81h
	add	bl,[bx-1]
	adc	bh,0
	mov	byte ptr[bx],0
	mov	dx,ds
	mov	ax,82h
	pop	ds
	pop	bx
	ret
	endp	


test	ends
	end




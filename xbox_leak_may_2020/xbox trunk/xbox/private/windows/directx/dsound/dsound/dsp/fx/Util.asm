;*******************************************************************
;
;       Util.asm    DSP utility routines  
;
;******************************************************************        
	
;************************************************************
; GPInit	Restore GP state to initial state, including
;		clearing XRAM and YRAM
;		
;************************************************************
GPInit
	GPClearInterruptRegister
        
        move #>0,r0
        move #>kGPMemory_Size_XRAM,x0

        jsr GPClearXRAM_All
        jsr GPClearYRAM_All	

 	rts
; ---- end GPInit----
	
;************************************************************
; GPWaitForStartFrame	Poll and wait for Start Frame (bit 1).
;			Afterward, clear the bit
;************************************************************
GPWaitForStartFrame 
	jclr 	#kStartFrameBit,x:INTERRUPT_REGISTER,GPWaitForStartFrame
; Clear start frame Bit 1
	movep    #>$2,x:INTERRUPT_REGISTER	; "Write-1-To-Clear-One" 
 	rts
; ---- end GPWaitForStartFrame ----


;************************************************************
; GPClearXRAM_All	Clear entire XRAM
; r0 has base address
; x0 has size
;************************************************************
GPClearXRAM_All
	clr	a
	rep	x0
	move	a,x:(r0)+
 	rts
; ---- end GPClearXRAM_All----

;************************************************************
; GPClearYRAM_All		Clear entire YRAM
;************************************************************
GPClearYRAM_All 
	move #>0,r0
	move #>kGPMemory_Size_YRAM,x0
	clr	a
	rep	x0
	move	a,y:(r0)+
 	rts
; ---- end GPClearYRAM_All ----



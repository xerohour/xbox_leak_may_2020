;	File	:	RvbFuncs.asm
;	Version	:	0.22
;	Date	:	20/09/2000
;
;	Author	:	RDA
;				Sensaura Ltd
;
;
;
; Description
; ===========
; Reverb support functions
;
; Version history
; ===============
;	0.1		All functions working and tested
;	0.2		Moved the function 'CopyFrame' from 'reverb.asm'
;	0.21	Added function 'ShiftLeftFrame_InPlace'
;	0.22	Added function 'AccumulateFrame_OutOfPlace'


	org	p:


;=========================
;	CopyFrame
;=========================
;	On Entry
;		r0=pointer to source
;		r1=poitner to destination
;	On Exit
;		r0	increased by SizeOfFrame
;		r1	increased by SizeOfFrame
;
;
;	Copy the source frame to the destination frame
;			r1[n]=r0[n]				0 <= n < SizeOfFrame
;
CopyFrame
	dor	#SizeOfFrame,RVB_CopyLoop
		move	x:(r0)+,x0							; Load from source frame
		move	x0,x:(r1)+							; Save in destination frame
RVB_CopyLoop
	rts



;===================================
;		ScaleFrame_InPlace
;===================================
;	On Entry
;		x0	Scaling factor
;		r0	Pointer to frame
;	On Exit
;		a	Undefined
;		x0	Preserved
;		x1	Undefined
;		r0	increased by SizeOfFrame
;		Frame pointed to by r0 on entry is scaled by x0
;
;			r0[n]=x0*r0[n]				0 <= n < SizeOfFrame

ScaleFrame_InPlace
	dor		#SizeOfFrame,RVB_ScaleLoop
		move	x:(r0),x1							; Load from source frame
		mpy		x1,x0,a								; Scale element
		move	a,x:(r0)+							; Store result back in source frame
RVB_ScaleLoop
	rts



;===========================================
;		MultiplyAccumulateFrame_InPlace
;===========================================
;	On Entry
;		x0	Scaling factor
;		r0	Pointer to frame 1
;		r1	Pointer to frame 2
;
;	On Exit
;		a	Undefined
;		x0	Preserved
;		x1	Undefined
;		r0	increased by SizeOfFrame
;		r1	increased by SizeOfFrame
;
;		Frame pointed to by r1 on entry is updated
;			r1[n]=r1[n]+x0*r0[n]				0 <= n < SizeOfFrame
;	
MultiplyAccumulateFrame_InPlace
	dor		#SizeOfFrame,RVB_MACLoop
		move	x:(r0)+,x1							; Load element from source frame 0
		move	x:(r1),a							; Load element from source frame 1
		macr	x1,x0,a								; Scale frame 0 and add it to frame 1
		move	a,x:(r1)+							; Store the result in frame 1
RVB_MACLoop
	rts




;===========================================
;		AccumulateFrame_InPlace
;===========================================
;	On Entry
;		r0	Pointer to frame
;		r1	Pointer to frame
;	On Exit
;		a	Undefined
;		x0	Preserved
;		r0	increased by SizeOfFrame
;		r1	increased by SizeOfFrame
;		Frame pointed to by r1 on entry is updated
;			r1[n]=r1[n]+r0[n]				0 <= n < SizeOfFrame
;	
AccumulateFrame_InPlace
	dor		#SizeOfFrame,RVB_AccumulateLoop
		move	x:(r0)+,x0							; Load element from frame 0
		move	x:(r1),a							; Load element from frame 1
		add		x0,a								; Add elements together
		move	a,x:(r1)+							; Store result in frame 1
RVB_AccumulateLoop
	rts




;===========================================
;		AccumulateFrame_OutOfPlace
;===========================================
;	On Entry
;		r0	Pointer to input frame 0
;		r1	Pointer to input frame 1
;		r2	Pointer to output frame
;	On Exit
;		a	Undefined
;		x0	Preserved
;		r0	increased by SizeOfFrame
;		r1	increased by SizeOfFrame
;		r2	increased by SizeOfFrame
;
;		Frame pointed to by r2 on entry is updated as follows
;			r2[n]=r1[n]+r0[n]				0 <= n < SizeOfFrame
;	
AccumulateFrame_OutOfPlace
	dor		#SizeOfFrame,RVB_AccumulateOOPLoop
		move	x:(r0)+,x0							; Load element from frame 0
		move	x:(r1)+,a							; Load element from frame 1
		add		x0,a								; Add elements together
		move	a,x:(r2)+							; Store result in frame 2
RVB_AccumulateOOPLoop
	rts




;===================================
;		ShiftLeftFrame_InPlace
;===================================
;	On Entry
;		x0	Number of places to shift frame
;		r0	Pointer to frame
;	On Exit
;		a	Undefined
;		x0	Preserved
;		r0	increased by SizeOfFrame (i.e. pointer to byte after end of input frame)
;		Frame pointed to by r0 on entry is multiplied by by 2^(x0)
;
;			r0[n]=r0[n] << x0				0 <= n < SizeOfFrame

ShiftLeftFrame_InPlace
	dor		#SizeOfFrame,RVB_ShiftLeftLoop
		move	x:(r0),a							; Load element from frame 0
		asl		x0,a,a								; Shift element left 'x0' places
		move	a,x:(r0)+							; Store result in frame 0
RVB_ShiftLeftLoop
	rts



;===========================================
;		MultiplyAccumulateShiftLeftFrame_InPlace
;===========================================
;	On Entry
;		x0	Scaling factor
;		x1	Bits to shift left
;		r0	Pointer to frame 1
;		r1	Pointer to frame 2
;
;	On Exit
;		a	Undefined
;		x0	Preserved
;		x1	Preserved
;		y1	Undefined
;		r0	increased by SizeOfFrame
;		r1	increased by SizeOfFrame
;
;		Frame2 pointed to by r1 on entry is updated
;			r1[n]=r1[n]+((x0*r0[n])<<x1)			0 <= n < SizeOfFrame
;	
MultiplyAccumulateShiftLeftFrame_InPlace
	dor		#SizeOfFrame,RVB_MACSLLoop
		move	x:(r0)+,y1							; Load element from frame 0
		move	x:(r1),a							; Load element from frame 1
		macr	y1,x0,a								; Scale frame 0 and add to frame 1
		asl		x1,a,a								; Shift result left 'x1' places
		move	a,x:(r1)+							; Store result in frame 1
RVB_MACSLLoop
	rts




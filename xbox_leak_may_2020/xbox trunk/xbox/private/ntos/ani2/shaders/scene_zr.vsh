xvs.1.1  

#define FINAL_MAT_COL0 c0
#define FINAL_MAT_COL1 c1
#define FINAL_MAT_COL2 c2
#define FINAL_MAT_COL3 c3	


#define Z_MUL			c16
#define Z_ADD			c17
#define POS_MUL			c18
#define POS_SHIFT		c19
; POS_MUL is something like 0.5f, 0.5f, 0.0f, 1.0f
; POS_SHIFT is 0.5f, 0.5f, 0.5f, 0.0f



#define VERT_POS	   v0

; Transform and project vertex.
dp4 r4.x, VERT_POS, FINAL_MAT_COL0
dp4 r4.y, VERT_POS, FINAL_MAT_COL1
dp4 r4.z, VERT_POS, FINAL_MAT_COL2
dp4 r4.w, VERT_POS, FINAL_MAT_COL3
mov oPos, r4

mov r10, r4.zzzz
mov r11, r4.wwww

add r8, r10, Z_ADD
mul r0.xyz, Z_MUL, r8
sge r0.w, r10, r10		; make the alpha component 1

; make sure oD0 is at least zero
slt r1, r10, r10
sge r1.w, r10, r10
max oD0, r0, r1

rcp r9, r11				; r9 is 1/w


mul r0, r4, r9			; divide screen coordinates by w


mul r3, r0, POS_MUL		; scaled by half (or so), so it goes from 0 to 1, z is nulled
add oT0, r3, POS_SHIFT
sge oT0.w, r9, r9

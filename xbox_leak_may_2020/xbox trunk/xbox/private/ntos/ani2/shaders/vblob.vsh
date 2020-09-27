;------------------------------------------------------------------------------
; Vertex shader to do ripple/wave effect.
;------------------------------------------------------------------------------
xvs.1.1

;------------------------------------------------------------------------------
; Vertex streams expected by this shader
;   struct VBlobConstantVertex
;   {
;   	D3DVECTOR unit_sphere_normal;	// v0
;   };
;   struct VBlobChangingVertex
;   {
;       D3DVECTOR4 normal;	// not normalized (do it in the GPU); w is the displacement due to bumps	// v1
;   };
;
; Expected vertex shaders constants
;    c4-c7    = Transpose of view*projection matrix
;    c8       = some constants, x=0, y=1, z=2, w=0.5
;    c9       = eye location
;    c10      = blob scaling
;    c11      = 1/ blob scaling
;    c12      = blob center
;
; Outputs:
; oPos        = position of vertex
; oT0         = surface normal (renormalize and dot with light)
; oT1         = direction to eye (renormalize and dot with light)
;------------------------------------------------------------------------------

#define VIEW_PROJ_T_0	c4
#define VIEW_PROJ_T_1	c5
#define VIEW_PROJ_T_2	c6
#define VIEW_PROJ_T_3	c7


#define ZERO			c8.x
#define ONE             c8.y
#define TWO				c8.z
#define HALF			c8.w


#define US_POSITION		v0
#define US_NORMAL		v0
#define LNORMAL			v1.xyz
#define DISPLACEMENT    v1.w

#define EYE_POS			c9
#define SCALING         c10
#define OO_SCALING      c11
#define CENTER          c12

; Destination temporary registers
#define L_EL_NORM       r9
#define ELLIPSE_NORMAL	r10
#define WORLD_POS       r11


; Adjust the unit-sphere normal for the scale factor
mul r10, US_NORMAL, OO_SCALING
dp3 r2.x, r10, r10
rsq r1.x, r2.x
mul ELLIPSE_NORMAL.xyz, r1.xxx, r10.xyz
mov ELLIPSE_NORMAL.w, ONE


mul r11, US_POSITION, SCALING
add r11, r11, CENTER			; r11 is now the position in world coordinates

; Add the displacement.
mov r0.xyz, v1.www
mov r0.w, ZERO
mad WORLD_POS, ELLIPSE_NORMAL, r0, r11		; r11 now has the displacement applied



;Normalize eye position
sub r0.xyz, EYE_POS.xyz, WORLD_POS.xyz
dp3 r2.x, r0.xyz, r0.xyz
rsq r1.x, r2.x 
mul r2.xyz, r1.x, r0.xyz
mov r2.w, ONE
mov r0.w, ONE
mov oT1, r2



;Normalize local surface normal
mul r0, LNORMAL, OO_SCALING
dp3 r2.x, r0, r0
rsq r1.x, r2.x 
mul L_EL_NORM.xyz, r1.x, r0
mov L_EL_NORM.w, ONE
mov oT0, L_EL_NORM




; Transform position by view*projection matrix.
dp4 oPos.x, WORLD_POS, VIEW_PROJ_T_0
dp4 oPos.y, WORLD_POS, VIEW_PROJ_T_1
dp4 oPos.z, WORLD_POS, VIEW_PROJ_T_2
dp4 oPos.w, WORLD_POS, VIEW_PROJ_T_3



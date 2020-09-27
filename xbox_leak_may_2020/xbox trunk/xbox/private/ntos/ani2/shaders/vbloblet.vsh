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
;
; Expected vertex shaders constants
;    c4-c7    = Transpose of view*projection matrix
;    c8       = some constants, x=0, y=1, z=2, w=0.5
;    c9       = eye location
;    c10      = blob center
;    c11      = direction of scaling
;    c12      = scaling perpendicular to direction
;    c13      = parallel minus perpendicular scaling multiplied by scaling direction
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
#define CENTER          c10
#define SCALE_DIR       c11
#define SCALE_DIR_PERP  c12
#define SCALE_DIR_PMP   c13
; SCALE_DIR_PMP is parallel scale minus perpendicular scale multiplied by SCALE_DIR

; Destination temporary registers
#define L_EL_NORM       r9
#define ELLIPSE_NORMAL	r10
#define WORLD_POS       r11


mov oT0, US_NORMAL


; Scale the point for wobble.
dp3 r0, US_POSITION, SCALE_DIR
mul r1, r0, SCALE_DIR_PMP
mad r2, SCALE_DIR_PERP, US_POSITION, r1
add WORLD_POS, r2, CENTER
mov WORLD_POS.w, ONE
; can combine the mul and add into another mad




;Normalize eye position
; (This stays in world coordinates, so it should probably just be normalized in the constant.
sub r0.xyz, EYE_POS.xyz, WORLD_POS.xyz
dp3 r2.x, r0.xyz, r0.xyz
rsq r1.x, r2.x 
mul r2.xyz, r1.x, r0.xyz
mov r2.w, ONE
mov r0.w, ONE
mov oT1, r2



; Transform position by view*projection matrix.
dp4 oPos.x, WORLD_POS, VIEW_PROJ_T_0
dp4 oPos.y, WORLD_POS, VIEW_PROJ_T_1
dp4 oPos.z, WORLD_POS, VIEW_PROJ_T_2
dp4 oPos.w, WORLD_POS, VIEW_PROJ_T_3



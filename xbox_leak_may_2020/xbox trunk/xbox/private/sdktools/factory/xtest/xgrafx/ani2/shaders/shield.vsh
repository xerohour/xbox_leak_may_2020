;------------------------------------------------------------------------------
; Vertex shader to render shields
;------------------------------------------------------------------------------
xvs.1.1

;------------------------------------------------------------------------------
; Vertex streams expected by this shader
;    struct ShieldVertex
;    {
;    	D3DVECTOR position;
;    	D3DVECTOR normal;
;    };
;
; Expected vertex shaders constants
;    c0-c3    = Transpose of object to world matrix
;    c4-c7    = Transpose of view*projection matrix
;    c8       = some constants, x=0, y=1, z=2, w=0.5
;    c9       = eye location in world space
;
; Outputs:
; oPos        = position of vertex
; oT0         = reflection vector (lookup into environment cubemap)
; oT1         = reflection vector for specular phong shading 
; oT2         = vertex normal in world coordinates
;------------------------------------------------------------------------------

#define WORLD_T_0		c0
#define WORLD_T_1		c1
#define WORLD_T_2		c2
#define WORLD_T_3		c3

#define VIEW_PROJ_T_0	c4
#define VIEW_PROJ_T_1	c5
#define VIEW_PROJ_T_2	c6
#define VIEW_PROJ_T_3	c7

#define ZERO			c8.x
#define ONE             c8.y
#define TWO				c8.z
#define HALF			c8.w

#define POSITION		v0
#define NORMAL			v1

#define EYE_POS			c9

#define BLOB_LIGHT_POS  c10
#define MOOD_LIGHT_POS  c11

; Destination temporary registers
#define WORLD_POS       r11
#define WORLD_NORMAL    r10

dp4 WORLD_POS.x, POSITION, WORLD_T_0
dp4 WORLD_POS.y, POSITION, WORLD_T_1
dp4 WORLD_POS.z, POSITION, WORLD_T_2
dp4 WORLD_POS.w, POSITION, WORLD_T_3

dp3 WORLD_NORMAL.x, NORMAL.xyz, WORLD_T_0.xyz
dp3 WORLD_NORMAL.y, NORMAL.xyz, WORLD_T_1.xyz
dp3 WORLD_NORMAL.z, NORMAL.xyz, WORLD_T_2.xyz

; Reflect eye vector off surface normal and store in T0.
sub r2, WORLD_POS, EYE_POS
;dp3 r2.x, r0.xyz, r0.xyz
;rsq r1.x, r2.x
;mul r2.xyz, r1.xxx, r0.xyz			; r2 now has normalized "vertex - eye" vector

dp3 r1.x, r2.xyz, WORLD_NORMAL.xyz
mul r1.y, r1.x, -TWO
mad r3.xyz, r1.yyy, WORLD_NORMAL.xyz, r2.xyz
mov oT0.xyz, r3.xyz ;  store reflection vector for cubemap lookup

; compute reflection vector for blob light
sub r8,WORLD_POS,BLOB_LIGHT_POS
dp3 r1.x, r8.xyz, WORLD_NORMAL.xyz
mul r1.y, r1.x, -TWO
mad r3.xyz, r1.yyy, WORLD_NORMAL.xyz, r8.xyz
mov oT1.xyz, r3.xyz ;  


; compute reflection vector for mood light
sub r8.xyz,WORLD_POS.xyz,MOOD_LIGHT_POS.xyz
dp3 r1.x, r8.xyz, WORLD_NORMAL.xyz
mul r1.y, r1.x, -TWO
mad r3.xyz, r1.yyy, WORLD_NORMAL.xyz, r8.xyz
mov oT2.xyz, r3.xyz ;  

; pass vertex-to-eye
mov oT3.xyz,-r2.xyz


; Transform position by view*projection matrix.
dp4 oPos.x, WORLD_POS, VIEW_PROJ_T_0
dp4 oPos.y, WORLD_POS, VIEW_PROJ_T_1
dp4 oPos.z, WORLD_POS, VIEW_PROJ_T_2
dp4 oPos.w, WORLD_POS, VIEW_PROJ_T_3



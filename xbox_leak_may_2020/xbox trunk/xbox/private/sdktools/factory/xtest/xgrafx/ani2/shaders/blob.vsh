;------------------------------------------------------------------------------
; Vertex shader to do ripple/wave effect.
;------------------------------------------------------------------------------
xvs.1.1

;------------------------------------------------------------------------------
; Vertex streams expected by this shader
;   struct BlobVertex
;   {
;   	D3DXVECTOR3 pos;						(v0)
;       // D3DXVECTOR3 normal;						(v1) // same as pos!
;   	// Diffuse color is set as a constant.
;   };
;
; Expected vertex shaders constants
;    c0-c3    = Transpose of world matrix
;    c4-c7    = Transpose of view*projection matrix
;    c8       = some constants, x=0, y=1, z=2, w=0.5
;
; Outputs:
; oT0         = surface normal (renormalize and dot with light)
; oD0         = base blob color
; oD1         = light direction
;------------------------------------------------------------------------------

#define WORLD_MAT_T_0	c0
#define WORLD_MAT_T_1	c1
#define WORLD_MAT_T_2	c2
#define WORLD_MAT_T_3	c3

#define VIEW_PROJ_T_0	c4
#define VIEW_PROJ_T_1	c5
#define VIEW_PROJ_T_2	c6
#define VIEW_PROJ_T_3	c7


#define ZERO			c8.x
#define ONE             c8.y
#define TWO				c8.z
#define HALF			c8.w


#define POSITION		v0
#define NORMAL			v0

#define LIGHT_AMB		c9

#define BLOB_COLOR		c10



mov oT0, NORMAL



; Transform position by world matrix (store in r2)
dp4 r2.x, POSITION, WORLD_MAT_T_0
dp4 r2.y, POSITION, WORLD_MAT_T_1
dp4 r2.z, POSITION, WORLD_MAT_T_2
dp4 r2.w, POSITION, WORLD_MAT_T_3

; Transform position by view*projection matrix.
dp4 oPos.x, r2, VIEW_PROJ_T_0
dp4 oPos.y, r2, VIEW_PROJ_T_1
dp4 oPos.z, r2, VIEW_PROJ_T_2
dp4 oPos.w, r2, VIEW_PROJ_T_3



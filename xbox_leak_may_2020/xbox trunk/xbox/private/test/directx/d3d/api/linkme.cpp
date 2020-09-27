#include "d3dapi.hpp"

D_( 0); 
D_( 1); 
D_( 2); 
D_( 3); 
D_( 4); 
D_( 5); 
D_( 6); 
D_( 7); 
D_( 8); 
D_( 9); 
D_(10); 
D_(11); 
D_(12); 
D_(13);
D_(14);
D_(15);
D_(16);
D_(17);
D_(18);
D_(19);

void CallAllDummyFunctions ()
{
	C_( 0); //IDirect3DTexture8
	C_( 1); //IDirect3DCubeTexture8
//	C_( 2); //cursor
	C_( 3); //IDirect3DVolume8
  C_( 4); //IDirect3D8 (including CreateDevice tests)
	C_( 5); //init for Idirect3D8
	C_( 6); //IDirect3DDevice8
	C_( 7); //init for IDirect3DDevice8
	C_( 8); //IDirect3DResource8
	C_( 9); //IDirect3DVolumeTexture8
//  C_(10); //lock
//  C_(11); //cubelock
//  C_(12); //volumelock
    C_(13); //vertexbuffer
//  C_(14); //uma //broken for the time being
//	C_(15); //swiz (temporary)
 // C_(16); //leak
 // C_(17); //logicop
 // C_(18); //push-size
  C_(19); //math speed & accuracy tests
}

//-----------------------------------------------------------------------------
// Name: DotProduct3 Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
  The DotProduct3 samples demonstrates an alternative approach to Direct3D
  bumpmapping. This technique is named after the mathematical operation which
  combines a light vector with a surface normal. The normals for a surface are
  traditional (x,y,z) vectors stored in RGBA format in a texture map (called a
  normal map, for this technique).


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The lighting equation for simulating bump mapping invloves using the dot
   product of the surface normal and the lighting vector. The lighting vector
   is simply passed into the texture factor, and the normals are encoded in a
   texture map. The blend stages, then, look like
       SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3 );
       SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
       SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

   The only trick then, is getting the normals stored in the texture. To do
   this, the components of a vector (XYZW) are each turned from a 32-bit
   floating value into a signed 8-bit integer and packed into a texture color
   (RGBA). The code show how to do this using a custom-generated normal map,
   as well as one built from an actual bumpmapping texture image.

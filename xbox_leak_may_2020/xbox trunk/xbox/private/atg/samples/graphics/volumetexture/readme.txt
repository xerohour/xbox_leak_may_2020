//-----------------------------------------------------------------------------
// Name: VolumeTexture Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The VolumeTexture sample illustrates how to use the new volume textures in
   Direct3D. Normally, a texture is thought of as a 2D image, which have a
   width and a height and whose "texels" are addressed with two coordinate, 
   tu and tv. Volume textures are the 3D counterparts, with a width, height,
   and depth, are are addressed with three coordinates, tu, tv, and tw.

   Volume textures can be used for interesting effects like patchy fog,
   explosions, etc..
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Volume textures are no more difficult to use than 2D textures. In this
   sample source code, look for the vertex declaration (which has a third 
   texture coordinate), texture creation (which also takes a depth dimension),
   and texture locking (again with the third dimension). The 3D rasterizer
   interpolates texel values much as it would for 2D textures.

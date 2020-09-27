//-----------------------------------------------------------------------------
// Name: ProjectedTexture Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The ProjectedTexture sample illustrates using projected textures. With this
   technique, a 2D texture can be "projected" onto a 3D object, much like a
   film projector casts an image onto a wall.

   Projected textures can be use for a variety of effects such as shadows and
   lighting.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The projected texture technique is the focus of this sample. With projected
   textures, an object is rendering with Direct3D generating the texture
   coordinates and the D3DTTFF_PROJECTED flag. The texture coordinates are 3D
   and are transformed into place via the texture transform (which is updated
   each frame as objects are moved).

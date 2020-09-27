//-----------------------------------------------------------------------------
// Name: Swizzle Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Swizzle sample shows a texture that the user can rotate and toggle
   various options to explore the performances differences between using
   swizzled and linear textures.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   In almost all cases, textures are "swizzled", which means the hardware
   (for performance reasons) stores them in a way such that the adressing
   index (from the tu, tv coordinates) are interleaved. To access the texture
   data, a Lock() call is made, and special code needed to access the
   swizzled texels.

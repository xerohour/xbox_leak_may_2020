//-----------------------------------------------------------------------------
// Name: BumpEarth Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The BumpEarth program demonstrates the bumpenvmapping style of bumpmapping
   for Direct3D. Bumpenvmapping is a texture blending technique used to render
   the appearance of rough, bumpy surfaces. This sample renders a rotating,
   bumpmapped planet Earth.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Bumpenvmapping is an advanced multitexture blending technique that can be
   used to render the appearance of rough, bumpy surfaces. The bump map itself
   is a texture that stores the perturbation data. Bumpmapping requires two
   textures, actually. One is an environment map, which contains the lights 
   that you see in the scene. The other is the actual bumpmapp, which contains
   values (stored as du and dv) used to "bump" the environment map's texture 
   coordinates. Some bumpmaps also contain luminance values to control the
   "shininess" of a particular texel.

   On the Xbox, bumpenvmapping requires the use of a pixel shader. The bumpmap
   is stored in a texture stage that is followed by the envmap. The bumpmap
   equation used the D3DTSS_BUMPMATRIX elements as well, which are set using
   the SetTextureStageState() API. There are two caveats to be aware of when
   using bumpenvmapping on the Xbox. First, due to the unfortunate way that 
   bump values (du and dv) are encoded, the envmap can not be filtered. Second,
   the bump equation does not include a per-pixel divide, so projected textures
   are not possible with bumpenvmapping.
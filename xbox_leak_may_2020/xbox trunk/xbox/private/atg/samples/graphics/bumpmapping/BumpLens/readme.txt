//-----------------------------------------------------------------------------
// Name: BumpLens Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The BumpLens sample demonstrates a lens effect that can be acheived using
   bumpmapping. Bumpmapping is a texture blending technique used to render the
   appearance of rough, bumpy surfaces, but can also be used for other effects
   as shown here.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Bumpmapping is an advanced multitexture blending technique that can be used
   to render the appearance of rough, bumpy surfaces. The bump map itself is a 
   texture that stores the perturbation data. Bumpmapping requires two
   textures, actually. One is an environment map, which contains the lights 
   that you see in the scene. The other is the actual bumpmapping, which 
   contain values (stored as du and dv) used to "bump" the environment maps 
   texture coordinates. Some bumpmaps also contain luminance values to control 
   the "shininess" of a particular texel.

   This sample uses bumpmapping in a non-traditional fashion. Since bumpmapping
   really just perturbs an environment map, it can be used for other effects. In
   this case, perturbing a background image (which could be rendered on the fly)
   to make a lens effect.


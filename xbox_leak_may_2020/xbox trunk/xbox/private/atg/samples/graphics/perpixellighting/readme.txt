//-----------------------------------------------------------------------------
// Name: PerPixelLighting Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PerPixelLighting sample uses the DotProduct3 texture stage state to
   achieve per pixel lighting effects (in this case, bumpmapping).
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The end result in this sample is an acumulation of many passes: one for the
   base texture, ambient lighting, directional lights, and point lights. The 
   lighting passes use the DotProduct3 texture stage state to perform the
   lighting calculation. The normal vectors are encoded in a texture, which
   serves as a per-pixel lookup. The light vector is passed in as a vertex
   component, or as a constant texture factor. See the sample source code for
   more details on the technique.

   Note that this sample implements the per-pixel lighting texhnique without
   the use of vertex shaders, and is provided for comparison purposes.

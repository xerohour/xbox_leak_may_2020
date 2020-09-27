//-----------------------------------------------------------------------------
// Name: Cartoon Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Cartoon sample illustrates a 3D rendering technique to achieve a cartoon
   effect.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The cartoon effect is essentially an environment mapping technique that uses
   a 1-D texture. The texture coordinates need to be computed each frame, based
   on a lighting calculation that involves the light position and the vertex
   normal. This calculation is performed most conveniently by a vertex shader.
   
   An additional pass is used for a cartoonish, black outline.   






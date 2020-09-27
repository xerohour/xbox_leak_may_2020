//-----------------------------------------------------------------------------
// Name: Tut05_Textures Xbox Graphics Tutorial
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Textures tutorial shows how to load a texture image from a file and use
   it to add texture to a polygon.


Required files and media
========================
   Copy all required media to the target machine before running this tutorial.


Programming Notes
=================
   Image files can be loaded into texture objects using the D3DX helper
   functions. To use textures, a suite of state calls are usually made to
   control how the textures are used (decal, blending, etc.). Also, vertices
   must have texture coordinates, with can either be supplied manually 
   (part of the vertex structure) or having D3D automatically generate
   texture coordinates for you (for example, based on the camera space
   position of the vertices).

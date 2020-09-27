//-----------------------------------------------------------------------------
// Name: CompressedNormals Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The CompressedNormals sample demonstrates the bandwidth-saving technique of
   using compressed vertex normals.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Vertex normals typically require one 32-bit floating point value for each
   of the normal's x, y, and z components. The Xbox graphics hardware supports
   compressing these normals into one packed 32-bit value. This saves 8 bytes
   of data per vertex.

   To use packed normals, the app must use the PACKEDNORM3 data type in the
   vertex shader declaration. For the fixed-pipeline, the vertex shader
   declaration must still be used. See the app for details.

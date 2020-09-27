//-----------------------------------------------------------------------------
// Name: StateShader Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The StateShader sample shows how to use a "state shader". This sample uses
   a state shader to invert a matrix. To show that everything works, it actually
   inverts the matrix again, to get back to the original rotation matrix. This
   is not visually exciting, just a proof of concept.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   A "vertex shader" operates on vertices. Additionally, since some shaders
   can write to vertex shader constants, you can have a "state shader" which
   only writes states. This might be useful for concatenating matrices, or
   any other operations that pre-compute constants. Normally, these operations
   are done on the CPU, but a state shader lets you offload more of that
   processing to the GPU.

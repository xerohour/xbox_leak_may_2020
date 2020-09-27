//-----------------------------------------------------------------------------
// Name: Minnaert Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   Sample to show how to use a 2D texture as a per-pixel lookup table
   for lighting (the tu and tv coordinates are used to look up a value
   for the LdotN contribution modulated with the EdotN contribution.)
   This is a partial solution to a more general form of BRDF lighting,
   and can be used to implement Minnaert lighting (nice, per-pixel self-
   shadowing effect), as well as satin, velvet and other effects.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The main focus of this sample is the construction of the 2D texture which
   are used as a 2D lookup table for the independant lighting equation inputs
   (LdotN and EdotN). Depending on the construction of this texture, different
   effects like Minnaert lighting, satin, or velvet, can be achieved. A vertex
   shader is also used for this effect, to compute the LdotN and EdotN factors,
   and output them as texture coordinates.


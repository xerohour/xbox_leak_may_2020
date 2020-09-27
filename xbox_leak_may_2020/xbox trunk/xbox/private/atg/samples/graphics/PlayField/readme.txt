//-----------------------------------------------------------------------------
// Name: PlayField Xbox Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

Description
===========
   The PlayField sample draws a grassy sports field.

Required files and media
========================
   Copy the media tree to the target machine before running this sample.

Programming Notes
=================
   The grass is drawn back-to-front using a series of "fins". The
   grass strip texture was sampled from geometry as a pre-process.
   The grass side-view texture is modulated by the underlying field
   texture to get stripes and rough spots.  When used in a sports
   game, the field texture can be updated dynamically to get shadows
   of the ball and players.

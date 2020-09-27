//-----------------------------------------------------------------------------
// Name: AlphaFog Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The AlphaFog sample illustrates a fogging technique that modifies the alpha
   value of far away objects. This is useful in scene-culling techniques to
   avoid the visible "pop" that occurs when far-away objects (like trees and
   buildings) get added to a scene.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The objects which get alpha-fogged are the columns. Look at the code (esp.
   the vertex shader) used to render the columns.

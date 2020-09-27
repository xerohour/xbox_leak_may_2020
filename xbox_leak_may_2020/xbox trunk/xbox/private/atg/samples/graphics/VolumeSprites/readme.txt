//-----------------------------------------------------------------------------
// Name: VolumeSprites Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The VolumeSprites sample shows how to use point sprites with volume
   textures. This can be useful and interesting, for apps that wish to display
   several textures within one particle system.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   When using point sprites, the hardware supplies the tu and tv texture
   coordinates. When using a volume texture, the app must still provide
   a third texture coordinate. This can be easily done using a vertex
   shader, writing to the oT3.x output register. (Note that point sprites
   always use texture stage 3.)

   When using volume textures, keep in mind that they have half the
   fillrate potential of 2D textures.

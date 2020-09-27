//-----------------------------------------------------------------------------
// Name: PointSprites Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PointSprites sample shows how to use the new Direct3D point sprites
   feature. A point sprite is simply a forward-facing, textured quad that is
   referenced only by (x,y,z) position coordinates. Point sprites are most
   often used for particle systems and related effects.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Without Direct3D's support, point sprites can be implemented with four
   vertices, that are oriented each frame towards the eyepoint (much like a
   billboard). With Direct3D, though, you can refer to each point sprite by
   just it's center position and a radius. This saves heavily on processor
   computation time and on bandwidth uploading vertex information to the
   graphics processor.

   In this sample, a particle system is implemented using point sprites. Each
   particle is actually implemented using multiple alpha-blended point sprites,
   giving the particle a motion-blur effect.
   
   One very important thing to keep in mind is that the hardware requires
   pointsprites to only use texture stage 3.

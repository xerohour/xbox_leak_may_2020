//-----------------------------------------------------------------------------
// Name: VolumeLight Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The VolumeLight sample uses a volumetexture to light a scene.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Using a volumetexture to add lighting effects to a scene is akin to the use
   of 2D lightmaps. The advantage with a 3D volume light map is that tex coords
   can be computed using automatic tex gen, which greatly simplies the coding
   of the effect.

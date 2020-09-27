//-----------------------------------------------------------------------------
// Name: Fur Xbox Sample
// 
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Fur sample demonstrates a technique for rendering hair or fur
on an object.  The fur is rendered using concentric shells of a volume
texture.  As the objects get further away, shells are blended together
and combined to improve rendering speed.  The sample also demonstrates
how the fur can be deformed, such as by a blow dryer (using right
trigger).
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================

In this sample, the hair textures are created at load time by taking
texture slices from a particle system which represents the individual
hairs. In a game, the hair texture creation would be done offline and
the compressed texture files would be saved.  See CreateHairTextures()
in the main Fur.cpp file for the parameters and XBFur::GenSlices for
the process of converting the procedural hair into textures.  See
Mipmap.cpp for how to compute high-quality mipmaps using the GPU.

   The main fur effect is created from the concentric shells with hair
lighting.  The "fins" add to the effect by making improved
silhouettes. The fins are edges of the original skin mesh extruded
along the normal.

   There are two kinds of level-of-detail used in this sample.  The
first is the standard geometric complexity. The center bear has 2000
faces, the surrounding bears 1000, and the distant bears have just
500.  The second kind of level-of-detail is with the concentric layers
of fur. Fur is an expensive effect, with 8x (or more, if m_iNumSlices
is set higher) cost over traditional texture mapping.  Since the
spacing between concentric slice textures diminishes with distance, a
good level-of-detail is to fade out the odd slices and composite the
odd slices into the even slices. This makes a smooth transition from
standard texture mapping (although with a fur lighting model) to the
full layered fur effect.  See ComputeLevelOfDetailTextures in
XBFur.cpp and the furfade*.psh pixel shaders in media\Shaders.

   There are several vertex shaders to choose from, depending on the
current configuration.  When the "blow dryer" effect is not active, a
less expensive shader is used.  When directional lighting is used
instead of local lighting, the less expensive directional lighting
vertex shader may be used.  These are created using #ifdef's in the
shader files.  See fur.vsh and fin.vsh in media\Shaders for more
details.

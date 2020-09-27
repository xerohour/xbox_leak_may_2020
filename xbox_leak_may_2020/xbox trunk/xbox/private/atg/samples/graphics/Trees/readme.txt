//-----------------------------------------------------------------------------
// Name: Trees Xbox Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

Description
===========

   The Trees sample demonstrates a technique for rendering trees with
multiple levels of detail.  The full geometry of the tree is used for
close-up renderings.  For middle distances, a set of "slice textures"
is used to render each branch.  For far distances, a set of slice
textures is used for the whole tree.  The number of slices decreases
with distance, so the most-distant trees are just like the familiar
one-card billboard trees.

Required files and media
========================
   Copy the media tree to the target machine before running this sample.

Programming Notes
=================

The slice textures are created by rendering the full geometry with
clipping planes placed around the desired slice region.  Since the
Xbox is used to render the slice, there is a close match to the full
geometry rendering.

Slice textures trade texture memory for geometric complexity.  In this
sample, a 30,000 polygon tree is adequately represented by around 100
polygons and 24 slice textures for the highest level of detail, and
from 8 slices down to 1 slice for the lowest level of detail.

The choice of what goes into the slices can be quite broad.  In this
sample, we create slice textures of individual branches, and of whole
trees. A similar technique may be used to create slices of a tree
grove or a tree forest tile.

Since the alpha channel is used to represent the partially transparent
portions of each slice texture, the slices must be rendered from back
to front (or front to back using a destination alpha channel in the
render target.)

To blend between the various levels of detail, the scene is drawn once
with the old LOD and once with the new LOD, and then the full
backbuffer is blended with a transition blend factor.

//-----------------------------------------------------------------------------
// Name: SwapCallback Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The SwapCallback sample demonstrates how to use Swap and VBlank callbacks
   to determine when the app is close to dropping frames. An app can use this
   information to scale back rendering quality instead of dropping frames.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The app is made to be fill bound by drawing a bunch of big, anti-aliased
   quads on the screen. The user can control the number of quads, and observe
   how close the app is to dropping frames.

   If an app does determine that is close to (or beyond) the point where it
   is dropping frames, it can takes steps to reduce fidelity in the app to
   reduce the overall frame time. What steps the app takes to reduce frame
   time actually depends on whether the app is fillbound, vertex bound, CPU
   bound, or a mix of the three.
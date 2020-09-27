//-----------------------------------------------------------------------------
// Name: BackBufferScale Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample shows how to use the SetBackBufferScale API to reduce
   (via a scale factor) the effective size of the backbuffer. Scaling the
   backbuffer dynamically lessens the fill requirements of the app, so
   this is a technique that could be useful for fillbound apps that are
   willing to sacrifice quality in order to maintain framerate.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   To prevent tearing, note that the rendering device is created with
   two backbuffers. After rendering a scene, the backbuffer is scaled and
   copied to the next buffer in the chain, via the Swap() API. In between
   the two swap calls, non-scaled elements (such as for UI objects) are
   drawn.

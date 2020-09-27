//-----------------------------------------------------------------------------
// Name: PushBuffer Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PushBuffer sample illustrates using static pushbuffers. Pushbuffers can
   be thought of as display lists or instruction buffers to the GPU. Rather
   than rendering a scene via a myriad of D3D calls each frame, the calls can
   be recorded into a static pushpuffer. Any dynamic data within a pushbuffer,
   such as vertex shader constants that control the rotation of an object, can
   be modified on the fly via "fixup" objects.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The pushbuffer used in this app simply draws a rotating triangle. The scene
   is rotated via the matrix set that is passed into the vertex shader. Each
   frame, before rendering, the static pushbuffer has a fixup object applied,
   which updates the vertex shader constants inside the pushbuffer.

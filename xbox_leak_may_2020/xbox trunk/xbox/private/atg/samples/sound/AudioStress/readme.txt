//-----------------------------------------------------------------------------
// Name: AudioStress Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The AudioStress sample demonstrates the effects of CPU and GPU load on
asynchronous streaming audio playback.  The two thumbsticks control the CPU
and GPU load, the scale of which are defined in constants CPU_MAXTIME and
GPU_MAXTIME.  Increasing the CPU/GPU load lowers the framerate, which also
lowers the rate at which packets get transferred among XMOs, leading to
glitching.  A higher CPU load also takes time away from the thread on which
decoding and rendering occurs, also causing glitching.  By increasing the
packet size and/or number of packets, higher CPU and GPU loads can occur
without audio glitching.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================

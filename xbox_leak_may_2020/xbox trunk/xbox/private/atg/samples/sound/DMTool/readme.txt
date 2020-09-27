//-----------------------------------------------------------------------------
// Name: DMTool Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The DMTool sample demonstrates how to use Direct Music Tools to 
synchronize video and audio.  The tool intercepts note messages and keeps
track of the most recent note message for each note on each channel, which
is then displayed on screen.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The sample loads all segments and downloads all instrument data when it
initializes.  Downloading instrument data can take a substantial amount of
time and is a synchronous call, and so should not be performed during
gameplay on the primary rendering thread.

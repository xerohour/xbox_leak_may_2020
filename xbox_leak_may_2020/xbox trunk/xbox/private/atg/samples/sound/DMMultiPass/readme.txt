//-----------------------------------------------------------------------------
// Name: DMMultiPass Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   Demonstrates how to use multipass audio effects with DirectMusic. The sample 
has the same functionality as the MultiPass sample, but does it using DirectMusic 
instead of DirectSound.  Several segments are loaded and playback/volume is 
individually controlled for each segment.  All segments are played on the same
3d audiopath, which is then positioned by retrieving the 3d buffer via 
GetObjectInPath.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================

//-----------------------------------------------------------------------------
// Name: WMVCutScene Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The WMVCutScene sample demonstrates how to use the WMV decoder to play back
WMV-encoded video cut scenes using the Direct3D overlay surface.  The playback
logic is encapsulated in the CWMVPlayer class, so the actual playback process
is pretty simple:
1) See if it's time to display a frame
2) If so, grab the video frame and display it, then decode the next frame
3) Repeat until done
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The audio streams may end before the end of the video.  However, the only
notification comes at the end of the entire video.  If the audio ends before
the video does, then the stream will display the starvation warning message,
but this can be safely ignored.

   The CWMVPlayer helper class should be used as follows:
1) Initialize the object with pointers to the D3D Device and DirectSound
    objects
2) When ready to begin playback, open the file and get the video information.
3) Set up textures to hold decoded video frames
4) Run in a loop, checking to see if a new frame is ready to be displayed.
    If it is, call GetTexture() to get the texture, and display it.  Then,
    decode the next frame.
5) When DecodeNext() returns S_FALSE, playback is done


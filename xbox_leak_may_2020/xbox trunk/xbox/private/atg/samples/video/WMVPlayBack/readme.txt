//-----------------------------------------------------------------------------
// Name: WMVPlayBack Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The WMVPlayBack sample demonstrates how to use the WMV Decoder XMO to play
WMV encoded videos on the XBox.  The WMV Decoder XMO processes both audio and
video XMEDIAPACKETS.  The audio packets are submitted to a DirectSoundStream
and the video data is stored into a texture for rendering onto a screen-space
quad.  Reference time is used to synchronize the audio and video.
   The DirectSoundStream automatically keeps track of the order in which 
audio packets were submitted, as well as paying attention to the reference time
timestamps on each packet.  
    The WMVPlayBack sample shows how to do similar management of video frames, 
by keeping a queue of pending frames, along with their timestamps.  The frame 
that is rendered is the most recent frame with a timestamp before the current 
reference time.  Earlier frames are marked as free to be filled by the decoder.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   On any given call to ProcessMultiple, the WMV Decoder XMO could return
audio data, video data, or both.  The caller needs to provide valid audio
and video packets to be filled.
   In order to determine when a video frame is free to be filled by the
decoder, we have to be sure that it is no longer being used by the GPU.  For
overlays, this is pretty easy: at the first vblank after a call to
UpdateOverlay, we know the new overlay surface is being used, so we can 
reclaim the previous surface.  For textures, there could be up to 3 frames
queued up in the push buffer, so we don't know that a texture has been
rendered until 3 vblanks after it was initially rendered.  

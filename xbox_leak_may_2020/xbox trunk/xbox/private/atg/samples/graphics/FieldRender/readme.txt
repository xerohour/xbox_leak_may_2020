//-----------------------------------------------------------------------------
// Name: FieldRender Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The FieldRender sample shows how to do field rendering on the Xbox.
   Normally, Xbox apps render to one 640x480 buffer at 60 Hz, and the hardware
   filters the buffer down to odd and even interlaced fields. With field
   rendering, an app renders seperately for the odd and even fields, and only
   to a 640x240 buffer.

   The technique saves on fillrate (since only half as many pixels are drawn)
   but the resulting quality is not as good.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Field rendering is enabled during device creation by using the
   D3DPRESENTFLAG_FIELD flag. The app must render at 60 fps for field
   rendering, and, to minimize flicker, the viewport must be offset by one
   scanline during the rendering of odd fields. Finally, note that the flicker
   filter does not work when field rendering is enabled.

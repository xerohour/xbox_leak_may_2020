//-----------------------------------------------------------------------------
// Name: WMAStream Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The WMAStream sample shows how to use the synchronous WMA decoder XMO on a
separate thread to play background music.  The worker thread will periodically
check for completed packets from the DirectSound stream, run the WMA decoder
to decode sound data, and then dispatch them back to the DirectSound stream.
This thread could potentially be used to run several different decoders.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   If the WMA decoder is run asynchronously, it spins off its own thread on
which to run.  Since this can cause substantial overhead if several of these
are used simultaneously, in some cases it makes more sense to have the title
create its own thread and manually control the processing of the decoder.  
   In this sample, we spin off a thread to handle the process of decoding 
packets and passing them off to the DirectSound stream.  The minimum frequency
with which processing should occur is determined by the amount of data that
is sent to the stream at one time.  Processing needs to occur frequently 
enough that the stream doesn't run out of data.

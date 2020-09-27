//-----------------------------------------------------------------------------
// Name: FileStream Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The FileStream sample shows how to use the wave file Media Object to
stream an ADPCM wav file from disk and loop it.  Since the Audio Processor 
can decompress ADPCM in hardware, all that has to be done to play an ADPCM
wav file is to pass the packets on to the DirectSoundStream.
   Since the wave file Media Object operates synchronously, the sample runs
the media object on a separate worker thread to avoid blocking the main thread.
   This sample has changed substantially from earlier releases - 
AdpcmCreateDecoder no longer exists because there's no need for it - ADPCM
data can be passed straight to the stream.

   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   As noted above, a worker thread is spun off to perform the audio processing.
This is because the source XMO operates synchronously, and would cause the
main thread to block while reading data from the disk.  By processing on a
separate thread, only that thread is blocked during I/O.  Alternately, the app
could have its own loading mechanism instead of using the wave file media 
object, and pass the ADPCM data straight to the DirectSoundStream.

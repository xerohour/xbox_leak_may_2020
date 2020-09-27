//-----------------------------------------------------------------------------
// Name: BackgroundMusic Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The BackgroundMusic sample demonstrates how one would play background WMA
music in an Xbox game.  This is done through the CMusicManager class, which 
encapsulates both soundtracks stored on the Xbox hard drive as well as WMA
files that ship with the game.
   The CMusicManager class uses 1 WMA decoder and 2 DirectSound streams to 
perform the playback.  When the decoder hits the end of the currently playing 
track, a new decoder is created to start decoding the next track while the end 
of the previous track is still playing.  It then performs a crossfade between 
the 2 streams to transition to the new track.  The duration of the crossfade 
is determined by the amount of decoded sound data that is buffers.  
   Since the 2 streams share the same audio buffer, the new stream could get 
starved as it is first starting up.  To avoid this, a small number of packets
are reserved for the new stream to use when it first starts playing.  
   By default, the CMusicManager class spins its own worker thread for 
processing, in order to prevent the main rendering loop from stalling.  
Alternately, MusicManagerDoWork() can be periodically called from a worker
thread spawned by the game.  This routine should never be called by the 
main game thread, as it could block on disk I/O.  Using synchronization
primitives to coordinate between the game thread and the worker thread would 
waste a lot of CPU time, so none were used.  Instead, only certain methods 
are meant to be called from the main game thread, and some of those have 
additional restrictions or caveats.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The user is limited to storing 100 soundtracks on their Xbox, which
means that we don't have to worry about using up too much memory when caching
soundtrack information.  Each soundtrack could have up to 500 songs in it,
which means it's not appropriate to attempt to cache every song in memory.

   The intent was to use as little CPU as possible.  As such, we only use
1 WMA decoder at a time, and use DLS envelopes to perform the crossfade.
That is also why there is no synchronization between the main game thread
and the worker thread.  NOTE: In the November release, there is no way to
trigger the release segment of a DLS envelope on a stream, so a volume
fade is used to emulate the functionality.

   One of the difficult aspects of WMA decoding is that the amount of CPU time
required to decode a packet of audio data can vary dramatically depending on
the content.  If the WMA decoding thread were allowed to run for its full 
quantum, it could consume as much as 20ms each time the thread is executed.
In order to alleviate these spikes, the decoder can be instructed to yield its
thread's quantum periodically.  This won't actually decrease the amount of time
it takes to decode a packet's worth of audio data.  In fact, it will slightly 
increase the time taken, but it spreads the time out over several thread 
quantums.  This gives a much finer granularity of execution than the thread 
scheduler's 20ms quantum.  Analysis has shown that with a dwYield value of 1, 
the maximum execution time per quantum of the worker thread is approximately
2.5ms for user soundtracks.  

   It's important to make sure that the worker thread still processes often
enough not to starve the streams it is feeding.  In addition, by spreading out
the worker thread's execution time, other work being done on that thread will 
be less responsive.  A yield rate of 1 is NOT sufficient to ensure that user
soundtracks will be decoded fast enough to keep the stream from starving.  
Some soundtracks may even starve with a yield rate of 3.

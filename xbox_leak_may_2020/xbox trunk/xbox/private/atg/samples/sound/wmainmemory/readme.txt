//-----------------------------------------------------------------------------
// Name: WMAInMemory Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The WMAInMemory sample shows how to use the synchronous WMA decoder XMO to
decode WMA data from memory rather than from a file.  The title implements a
callback function which the decoder uses to get a pointer to the WMA data.
From that point on, things function similarly to the other WMA streaming
samples AsyncXMO and WMAStream.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The In-Memory WMA decoder operates synchronously when Process() is called.
It's important to note the effect this can have on the title, since the main
title thread will be waiting for the decoding to complete before processing
any further.  The AsyncXMO sample avoids this by creating an asynchronous 
WMA decoder.  The WMAStream sample avoids this by creating a worker thread
which will call Process() on the WMA decoder.  Running synchronously is less
of an issue decoding from memory, since there is no reading from file, but
in a real-game situation, this would probably be best done on a worker 
thread, similar to WMAStream.
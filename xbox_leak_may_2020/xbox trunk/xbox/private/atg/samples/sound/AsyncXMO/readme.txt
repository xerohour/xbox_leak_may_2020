//-----------------------------------------------------------------------------
// Name: AsyncXMO Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The AsyncXMO sample demonstrates how to use asynchronous XMOs.  The idea is
to allocate a pool of packets to be transferred through the XMO chain.  
Periodically, the title checks the status of the packets and transfers completed
packets to the next step in the chain.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Asynchronous XMOs actually spin off a worker thread for each xmo created.
Because of this overhead, asynchronous XMOs should generally be used for
things like single shot sound effects, etc.  Background music and sounds 
should be played by having the title create its own worker thread running
several synchronous XMOs (see the WMAStream sample).

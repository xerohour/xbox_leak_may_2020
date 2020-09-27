//-----------------------------------------------------------------------------
// Name: AntiAlias Xbox Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The AntiAlias sample - Shows how to use the various antialias modes on the
   Xbox
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Allows you to change the antialias modes on the fly so that you can see the
   appearance of the objects and the performance penalties for turning each 
   antialias mode on.  Far and away, the largest win is edge antialiasing which 
   does not require a larger backbuffer.  In many cases this looks as good as 
   9x sampling!  There caveats to the edge anti-aliasing which are commented in
   the code.

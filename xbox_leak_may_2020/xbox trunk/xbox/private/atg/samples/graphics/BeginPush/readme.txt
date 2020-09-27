//-----------------------------------------------------------------------------
// Name: BeginPush Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The BeginPush sample demonstrates the use of the BeginPush() and EndPush()
   Direct3D APIs that give the developer tighter control over the pushbuffer.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Using the BeginPush() API can result in a performance win, although it's 
   use is not trivial. In a nutshell, the API is like exposing the inner 
   workings of the Begin()/End() API, so the app can minimize overhead.
   Refer to the code for details on using the API and the helper macros that
   go with.

//-----------------------------------------------------------------------------
// Name: PersistDisplay Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PersistDisplay sample shows how to persist an image in the frame buffer
   while rebooting the Xbox to launch a new title xbe.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Persisting the display includes two steps: (1) actually persisting before
   launching a new title, and (2) optionally transition the persisted surface
   in a startup animation after the new title loads.

   This sample shows how to do both using the PersistDisplay() and 
   GetPersistedDisplay() API, respectively.

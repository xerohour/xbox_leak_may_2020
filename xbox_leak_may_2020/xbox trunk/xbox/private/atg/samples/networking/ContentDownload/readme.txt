//-----------------------------------------------------------------------------
// Name: ContentDownload
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample illustrates Xbox Online content enumeration for hard disks,
   DVDs, and online servers. Illustrates content download, installation,
   verification and removal.

Programming Notes
=================
   DVD enumeration is currently disabled. Selecting DVD enumeration displays
   an error message. If installation is cancelled, it is up to the title
   to handle removing the content that was installed. This sample shows
   how to properly handle installation cancellation.
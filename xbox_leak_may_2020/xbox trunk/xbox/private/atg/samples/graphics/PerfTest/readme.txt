//-----------------------------------------------------------------------------
// Name: PerfTest Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PerfTest sample shows how to use the graphics performance testing API.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The performance testing API is defined in the <d3d8perf.h> header file.
   With performance testing enabled, the Direct3D subsystem tracks API calls
   and keeps certain performance counters which can be extracted by the app.
   This is useful to find track down wait conditions and other performance
   limiting features of an app.

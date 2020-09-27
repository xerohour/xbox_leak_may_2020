//-----------------------------------------------------------------------------
// Name: VSTune Xbox Tool
// 
// Copyright (c) 2002 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
This tool demonstrates the performance capabilities of the vertex shader and fixed 
function pipelines on the Xbox.  It is user customizable so that the developer can 
test vertex shaders and analyze their performance versus the fixed function pipeline 
and/or versus other vertex shaders.   

Required files and media
========================
   Copy the INI file and any custom vertex shaders to the directory containing the XBE.


Programming Notes
=================
The INI file must be copied over to the Xbox in the directory where the .XBE lives. The
tool will not do anything without the .INI file to describe the test its supposed to
perform.  Please see the vstune.ini file for a complete description of each entry in the
file and how it can be changed to test different aspects of the transform and lighting
pipeline.  The tool will prompt if it cannot find a .INI file.

The INI file can be changed on the fly without restarting the application.  Simply copy
a new version into the Xbox directory where the .INI is stored and the application will
immediately load the new .INI file and use it as the new test.

You can also specify a vertex shader in the .INI file to test its throughput as well as
testing the fixed function pipeline.

There are two basic tests the tool can perform. The first is the "current" test which is
defined in the .INI file.  It tests the performance throughput of the transform and
lighting pipeline based on the specifications in the .INI file.  The second test is the
PushBuffer Call test.  This test shows the throughput of the "current" test with
pushbuffers of varying sizes.  The pushbuffer sizes can be set in the INI file.
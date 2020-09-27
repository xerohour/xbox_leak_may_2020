//-----------------------------------------------------------------------------
// Name: Shadowbuffer Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
This sample illustrates how to do shadowbuffering on the xbox.




Programming Notes
=================
If the projected q goes negative the shadowbuffer lookup is undefined.
(In this sample, it shows up as black or 'in shadow') We therefore 
clamp q.w to 0. There are other potential solutions to this problem. 
Also if q is projected beyond the far clip plane of the light, 
you will see black as well.
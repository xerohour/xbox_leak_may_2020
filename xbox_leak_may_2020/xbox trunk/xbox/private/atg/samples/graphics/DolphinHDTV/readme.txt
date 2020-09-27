//-----------------------------------------------------------------------------
// Name: DolphinHDTV Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The DolphinHDTV sample demonstrates how a game can determine if the user's
   Xbox is hooked up to a HDTV set, and how to take advantage of HDTV modes.  

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   There are several settings in the dashboard that affects how a game should
   make use of HDTV modes.  First, the user can select whether or not they
   have a widescreen TV.  This option is available no matter which AV pack is
   connected.  A widescreen TV has a 16x9 aspect ratio, but still uses a
   standard NTSC or PAL signal.  When enabling a widescreen video mode, the
   game can widen the field of view to more accurately fill the user's screen,
   rather than stretching a narrower field of view onto the wider set.

   The other options are for enabling specific HDTV modes, and are only
   available then the HDTV AV pack is connected.  The modes are 480p, 720p, 
   and 1080i.  Before making use of any of these modes, the game must check
   that the HDTV AV pack is connected, and that the mode has been specifically
   enabled.

   If a game only wishes to make use of 480p, it can just leave the Flags field
   of the D3DPRESENT_PARAMS structure empty (ie, don't specify interlaced or
   progressive).  Direct3D will automatically select 480i or 480p as 
   appropriate; this should have no impact on your game design or performance.
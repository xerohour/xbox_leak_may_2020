//-----------------------------------------------------------------------------
// Name: XArena
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   A basic game that illustrates the essentials of multiplayer gaming.
   

Programming Notes
=================
   This is a more robust DPlay application that illustrates the same concepts
as XSimplePeer, but expands upon them by sending more complex game related
messages.

   At this point, the players can drive their hovertanks around, and shoot
at other players.

   Basic dead-reckoning is implemented, but there are still many important
issues regarding networked gameplay that need to be addressed. Check the
comments in net.cpp for a more thorough description of these issues.


Items to Finish
===============
Add global time so synchronization is more precise.
Add item spawns.
Add server verification of death.
//-----------------------------------------------------------------------------
// Name: MatchMaking
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample illustrates online matchmaking on Xbox. Code shows how
   to handle both QuickMatch and CustomMatch methods of findind games.
   Shows how to use the MatchMaking API, using session integers, strings
   and blobs. Shows how to get search results, add and remove players 
   from a session.

   Once a session has been created or found, enters game state and
   shows how players connect using the secure Xbox network state and
   how they communicate using standard Winsock calls.


Programming Notes
=================
   This sample automatically generates user accounts on the Xbox if
   none exist. In future versions, developers will have more control
   over account creation. On the shipping console, account creation
   will be handled by the Xbox Dashboard.


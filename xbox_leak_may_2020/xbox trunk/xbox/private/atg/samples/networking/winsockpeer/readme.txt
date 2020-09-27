//-----------------------------------------------------------------------------
// Name: WinsockPeer
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample illustrates Winsock peer-to-peer networking on Xbox.
   Code is based on the DX8 SimplePeer sample. Allows player to start/host 
   a new multiplayer game, connect to an existing multiplayer game, and 
   send simple messages to other players. Full functionality of this sample 
   requires that it be run on at least two different connected Xboxes.

   This sample uses the secure Xbox network stack, and shows the methods
   by which hosts and clients establish secure network sessions.

   This sample properly handles player disconnects, but does not include
   host migration. If the host leaves the game, other players in the
   game can continue to play, but no new players can join.

   Additional details relevant to this sample can be found in the
   Xbox Secure Network Library white paper.

Programming Notes
=================
   This sample shows an example call to XNetStartup(), which initializes
   the secure network stack. You must call XNetStartup() before calling
   WSAStartup().

   Host notes
   ----------

   When the host starts a game, he must register a network session using 
   XNetCreateKey and XNetRegisterKey. When the session is complete (game is
   done), the host unregisters the session using XNetUnregisterKey.

   The host must acquire his XNADDR using XNetGetTitleXnAddr(). The host
   then waits for clients to find and join his game.

   When the host receives a "find game" message from a potential client,
   he cannot reply directly to the client, since no secure session has
   been established. He must instead broadcast a "found game" message.
   This means that the host must send back a value that the requesting
   client "knows" about. The method used by the sample is for the client
   to generate a nonce and send the nonce with the "find game" message.
   The host then resends the nonce back in its "found game" message.
   The client with the matching nonce knows that the "found game" message
   is for him.

   When the host sends the "found game" message to the client, the host
   also sends his XNADDR and the session key information.

   When the host receives a "join game" message, he calls
   XNetInAddrToXnAddr() to convert the client's INADDR to an XNADDR. If
   this call is successful, then a secure session exists between the
   host and the client. From that moment on, the host can use the
   client's INADDR with normal Winsock calls in order to communicate with
   the client.

   In response to the "join game" message, the host issues a list of
   all the existing players to the new client. The key elements of this
   list are the XNADDRs of each of the existing players.

   Client notes
   ------------

   To find a game, a client broadcasts a "find game" message. As noted
   above, he sends a nonce so that he can verify the responses he receives
   from any active games.

   After compiling a list of available games, the client selects a game.
   At this point, he must do the following to establish a secure session
   with the host game: 1) call XNetRegisterKey() using the host's session key
   information, and 2) call XNetXnAddrToInAddr() to convert the host's
   XNADDR to an INADDR. Using the new INADDR, he can send a "join game"
   message directly to the host.

   When the client receives a "join approved" message, he gets a list
   of all the other players in the game. He can convert each player's
   XNADDR to an INADDR using XNetXnAddrToInAddr(). From that point on,
   the client can communicate directly to any of the other players
   in the game by using their INADDR.

   When the client leaves the game, he unregisters the session using
   XNetUnregisterKey().


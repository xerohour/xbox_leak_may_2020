//-----------------------------------------------------------------------------
// 
// Sample Name: SimplePeer Sample
// 
// Copyright (c) 1999 Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  SimplePeer illustrates how to network to other players on the 
  using DirectPlay.  After joining or creating a session, a simple game 
  begins immediately.  Other players may join the game in progress at any time.  
  
  The game itself is very simple, passing a single DirectPlay message to all connected
  players when the "Wave To other players" button is pressed.

Path
====
  Source: Mssdk\Samples\Multimedia\DPlay\Src\SimplePeer

  Executable: Mssdk\Samples\Multimedia\DPlay\Bin

User's Guide
============
  Enter the player's name, and choose a connection type.  You can either choose 
  "Wait for Lobby Connection" for inside-out lobby launching or choose a 
  service provider.  Use the Multiplayer Games dialog to either search for an 
  active game to join, or to start a new game.  After game has been joined or 
  created, the game begins immediately.  Other players may join the game at 
  any time.  The host player may also leave at anytime since the DirectPlay 
  automatically migrates the host player.

Programming Notes
=================
  This sample was intended to be very simple, showing the basics of using 
  the DirectPlay API.  Here's a quick run through the various steps 
  of SimplePeer:
  
  * Initialize DirectPlay. See WinMain()
        1. Init COM with CoInitialize()
        2. Create a IDirectPlay8Peer* with CoCreateInstance()
        3. Call IDirectPlay8Peer::Initialize to tell DPlay about our message handler
        
  * Prompt to either create or join a DirectPlay game
        This sample calls upon the helper class CNetConnectWizard for this task.
        The uses dialog boxes to query the user what to do, however most games will 
        want to use a fanicer graphics layer.  Here's what CNetConnectWizard does 
        after calling CNetConnectWizard::DoConnectWizard():
        
        1. CNetConnectWizard enumerates and displays DPlay's service providers with
                IDirectPlay8Peer::EnumServiceProviders. 
                See CNetConnectWizard::ConnectionsDlgFillListBox()
        2. Once a service provider has been chosen via the UI it creates a DirectPlay 
                address by creating a IDirectPlay8Address* with CoCreateInstance, and calls 
                IDirectPlay8Address::SetSP with the SP's guid. 
                See CNetConnectWizard::ConnectionsDlgOnOK() 
        3. Call IDirectPlay8Peer::EnumHosts passing in the IDirectPlay8Address* to enum
                all the games in progress on that SP. See CNetConnectWizard::SessionsDlgEnumHosts()
        4. Wait for DPN_MSGID_ENUM_HOSTS_RESPONSE messages to come in on the callback.
                Upon recieve these, put them in a data structure.  You will need to deep copy 
                the DPN_APPLICATION_DESC, and the pAddressSender.  Also be careful to 
                not to add duplicates to the list, and you will need to manage this structure 
                yourself including expiring old enumeratations.  See 
                CNetConnectWizard::SessionsDlgNoteEnumResponse() and 
                CNetConnectWizard::SessionsDlgExpireOldHostEnums().
        5. It displays the list to the user, and allows the user to either choose 
                a game from the list or create a new one.          
           - If joining a game from the list, it calls IDirectPlay8Peer::SetPeerInfo()
                to set the player's name, and then calls IDirectPlay8Peer::Connect()
                passing in the DPN_APPLICATION_DESC*, as well as pAddressSender 
                from the selected game. This will async complete, so wait for 
                DPN_MSGID_CONNECT_COMPLETE, and read the connect result from msg.
                See CNetConnectWizard::SessionsDlgJoinGame().
           - If creating a new game, it calls IDirectPlay8Peer::SetPeerInfo()
                to set the player's name, and then calls IDirectPlay8Peer::Host()
                passing in a DPN_APPLICATION_DESC filled with various info such
                as the game name, max players, and the app guid. It also passes in
                the IDirectPlay8Address* that describes which SP to use.
                See CNetConnectWizard::SessionsDlgCreateGame().
                
  * Handle DirectPlay system messages.  See DirectPlayMessageHandler()
        - Upon DPN_MSGID_ADD_PLAYER it calls IDirectPlay8Peer::GetPeerInfo
                to retrieve the player's name.  It then creates a app specific
                structure for the player, and stores the pointer to this in
                a global linked-list.  Also it passes the pointer to this 
                structure to DPlay in the pvPlayerContext field.  This prompts
                DirectPlay to return that pointer whenever a message from or about
                that player is received, so instead of traversing the linked-list 
                to find the player's structure the pvPlayerContext will be pointing
                directly to the correct structure.
        - Upon DPN_MSGID_DELETE_PLAYER it gets the player's structure point from 
                pDeletePlayerMsg->pvPlayerContext and then removes and deletes 
                the structure from the linked-list.
        - Upon DPN_MSGID_CONNECTION_TERMINATED it shuts down the dialog.
        - Upon DPN_MSGID_RECEIVE it casts pReceiveMsg->pReceiveData into 
                a generic app defined structure that helps it figure out 
                what structure is really contained in pReceiveMsg->pReceiveData.
                For this simple example, if the type is GAME_MSGID_WAVE it 
                just executes an simple action.
                
  * Send a DirectPlay packet
        - If the user presses, "Wave to other players!" button then it 
                calls IDirectPlay8Peer::SendTo() with DPNID_ALL_PLAYERS_GROUP
                and a data message that is simply a DWORD containing GAME_MSGID_WAVE.
                See WaveToAllPlayers().
                
  * Clean up.  See bottom of WinMain()
        1. Call IDirectPlay8Peer::Close()
        2. Release the IDirectPlay8Peer*
        4. Free any app-specific resources
        5. Call CoUninitialize().
        

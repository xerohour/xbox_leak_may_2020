//-----------------------------------------------------------------------------
// Name: DebugConsole Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The DebugConsole sample demonstrates how to communicate    with the XDK 
   machine via the debug channel.  The debug monitor runs on the XDK machine 
   and listens for connections.  Once connected, commands can be sent to the 
   debug monitor for it to execute.  In addition, a title running on the XDK 
   machine can register itself with the debug monitor to receive extended 
   commands, prefixed with a designated string.  This console is designed to 
   work with the DebugChannel sample to demonstrate this.  After launching 
   the console on your development machine and the DebugChannel sample on your 
   XDK machine, type "connect" to connect to the debug monitor and 
   DebugChannel's External Command Processor.  Typing "help" will give you a 
   list of commands you can execute.
   

Required files and media
========================
   None.


Programming Notes
=================
   This sample demonstrates how to communicate to the debug monitor from a
   remote machine.  It also shows how to send commands to a registered 
   External command processor running on the XDK machine.  See the readme
   for the DebugChannel sample for more details on how this is handled.

   Note how the debug monitor callback functions ExtNotifyFunc() and 
   HandleDebugString() have to put their messages to print into a queue to
   be processed by the main thread.  If the callback functions called 
   RtfPrintf to print immediately, they would end up calling SendMessage -
   this blocks until the windows message has completed processing.  However,
   if the main thread is disconnecting, DmNotify blocks until all pending
   debug monitor notifications have completed.  This could cause a deadlock.
   sampels
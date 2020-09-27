//-----------------------------------------------------------------------------
// Name: DebugChannel Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The DebugChannel sample demonstrates how to add code to an application so that
   it can be controlled from the development system via the debug channel.
   In order to use this sample, you should compile and run the Remote Xbox
   Debug Console sample on your development machine.  First, launch the 
   DebugChannel sample on your XDK, and then connect to it from the Debug Console.
   Typing "help" will give you a list of commands you can execute.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   This sample provides basic examples of what can be done through the debug
   channel:
   * Executing a function inside the running application - for example, the
     RCmdTexture function to select a texture.  Arguments can be pulled from
     parameters passed to the function.
   * Modifying application variables - this is really just a special case of
     calling a function whose purpose is to modify memory.  In this sample,
     the spin rate and light color settings can be manipulated.  A notification
     function can also be called - for example, setting a flag to denote that
     we should re-define the light.
   * Sending messages through the debug channel.  Messages can be sent just
     to a console that asked for them, rather than putting them in the debug
     output.

   It's important to note the synchronization that is performed between the
   external command processor function DCCmdProcessorProc() and the polling
   function DCHandleCmds().  The ECP function will be called from the
   debug monitor thread, not the application thread.  Any work done in the
   ECP notification function should be threadsafe or protected, as in this
   sample.

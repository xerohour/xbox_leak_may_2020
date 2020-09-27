//-----------------------------------------------------------------------------
// Name: Auth
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample illustrates Xbox Online account enumeration, PIN entry,
   PIN validation, logon and authentication. The sample shows proper online
   multitasking using XOnlineTaskContinue(), as well as error handling
   for PIN and logon failure.


Programming Notes
=================
   Sample shows how to enumerate accounts on hard disk and memory units.
   Sample framework doesn't save the full XINPUT_STATE for controllers,
   only the gamepad portion of the struct, so this sample must create a
   local copy for the XOnlineStartPINInput() function (and friends).
   Games would typically save and use the full struct.

   The PIN codes for users created by XOnlineSetupTempAccounts() are
   always "X, Y, black, white"


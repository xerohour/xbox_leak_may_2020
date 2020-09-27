//-----------------------------------------------------------------------------
// File: Commands.h
//
// Desc: Functions to control a list of commands and functions assigned to the
//		 console
//
// Hist: 06.11.2001 - New
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef COMMANDS_H
#define COMMANDS_H

// Function pointer, used to hold function address to run when linked to a
// command
typedef void (*commandFunction)(void);




//-----------------------------------------------------------------------------
// Name: InitCommands()
// Desc: Just ensures command list is set correctly
//-----------------------------------------------------------------------------
VOID InitCommands();




//-----------------------------------------------------------------------------
// Name: AddCommand()
// Desc: Adds the command address and name into a list of commands, ensuring
//			that the command does not already exist in the list.
//-----------------------------------------------------------------------------
VOID AddCommand( WCHAR* strNewCommandName, commandFunction strNewFunctionName );

VOID RemoveAllCommands();




//-----------------------------------------------------------------------------
// Name: CompleteCommand()
// Desc: Called to see if we have a command in the list that matches the partial
//	name passed in
//-----------------------------------------------------------------------------
WCHAR* CompleteCommand( WCHAR* strPartialCommand );




//-----------------------------------------------------------------------------
// Name: FindAndExecuteCommand()
// Desc: Parses list of commands and executes function if available
//-----------------------------------------------------------------------------
VOID FindAndExecuteCommand( WCHAR* strCommandString );




#endif

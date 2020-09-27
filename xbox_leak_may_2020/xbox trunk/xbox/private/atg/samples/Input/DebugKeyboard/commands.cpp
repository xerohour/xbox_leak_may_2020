//-----------------------------------------------------------------------------
// File: Commands.cpp
//
// Desc: Functions to control a list of commands and functions assigned to the
//		 console
//
// Hist: 06.11.2001 - New
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "commands.h"


// Linked list structure for holding command list
struct COMMANDS_STRUCT
{
	WCHAR*  		 strName;
	commandFunction	 fnCommand;
	COMMANDS_STRUCT* pNext;
};

static COMMANDS_STRUCT* g_commandFunctions;




//-----------------------------------------------------------------------------
// Name: InitCommands()
// Desc: Just ensures command list is set correctly
//-----------------------------------------------------------------------------
VOID InitCommands()
{
	g_commandFunctions = NULL;
}




//-----------------------------------------------------------------------------
// Name: AddCommand()
// Desc: Adds the command address and name into a list of commands, ensuring
//			that the command does not already exist in the list.
//-----------------------------------------------------------------------------
VOID AddCommand( WCHAR* strNewCommandName, commandFunction fnNewCommand )
{

	COMMANDS_STRUCT* newCommand;
	
	// Ensure that the command isn't previously defined
	for ( newCommand = g_commandFunctions; newCommand; newCommand = newCommand->pNext )
	{
		if( wcscmp( strNewCommandName, newCommand->strName ) == 0 )
		{
//			DebugMessageString( __FILE__, __LINE__, WARNING, "AddCommand() failed, command already exists" );
			return;
		}
	}

	// Have gone through all the command list without finding this command, so add it to the list
	// Note that so commands passed in have no function assigned
	newCommand = (COMMANDS_STRUCT*)malloc ( sizeof(COMMANDS_STRUCT) );
	if( newCommand == NULL )
	{
//			DebugMessageString( __FILE__, __LINE__, ERROR, "AddCommand() failed, command malloc returned NULL" );
			return;
	}
	newCommand->strName		= strNewCommandName;
	newCommand->fnCommand	= fnNewCommand;
	newCommand->pNext		= g_commandFunctions;
	g_commandFunctions = newCommand;
}




//-----------------------------------------------------------------------------
// Name: RemoveAllCommands()
// Desc: 
//-----------------------------------------------------------------------------
VOID RemoveAllCommands()
{
//	g_commandFunctions = NULL;
//	#pragma message(Reminder "Complete remove all commands !")
}




//-----------------------------------------------------------------------------
// Name: RemoveCommand()
// Desc: 
//-----------------------------------------------------------------------------
VOID RemoveCommand( char *i_szCommandName )
{
	// need to track for the special case of start and end of the list
//	g_commandFunctions = NULL;
//	#pragma message(Reminder "Complete remove command !")
}




//-----------------------------------------------------------------------------
// Name: CompleteCommand()
// Desc: Called to see if we have a command in the list that matches the partial
//	name passed in
//-----------------------------------------------------------------------------
WCHAR *CompleteCommand( WCHAR *i_partialCommand )
{
	COMMANDS_STRUCT* cmd;
	INT			     iPartialCommandLength;
	
	iPartialCommandLength = wcslen( i_partialCommand );
	
	if( iPartialCommandLength == 0 )
		return NULL;
		
	for( cmd = g_commandFunctions; cmd; cmd = cmd->pNext )
	{
		if( wcsncmp( i_partialCommand, cmd->strName, iPartialCommandLength ) == 0 )
			return cmd->strName;
	}

	return NULL;
}




//-----------------------------------------------------------------------------
// Name: FindAndExecuteCommand()
// Desc: Parses list of commands and executes function if available
//-----------------------------------------------------------------------------
VOID FindAndExecuteCommand( WCHAR* strCommand )
{

	COMMANDS_STRUCT* cmd;
	INT			     iCommandLength;
	
	iCommandLength = wcslen( strCommand );
	
	if( iCommandLength == 0 )
		return;
		
	for( cmd = g_commandFunctions; cmd; cmd = cmd->pNext )
	{
		if( wcsncmp( strCommand, cmd->strName, iCommandLength ) == 0 )
		{
			if( cmd->fnCommand != NULL )
				cmd->fnCommand ();
			return;
		}
	}
}
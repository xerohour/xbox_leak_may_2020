#include <xtl.h>
#include <stdio.h>
#include <XBFont.h>
#include <XBUtil.h>
#include "keyboard.h"
#include "commands.h"
#include "console.h"


VOID DrawFilledRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwFillColor, 
			         DWORD dwOutlineColor );


static VOID ClearBuffer( VOID );
static VOID Execute( VOID );
static VOID AddCurrentStringToBuffer( VOID );

// Internal functions to link to console
static VOID RebootFunction();
static VOID ClearFunction();
static VOID HelpFunction();

// Boolean to indicate whether the console is active (being displayed and taking input)
static BOOL	g_bConsoleActive = FALSE;

// Table of strings holding the previous 10 command lines, note that we can see
// only ten lines, which are displayed, previous command recall is not implemented and
// if it was we would have to implement two buffers, one for the command line as below
// and an additional screen buffer to hold the commands and output feedback
#define NUM_TEXTLINES	10
#define TEXTLINE_LENGTH	80

static WCHAR	strTextLine[ NUM_TEXTLINES ][ TEXTLINE_LENGTH ];
static INT		iTextLineInsertIndex = 0;

// Current text buffer
static WCHAR	strCurrentTextLine[ TEXTLINE_LENGTH ];
static INT		iCurrentCursorPosition;

// Font for console (uses smaller font than default)
static CXBFont* g_pFont = NULL;




//-----------------------------------------------------------------------------
// Name: InitConsole()
// Desc: Initialises console
//-----------------------------------------------------------------------------
VOID InitConsole( CXBFont* pFont )
{
	g_bConsoleActive = FALSE;
	g_pFont          = pFont;

	ClearBuffer();
	ClearCurrentTextLine();

	// Setup command structure for holding commands and functions to call,
	// using NULL just adds string completion for the console
	InitCommands();
	AddCommand( L"reboot",  RebootFunction );
	AddCommand( L"clear",   ClearFunction );
	AddCommand( L"help",    HelpFunction );
}




//-----------------------------------------------------------------------------
// Name: OpenConsole()
// Desc: Sets the console to be active and initialises the current text line
//-----------------------------------------------------------------------------
VOID OpenConsole()
{
	// if we are already open then just return, but display an warning message
	// in the debugger
	if( g_bConsoleActive == TRUE )
		return;

	// Open the keyboard device if it requires it, this allows us to share the
	// keyboard with other areas of the game
	if( FAILED( XBInput_InitDebugKeyboard() ) )
		return;

	// Set the console window to be active and clear the current text line
	g_bConsoleActive = TRUE;

	ClearCurrentTextLine();
}




//-----------------------------------------------------------------------------
// Name: CloseConsole()
// Desc: Sets the console to be inactive
//-----------------------------------------------------------------------------
VOID CloseConsole()
{
	// Remove any existing text from the input line
	ClearCurrentTextLine();

	g_bConsoleActive = FALSE;
}




//-----------------------------------------------------------------------------
// Name: ToggleConsole
// Desc: Switches between open and close
//-----------------------------------------------------------------------------
VOID ToggleConsole()
{
	if( ! IsConsoleActive() )
		OpenConsole();
	else
		CloseConsole();
}




//-----------------------------------------------------------------------------
// Name: IsConsoleActive()
// Desc: Returns whether the console is currently active or not
//-----------------------------------------------------------------------------
BOOL IsConsoleActive()
{
	return g_bConsoleActive;
}




//-----------------------------------------------------------------------------
// Name: ClearCurrentTextLine()
// Desc: Clear the text buffer and reset the current cursor position within the
//			string
//-----------------------------------------------------------------------------
VOID ClearCurrentTextLine()
{
	iCurrentCursorPosition = 0;

	ZeroMemory( strCurrentTextLine, sizeof(strCurrentTextLine) );
}




//-----------------------------------------------------------------------------
// Name: ClearConsole()
// Desc: Clear console buffer and current input string
//-----------------------------------------------------------------------------
VOID ClearConsole()
{
	ClearBuffer();
	ClearCurrentTextLine();
}




//-----------------------------------------------------------------------------
// Name: ProcessConsole()
// Desc: Get keypress and perform any necessary commands
//-----------------------------------------------------------------------------
VOID ProcessConsole()
{
	// handle input from keyboard
	CHAR cInputKey = XBInput_GetKeyboardInput();

	if( cInputKey != '\0' )
	{
		// First handle a reserved key, in this case either the ESC character to
		// close the console or the TAB key to attempt command line completion.
		// Then check for return, anything else must be an ascii value in this
		// sample, this could be expanded to handle all buttons
		switch( cInputKey )
		{
			case DELETE_KEY:
				if( iCurrentCursorPosition != 0 )
				{
					iCurrentCursorPosition--;
					strCurrentTextLine[ iCurrentCursorPosition ] = '\0';
				}
				break;

			case RETURN_KEY:
				Execute();
				break;

			case TAB_KEY:
				WCHAR *matchedName;
				// pass in whole of buffer string to find name match, if match is returned
				// then copy it into the current buffer
				matchedName = CompleteCommand( strCurrentTextLine );

				if( matchedName != NULL )
				{
					wcsncpy( strCurrentTextLine, matchedName, wcslen( matchedName ) );
					iCurrentCursorPosition = wcslen( strCurrentTextLine );
				}

				break;
			
			case ESC_KEY:
				CloseConsole();
				break;
			
			default:
				// Not really necessary to check value here, as we are using
				// the ASCII value returned by the keyboard, so it should be
				// some form of printable character
				if( iCurrentCursorPosition < TEXTLINE_LENGTH - 1 )
				{
					strCurrentTextLine[ iCurrentCursorPosition ] = cInputKey;
					iCurrentCursorPosition++;
				}
				break;
		}
	}
}




//-----------------------------------------------------------------------------
// Name: DrawConsole()
// Desc: Draws the console on top of the screen
//-----------------------------------------------------------------------------
VOID DrawConsole()
{
	if( !g_bConsoleActive )
		return;

	DrawFilledRect( 0, 0, 640, 241, 0x90000000, 0xffffff00 );

	// Now create a temporary buffer to hold the final output string for
	// current input, this is so we can add formatting around the input
	// text
	WCHAR	strTempDisplayBuffer[TEXTLINE_LENGTH+2];
	swprintf( strTempDisplayBuffer, L"> %s", strCurrentTextLine );
	g_pFont->DrawText( 50, 220, 0xffffff00, strTempDisplayBuffer );

	// Get the value for the index of next space for adding to the command
	// buffer, this will be used to display the command history on screen
	INT iCurrentLineIndex = iTextLineInsertIndex;

	for( INT i = 0; i < NUM_TEXTLINES; i++ )
	{
		g_pFont->DrawText( 40.0f, 20.0f + ( 20.0f * i ), 0xffffffff, strTextLine[ iCurrentLineIndex ] );
		iCurrentLineIndex = ( iCurrentLineIndex + 1 ) % NUM_TEXTLINES;
	}
}




//-----------------------------------------------------------------------------
// Name: ShutdownConsole()
// Desc: Tidies up console
//-----------------------------------------------------------------------------
VOID ShutdownConsole()
{
	RemoveAllCommands();

	g_bConsoleActive = FALSE;

	ClearBuffer();
	ClearCurrentTextLine();

	g_pFont = NULL;
}




//-----------------------------------------------------------------------------
// Name: Execute()
// Desc: Processes return key pressed, checks whether input is a command and
//			whether to execute the command, then adds command to the command
//			history buffer
//-----------------------------------------------------------------------------
VOID Execute()
{
	// Check that we have a string of some info first, if not skip next two
	// operations
	if( wcslen( strCurrentTextLine ) != 0 )
	{
		// First try to find command and execute that, because command may
		// effect console, if not command than just display text
		FindAndExecuteCommand( strCurrentTextLine );
	}

	// Then add current string to keyboard buffer
	AddCurrentStringToBuffer();
}




//-----------------------------------------------------------------------------
// Name: AddCurrentStringToBuffer()
// Desc: Copies last string input into the correct location in the command
//			history buffer
//-----------------------------------------------------------------------------
VOID AddCurrentStringToBuffer()
{
	// we just keep a track of the index, this saves us having to copy the whole buffer
	// around, but does mean we need to use extra logic to display on screen

	ZeroMemory( strTextLine[iTextLineInsertIndex], sizeof(strTextLine[iTextLineInsertIndex]) );

	wcsncpy( strTextLine[iTextLineInsertIndex], strCurrentTextLine, TEXTLINE_LENGTH );

	// move table pointer
	iTextLineInsertIndex = ( iTextLineInsertIndex + 1 ) % NUM_TEXTLINES;

	// clear string we are about to overwrite, then add current string
	ClearCurrentTextLine();
}




//-----------------------------------------------------------------------------
// Name: ClearBuffer()
// Desc: Resets all of the command history buffer
//-----------------------------------------------------------------------------
VOID ClearBuffer()
{
	for( int i = 0; i < NUM_TEXTLINES; i++ )
	{
		ZeroMemory( strTextLine[i], sizeof(strTextLine[i]) );
	}

	iTextLineInsertIndex = 0;
}




//-----------------------------------------------------------------------------
// Name: RebootFunction()
// Desc: Internal function to call from console
//-----------------------------------------------------------------------------
VOID RebootFunction()
{
	XLaunchNewImage( NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: ClearFunction()
// Desc: Internal function to call from console
//-----------------------------------------------------------------------------
VOID ClearFunction()
{
	ClearConsole();
}

//-----------------------------------------------------------------------------
// Name: HelpFunction()
// Desc: Internal function to call from console
//-----------------------------------------------------------------------------
VOID HelpFunction()
{
	// Then add current string to keyboard buffer
	AddCurrentStringToBuffer();


	wcscpy( strCurrentTextLine, L"   reboot - Reboots the system" );
	AddCurrentStringToBuffer();
	wcscpy( strCurrentTextLine, L"   clear  - Clears the console" );
	AddCurrentStringToBuffer();
	wcscpy( strCurrentTextLine, L"   help   - Displays list of commands" );
}
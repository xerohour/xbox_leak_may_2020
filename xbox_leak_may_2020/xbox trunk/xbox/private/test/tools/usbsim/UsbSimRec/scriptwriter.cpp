/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	scriptwriter.cpp

  Abstract:

	Defines the USB Simulator Recorder functions which write input events to a JScript file.

  Author:

	Mabel Santos (t-msanto) 25-May-2001

  Revision History:

*************************************************************************************************/

#include "usbsimrec.h"

/*************************************************************************************************

Routine Description:

	Default JScript constructor

	Creates a script object.  Writes statements to declare functions and data structures in the
	script file.

Arguments:

	none

Return Value:

	none

*************************************************************************************************/

JScript::JScript( void )
{
	DebugOutput( "Opening script output file: %s\r\n", FileName );

	// Open script output file in append mode
	OutFile.open( FileName, ios::app );
	// assert( OutFile );
	if( OutFile )
	{
		// Write file name, date and time stamp
		OutFile << "// USB SIMULATOR RECORDER" << endl;
		OutFile << "// Script File: " << FileName << endl;

		char DateBuff[9];
		char TimeBuff[9];
		_strdate(DateBuff);
		_strtime(TimeBuff);
 
		OutFile << "// Created " << DateBuff << " " << TimeBuff << endl;
		OutFile << endl;

		DebugOutput( "Writing declaration statements to script file: %s\r\n", FileName );
		// Write declaration statements to script file

		// Rate for script replay
		OutFile << "var rate = 1;" << endl;

		// InsertDevice function for plugging in Duke
		OutFile << endl;
		OutFile << "function InsertDevice(dev)" << endl;
		OutFile << "{" << endl;
		OutFile << "var retry = 3;" << endl;
		OutFile << "var time;" << endl;
		OutFile << "do  {" << endl;
		OutFile << "    dev.Plug();" << endl;
		OutFile << "    for(time=32; time<1000; time*=2)" << endl;
		OutFile << "        {" << endl;
		OutFile << "        WScript.Sleep(time);" << endl;
		OutFile << "        if(dev.enumerated == true) return true;" << endl;
		OutFile << "        }" << endl;
		OutFile << "        dev.Unplug();" << endl;
		OutFile << "    WScript.Sleep(250);" << endl;
		OutFile << "    --retry;" << endl;
		OutFile << "    } while(retry != 0);" << endl;
		OutFile << "return false;" << endl;
		OutFile << "}" << endl;

		// Declare Dukes array
		OutFile << endl;
		OutFile << "dukes = new Array(" << NUM_PORTS << ");" << endl;

		// Create USB Simulator object
		OutFile << endl;
		OutFile << "var simulator = WScript.CreateObject(\"USBSimulator.Simulator\");" << endl;
		OutFile << "var version = simulator.GetVersion(\"" << SimIp << "\");" << endl;
		OutFile << "var address = simulator.GetMACAddr(\"" << SimIp << "\");" << endl;
		OutFile << "var boards = simulator.GetUSBPorts(\"" << SimIp << "\");" << endl;
		OutFile << "var numboards = (boards&0x01?1:0) + (boards&0x02?1:0) + (boards&0x04?1:0) ";
		OutFile << "+ (boards&0x08?1:0) + (boards&0x10?1:0) + (boards&0x20?1:0) + (boards&0x40?1:0) ";
		OutFile << "+ (boards&0x80?1:0);" << endl;

		OutFile << endl;
		OutFile << "WScript.Echo(\" Version:    \" + version + \"\\n\");" << endl;
		OutFile << "WScript.Echo(\" MAC Addr:   \" + address + \"\\n\");" << endl;
		OutFile << "WScript.Echo(\" USB Boards: \" + numboards + \" (mask: \" + boards + \")\\n\");" << endl;

		DebugOutput( "Closing script output file: %s\r\n", FileName );
	}
	else
	{ 
		DebugPrint( "File %s: Open operation FAILED.\r\n", FileName );
		DebugPrint( "Please check that the hard drive on the recorder Xbox is not full and restart the program.\r\n" );
	}
	// Close output file and flush the output buffer
	OutFile.close();
}

/*************************************************************************************************

Routine Description:

	ScriptInsert

	Writes statements to create and plug in a Duke object in the script file.

Arguments:

	DWORD Port		Array index to be used for inserting the new Duke.

Return Value:

	none

*************************************************************************************************/

void
JScript::ScriptInsert( DWORD Port )
{
	// Is there a controller plugged into the port?
	if( DukeStates[Port].DukeHandle )
	{
		DebugOutput( "Opening script output file: %s\r\n", FileName );
		// Open script output file in append mode
		OutFile.open( FileName, ios::app );
		//assert( OutFile );
		if( OutFile )
		{
			DebugOutput( "Writing insertion statements to script file: %s\r\n", FileName );
			// Write insertion statements to script file
			OutFile << endl;
			OutFile << "dukes[" << Port << "] = WScript.CreateObject(\"USBSimulator.Duke\");" << endl;
			OutFile << "dukes[" << Port << "].port = " << Port + 1 << ";" << endl;
			OutFile << "dukes[" << Port << "].ip = \"" << SimIp << "\";" << endl;
			OutFile << "InsertDevice(dukes[" << Port << "]);" << endl;

			DebugOutput( "Closing script output file: %s\r\n", FileName );
		}
		else
		{ 
			DebugPrint( "File %s: Open operation FAILED.\r\n", FileName );
			DebugPrint( "Please check that the hard drive on the recorder Xbox is not full and restart the program.\r\n" );
		}
		// Close output file and flush the output buffer
		OutFile.close();
	}
}

/*************************************************************************************************

Routine Description:

	ScriptRemove

	Writes statements to unplug, free and disconnect a Duke object in the script file.

Arguments:

	DWORD Port		Array index to be used for removing the Duke.

Return Value:

	none

*************************************************************************************************/

void
JScript::ScriptRemove( DWORD Port )
{
	// Is there a controller plugged into the port?
	if( DukeStates[Port].DukeHandle )
	{
		DebugOutput( "Opening script output file: %s\r\n", FileName );
		// Open script output file in append mode
		OutFile.open( FileName, ios::app );
		//assert( OutFile );

		if( OutFile )
		{
			DebugOutput( "Writing removal statements to script file: %s\r\n", FileName );
			// Write removal statements to script file
			OutFile << endl;
			OutFile << "dukes[" << Port << "].Unplug();" << endl;
			OutFile << "dukes[" << Port << "].Free();" << endl;
			OutFile << "WScript.DisconnectObject(dukes[" << Port << "]);" << endl;

			DebugOutput( "Closing script output file: %s\r\n", FileName );
		}
		else
		{ 
			DebugPrint( "File %s: Open operation FAILED.\r\n", FileName );
			DebugPrint( "Please check that the hard drive on the recorder Xbox is not full and restart the program.\r\n" );
		}
		// Close output file and flush the output buffer
		OutFile.close();
	}
}

/*************************************************************************************************

Routine Description:

	ScriptSetState

	Writes statements to set the Duke button state in the script file.  Uses the controller state
	and timing information obtained in ReadGamepads().

Arguments:

	DWORD Port					Array index to be used for setting the Duke button state.
	LARGE_INTEGER ElapsedTime	Number of milliseconds to sleep before changing state.

Return Value:

	none

*************************************************************************************************/

void
JScript::ScriptSetState( DWORD Port, const LARGE_INTEGER& ElapsedTime ) 						
{
	// Is there a controller plugged into the port?
	if( DukeStates[Port].DukeHandle )
	{
		// Variable to hold string version of elapsed time
		char TimeBuffer[10];
	
		DebugOutput( "Opening script output file: %s\r\n", FileName );
		// Open script output file in append mode
		OutFile.open( FileName, ios::app );
		//assert( OutFile );
		if( OutFile )
		{
			// Convert elapsed time to a string
			sprintf( TimeBuffer, "%I64u", ElapsedTime.QuadPart );
		
			DebugOutput( "Writing controller state statements to script file: %s\r\n", FileName );
			// Write controller state statements to script file
			OutFile << endl;
			OutFile << "WScript.Sleep(" << TimeBuffer << " / rate);" << endl;
			OutFile << "dukes[" << Port << "].SetButtonState( " << CurrState.Gamepad.wButtons << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[0] << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[1] << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[2] << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[3] << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[4] << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[5] << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[6] << ", "
				<< (DWORD) CurrState.Gamepad.bAnalogButtons[7] << ", "
				<< CurrState.Gamepad.sThumbLX << ", " << CurrState.Gamepad.sThumbLY << ", " 
				<< CurrState.Gamepad.sThumbRX << ", " << CurrState.Gamepad.sThumbRY << " );" << endl;

			DebugOutput( "Closing script output file: %s\r\n", FileName );
		}
		else
		{ 
			DebugPrint( "File %s: Open operation FAILED.\r\n", FileName );
			DebugPrint( "Please check that the hard drive on the recorder Xbox is not full and restart the program.\r\n" );
		}
		// Close output file and flush the output buffer
		OutFile.close();
	}
}

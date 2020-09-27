
/* Copyright Micrsooft 2001 - all rights reserved

  Author: sethmil

  History:

		created		5/9/01		sethmil

  Abstract:
		A wrapper DLL for ActiveWire functions
		uses words as output instead of the activewire
		byte arrays.

*/

#ifndef __AW_H__
#define __AW_H__


// includes
#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
	#include <xtl.h>
#else
	#include <windows.h>
#endif


//#define	DEVROOTNAME		"AW-"
#define AW_DLL_NAME		"aw.dll"

#ifdef AW_DLL_EXPORT
#define AW_API __declspec(dllexport)
#else
#define AW_API __declspec(dllimport)
#endif




//////////////////////////////////////////////////////////////////
// class CAWDevice
// an IO class for activewire devices
/////////////////////////////////////////////////////////////////
//
class AW_API CAWDevice
{
public:
	// default constructor
	CAWDevice();

	// enumerates device devnum
	CAWDevice( DWORD devnum );

	// destructor
	~CAWDevice();

	// writes a word to the port
	DWORD OutPort (WORD data );

	// reads a word from the port
	DWORD InPort (WORD *data );

	// enables the port for input/output
	// 1=output
	// 0=input
	DWORD EnablePort( WORD data );

	// outputs bytes sequentially to the output
	// even subscript bytes are the lower 8 bits
	// odd subscript byytes are the upper 8 bits
	DWORD OutPortEx( BYTE*, DWORD count );

	// reads bytes sequentially from the port
	DWORD InPortEx( BYTE*, DWORD count );

	// enables the port sequentially
	DWORD EnablePortEx( BYTE*, DWORD count );

	// opens a device
	DWORD Open( DWORD devnum );

	// closes the device
	DWORD Close( );

	// returns the last error code
	DWORD GetLastError();

	// sets the last error code
	VOID SetLastError( DWORD errcode );

	// returns a string for the AW error message
	char* ErrorMessage( DWORD errcode );
	// returns string for the last error message
	char* ErrorMessage();

	// TRUE if the device is valid and opened
	BOOL DeviceIsOpen();

protected:
	int iDeviceNum;
	DWORD dwErrCode;
};

//////////////////////////////////////////////////////////////////////////

// Exported Functions
// See ActiveWire documentation for descriptions of these functions
//

extern "C" {

// writes a word to the port
AW_API DWORD AWOutPort ( WORD data );

/* writes count bytes to the ports
 array elements with even subscripts are written to the lower
 8 bits of the port, elements with odd subscripts are written to
 the upper 8 bits of the port.  If count > 2, bytes are written consecutively to
 the port
*/
AW_API DWORD AWOutPortEx ( BYTE* data, DWORD count );


// reads a word from the port
AW_API DWORD AWInPort ( WORD *data );


/* writes count bytes from the ports
 array elements with even subscripts are read from the lower
 8 bits of the port, elements with odd subscripts are read from
 the upper 8 bits of the port.  If count > 2, bytes are read consecutively from
 the port
*/
AW_API DWORD AWInPortEx ( BYTE* data, DWORD count );


/* enables input or output of the port
	for each bit:
		write a 1 to make the bit an output
		write a 0 to make the bit an input
*/
AW_API DWORD AWEnablePort ( WORD data );


/* enables input or output of the port
	for each bit:
		write a 1 to make the bit an output
		write a 0 to make the bit an input
	array elements with even subscripts enable the lower
	 8 bits of the port, elements with odd subscripts enable
	 the upper 8 bits of the port.  If count > 2, enable data is written
	 consecutively to the por
*/	
AW_API DWORD AWEnablePortEx ( BYTE* data, DWORD count );


/*
	opens the USB device.  
	devnum is the device number
	0 is the 1st device attached, 1 is the second, etc

	each thread can open one device at a time.
*/
AW_API DWORD AWOpen ( WORD devnum );


/*
	closes the device that is currently opened by the thread
*/
AW_API DWORD AWClose (  );


/*
	returns error string
*/
AW_API char * AWErrorMessage( DWORD errcode );



// Error Code
enum AW_ERROR_CODE_T {
	AW_OK	= 0,			// success
	AW_ERROR_FATAL,			// Fatal error, cannot continue
	AW_ERROR_SYSTEM,			// System error, use WIN32 GetLastError() for further error code
	AW_ERROR_MEMORY,			// Run out of memory
	
	AW_ERROR_FILEOPEN,		// File open failure
	AW_ERROR_FILEWRITE,		// File write failure
	AW_ERROR_FILEREAD,		// File read failure
	AW_ERROR_FILECLOSE,		// File close failure
	AW_ERROR_FILEFORMAT,		// File format error
	AW_ERROR_FILECHECKSUM,	// File checksum error
	AW_ERROR_FILEEOF,		// Unexpected end of file 

	AW_ERROR_HARDWARE,		// Hardware error, such as the device unplugged

	AW_ERROR_SOFTWARE,		// Software error, possibly a bug...
	AW_ERROR_NOTIMPLEMENTED,	// Not implemented yet...
	AW_ERROR_NOSUPPORT,		// Not supported

	AW_ERROR_USBNOTOPEN,		// Not opend yet
	AW_ERROR_USBNOTCLOSE,	// Not closed yet

	AW_ERROR_USBBADPIPE,		// Bad USB pipe
	AW_ERROR_USBBADOPCODE,	// Bad USB Command/Status Opcode
	AW_ERROR_USBZEROPACKET,	// Zero length USB data packet
	AW_ERROR_USBSHORTPACKET,	// Short USB data packet
	AW_ERROR_USBLONGPACKET,	// Longer USB data packet

	AW_ERROR_TIMEOUT,		// Time out, may or may not a problem...
	AW_ERROR_TRYAGAIN,		// Don't get too serious, try again, may work next time
	AW_ERROR_UNKNOWN,		// Unknown... Truth is out there...   :)
};

}  // extern "C"


#endif // __AW_H__



